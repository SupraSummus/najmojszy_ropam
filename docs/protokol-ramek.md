# Protokół ramek

Odtworzony z firmware'u OptimaGSM v3.4
(adresy funkcji odnoszą się do obrazu załadowanego pod `0x9D000000`).
Nic z tego nie zostało jeszcze sprawdzone na prawdziwym kablu.
Oznaczenia: ✅ wynika wprost z kodu, ❓ wniosek albo przypuszczenie.

Implementacja: [`tools/ropamnet.py`](../tools/ropamnet.py).

## UART-y centrali

Wszystkie cztery porty są ustawione identycznie ✅:
115200 b/s przy zegarze peryferiów 80 MHz
(`UARTSetDataRate(id, 80000000, 115200)` w `FUN_9d050f58`).
Format znaku zakładamy 8N1 ❓.

| Port w kodzie | UART PIC32 | Adres SFR | Bufor RX | Przeznaczenie |
|---|---|---|---|---|
| 0 | UART1 | `0xBF806000` | 512 B | magistrala RopamNET (RS-485) ✅ |
| 1 | UART2 | `0xBF806800` | 128 B | złącze EXP, moduł FGR-4 ❓ |
| 2 | UART4 | `0xBF806200` | 1500 B | port serwisowy RS232TTL, XMODEM ([`protokol-serwisowy.md`](protokol-serwisowy.md)) |
| 3 | UART5 | `0xBF806A00` | 1024 B | modem SIM900 (komendy AT) ✅ |

Przesłanki:

- port 0: przed nadawaniem ustawiany jest pin portu F (maska `0x08`), a po nadaniu zerowany,
  czyli sterowanie kierunkiem transceivera RS-485 ✅;
- port 1: używają go funkcje wysyłające MMS ze zdjęciami z karty SD ✅,
  a pod debugowym przełącznikiem trafia tam też zrzut ramek `Rxframe adres:%x cmd:%x len:%x` ✅;
- port 2: na nim działa XMODEM-owa obsługa komend programowania ✅
  i tam idzie zrzut wyjątku procesora `exception adress:%x ID:%i` ✅;
- port 3: wszystkie komendy `AT…` ✅.

## Format ramki

Ten sam format działa na portach 0 i 1 ✅
(`FUN_9d052f50` wysyła, `FUN_9d0532dc` odbiera).

### Zapytanie (centrala → urządzenie)

```
+-------+---------+---------+------------+--------+--------+
| adres | komenda | długość | dane …     | CRC_hi | CRC_lo |
+-------+---------+---------+------------+--------+--------+
  1 B     1 B       1 B       0–200 B      2 B
```

- Długość `0xAA` albo `0xFF` oznacza ramkę bez danych ✅.
- Adres `0xFE` to rozgłoszenie: centrala nie czeka na potwierdzenie ✅.

### Potwierdzenie (1 bajt)

| Bajt | Znaczenie |
|---|---|
| `0x03` | odebrano poprawnie ✅ |
| `0x01` | zła suma CRC, centrala ponawia ✅ |
| brak | po limicie czasu centrala traktuje to jak `0x02` („noack”) i ponawia ✅ |

Centrala ponawia ramkę najwyżej 3 razy ✅.
Liczniki z tego mechanizmu są w raporcie diagnostycznym
`Frames ok:%u noack:%u badcrc:%u lost:%.1f%%` ✅.

### Odpowiedź (urządzenie → centrala)

```
+------+---------+---------+------------+--------+--------+
| 0xAA | komenda | długość | dane …     | CRC_hi | CRC_lo |
+------+---------+---------+------------+--------+--------+
```

Centrala szuka w strumieniu bajtu `0xAA`,
odbiera resztę, sprawdza CRC
i sama odsyła bajt potwierdzenia (`0x03` albo `0x01`) ✅.
Na porcie 0 odczekuje przy tym kilkaset mikrosekund ❓
i przełącza kierunek RS-485.

### CRC

CRC liczone jest po wszystkich bajtach od adresu do końca danych
i wysyłane starszym bajtem naprzód ✅ (`FUN_9d052d48`, `FUN_9d052c94`):

```python
crc = 0xFFFF
for b in bajty:
    crc ^= b
    for _ in range(8):
        crc >>= 1
        if crc & 1:
            crc ^= 0xA001
```

To **nie jest** standardowe CRC-16/MODBUS:
tam bit 0 testuje się przed przesunięciem.
Kolejność potwierdza asembler (`srl`, potem `andi …, 1`) ✅.

## Znane komendy

### Adres `0xB0` (port 0)

Rodzina komend `0xF0`–`0xF7`,
obsługująca grupy po 8 modułów wejść/wyjść,
najpewniej koncentrator Hub-IQPLC ❓.

| Komenda | Długość danych | Funkcja w firmware'ze |
|---|---|---|
| `0xF0` | 8 | `FUN_9d04f590` (wymiana stanu, odpowiedź również `0xF0`) |
| `0xF1` | 8 | `FUN_9d04f668` (wymiana stanu, odpowiedź `0xF1`, 84 B danych) |
| `0xF2` | 4 | `FUN_9d04f7bc` |
| `0xF3` | 32 | `FUN_9d04f740` |
| `0xF4` | 1 | `FUN_9d04f864` |
| `0xF5` | 2 | `FUN_9d04f8f8` |
| `0xF6` | 6 | `FUN_9d04f994` |
| `0xF7` | 0 | `FUN_9d04fa08` |

## Organizacja magistrali

### Gniazda modułów

Centrala trzyma tablicę 18 gniazd po 11 B (`g_modules`) ✅.
Bajt 0 to adres na magistrali, bit 0 bajtu 1 oznacza moduł aktywny.
Numer gniazda ma stałe znaczenie (`bus_poll_round`) ✅:

| Gniazdo | Obsługa |
|---|---|
| 1–4 | panele dotykowe TPR:1–4 (`tpr_service`) |
| 5 | `FUN_9d05d0a0` ❓ |
| 6 | ekspander wejść na magistrali, np. EXP-I8-RN (`expin_service`, niżej) |
| 7 | zasilacz buforowy, najpewniej PSR-ECO-5012-RS (`psu_service`, niżej) |
| 8 | moduł na porcie 1 (złącze EXP) ❓ |
| 9 | koncentrator Hub-IQPLC (`hub_poll`) |
| 10 | `FUN_9d058b88` ❓ |
| 11, 12 | `FUN_9d066c0c` ❓ (np. APx-Aero) |
| 13, 14 | `FUN_9d0635f0` ❓ |
| 15 | zarządzanie sesją zdalną ❓ |

Tablica jest też częścią odpowiedzi na komendę serwisową `0x24`
([`stan-centrali.md`](stan-centrali.md)).

### Cykl pracy

Zadanie magistrali (`task_bus`) w pętli ✅:

1. obsługuje zlecenia od innych zadań,
   np. wysłanie ustawień paneli (sekcje `M_TPR1`–`M_TPR4`, po 2920 B)
   albo identyfikację modułów;
2. jeśli zleceń nie ma, wykonuje obieg odpytywania aktywnych gniazd (`bus_poll_round`),
   każde gniazdo ma własny licznik okresu;
3. co 50 obiegów rozgłasza stan systemu ramką `0x10` na adres `0xFE`;
4. co 410 obiegów rozgłasza ramkę `0x3C` na adres `0xFE`;
5. między obiegami czeka ok. 100 ms (`os_delay(100)`).

### Rozgłaszany stan: komenda `0x10`

Dane ramki to dokładnie 220-bajtowy nagłówek stanu (`status_header`),
ten sam co na początku odpowiedzi na komendę serwisową `0x24` ✅:
maski stanów wejść, maska wyjść, stan stref, flagi i czas
(opis pól w [`stan-centrali.md`](stan-centrali.md)).
Adres `0xFE` oznacza rozgłoszenie bez potwierdzenia.

**Bierny podsłuch magistrali wystarczy więc,
żeby znać pełny stan systemu**,
bez żadnej sesji serwisowej i bez wpływu na pracę centrali.

## Panel TPR (gniazda 1–4)

Obsługa jednego panelu (`tpr_service`) ✅:

1. centrala wysyła odpytanie `[adres][0x2B][0]`;
2. panel odpowiada ramką `[0xAA][komenda][długość][dane][CRC]`
   ze swoim żądaniem;
3. centrala obsługuje żądanie i odsyła `[adres][ta sama komenda][długość][dane][CRC]`.

Żądania panelu:

| Komenda | Dane od panelu | Działanie centrali | Odpowiedź |
|---|---|---|---|
| `0x11` | 10 B | stan panelu: m.in. napięcie zasilania, sabotaż obudowy, wejścia panelu | brak |
| `0x12` | kod | weryfikacja kodu | 2 B: numer użytkownika i uprawnienia |
| `0x13` | kod, … | ❓, zdarzenie `0x62` w dzienniku | 1 B |
| `0x14` | kod, … | ❓, zdarzenie `0x64` | 1 B |
| `0x15` | 4 B | ❓, zdarzenie `0x63` | 1 B |
| `0x16` | strefa, tryb | działanie na strefie bez kodu (szybkie uzbrojenie ❓) | 1 = OK, `0xFE` = zła strefa, `0x65` = odmowa |
| `0x18` | 8 B | ❓, zdarzenie `0x0D` | 1 B: 1 = OK |
| `0x1C` | kod, strefy, tryb | **uzbrojenie stref kodem** | numer użytkownika albo `0xFE`/`0x65` |
| `0x1D` | kod, strefy | działanie kodem na strefach ❓ (rozbrojenie albo kasowanie alarmu) | numer użytkownika |
| `0x1E` | – | ❓ | 3 B |
| `0x20` | zakres | odczyt dziennika zdarzeń, rekordy po 19 B | seria ramek |
| `0x23`, `0x24` | podkomenda | przesyłanie ustawień panelu w obie strony (wiele ramek, kontynuacja `'!'`) | |
| `0x25`–`0x40` | | do rozpracowania | |

### Kody i strefy

- Kod jest napisem (porównanie `strcmp`) ✅.
  W żądaniu `0x1C` bajt danych 0 to długość kodu ❓ (razem z końcowym zerem),
  kod zaczyna się od bajtu 2,
  a zaraz za nim jest bajt stref i trybu ✅.
- Bajt stref i trybu: młodsze 4 bity to **maska stref 1–4**,
  starsze 4 bity to tryb (niezerowy = czuwanie nocne) ✅.
- Weryfikacja kodu (`code_lookup`) przeszukuje tablicę 32 użytkowników po 34 B
  (kod jako napis, bajt `+0x0D` to uprawnienia) ✅.
  Czterocyfrowy kod serwisowy daje numer 100, brak dopasowania `0xFF`.
- Stan strefy to 8 B ✅:
  młodsze 4 bity pierwszego słowa to stan (3 = gotowa, rozbrojona; 5 = odliczanie czasu na wyjście ❓),
  bit 11 = czuwanie nocne,
  drugie słowo to licznik czasu na wyjście w ms.

### Port 1

Przed właściwą ramką centrala wysyła ramkę „budzącą”
z tym samym adresem, komendą `0x9B` i bez danych,
a po 150 ms ramkę właściwą ✅ (`FUN_9d053b2c`).

## Ekspander wejść (gniazdo 6)

Odczytane z firmware'u (`expin_service`), niesprawdzone na kablu ❓.
Adres modułu to bajt 0 gniazda 6;
w gnieździe są też numer pierwszego wejścia systemu, które moduł obsługuje,
i liczba tych wejść (razem najwyżej do wejścia 48).

Obieg (licznik w gnieździe):

- zwykle wymiana `0xDD`;
- co 60. raz zamiast niej `0xDC`: 1 B, odpowiedź 64 B (opis modułu ❓);
- co 11. obieg `0xDF` z tymi samymi 8 B co `0xDD`,
  odpowiedź to pełny blok stanu modułu (do ok. 0x76 B),
  w którym pod `0x67`–`0x69` są te same 3 bajty co pod 19–21 odpowiedzi `0xDD`.

### `0xDD`: zapytanie, 8 B

| Bajt | Treść |
|---|---|
| 0 | bit 0 i bit 3: któraś strefa w czuwaniu; bit 1: flaga sesji serwisowej; bit 2: bit 0 flag systemu ❓; bit 7: zawsze 1 |
| 1–2 | maska 16 wejść modułu, które teraz czuwają: wszystkie ich strefy w czuwaniu (pełnym albo nocnym), a wejście nie jest wyłączone ❓ |
| 3–4 | stany stref zebrane w maski po 4 bity (bit = strefa): m.in. odliczanie czasu na wyjście ❓ |
| 5 | bity 0–3: czuwanie nocne stref 1–4; bity 4–7 ❓ |
| 6–7 | stan wyjść 17–32 (z uwzględnieniem polaryzacji wyjścia) |

Ekspander dostaje więc stan czuwania, stref i części wyjść;
wyjścia 17–32 sugerują, że ta sama wymiana obsługuje też ekspandery wyjść ❓.

### `0xDD`: odpowiedź

| Bajt | Treść |
|---|---|
| 0–15 | stan każdego z 16 wejść, opis niżej |
| 16 | bit 7: zdarzenie `0x7F` (argument: bajt 18) i flaga w stanie systemu, wtedy wszystkie wejścia jak przy bicie 3 (sabotaż modułu ❓); bit 6: po ok. 1800 obiegach centrala zleca coś modułowi ❓; bit 4: flaga gniazda |
| 17–18 | ❓ (bajt 18 to argument zdarzenia `0x7F`) |
| 19–21 | bity 0–4: numer ❓, bity 8–12: przyciski ❓, bit 13: zdarzenie `0x6A`; bajt 21 ≠ 0 zapisywany osobno ❓ |

Bajt stanu wejścia:

| Bit | Znaczenie |
|---|---|
| 0 | wejście obsługiwane; bez niego wartość linii 60000 |
| 1 | naruszenie: wartość linii 2200 zamiast 1100 |
| 2 | wartość linii 60000 (przerwa ❓) |
| 3 | zdarzenia `0x71`/`0x72` przy zmianie; wartość linii 2200 albo 60000 zależnie od ustawienia ❓ |
| 4 | zdarzenia `0x73`/`0x74` przy zmianie, przy `0x73` także SMS z ustawień |

Centrala nie przenosi stanów wejść wprost,
tylko udaje zmierzone wartości linii (1100 spoczynek, 2200 naruszenie, 60000 przerwa)
i wpisuje je do tablicy wejść (`inputs_set_lines`),
tej samej, w której są pomiary wejść płyty głównej
(pod `0xDC` odpowiedzi na komendę serwisową `0x24`, [`stan-centrali.md`](stan-centrali.md)).
Dalej wejścia ekspandera przechodzą tę samą logikę co wejścia płyty.

## Zasilacz (gniazdo 7)

Odczytane z firmware'u (`psu_service`), niesprawdzone na kablu ❓.
Przypisanie do PSR-ECO-5012-RS wynika z listy modułów w instrukcji
i z treści wymiany (230 V, akumulator).

Zapytanie `0xDD`, 4 B:
bajt 0 to jeden bit ustawień centrali ❓, reszta to zera.

Odpowiedź `0xDD`:

| Bajt | Treść |
|---|---|
| 0 | bit 0: jest 230 V; bity 3–5: usterki (każda ustawia flagę w stanie systemu) |
| 1 | ❓ |
| 2–13 | sześć wartości 16-bitowych, starszy bajt pierwszy (napięcia, prądy ❓) |
| 14 | akumulator: 0 sprawny, 1 albo 2 usterka |

Brak 230 V centrala sygnalizuje od razu,
a po czasie z ustawień zapisuje zdarzenie `0x29` z argumentem 1
(powrót: `0x29` z argumentem 0).
Usterka akumulatora to zdarzenie `0x2B` (1 początek, 0 koniec).
Przy zdarzeniach wysyła SMS-y z ustawień ❓.

## Do zrobienia

- Moduły w gniazdach 5, 8 i 10–14.
- Ekspander i zasilacz: sprawdzić na kablu; znaczenie bajtów oznaczonych ❓.
- Dokładny układ żądań `0x12`–`0x1D` (pola przed kodem) i komend `0x25`–`0x40`.
- Weryfikacja na kablu: pasywny podsłuch RS-485 z 115200 8N1,
  sprawdzenie CRC narzędziem `ropamnet.py check`
  i porównanie ramki `0x10` z odpowiedzią na `0x24`.
