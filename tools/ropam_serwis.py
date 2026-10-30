#!/usr/bin/env python3
"""Klient protokołu serwisowego OptimaGSM (XMODEM na porcie RS232TTL).

Odtworzony z firmware'u v3.4 i sprawdzony na sprzęcie
(z OptimaGSM Managerem jako wzorcem).
Obsługuje wyłącznie operacje odczytu.
Celowo nie ma tu komend 0x26 i 0x2F: restartują centralę.

Każda komenda włącza w centrali flagę sesji serwisowej na ok. 1,5 min,
w czasie której część zdarzeń jest pomijana (docs/stan-centrali.md).

Użycie:
    ropam_serwis.py PORT nasluch [SEKUNDY]   # pokazuje, co nadaje centrala
    ropam_serwis.py PORT id                  # komenda 0x01
    ropam_serwis.py PORT stan [PLIK]         # komenda 0x24 (stan na żywo)
    ropam_serwis.py PORT konfiguracja HASŁO PLIK   # komenda 0x03
        HASŁO: 6 znaków, hex:xxxxxxxxxxxx albo plik:ŚCIEŻKA (hex w pliku)
    ROPAM_DEBUG=1 wypisuje każdy bajt na stderr.

PORT to np. /dev/ttyUSB0 (kabel USB-MGSM), /dev/ttyACM0 (most na XIAO, esp32/)
albo socket://ADRES:PORT (dowolny most TCP).
Wymaga pyserial.
"""
import os
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


DEBUG = os.environ.get("ROPAM_DEBUG") == "1"


def log(direction, data):
    if DEBUG and data:
        print(f"{time.monotonic():.3f} {direction} {data.hex(' ')}",
              file=sys.stderr)


def write(port, data):
    log(">>", data)
    port.write(data)


def read(port, n, timeout):
    port.timeout = timeout
    data = port.read(n)
    log("<<", data)
    return data


def read_byte(port, timeout):
    b = read(port, 1, timeout)
    return b[0] if b else None


def wait_for(port, wanted, timeout):
    end = time.monotonic() + timeout
    while time.monotonic() < end:
        b = read_byte(port, end - time.monotonic())
        if b in wanted:
            return b
    return None


def wait_fresh_invite(port, timeout, quiet=0.5):
    """Czeka na 'C' poprzedzone ciszą na linii.

    Zaległe bajty z przerwanych sesji (seria 'C', CAN-y) potrafią dotrzeć
    nawet po reset_input_buffer(); prawdziwe zaproszenie to pojedyncze 'C'
    co 2 s, więc przed nim linia milczy.
    """
    port.reset_input_buffer()
    end = time.monotonic() + timeout
    last = time.monotonic()
    while time.monotonic() < end:
        b = read_byte(port, 0.1)
        now = time.monotonic()
        if b is None:
            continue
        if b == CRC_REQ and now - last >= quiet:
            return True
        last = now
    return False


def send(port, data, timeout=10):
    """Wysyła dane XMODEM-em do centrali (centrala zaczyna od 'C')."""
    if not wait_fresh_invite(port, timeout):
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
            write(port, packet)
            # Liczy się tylko ACK; NAK albo cisza oznacza ponowienie.
            if wait_for(port, {ACK, NAK}, 2) == ACK:
                break
        else:
            raise IOError(f"blok {n} nie potwierdzony")
    for _ in range(10):
        write(port, bytes([EOT]))
        if wait_for(port, {ACK}, 2) == ACK:
            return
    raise IOError("EOT nie potwierdzony")


def receive(port, timeout=10):
    """Odbiera dane XMODEM-em od centrali (z dopełnieniem)."""
    out = bytearray()
    expected = 1
    end = time.monotonic() + timeout
    write(port, bytes([CRC_REQ]))
    while True:
        head = read_byte(port, 2)
        if head is None:
            if time.monotonic() > end:
                raise TimeoutError("brak danych od centrali")
            if not out:
                write(port, bytes([CRC_REQ]))
            continue
        if head == EOT:
            write(port, bytes([ACK]))
            return bytes(out)
        if head == CAN:
            raise IOError("centrala przerwała transmisję")
        if head not in SIZES:
            continue
        size = SIZES[head]
        rest = read(port, 2 + size + 2, 2)
        if len(rest) != 2 + size + 2 or rest[0] != (~rest[1] & 0xFF):
            write(port, bytes([NAK]))
            continue
        payload = rest[2:2 + size]
        if crc16_xmodem(payload) != int.from_bytes(rest[-2:], "big"):
            write(port, bytes([NAK]))
            continue
        if rest[0] == expected & 0xFF:
            out += payload
            expected += 1
        write(port, bytes([ACK]))


def strip_padding(data):
    i = data.rfind(0x1A)
    return data[:i] if i >= 0 and not any(data[i + 1:]) else data


STATUS_SIZE = 1100


def bits(value, count):
    return [i + 1 for i in range(count) if value >> i & 1]


def print_status(data):
    """Wypisuje pola stanu (komenda 0x24) rozpoznane w firmware'ze."""
    u64 = [int.from_bytes(data[o:o + 8], "little") for o in (0, 8, 16, 24)]
    print("wejścia naruszone:", bits(u64[0], 48) or "-")
    print("wejścia w sabotażu:", bits(u64[1], 48) or "-")
    print("wejścia z flagą 2 (?):", bits(u64[2], 48) or "-")
    print("wejścia z flagą 3 (?):", bits(u64[3], 48) or "-")
    outputs = int.from_bytes(data[0x30:0x34], "little")
    print("wyjścia włączone:", bits(outputs, 32) or "-")
    analog = [int.from_bytes(data[0xDC + 2 * i:0xDE + 2 * i], "little")
              for i in range(48)]
    print("wartości linii wejść (surowe):", analog)


def main():
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    port = serial.serial_for_url(sys.argv[1], 115200, timeout=1)
    cmd = sys.argv[2]
    if cmd == "nasluch":
        end = time.monotonic() + float(sys.argv[3] if len(sys.argv) > 3
                                       else 10)
        while time.monotonic() < end:
            b = read(port, 64, 1)
            if b:
                print(f"{time.strftime('%H:%M:%S')}  {b.hex(' ')}  {b!r}")
    elif cmd == "id":
        send(port, bytes([0x01]))
        print(strip_padding(receive(port)).hex(" "))
    elif cmd == "stan" and len(sys.argv) in (3, 4):
        send(port, bytes([0x24]))
        data = receive(port)[:STATUS_SIZE]
        # Opcjonalna druga transmisja (odpowiedź modemu): centrala czeka
        # wtedy na nasze 'C', zamiast od razu nadawać własne.
        if wait_for(port, {CRC_REQ}, 1.5) is None:
            extra = strip_padding(receive(port))
            print("modem:", extra.decode("latin-1", "replace").strip())
        if len(sys.argv) == 4:
            with open(sys.argv[3], "wb") as f:
                f.write(data)
        print_status(data)
    elif cmd == "konfiguracja" and len(sys.argv) == 5:
        arg = sys.argv[3]
        if arg.startswith("plik:"):
            with open(arg[5:]) as f:
                arg = "hex:" + f.read().strip()
        code = (bytes.fromhex(arg[4:]) if arg.startswith("hex:")
                else arg.encode("ascii"))
        if len(code) != 6:
            sys.exit("kod musi mieć 6 bajtów")
        send(port, bytes([0x03]) + code)
        # Najpierw status (1 = OK, 2 = złe hasło), potem osobno konfiguracja.
        status = int.from_bytes(receive(port)[:4], "little")
        if status != 1:
            sys.exit(f"centrala nie wydała konfiguracji, status {status}"
                     + (" (złe hasło)" if status == 2 else ""))
        data = strip_padding(receive(port, timeout=120))
        with open(sys.argv[4], "wb") as f:
            f.write(data)
        print(f"zapisano {len(data)} B")
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main()
