#!/usr/bin/env bash
# Buduje program rvuos z esp32/rvuos/ dla ESP32-C6 i opcjonalnie wgrywa go.
#
# Użycie: esp32/rvuos/zbuduj.sh [PROGRAM] [wgraj] [PORT]
#   PROGRAM  nazwa pliku bez .c, domyślnie ropam-most
#   wgraj    po zbudowaniu wgrywa obraz na płytkę
#   PORT     domyślnie /dev/ttyACM0
#
# rvuos to submoduł third_party/rvuos; skrypt nakłada na niego łatki
# z esp32/rvuos/*.patch (jeśli jeszcze nie są nałożone)
# i podlinkowuje programy do jego user/.
set -euo pipefail

here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/../.." && pwd)
rvuos=$root/third_party/rvuos
program=${1:-ropam-most}
port=${3:-/dev/ttyACM0}

git -C "$root" submodule update --init third_party/rvuos
for patch in "$here"/*.patch; do
    if git -C "$rvuos" apply --reverse --check "$patch" 2>/dev/null; then
        continue  # już nałożona
    fi
    git -C "$rvuos" apply "$patch"
    echo "nałożono $(basename "$patch")"
done
ln -sfn "$here/$program.c" "$rvuos/user/$program.c"
# Biblioteki (ropam.c, tcp.c, …) programy dołączają jako "ropam/plik.c":
# budowanie rvuos kompiluje jeden plik na program.
mkdir -p "$rvuos/user/ropam"
for f in "$here"/*.c "$here"/*.h; do
    ln -sfn "$f" "$rvuos/user/ropam/$(basename "$f")"
done

image=build/esp32c6/kernel-$program.bin
make -C "$rvuos" BOARD=esp32c6 USER_PROGRAMS="$program" "$image"
echo "obraz: $rvuos/$image"

if [[ ${2:-} == wgraj ]]; then
    # Program nie kończy się, więc narzędzie rvuos zakończy się po czasie;
    # to nie jest błąd.
    (cd "$rvuos" && python3 tools/esp32c6-run.py --chip esp32c6 --port "$port" \
        --timeout 3 "$image") || true
fi
