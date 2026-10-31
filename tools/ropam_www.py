#!/usr/bin/env python3
"""Strona WWW ze stanem centrali OptimaGSM, tylko do odczytu.

Co kilka sekund czyta stan centrali komendą serwisową 0x24,
co kilka minut ostatnie zdarzenia z dziennika (0x04),
i po każdym odczycie od razu kończy sesję serwisową (ropam_serwis.end_session),
żeby centrala nie zostawała w trybie ON-LINE.
Niczego w centrali nie zmienia: strona nie ma żadnych przycisków.

Użycie:
    ropam_www.py PORT [--konfiguracja PLIK] [--adres ADRES] [--port N]
                      [--co SEKUNDY] [--dziennik-co SEKUNDY]

PORT jak w ropam_serwis.py (np. /dev/ttyACM0, most na XIAO).
PLIK to konfiguracja z `ropam_serwis.py PORT konfiguracja …`:
z niej biorą się nazwy wejść, wyjść i stref.
Domyślnie serwer słucha tylko na 127.0.0.1:8080;
stan centrali alarmowej to nie jest coś, co warto pokazywać całej sieci.
Pod /stan.json ten sam stan dla programów.
"""
import argparse
import html
import json
import os
import re
import sys
import threading
import time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

import serial

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ropam_config  # noqa: E402
import ropam_serwis  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

ZONE_STATES = {3: "rozbrojona", 5: "odliczanie czasu na wyjście"}


def event_texts():
    """Kody zdarzeń z tabeli w docs/zdarzenia.md (teksty z firmware'u)."""
    texts = {}
    path = os.path.join(ROOT, "docs", "zdarzenia.md")
    try:
        with open(path, encoding="utf-8") as f:
            for line in f:
                m = re.match(r"\| `0x([0-9A-F]{2})` \| ([^|]*) \|", line)
                if m:
                    texts.setdefault(int(m.group(1), 16), m.group(2).strip())
    except OSError:
        pass
    return texts


class Panel:
    """Odczyty z centrali w osobnym wątku; strona tylko je pokazuje."""

    def __init__(self, port_name, config, every, log_every):
        self.port_name = port_name
        self.config = config
        self.every = every
        self.log_every = log_every
        self.lock = threading.Lock()
        self.status = None
        self.status_time = None
        self.events = []
        self.events_time = None
        self.error = None
        self.error_time = None

    def run(self):
        port = None
        next_log = 0
        while True:
            try:
                if port is None:
                    port = serial.serial_for_url(self.port_name, 115200,
                                                 timeout=1)
                ropam_serwis.send(port, bytes([0x24]))
                data = ropam_serwis.receive(port)[:ropam_serwis.STATUS_SIZE]
                ropam_serwis.end_session(port)
                with self.lock:
                    self.status, self.status_time = data, time.time()
                if time.time() >= next_log:
                    _, records = ropam_serwis.read_log(port)
                    ropam_serwis.end_session(port)
                    with self.lock:
                        self.events = records[-50:]
                        self.events_time = time.time()
                    next_log = time.time() + self.log_every
            except (OSError, TimeoutError, serial.SerialException) as e:
                with self.lock:
                    self.error, self.error_time = str(e), time.time()
                if port is not None:
                    port.close()
                    port = None
            time.sleep(self.every)

    def snapshot(self):
        with self.lock:
            return dict(status=self.status, status_time=self.status_time,
                        events=list(self.events),
                        events_time=self.events_time,
                        error=self.error, error_time=self.error_time)


def describe(snap, config, texts):
    """Stan w postaci słownika: to samo dla strony i dla /stan.json."""
    out = {"odczyt": snap["status_time"], "blad": snap["error"],
           "blad_czas": snap["error_time"], "strefy": [], "wejscia": [],
           "wyjscia": [], "zdarzenia": []}
    names = {"I": {}, "O": {}, "S": {}}
    used_inputs = set()
    if config:
        text = ropam_config.text
        for n, r in enumerate(config.records("INPUT", 168), 1):
            names["I"][n] = text(r[:0x16])
            if int.from_bytes(r[0x24:0x28], "little") & 0xF:
                used_inputs.add(n)
        for n, r in enumerate(config.records("OUT", 224), 1):
            names["O"][n] = text(r[:0x14])
        for n, r in enumerate(config.records("STREFY", 44), 1):
            names["S"][n] = text(r[:0x10])
    status = snap["status"]
    if status:
        violated = int.from_bytes(status[0:8], "little")
        tamper = int.from_bytes(status[8:16], "little")
        outputs = int.from_bytes(status[0x30:0x34], "little")
        for z in range(4):
            word = int.from_bytes(status[0x4C + 8 * z:0x50 + 8 * z], "little")
            state = word & 0xF
            if word >> 11 & 1:
                desc = "czuwanie nocne"
            elif word >> 10 & 1:
                desc = "czuwanie"
            else:
                desc = ZONE_STATES.get(state, f"stan {state}")
            out["strefy"].append({"nr": z + 1, "nazwa": names["S"].get(z + 1),
                                  "stan": desc, "surowy": word})
        for n in range(1, 49):
            v = violated >> (n - 1) & 1
            t = tamper >> (n - 1) & 1
            line = int.from_bytes(status[0xDC + 2 * (n - 1):
                                         0xDE + 2 * (n - 1)], "little")
            if config and n not in used_inputs and not (v or t):
                continue
            if not config and not (v or t):
                continue
            out["wejscia"].append({"nr": n, "nazwa": names["I"].get(n),
                                   "naruszone": bool(v), "sabotaz": bool(t),
                                   "linia": line})
        for n in range(1, 33):
            on = bool(outputs >> (n - 1) & 1)
            if on or (config and not names["O"][n].startswith("Wyjscie")):
                out["wyjscia"].append({"nr": n, "nazwa": names["O"].get(n),
                                       "wlaczone": on})
        flags = int.from_bytes(status[0xCE:0xD0], "little")
        out["sesja_przed_odczytem"] = bool(flags & 2)
    for r in reversed(snap["events"]):
        out["zdarzenia"].append({
            "czas": "20%02d-%02d-%02d %02d:%02d:%02d" % tuple(r[:6]),
            "kod": r[6], "opis": texts.get(r[6], "?"),
            "argument": r[8], "argument2": r[9]})
    return out


def when(t):
    return time.strftime("%H:%M:%S", time.localtime(t)) if t else "jeszcze nie"


def page(state, every):
    e = html.escape

    def name(item, prefix):
        label = f"{prefix}{item['nr']}"
        return f"{label} {e(item['nazwa'])}" if item.get("nazwa") else label

    rows = []
    rows.append("<h2>Strefy</h2><table>")
    for z in state["strefy"]:
        cls = "on" if "czuwanie" in z["stan"] else ""
        rows.append(f"<tr class='{cls}'><td>{name(z, 'S')}</td>"
                    f"<td>{e(z['stan'])}</td></tr>")
    rows.append("</table><h2>Wejścia</h2><table>"
                "<tr><th>Wejście</th><th>Stan</th><th>Linia</th></tr>")
    for i in state["wejscia"]:
        flags = [w for w, v in (("naruszone", i["naruszone"]),
                                ("sabotaż", i["sabotaz"])) if v]
        cls = "alarm" if flags else ""
        rows.append(f"<tr class='{cls}'><td>{name(i, 'I')}</td>"
                    f"<td>{', '.join(flags) or 'spoczynek'}</td>"
                    f"<td class='num'>{i['linia']}</td></tr>")
    rows.append("</table><h2>Wyjścia</h2><table>")
    for o in state["wyjscia"]:
        cls = "on" if o["wlaczone"] else ""
        rows.append(f"<tr class='{cls}'><td>{name(o, 'O')}</td>"
                    f"<td>{'włączone' if o['wlaczone'] else 'wyłączone'}</td>"
                    "</tr>")
    rows.append("</table><h2>Ostatnie zdarzenia</h2><table>")
    for ev in state["zdarzenia"]:
        rows.append(f"<tr><td class='num'>{ev['czas']}</td>"
                    f"<td>{e(ev['opis'])}</td>"
                    f"<td class='num'>0x{ev['kod']:02X}</td>"
                    f"<td class='num'>{ev['argument']}</td></tr>")
    rows.append("</table>")
    error = ""
    if state["blad"] and (not state["odczyt"]
                          or state["blad_czas"] > state["odczyt"]):
        error = (f"<p class='err'>Ostatni odczyt się nie udał"
                 f" ({when(state['blad_czas'])}): {e(state['blad'])}</p>")
    return f"""<!doctype html>
<html lang="pl"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta http-equiv="refresh" content="{max(every, 5)}">
<title>Centrala</title>
<style>
:root {{ --bg: #fff; --fg: #222; --dim: #777; --line: #ddd;
         --on: #e6f2e6; --alarm: #f8e0e0; --err: #a00; }}
@media (prefers-color-scheme: dark) {{
  :root {{ --bg: #181818; --fg: #ddd; --dim: #999; --line: #333;
           --on: #1f3320; --alarm: #42201f; --err: #f88; }} }}
body {{ font: 15px/1.4 system-ui, sans-serif; background: var(--bg);
        color: var(--fg); margin: 0 auto; padding: 1em; max-width: 46em; }}
h1 {{ font-size: 1.3em; }} h2 {{ font-size: 1.05em; margin-top: 1.5em; }}
table {{ border-collapse: collapse; width: 100%; }}
td, th {{ padding: .3em .5em; border-bottom: 1px solid var(--line);
          text-align: left; }}
.num {{ font-variant-numeric: tabular-nums; white-space: nowrap; }}
tr.on {{ background: var(--on); }} tr.alarm {{ background: var(--alarm); }}
.dim {{ color: var(--dim); }} .err {{ color: var(--err); }}
</style></head><body>
<h1>Stan centrali</h1>
<p class="dim">Odczyt stanu: {when(state['odczyt'])};
odświeżanie co {every} s; tylko podgląd.</p>
{error}
{''.join(rows)}
</body></html>"""


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("serial_port")
    ap.add_argument("--konfiguracja")
    ap.add_argument("--adres", default="127.0.0.1")
    ap.add_argument("--port", type=int, default=8080)
    ap.add_argument("--co", type=float, default=10,
                    help="odstęp odczytów stanu w sekundach")
    ap.add_argument("--dziennik-co", type=float, default=300,
                    help="odstęp odczytów dziennika w sekundach")
    args = ap.parse_args()
    config = None
    if args.konfiguracja:
        with open(args.konfiguracja, "rb") as f:
            config = ropam_config.Config(f.read())
    texts = event_texts()
    panel = Panel(args.serial_port, config, args.co, args.dziennik_co)
    threading.Thread(target=panel.run, daemon=True).start()

    class Handler(BaseHTTPRequestHandler):
        def do_GET(self):
            state = describe(panel.snapshot(), config, texts)
            if self.path == "/stan.json":
                body = json.dumps(state, ensure_ascii=False).encode()
                kind = "application/json; charset=utf-8"
            elif self.path == "/":
                body = page(state, int(args.co)).encode()
                kind = "text/html; charset=utf-8"
            else:
                self.send_error(404)
                return
            self.send_response(200)
            self.send_header("Content-Type", kind)
            self.send_header("Content-Length", str(len(body)))
            self.end_headers()
            self.wfile.write(body)

        def log_message(self, *_):
            pass

    server = ThreadingHTTPServer((args.adres, args.port), Handler)
    print(f"http://{args.adres}:{args.port}/")
    server.serve_forever()


if __name__ == "__main__":
    main()
