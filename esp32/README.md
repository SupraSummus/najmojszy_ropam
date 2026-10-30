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
Przepływ bajtów do centrali: do sprawdzenia po podłączeniu.

### Połączenia

Gniazdo RJ12 centrali ma trzy używane żyły: masę, nadawanie (TX) i odbiór (RX) centrali.
Procesor centrali pracuje na 3,3 V, ale poziomu na gnieździe nie znamy;
ESP32-C6 nie toleruje 5 V.

| Gniazdo RJ12 | XIAO ESP32-C6 |
|---|---|
| GND | GND |
| TX centrali | **D7** (GPIO17, odbiór) przez 1 kΩ, jeśli linia ma 3,3 V, albo przez dzielnik 10 kΩ / 20 kΩ do GND, jeśli 5 V |
| RX centrali | **D6** (GPIO16, nadawanie) przez 1 kΩ |

TX centrali rozpoznasz po tym, że w spoczynku nadaje `C` co 2 s:
z wgranym mostem widać je w `picocom /dev/ttyACM0`.
Zamiana żył niczego nie uszkodzi przy rezystorach 1 kΩ, po prostu nie będzie danych.

Przy każdym restarcie XIAO ROM układu wypisuje na D6 krótki tekst startowy,
który trafia do centrali;
nie ma w nim bajtów nagłówków XMODEM ani CAN, więc centrala go pomija.

Każda komenda serwisowa włącza w centrali tryb ON-LINE na ok. 1,5 min
([`docs/stan-centrali.md`](../docs/stan-centrali.md)).

## Dalej

- Podsłuch magistrali RS-485 (wymaga transceivera, np. MAX3485, tylko na odbiór);
  narzędzie do analizy zapisu: `tools/ropamnet.py analizuj`.
- Most przez sieć: rvuos ma stos z UDP (bez TCP).
