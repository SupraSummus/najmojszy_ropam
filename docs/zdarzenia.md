# Dziennik zdarzeń

Odczytany z firmware'u OptimaGSM v3.4 i sprawdzony na sprzęcie.
Oznaczenia: ✅ wynika wprost z kodu, 🔌 sprawdzone na sprzęcie,
❓ wniosek albo przypuszczenie.

Klient: `tools/ropam_serwis.py PORT dziennik [PLIK]`.

## Przechowywanie

Dziennik to pierścień 10 000 rekordów po 16 B we flashu ✅.
Licznik rekordów i znacznik zawinięcia pierścienia są w RAM-ie (`0xA0000118`).

Rekord (`event_log_ex`) ✅ 🔌:

| Bajt | Treść |
|---|---|
| 0–5 | czas: rok − 2000, miesiąc, dzień, godzina, minuta, sekunda |
| 6 | kod zdarzenia (tabela niżej) |
| 7 | nieustawiany |
| 8 | argument (np. numer wejścia, numer gniazda modułu) |
| 9 | drugi argument |
| 10–15 | nieustawiane: zostają tam przypadkowe bajty ze stosu |

## Odczyt: komenda serwisowa `0x04`

Pakiet komendy: `0x04` i jeden bajt argumentu ✅ 🔌.

1. Centrala wysyła liczbę rekordów (4 B, little-endian),
   albo 10 000, gdy pierścień się zawinął.
2. Potem osobnymi transmisjami XMODEM paczki po 100 rekordów (1600 B),
   od pierwszej do tej, która zawiera ostatni rekord włącznie;
   ostatnia paczka jest zawsze pełna, więc jej końcówka to nieważne rekordy.

Argument 1 oznacza cały dziennik (do 100 paczek, ok. 160 KB),
każda inna wartość tylko ostatnie 1000 rekordów (11 paczek).

**Komenda `0x05` kasuje cały dziennik**,
a potem uruchamia test wyjść: po kolei włącza wyjścia i sygnalizację ✅.
Klient jej nie ma i nie należy jej wysyłać do działającej instalacji.

## Kody zdarzeń

Teksty pochodzą wprost z firmware'u,
z tablicy par (kod, wskaźnik na tekst) pod `0x9D049064` (`event_texts`) ✅.
Pisownia jak w firmware'ze, zwykle bez polskich znaków.
Kolumnę argumentu wypełniamy na podstawie miejsc,
w których firmware zapisuje dane zdarzenie.

Uwagi do tablicy:
`0x17` i `0x18` mają ten sam tekst,
kod `0x6F` występuje dwa razy (drugi raz z „(E)”, najpewniej błąd w tablicy zamiast `0x7E`),
a `0x7A` ma pusty tekst.
Zdarzenia z tekstem kończącym się na „z” (`0x75`, `0x76`)
centrala uzupełnia nazwą modułu z `g_module_names`
([`protokol-ramek.md`](protokol-ramek.md#gniazda-modułów)).

| Kod | Tekst | Argument |
|---|---|---|
| `0x01` | Alarm z linii | numer wejścia ❓ |
| `0x02` | Koniec naruszenia linii |  |
| `0x03` | Zalaczenie czuwania | ❓ |
| `0x04` | Wylaczenie czuw./alarmu | ❓ |
| `0x05` | Alarm z linii 24h |  |
| `0x07` | Naruszenie linii info |  |
| `0x06` | Alarm z linii cichej |  |
| `0x08` | Alarm z linii licznikowej |  |
| `0x09` | Alarm po czasie na wejscie |  |
| `0x0A` | Skasowanie alarmu wejsciem |  |
| `0x0B` | Naruszenie linii opoznionej | numer wejścia |
| `0x0C` | Naruszenie linii op.war. | numer wejścia |
| `0x0D` | Zablokowanie wejsc |  |
| `0x0E` | Odblokowanie wejsc |  |
| `0x0F` | Blokada wejscia po 3 alarmach |  |
| `0x10` | Sabotaż wejscia | numer wejścia |
| `0x11` | Koniec sabotazu wejscia | numer wejścia |
| `0x12` | Awaria wejscia |  |
| `0x13` | Koniec awarii wejscia |  |
| `0x14` | Zalaczenie wyjscia |  |
| `0x15` | Wylaczenie wyjscia |  |
| `0x16` | Zapytanie o status systemu |  |
| `0x17` | Reset zegara transmisji |  |
| `0x18` | Reset zegara transmisji |  |
| `0x19` | Zmiana numeru email |  |
| `0x1A` | Test transmisji |  |
| `0x1B` | Programowanie | 0 |
| `0x1C` | Tryb serwisowy | 1 = włączenie, 0 = wyłączenie |
| `0x1D` | Zmiana konfiguracji poprzez sms |  |
| `0x1E` | Zmiana kodu dostepu sms |  |
| `0x1F` | Niski poziom sieci | poziom sygnału ❓ |
| `0x20` | Poziom sieci ok | poziom sygnału ❓ |
| `0x21` | Modem nie zarejestrowany w sieci |  |
| `0x22` | Modem zarejestrowany w sieci |  |
| `0x23` | GPRS |  |
| `0x24` | Zaklocanie sygnalu GSM |  |
| `0x25` | Restart centrali: | przyczyna restartu ❓ |
| `0x26` | Calkowita utrata zasilania |  |
| `0x27` | Blad zegara centrali |  |
| `0x28` | Zegar centrali ustawiony |  |
| `0x29` | Zasilanie AC230V | 1 = brak, 0 = powrót |
| `0x2A` | Akumulator rozladowany |  |
| `0x2B` | Akumulator | 1 = usterka, 0 = koniec |
| `0x2C` | Niskie napiecie zasilania <11V |  |
| `0x2D` | Napiecie zasilania poprawne |  |
| `0x2E` | Blad ustawien |  |
| `0x2F` | Odtworzenie kopii ustawien |  |
| `0x30` | Przeciazenie wyjscia OUT1 |  |
| `0x31` | Wyjscie OUT1 ok |  |
| `0x34` | Przeciazenie wyjscia O2 |  |
| `0x35` | Wyjście O2 ok |  |
| `0x32` | Obciazenie wyjscia O1 |  |
| `0x33` | Obciazenie wyjscia O2 |  |
| `0x36` | Przeciazenie wyjscia KBD |  |
| `0x37` | Przeciazenie wyjscia AUX |  |
| `0x38` | Zasilanie czujek temp. |  |
| `0x39` | Awaria zasilania modemu |  |
| `0x3A` | Nieudane wyslanie smsa |  |
| `0x3B` | Problem z wyslaniem 3 sms |  |
| `0x3C` | Wyslanie sms |  |
| `0x3E` | MMS wyslany |  |
| `0x3F` | Blad wyslania MMSa |  |
| `0x40` | E-mail wyslany |  |
| `0x41` | Blad wyslania e-mail |  |
| `0x3D` | Nieudane polaczenie glosowe |  |
| `0x42` | Odebranie pol. alarmowego |  |
| `0x43` | Zakonczenie powiadomienia poprzez # |  |
| `0x44` | Polaczenie z zewnatrz |  |
| `0x45` | Polaczenie przychodzace odebrane |  |
| `0x46` | Polaczenie przychodzace odrzucone |  |
| `0x47` | Oddzwonienie zwrotne na numer |  |
| `0x48` | Nierozpoznay sms odeslany (echo) |  |
| `0x49` | Utrata polaczenia z modemem |  |
| `0x4A` | Powrót polaczenia z modemem |  |
| `0x4B` | Restart modemu |  |
| `0x4C` | Brak lub blad karty SIM |  |
| `0x4D` | Blad kodu PIN |  |
| `0x4E` | Blokada wysylania sms |  |
| `0x4F` | Blokada wysylania awarii |  |
| `0x50` | Blokada wysylania MMS |  |
| `0x51` | Blokada wysylania e-mail |  |
| `0x52` | Blokada dzwonienia |  |
| `0x53` | Utrata sygnalu wizji |  |
| `0x54` | Powrót sygnalu wizji |  |
| `0x55` | Polecenie wykonania zdjec |  |
| `0x56` | Wykryty ruch |  |
| `0x57` | Czujnik temp. |  |
| `0x58` | Temperatura HiA |  |
| `0x59` | Temperatura LoA |  |
| `0x5A` | Temperatura HiB |  |
| `0x5B` | Temperatura LoB |  |
| `0x5D` | Alarm przyrostu temp. |  |
| `0x5C` | Temperatura | ❓ (co 6 h i po starcie) |
| `0x5E` | Wartosc wejscia AI niska |  |
| `0x5F` | Wartosc wejscia AI wysoka |  |
| `0x60` | Alarm przyrostu analog |  |
| `0x61` | Wartosc analogowa |  |
| `0x62` | Utworzenie kodu uzytkownika |  |
| `0x63` | Usuniecie kodu uzytkownika |  |
| `0x64` | Zmiana kodu uzytkownika |  |
| `0x65` | Trzy bledne hasla |  |
| `0x66` | Nieudane polacznie z serwerem |  |
| `0x67` | Nieudane wyslanie 3 pakietów |  |
| `0x68` | Udane polaczenie z serwerem |  |
| `0x69` | Utrata polaczenia z serwerem |  |
| `0x6A` | Slaba bateria pilota |  |
| `0x6B` | Przycisk panik pilota |  |
| `0x6C` | Nacisniecie pilota (A) |  |
| `0x6D` | Nacisniecie pilota (B) |  |
| `0x6E` | Nacisniecie pilota (C) |  |
| `0x6F` | Nacisniecie pilota (D) |  |
| `0x70` | Wyzwolenie przycisku domofonu |  |
| `0x71` | Brak pol. z urzadz. Aero | numer wejścia |
| `0x72` | Powrot pol. z urzadz. Aero | numer wejścia |
| `0x73` | Slaba bateria urzadz. Aero | numer wejścia |
| `0x74` | Bateria urzadz. Aero ok | numer wejścia |
| `0x75` | Utrata polaczenia z | numer gniazda |
| `0x76` | Powrot polaczenia z | numer gniazda |
| `0x77` | Wylacznik sab. otwarty | numer gniazda |
| `0x78` | Wylacznik sab. zamkniety | numer gniazda |
| `0x79` | Blad wykonania skryptu LP |  |
| `0x7A` | (pusty) |  |
| `0x7B` | Proba dostepu IP |  |
| `0x7C` | 3 bledy logowania |  |
| `0x7D` | IP zalogowano |  |
| `0x6F` | Nacisniecie pilota (E) |  |
| `0x7F` | Zagluszanie systemu Aero |  |
| `0x80` | Niskie napiecie ekaspandera | drugi argument: numer gniazda |
| `0x81` | Awaria aux ekspandera | drugi argument: numer gniazda |
| `0x82` | Restart ekspadera | numer gniazda |
