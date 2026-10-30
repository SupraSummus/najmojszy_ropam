# Firmware

Pliki leżą w [`firmware/`](../firmware/).

## Centrala OptimaGSM: `optimagsm_vX.Y_pl.flsh`

### Format

Plik to tekstowy Intel HEX (rekordy typu `00`, `01`, `04`)
z trzema odstępstwami od standardu:

1. Bajty danych w rekordach typu `00` są XOR-owane 16-bajtowym kluczem
   według pozycji bajtu w rekordzie (nie według adresu):

   ```
   klucz = 64 52 f7 b6 dc e0 0d f1 98 31 cf 4b f9 f3 33 9f
   jawny[i] = szyfr[i] ^ klucz[i % 16]
   ```

   Ten sam klucz działa dla wersji 3.2, 3.3 i 3.4.
2. Sumy kontrolne rekordów nie zgadzają się
   ani z danymi zaszyfrowanymi, ani z odszyfrowanymi.
   Możliwe, że liczy się je jeszcze inaczej. ❓
3. Po rekordzie końca pliku są dwie dodatkowe linie:
   `=xxxx` (4 cyfry szesnastkowe, pewnie CRC-16 ❓)
   i `?X.Y` (wersja firmware'u).

Dekoder: [`tools/flsh.py`](../tools/flsh.py) zapisuje zwykły Intel HEX,
a [`tools/ihex.py`](../tools/ihex.py) wypisuje segmenty albo zapisuje je jako `.bin`.

```sh
python3 tools/flsh.py firmware/optimagsm/optimagsm_v3.4_pl.flsh > v3.4.hex
python3 tools/ihex.py info v3.4.hex
python3 tools/ihex.py bin v3.4.hex v3.4/
```

### Jak doszliśmy do klucza

- Ten sam szyfrogram występuje pod różnymi adresami,
  więc szyfr nie zależy od adresu.
- Rozkład bajtów w każdej kolumnie (pozycji w rekordzie)
  ma te same różnice między najczęstszymi wartościami (`^0x21`, `^0x01` itd.),
  czyli to XOR ze stałą na kolumnę.
- Słowa konfiguracyjne zaczynały się od `9b ad`,
  co po XOR z `64 52` daje oczekiwane `ff ff`.
- Brakujące kolumny ustaliliśmy atakiem ze znanym tekstem jawnym
  (`OptimaGSM`, `wejscie`, `alarm`).

### Mapa pamięci (v3.4)

Typowy układ aplikacji PIC32MX za bootloaderem:

| Adres | Zawartość |
|---|---|
| `0x1D000180` | wektor wyjątków ogólnych |
| `0x1D000200`–`0x1D000867` | tablica wektorów przerwań (co 0x20 B, po 8 B) |
| `0x1D001000` | początek kodu aplikacji |
| do `0x1D06E66B` | koniec obrazu (ok. 440 KB, więc układ ma co najmniej 512 KB flasha) |
| `0x1FC02FF0`–`0x1FC02FFF` | słowa konfiguracyjne DEVCFG3…DEVCFG0 |

Bootloadera (AN1388, boot flash `0x1FC00000`) w pliku nie ma.

Słowa konfiguracyjne z pliku (v3.4):

| Rejestr | Wartość |
|---|---|
| DEVCFG3 | `0x3CFFFFFF` |
| DEVCFG2 | `0xFFF8FFD9` |
| DEVCFG1 | `0xFF70CDDB` |
| DEVCFG0 | `0x7FFFFFFF` |

To wartości z pliku.
Nie wiadomo, czy bootloader je w ogóle programuje
(AN1388 standardowo pomija słowa konfiguracyjne),
więc nie świadczą o ustawieniach fabrycznych, np. o ochronie kodu.

### Co widać w napisach

- Pełna lista zdarzeń dziennika
  (alarmy, czuwanie, sabotaże, zasilanie, GSM).
- Obsługa modemu SIM900 komendami AT:
  SMS, połączenia głosowe, GPRS (`AT+CIPSTART="TCP"`, `"UDP"`),
  e-mail (`AT+SMTP…`), MMS (`AT+CMMS…`),
  NTP z `tempus1.gum.gov.pl`.
- RopamBridge (`Bridge:%s %i`, „Blad akywacji uslugi RopamBridge”).

### Co wynika z kodu

- Rodzina PIC32MX5xx/6xx/7xx:
  numery przerwań UART-ów (U2 41/42, U4 68/69, U5 75) pasują tylko do niej.
- 128 KB RAM (stos startuje pod `0xA001FFE8`), więc model z 512 KB flasha i 128 KB RAM,
  np. PIC32MX795F512 albo 695F512 ❓.
- Zegar: kwarc XT, PLL ÷2 ×20 ÷1, szyna peryferiów bez dzielnika,
  czyli 80 MHz przy kwarcu 8 MHz.
  Potwierdza to wywołanie `UARTSetDataRate(…, 80000000, …)`.
- `$gp = 0xA0007FF0` (ustawiane w `crt0`).
- Kod skompilowano bez optymalizacji (`-O0`, XC32 z biblioteką peryferiów PLIB),
  dlatego dekompilacja jest bardzo czytelna.

Protokoły odczytane z kodu: [`protokol-ramek.md`](protokol-ramek.md).

### Analiza w Ghidrze

```sh
tools/ghidra/analyze.sh firmware/optimagsm/optimagsm_v3.4_pl.flsh
```

Skrypt odszyfrowuje plik, składa płaski obraz,
ładuje go jako MIPS32 little-endian pod `0x9D000000`,
dodaje bloki RAM i SFR, ustawia `$gp`, oznacza wektory przerwań
i zapisuje zdekompilowany C do `build/<nazwa>/decomp.c`.
Asembler wybranych funkcji wypisuje `tools/ghidra/Disasm.java`.

## Panel TPR: `tpr_optima_vX.Y_pl.flsh`

Nagłówek `rpmflash` (8 B) i 2 B (suma kontrolna? ❓), dalej jawny obraz binarny:

- brak szyfrowania: indeks koincydencji ok. 0,05, typowy dla kodu maszynowego,
  bez okresowości, która wskazywałaby na XOR z kluczem;
- na początku tablica 32-bitowych wartości little-endian
  (`0x00000592`, `0x00000593`, …, `0x0080F5C0`, `0x008076AA`),
  najpewniej wektory przerwań;
- pod koniec jawny opis: `Firmware dla TPR OptimaGSM` i wersja (`1.4`);
- napisów interfejsu nie widać wprost (czcionki graficzne albo inne kodowanie ❓).

Procesor panelu jest nieznany:
adresy nie pasują do PIC32 ani do ARM Cortex-M.
Do dalszej analizy potrzebne jest oznaczenie układu z płytki panelu.
Panel aktualizuje się przez własne złącze RJ12 programem Partner GSM.

## APm-Aero: `aero_apm_X.Y.flsh`

Pierwsza linia to JSON z metadanymi
(`code`, `family`, `version`, `device`, `md5`, `size`).
Dalej base64 z plikiem w formacie TI-TXT (`@8000`, bajty szesnastkowo),
czyli najpewniej MSP430,
zgodnie z narzędziem `BSL.exe` w pakiecie aktualizacyjnym.
Bajty wyglądają na zaszyfrowane. ❓
