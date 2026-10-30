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
