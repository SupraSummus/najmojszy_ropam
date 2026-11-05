# Uwagi o bezpieczeństwie

Co warto wiedzieć,
zanim podłączy się centralę do sieci albo do własnej elektroniki.

Zasada: jeśli znajdziemy błąd,
który pozwala zaszkodzić cudzej instalacji
(np. zdalnie rozbroić centralę albo przejąć jej konfigurację),
najpierw zgłaszamy go producentowi,
a szczegóły publikujemy dopiero po czasie na poprawkę.

## Modbus TCP przez AP-IP

Serwer Modbus TCP w module AP-IP (port 502)
nie ma szyfrowania ani uwierzytelnienia
([`modbus-tcp.md`](modbus-tcp.md)).
Każdy w tej samej sieci może czytać stan wejść,
a przy włączonym sterowaniu przez WWW/IP także przełączać wyjścia.
Jeśli czuwanie jest sterowane wyjściem
([obejście producenta](modbus-tcp.md#ograniczenia)),
dotyczy to też uzbrajania i rozbrajania.

Port 502 powinien być dostępny tylko w zaufanej sieci,
bez przekierowania z internetu.
Gdy AP-IP pracuje jako punkt dostępowy Wi-Fi,
tę sieć chroni tylko hasło Wi-Fi,
więc warto ustawić własne, mocne.

## Zdalne programowanie przez GPRS

Polecenie SMS `connect` każe centrali połączyć się z podanym serwerem,
a przez to połączenie działają te same komendy co przez kabel
([tryb zdalny](protokol-serwisowy.md#tryb-zdalny)),
także zapis konfiguracji ❓.
Kod uprawniający do tego polecenia
chroni więc całą konfigurację.

## Magistrala RopamNET

Ramki na magistrali RS-485 nie są szyfrowane ani podpisane 🔌.
Kto ma dostęp do przewodów magistrali,
może podsłuchać stan systemu, który centrala rozgłasza co 0,56 s 🔌,
a także kody wpisywane na panelach,
bo w żądaniach panelu idą one jako zwykły napis ✅
(na magistrali jeszcze niewidziane ❓).
Funkcje panelu działają bez kodu 🔌,
więc urządzenie podające się za panel mogłoby je wywołać ❓.

Dlatego magistralę najlepiej prowadzić w obszarze chronionym.
Panel albo moduł poza nim (np. przy furtce czy w garażu)
daje dostęp do przewodów,
a funkcją takiego panelu nie warto sterować niczego ważnego.
To samo dotyczy bramki podłączonej do magistrali:
jej dostęp przez Wi-Fi trzeba chronić tak jak same przewody.
