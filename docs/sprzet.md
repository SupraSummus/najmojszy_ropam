# Sprzęt: OptimaGSM

Rodzina obejmuje warianty OptimaGSM, OptimaGSM-PS
(z wbudowanym zasilaczem i obsługą akumulatora `+BAT`/`-BAT`)
oraz wersje -D9M na szynę DIN.
Elektronika sterująca i firmware są wspólne.

Źródło: instrukcja instalacji OptimaGSM (© 2015 Ropam Elektronik),
o ile nie zaznaczono inaczej.
Oznaczenia: ✅ potwierdzone w dokumentacji,
❓ niepotwierdzone albo z drugiej ręki.

## Interfejsy komunikacyjne

Centrala ma trzy drogi komunikacji, które mogą nas interesować.

| Złącze | Warstwa fizyczna | Do czego służy |
|---|---|---|
| `RS232TTL` (RJ12) | UART, poziomy TTL 0/5 V | programowanie, aktualizacja firmware'u, podgląd ON-LINE |
| `A`, `B`, `GND` | EIA-485 (RS-485), magistrala **RopamNET** | panele dotykowe TPR, ekspandery `-RN`, koncentrator IQPLC |
| `EXP` | lokalne złącze modułów | moduł FGR-4 (MMS), wg innych źródeł także AP-IP ❓ |

Pozostałe złącza:
`X2` (lokalny ekspander wejść EXP-I8),
`X3` (lokalny wyświetlacz LCD, zworka resetu),
`VSR` (syntezer mowy, moduł audio),
`T1`–`T4` (czujniki temperatury TSR-1, linia `Data`, zasilanie 3,3 V).

### Port serwisowy RS232TTL ✅

- Gniazdo RJ12 na płycie.
- Producent wymaga kabla RS232-MGSM albo USB-MGSM
  z wbudowanym konwerterem na poziomy TTL.
- Zworka `PR` uruchamia tryb aktualizacji firmware'u przez ten port.
- W trybie ON-LINE (podgląd wejść, sterowanie wyjściami z komputera)
  **centrala nie wykonuje swoich normalnych funkcji**,
  np. nie wysyła SMS-ów przy naruszeniu wejść.
  Nie nadaje się więc na stałe łącze bramki.

Pinout kabla od strony RJ12 (❓, z forum elektroda.pl):

| Pin RJ12 | Sygnał | Kolor |
|---|---|---|
| 4 | GND | czarny |
| 5 | RX komputera | niebieski |
| 6 | TX komputera | biały |

Kabel USB-MGSM to ❓ konwerter na układzie FTDI.
Instalator OptimaGSM Managera zawiera standardowy sterownik FTDI (`ftd2xx`),
co to potwierdza pośrednio.
Pod Linuksem powinien działać natywnie jako `/dev/ttyUSB*` (sterownik `ftdi_sio`).

Z firmware'u ❓: port serwisowy to najpewniej UART4 mikrokontrolera, 115200 b/s
(zob. [`protokol-ramek.md`](protokol-ramek.md)).
Nieznane: komendy protokołu serwisowego.

### Magistrala RopamNET ✅

- EIA-485, łączenie `A`–`A`, `B`–`B`, `GND`–`GND`.
- Topologia „pętli” (w praktyce linia),
  terminacja 120 Ω na końcach (zworka `JT1` na centrali).
- Urządzenia: panele TPR-1x, TPR-2x (i TPR-4x ❓),
  ekspandery EXP-I8-RN, EXP-O8R-RN, EXP-O8T-RN,
  koncentrator Hub-IQPLC.
- Panele i ekspandery są wykrywane „identyfikacją” w OptimaGSM Managerze,
  więc mają adresy na magistrali.

Z firmware'u ✅: UART1 mikrokontrolera, 115200 b/s,
ramki z adresem, komendą, długością i CRC-16, potwierdzane jednym bajtem.
Szczegóły w [`protokol-ramek.md`](protokol-ramek.md).
Wg forum elektroda.pl port RS232TTL w NeoGSM jest w normalnej pracy nieaktywny,
co sugeruje, że producent celowo nie przewiduje integracji po porcie serwisowym.

## Pozostałe parametry

- Modem GSM: SIMCom SIM900 (quad-band, GPRS class 10, CSD).
- Wejścia: 8 na płycie, do 32 z ekspanderami
  (Modbus z AP-IP raportuje do 48 wejść i 40 wyjść).
- Wyjścia: O1–O2 prądowe, O3–O8 typu open collector 100 mA.
- Wejście analogowe 0–10 V, 4 czujniki temperatury.
- Mikrokontroler: ✅ **PIC32MX** z co najmniej 512 KB flasha.
  Potwierdzają to mapa pamięci i słowa konfiguracyjne w firmware'ze
  (zob. [`firmware.md`](firmware.md))
  oraz aktualizator oparty na bootloaderze Microchip AN1388.
  Dokładny model ❓: trzeba odczytać z płyty.
- Panel TPR i moduł APm-Aero mają własne procesory
  (APm-Aero najpewniej MSP430).
