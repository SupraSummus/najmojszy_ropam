#!/usr/bin/env bash
# Uruchamia OptimaGSM Managera pod Wine z COM1 podpiętym wprost do portu
# i zapisem całego ruchu przez LD_PRELOAD (tools/ttysniff).
# (Pseudoterminal z socat nie działa: Wine zgłasza błąd GetCommState.)
#
# Użycie: tools/wine_sniff.sh [PORT] [PLIK_LOGU]
#   PORT       domyślnie /dev/ttyUSB0
#   PLIK_LOGU  domyślnie private/sniff_<data>.log (może zawierać hasła!)
# Zmienne: WINEPREFIX (domyślnie ~/.local/share/wine-ropam),
#          MANAGER (ścieżka do OptimaGSMManager.exe).
set -euo pipefail

root=$(cd "$(dirname "$0")/.." && pwd)
port=${1:-/dev/ttyUSB0}
log=$(realpath -m "${2:-$root/private/sniff_$(date +%Y%m%d_%H%M%S).log}")
export WINEPREFIX=${WINEPREFIX:-$HOME/.local/share/wine-ropam}
export WINEDEBUG=${WINEDEBUG:--all}
export WINEDLLOVERRIDES=${WINEDLLOVERRIDES:-mscoree,mshtml=}
manager=${MANAGER:-$root/vendor/optimagsm-manager-2.5/app/OptimaGSMManager.exe}
so=$root/build/ttysniff.so

mkdir -p "$(dirname "$log")" "$root/build"
gcc -shared -fPIC -O2 -o "$so" "$root/tools/ttysniff/ttysniff.c" -ldl

# Samo dowiązanie w dosdevices Wine nadpisuje przy starcie; trwałe
# przypisanie portu jest w rejestrze.
wine reg add 'HKLM\Software\Wine\Ports' /v COM1 /d "$port" /f >/dev/null
wineserver -w
echo "COM1 -> $port, log: $log"
cd "$(dirname "$manager")"
TTYSNIFF_LOG=$log TTYSNIFF_MATCH=$(basename "$port") LD_PRELOAD=$so \
    wine "$(basename "$manager")"
