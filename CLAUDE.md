# najmojszy_ropam

Rozpoznanie i otwarta integracja centrali alarmowej Ropam OptimaGSM.
Cel: zrozumieć, co centrala robi i jak gada,
żeby obsługiwać ją z Linuksa
i docelowo zbudować bramkę Wi-Fi (prawdopodobnie ESP32).

## Konwencje

- Główna gałąź: `main`.
- Dokumentacja i komentarze po polsku.
- Każdy tekst (Markdown, commity, komentarze) piszemy w
  [Semantic Line Breaks](https://sembr.org/):
  - łamiemy linię po każdym zdaniu,
  - łamiemy po zdaniu składowym zakończonym przecinkiem, średnikiem, dwukropkiem lub myślnikiem,
  - możemy łamać po zdaniu podrzędnym, gdy to poprawia czytelność albo skraca linię,
  - nigdy nie łamiemy wewnątrz wyrazu z łącznikiem,
  - linie do ok. 80 znaków (dłuższe tylko dla linków i kodu),
  - złamanie linii nie może zmieniać renderowanego wyniku.

## Repo jest publiczne

- Nie opisujemy konkretnej instalacji:
  posiadanego modelu i wersji, podłączonych modułów, stref, wejść,
  numerów telefonów, kodów, kluczy, logiki, adresów IP, lokalizacji.
- Dokumentacja opisuje urządzenie i protokoły ogólnie.
  Przykłady ramek i konfiguracji muszą być syntetyczne albo zanonimizowane.
- Kopie konfiguracji centrali (`*.rpm`) i pliki logiki z prawdziwej instalacji
  nigdy nie trafiają do repo,
  także we fragmentach czy jako dane testowe.

## Zasady bezpieczeństwa

- To działający system alarmowy.
  Nie nadajemy niczego na magistralę RopamNET (RS-485),
  dopóki nie wiemy dokładnie, co nadajemy.
  Podsłuch prowadzimy transceiverem w trybie tylko do odbioru.
- Tryb ON-LINE na porcie serwisowym RS232TTL wyłącza normalne funkcje centrali
  (np. nie wysyła SMS-ów przy naruszeniu wejść).
  Po każdej sesji serwisowej trzeba sprawdzić, że centrala wróciła do normalnej pracy.
- Konfigurację zmieniamy tylko przez `tools/ropam_serwis.py zmien|przywroc`
  ([procedura](docs/programowanie.md)) i tylko za zgodą właściciela.
  Każdy zapis może wyłączyć wyjścia ustawione przez LogicProcessor.
- Oryginalne pliki firmware'u Ropamu (`*.flsh`) trzymamy w `firmware/`
  (decyzja właściciela repo; jeśli Ropam zgłosi zastrzeżenia, usuwamy).
  Odszyfrowanych obrazów nie commitujemy, bo każdy odtworzy je `tools/flsh.py`.
- Pozostałe pliki Ropamu (instalatory, programy, PDF-y) trzymamy lokalnie w `vendor/`,
  który jest w `.gitignore`.
- Zrzuty ruchu (`captures/`) mogą zawierać kody użytkowników i klucze.
  Przed commitem trzeba je przejrzeć albo zanonimizować.
