#!/usr/bin/env python3
"""Klient protokołu serwisowego OptimaGSM (XMODEM na porcie RS232TTL).

Odtworzony z firmware'u v3.4 i sprawdzony na sprzęcie
(z OptimaGSM Managerem jako wzorcem).
Obsługuje operacje odczytu; wyjątki (testowy SMS, polecenia SMS
sterujące) wymagają ROPAM_STERUJ=1.
Zmiana konfiguracji (zmien, przywroc) wymaga ROPAM_ZAPIS=1
i potwierdzenia; zasady w docs/programowanie.md.
Celowo nie ma tu komend 0x1F i 0x2F (restart centrali)
ani 0x05 (kasuje dziennik zdarzeń i testuje wyjścia).

Każda komenda włącza w centrali flagę sesji serwisowej na ok. 1,5 min,
w czasie której część zdarzeń jest pomijana (docs/stan-centrali.md).
Klient gasi ją od razu po każdej komendzie (end_session);
ROPAM_ZOSTAW_SESJE=1 to wyłącza.

Użycie:
    ropam_serwis.py PORT nasluch [SEKUNDY]   # pokazuje, co nadaje centrala
    ropam_serwis.py PORT id                  # komenda 0x01
    ropam_serwis.py PORT stan [PLIK]         # komenda 0x24 (stan na żywo)
    ropam_serwis.py PORT konfiguracja HASŁO PLIK   # komenda 0x03
    ropam_serwis.py PORT dziennik [PLIK]     # komenda 0x04: ostatnie 1000 zdarzeń
    ropam_serwis.py PORT raport              # komenda 0x4C: raport diagnostyczny
    ropam_serwis.py PORT sms KOD POLECENIE   # komenda 0x19: polecenie SMS kablem
        KOD: 4 cyfry albo plik:ŚCIEŻKA; bez ROPAM_STERUJ=1 tylko polecenia
        odczytujące (SMS_READ_ONLY), bo inne uzbrajają, sterują wyjściami itd.
    ropam_serwis.py PORT wyslijsms TEKST     # komenda 0x13: prawdziwy SMS
        z TEKST na pierwszy numer sekcji TEL (albo „NUMER;treść”);
        tylko z ROPAM_STERUJ=1
    ropam_serwis.py PORT koniec              # od razu kończy sesję serwisową
    ropam_serwis.py PORT zmien HASŁO POLE=WARTOŚĆ …
        świeży odczyt, kopia w private/kopie/, różnica; z ROPAM_ZAPIS=1
        i po wpisaniu „tak” zapis (0x30 albo 0x02) i odczyt sprawdzający;
        pola: ropam_config.py PLIK pola
    ropam_serwis.py PORT przywroc HASŁO PLIK # wgranie kopii (0x02), jak wyżej
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

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ropam_config as rc  # noqa: E402

SOH, STX, EOT, ACK, NAK, CAN = 0x01, 0x02, 0x04, 0x06, 0x15, 0x18
BLK64 = 0x19  # rozszerzenie Ropamu: blok 64 B
CRC_REQ = ord("C")
SIZES = {BLK64: 64, SOH: 128, STX: 1024}


crc16_xmodem = rc.crc16_xmodem


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
    Zwraca True albo liczbę bajtów odebranych bez ciszy przed 'C'.
    """
    port.reset_input_buffer()
    end = time.monotonic() + timeout
    last = time.monotonic()
    seen = 0
    while time.monotonic() < end:
        b = read_byte(port, 0.1)
        now = time.monotonic()
        if b is None:
            continue
        if b == CRC_REQ and now - last >= quiet:
            return True
        last = now
        seen += 1
    return seen


def send(port, data, timeout=10):
    """Wysyła dane XMODEM-em do centrali (centrala zaczyna od 'C')."""
    invite = wait_fresh_invite(port, timeout)
    if invite is not True:
        if invite > 10 * timeout:
            # Zob. docs/protokol-serwisowy.md, „Objaw: centrala słyszy własne echo”.
            raise TimeoutError(
                f"centrala nadaje bez przerwy ({invite} B w {timeout} s): "
                "najpewniej słyszy własne echo, "
                "więc nasz nadajnik nie steruje jej linią RX")
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
            # Liczy się tylko ACK; NAK albo cisza oznacza ponowienie,
            # a po pierwszym bloku także nowe 'C' (centrala zaprasza od nowa).
            if wait_for(port, {ACK, NAK} | ({CRC_REQ} if n == 1 else set()),
                        2) == ACK:
                break
        else:
            raise IOError(f"blok {n} nie potwierdzony")
    for _ in range(10):
        write(port, bytes([EOT]))
        if wait_for(port, {ACK}, 2) == ACK:
            return
    raise IOError("EOT nie potwierdzony")


def purge(port, quiet=0.05, limit=2):
    """Wyrzuca bajty, aż linia ucichnie na quiet sekund (najwyżej limit s)."""
    end = time.monotonic() + limit
    while time.monotonic() < end and read(port, 4096, quiet):
        pass


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
            # Przerwanie to dwa CAN z rzędu; pojedynczy to najpewniej
            # uszkodzony bajt.
            if read_byte(port, 0.1) == CAN:
                raise IOError("centrala przerwała transmisję")
            continue
        if head not in SIZES:
            continue
        size = SIZES[head]
        rest = read(port, 2 + size + 2, 2)
        payload = rest[2:2 + size]
        if (len(rest) != 2 + size + 2 or rest[0] != (~rest[1] & 0xFF)
                or crc16_xmodem(payload) != int.from_bytes(rest[-2:], "big")):
            # Reszta złego bloku może być jeszcze w drodze; bez czekania
            # na ciszę jej bajty 0x18 i 0x04 wzięlibyśmy za CAN i EOT.
            purge(port)
            write(port, bytes([NAK]))
            continue
        if rest[0] == expected & 0xFF:
            out += payload
            expected += 1
        write(port, bytes([ACK]))


def strip_padding(data):
    i = data.rfind(0x1A)
    return data[:i] if i >= 0 and not any(data[i + 1:]) else data


def end_session(port, timeout=10):
    """Gasi flagę sesji serwisowej bez czekania ok. 1,5 min.

    Flaga gaśnie po trzech kolejnych nieudanych odbiorach komendy
    (task_service), a odbiór przerwany przez nadawcę dwoma CAN
    kończy się od razu ACK-iem (xmodem_receive).
    Następne zaproszenie 'C' przychodzi wtedy natychmiast.
    """
    if wait_fresh_invite(port, timeout) is not True:
        raise TimeoutError("centrala nie wysłała 'C'")
    for n in range(3):
        if n:
            if wait_for(port, {CRC_REQ}, 3) is None:
                raise TimeoutError("brak 'C' po przerwaniu")
        write(port, bytes([CAN, CAN]))
        if wait_for(port, {ACK}, 2) != ACK:
            raise IOError("centrala nie potwierdziła przerwania")


# Polecenia SMS, które tylko odczytują (choć „stan” zapisuje zdarzenie 0x16).
SMS_READ_ONLY = {"stan", "wejscia", "lines", "gprsstat", "netstat", "lanstat",
                 "hardware"}


def sms_command(port, code, command):
    """Komenda 0x19: polecenie SMS „KKKK polecenie” wykonane przez kabel.

    Centrala odsyła tekst, który wysłałaby SMS-em (do trzech wiadomości);
    zadanie modemu nie rusza wtedy kolejki SMS-ów, więc nic nie wychodzi
    do sieci. Bez ważnego kodu centrala nic nie odsyła.
    """
    if len(code) != 4:
        raise ValueError("kod poleceń SMS ma dokładnie 4 znaki")
    send(port, bytes([0x19]) + f"{code} {command}".encode("ascii") + b"\0")
    reply = strip_padding(receive(port, timeout=30)).split(b"\0")[0]
    return reply.decode("cp1250", "replace")


STATUS_SIZE = 1100
LOG_RECORD = 16


def read_log(port):
    """Komenda 0x04 z argumentem 0: ostatnie (najwyżej) 1000 zdarzeń.

    Centrala wysyła najpierw liczbę zdarzeń (4 B),
    potem osobnymi transmisjami paczki po 100 rekordów po 16 B,
    od paczki 0 do paczki zawierającej ostatni rekord włącznie.
    Zwraca (liczba, rekordy); rekordów może być więcej niż zdarzeń,
    bo ostatnia paczka jest zawsze pełna.
    """
    send(port, bytes([0x04, 0x00]))
    count = int.from_bytes(receive(port)[:4], "little")
    blocks = min(count, 1000) // 100 + 1
    data = bytearray()
    for _ in range(blocks):
        data += receive(port)[:100 * LOG_RECORD]
    records = [bytes(data[i:i + LOG_RECORD])
               for i in range(0, len(data), LOG_RECORD)]
    return count, records[:min(count, 1000)]


def parse_password(arg):
    """Hasło komunikacji: 6 znaków, hex:… albo plik:ŚCIEŻKA (hex w pliku)."""
    if arg.startswith("plik:"):
        with open(arg[5:]) as f:
            arg = "hex:" + f.read().strip()
    code = (bytes.fromhex(arg[4:]) if arg.startswith("hex:")
            else arg.encode("ascii"))
    if len(code) != 6:
        raise ValueError("hasło komunikacji ma 6 bajtów")
    return code


def read_config(port, code):
    """Komenda 0x03: status, potem konfiguracja (z dopełnieniem XMODEM)."""
    send(port, bytes([0x03]) + code)
    status = int.from_bytes(receive(port)[:4], "little")
    if status != 1:
        raise PermissionError(
            f"centrala nie wydała konfiguracji, status {status}"
            + (" (złe hasło)" if status == 2 else ""))
    return receive(port, timeout=120)


def read_config_exact(port, code):
    """Konfiguracja do zapisu: tylko format, który znamy (firmware 3.4).

    Rozmiar formatu jest zapisany na początku bloba.
    """
    data = read_config(port, code)[:rc.CONFIG_SIZE]
    if (int.from_bytes(data[:4], "little") != rc.CONFIG_SIZE
            or not rc.Config(data).crc_ok):
        raise RuntimeError("konfiguracja w nieznanym formacie"
                           " albo ze złym CRC")
    return data


PATCH_MAX = 66  # dane łatki 0x30 muszą mieć mniej niż 199 B
PATCH_END = rc.DATA_START + 52000  # łatka sięga tylko danych sekcji


def finish_write(port):
    """Statusy po zapisie (0x02, 0x30).

    Po udanym zapisie centrala może odesłać jeszcze do dwóch statusów,
    po wysłaniu ustawień modułom (docs/programowanie.md).
    Gdy skończyła, od razu zaprasza 'C' do następnej komendy;
    gdy milczy, czeka na nasze 'C' z kolejnym statusem.
    """
    statuses = [int.from_bytes(receive(port, timeout=60)[:4], "little")]
    while statuses[0] == 1 and len(statuses) < 3:
        if wait_for(port, {CRC_REQ}, 1.5) is not None:
            break
        statuses.append(int.from_bytes(receive(port, timeout=60)[:4],
                                       "little"))
    return statuses


def write_patch(port, changed, new):
    """Komenda 0x30: trójki (przesunięcie od DATA_START, wartość),
    a w pakiecie komendy CRC całego obrazu po łatce.

    Centrala łata obraz konfiguracji, który wczytał nasz odczyt 0x03;
    gdy coś go w międzyczasie zmieniło, CRC się nie zgodzi.
    """
    triples = b"".join((i - rc.DATA_START).to_bytes(2, "little")
                       + bytes([new[i]]) for i in changed)
    crc = rc.crc16_xmodem(new[:rc.CONFIG_SIZE - 4])
    send(port, bytes([0x30]) + len(triples).to_bytes(2, "little")
         + crc.to_bytes(2, "little"))
    send(port, triples)
    return finish_write(port)


def write_full(port, new):
    """Komenda 0x02: cała konfiguracja."""
    send(port, bytes([0x02]) + rc.CONFIG_SIZE.to_bytes(2, "little"))
    send(port, bytes(new), timeout=10)
    return finish_write(port)


def save_backup(data):
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    folder = os.environ.get("ROPAM_KOPIE",
                            os.path.join(root, "private", "kopie"))
    os.makedirs(folder, exist_ok=True)
    stem = os.path.join(folder, time.strftime("konfiguracja_%Y%m%d_%H%M%S"))
    for n in range(100):
        path = stem + (f"_{n}" if n else "") + ".bin"
        try:
            fd = os.open(path, os.O_WRONLY | os.O_CREAT | os.O_EXCL, 0o600)
        except FileExistsError:
            continue
        with os.fdopen(fd, "wb") as f:
            f.write(data)
        return path
    raise FileExistsError(stem)


def confirm_write():
    if os.environ.get("ROPAM_ZAPIS") != "1":
        print("na sucho: nic nie zapisano (ROPAM_ZAPIS=1, żeby zapisać)")
        return False
    try:
        answer = input("Zapisać to do centrali? Wpisz „tak”: ")
    except EOFError:
        answer = ""
    if answer.strip() != "tak":
        print("przerwane, nic nie zapisano")
        return False
    return True


def program(port, code, old, new, full=False, confirm=confirm_write):
    """Różnica, potwierdzenie, zapis, odczyt sprawdzający.

    Bez `full` zapisuje tylko, gdy jest różnica, i wybiera łatkę,
    jeśli się da. Zwraca True, gdy centrala ma dokładnie `new`.
    """
    diff = rc.show_differences(old, new)
    if not diff and not full:
        print("nic do zapisania")
        return True
    if not confirm():
        return False
    crc = range(rc.CRC_OFFSET, rc.CRC_OFFSET + 2)
    changed = [i for i, *_ in diff if i not in crc]
    if full or len(changed) > PATCH_MAX or any(
            not rc.DATA_START <= i < PATCH_END for i in changed):
        print("zapis całości (0x02)")
        statuses = write_full(port, new)
    else:
        print(f"łatka (0x30), bajtów: {len(changed)}")
        statuses = write_patch(port, changed, new)
    print("statusy:", statuses)
    # Odczyt także po odmowie: wczytuje plik do RAM centrali od nowa,
    # więc obraz zepsuty odrzuconym zapisem znika.
    comm = rc.Config(bytes(new)).sections["COMM"][0]
    try:
        back = read_config_exact(port, bytes(new[comm + 1:comm + 7]))
    except PermissionError:  # zapis odrzucony, a hasło miało się zmienić
        back = read_config_exact(port, code)
    if back == bytes(new):
        print("odczyt po zapisie: zgodny z zamierzonym")
        return True
    if back == bytes(old):
        print("odczyt po zapisie: konfiguracja BEZ ZMIAN (zapis odrzucony)")
    else:
        print("odczyt po zapisie: INNY niż zamierzony, różnice:")
        rc.show_differences(new, back)
    return False


def outputs_mask(port):
    """Maska włączonych wyjść 1–32 ze stanu (0x24, przesunięcie 0x30)."""
    send(port, bytes([0x24]))
    data = receive(port)[:STATUS_SIZE]
    if wait_for(port, {CRC_REQ}, 1.5) is None:
        receive(port)  # opcjonalna odpowiedź modemu
    return int.from_bytes(data[0x30:0x34], "little")


def report_outputs(before, after):
    """Zapis konfiguracji przepisuje stan wyjść z ustawień,
    a programu LogicProcessora nie uruchamia od nowa:
    wyjście ustawione przez program przy starcie (albo SMS-em)
    może zostać wyłączone (docs/programowanie.md).
    """
    if before == after:
        print("wyjścia bez zmian")
        return
    off = bits(before & ~after, 32)
    on = bits(after & ~before, 32)
    print("UWAGA: zmieniły się wyjścia po zapisie;"
          + (f" wyłączone: {off}" if off else "")
          + (f" włączone: {on}" if on else ""))
    print("  sprawdź, czy trzeba je przywrócić (panel, SMS, restart centrali)")


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
    flags = int.from_bytes(data[0xCE:0xD0], "little")
    print("sesja serwisowa przed tym odczytem:", "tak" if flags & 2 else "nie")


def change_config(port, cmd, password, args):
    """zmien / przywroc: argumenty sprawdzone, zanim ruszymy centralę."""
    code = parse_password(password)
    if cmd == "zmien":
        changes = rc.parse_changes(args)
    else:
        with open(args[0], "rb") as f:
            restored = f.read()
        if (len(restored) != rc.CONFIG_SIZE
                or not rc.Config(restored).crc_ok):
            raise ValueError(f"{args[0]}: to nie jest poprawna konfiguracja"
                             " w znanym formacie")
    try:
        old = read_config_exact(port, code)
        print("kopia obecnej konfiguracji:", save_backup(old))
        if cmd == "zmien":
            new = rc.apply_changes(old, changes)
        else:
            new = restored
            # Nagłówek (wersja Managera) może się różnić, katalog sekcji nie.
            if (new[0x0E:rc.DATA_START] != old[0x0E:rc.DATA_START]
                    or new[rc.NSECTIONS_OFFSET] != old[rc.NSECTIONS_OFFSET]):
                raise ValueError("inny katalog sekcji niż w centrali")
        outputs = outputs_mask(port)
        ok = program(port, code, old, new, full=(cmd == "przywroc"))
        report_outputs(outputs, outputs_mask(port))
        return ok
    finally:
        end_session(port)


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
        data = strip_padding(read_config(port, parse_password(sys.argv[3])))
        with open(sys.argv[4], "wb") as f:
            f.write(data)
        print(f"zapisano {len(data)} B")
    elif cmd == "dziennik" and len(sys.argv) in (3, 4):
        count, records = read_log(port)
        print(f"zdarzeń w dzienniku: {count}, odczytano {len(records)}")
        if len(sys.argv) == 4:
            with open(sys.argv[3], "wb") as f:
                f.write(b"".join(records))
        for r in records[-20:]:
            print(r.hex(" "))
    elif cmd == "raport":
        send(port, bytes([0x4C]))
        text = strip_padding(receive(port)).rstrip(b"\0")
        print(text.decode("cp1250", "replace"))
    elif cmd == "sms" and len(sys.argv) >= 5:
        code = sys.argv[3]
        if code.startswith("plik:"):
            with open(code[5:]) as f:
                code = f.read().strip()
        command = " ".join(sys.argv[4:])
        if (command.split()[0] not in SMS_READ_ONLY
                and os.environ.get("ROPAM_STERUJ") != "1"):
            sys.exit(f"„{command.split()[0]}” nie jest na liście poleceń "
                     "odczytujących; ROPAM_STERUJ=1, jeśli na pewno")
        print(sms_command(port, code, command))
    elif cmd == "wyslijsms" and len(sys.argv) >= 4:
        if os.environ.get("ROPAM_STERUJ") != "1":
            sys.exit("wyslijsms wysyła prawdziwy SMS; ROPAM_STERUJ=1, "
                     "jeśli na pewno")
        text = " ".join(sys.argv[3:])
        if not text.isascii() or len(text.partition(";")[2] or text) > 160:
            sys.exit("SMS: tylko ASCII, treść do 160 znaków")
        text = text.encode("ascii")
        # svc_dispatch, case 0x13 → FUN_9d02109c: „NUMER;treść” albo sama
        # treść (wtedy numer z początku sekcji TEL); do kolejki modemu
        # i czekanie na wynik. Status 1 ma znaczyć „wysłany”,
        # ale zdarzył się 0 przy SMS-ie, który doszedł.
        send(port, bytes([0x13]) + text + b"\0")
        status = int.from_bytes(receive(port, timeout=120)[:4], "little")
        print(f"status {status} (1 = wysłany; 0 bywa mimo doręczenia)")
    elif cmd in ("zmien", "przywroc") and len(sys.argv) >= 5:
        sys.exit(0 if change_config(port, cmd, sys.argv[3], sys.argv[4:])
                 else 1)
    elif cmd == "koniec":
        end_session(port)
        print("sesja serwisowa zakończona")
        return
    else:
        sys.exit(__doc__)
    if cmd != "nasluch" and os.environ.get("ROPAM_ZOSTAW_SESJE") != "1":
        end_session(port)


if __name__ == "__main__":
    try:
        main()
    except (PermissionError, ValueError, RuntimeError, TimeoutError) as e:
        sys.exit(f"błąd: {e}")
