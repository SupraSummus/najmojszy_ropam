#!/usr/bin/env python3
"""Odszyfrowanie firmware'u OptimaGSM (*.flsh) do zwykłego Intel HEX.

Plik *.flsh to Intel HEX, w którym bajty danych są XOR-owane
16-bajtowym kluczem według pozycji bajtu w rekordzie.
Sumy kontrolne rekordów nie zgadzają się ani przed, ani po odszyfrowaniu,
więc zapisujemy je od nowa.
Na końcu pliku są dwie linie spoza formatu: `=xxxx` i `?wersja`.

Użycie:
    flsh.py PLIK.flsh > PLIK.hex
"""
import sys
from pathlib import Path

KEY = bytes.fromhex("6452f7b6dce00df19831cf4bf9f3339f")


def decode(path):
    """Zwraca (linie Intel HEX, linie trailera)."""
    out, trailer = [], []
    for line in Path(path).read_text(encoding="latin-1").split():
        if not line.startswith(":"):
            trailer.append(line)
            continue
        raw = bytearray(bytes.fromhex(line[1:]))
        if raw[3] == 0x00:
            for i in range(raw[0]):
                raw[4 + i] ^= KEY[i % len(KEY)]
        raw[-1] = -sum(raw[:-1]) & 0xFF
        out.append(":" + raw.hex().upper())
    return out, trailer


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    lines, trailer = decode(sys.argv[1])
    print("\n".join(lines))
    for t in trailer:
        print(f"trailer: {t}", file=sys.stderr)


if __name__ == "__main__":
    main()
