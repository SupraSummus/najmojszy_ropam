# Uwagi o bezpieczeństwie

Słabości widoczne w dokumentacji i firmware'ze OptimaGSM.
Opisujemy je rzeczowo, żeby posiadacze central wiedzieli, na czym stoją.

Zasada: jeśli znajdziemy błąd,
który pozwala zaszkodzić cudzej instalacji
(np. zdalnie rozbroić centralę albo przejąć jej konfigurację),
najpierw zgłaszamy go producentowi,
a szczegóły publikujemy dopiero po czasie na poprawkę.

## Firmware „chroniony” stałym kluczem XOR

Pliki aktualizacji są XOR-owane jednym 16-bajtowym kluczem,
takim samym we wszystkich znanych wersjach
([`firmware.md`](firmware.md)).
To nie jest szyfrowanie ani podpis:
klucz da się odzyskać statystyką w kilkanaście minut.
Nie chroni to ani przed analizą, ani przed wgraniem zmodyfikowanego firmware'u,
a jedynie utrudnia właścicielowi sprawdzenie, co robi jego urządzenie.

Nie wiadomo jeszcze, czy bootloader w jakikolwiek sposób weryfikuje pochodzenie firmware'u. ❓

## Modbus TCP bez autoryzacji

Moduł AP-IP wystawia Modbus TCP na porcie 502
bez szyfrowania i bez uwierzytelnienia
([`modbus-tcp.md`](modbus-tcp.md)).
Każdy w tej samej sieci może czytać stan wejść
i, przy włączonym sterowaniu przez WWW/IP, przełączać wyjścia.
Producent sam to przyznaje i dlatego nie udostępnia przez Modbus czuwania,
ale zalecane obejście (sterowanie czuwaniem przez wyjście)
przenosi tę samą lukę na uzbrajanie i rozbrajanie.

W domyślnym trybie Wi-Fi AP-IP działa jako punkt dostępowy (`192.168.10.1`).
Hasło sieci i jej domyślne ustawienia: do sprawdzenia. ❓

## Tryb ON-LINE wyłącza powiadomienia

Według instrukcji instalacji w trybie ON-LINE (połączenie serwisowe)
centrala nie wykonuje swoich standardowych funkcji,
np. nie wysyła SMS-ów przy naruszeniu wejść.
Nie wiadomo, czy centrala sygnalizuje ten stan w jakikolwiek widoczny sposób,
ani czy da się go włączyć zdalnie (przez GPRS). ❓

## Zapis konfiguracji bez kodu serwisowego

W protokole serwisowym ([`protokol-serwisowy.md`](protokol-serwisowy.md))
odczyt konfiguracji wymaga 6-znakowego kodu serwisowego,
ale w kodzie obsługi zapisu (komenda `0x02`) nie widać żadnej kontroli kodu,
tylko kontrolę rozmiaru i sumy CRC.
Jeśli to się potwierdzi, ktoś z fizycznym dostępem do gniazda RJ12
(wewnątrz obudowy, zwykle chronionej czujnikiem sabotażu)
może nadpisać całą konfigurację, łącznie z kodami użytkowników,
bez znajomości kodu serwisowego. ❓

## Zdalne programowanie przez GPRS

Od firmware'u 1.7 komenda SMS `[kod] connect IP:port`
każe centrali połączyć się z dowolnym serwerem
i udostępnić mu programowanie.
Bezpieczeństwo zależy więc od tajności kodu SMS
i od tego, czy sesja jest dodatkowo uwierzytelniana albo szyfrowana. ❓
