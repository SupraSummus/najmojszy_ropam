# Źródła

Pliki producenta trzymamy lokalnie w `vendor/` (poza repo).

## Dokumentacja Ropam

- [Strona wsparcia OptimaGSM](https://ropam.com.pl/wsparcie/optimagsm/):
  instrukcje, komendy SMS, LogicProcessor, (martwy) link do firmware'u v4.1.
- [Instrukcja instalacji OptimaGSM (kopia Eltrox, 2015)](https://ftp.eltrox.pl/INSTRUKCJE/ROPAM/MANIPULATORY/2_Instrukcja_instalacji_OptimaGSM.pdf):
  złącza, RopamNET, parametry, historia wersji firmware'u.
- [Nota: Modbus TCP/IP – protokół integracji z centralą](https://ropam.com.pl/wsparcie/pliki/noty_aplik/an_optimagsm_apip_modbus_tcpip.pdf).
- [OptimaGSM: szybki start](https://ropam.com.pl/wsparcie/wp-content/uploads/2018/04/an_optimagsm_szybki_start.pdf):
  AP-IP, klucz TCP/IP, port 8888.
- [OptimaGSM Manager](https://ropam.com.pl/wsparcie/optimagsm-manager-4/):
  instalator v2.5 (Inno Setup; w środku `OptimaGSMManager.exe`, `OptimaUpdate.exe`, sterownik FTDI).
- [Programy do aktualizacji](https://ropam.com.pl/wsparcie/cennik/programy-do-aktualizacji/).

## Społeczność

- [elektroda.pl: kabel MGSM, pinout RJ12](https://poszukaj.elektroda.pl/szukaj,mgsm-ropam-kabel.html).
- [Forum Ropam: instrukcja aktualizacji OptimaGSM](https://ropam.com.pl/forum/index.php?topic=992.0).
- [AI-Speaker: Ropam, integracja](https://ai-speaker.discourse.group/t/ropam-integracja/2054):
  Modbus TCP dla OptimaGSM z AP-IP.
- [tjaworski997/X.RopamNeo](https://github.com/tjaworski997/X.RopamNeo):
  integracja NeoGSM-IP (inna centrala, ale ten sam producent).

## Metodyka (inne urządzenia)

- [Galaxy RS485 protocol reverse engineering](http://datashed.science/projects/galaxy/protocol/):
  analiza magistrali klawiatur innej centrali alarmowej.
- [Platinum 3100S RS485](https://www.steeman.be/posts/reverse-engineering-platinum-inverter-rs485/):
  podsłuch pasywny, sumy kontrolne.
- Microchip AN1388, „PIC32 Bootloader”:
  protokół, z którego wywodzi się OptimaUpdate.
