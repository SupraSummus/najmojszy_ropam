#!/usr/bin/env python3
"""Rozbiór plików Intel HEX (np. firmware'u OptimaGSM w formacie *.flsh).

Użycie:
    ihex.py info PLIK.flsh
    ihex.py bin PLIK.flsh KATALOG   # każdy ciągły segment jako .bin
    ihex.py flat PLIK.hex WYJŚCIE.bin START KONIEC
        # płaski obraz [START, KONIEC), luki wypełnione 0xFF
"""
import sys
from pathlib import Path


def parse(path):
    """Zwraca słownik {adres: bajt} z rekordów danych."""
    mem = {}
    base = 0
    for n, line in enumerate(Path(path).read_text(encoding="ascii").splitlines(), 1):
        line = line.strip()
        if not line:
            continue
        if not line.startswith(":"):
            raise ValueError(f"linia {n}: brak ':'")
        raw = bytes.fromhex(line[1:])
        if sum(raw) & 0xFF:
            raise ValueError(f"linia {n}: zła suma kontrolna")
        length, addr, rtype = raw[0], int.from_bytes(raw[1:3], "big"), raw[3]
        data = raw[4:4 + length]
        if rtype == 0x00:
            for i, b in enumerate(data):
                mem[base + addr + i] = b
        elif rtype == 0x01:
            break
        elif rtype == 0x02:
            base = int.from_bytes(data, "big") << 4
        elif rtype == 0x04:
            base = int.from_bytes(data, "big") << 16
        elif rtype in (0x03, 0x05):
            pass  # adres startowy; nieistotny dla obrazu pamięci
        else:
            raise ValueError(f"linia {n}: nieznany typ rekordu {rtype:#x}")
    return mem


def segments(mem):
    """Dzieli obraz pamięci na ciągłe segmenty [(start, bytes)]."""
    out = []
    start = prev = None
    buf = bytearray()
    for a in sorted(mem):
        if prev is not None and a != prev + 1:
            out.append((start, bytes(buf)))
            buf = bytearray()
            start = None
        if start is None:
            start = a
        buf.append(mem[a])
        prev = a
    if buf:
        out.append((start, bytes(buf)))
    return out


def main():
    if len(sys.argv) < 3 or sys.argv[1] not in ("info", "bin", "flat"):
        sys.exit(__doc__)
    mem = parse(sys.argv[2])
    if sys.argv[1] == "flat":
        start, end = int(sys.argv[4], 0), int(sys.argv[5], 0)
        img = bytearray(b"\xff" * (end - start))
        for a, b in mem.items():
            if start <= a < end:
                img[a - start] = b
        Path(sys.argv[3]).write_bytes(img)
        return
    segs = segments(mem)
    if sys.argv[1] == "info":
        print(f"{len(mem)} bajtów w {len(segs)} segmentach")
        for start, data in segs:
            end = start + len(data) - 1
            print(f"  {start:#010x}–{end:#010x}  {len(data):8d} B")
    else:
        outdir = Path(sys.argv[3])
        outdir.mkdir(parents=True, exist_ok=True)
        for start, data in segs:
            (outdir / f"{start:08x}.bin").write_bytes(data)


if __name__ == "__main__":
    main()
