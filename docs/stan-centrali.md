# Stan centrali na żywo (komenda `0x24`)

Odczytany z firmware'u OptimaGSM v3.4 (`svc_dispatch`, `status_header`)
i sprawdzony na sprzęcie.
Oznaczenia: ✅ wynika wprost z kodu, 🔌 sprawdzone na sprzęcie,
❓ wniosek albo przypuszczenie.

Klient: `tools/ropam_serwis.py PORT stan [PLIK]`.

## Sesja serwisowa

Każda obsłużona komenda protokołu serwisowego
(także `0x01`, `0x03` i `0x24`)
ustawia w centrali flagę sesji serwisowej (bit 1 zmiennej `g_sys_flags`) ✅.
Flaga gaśnie po trzech kolejnych pustych cyklach nasłuchu,
a każdy trwa do 16 × 2 s,
czyli ok. 1,5 minuty po ostatniej komendzie ✅.

Póki flaga jest ustawiona, centrala zachowuje się inaczej ✅, m.in.:

- pomija zdarzenia zmian stref z modułu na złączu EXP,
- wstrzymuje akcję alarmową jednego z modułów,
- odpytuje koncentrator magistrali w trybie podglądu.

To jest „tryb ON-LINE” z instrukcji, w którym centrala „nie wykonuje swoich standardowych funkcji”.

### Zakończenie sesji bez czekania

Centrala nie wykrywa, czy kabel jest wpięty;
o trybie ON-LINE decyduje tylko ta flaga,
więc do powrotu do normalnej pracy nie trzeba nic odłączać.
Flagę można zgasić od razu ✅ 🔌:
nieudany odbiór kończy się natychmiast,
gdy nadawca przerwie go dwoma `CAN` (centrala odpowiada `ACK`
i od razu wysyła następne zaproszenie `C`),
więc trzy razy `C` → `CAN CAN` → `ACK` to trzy puste cykle w ok. 35 ms.
Restart (`0x2F`), którym kończy sesję Manager, nie jest do tego potrzebny.

Sprawdzone na sprzęcie przez pole `0x0CE` odpowiedzi `0x24`
(odpowiedź pokazuje flagę sprzed obsługi tej komendy):
po `0x01` flaga jest ustawiona, po `0x01` i trzech przerwaniach zgaszona.
`tools/ropam_serwis.py` robi to sam po każdej komendzie
(polecenie `koniec` robi tylko to).

Zapis konfiguracji (`0x02`) też nie wymaga restartu:
centrala od razu wczytuje nową konfigurację,
przelicza wejścia i rozsyła ustawienia panelom TPR ✅.
Czy wszystkie ustawienia (np. modemu) działają bez restartu, nie wiemy ❓.
**Wniosek dla bramki:** stałe odpytywanie `0x24` trzymałoby centralę w sesji serwisowej.
Do okazjonalnego podglądu się nada,
ale stała integracja powinna iść inną drogą (magistrala RopamNET albo AP-IP).

## Komendy `0x1F` i `0x2F`: restart

`0x2F` odsyła status 1, a potem wywołuje `sys_restart`,
który kończy się `SoftReset()`: odblokowanie `SYSKEY` i zapis `RSWRST` ✅.
`0x1F` robi to samo bez statusu,
ale wcześniej wpisuje 1 pod `0xA001FFF0`, najpewniej dla bootloadera ❓.
(Wcześniej opisywaliśmy tu `0x26`, błędnie: to polecenie dla modułu FGR-4.)
OptimaGSM Manager wysyła `0x2F` przy każdym rozłączeniu kablem 🔌,
więc **każde rozłączenie Managera restartuje centralę**.
Nasz klient tych komend nie wysyła.

## Odpowiedź na `0x24`

1100 B (`0x44C`) w jednej transmisji XMODEM (bloki 1024 B) 🔌.
Jeśli w konfiguracji ustawiony jest odpowiedni bit,
po niej przychodzi druga transmisja z odpowiedzią modemu na komendę AT ✅.

### Nagłówek (`0x000`–`0x0DB`, `status_header`)

| Przesunięcie | Rozmiar | Zawartość |
|---|---|---|
| `0x000` | 8 | maska wejść 1–48 w stanie 1 (naruszenie ❓) ✅ |
| `0x008` | 8 | maska wejść 1–48 w stanie 3 (sabotaż ❓) ✅ |
| `0x010` | 8 | maska wejść z flagami bitów 6, 7, 11 opisu wejścia ❓ |
| `0x018` | 8 | maska wejść z flagą bitu 12 opisu wejścia ❓ |
| `0x020` | 16 | dwa pola po 8 B (`0xA00010A0`, `0xA00010A8`) ❓ |
| `0x030` | 4 | maska włączonych wyjść 1–32 ✅ |
| `0x034` | 4 | maska modułów IO/IOE koncentratora ❓ |
| `0x038` | 8 | dwie wartości 32-bitowe ❓ |
| `0x040` | 2 | wartość 16-bitowa ❓ |
| `0x044` | 8 | struktura chroniona muteksem ❓ |
| `0x04C` | 32 | stan stref: 4 × 8 B; młodsze 4 bity = stan (3 = rozbrojona 🔌, 5 = odliczanie wyjścia ❓), bit 10 = czuwanie, bit 11 = noc ✅ |
| `0x06C` | 12 | ❓ |
| `0x078` | 64 | ❓ |
| `0x0B8` | 16 | flagi systemowe, w tym tryb lokalny/zdalny ✅ |
| `0x0C8` | 8 | kopia RAM-u od `0xA0000154`: bajty 0–3 zmieniają się przy każdym odczycie (licznik ❓), pod `0x0CE` u16 `g_sys_flags` (bit 1 = sesja serwisowa) ✅ 🔌 |
| `0x0D0` | 8 | czas ❓ |
| `0x0D8` | 4 | ❓ |

Maski są little-endian, bit 0 to wejście (wyjście) 1.
Stan wejścia to bajt 2 czterobajtowego wpisu w tablicy stanów,
chronionej muteksem ✅.

### Dalsza część

| Przesunięcie | Rozmiar | Zawartość |
|---|---|---|
| `0x0DC` | 96 | 48 × u16: wartość linii każdego wejścia (Manager pokazuje opór w kΩ) ❓ |
| `0x13C` | 198 | tablica modułów magistrali, 18 × 11 B ✅ |
| `0x202` | zmienny | kolejne struktury o długościach zależnych od konfiguracji ✅ |

Kolejność struktur po tablicy modułów:
bufor `0xA0000280` (długość z konfiguracji),
18 B z `0xA0001EB4`,
bufor `0xA0001284` (długość z konfiguracji),
48 B z `0xA0001EC8`,
stan koncentratora `0xA0001EF8` (długość z konfiguracji),
2 B stanu modułu na złączu EXP,
64 B z `0xA0001234` ✅.

### Obserwacje

Na podstawie 183 kolejnych odczytów wykonanych przez Managera 🔌:

- maska `0x000` zmienia się, gdy czujki widzą ruch,
- pola `0x0C8`–`0x0D5` zmieniają się niemal przy każdym odczycie,
- wartości linii wejść (`0x0DC` …) wahają się,
- wejścia nieużywane (z domyślną nazwą i bez strefy) mają stały stan,
  który maski pokazują jako stan 1 albo 3 🔌,
- połączenie z konfiguracją daje czytelny podgląd:
  `tools/ropam_config.py KONFIGURACJA stan PLIK_STANU`.
