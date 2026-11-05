# Programowanie centrali

Jak zmieniać konfigurację centrali z Linuksa w miarę bezpiecznie.
Format: [`konfiguracja.md`](konfiguracja.md),
komendy: [`protokol-serwisowy.md`](protokol-serwisowy.md).
Oznaczenia: ✅ z kodu, 🔌 sprawdzone, ❓ przypuszczenie.

## Narzędzia

```sh
tools/ropam_config.py PLIK pola                        # co umiemy zmieniać
tools/ropam_config.py PLIK ustaw WYNIK wejscie.2.nazwa=Kuchnia   # offline
tools/ropam_config.py PLIK roznica INNY

tools/ropam_serwis.py PORT zmien HASŁO wejscie.2.nazwa=Kuchnia   # na sucho
ROPAM_ZAPIS=1 tools/ropam_serwis.py PORT zmien HASŁO wejscie.2.nazwa=Kuchnia
ROPAM_ZAPIS=1 tools/ropam_serwis.py PORT przywroc HASŁO private/kopie/….bin
```

`zmien` czyta konfigurację z centrali, zapisuje kopię w `private/kopie/`
i pokazuje różnicę.
Zapisuje dopiero z `ROPAM_ZAPIS=1` i po wpisaniu „tak”:
małą zmianę łatką `0x30`, dużą całością `0x02`.
Potem czyta konfigurację z powrotem i porównuje,
a na koniec ostrzega, jeśli zmieniły się włączone wyjścia.
`przywroc` wgrywa kopię w całości, także identyczną z tą w centrali.
Oba obsługują tylko format firmware'u 3.4
(rozmiar zapisany na początku bloba).

Testy na udawanej centrali: `python3 tools/test_udawana_centrala.py`.

## Co robi firmware przy zapisie

`0x02`, cała konfiguracja ✅:

- Blob trafia do obrazu konfiguracji w RAM,
  a do pliku tylko wtedy, gdy zgadza się CRC.
  Zły albo przerwany transfer niczego nie zmienia;
  centrala po ok. 1 s odsyła status 0.
- Najpierw `config.rpm`, potem kopia `cfgcpy.rpm`;
  przy starcie zepsuty `config.rpm` jest odtwarzany z kopii.
- Nowa konfiguracja działa od razu, bez restartu.
- Zapis nie wymaga hasła komunikacji.
- Po statusie 1 centrala może odesłać jeszcze dwa statusy:
  po wysłaniu ustawień panelom TPR (gniazda 1–4)
  i modułowi z gniazda 8.

`0x30`, łatka ✅ (sprawdzone w asemblerze, dekompilacja Ghidry myli):

- Pakiet: `0x30`, długość danych (u16), CRC całego obrazu po łatce (u16).
- Dane: trójki (przesunięcie od `0x196` u16, wartość), mniej niż 199 B.
  Nagłówka i katalogu sekcji łatka nie zmieni.
- Łatka trafia do obrazu w RAM z ostatniego odczytu `0x03`.
  Gdy coś go w międzyczasie zmieniło, CRC się nie zgodzi.
  Obraz zepsuty odrzuconą łatką naprawia dopiero następny `0x03`.

Skutek uboczny 🔌:
po zapisie firmware przepisuje stan wyjść z ustawień,
ale programu LogicProcessora nie uruchamia od nowa.
Wyjście, które program włącza tylko przy starcie, zostaje wyłączone,
nawet gdy konfiguracja się nie zmieniła.
Pomaga zwykłe włączenie (panel, SMS) albo restart centrali (`0x2F`).

Szyfrowanie portu serwisowego włącza tylko komenda `0x42`
w trybie serwisowym, nie konfiguracja ✅,
więc zapis nie odetnie nam portu.

Gdy coś pójdzie źle:
`przywroc` z kopią,
potem Manager z kopią `.rpm`,
w ostateczności reset fabryczny zworką X3 (instrukcja, rozdz. 3.12)
i wgranie kopii.

## Jak zapisuje OptimaGSM Manager

Podejrzane na udawanej centrali 🔌:

- Po odczycie wysyła łatkę `0x30` z różnicami.
- Nie zapisuje tego, co przeczytał, bajt w bajt:
  przy okazji „porządkuje” niektóre pola,
  np. w swojej domyślnej konfiguracji dopisał `\r\n\r\n` w `NAPIS`
  i zmienił maski w `M_TPR1` z `1f` na `0f`.
- W nagłówku wpisuje swoją wersję (bajt 4: 25 dla 2.5)
  i liczy CRC z tym nagłówkiem,
  choć łatka nagłówka nie przenosi.
  Pierwsza łatka do konfiguracji zapisanej inną wersją Managera
  zostaje więc odrzucona, a następny zapis idzie całością `0x02`.
- Nowa konfiguracja („Plik → Nowy”):
  hasło komunikacji `111111`,
  zdalny dostęp TCP/IP włączony (flagi `COMM` = `0x03`),
  losowy klucz TCP/IP ❓.
  Po wcześniejszym odczycie „Nowy” zostawia w modelu część danych z centrali.

## Plan

1. Udawana centrala i narzędzie zapisu: są.
2. Zapis bez zmian na prawdziwej centrali: zrobiony 🔌.
3. Pierwsza prawdziwa zmiana: nazwa wejścia, potem powrót.
4. Mapa kolejnych pól metodą różnicową w Managerze,
   na konfiguracji domyślnej, nie prawdziwej.

Zapisy na prawdziwej centrali tylko za zgodą właściciela.

## Procedura zapisu

1. Strefy rozbrojone, ktoś na miejscu, kopia `.rpm` pod ręką.
2. `zmien` albo `przywroc` na sucho, przegląd różnicy.
3. Zapis z `ROPAM_ZAPIS=1`; odczyt sprawdzający musi się zgadzać.
4. Wyjścia jak przed zapisem (narzędzie ostrzeże),
   w dzienniku bez zdarzenia `0x2E` (błąd konfiguracji ❓).

## Udawana centrala

[`tools/udawana_centrala.py`](../tools/udawana_centrala.py)
odpowiada na port serwisowy jak firmware 3.4,
z obrazem konfiguracji w RAM i „plikiem”.
Manager z nią rozmawia przez
[`tools/wine_udawana.sh`](../tools/wine_udawana.sh):

```sh
Xvfb :99 &
DISPLAY=:99 WINEPREFIX=~/kopia-prefiksu \
    tools/wine_udawana.sh KONFIGURACJA.bin private/udawana
```

Pseudoterminal nie obsługuje ioctl-i linii modemu,
bez których Wine nie otworzy COM-a;
[`tools/ttysniff`](../tools/ttysniff/ttysniff.c) je udaje.
Blokuje też otwieranie `/dev/ttyUSB*` i `/dev/ttyACM*`,
bo Wine zakłada dowiązania COMn także do kabla prawdziwej centrali.
Skrypt zmienia COM1 w prefiksie, więc lepiej dać mu kopię.
