# ESP32

Programy dla Seeed XIAO ESP32-C6 pisane na [rvuos](https://github.com/SupraSummus/rvuos),
mikrojądrze z przestrzenią użytkownika chronioną przez PMP.
rvuos jest submodułem w `third_party/rvuos`.
Zmiany, których potrzebujemy w nim samym, są łatkami w `rvuos/*.patch`;
skrypt budujący nakłada je sam.

Budowanie i wgrywanie (potrzebne: clang z obsługą RISC-V, lld, llvm-objcopy, esptool):

```sh
esp32/rvuos/zbuduj.sh ropam-most wgraj   # buduje i wgrywa na /dev/ttyACM0
```

## Bramka: strona ze stanem centrali przez Wi-Fi

`zbuduj.sh bramka wgraj` buduje system Wi-Fi rvuos z dwoma dodatkami
(łatki `rvuos-net-tcp.patch` i `rvuos-wifi-ropam.patch`):

- klient magistrali `ropam` (`szyna-klient.c` nad `szyna.c`),
  osobny proces z UART0, który tylko słucha RopamNET
  (połączenia jak przy podsłuchu, niżej)
  i publikuje w pamięci dzielonej z procesem sieciowym
  ostatnie rozgłoszenie stanu (`0x10`) i ostatni raport panelu (`0x11`);
- serwer HTTP na porcie 80 w procesie sieciowym (`tcp.c`, `strona.c`):
  `/` to strona z zegarem centrali, strefami, wejściami, wyjściami,
  zasilaniem panelu i jego klawiszem funkcyjnym (`fn`),
  `/stan.json` to samo dla programów.
  Tylko podgląd, bez żadnych poleceń.

Stan jest świeży co ok. 0,56 s, a centrala nie wchodzi w sesję serwisową 🔌.
Bramka nie zmienia liczników straconych ramek
w raporcie diagnostycznym centrali 🔌.

Sieć bierze z `private/wifi.conf` (poza gitem), linie `ssid=` i `pass=`;
skrypt dopisuje `run=0`, żeby system działał bez końca.
`antenna=ufl` przełącza XIAO na złącze anteny zewnętrznej.
Adres IP widać w logu startu, który skrypt pokazuje przez minutę.

Jak to jest ułożone:

- Proces główny zaraz po starcie, zanim system zacznie łączyć się z siecią,
  ustawia RE/DE (D8) nisko i DI (D9) wysoko,
  mierzy poziom spoczynkowy RO
  i w razie potrzeby odwraca wejście U0RXD w matrycy GPIO
  (w logu startu: `root: ropam bus RX inverted` albo `straight`).
  Proces `ropam` tego zrobić nie może,
  bo z kodem, danymi, gniazdem, UART0, licznikiem czasu (`BOOT_CAP_COUNTER`)
  i obszarem migawki ma już wszystkie osiem regionów, jakie może mieć proces.
- Proces `ropam` na bajty czeka budzikiem po 2 ms
  (kolejka UART0 mieści ok. 11 ms magistrali),
  a czas liczy z licznika, nie z liczby drzemek.
  Migawkę publikuje po każdej ramce stanu albo panelu, a poza tym co sekundę,
  żeby liczniki (`bajty`, `ramki`, `stany`, `smieci`) było widać,
  nawet gdy nic się nie składa w ramki.
- Klient włącza filtr odbioru UART-u (impulsy krótsze niż ok. 1,6 µs):
  przy podłączeniu do portu serwisowego radio XIAO wprowadzało
  na linię odbioru krótkie zakłócenia 🔌.
  Za transceiverem RS-485 bajty śmieci jak dotąd się nie pojawiają 🔌.
- Obszar na dane procesów potomnych (`ROOM_SIZE`) jest powiększony do 64 KiB:
  proces sieciowy z TCP ma 32 KiB, `ropam` 16 KiB, echo i zegar po 8 KiB.

Wcześniejsza wersja czytała stan przez port serwisowy
(`ropam-klient.c`, w historii gita, nad `ropam.c`:
komenda `0x24` co 5 s z zamknięciem sesji).
Sam klient protokołu serwisowego (`ropam.c`) został dla `ropam-stan.c` i testów.

Testy na komputerze: `esp32/rvuos/test/tcp-test.c`, `strona-test.c`,
`szyna-test.c` i `ropam-test.c`.
Polecenia kompilacji są na ich początku.

## `rvuos/ropam-podsluch.c`: bierny podsłuch magistrali RopamNET

XIAO z transceiverem RS-485 słucha magistrali,
dzieli strumień na ramki, potwierdzenia i śmieci
i każdą pozycję wypisuje na konsolę USB jako linię 🔌:

```
3.495700 A 1: 03
3.495901 R 15: aa 11 0a 00 fa 00 fa b1 04 00 1a 03 00 63 10
```

Kolejno: czas pierwszego bajtu od startu, rodzaj, liczba bajtów,
błędy UART-u i bajty.
Rodzaj to `R` (ramka z poprawnym CRC), `A` (bajt potwierdzenia) albo `?` (śmieci).
Błędy: `!R` błąd ramki, `!Z` zakłócenie, `!P` przepełniona kolejka.
Co 10 s linia `#` z licznikami.

Ramek nie da się oddzielać ciszą:
idą jedna za drugą razem z potwierdzeniami, często bez przerwy 🔌
(zob. [`docs/protokol-ramek.md`](../docs/protokol-ramek.md)).
Parser (`szyna.c`) robi więc to samo co `parse_stream` w `tools/ropamnet.py`:
od każdej pozycji próbuje ramki o długości z trzeciego bajtu i sprawdza CRC.
Na resztę ramki czeka do 3 s ciszy.

Program **niczego nie nadaje**:
RE/DE trzyma stale w stanie niskim, DI w wysokim,
a do UART-u nigdy nie pisze.
Centrala co ok. 0,56 s rozgłasza w ramce `0x10` cały nagłówek stanu,
ten sam co w odpowiedzi na komendę serwisową `0x24` 🔌,
więc podsłuch daje stan bez sesji serwisowej i bez trybu ON-LINE.

```sh
esp32/rvuos/zbuduj.sh ropam-podsluch wgraj
stty -F /dev/ttyACM0 raw -echo
cat /dev/ttyACM0 | tee szyna.log | python3 -u tools/ropamnet.py log - [KONFIGURACJA]
```

`ropamnet.py log` opisuje ramki (adres, komenda, kierunek),
a z plikiem konfiguracji dekoduje rozgłaszany stan z nazwami wejść i stref.
Zapis może zawierać kody użytkowników wpisywane na panelach:
nie trafia do repo bez przejrzenia (zob. `CLAUDE.md`).

Polaryzację program wykrywa sam (`rs485.c`):
przy starcie przez ok. 0,2 s mierzy poziom spoczynkowy RO,
a gdy linia jest głównie w stanie niskim, odwraca wejście U0RXD w matrycy GPIO.
Napisy A i B na modułach z transceiverem bywają odwrotne niż na centrali 🔌,
więc „zamienione A i B” to tu zwykła sytuacja, a nie usterka.
Jeśli przez 10 s są same błędy ramki, program próbuje drugiej polaryzacji.

Polecenia z komputera (znaki wysłane na `/dev/ttyACM0`), oba tylko do odbioru:
`i` odwraca wejście RX,
a `p` przez 2 s mierzy na RO najkrótsze impulsy i poziom spoczynkowy
(czas bitu przy 115200 b/s to 8,68 µs).

Konsola wysyła pełne pakiety USB po 64 B
(drobne, wysyłane po każdej pozycji, nie nadążały za magistralą)
i ponawia wysłanie, gdy pakiet utknął przed otwarciem portu przez komputer;
bez tego konsola potrafiła zamilknąć na dobre.

Logika ramek i CRC jest w `szyna.c`, test na komputerze w `test/szyna-test.c`.

### Połączenia z transceiverem

| Transceiver | XIAO ESP32-C6 |
|---|---|
| RO | **D7** (GPIO17, U0RXD) |
| RE i DE (razem) | **D8** (GPIO19), stale niski: odbiór |
| DI | **D9** (GPIO20), stale wysoki |
| VCC | **3V3** |
| GND | GND, wspólna z `GND` magistrali |
| A, B | `A`, `B` magistrali centrali |

Przed podłączeniem do działającej magistrali:

- **Zasilanie 3,3 V.**
  Popularne moduły z MAX485 są opisane na 5 V;
  zasilone z 5 V dają na RO 5 V, a ESP32-C6 ich nie toleruje.
  MAX3485 (albo inny układ na 3,3 V) jest właściwy;
  MAX485 przy 3,3 V zwykle działa, ale poza katalogiem ❓.
- **Terminator.**
  Takie moduły mają zwykle na płytce opornik 120 Ω między A i B.
  Magistrala ma już terminację na końcach (zworka `JT1` w centrali),
  a trzeci terminator w środku linii obciąża nadajniki centrali i modułów
  ponad normę EIA-485.
  Jeśli podsłuch nie jest na końcu linii, opornik trzeba wylutować.
- **RE/DE od resetu do startu programu.**
  W tym czasie (rozruch ROM-u i jądra) D8 nie jest sterowany
  i może być podciągany do góry ❓,
  a wysoki DE włącza nadajnik, który zagłusza magistralę.
  Do podsłuchu najpewniej połączyć RE/DE wprost z masą, bez D8;
  jeśli ma zostać na D8, to z opornikiem ok. 10 kΩ do masy.
- Zamiana A z B niczego nie uszkodzi, a program sam ją wykrywa (wyżej).

Linia startowa podaje odczytane poziomy D8, D9 i D7;
D8 musi mieć 0.

## `rvuos/ropam-stan.c`: odczyt stanu na konsolę USB

Pierwszy krok bramki: XIAO sam czyta stan centrali co 10 s i wypisuje go na konsolę USB.

## `rvuos/ropam-most.c`: most USB ↔ gniazdo serwisowe

XIAO udaje kabel USB-MGSM:
bajty z portu USB (`/dev/ttyACM0`) idą bez zmian na UART0 i z powrotem.
Klient działa z nim tak jak z kablem:

```sh
python3 tools/ropam_serwis.py /dev/ttyACM0 id
```

UART0 zostaje taki, jak ustawił go ROM układu: 115200 8N1.
Wymaga łatki `rvuos/rvuos-uart0.patch`,
która dopisuje rejestry UART0 do urządzeń przydzielanych procesom.

Sprawdzone na płytce: program startuje,
a zwykłe otwarcie portu (DTR i RTS włączone razem) nie restartuje układu.
Ręczne wyłączenie DTR i RTS przed otwarciem portu restartuje układ.

Sprawdzone z centralą (D7 równolegle z kablem USB-MGSM):
XIAO odbiera to samo co kabel, bajt w bajt,
zarówno `C` w spoczynku, jak i całą odpowiedź XMODEM na komendę `id`.
Można więc go używać jako podsłuchu sesji Managera.
Sprawdzone w obie strony (RX centrali na D8, kabel USB-MGSM w bit-bang):
`ropam_serwis.py /dev/ttyACM0 id` i `stan` działają jak przez kabel,
także transfer 1100 B w blokach po 1024 B.
Działa też z kablem wpiętym w gniazdo, ale odłączonym od komputera:
spoczynek nadajnika XIAO za 1 kΩ przeważa echo niezasilonego FT232BL.

Pierwsza wersja budziła się okresowym budzikiem rvuos co 500 µs
i po 10–20 minutach od startu zaczynała gubić bajty w obu kierunkach
(nasze ACK docierały do centrali za późno, jej bloki przychodziły rwane);
pomagał tylko restart XIAO.
Teraz pętla odpytuje oba FIFO bez przerwy, bez budzika;
przyczyna w rvuos (budzik po dłuższym czasie od startu ❓) do zbadania.

Po drodze wyszły dwie usterki montażu, warte zapamiętania:

1. kabel USB-MGSM wpięty w gniazdo, ale odłączony od komputera:
   centrala słyszała własne echo
   ([`docs/protokol-serwisowy.md`](../docs/protokol-serwisowy.md#objaw-centrala-słyszy-własne-echo)),
   które ustało po podłączeniu kabla do USB;
2. RX centrali podłączony do D8 zamiast do D6:
   `ropam-d6in` pokazał D6 wiszący w powietrzu.
   Most wyprowadza teraz TX na obie nóżki.

### Połączenia

Gniazdo RJ12 centrali ma trzy używane żyły: masę, nadawanie (TX) i odbiór (RX) centrali.
Procesor centrali pracuje na 3,3 V;
ESP32-C6 nie toleruje 5 V.
Kabel USB-MGSM (FT232BL) nadaje do centrali w logice 3,3 V:
na RX centrali w spoczynku jest ok. 3,4 V.
Dlatego D6 przez 1 kΩ może zostać wpięty nawet razem z kablem;
przy kolizji nadajników płynie najwyżej ok. 3,4 mA.
Nadawać przez XIAO da się jednak tylko bez kabla:
jego nadajnik trzyma linię RX centrali mocniej niż D6 za opornikiem.
TX centrali też pracuje w logice 3,3 V (w spoczynku ok. 3,2 V),
więc D7 łączymy przez sam opornik 1–10 kΩ, bez dzielnika.
Dzielnik 10 kΩ / 20 kΩ dawałby ok. 2,2 V,
czyli mniej niż próg stanu wysokiego ESP32-C6 (0,75 × 3,3 V ≈ 2,5 V).

| Gniazdo RJ12 | XIAO ESP32-C6 |
|---|---|
| GND | GND |
| TX centrali | **D7** (GPIO17, odbiór) przez 1–10 kΩ |
| RX centrali | **D6** (GPIO16, nadawanie) albo **D8** (GPIO19, ten sam sygnał przez matrycę GPIO) przez 1 kΩ |

TX centrali rozpoznasz po tym, że w spoczynku nadaje `C` co 2 s:
z wgranym mostem widać je w `picocom /dev/ttyACM0`.
Zamiana żył niczego nie uszkodzi przy rezystorach 1 kΩ, po prostu nie będzie danych.

Przy każdym restarcie XIAO ROM układu wypisuje na D6 krótki tekst startowy,
który trafia do centrali;
nie ma w nim bajtów nagłówków XMODEM ani CAN, więc centrala go pomija.

Każda komenda serwisowa włącza w centrali tryb ON-LINE na ok. 1,5 min
([`docs/stan-centrali.md`](../docs/stan-centrali.md)).

## Diagnostyka

`rvuos/ropam-piny.c` wypisuje, jak ROM zostawił GPIO16 i GPIO17
(IO MUX, matryca GPIO, UART0),
i przez ok. 50 ms liczy zmiany stanu obu wyprowadzeń.
Niczego nie zmienia ani nie nadaje; po nim trzeba wgrać z powrotem most.

```sh
esp32/rvuos/zbuduj.sh ropam-piny wgraj
esp32/rvuos/zbuduj.sh ropam-most wgraj
```

Po starcie z flasha obie nóżki mają w IO MUX funkcję 0,
czyli bezpośrednio U0TXD i U0RXD, z podciąganiem.

`rvuos/ropam-d6.c` przekazuje komputerowi to, co nadaje centrala,
i przez 2 s (między znacznikami `#D6=0` i `#D6=1`) trzyma D6 w stanie niskim.
Jeśli D6 dochodzi do RX centrali, w tej fazie zmienia się jej odpowiedź
(echo znika, centrala odbiera bajty 0x00 z błędem ramki i je pomija).
Wynik najprościej obejrzeć w wyjściu narzędzia wgrywającego:

```sh
esp32/rvuos/zbuduj.sh ropam-d6 wgraj
```

`rvuos/ropam-d6in.c` robi z D6 wejście i przez 2 × 3 s
(podciąganie w dół, potem w górę) liczy jego stany niskie i zmiany.
Nóżka wisząca w powietrzu jest niska w pierwszej fazie, wysoka w drugiej
i w żadnej się nie zmienia.
Sygnał na żyle RX centrali najprościej dać z kabla USB-MGSM w trybie bit-bang
(nóżka TXD jako wyjście, `set_bitmode(0x01, BITMODE_BITBANG)` w libftdi);
centrala odbiera wtedy śmieci, z których nie złoży poprawnego pakietu.

Gdy centrala słyszy własne echo
(`ropam_serwis.py` mówi wtedy „centrala nadaje bez przerwy”),
po kolei:

1. kabel USB-MGSM musi być odłączony od żył gniazda, nie tylko od komputera:
   niezasilony FT232BL zasila się przez diody wejścia z TX centrali
   i może oddawać sygnał na RX ❓;
   jeśli ma zostać wpięty, podłącz go do USB
   i odłącz jego nadajnik: `python3 tools/ftdi_odlacz.py`
   (tryb bit-bang, wszystkie nóżki jako wejścia;
   do zwykłej pracy kabel wraca po ponownym podłączeniu USB);
2. miernikiem: ciągłość od wolnego końca opornika D6 do żyły RX centrali;
3. przy wszystkim odłączonym: między żyłami TX i RX nie może być zwarcia;
4. przy wgranym moście i odłączonym kablu
   na żyle RX centrali powinno być stałe ok. 3,3 V (spoczynek D6).

## Dalej

- Otwieranie furtki z bramki: najpierw ustalić, gdzie jest wyjście zamka
  (np. pełny odczyt `0x24` w czasie otwarcia),
  potem bezpieczna droga sterowania (zob. `docs/protokol-ramek.md`, stan panelu).
- Bramka wykrywa polaryzację tylko przy starcie;
  brak jej drugiej próby po samych błędach ramki, jak w podsłuchu
  (proces bramki nie widzi błędów UART-u ani matrycy GPIO).
- Most przez sieć do narzędzi z komputera (TCP już jest, `tcp.c`).
