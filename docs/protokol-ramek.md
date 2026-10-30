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
| 2 | UART4 | `0xBF806200` | 1500 B | port serwisowy RS232TTL ❓ |
| 3 | UART5 | `0xBF806A00` | 1024 B | modem SIM900 (komendy AT) ✅ |

Przesłanki:

- port 0: przed nadawaniem ustawiany jest pin portu F (maska `0x08`), a po nadaniu zerowany,
  czyli sterowanie kierunkiem transceivera RS-485 ✅;
- port 1: używają go funkcje wysyłające MMS ze zdjęciami z karty SD ✅,
  a pod debugowym przełącznikiem trafia tam też zrzut ramek `Rxframe adres:%x cmd:%x len:%x` ✅;
- port 2: tam idzie zrzut wyjątku procesora `exception adress:%x ID:%i` ✅,
  a odbiór działa przez DMA, co pasuje do dużych bloków konfiguracji ❓;
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

### Inne (port 0)

Komendy `0x10`, `0x2B`, `0x3C` wysyłane pod adres podany w argumencie
(`FUN_9d01cde0`, `FUN_9d01ce74`, `FUN_9d01cd4c`).
Główna pętla obsługi magistrali to `FUN_9d01d264` ❓,
prawdopodobnie odpytywanie paneli TPR i ekspanderów.

### Port 1

Przed właściwą ramką centrala wysyła ramkę „budzącą”
z tym samym adresem, komendą `0x9B` i bez danych,
a po 150 ms ramkę właściwą ✅ (`FUN_9d053b2c`).

## Do zrobienia

- Mapa adresów paneli TPR, ekspanderów EXP-I8-RN/EXP-O8x-RN i ich komend
  (zacząć od `FUN_9d01d264`).
- Protokół portu serwisowego (port 2): komendy odczytu i zapisu konfiguracji, tryb ON-LINE.
- Weryfikacja na kablu: pasywny podsłuch RS-485 z 115200 8N1 i sprawdzenie CRC narzędziem `ropamnet.py check`.
