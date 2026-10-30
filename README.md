# najmojszy_ropam

Próba pełnego zrozumienia centrali alarmowej **Ropam OptimaGSM**
i uniezależnienia się od windowsowego oprogramowania producenta.

Docelowo:

1. obsługa centrali z Linuksa
   (odczyt stanu, sterowanie, a może i konfiguracja),
2. bramka Wi-Fi na ESP32,
   którą da się wpiąć np. w Home Assistanta.

## Stan

Etap rozpoznania.
Co wiemy, a czego nie, opisują pliki w [`docs/`](docs/):

- [`docs/sprzet.md`](docs/sprzet.md):
  złącza, interfejsy i warstwa fizyczna,
- [`docs/drogi-integracji.md`](docs/drogi-integracji.md):
  możliwe sposoby rozmowy z centralą i ich ocena,
- [`docs/modbus-tcp.md`](docs/modbus-tcp.md):
  jedyny protokół, który Ropam opisał publicznie,
- [`docs/firmware.md`](docs/firmware.md):
  format plików `*.flsh` i co jest w odszyfrowanym firmware'ze,
- [`docs/protokol-ramek.md`](docs/protokol-ramek.md):
  format ramek RopamNET odczytany z firmware'u,
- [`docs/protokol-serwisowy.md`](docs/protokol-serwisowy.md):
  XMODEM-owy protokół programowania przez gniazdo RJ12,
- [`docs/uwagi-bezpieczenstwa.md`](docs/uwagi-bezpieczenstwa.md):
  słabości widoczne w dokumentacji i firmware'ze,
- [`docs/zrodla.md`](docs/zrodla.md):
  skąd to wszystko wiemy.

Zrozumiany kod firmware'u (nazwy, komentarze, czytelny C) jest w [`re/`](re/).

Konwencje projektu są w [`CLAUDE.md`](CLAUDE.md).
