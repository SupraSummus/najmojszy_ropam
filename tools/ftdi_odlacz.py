#!/usr/bin/env python3
"""Odłącza nadajnik kabla USB-MGSM (FT232BL) od linii RX centrali.

Przełącza układ w tryb bit-bang ze wszystkimi nóżkami jako wejściami:
TXD przestaje sterować linią,
zostaje tylko wewnętrzne podciąganie układu (ok. 200 kΩ),
które inny nadajnik (np. most na XIAO, esp32/) bez trudu przeważa.
Zasilony układ nie oddaje też na RX echa z TX centrali,
jak robi to niezasilony (sprawdzone na sprzęcie,
docs/protokol-serwisowy.md).

Stan zostaje po zakończeniu programu;
żeby kabel znów działał jako port szeregowy,
trzeba go odłączyć od USB i podłączyć ponownie.

Użycie:
    ftdi_odlacz.py           # pierwszy FT232 (0403:6001)
    ftdi_odlacz.py ODCZYT    # tylko pokazuje poziomy nóżek, nic nie zmienia
Wymaga libftdi1 z modułem Pythona (ftdi1).
"""
import sys

import ftdi1

VID, PID = 0x0403, 0x6001
# Nóżki w trybie bit-bang FT232: D0 = TXD, D1 = RXD, D2 = RTS#, D3 = CTS#, ...
PINS = ["TXD", "RXD", "RTS", "CTS", "DTR", "DSR", "DCD", "RI"]


def check(ctx, ret, what):
    if ret < 0:
        sys.exit(f"{what}: {ftdi1.get_error_string(ctx)} ({ret})")


def main():
    only_read = sys.argv[1:] == ["ODCZYT"]
    if sys.argv[1:] and not only_read:
        sys.exit(__doc__)
    ctx = ftdi1.new()
    check(ctx, ftdi1.usb_open(ctx, VID, PID), "otwarcie FT232")
    if not only_read:
        check(ctx, ftdi1.set_bitmode(ctx, 0x00, ftdi1.BITMODE_BITBANG),
              "tryb bit-bang")
    ret, pins = ftdi1.read_pins(ctx)
    check(ctx, ret, "odczyt nóżek")
    if isinstance(pins, (bytes, str)):
        pins = ord(pins[:1])
    print(" ".join(f"{name}={pins >> i & 1}" for i, name in enumerate(PINS)))
    if not only_read:
        print("TXD kabla nie steruje już linią (wszystkie nóżki to wejścia)")
    ftdi1.usb_close(ctx)
    ftdi1.free(ctx)


if __name__ == "__main__":
    main()
