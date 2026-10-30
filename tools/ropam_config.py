#!/usr/bin/env python3
"""Czytelny podgląd konfiguracji OptimaGSM (blob config.rpm z centrali).

Blob pobiera `ropam_serwis.py PORT konfiguracja …`.
Format i znaczenie pól: docs/konfiguracja.md.

Użycie:
    ropam_config.py PLIK sekcje
    ropam_config.py PLIK wejscia
    ropam_config.py PLIK wyjscia
    ropam_config.py PLIK strefy
    ropam_config.py PLIK zrzut NAZWA_SEKCJI    # surowe bajty sekcji (hex)
"""
import struct
import sys

CONFIG_SIZE = 52412  # firmware 3.4
DATA_START = 0x196

INPUT_TYPES = [
    "ZWYKŁA CICHA", "24h", "ZWYKŁA", "INFO", "ZAŁ/WYŁ CZUWANIE",
    "OPÓŹNIONA", "OPÓŹNIONA WAR.", "LICZNIKOWA", "KASUJĄCA ALARM",
    "ZAŁ. CZUWANIE", "WYŁ. CZUWANIE",
]

# Kolejność z listy w OptimaGSM Managerze (newoutcheck) ❓
OUTPUT_SOURCES = [
    "Alarm", "Sabotaż", "Czuwanie pełne", "Czuwanie noc", "Czas wejścia",
    "Czas wyjścia", "Potwierdzenie pulsami", "Logic processor",
    "Awaria zbiorcza", "Brak AC", "SMS", "CLIP", "Kod DTMF", "WWW",
    "Timer 1", "Timer 2", "Timer 3", "Timer 4",
]


def crc16_xmodem(data):
    crc = 0
    for b in data:
        crc ^= b << 8
        for _ in range(8):
            crc = (crc << 1) ^ 0x1021 if crc & 0x8000 else crc << 1
            crc &= 0xFFFF
    return crc


def text(raw):
    return raw.split(b"\0")[0].decode("cp1250", "replace")


class Config:
    def __init__(self, data):
        if len(data) < CONFIG_SIZE:
            raise ValueError(f"za krótki plik: {len(data)} B")
        self.data = data[:CONFIG_SIZE]
        self.crc_ok = (crc16_xmodem(self.data[:CONFIG_SIZE - 4])
                       == struct.unpack_from("<H", self.data, 0xCCB8)[0])
        self.sections = {}
        for i in range(self.data[0xCCB6]):
            entry = self.data[0x0E + 14 * i:0x0E + 14 * (i + 1)]
            off, size = struct.unpack_from("<HH", entry, 8)
            start = DATA_START + off
            self.sections[text(entry[:8])] = (start, size)

    def section(self, name):
        start, size = self.sections[name]
        return self.data[start:start + size]

    def records(self, name, size):
        sec = self.section(name)
        return [sec[i:i + size] for i in range(0, len(sec), size)]


def bits(value, names=None, count=32):
    out = []
    for i in range(count):
        if value >> i & 1:
            out.append(names[i] if names and i < len(names) else str(i + 1))
    return out


def show_sections(cfg):
    print(f"CRC: {'zgodne' if cfg.crc_ok else 'NIEZGODNE'}")
    for name, (start, size) in cfg.sections.items():
        print(f"{name:8s} 0x{start:04x} {size:6d} B")


def show_inputs(cfg):
    for n, r in enumerate(cfg.records("INPUT", 168), 1):
        name = text(r[:0x16])
        typ, = struct.unpack_from("<H", r, 0x16)
        react, = struct.unpack_from("<I", r, 0x18)
        block, = struct.unpack_from("<H", r, 0x20)
        flags, = struct.unpack_from("<I", r, 0x24)
        tname = INPUT_TYPES[typ] if typ < len(INPUT_TYPES) else f"typ {typ}"
        zones = bits(flags & 0xF, count=4)
        extra = []
        if flags & 0x10:
            extra.append("nocna")
        if flags & ~0x1F:
            extra.append(f"flagi 0x{flags & ~0x1F:x}")
        print(f"I{n:<2d} {name:22s} {tname:17s}"
              f" strefy {','.join(zones) or '-':8s}"
              f" reakcja {react} ms, blokada {block} min,"
              f" polaryzacja {r[0x28] & 0x1F} {' '.join(extra)}")


def show_outputs(cfg):
    for n, r in enumerate(cfg.records("OUT", 224), 1):
        name = text(r[:0x14])
        src, = struct.unpack_from("<I", r, 0x18)
        time_s, = struct.unpack_from("<H", r, 0x1C)
        print(f"O{n:<2d} {name:20s} czas {time_s:4d} s  źródła:"
              f" {', '.join(bits(src, OUTPUT_SOURCES)) or '-'}")


def show_zones(cfg):
    for n, r in enumerate(cfg.records("STREFY", 44), 1):
        entry, exit_ = struct.unpack_from("<II", r, 0x18)
        print(f"S{n} {text(r[:0x10]):16s} czas na wejście {entry} s,"
              f" na wyjście {exit_} s")


def main():
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    with open(sys.argv[1], "rb") as f:
        cfg = Config(f.read())
    cmd = sys.argv[2]
    if cmd == "sekcje":
        show_sections(cfg)
    elif cmd == "wejscia":
        show_inputs(cfg)
    elif cmd == "wyjscia":
        show_outputs(cfg)
    elif cmd == "strefy":
        show_zones(cfg)
    elif cmd == "zrzut" and len(sys.argv) == 4:
        sec = cfg.section(sys.argv[3])
        for i in range(0, len(sec), 16):
            print(f"{i:04x}  {sec[i:i + 16].hex(' ')}")
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
