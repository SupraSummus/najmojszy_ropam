#!/usr/bin/env python3
"""Ramki magistrali RopamNET wg firmware'u OptimaGSM v3.4.

Odtworzone z kodu (FUN_9d052f50, FUN_9d0532dc, FUN_9d052d48),
jeszcze niezweryfikowane na prawdziwej magistrali.

Użycie:
    ropamnet.py ADRES KOMENDA [DANE_HEX]   # wypisuje ramkę zapytania
    ropamnet.py check RAMKA_HEX            # sprawdza CRC ramki
"""
import sys

ACK_OK = 0x03
ACK_BAD_CRC = 0x01
ACK_TIMEOUT = 0x02  # wartość, którą centrala przyjmuje przy braku bajtu

REPLY_START = 0xAA  # pierwszy bajt odpowiedzi urządzenia
BROADCAST = 0xFE  # adres bez potwierdzenia
NO_DATA = (0xAA, 0xFF)  # wartości pola długości oznaczające brak danych


def crc16(data):
    """CRC-16 jak w firmware'ze: start 0xFFFF, wielomian 0xA001.

    Uwaga: w odróżnieniu od CRC-16/MODBUS bit 0 jest testowany
    już po przesunięciu, a nie przed nim.
    """
    crc = 0xFFFF
    for b in data:
        crc ^= b
        for _ in range(8):
            crc >>= 1
            if crc & 1:
                crc ^= 0xA001
    return crc


def frame(addr, cmd, data=b""):
    """Ramka: adres, komenda, długość, dane, CRC (starszy bajt pierwszy)."""
    if len(data) > 200:
        raise ValueError("za długie dane")
    head = bytes([addr, cmd, len(data)])
    crc = crc16(head + data)
    return head + data + crc.to_bytes(2, "big")


def check(raw):
    """Zwraca (adres, komenda, dane, czy_crc_ok)."""
    addr, cmd, length = raw[0], raw[1], raw[2]
    n = 0 if length in NO_DATA else length
    body, crc = raw[:3 + n], raw[3 + n:5 + n]
    return addr, cmd, raw[3:3 + n], crc16(body) == int.from_bytes(crc, "big")


def main():
    args = sys.argv[1:]
    if len(args) == 2 and args[0] == "check":
        addr, cmd, data, ok = check(bytes.fromhex(args[1]))
        print(f"adres={addr:#04x} komenda={cmd:#04x} "
              f"dane={data.hex(' ') or '-'} crc={'OK' if ok else 'ZŁE'}")
    elif len(args) in (2, 3):
        data = bytes.fromhex(args[2]) if len(args) == 3 else b""
        print(frame(int(args[0], 0), int(args[1], 0), data).hex(" "))
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
