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

### Odporność na zakłócenia

Na długim kablu albo obok radia (np. ESP32 z włączonym Wi-Fi)
zdarzają się przekłamane, zgubione i dołożone bajty 🔌.
Żeby odbiorca kosztował wtedy jedno ponowienie bloku, a nie cały odczyt:

- Po złym bloku czekamy na ciszę na linii (ok. 50 ms), dopiero potem NAK.
  Gdy zakłócenie dołożyło bajty, blok kończy się wcześniej,
  a jego ogon jest jeszcze w drodze;
  bez czekania bajty danych `0x18` i `0x04` z ogona
  odbiorca bierze za CAN i EOT.
- Przerwanie to dwa CAN z rzędu; pojedynczy pomijamy.
- Gdy po pierwszym bloku komendy centrala odpowiada nowym `'C'` zamiast ACK,
  bloku nie przyjęła: wysyłamy go od razu jeszcze raz.

Test z udawaną centralą (`esp32/rvuos/test/ropam-test.c`)
sprawdza te przypadki.

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
| `0x2F` | status, a potem **restart centrali** (`sys_restart`, `SoftReset`); Manager wysyła `0x2F` przy rozłączeniu | ✅ 🔌 |
| `0x04` | odczyt dziennika zdarzeń: liczba rekordów, potem paczki po 100 rekordów po 16 B; argument 1 = cały, inny = ostatnie 1000; opis w [`zdarzenia.md`](zdarzenia.md) | ✅ 🔌 |
| `0x05` | **kasuje dziennik zdarzeń** i uruchamia test: wyłącza wyjścia 1–8, potem co 5 s włącza je po kolei i zostawia włączone; nie wysyłać | ✅ |
| `0x07` | **wyszukanie modułów**: zadanie magistrali nr 4 zeruje adresy we wszystkich gniazdach i odpytuje magistralę od nowa; odpowiedź to nowa tablica gniazd (198 B); zmienia stan, nie wysyłać bez potrzeby | ✅ |
| `0x08` | zadanie magistrali nr 14 (ponowne sprawdzenie gniazd modułów ❓), potem tablica gniazd (198 B); zmienia stan | ✅ |
| `0x0D` | ustawienie zegara: 8 B daty i czasu (`rtc_set`) | ✅ |
| `0x0F` | najpewniej testowy e-mail ❓: tekst do kolejki z odbiorcą 1, 30 s czekania, status; bez skonfigurowanej sekcji `SMTP` (bajt `0xA2`) od razu status 0 🔌 | ✅ |
| `0x10`, `0x11` | stała komenda AT do modemu; włącza (`0x10`) albo wyłącza (`0x11`) dołączanie odpowiedzi modemu do `0x24` | ✅ |
| `0x12` | wpisanie PIN-u karty SIM do modemu (`AT+CPIN`), opcja „WPISZ KOD PIN” z instrukcji | ✅ |
| `0x13` | **wysłanie SMS-a**, opis niżej | ✅ 🔌 |
| `0x15`, `0x18` | zadania magistrali nr 2 (12 B) i 3 (26 B) z danymi ❓ | |
| `0x19` | polecenie SMS podane kablem; odpowiedź tekstem jak na SMS | ✅ |
| `0x1E` | ❓, potem polecenie dla paneli TPR (zadanie magistrali nr 11) | |
| `0x1F` | **restart centrali** bez statusu, ze znacznikiem `1` pod `0xA001FFF0` (wejście do bootloadera, czyli aktualizacja firmware'u ❓); nie wysyłać | ✅ |
| `0x21` | `FUN_9d061674` ❓ | |
| `0x26`–`0x29` | polecenie dla kanału 1–4 modułu FGR-4 na złączu EXP (zadanie magistrali nr 5: ramka do adresu `0xEE` z bitem kanału i czasem, potem odczyt) | ✅ |
| `0x2A` | zadanie magistrali nr 6 z 32 B danych, status; przy `0xF7` także 6 B ❓ | |
| `0x2B` | zadanie magistrali nr 11 z 32 B danych (polecenie dla paneli TPR ❓) | |
| `0x2C`, `0x2D` | wymuszenie włączenia (`0x2C`) albo wyłączenia (`0x2D`) wyjścia o numerze z bajtu 1 (od 0) | ✅ |
| `0x2E` | komenda do modemu ❓ | |
| `0x30` | zapis wybranych bajtów konfiguracji: trójki (przesunięcie u16, wartość), nowe CRC, dalej jak `0x02` | ✅ |
| `0x31` | włącza albo wyłącza tryb serwisowy (bit 0 `g_sys_flags`, zdarzenie `0x1C`); przy włączeniu wyłącza aktywne wyjścia stref (sygnalizatory ❓); otwiera komendy `0x32`–`0x45` | ✅ |
| `0x32`–`0x45` | tablica `svc_ext_handlers`, tylko w trybie serwisowym albo przy fladze w ustawieniach | ✅ |
| `0x4A` | zadanie magistrali `0x4A` z 1 B, status, potem 1500 B ❓ | |
| `0x4B` | odbiór danych i zadanie magistrali `0x4B` (zapis do modułu ❓) | |
| `0x4C` | raport diagnostyczny tekstem: czas zbierania statystyk, ramki RopamNET (dobre, bez potwierdzenia, zły CRC, odsetek straconych), błędy komunikacji każdego modułu, błędy czujników temperatury | ✅ 🔌 |

Kody nieobecne w dyspozytorze (np. `0x06`, `0x09`–`0x0C`, `0x0E`, `0x14`, `0x16`, `0x17`, `0x1A`–`0x1D`, `0x20`, `0x22`, `0x23`, `0x25`)
nie robią nic poza ustawieniem flagi sesji ✅.
Wbrew wcześniejszemu opisowi `0x26` nie restartuje centrali;
restart robią `0x1F` i `0x2F` (`sys_restart`) ✅.

## Komendy dodatkowe `0x32`–`0x45`

Obsługuje je `svc_dispatch_ext` z tablicy `svc_ext_handlers` (20 wpisów) ✅.
Działają tylko wtedy, gdy włączony jest tryb serwisowy (`0x31 1`)
albo gdy w ustawieniach jest ustawiona flaga pod `0xA0015A46` ❓.
Wygląda to na protokół aplikacji Ropamu ❓:
operacje idą w ponumerowanych sesjach,
a sesję otwiera logowanie kodem użytkownika z uprawnieniem do sterowania zdalnego.

| Kod | Działanie |
|---|---|
| `0x33`, `0x34` | logowanie typu wyzwanie–odpowiedź kodem użytkownika; odpowiedź: uprawnienia do stref 1–4 i nazwa użytkownika; po kilku błędach blokada i SMS ✅ |
| `0x35` | sterowanie wyjściami w sesji ❓ |
| `0x36` | **uzbrojenie stref** w sesji: maska stref, tryb nocny; centrala zawęża ją do uprawnień kodu ✅ |
| `0x37` | rozbrojenie stref w sesji ❓ |
| `0x32`, `0x38`–`0x45` | do rozpracowania |

Szczegóły kryptograficzne logowania na razie pomijamy:
dotyczą słabości opisanej w [`uwagi-bezpieczenstwa.md`](uwagi-bezpieczenstwa.md),
którą najpierw zgłosimy producentowi.

## Wysłanie SMS-a: komenda `0x13`

Treść komendy to napis z końcowym zerem ✅:
`NUMER;treść` (numer zaczyna się od `+` albo cyfry)
albo sama treść,
i wtedy odbiorcą jest numer z początku sekcji konfiguracji `TEL` ✅.
Numer do 20 znaków, treść do 160 ✅.
Centrala odmawia (status 0) bez gotowego modemu
(bit 1 pierwszego bajtu flag systemowych, `0x0B8` w nagłówku stanu) ✅.
Wiadomość idzie do kolejki zadania modemu,
a centrala czeka na wynik kilka do kilkudziesięciu sekund
(czas z sekcji `TIMES`) i odsyła status ✅.
Tę samą funkcję wywołuje żądanie `0x35` panelu TPR (z innym czasem czekania) ✅.

Sprawdzone 🔌: sama treść „test bramki” doszła SMS-em na numer z `TEL`,
choć centrala po ok. 20 s odesłała status 0,
a w dzienniku zdarzeń nie pojawił się żaden wpis o SMS-ie.
Status 0 nie znaczy więc, że SMS nie wyszedł;
sesja serwisowa nie blokuje też wysyłki.

Klient: `ROPAM_STERUJ=1 tools/ropam_serwis.py PORT wyslijsms TEKST`.

## Polecenia SMS przez kabel: komenda `0x19`

Treść komendy to tekst polecenia SMS (z końcowym zerem) ✅ 🔌.
Centrala przetwarza go tak jak SMS (`sms_execute`),
a odpowiedzi, które wysłałaby SMS-em, trafiają do kolejki SMS-ów.
Na czas komendy centrala ustawia znacznik (`0xA0000135`),
przy którym zadanie modemu tej kolejki nie rusza ✅;
komenda zabiera z niej do trzech wiadomości
i odsyła je jedną transmisją XMODEM jako tekst.
Do sieci GSM nic więc nie wychodzi.
(Gdyby w kolejce czekał akurat prawdziwy SMS, np. o alarmie,
komenda też by go zabrała.)
Przy złym kodzie odpowiedzi nie ma.

Przykład odpowiedzi na `stan` (wartości zmyślone, układ prawdziwy) 🔌:

```
26/01/15 12:00
sv:3.4
GSM:2,20dbm
Uz:13.8V
Wejscia:00000000
Wyjscia:xxxxxxx0
Temp.
T1.nazwa:21.5
S1.nazwa:rozbrojony
```

Format: `KKKK polecenie`, czyli czterocyfrowy kod użytkownika, spacja i polecenie ✅
(w wariancie z aplikacji polecenie zaczyna się dopiero od 12. znaku).
Bez ważnego kodu polecenie się nie wykona.
Kod serwisowy (użytkownik 100) nie ma uprawnień do większości poleceń,
a gdy użyty kod to wciąż fabryczne `5555`,
centrala dopisuje do odpowiedzi „Uzywasz hasla fabrycznego, zmien…” ✅.

Polecenia z tablicy w firmware'ze (52, `0x9D05759C`) ✅:
`zal`, `wyl`, `resettest`, `tel`, `dodajtel`, `usuntel`, `stan`, `restart`, `czas`,
`korekta`, `blokuj`, `odblokuj`, `kod`, `doladuj`, `ontp`, `offtp`, `tempmonit`,
`centrum`, `echo`, `ussd`, `downloading`, `email`, `odeslijsms`, `outs`, `tempa`,
`tempb`, `ailo`, `aihi`, `blokujbin`, `hardware`, `rygiel`, `wejscia`, `katalog`,
`zdjecie`, `pobierz`, `onr`, `offr`, `connect`, `gprsstat`, `historia`, `wifi`,
`apipdefault`, `apiprestart`, `lanstat`, `grzanie`, `walkon`, `walkoff`,
`deviceup`, `setapn`, `netstat`, `wrltemp`, `lines`.

Część tylko odczytuje (np. `stan`, `wejscia`, `gprsstat`, `lanstat`, `netstat`, `lines`),
ale już `stan` zapisuje w dzienniku zdarzenie `0x16` („Zapytanie o status systemu”).
Inne sterują systemem (`zal`/`wyl` uzbrajają i rozbrajają, `blokuj`, `outs`, `rygiel`)
albo zmieniają ustawienia (`kod`, `dodajtel`, `czas`).
Dla bramki to najprostsza droga do uzbrajania przez port serwisowy ❓;
wymaga 4-cyfrowego kodu użytkownika z uprawnieniem do sterowania SMS
(kod dłuższy niż 4 cyfry tu nie zadziała).
Sprawdzone na sprzęcie: `stan`.

Klient: `tools/ropam_serwis.py PORT sms KOD POLECENIE`;
bez `ROPAM_STERUJ=1` przepuszcza tylko polecenia odczytujące.

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
