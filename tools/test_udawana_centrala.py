#!/usr/bin/env python3
"""Nasz klient (sprawdzony na prawdziwej centrali) kontra udawana centrala.

Sprawdza zapis konfiguracji ze wszystkimi zabezpieczeniami
i to, że udawana centrala odrzuca to samo co firmware.
Konfiguracja jest syntetyczna: sekcje INPUT i COMM, poprawne CRC.

Użycie: python3 tools/test_udawana_centrala.py
"""
import contextlib
import io
import os
import sys
import tempfile
import threading

import serial

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ropam_config as rc  # noqa: E402
import ropam_serwis as rs  # noqa: E402
import udawana_centrala as uc  # noqa: E402

PASSWORD = b"111111"
COMM = rc.DATA_START + 48 * 168 + 8  # za INPUT, sekcje rozdziela 8 B


def synthetic_config():
    blob = bytearray(rc.CONFIG_SIZE)
    blob[0:4] = rc.CONFIG_SIZE.to_bytes(4, "little")
    for entry, name, off, size in ((0x0E, b"INPUT", 0, 48 * 168),
                                   (0x1C, b"COMM", COMM - rc.DATA_START, 41)):
        blob[entry:entry + 12] = (name.ljust(8, b"\0")
                                  + off.to_bytes(2, "little")
                                  + size.to_bytes(2, "little"))
    blob[rc.NSECTIONS_OFFSET] = 2
    blob[COMM] = 0x01
    blob[COMM + 1:COMM + 7] = PASSWORD
    return bytes(rc.fix_crc(blob))


def start(cfg, modules=0):
    master, _slave, path = uc.open_pty()
    panel = uc.Panel(cfg, tempfile.mkdtemp(prefix="udawana-"), modules)
    threading.Thread(target=panel.run, args=(uc.Line(master),),
                     daemon=True).start()
    return panel, serial.serial_for_url(path, 115200, timeout=1)


def status(port):
    return int.from_bytes(rs.receive(port)[:4], "little")


def test_firmware_rejects(cfg):
    """Udawana centrala odrzuca to, co firmware, i nie zostawia śladów."""
    panel, port = start(cfg)
    rs.send(port, b"\x03000000")
    assert status(port) == 2, "złe hasło"

    bad = bytearray(cfg)
    bad[COMM + 10] ^= 0xFF  # bez poprawki CRC
    rs.send(port, b"\x02" + rc.CONFIG_SIZE.to_bytes(2, "little"))
    rs.send(port, bytes(bad))
    assert status(port) == 0, "0x02 ze złym CRC"

    rs.send(port, b"\x30\x03\x00\xef\xbe")  # CRC w pakiecie: 0xBEEF
    rs.send(port, (COMM - rc.DATA_START).to_bytes(2, "little") + b"\x03")
    assert status(port) == 0, "0x30 ze złym CRC"
    assert panel.flash == cfg, "plik bez zmian"
    assert rs.read_config_exact(port, PASSWORD) == cfg, "0x03 czyści RAM"


def test_program(cfg):
    """rs.program: potwierdzenie, łatka, odmowa, całość, statusy modułów."""
    panel, port = start(cfg, modules=2)
    yes = lambda: True  # noqa: E731
    old = rs.read_config_exact(port, PASSWORD)
    new = rc.apply_changes(old, rc.parse_changes(
        ["COMM.zdalny_tcp=1", "wejscie.2.nazwa=Kuchnia"]))

    assert not rs.program(port, PASSWORD, old, new, confirm=lambda: False)
    assert panel.flash == cfg, "bez potwierdzenia nic nie wysyła"

    assert rs.program(port, PASSWORD, old, new, confirm=yes)
    assert panel.flash == new and 0x30 in panel.commands, "łatka"

    # Obraz w RAM inny niż nasz odczyt: CRC się nie zgodzi.
    panel.ram[rc.DATA_START] ^= 0xFF
    back = rc.apply_changes(new, rc.parse_changes(["COMM.zdalny_tcp=0"]))
    assert not rs.program(port, PASSWORD, new, back, confirm=yes)
    assert panel.flash == new and panel.ram == bytearray(new), "odmowa"

    panel.commands.clear()
    assert rs.program(port, PASSWORD, new, cfg, full=True, confirm=yes)
    assert panel.flash == cfg and 0x02 in panel.commands, "całość"

    panel.commands.clear()
    assert rs.program(port, PASSWORD, cfg, cfg, confirm=yes)
    assert panel.commands == [], "bez różnicy nic nie wysyła"


def main():
    cfg = synthetic_config()
    out = io.StringIO()
    try:
        with contextlib.redirect_stdout(out):
            test_firmware_rejects(cfg)
            test_program(cfg)
    except BaseException:
        print(out.getvalue())
        raise
    print("OK")


if __name__ == "__main__":
    main()
