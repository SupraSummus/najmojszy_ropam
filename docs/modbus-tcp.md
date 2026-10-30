# Modbus TCP przez moduł AP-IP

Jedyny protokół integracji, który Ropam opisał publicznie.
Źródło: nota aplikacyjna „MODBUS TCP/IP – protokół integracji z centralą”,
wersja 1.0RH z 10.11.2016.

Wymaga modułu **AP-IP** (Ethernet/Wi-Fi) podłączonego do centrali.
Bez niego ta droga nie istnieje.

## Wymagania

- OptimaGSM z firmware'em od v2.4,
- AP-IP z firmware'em od v1.5,
- serwer Modbus włączony w AP-IP w OptimaGSM Managerze,
- zapis wyjść wymaga opcji „sterowanie przez WWW/IP”.

## Połączenie

- Modbus TCP, slave, port 502.
- AP-IP po Ethernecie: domyślnie klient DHCP.
- AP-IP po Wi-Fi: domyślnie access point z DHCP, adres `192.168.10.1`;
  w trybie klienta adres z routera.
- Brak szyfrowania i brak autoryzacji.

Obsługiwane funkcje:
`01` Read Coils,
`02` Read Discrete Inputs,
`04` Read Input Registers,
`05` Write Single Coil.

## Mapa danych

### Discrete inputs (odczyt)

| Adres | Znaczenie |
|---|---|
| 0–47 | stan fizyczny wejść I1–I48 (z czasem naruszenia i powrotu 250 ms) |

### Coils (odczyt i zapis)

| Adres | Znaczenie |
|---|---|
| 0–39 | wyjścia O1–O40 |

### Input registers (16 bit, odczyt)

| Adres | Znaczenie | Jednostka |
|---|---|---|
| 0–3 | czujniki temperatury T1–T4 | 0,1 °C |
| 12 | wejście analogowe 0–10 V | mV |
| 14 | poziom sygnału GSM | 0–5 |
| 16 | zasilanie podstawowe obecne | 1/0 |
| 18 | napięcie zasilania centrali | mV |
| 20–27 | pobór mocy modułów IOE-IQPL o adresach 1–8 | W |
| 30–37 | napięcie zasilania modułów IOE-IQPL o adresach 1–8 | V |
| 40–44 | czas systemowy: minuta, godzina, dzień, miesiąc, rok | |
| 50–53 | kod awarii, bajty 0–3 | |
| 60 | wersja firmware'u AP-IP | |
| 61 | wersja firmware'u OptimaGSM | |

## Ograniczenia

Celowo **nie ma** rejestrów czuwania ani stanu stref.
Producent proponuje obejście przez logikę centrali:

- uzbrajanie: wyjście sterowane po Modbusie,
  w centrali „przepisane” na wejście typu ZAŁ/WYŁ czuwanie, KASUJ alarm itd.,
- stan czuwania i alarmu: wyjście bistabilne sterowane statusem systemu,
  odczytywane jako coil.
