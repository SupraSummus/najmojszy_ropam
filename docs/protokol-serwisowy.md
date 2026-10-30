# Protokół serwisowy

Protokół, którym OptimaGSM Manager programuje centralę.
Odtworzony z firmware'u OptimaGSM v3.4,
jeszcze niezweryfikowany na prawdziwym kablu.
Oznaczenia: ✅ wynika wprost z kodu, ❓ wniosek albo przypuszczenie.

Klient: [`tools/ropam_serwis.py`](../tools/ropam_serwis.py).

## Warstwa fizyczna

- UART4 mikrokontrolera, w kodzie „port 2” ✅,
  najpewniej wyprowadzony na gniazdo RJ12 `RS232TTL` ❓.
- 115200 b/s ✅, 8N1 ❓, poziomy TTL 0/5 V.

## Kto zaczyna

Osobne zadanie RTOS (`0x9d02b640`)
po starcie odczekuje chwilę,
a potem w nieskończonej pętli woła dyspozytor `FUN_9d03c3f0` ✅.
Dyspozytor próbuje odebrać pakiet komendy przez XMODEM,
więc centrala co 2 sekundy wysyła na port bajt `'C'` (0x43),
po 16 próbach wysyła 3 × CAN i zaczyna od nowa ✅.

Po podłączeniu kabla powinno być widać strumień `C` co 2 sekundy.
To najprostszy test, że jesteśmy na właściwym porcie i z właściwą prędkością.

## Transport: XMODEM

Implementacja odpowiada popularnemu `xmodem.c` Georges'a Menie,
z jednym niestandardowym rozszerzeniem ✅.

| Bajt | Znaczenie |
|---|---|
| `0x01` SOH | blok 128 B |
| `0x02` STX | blok 1024 B |
| `0x19` | blok 64 B (rozszerzenie Ropamu) |
| `0x04` EOT | koniec transmisji |
| `0x06` ACK | potwierdzenie |
| `0x15` NAK | błąd, powtórz |
| `0x18` CAN | przerwanie (dwa pod rząd) |
| `'C'` | odbiorca chce sum CRC-16 |

Blok: nagłówek, numer bloku (od 1), dopełnienie numeru do 0xFF, dane,
CRC-16/XMODEM (wielomian 0x1021, start 0, starszy bajt pierwszy) ✅.
W trybie bez `'C'` zamiast CRC jest 8-bitowa suma bajtów ✅.

Centrala jako nadawca dobiera rozmiar bloku według długości całej transmisji ✅:
do 64 B bloki 64 B (`0x19`),
do 1023 B bloki 128 B (SOH),
powyżej bloki 1024 B (STX).
Dopełnienie: jeden bajt `0x1A`, dalej zera ✅.
XMODEM nie przenosi długości, więc odbiorca dostaje dane z dopełnieniem.

## Sesja

1. Centrala nadaje `'C'`.
2. Komputer wysyła **pakiet komendy** XMODEM-em (do 255 B, w praktyce jeden blok).
   Pierwszy bajt to kod komendy, dalej argumenty.
3. Zależnie od komendy centrala:
   odsyła dane XMODEM-em (komputer zaczyna od `'C'`),
   odbiera dane XMODEM-em (centrala zaczyna od `'C'`),
   albo odsyła status.
4. **Status** to 4-bajtowa liczba little-endian wysłana XMODEM-em ✅
   (`FUN_9d03c2dc`), zwykle 1 = sukces, 0 = błąd ❓.

## Komendy

Kody `0x32`–`0x45` obsługuje tablica wskaźników w RAM,
aktywna tylko przy ustawionej fladze ❓ (być może tryb ON-LINE).
Pozostałe są w `switch` w `FUN_9d03c3f0`.

| Kod | Działanie | Pewność |
|---|---|---|
| `0x01` | identyfikacja: 12 B (`00 22 xx 14` + 8 B), zapisuje w dzienniku zdarzenie programowania | ✅ |
| `0x02` | zapis konfiguracji: blob 52 412 B (v3.4) z CRC-16/XMODEM, zapis do pliku `0:config.rpm` i kopii `0:cfgcpy.rpm`; odpowiedź: status | ✅ |
| `0x03` | odczyt konfiguracji: argument to 6-znakowy kod serwisowy; przy zgodności wysyła `0:config.rpm`, inaczej status 2 | ✅ |
| `0x07` | odczyt bloku 198 B (`0xC6`) | ✅ |
| `0x08` | odczyt tablicy modułów magistrali, 198 B | ❓ |
| `0x12` | wpisanie PIN-u karty SIM do modemu (`AT+CPIN`), opcja „WPISZ KOD PIN” z instrukcji | ✅ |
| `0x04`, `0x05`, `0x0D`, `0x0F`–`0x31`, `0x4A`–`0x4C` | do rozpracowania | |

Rozmiar konfiguracji jest zaszyty w firmware'ze (`0x9d06b79c`),
więc może się różnić między wersjami.
Pliki `.rpm` zapisywane przez OptimaGSM Managera nie mają tego samego rozmiaru
co blob w v3.4, więc albo mają dodatkowe sekcje, albo pochodzą z innej wersji ❓.

## Tryb zdalny

Gdy w konfiguracji ustawiony jest odpowiedni bit,
ta sama obsługa komend działa przez modem (port 3, GPRS),
ale zamiast XMODEM-a używa ramki `'S'`, 2 bajty długości, dane, suma ✅
(`FUN_9d04f1a4`, `FUN_9d04f3bc`).
Przy innym bicie dane przechodzą przez `FUN_9d062524`
z 16-bajtowym kluczem z konfiguracji ❓ (szyfrowanie kluczem TCP/IP?).
Do rozpracowania.
