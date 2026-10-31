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

- klient centrali `ropam` (`ropam-klient.c` nad `ropam.c`),
  osobny proces z UART0, który co 5 s czyta stan (komenda `0x24`),
  zamyka sesję serwisową i publikuje migawkę w pamięci dzielonej z procesem sieciowym;
- serwer HTTP na porcie 80 w procesie sieciowym (`tcp.c`, `strona.c`):
  `/` to strona ze strefami, wejściami i wyjściami, `/stan.json` to samo dla programów.
  Tylko podgląd, bez żadnych poleceń.

Sieć bierze z `private/wifi.conf` (poza gitem), linie `ssid=` i `pass=`;
skrypt dopisuje `run=0`, żeby system działał bez końca.
`antenna=ufl` przełącza XIAO na złącze anteny zewnętrznej.
Adres IP widać w logu startu, który skrypt pokazuje przez minutę.

Jak to jest ułożone:

- Proces główny ustawia TX UART0 na D8 zaraz po starcie,
  zanim system zacznie łączyć się z siecią.
  Póki D8 nie nadaje, wejście RX centrali wisi w powietrzu,
  a centrala bierze to za śmieci i potem odrzuca nasze bloki
  (odpowiada `C` i `CAN` zamiast `ACK`).
  Z pinami ustawianymi dopiero w procesie `ropam`,
  ok. 10 s po starcie, odczyty się nie udawały 🔌.
- Proces `ropam` dostaje UART0, licznik czasu (`BOOT_CAP_COUNTER`)
  i obszar migawki, czyli razem z kodem, danymi i gniazdem
  wszystkie osiem regionów, jakie może mieć proces.
  Na bajty czeka budzikiem po 2 ms,
  a czas liczy z licznika, nie z liczby drzemek.
- Sesję serwisową kończy zaraz po odebraniu stanu,
  na pierwszym zaproszeniu `C` po transmisji.
  Centrala jest w trybie ON-LINE ok. 0,26 s na każdy odczyt 🔌,
  czyli ok. 5% czasu przy odczycie co 5 s (`ROPAM_EVERY_MS` w `bramka.h`).
  W tym czasie pomija m.in. synchronizację wyjść z koncentratorem
  i nadrabia ją w następnym przebiegu odpytywania
  (szczegóły w [`docs/stan-centrali.md`](../docs/stan-centrali.md)).
- Przy włączonym Wi-Fi na linii odbioru (D7) pojawiają się zakłócenia 🔌:
  UART zgłasza `GLITCH_DET` i błędy ramki,
  a bez Wi-Fi tego nie widać.
  Klient włącza filtr odbioru UART-u (impulsy krótsze niż ok. 1,6 µs),
  który usuwa krótkie zakłócenia;
  rzadkie dłuższe zakłócenia odbiór XMODEM przeżywa jednym ponowieniem bloku
  (zob. „Odporność na zakłócenia” w [`docs/protokol-serwisowy.md`](../docs/protokol-serwisowy.md)).
  Krótszy przewód do D7, mniejszy opornik szeregowy (1 kΩ zamiast 10 kΩ)
  i antena zewnętrzna (`antenna=ufl`) z dala od przewodów
  powinny zakłóceń ograniczyć ❓.
- Obszar na dane procesów potomnych (`ROOM_SIZE`) jest powiększony do 64 KiB:
  proces sieciowy z TCP ma 32 KiB, `ropam` 16 KiB, echo i zegar po 8 KiB.

Testy na komputerze: `esp32/rvuos/test/tcp-test.c`, `strona-test.c`
i `ropam-test.c` (klient z udawaną centralą i udawanym czasem;
sprawdza m.in., że flaga sesji gaśnie także po nieudanym odczycie).
Polecenia kompilacji są na ich początku.

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

- Podsłuch magistrali RS-485 (wymaga transceivera, np. MAX3485, tylko na odbiór);
  narzędzie do analizy zapisu: `tools/ropamnet.py analizuj`.
- Most przez sieć do narzędzi z komputera (TCP już jest, `tcp.c`).
