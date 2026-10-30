# Konfiguracja centrali

Format bloba `config.rpm`, który centrala trzyma w pamięci
i wydaje komendą serwisową `0x03`
([`protokol-serwisowy.md`](protokol-serwisowy.md)).
Odczytany z firmware'u OptimaGSM v3.4,
z porównaniem z instrukcją instalacji i listami z OptimaGSM Managera.
Oznaczenia: ✅ wynika wprost z kodu, 📖 z instrukcji albo Managera,
❓ wniosek albo przypuszczenie.

Podgląd: `tools/ropam_config.py PLIK sekcje|wejscia|wyjscia|strefy|zrzut NAZWA`.

## Kontener

| Przesunięcie | Zawartość |
|---|---|
| `0x0000` | rozmiar (u16, `0xCCBC`), dwa bajty ❓, wersja formatu (u8), wersja firmware'u × 10 (u8), … |
| `0x000E` | katalog: 28 wpisów po 14 B (nazwa 8 B, przesunięcie u16, rozmiar u16, 2 B ❓) |
| `0x0196` | dane sekcji (przesunięcia w katalogu liczone od tego miejsca) |
| `0xCCB6` | liczba sekcji |
| `0xCCB8` | CRC-16/XMODEM pierwszych `0xCCB8` bajtów (u16 little-endian) |

Sekcje w v3.4 (kolejność i rozmiary takie same w każdej centrali z tą wersją ❓):

| Sekcja | Rozmiar | Zawartość |
|---|---|---|
| `INPUT` | 8064 | 48 wejść × 168 B |
| `OUT` | 7168 | 32 wyjścia × 224 B |
| `TEST` | 44 | test transmisji ❓ |
| `TIMES` | 14 | czasy globalne ❓ |
| `STREFY` | 176 | 4 strefy × 44 B |
| `COUNT` | 18 | liczniki ❓ |
| `SMTP` | 174 | serwer poczty |
| `COMM` | 41 | komunikacja, m.in. hasło komunikacji (bajty 1–6) ✅ |
| `OPT` | 260 | opcje systemowe ❓ |
| `GPRSCFG` | 176 | GPRS, monitoring |
| `TIMR` | 648 | timery |
| `TEL` | 136 | numery telefonów |
| `EMAIL` | 248 | adresy e-mail |
| `NAPIS` | 1612 | teksty komunikatów ❓ |
| `DEV` | 200 | ❓ |
| `ANINP` | 224 | wejście analogowe |
| `WIRE` | 1152 | czujniki 1-Wire / temperatury ❓ |
| `LOGBIN` | 11936 | skompilowany program LogicProcessor ❓ |
| `LOGTXT` | 4096 | źródło programu LogicProcessor ❓ |
| `M_TPR1`–`M_TPR4` | 4 × 2920 | ustawienia paneli TPR, wysyłane do paneli magistralą ✅ |
| `M_RF4` | 298 | moduł pilotów RF-4 |
| `EVNT` | 1280 | ❓ |
| `APCFG` | 362 | moduł AP-IP |
| `VAR1` | 68 | bramka domofonowa VAR-1 |
| `WIFI` | 87 | Wi-Fi (AP-IP) |

## Wejście (`INPUT`, 168 B)

| Przesunięcie | Typ | Zawartość |
|---|---|---|
| `0x00` | tekst | nazwa (do 22 B z zerem; za zerem mogą zostać resztki starszych nazw) |
| `0x16` | u16 | typ wejścia, tabela niżej ✅ |
| `0x18` | u32 | czas reakcji w ms 📖 (min. 250) |
| `0x1C`, `0x1E` | u16 | wartości w minutach ❓ |
| `0x20` | u16 | blokada po naruszeniu w minutach ✅ 📖 |
| `0x22` | u16 | czas w sekundach ❓ |
| `0x24` | u32 | flagi: bity 0–3 = przynależność do stref 1–4 ✅, bit 4 = linia nocna ✅, bity 5–8 ❓ |
| `0x28` | u8 | polaryzacja (5 bitów) ❓ (NO, NC, EOL, 2EOL/NO, 2EOL/NC, wyłączone) |
| `0x34` | tekst | treść SMS przy naruszeniu (do 20 znaków) 📖 ❓ |

Typy wejść (wg gałęzi kodu i zapisywanych zdarzeń ✅, nazwy 📖):

| Wartość | Typ | Zdarzenie w dzienniku |
|---|---|---|
| 0 | ZWYKŁA CICHA | „Alarm z linii cichej” |
| 1 | 24h | „Alarm z linii 24h” |
| 2 | ZWYKŁA | „Alarm z linii” |
| 3 | INFO | „Naruszenie linii info” |
| 4 | ZAŁ/WYŁ CZUWANIE | uzbrojenie/rozbrojenie stref wejścia |
| 5 | OPÓŹNIONA | „Naruszenie linii opóźnionej” |
| 6 | OPÓŹNIONA WAR. | „Naruszenie linii op.war.” |
| 7 | LICZNIKOWA | „Alarm z linii licznikowej” |
| 8 | KASUJĄCA ALARM | „Skasowanie alarmu wejściem” |
| 9 | ZAŁ. CZUWANIE | ❓ |
| 10 | WYŁ. CZUWANIE | ❓ |

## Wyjście (`OUT`, 224 B)

| Przesunięcie | Typ | Zawartość |
|---|---|---|
| `0x00` | tekst | nazwa |
| `0x18` | u32 | maska źródeł wyzwolenia ❓, w kolejności listy Managera: Alarm, Sabotaż, Czuwanie pełne, Czuwanie noc, Czas wejścia, Czas wyjścia, Potwierdzenie pulsami, Logic processor, Awaria zbiorcza, Brak AC, SMS, CLIP, Kod DTMF, WWW, Timer 1–4 📖 |
| `0x1C` | u16 | czas działania w sekundach ❓ |
| `0x20`, `0x35` | tekst | dwa krótkie teksty, najpewniej komendy SMS włączenia i wyłączenia ❓ |

## Strefa (`STREFY`, 44 B)

| Przesunięcie | Typ | Zawartość |
|---|---|---|
| `0x00` | tekst | nazwa |
| `0x18` | u32 | czas na wejście w sekundach ❓ |
| `0x1C` | u32 | czas na wyjście w sekundach ✅ (`zone_arm` mnoży go przez 1000) |
