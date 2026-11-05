#!/usr/bin/env bash
# OptimaGSM Manager pod Wine rozmawia z udawaną centralą
# (tools/udawana_centrala.py) zamiast z prawdziwą.
# Pozwala podejrzeć, co Manager wysyła przy zapisie, bez ryzyka.
#
# COM1 to pseudoterminal udawanej centrali; tools/ttysniff udaje dla niego
# ioctl-e linii modemu i blokuje otwarcie prawdziwych portów
# (/dev/ttyUSB*, /dev/ttyACM*), na które Wine sam zakłada dowiązania COMn.
#
# Użycie: tools/wine_udawana.sh KONFIGURACJA KATALOG [opcje udawanej centrali]
#   KONFIGURACJA  blob z centrali; prawdziwy trzymamy w private/
#   KATALOG       log i odebrane zapisy, np. private/udawana
# Zmienne: WINEPREFIX (domyślnie ~/.local/share/wine-ropam; najlepiej kopia,
#          bo skrypt zmienia w nim COM1), MANAGER, DISPLAY (np. :99 z Xvfb).
set -euo pipefail

[ $# -ge 2 ] || { sed -n '2,15p' "$0"; exit 1; }
root=$(cd "$(dirname "$0")/.." && pwd)
cfg=$(realpath "$1")
out=$(realpath -m "$2")
shift 2
export WINEPREFIX=${WINEPREFIX:-$HOME/.local/share/wine-ropam}
export WINEDEBUG=${WINEDEBUG:--all}
export WINEDLLOVERRIDES=${WINEDLLOVERRIDES:-mscoree,mshtml=}
manager=${MANAGER:-$root/vendor/optimagsm-manager-2.5/app/OptimaGSMManager.exe}
so=$root/build/ttysniff.so
link=$out/com1

mkdir -p "$out" "$root/build"
gcc -shared -fPIC -O2 -o "$so" "$root/tools/ttysniff/ttysniff.c" -ldl

# Wineserver musi wystartować z LD_PRELOAD, bo to on otwiera porty.
wineserver -k 2>/dev/null || true
wineserver -w

rm -f "$link"
python3 -u "$root/tools/udawana_centrala.py" "$cfg" "$out" --link "$link" "$@" &
panel=$!
trap 'kill $panel 2>/dev/null || true' EXIT
for _ in $(seq 50); do [ -L "$link" ] && break; sleep 0.1; done
pty=$(readlink -f "$link")

export TTYSNIFF_LOG=$out/manager_sniff.log
export TTYSNIFF_MATCH=$pty
export TTYSNIFF_BLOCK=/dev/ttyUSB:/dev/ttyACM
export LD_PRELOAD=$so
wine reg add 'HKLM\Software\Wine\Ports' /v COM1 /d "$pty" /f >/dev/null
echo "COM1 -> $pty (udawana centrala), log: $out/log.txt"
cd "$(dirname "$manager")"
wine "$(basename "$manager")"
