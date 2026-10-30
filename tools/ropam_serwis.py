#!/usr/bin/env python3
"""Klient protokołu serwisowego OptimaGSM (XMODEM na porcie RS232TTL).

Odtworzony z firmware'u v3.4, jeszcze niezweryfikowany na sprzęcie.
Obsługuje wyłącznie operacje odczytu.

Użycie:
    ropam_serwis.py PORT nasluch [SEKUNDY]   # pokazuje, co nadaje centrala
    ropam_serwis.py PORT id                  # komenda 0x01
    ropam_serwis.py PORT konfiguracja KOD PLIK   # komenda 0x03

PORT to np. /dev/ttyUSB0. Wymaga pyserial.
"""
import sys
import time

import serial

SOH, STX, EOT, ACK, NAK, CAN = 0x01, 0x02, 0x04, 0x06, 0x15, 0x18
BLK64 = 0x19  # rozszerzenie Ropamu: blok 64 B
CRC_REQ = ord("C")
SIZES = {BLK64: 64, SOH: 128, STX: 1024}


def crc16_xmodem(data):
    crc = 0
    for b in data:
        crc ^= b << 8
        for _ in range(8):
            crc = (crc << 1) ^ 0x1021 if crc & 0x8000 else crc << 1
            crc &= 0xFFFF
    return crc


def block_size(total):
    """Ten sam dobór rozmiaru bloku co w centrali."""
    if total >= 0x400:
        return STX, 1024
    if total > 0x40:
        return SOH, 128
    return BLK64, 64


def read_byte(port, timeout):
    port.timeout = timeout
    b = port.read(1)
    return b[0] if b else None


def wait_for(port, wanted, timeout):
    end = time.monotonic() + timeout
    while time.monotonic() < end:
        b = read_byte(port, end - time.monotonic())
        if b in wanted:
            return b
    return None


def send(port, data, timeout=10):
    """Wysyła dane XMODEM-em do centrali (centrala zaczyna od 'C')."""
    if wait_for(port, {CRC_REQ}, timeout) is None:
        raise TimeoutError("centrala nie wysłała 'C'")
    head, size = block_size(len(data))
    for n, off in enumerate(range(0, max(len(data), 1), size), 1):
        chunk = data[off:off + size]
        payload = bytearray(size)
        payload[:len(chunk)] = chunk
        if len(chunk) < size:
            payload[len(chunk)] = 0x1A
        crc = crc16_xmodem(payload)
        packet = bytes([head, n & 0xFF, ~n & 0xFF]) + payload \
            + crc.to_bytes(2, "big")
        for _ in range(10):
            port.write(packet)
            if wait_for(port, {ACK, NAK, CAN}, 2) == ACK:
                break
        else:
            raise IOError(f"blok {n} nie potwierdzony")
    for _ in range(10):
        port.write(bytes([EOT]))
        if wait_for(port, {ACK}, 2) == ACK:
            return
    raise IOError("EOT nie potwierdzony")


def receive(port, timeout=10):
    """Odbiera dane XMODEM-em od centrali (z dopełnieniem)."""
    out = bytearray()
    expected = 1
    end = time.monotonic() + timeout
    port.write(bytes([CRC_REQ]))
    while True:
        head = read_byte(port, 2)
        if head is None:
            if time.monotonic() > end:
                raise TimeoutError("brak danych od centrali")
            if not out:
                port.write(bytes([CRC_REQ]))
            continue
        if head == EOT:
            port.write(bytes([ACK]))
            return bytes(out)
        if head == CAN:
            raise IOError("centrala przerwała transmisję")
        if head not in SIZES:
            continue
        size = SIZES[head]
        port.timeout = 2
        rest = port.read(2 + size + 2)
        if len(rest) != 2 + size + 2 or rest[0] != (~rest[1] & 0xFF):
            port.write(bytes([NAK]))
            continue
        payload = rest[2:2 + size]
        if crc16_xmodem(payload) != int.from_bytes(rest[-2:], "big"):
            port.write(bytes([NAK]))
            continue
        if rest[0] == expected & 0xFF:
            out += payload
            expected += 1
        port.write(bytes([ACK]))


def strip_padding(data):
    i = data.rfind(0x1A)
    return data[:i] if i >= 0 and not any(data[i + 1:]) else data


def main():
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    port = serial.Serial(sys.argv[1], 115200, timeout=1)
    cmd = sys.argv[2]
    if cmd == "nasluch":
        end = time.monotonic() + float(sys.argv[3] if len(sys.argv) > 3
                                       else 10)
        while time.monotonic() < end:
            b = port.read(64)
            if b:
                print(f"{time.strftime('%H:%M:%S')}  {b.hex(' ')}  {b!r}")
    elif cmd == "id":
        send(port, bytes([0x01]))
        print(strip_padding(receive(port)).hex(" "))
    elif cmd == "konfiguracja" and len(sys.argv) == 5:
        code = sys.argv[3].encode("ascii")
        if len(code) != 6:
            sys.exit("kod serwisowy musi mieć 6 znaków")
        send(port, bytes([0x03]) + code)
        data = receive(port, timeout=120)
        with open(sys.argv[4], "wb") as f:
            f.write(data)
        print(f"zapisano {len(data)} B (z dopełnieniem XMODEM)")
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
