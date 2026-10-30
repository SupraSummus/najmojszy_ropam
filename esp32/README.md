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
Nadawanie z D6 do centrali: jeszcze nie działa.
Przy pierwszej próbie (kabel USB-MGSM odłączony od komputera)
centrala słyszała własne echo
([`docs/protokol-serwisowy.md`](../docs/protokol-serwisowy.md#objaw-centrala-słyszy-własne-echo)),
choć D6 nadawał i w spoczynku trzymał stan wysoki
(sprawdzone programem `ropam-piny`, niżej).
Program `ropam-d6` trzymał potem D6 przez 2 s w stanie niskim
i rytm echa nie zmienił się ani trochę:
D6 nie ma żadnego wpływu na RX centrali.
Linią steruje więc coś silnego (kabel, zwarcie TX z RX),
albo D6 do niej nie dochodzi; zob. „Diagnostyka”.

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
| RX centrali | **D6** (GPIO16, nadawanie) przez 1 kΩ |

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
- Most przez sieć: rvuos ma stos z UDP (bez TCP).
