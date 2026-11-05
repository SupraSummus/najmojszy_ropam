# Drogi integracji

Możliwe sposoby rozmowy z centralą,
od najprostszej do najambitniejszej.

| # | Droga | Medium | Udokumentowane | Nadaje się na stałą bramkę |
|---|---|---|---|---|
| A | Modbus TCP przez AP-IP | Ethernet/Wi-Fi | ✅ | tak, ale okrojone |
| B | protokół serwisowy | RS232TTL (RJ12) | ❌ | nie (tryb ON-LINE wyłącza funkcje centrali) |
| C | protokół serwisowy po TCP | GPRS, `connect IP:port` | częściowo | nie (restart po każdej sesji) |
| D | magistrala RopamNET | RS-485 | ❌ | tak, najpełniejsza |
| E | bootloader PIC32 | RS232TTL + zworka `PR` | prawie (AN1388) | nie, tylko do firmware'u |

## A. Modbus TCP przez AP-IP

Opisana w [`modbus-tcp.md`](modbus-tcp.md).
Wymaga dokupienia modułu AP-IP.
Daje wejścia, wyjścia, temperatury, zasilanie i awarie,
ale celowo nie daje czuwania ani stref.
Uzbrajanie trzeba obchodzić logiką centrali (wyjście → wirtualne wejście).

Najszybsza droga do Home Assistanta,
ale nie odpowiada na pytanie, jak centrala naprawdę gada.

## B. Protokół serwisowy na RS232TTL

Tym gada OptimaGSM Manager:
konfiguracja, odczyt i zapis ustawień, podgląd ON-LINE, sterowanie wyjściami.

Co wiemy o programie (wersja 2.5):

- napisany w C++Builderze (VCL, Embarcadero), kod natywny x86,
- port COM otwiera zwykłym Win32 API (`CreateFile`, `SetCommState`),
  bez bezpośredniego użycia biblioteki FTDI,
- ma też klienta i serwer TCP (Indy: `TIdTCPClient`, `TIdTCPServer`)
  oraz OpenSSL,
- tryby połączenia: kabel, „TCP/IP Ropam Bridge”, „TCP/IP serwer lokalny”, moduł AP-IP.

Skoro program używa zwykłego portu COM,
można go uruchomić pod Wine
z portem COM przekierowanym na pseudoterminal
i podsłuchać całą rozmowę przez `socat`,
bez sprzętowego sniffera:

```
OptimaGSM Manager (Wine) → COM1 → /dev/pts/X ⇄ socat -x ⇄ /dev/ttyUSB0 → centrala
```

To najtańszy sposób na poznanie protokołu.
Analiza statyczna w Ghidrze może potem objaśnić to, co widać na kablu.

Ograniczenie:
w trybie ON-LINE centrala nie wykonuje swoich zwykłych funkcji,
więc ta droga służy do konfiguracji i nauki, a nie do stałej bramki.
Nie wiemy też, czy port w ogóle odpowiada w normalnej pracy.

## C. Protokół serwisowy po TCP

Od firmware'u 1.7 centrala na komendę SMS `[kod] connect IP:port`
sama łączy się przez GPRS z podanym serwerem
(RopamBridge albo komputer serwisowy z publicznym IP).
Manager ma do tego wbudowany serwer TCP.

To ten sam protokół co w punkcie B ✅,
tylko w innych ramkach i szyfrowany kluczem TCP/IP z konfiguracji
([tryb zdalny](protokol-serwisowy.md#tryb-zdalny)).
Wymaga włączonej w konfiguracji opcji zdalnego dostępu,
a po każdej sesji centrala się restartuje,
więc nadaje się do programowania, nie na stałą bramkę.

## D. Magistrala RopamNET (RS-485)

Na niej centrala rozmawia z panelami TPR i ekspanderami.
Ta droga daje najwięcej:
panel dotykowy potrafi uzbrajać, rozbrajać i pokazywać stan stref,
więc bramka udająca panel albo ekspander miałaby te same możliwości.

Plan:

1. Podsłuch pasywny:
   adapter USB-RS485 tylko do odbioru, wpięty równolegle do `A`, `B`, `GND`.
   Najpierw prędkość transmisji (analizator stanów logicznych),
   potem ramki, adresy, sumy kontrolne.
2. Korelacja z akcjami na panelu:
   naruszenie wejścia, uzbrojenie, dotknięcie panelu.
3. Dopiero potem nadawanie, na końcu docelowo z ESP32 z transceiverem 3,3 V
   (np. MAX3485).

Warunek: w instalacji jest panel TPR albo ekspander `-RN`,
inaczej na magistrali nie będzie czego słuchać.

## E. Bootloader PIC32

Program OptimaUpdate to przeróbka przykładowej aplikacji Microchipa
„PIC32 Bootloader Application” z noty AN1388
(ścieżka PDB: `Bootloader_Optima\Aplikacja do aktualizacji\...`).
Stąd wniosek, że w centrali pracuje ❓ **PIC32**.
Trzeba to potwierdzić na płycie.

Protokół bootloadera AN1388 jest publiczny,
ale standardowo **nie ma komendy odczytu flasha**,
więc nie zrzucimy nim firmware'u.

Firmware ma format `*.flsh`:
Intel HEX z danymi XOR-owanymi stałym kluczem.
Mamy odszyfrowane wersje 3.2–3.4,
patrz [`firmware.md`](firmware.md).
Wersja 4.1 (`optimagsm_v4.1Q.flsh_.zip`) była publikowana,
ale link jest martwy, a Wayback Machine nie ma kopii.

Jawny obraz firmware'u to najlepsze źródło wiedzy o wszystkich protokołach naraz:
porcie serwisowym, RopamNET i rozmowie z AP-IP.

## Rekomendowana kolejność

1. Analiza firmware'u w Ghidrze:
   znaleźć obsługę UART-ów (port serwisowy, RS-485, modem)
   i wyciągnąć z kodu format ramek.
2. Droga B przez Wine i `socat`: weryfikacja na żywo tego, co wyczytamy z kodu.
3. Droga D, pasywnie: tym docelowo gadałaby bramka ESP32.
4. Zdjęcie płyty: dokładny model PIC32, rewizja, złącza ICSP.
