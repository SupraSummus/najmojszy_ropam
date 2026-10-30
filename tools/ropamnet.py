#!/usr/bin/env python3
"""Ramki magistrali RopamNET wg firmware'u OptimaGSM v3.4.

Odtworzone z kodu (rnet_send_frame, rnet_recv_frame, rnet_crc16),
jeszcze niezweryfikowane na prawdziwej magistrali.

Użycie:
    ropamnet.py ADRES KOMENDA [DANE_HEX]   # wypisuje ramkę zapytania
    ropamnet.py check RAMKA_HEX            # sprawdza CRC ramki
    ropamnet.py analizuj PLIK [KONFIGURACJA]
        # dzieli surowy zapis z magistrali (binarny) na ramki;
        # z konfiguracją dekoduje rozgłaszany stan (0x10) z nazwami
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
    już po przesunięciu, a nie przed nim.
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
    **{c: f"koncentrator 0x{c:02X}" for c in range(0xF0, 0xF8)},
}
DEVICE_CMDS = {
    0x11: "stan panelu", 0x12: "weryfikacja kodu", 0x16: "strefa bez kodu",
    0x1C: "uzbrojenie kodem", 0x1D: "działanie kodem na strefach",
    0x20: "odczyt dziennika", 0x23: "ustawienia panelu",
    0x24: "ustawienia panelu",
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
    cfg = None
    if config_path:
        sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
        import ropam_config
        with open(config_path, "rb") as f:
            cfg = ropam_config.Config(f.read())
    for off, kind, value in parse_stream(data):
        if kind == "ramka":
            addr, cmd, payload = value
            print(f"{off:08x}  [{addr:02x} {cmd:02x} {len(payload):3d} B]"
                  f"  {describe(addr, cmd)}")
            if cmd == 0x10 and addr != REPLY_START and cfg:
                ropam_config.show_status(cfg, payload)
        elif kind == "ack":
            names = {ACK_OK: "OK", ACK_BAD_CRC: "zły CRC"}
            print(f"{off:08x}  ack 0x{value:02x} {names.get(value, '')}")
        else:
            print(f"{off:08x}  ? {len(value)} B: {value[:16].hex(' ')}")


def main():
    args = sys.argv[1:]
    if args[:1] == ["analizuj"] and len(args) in (2, 3):
        analyze(*args[1:])
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
