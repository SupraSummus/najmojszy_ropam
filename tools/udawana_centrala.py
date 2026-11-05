#!/usr/bin/env python3
"""Udawana centrala OptimaGSM na pseudoterminalu.

Port serwisowy jak w firmware'ze v3.4: XMODEM od strony centrali
(xmodem_receive, xmodem_transmit) i komendy 0x01, 0x24, 0x03, 0x02, 0x30,
0x04, 0x2F (svc_dispatch).
Konfiguracja ma obraz w RAM i „plik”, jak w centrali.
Odebrane zapisy i log trafiają do KATALOGU.
Manager pod Wine z tą centralą: tools/wine_udawana.sh.

Użycie: udawana_centrala.py KONFIGURACJA KATALOG [--link ŚCIEŻKA] [--moduly N]
    --link    dowiązanie do pseudoterminala
    --moduly  po udanym zapisie jeszcze N statusów (0–2),
              jak centrala z panelami TPR albo modułem w gnieździe 8
"""
import argparse
import os
import select
import sys
import time
import tty

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ropam_config as rc  # noqa: E402
from ropam_serwis import (  # noqa: E402
    ACK, CAN, CRC_REQ, EOT, SIZES, STATUS_SIZE, block_size, crc16_xmodem)

# 0x01: 0, wersja 3.4, wersja bootloadera ❓, 0x14, ID (syntetyczne)
DEFAULT_ID = bytes([0x00, 0x22, 0x22, 0x14]) + b"SYNTETYC"


class Line:
    """Strona centrali pseudoterminala; getc z czasem jak svc_getc."""

    def __init__(self, fd):
        self.fd = fd
        self.buf = bytearray()

    def getc(self, timeout_ms):
        if not self.buf:
            if select.select([self.fd], [], [], timeout_ms / 1000)[0]:
                try:
                    self.buf += os.read(self.fd, 4096)
                except OSError:
                    pass
        return self.buf.pop(0) if self.buf else None

    def read(self, n, timeout_ms):
        out = bytearray()
        while len(out) < n:
            c = self.getc(timeout_ms)
            if c is None:
                return None
            out.append(c)
        return bytes(out)

    def put(self, data):
        data = bytes(data)
        while data:
            # Nikt nie czyta przez 1 s: bajty przepadają jak w kablu.
            if not select.select([], [self.fd], [], 1)[1]:
                return
            data = data[os.write(self.fd, data):]

    def flush(self):
        """svc_flush: wyrzuca to, co zalega na wejściu."""
        self.buf.clear()
        while select.select([self.fd], [], [], 0)[0]:
            try:
                if not os.read(self.fd, 4096):
                    return
            except OSError:
                return


def xmodem_receive(line, maxlen):
    """Odbiór jak w firmware'ze: dane (≤ maxlen) albo None."""
    out = bytearray()
    expected = 1
    first = True
    while True:
        for _ in range(16):
            if first:
                line.put([CRC_REQ])
            c = line.getc(2000)
            if c in SIZES or c == EOT:
                break
            if c == CAN and line.getc(1000) == CAN:
                line.flush()
                line.put([ACK])
                return None
        else:
            line.flush()
            line.put([CAN] * 3)
            return None
        if c == EOT:
            line.flush()
            line.put([ACK])
            return bytes(out)
        first = False
        size = SIZES[c]
        rest = line.read(size + 4, 4000)
        if (rest is None or rest[0] != ~rest[1] & 0xFF or
                crc16_xmodem(rest[2:-2]) != int.from_bytes(rest[-2:], "big")):
            line.flush()
            line.put([0x15])  # NAK
            continue
        if rest[0] == expected & 0xFF:
            out += rest[2:-2][:max(0, maxlen - len(out))]
            expected += 1
        elif rest[0] != (expected - 1) & 0xFF:  # powtórzony dostaje ACK
            line.flush()
            line.put([0x15])
            continue
        line.put([ACK])


def xmodem_transmit(line, data):
    """Nadawanie jak w firmware'ze; odbiorca zaczyna od 'C'."""
    for _ in range(16):
        c = line.getc(2000)
        if c == CRC_REQ:
            line.flush()
            break
        if c == CAN and line.getc(1000) == CAN:
            line.put([ACK])
            return False
    else:
        line.put([CAN] * 3)
        return False
    head, size = block_size(len(data))
    # Dane równe wielokrotności bloku: firmware dokłada blok z samym 0x1A.
    for n, off in enumerate(range(0, len(data) + 1, size), 1):
        block = bytearray(data[off:off + size].ljust(size, b"\0"))
        if len(data) - off < size:
            block[len(data) - off] = 0x1A
        packet = (bytes([head, n & 0xFF, ~n & 0xFF]) + block
                  + crc16_xmodem(block).to_bytes(2, "big"))
        for _ in range(2):
            line.put(packet)
            if line.getc(1000) == ACK:
                break
        else:
            line.put([CAN] * 3)
            return False
    for _ in range(10):
        line.put([EOT])
        if line.getc(2000) == ACK:
            break
    line.flush()
    return True


class Panel:
    """Stan centrali: plik config.rpm (flash) i jego obraz g_config (RAM).

    Jak w firmware'ze: 0x03 wczytuje plik do RAM i wysyła RAM,
    0x02 odbiera prosto do RAM, 0x30 łata RAM,
    a plik zmienia się tylko przy zgodnym CRC.
    """

    def __init__(self, cfg, outdir, modules=0):
        self.flash = bytes(cfg)
        self.ram = bytearray(cfg)
        self.outdir = outdir
        self.modules = modules
        self.commands = []
        self.saved = 0
        self.log = open(os.path.join(outdir, "log.txt"), "a", buffering=1)

    def say(self, text):
        line = f"{time.strftime('%H:%M:%S')} {text}"
        print(line, flush=True)
        self.log.write(line + "\n")

    def save(self, name, data):
        self.saved += 1
        path = os.path.join(self.outdir, f"{self.saved:03d}_{name}")
        with open(path, "wb") as f:
            f.write(data)
        return path

    def status(self, line, value):
        self.say(f"   status {value}")
        xmodem_transmit(line, value.to_bytes(4, "little"))

    def handle(self, line, pkt):
        cmd = pkt[0]
        self.commands.append(cmd)
        if cmd == 0x01:
            self.say("0x01 identyfikacja")
            xmodem_transmit(line, DEFAULT_ID)
        elif cmd == 0x24:  # Manager pyta co 0,5 s, więc bez logu
            xmodem_transmit(line, bytes(STATUS_SIZE))
        elif cmd == 0x03:
            comm = rc.Config(self.flash).sections["COMM"][0]
            ok = pkt[1:7] == self.flash[comm + 1:comm + 7]
            self.say("0x03 odczyt konfiguracji, hasło "
                     + ("dobre" if ok else "ZŁE"))
            self.status(line, 1 if ok else 2)
            if ok:
                self.ram = bytearray(self.flash)
                xmodem_transmit(line, self.flash)
        elif cmd == 0x02:
            size = pkt[1] | pkt[2] << 8
            self.say(f"0x02 zapis całości, {size} B")
            data = (xmodem_receive(line, rc.CONFIG_SIZE)
                    if size == rc.CONFIG_SIZE else None)
            if data is None:
                self.status(line, 0)
                return
            self.ram[:len(data)] = data
            self.save("zapis_0x02.bin", data)
            crc = int.from_bytes(self.ram[rc.CRC_OFFSET:rc.CRC_OFFSET + 2],
                                 "little")
            self.commit(line, len(data) == rc.CONFIG_SIZE, crc)
        elif cmd == 0x30:
            self.patch(line, pkt)
        elif cmd == 0x2F:
            self.say("0x2F restart (udajemy, że centrala wstaje od razu)")
            self.status(line, 1)
            self.ram = bytearray(self.flash)
        elif cmd == 0x04:
            self.say("0x04 dziennik: pusty")
            xmodem_transmit(line, bytes(4))
            xmodem_transmit(line, bytes(100 * 16))
        else:
            self.say(f"0x{cmd:02X} nieobsługiwana, bez odpowiedzi: "
                     + self.save(f"komenda_{cmd:02x}.bin", pkt))

    def patch(self, line, pkt):
        """0x30: trójki (przesunięcie od DATA_START, wartość);
        CRC całego obrazu po łatce w pakiecie komendy (bajty 3–4)."""
        n = pkt[1] | pkt[2] << 8
        self.say(f"0x30 łatka, {n} B")
        data = xmodem_receive(line, n) if n < 199 else None
        if data is None or len(data) != n:
            self.status(line, 0)
            return
        self.save("dane_30.bin", data)
        for i in range(0, n - 2, 3):
            off = data[i] | data[i + 1] << 8
            if off >= 52000:
                self.status(line, 0)
                return
            self.ram[rc.DATA_START + off] = data[i + 2]
        self.commit(line, True, pkt[3] | pkt[4] << 8)

    def commit(self, line, complete, crc):
        """Wspólny koniec 0x02 i 0x30 (svc_dispatch, LAB_9d03c8d4)."""
        actual = crc16_xmodem(self.ram[:rc.CONFIG_SIZE - 4])
        if not complete or actual != crc:
            self.say(f"   ODRZUCONE: CRC obrazu {actual:04x},"
                     f" podane {crc:04x}")
            time.sleep(1)  # os_delay(10000)
            self.status(line, 0)
            return
        self.ram[rc.CRC_OFFSET:rc.CRC_OFFSET + 2] = crc.to_bytes(2, "little")
        diff = rc.differences(self.flash, self.ram)
        for i, desc, a, b in diff[:20]:
            self.say(f"   {desc}: {a:02x} -> {b:02x}")
        if len(diff) > 20:
            self.say(f"   … razem {len(diff)} B")
        self.flash = bytes(self.ram)
        self.say("   ZAPISANE: " + self.save("plik.bin", self.flash))
        self.status(line, 1)
        for _ in range(self.modules):
            time.sleep(0.5)  # ustawienia idą magistralą do modułów
            self.status(line, 1)

    def run(self, line):
        while True:
            pkt = xmodem_receive(line, 255)
            if pkt:
                self.handle(line, pkt)


def open_pty(link=None):
    """Pseudoterminal; strona podrzędna zostaje otwarta,
    żeby po zamknięciu portu przez klienta odczyt nie kończył się EIO."""
    master, slave = os.openpty()
    tty.setraw(slave)
    os.set_blocking(master, False)
    path = os.ttyname(slave)
    if link:
        if os.path.lexists(link):
            os.remove(link)
        os.symlink(path, link)
    return master, slave, path


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("konfiguracja")
    ap.add_argument("katalog")
    ap.add_argument("--link")
    ap.add_argument("--moduly", type=int, default=0, choices=(0, 1, 2))
    a = ap.parse_args()
    with open(a.konfiguracja, "rb") as f:
        cfg = f.read()[:rc.CONFIG_SIZE]
    if not rc.Config(cfg).crc_ok:
        sys.exit("konfiguracja ma złe CRC")
    os.makedirs(a.katalog, exist_ok=True)
    master, _slave, path = open_pty(a.link)
    panel = Panel(cfg, a.katalog, a.moduly)
    panel.say(f"udawana centrala na {path}")
    try:
        panel.run(Line(master))
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()
