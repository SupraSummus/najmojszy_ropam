# Protokół serwisowy

Protokół, którym OptimaGSM Manager programuje centralę.
Odtworzony z firmware'u OptimaGSM v3.4
i sprawdzony na sprzęcie, także przez podsłuch oryginalnego Managera.
Oznaczenia: ✅ wynika wprost z kodu, 🔌 sprawdzone na sprzęcie,
❓ wniosek albo przypuszczenie.

Klient: [`tools/ropam_serwis.py`](../tools/ropam_serwis.py).

## Warstwa fizyczna

- UART4 mikrokontrolera, w kodzie „port 2” ✅,
  najpewniej wyprowadzony na gniazdo RJ12 `RS232TTL` ❓.
- 115200 b/s ✅, 8N1 ❓,
  poziomy logiczne 3,3 V w obu kierunkach
  (zmierzone: TX centrali ok. 3,2 V, RX przy wpiętym kablu USB-MGSM ok. 3,4 V) ✅.

## Kto zaczyna

Osobne zadanie RTOS (`0x9d02b640`)
po starcie odczekuje chwilę,
a potem w nieskończonej pętli woła dyspozytor `FUN_9d03c3f0` ✅.
Dyspozytor próbuje odebrać pakiet komendy przez XMODEM,
więc centrala co 2 sekundy wysyła na port bajt `'C'` (0x43),
po 16 próbach wysyła 3 × CAN i zaczyna od nowa ✅.

Po podłączeniu kabla powinno być widać strumień `C` co 2 sekundy.
To najprostszy test, że jesteśmy na właściwym porcie i z właściwą prędkością.

**Potwierdzone na sprzęcie** (kabel USB-MGSM, FTDI, 115200 b/s, sam odbiór):
centrala w normalnej pracy nadaje `0x43` co 2 sekundy.
Potwierdza to port (UART4 na gnieździe RJ12), prędkość i działanie zadania serwisowego.

### Objaw: centrala słyszy własne echo

Jeśli RX centrali dostaje z powrotem to, co nadaje jej TX,
zamiast `C` co 2 s na linii jest ok. 950 B/s w powtarzającym się cyklu 21 bajtów:

```
43 ×16, 18 18 18, 43, 06
```

Odbiornik XMODEM (`xmodem_receive`) wyjaśnia go co do bajtu ✅:

1. centrala wysyła `C` i od razu odbiera echo `C`,
   którego nie zna, więc liczy próbę jako nieudaną;
2. po 16 próbach wysyła `CAN CAN CAN` i kończy odbiór;
3. następny odbiór zaczyna od `C`,
   a odbiera echo `CAN CAN`, czyli przerwanie od nadawcy,
   na które odpowiada `ACK`.

Tempo (bajt na ok. 1 ms) wyznacza takt systemu,
z którym `svc_getc` sprawdza bufor.
Taki stan nie włącza flagi sesji serwisowej,
bo `svc_dispatch` nie dostaje żadnego poprawnego pakietu ✅,
a z bajtów `C`, `CAN` i `ACK` pakiet komendy nie powstanie.
Centrala pracuje więc normalnie,
ale żadna komenda z zewnątrz do niej nie dotrze.

Spotkaliśmy go, gdy kabel USB-MGSM był wpięty w gniazdo,
ale odłączony od komputera:
niezasilony FT232BL oddawał na RX to, co nadawał TX centrali.
Po podłączeniu kabla do USB echo ustało ✅.

## Transport: XMODEM

Implementacja odpowiada popularnemu `xmodem.c` Georges'a Menie,
z jednym niestandardowym rozszerzeniem ✅.

| Bajt | Znaczenie |
|---|---|
| `0x01` SOH | blok 128 B |
| `0x02` STX | blok 1024 B |
| `0x19` | blok 64 B (rozszerzenie Ropamu) |
| `0x04` EOT | koniec transmisji |
| `0x06` ACK | potwierdzenie |
| `0x15` NAK | błąd, powtórz |
| `0x18` CAN | przerwanie (dwa pod rząd) |
| `'C'` | odbiorca chce sum CRC-16 |

Blok: nagłówek, numer bloku (od 1), dopełnienie numeru do 0xFF, dane,
CRC-16/XMODEM (wielomian 0x1021, start 0, starszy bajt pierwszy) ✅.
W trybie bez `'C'` zamiast CRC jest 8-bitowa suma bajtów ✅.

Centrala jako nadawca dobiera rozmiar bloku według długości całej transmisji ✅:
do 64 B bloki 64 B (`0x19`),
do 1023 B bloki 128 B (SOH),
powyżej bloki 1024 B (STX).
Dopełnienie: jeden bajt `0x1A`, dalej zera ✅.
XMODEM nie przenosi długości, więc odbiorca dostaje dane z dopełnieniem.

## Sesja

1. Centrala nadaje `'C'`.
2. Komputer wysyła **pakiet komendy** XMODEM-em (do 255 B, w praktyce jeden blok).
   Pierwszy bajt to kod komendy, dalej argumenty.
3. Zależnie od komendy centrala:
   odsyła dane XMODEM-em (komputer zaczyna od `'C'`),
   odbiera dane XMODEM-em (centrala zaczyna od `'C'`),
   albo odsyła status.
4. **Status** to 4-bajtowa liczba little-endian wysłana XMODEM-em ✅
   (`FUN_9d03c2dc`), zwykle 1 = sukces, 0 = błąd ❓.

## Komendy

Kody `0x32`–`0x45` obsługuje tablica wskaźników w RAM,
aktywna tylko przy ustawionej fladze ❓ (być może tryb ON-LINE).
Pozostałe są w `switch` w `FUN_9d03c3f0`.

| Kod | Działanie | Pewność |
|---|---|---|
| `0x01` | identyfikacja: 12 B, opis niżej; zapisuje w dzienniku zdarzenie programowania | ✅ sprawdzone na sprzęcie |
| `0x02` | zapis konfiguracji: blob 52 412 B (v3.4) z CRC-16/XMODEM, zapis do pliku `0:config.rpm` i kopii `0:cfgcpy.rpm`; odpowiedź: status | ✅ |
| `0x03` | odczyt konfiguracji: argument to 6-bajtowe „hasło komunikacji” (sekcja `COMM`); odpowiedź to dwie transmisje: status (1 = OK, 2 = złe hasło), a przy sukcesie konfiguracja (52 412 B, bloki 1024 B) | 🔌 |
| `0x24` | stan centrali na żywo, 1100 B, opis w [`stan-centrali.md`](stan-centrali.md); Manager wysyła ją co ok. 0,5 s, póki jest połączony | 🔌 |
| `0x26`, `0x2F` | status, a potem **restart centrali** (`SoftReset`); Manager wysyła `0x2F` przy rozłączeniu | ✅ 🔌 |
| `0x07` | odczyt bloku 198 B (`0xC6`) | ✅ |
| `0x08` | odczyt tablicy modułów magistrali, 198 B | ❓ |
| `0x12` | wpisanie PIN-u karty SIM do modemu (`AT+CPIN`), opcja „WPISZ KOD PIN” z instrukcji | ✅ |
| `0x4C` | raport diagnostyczny tekstem (m.in. statystyki ramek RopamNET) | ✅ |
| `0x04`, `0x05`, `0x0D`, `0x0F`–`0x23`, `0x27`–`0x2E`, `0x30`, `0x31`, `0x4A`, `0x4B` | do rozpracowania | |

## Jak pracuje OptimaGSM Manager

Podsłuchane na sprzęcie (Manager 2.5 pod Wine, [`tools/wine_sniff.sh`](../tools/wine_sniff.sh)) 🔌:

1. **Połączenie kablowe:** `0x01` (identyfikacja),
   potem `0x24` w pętli co ok. 0,5 s, póki połączenie trwa.
2. **Odczyt:** `0x03` z hasłem komunikacji,
   centrala odsyła status, a potem całą konfigurację.
   Odpytywanie `0x24` trwa dalej.
3. **Rozłączenie:** `0x2F`, które restartuje centralę.

Manager nie pyta o hasło komunikacji,
bo bierze je z otwartego pliku `.rpm` (pole „Hasło komunikacji” w zakładce komunikacji).
Plik `.rpm` jest zaszyfrowany kluczem wbudowanym w Managera:
Manager otwiera go bez pytania o hasło.

Pakiety komend Managera są identyczne z tymi z naszego klienta
(jeden blok 64 B z nagłówkiem `0x19`, dopełnienie `0x1A` i zera).
Odczyt konfiguracji przez `tools/ropam_serwis.py`
daje plik identyczny bajt w bajt z odczytem Managera 🔌.

Każda komenda (także z naszego klienta) włącza w centrali sesję serwisową na ok. 1,5 minuty,
zob. [`stan-centrali.md`](stan-centrali.md).
Nasz klient nie wysyła `0x24` w pętli ani nie wysyła `0x2F`, więc nie restartuje centrali.

### Odpowiedź na `0x01`

| Bajt | Znaczenie |
|---|---|
| 0 | `0x00` |
| 1 | wersja firmware'u × 10 (np. `0x22` = 3.4); w kodzie stała `FW_VERSION` |
| 2 | wersja × 10 z końca RAM-u, najpewniej bootloadera ❓ |
| 3 | `0x14`, znaczenie nieznane |
| 4–11 | unikalne ID centrali (8 B) |

Odpowiedź przychodzi jednym blokiem 64 B (nagłówek `0x19`) z dopełnieniem.

## Konfiguracja w centrali (`config.rpm`)

Blob o stałym rozmiarze, zaszytym w firmware'ze (`CONFIG_SIZE`, 52 412 B w v3.4) ✅,
z CRC zgodnym z opisem poniżej 🔌:

| Przesunięcie | Zawartość |
|---|---|
| `0x0000` | nagłówek 14 B; bajt 4 to wersja formatu, bajt 5 wersja z `g_boot_version` |
| `0x000E` | katalog: 28 wpisów po 14 B (nazwa 8 B, przesunięcie u16, rozmiar u16, 2 B ❓) |
| `0x0196` | dane sekcji; każda kolejna sekcja zaczyna się 8 B za końcem poprzedniej |
| `0xCCB6` | liczba sekcji |
| `0xCCB8` | CRC-16/XMODEM (wyliczane z pierwszych `CONFIG_SIZE − 4` bajtów) |

Sekcje mają nazwy tekstowe i lądują w osobnych strukturach w RAM
(`cfg_load_all`, `cfg_store_all`).
Nazwy w v3.4:
`INPUT`, `OUT`, `TEST`, `COUNT`, `TIMES`, `STREFY`, `SMTP`, `COMM`, `GPRSCFG`,
`TIMR`, `TEL`, `EMAIL`, `ANINP`, `M_RF4`, `APCFG`, `VAR1`, `WIFI`, `LOGTXT` i kilka krótszych.
Sekcja `COMM` (33 B) zawiera m.in. bajt flag i hasło komunikacji (bajty 1–6) ✅.

## Kopia konfiguracji z OptimaGSM Managera (`*.rpm`)

To inny format niż blob w centrali:
zaczyna się od `"Optim\0"` i 8-bajtowego ID centrali,
a reszta jest zaszyfrowana porządnym szyfrem
(entropia ≈ 8 bitów na bajt, brak okresowości),
więc nie jest to prosty XOR jak w firmware'ze.
Plik jest o ok. 5,5 KB większy od bloba z centrali ❓ (np. dodatkowe dane Managera).
Do rozpracowania w kodzie Managera.

## Tryb zdalny

Gdy w konfiguracji ustawiony jest odpowiedni bit,
ta sama obsługa komend działa przez modem (port 3, GPRS),
ale zamiast XMODEM-a używa ramki `'S'`, 2 bajty długości, dane, suma ✅
(`FUN_9d04f1a4`, `FUN_9d04f3bc`).
Przy innym bicie dane przechodzą przez `FUN_9d062524`
z 16-bajtowym kluczem z konfiguracji ❓ (szyfrowanie kluczem TCP/IP?).
Do rozpracowania.
