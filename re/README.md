# Zrozumiany kod

Tu odkładamy to, co zrozumieliśmy z firmware'u.

Każdy katalog odpowiada jednemu obrazowi firmware'u
(nazwa jak plik w [`firmware/`](../firmware/) bez `.flsh`):

- `symbols.txt` to **źródło prawdy**:
  adres, rodzaj (`f` funkcja, `d` dane), nasza nazwa i komentarz.
  To jedyny plik, który edytujemy ręcznie.
- `src/` zawiera C wygenerowany przez Ghidrę dla funkcji nazwanych w `symbols.txt`,
  już z naszymi nazwami i komentarzami.
  Nie edytujemy go ręcznie, tylko regenerujemy.

## Jak dodać wiedzę

1. Rozpracuj funkcję albo zmienną
   (pełny zrzut `build/<obraz>/decomp.c`, asembler przez `tools/ghidra/Disasm.java`).
2. Dopisz linię do `symbols.txt`.
   Przypuszczenia oznacz w komentarzu znakiem ❓.
3. Wygeneruj ponownie:

   ```sh
   tools/ghidra/analyze.sh firmware/optimagsm/optimagsm_v3.4_pl.flsh
   ```

4. Commituj `symbols.txt` razem z `src/`,
   żeby w diffie było widać, jak zmienił się zrozumiany kod.

Wnioski wyższego poziomu (protokoły, formaty) opisujemy w [`docs/`](../docs/).

## Postęp

| Obraz | Funkcji | Nazwanych |
|---|---|---|
| `optimagsm_v3.4_pl` | 1131 | 65 |
