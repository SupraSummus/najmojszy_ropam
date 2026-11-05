#!/usr/bin/env python3
"""Profil instalacji dla strony bramki na XIAO (esp32/rvuos/strona.c).

Z konfiguracji centrali bierze nazwy wejść, wyjść i stref,
typy wejść i ich strefy, a z pliku OCZEKIWANIA stany,
których strona ma pilnować.
Wypisuje nagłówek C ze stałą strukturą `strona_profil`.
Zawiera dane instalacji, więc trafia tylko do katalogu budowania,
nigdy do repo; buduje go `esp32/rvuos/zbuduj.sh bramka`.

Używane są wejścia i wyjścia z nazwą inną niż domyślna
albo z oczekiwanym stanem
i strefy, do których należy któreś z używanych wejść.

OCZEKIWANIA: linie `wejscie.N=naruszone|w_normie`,
`wyjscie.N=wlaczone|wylaczone`; `#` zaczyna komentarz.

Użycie: profil_bramki.py KONFIGURACJA [OCZEKIWANIA]
"""
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ropam_config as rc  # noqa: E402

STATES = {"wejscie": {"naruszone": 1, "w_normie": 0},
          "wyjscie": {"wlaczone": 1, "wylaczone": 0}}
COUNTS = {"wejscie": 48, "wyjscie": 32}


def c_string(s):
    """Literał C w UTF-8; ósemkowe sekwencje, bo \\x połyka następne cyfry."""
    if s is None:
        return "0"
    out = []
    for b in s.encode("utf-8"):
        ch = chr(b)
        out.append(ch if 0x20 <= b < 0x7F and ch not in '"\\?' else f"\\{b:03o}")
    return '"' + "".join(out) + '"'


def used(name, default, expected):
    """Nazwa albo None, gdy domyślna z Managera („Wejscie 7”) bez oczekiwań."""
    name = name.strip()
    if expected or name and not re.fullmatch(default + r" \d+", name):
        return name or default
    return None


def expectations(path):
    known = {"wejscie": 0, "wyjscie": 0}
    expected = {"wejscie": 0, "wyjscie": 0}
    with open(path, encoding="utf-8") as f:
        for n, line in enumerate(f, 1):
            line = line.split("#")[0].strip()
            if not line:
                continue
            m = re.fullmatch(r"(wejscie|wyjscie)\.(\d+)\s*=\s*(\w+)", line)
            kind = m and m[1]
            if (not m or m[3] not in STATES[kind]
                    or not 1 <= int(m[2]) <= COUNTS[kind]):
                sys.exit(f"{path}:{n}: nie rozumiem: {line}")
            bit = 1 << int(m[2]) - 1
            known[kind] |= bit
            if STATES[kind][m[3]]:
                expected[kind] |= bit
    return known, expected


def profile(cfg, known, expected):
    inputs, types, zones = [], [], []
    for n, r in enumerate(cfg.records("INPUT", 168)):
        name = used(rc.text(r[:0x16]), "Wejscie", known["wejscie"] >> n & 1)
        flags, = struct.unpack_from("<I", r, 0x24)
        inputs.append(name)
        types.append(struct.unpack_from("<H", r, 0x16)[0] if name else 0)
        zones.append(flags & 0xF if name else 0)
    outputs = [used(rc.text(r[:0x14]), "Wyjscie", known["wyjscie"] >> n & 1)
               for n, r in enumerate(cfg.records("OUT", 224))]
    any_zone = 0
    for z in zones:
        any_zone |= z
    zone_names = [rc.text(r[:0x10]).strip() or f"S{n}" if any_zone >> n - 1 & 1
                  else None
                  for n, r in enumerate(cfg.records("STREFY", 44), 1)]

    def array(values, conv=str):
        return "{ " + ", ".join(conv(v) for v in values) + " }"

    return "\n".join([
        "/* Wygenerowane przez tools/profil_bramki.py; dane instalacji,"
        " nie do repo. */",
        "static const struct strona_profil profil_instalacji = {",
        f"    .inputs = {array(inputs, c_string)},",
        f"    .input_type = {array(types)},",
        f"    .input_zones = {array(zones)},",
        f"    .outputs = {array(outputs, c_string)},",
        f"    .zones = {array(zone_names, c_string)},",
        f"    .inputs_known = 0x{known['wejscie']:x}ull,",
        f"    .inputs_expected = 0x{expected['wejscie']:x}ull,",
        f"    .outputs_known = 0x{known['wyjscie']:x}u,",
        f"    .outputs_expected = 0x{expected['wyjscie']:x}u,",
        "};",
        "#define PROFIL (&profil_instalacji)",
        "",
    ])


def main():
    if len(sys.argv) not in (2, 3):
        sys.exit(__doc__)
    with open(sys.argv[1], "rb") as f:
        cfg = rc.Config(f.read())
    if not cfg.crc_ok:
        sys.exit("konfiguracja ma złe CRC")
    known = expected = {"wejscie": 0, "wyjscie": 0}  # tylko do odczytu
    if len(sys.argv) == 3:
        known, expected = expectations(sys.argv[2])
    sys.stdout.write(profile(cfg, known, expected))


if __name__ == "__main__":
    main()
