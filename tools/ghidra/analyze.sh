#!/usr/bin/env bash
# Analiza firmware'u OptimaGSM w Ghidrze bez GUI.
# Użycie: tools/ghidra/analyze.sh firmware/optimagsm/optimagsm_v3.4_pl.flsh
# Symbole: re/<nazwa>/symbols.txt (jeśli istnieje)
# Wyniki: build/<nazwa>/ (projekt Ghidry, obraz, decomp.c)
#         re/<nazwa>/src/ (C funkcji nazwanych w symbols.txt)
set -euo pipefail

here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/../.." && pwd)
flsh=$1
name=$(basename "$flsh" .flsh)
out=$root/build/$name
re=$root/re/$name
headless=${GHIDRA_HEADLESS:-/opt/ghidra/support/analyzeHeadless}

mkdir -p "$out"
python3 -I "$root/tools/flsh.py" "$flsh" > "$out/$name.hex"
python3 -I "$root/tools/ihex.py" flat "$out/$name.hex" "$out/$name.bin" \
    0x1D000000 0x1D080000

post=()
if [[ -f $re/symbols.txt ]]; then
    post+=(-postScript ApplySymbols.java "$re/symbols.txt")
    post+=(-postScript ExportDecomp.java "$out/decomp.c" "$re/src")
else
    post+=(-postScript ExportDecomp.java "$out/decomp.c")
fi

"$headless" "$out" "$name" -overwrite \
    -import "$out/$name.bin" \
    -loader BinaryLoader -loader-baseAddr 0x9D000000 \
    -processor MIPS:LE:32:default \
    -scriptPath "$here" \
    -preScript PrepPic32.java 0xA0007FF0 \
    "${post[@]}"
