#!/usr/bin/env python3
"""Ramki magistrali RopamNET wg firmware'u OptimaGSM v3.4.

Odtworzone z kodu (rnet_send_frame, rnet_recv_frame, rnet_crc16)
i sprawdzone podsłuchem prawdziwej magistrali (format ramek, CRC,
potwierdzenia, rozgłaszany stan 0x10).

Użycie:
    ropamnet.py ADRES KOMENDA [DANE_HEX]   # wypisuje ramkę zapytania
    ropamnet.py check RAMKA_HEX            # sprawdza CRC ramki
    ropamnet.py analizuj PLIK [KONFIGURACJA]
        # dzieli surowy zapis z magistrali (binarny) na ramki;
        # z konfiguracją dekoduje rozgłaszany stan (0x10) z nazwami
    ropamnet.py log PLIK|- [KONFIGURACJA]
        # opisuje linie z podsłuchu na XIAO (esp32/rvuos/ropam-podsluch.c),
        # z pliku albo na żywo ze standardowego wejścia
"""
import os
import sys

ACK_OK = 0x03
ACK_BAD_CRC = 0x01
ACK_TIMEOUT = 0x02  # wartość, którą centrala przyjmuje przy braku bajtu

REPLY_START = 0xAA  # pierwszy bajt odpowiedzi urządzenia
BROADCAST = 0xFE  # adres bez potwierdzenia
NO_DATA = (0xAA, 0xFF)  # wartości pola długości oznaczające brak danych


def crc16(data):
    """CRC-16 jak w firmware'ze: start 0xFFFF, wielomian 0xA001.

    Uwaga: w odróżnieniu od CRC-16/MODBUS bit 0 jest testowany
    już po przesunięciu, a nie przed nim,
    więc CRC nie zależy od bitu 0 żadnego bajtu.
    Tak liczą centrala i moduły na magistrali (sprawdzone podsłuchem).
    """
    crc = 0xFFFF
    for b in data:
        crc ^= b
        for _ in range(8):
            crc >>= 1
            if crc & 1:
                crc ^= 0xA001
    return crc


# Nazwy komend z docs/protokol-ramek.md (❓ = przypuszczenie)
MASTER_CMDS = {
    0x10: "stan systemu (rozgłoszenie)", 0x2B: "odpytanie",
    0x3C: "rozgłoszenie 0x3C", 0x9B: "budzenie (port 1)",
    0xDC: "opis modułu? (AP-AERO)", 0xDD: "wymiana stanu modułu",
    0xDF: "pełny stan modułu (AP-AERO)",
    **{c: f"koncentrator 0x{c:02X}" for c in range(0xF0, 0xF8)},
}
DEVICE_CMDS = {
    0x11: "stan panelu", 0x12: "weryfikacja kodu", 0x16: "strefa bez kodu",
    0x1C: "uzbrojenie kodem", 0x1D: "działanie kodem na strefach",
    0x20: "odczyt dziennika", 0x23: "ustawienia panelu",
    0x24: "ustawienia panelu",
    0xDD: "stan modułu", 0xDF: "pełny stan modułu (AP-AERO)",
}


def frame(addr, cmd, data=b""):
    """Ramka: adres, komenda, długość, dane, CRC (starszy bajt pierwszy)."""
    if len(data) > 255 or len(data) in NO_DATA:
        raise ValueError("długości 0xAA i 0xFF oznaczają brak danych")
    head = bytes([addr, cmd, len(data)])
    crc = crc16(head + data)
    return head + data + crc.to_bytes(2, "big")


def check(raw):
    """Zwraca (adres, komenda, dane, czy_crc_ok)."""
    addr, cmd, length = raw[0], raw[1], raw[2]
    n = 0 if length in NO_DATA else length
    body, crc = raw[:3 + n], raw[3 + n:5 + n]
    return addr, cmd, raw[3:3 + n], crc16(body) == int.from_bytes(crc, "big")


def parse_stream(data):
    """Dzieli surowy strumień na ramki, potwierdzenia i nierozpoznane bajty.

    Zwraca listę krotek (przesunięcie, rodzaj, wartość):
    ("ramka", (adres, komenda, dane)), ("ack", bajt), ("?", bajty).
    """
    out, junk, i = [], bytearray(), 0
    while i < len(data):
        if i + 5 <= len(data):
            length = data[i + 2]
            n = 0 if length in NO_DATA else length
            end = i + 3 + n + 2
            if end <= len(data) and crc16(data[i:i + 3 + n]) == \
                    int.from_bytes(data[i + 3 + n:end], "big"):
                if junk:
                    out.append((i - len(junk), "?", bytes(junk)))
                    junk = bytearray()
                out.append((i, "ramka", (data[i], data[i + 1],
                                         bytes(data[i + 3:i + 3 + n]))))
                i = end
                continue
        if data[i] in (ACK_OK, ACK_BAD_CRC, ACK_TIMEOUT) and not junk:
            out.append((i, "ack", data[i]))
        else:
            junk.append(data[i])
        i += 1
    if junk:
        out.append((len(data) - len(junk), "?", bytes(junk)))
    return out


def describe(addr, cmd):
    if addr == REPLY_START:
        return f"urządzenie → centrala  {DEVICE_CMDS.get(cmd, '')}"
    who = "wszyscy" if addr == BROADCAST else f"0x{addr:02X}"
    return f"centrala → {who:8s} {MASTER_CMDS.get(cmd, '')}"


def analyze(path, config_path=None):
    with open(path, "rb") as f:
        data = f.read()
    cfg = load_config(config_path)
    for off, kind, value in parse_stream(data):
        if kind == "ramka":
            addr, cmd, payload = value
            print(f"{off:08x}  [{addr:02x} {cmd:02x} {len(payload):3d} B]"
                  f"  {describe(addr, cmd)}")
            if cmd == 0x10 and addr != REPLY_START and cfg:
                import ropam_config
                ropam_config.show_status(cfg, payload)
        elif kind == "ack":
            names = {ACK_OK: "OK", ACK_BAD_CRC: "zły CRC"}
            print(f"{off:08x}  ack 0x{value:02x} {names.get(value, '')}")
        else:
            print(f"{off:08x}  ? {len(value)} B: {value[:16].hex(' ')}")


def bits(value, count):
    return [i + 1 for i in range(count) if value >> i & 1]


def status_summary(st):
    """Jedna linia z nagłówka stanu (rozgłoszenie 0x10, docs/stan-centrali.md)."""
    if len(st) < 0xD6:
        return "za krótki nagłówek stanu"
    y, mo, d, h, mi, se = st[0xD0:0xD6]
    viol = bits(int.from_bytes(st[0:8], "little"), 48)
    outs = bits(int.from_bytes(st[0x30:0x34], "little"), 32)
    zones = []
    for z in range(4):
        word = int.from_bytes(st[0x4C + 8 * z:0x50 + 8 * z], "little")
        mode = "czuwa" if word >> 10 & 1 else ""
        mode += " noc" if word >> 11 & 1 else ""
        zones.append(f"S{z + 1}={word & 0xF}{('(' + mode.strip() + ')') if mode else ''}")
    session = int.from_bytes(st[0xCE:0xD0], "little") >> 1 & 1
    return (f"20{y:02d}-{mo:02d}-{d:02d} {h:02d}:{mi:02d}:{se:02d}"
            f"  naruszone {viol or '-'}  wyjścia {outs or '-'}  {' '.join(zones)}"
            f"{'  SESJA SERWISOWA' if session else ''}")


def load_config(config_path):
    if not config_path:
        return None
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    import ropam_config
    with open(config_path, "rb") as f:
        return ropam_config.Config(f.read())


def log(path, config_path=None):
    """Linie z ropam-podsluch: "S.UUUUUU K N[ !R!Z!P]: xx xx …".

    K to rodzaj pozycji wg parsera na XIAO: R ramka, A potwierdzenie, ? śmieci.
    Pozycje „?” próbujemy tu jeszcze podzielić na ramki
    (starsze zapisy dzieliły strumień po przerwach, nie po ramkach).
    """
    cfg = load_config(config_path)
    src = sys.stdin if path == "-" else open(path, encoding="ascii",
                                             errors="replace")
    for raw in src:
        line = raw.strip()
        head, sep, hexes = line.partition(":")
        fields = head.split()
        if not sep or len(fields) < 3 or line.startswith("#"):
            if line:
                print(line, flush=True)
            continue
        stamp, kind, flags = fields[0], fields[1], " ".join(fields[3:])
        try:
            data = bytes.fromhex(hexes)
        except ValueError:
            print(f"{stamp}  nieczytelna linia: {line}", flush=True)
            continue
        if kind == "R":
            parts = [(0, "ramka", check(data)[:3])]
        else:
            parts = parse_stream(data)
        for _, what, value in parts:
            if what == "ramka":
                addr, cmd, payload = value
                print(f"{stamp}  [{addr:02x} {cmd:02x} {len(payload):3d} B]"
                      f"  {describe(addr, cmd)} {flags}", flush=True)
                if cmd == 0x11 and addr == REPLY_START and len(payload) == 10:
                    volts = int.from_bytes(payload[4:6], "little") / 100
                    print(f"{stamp}    panel: zasilanie {volts:.2f} V"
                          f"  fn={payload[9] & 1}", flush=True)
                if cmd == 0x10 and addr != REPLY_START:
                    print(f"{stamp}    {status_summary(payload)}", flush=True)
                    if cfg:
                        import ropam_config
                        ropam_config.show_status(cfg, payload)
            elif what == "ack":
                names = {ACK_OK: "OK", ACK_BAD_CRC: "zły CRC"}
                print(f"{stamp}  ack 0x{value:02x} {names.get(value, '')}"
                      f" {flags}", flush=True)
            else:
                print(f"{stamp}  ? {len(value)} B: {value[:16].hex(' ')}"
                      f" {flags}", flush=True)


def main():
    args = sys.argv[1:]
    if args[:1] == ["analizuj"] and len(args) in (2, 3):
        analyze(*args[1:])
        return
    if args[:1] == ["log"] and len(args) in (2, 3):
        log(*args[1:])
        return
    if len(args) == 2 and args[0] == "check":
        addr, cmd, data, ok = check(bytes.fromhex(args[1]))
        print(f"adres={addr:#04x} komenda={cmd:#04x} "
              f"dane={data.hex(' ') or '-'} crc={'OK' if ok else 'ZŁE'}")
    elif len(args) in (2, 3):
        data = bytes.fromhex(args[2]) if len(args) == 3 else b""
        print(frame(int(args[0], 0), int(args[1], 0), data).hex(" "))
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
