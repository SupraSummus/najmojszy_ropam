# ESP32

Oprogramowanie na Seeed XIAO ESP32-C6 (inne ESP32 też powinny pasować po zmianie pinów).

## `sniffer/`: bierny podsłuch magistrali

Przekazuje bajty z magistrali RopamNET bez zmian przez USB do komputera.
Nic nie nadaje.

### Połączenia

Potrzebny transceiver RS-485 zasilany 3,3 V (np. MAX3485, SP3485).
Bezpośrednie podłączenie A/B do GPIO uszkodzi ESP32.

| Transceiver | XIAO ESP32-C6 | Centrala |
|---|---|---|
| RO | D7 | |
| DI | (niepodłączone) | |
| DE, /RE | GND (na stałe: tylko odbiór) | |
| VCC | 3V3 | |
| GND | GND | `GND` |
| A | | `A` |
| B | | `B` |

Bez dodatkowego terminatora 120 Ω.
Szczegóły i środki ostrożności: [`docs/protokol-ramek.md`](../docs/protokol-ramek.md).

### Użycie

1. Wgraj `sniffer/sniffer.ino` (Arduino IDE z rdzeniem ESP32,
   płytka „XIAO_ESP32C6”, „USB CDC On Boot: Enabled”).
2. Zapisz strumień:

   ```sh
   stty -F /dev/ttyACM0 raw -echo
   timeout 60 cat /dev/ttyACM0 > private/magistrala.bin
   ```

3. Przeanalizuj:

   ```sh
   python3 tools/ropamnet.py analizuj private/magistrala.bin
   python3 tools/ropamnet.py analizuj private/magistrala.bin private/config.bin  # z nazwami
   ```

Zapisy z magistrali trzymamy w `private/`:
żądania panelu zawierają kody użytkowników.

## `most/`: Wi-Fi zamiast kabla serwisowego (niesprawdzone)

Przezroczysty most TCP (port 2323) ↔ gniazdo serwisowe RJ12.
Klient działa przez sieć tak samo jak przez kabel:

```sh
python3 tools/ropam_serwis.py socket://ADRES:2323 id
```

Każda komenda serwisowa włącza w centrali tryb ON-LINE na ok. 1,5 min
([`docs/stan-centrali.md`](../docs/stan-centrali.md)),
więc most nadaje się do konfiguracji i okazjonalnego podglądu,
a nie do ciągłego monitoringu.

### Połączenia

Gniazdo RJ12 ma trzy używane żyły: masę, nadawanie (TX) i odbiór (RX) centrali.
TX centrali rozpoznasz po tym, że w spoczynku nadaje `C` co 2 s
(widać to szkicem `sniffer` i `picocom /dev/ttyACM0`).

Procesor centrali pracuje na 3,3 V, ale poziomu na gnieździe nie znamy:
zmierz stan spoczynkowy TX centrali.
ESP32-C6 nie toleruje 5 V.

| Gniazdo RJ12 | XIAO ESP32-C6 |
|---|---|
| GND | GND |
| TX centrali | D7 przez 1 kΩ (3,3 V) albo dzielnik 10 kΩ / 20 kΩ do GND (5 V) |
| RX centrali | D6 przez 1 kΩ |

Dane sieci Wi-Fi: skopiuj `most/sekrety.h.przyklad` jako `most/sekrety.h`.
