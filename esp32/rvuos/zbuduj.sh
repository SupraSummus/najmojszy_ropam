#!/usr/bin/env bash
# Buduje program rvuos z esp32/rvuos/ dla ESP32-C6 i opcjonalnie wgrywa go.
#
# Użycie: esp32/rvuos/zbuduj.sh [PROGRAM] [wgraj] [PORT]
#   PROGRAM  nazwa pliku bez .c, domyślnie ropam-most;
#            „bramka” to system Wi-Fi rvuos z klientem centrali i stroną HTTP
#            (sieć z private/wifi.conf: linie ssid= i pass=)
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
[[ $program == bramka ]] || ln -sfn "$here/$program.c" "$rvuos/user/$program.c"
# Biblioteki (ropam.c, tcp.c, …) programy dołączają jako "ropam/plik.c":
# budowanie rvuos kompiluje jeden plik na program.
mkdir -p "$rvuos/user/ropam"
for f in "$here"/*.c "$here"/*.h; do
    ln -sfn "$f" "$rvuos/user/ropam/$(basename "$f")"
done

if [[ $program == bramka ]]; then
    make -C "$rvuos" BOARD=esp32c6 build/esp32c6/kernel-wifi-esp32c6.bin build/esp32c6/wifi-drv.bin
    if [[ ${2:-} == wgraj ]]; then
        conf=$root/private/wifi.conf
        [[ -f $conf ]] || { echo "brak $conf (linie ssid= i pass=)" >&2; exit 1; }
        # Ustawienia trafiają do flasha XIAO (adres INPUT_FLASH w rvuos);
        # run=0: system działa bez końca, a nie 30 s jak przebieg testowy rvuos.
        tmp=$(mktemp)
        trap 'rm -f "$tmp"' EXIT
        chmod 600 "$tmp"
        cat "$conf" > "$tmp"
        grep -q '^run=' "$conf" || printf '\nrun=0\n' >> "$tmp"
        # Sterownik pod FLASH_WINDOW_FLASH; minuta logu startu, w nim adres IP, potem XIAO pracuje dalej.
        (cd "$rvuos" && python3 tools/esp32c6-run.py --chip esp32c6 --port "$port" --timeout 60 \
            --flash 0x210000:build/esp32c6/wifi-drv.bin --text 0x80000:"$tmp" \
            build/esp32c6/kernel-wifi-esp32c6.bin) || true
    fi
    exit
fi

image=build/esp32c6/kernel-$program.bin
make -C "$rvuos" BOARD=esp32c6 USER_PROGRAMS="$program" "$image"
echo "obraz: $rvuos/$image"

if [[ ${2:-} == wgraj ]]; then
    # Program nie kończy się, więc narzędzie rvuos zakończy się po czasie;
    # to nie jest błąd.
    (cd "$rvuos" && python3 tools/esp32c6-run.py --chip esp32c6 --port "$port" \
        --timeout 3 "$image") || true
fi
