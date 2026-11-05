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
    ropam_config.py PLIK stan PLIK_STANU       # stan z `ropam_serwis.py stan`
    ropam_config.py PLIK pola                  # pola, które umiemy zmieniać
    ropam_config.py PLIK ustaw WYNIK POLE=WARTOŚĆ …   # nowy blob, offline
    ropam_config.py PLIK roznica INNY          # różnice dwóch blobów

Zmiana w centrali: `ropam_serwis.py PORT zmien …` (docs/programowanie.md).
"""
import struct
import sys

CONFIG_SIZE = 52412  # firmware 3.4
DATA_START = 0x196
NSECTIONS_OFFSET = 0xCCB6
CRC_OFFSET = 0xCCB8

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


def _crc_table():
    table = []
    for i in range(256):
        crc = i << 8
        for _ in range(8):
            crc = ((crc << 1) ^ 0x1021 if crc & 0x8000 else crc << 1) & 0xFFFF
        table.append(crc)
    return table


_CRC_TABLE = _crc_table()


def crc16_xmodem(data):
    """CRC-16/XMODEM: wielomian 0x1021, start 0."""
    crc = 0
    for b in data:
        crc = (crc << 8 & 0xFFFF) ^ _CRC_TABLE[crc >> 8 ^ b]
    return crc


def text(raw):
    return raw.split(b"\0")[0].decode("cp1250", "replace")


class Config:
    def __init__(self, data):
        if len(data) < CONFIG_SIZE:
            raise ValueError(f"za krótki plik: {len(data)} B")
        self.data = data[:CONFIG_SIZE]
        self.crc_ok = (crc16_xmodem(self.data[:CONFIG_SIZE - 4])
                       == struct.unpack_from("<H", self.data, CRC_OFFSET)[0])
        self.sections = {}
        for i in range(self.data[NSECTIONS_OFFSET]):
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


def fix_crc(blob):
    """Wpisuje CRC liczone z pierwszych CONFIG_SIZE − 4 bajtów."""
    blob = bytearray(blob)
    blob[CRC_OFFSET:CRC_OFFSET + 2] = \
        crc16_xmodem(blob[:CONFIG_SIZE - 4]).to_bytes(2, "little")
    return blob


# Pola, które umiemy zmieniać:
# nazwa → (sekcja, przesunięcie, rodzaj, parametr).
# Rodzaje: "bit" (parametr: numer bitu), "tekst" (parametr: rozmiar pola
# razem z końcowym zerem). Opis pól: docs/konfiguracja.md.
FIELDS = {
    "COMM.sms_konfiguracja": ("COMM", 0, "bit", 0),
    "COMM.zdalny_tcp": ("COMM", 0, "bit", 1),
    "COMM.zdalny_kod_glowny": ("COMM", 0, "bit", 2),  # ❓
}
for _n in range(48):
    FIELDS[f"wejscie.{_n + 1}.nazwa"] = ("INPUT", 168 * _n, "tekst", 0x16)


def field_get(cfg, name):
    sec, off, kind, arg = FIELDS[name]
    data = cfg.section(sec)
    if kind == "bit":
        return data[off] >> arg & 1
    return text(data[off:off + arg])


def parse_changes(args):
    """„POLE=WARTOŚĆ” → [(pole, wartość)], sprawdzone przed użyciem."""
    changes = []
    for arg in args:
        name, sep, value = arg.partition("=")
        if not sep:
            raise ValueError(f"„{arg}”: oczekiwane POLE=WARTOŚĆ")
        if name not in FIELDS:
            raise ValueError(f"nieznane pole {name} (lista: `pola`)")
        kind, size = FIELDS[name][2:]
        if kind == "bit":
            if value not in ("0", "1"):
                raise ValueError(f"{name}: 0 albo 1")
            changes.append((name, int(value)))
            continue
        if not value.isascii() or not value.isprintable():
            raise ValueError(f"{name}: tylko drukowalne ASCII")
        if len(value) >= size:
            raise ValueError(f"{name}: najwyżej {size - 1} znaków")
        # Manager dopełnia nazwę zerami do końca pola.
        changes.append((name, value.encode().ljust(size, b"\0")))
    return changes


def apply_changes(blob, changes):
    """Blob z wprowadzonymi zmianami (z parse_changes) i nowym CRC."""
    cfg = Config(bytes(blob))
    blob = bytearray(blob)
    for name, value in changes:
        sec, off, kind, arg = FIELDS[name]
        pos = cfg.sections[sec][0] + off
        if kind == "bit":
            blob[pos] = blob[pos] & ~(1 << arg) | value << arg
        else:
            blob[pos:pos + arg] = value
    return fix_crc(blob)


def where(cfg, offset):
    """Opis miejsca w blobie: sekcja i przesunięcie w niej."""
    for name, (start, size) in cfg.sections.items():
        if start <= offset < start + size:
            return f"{name}[{offset - start}]"
    if offset < DATA_START:
        return "nagłówek"
    if offset >= CRC_OFFSET:
        return "CRC"
    if offset >= NSECTIONS_OFFSET:
        return "liczba sekcji"
    return "poza sekcjami"


def differences(old, new):
    """Lista (przesunięcie, opis, stary, nowy) dla różniących się bajtów."""
    cfg = Config(bytes(old))
    return [(i, where(cfg, i), old[i], new[i])
            for i in range(CONFIG_SIZE) if old[i] != new[i]]


def show_differences(old, new):
    diff = differences(old, new)
    for i, desc, a, b in diff:
        show = (lambda v: f"{v:02x} {chr(v) if 32 <= v < 127 else ' '}")
        print(f"  {i:#06x} {desc:20s} {show(a)} -> {show(b)}")
    if not diff:
        print("  bez różnic")
    return diff


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


def show_status(cfg, status):
    """Stan na żywo (zapis z `ropam_serwis.py PORT stan PLIK`) z nazwami."""
    inputs = cfg.records("INPUT", 168)
    outputs = cfg.records("OUT", 224)
    zones = cfg.records("STREFY", 44)
    masks = [int.from_bytes(status[o:o + 8], "little") for o in (0, 8)]
    for label, mask in zip(("naruszone", "stan 3 (sabotaż?)"), masks):
        # Tylko wejścia przypisane do jakiejś strefy albo nazwane inaczej
        # niż domyślnie mają sens; resztę raportujemy zbiorczo.
        named = [n for n in bits(mask, count=48)
                 if not text(inputs[int(n) - 1][:0x16]).startswith("Wejscie ")]
        rest = len(bits(mask, count=48)) - len(named)
        print(f"wejścia {label}:")
        for n in named:
            print(f"  I{n} {text(inputs[int(n) - 1][:0x16])}")
        if rest:
            print(f"  (+{rest} nieużywanych wejść)")
    out_mask = int.from_bytes(status[0x30:0x34], "little")
    print("wyjścia włączone:")
    for n in bits(out_mask, count=32):
        print(f"  O{n} {text(outputs[int(n) - 1][:0x14])}")
    print("strefy:")
    for z in range(4):
        word = int.from_bytes(status[0x4C + 8 * z:0x50 + 8 * z], "little")
        state = word & 0xF
        flags = [name for bit, name in ((10, "czuwanie"), (11, "noc"))
                 if word >> bit & 1]
        print(f"  S{z + 1} {text(zones[z][:0x10]):16s} stan {state}"
              f" {' '.join(flags)}")


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
    elif cmd == "stan" and len(sys.argv) == 4:
        with open(sys.argv[3], "rb") as f:
            show_status(cfg, f.read())
    elif cmd == "pola":
        for name in FIELDS:
            print(f"{name:28s} {field_get(cfg, name)}")
    elif cmd == "ustaw" and len(sys.argv) >= 5:
        try:
            blob = apply_changes(cfg.data, parse_changes(sys.argv[4:]))
        except ValueError as e:
            sys.exit(str(e))
        show_differences(cfg.data, blob)
        with open(sys.argv[3], "wb") as f:
            f.write(blob)
        print(f"zapisano {sys.argv[3]}")
    elif cmd == "roznica" and len(sys.argv) == 4:
        with open(sys.argv[3], "rb") as f:
            other = f.read()
        if len(other) < CONFIG_SIZE:
            sys.exit(f"{sys.argv[3]}: {len(other)} B,"
                     f" oczekiwane {CONFIG_SIZE}")
        show_differences(cfg.data, other[:CONFIG_SIZE])
    elif cmd == "zrzut" and len(sys.argv) == 4:
        sec = cfg.section(sys.argv[3])
        for i in range(0, len(sec), 16):
            print(f"{i:04x}  {sec[i:i + 16].hex(' ')}")
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
