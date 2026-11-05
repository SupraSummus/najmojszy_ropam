#ifndef ROPAM_STRONA_H
#define ROPAM_STRONA_H

/*
 * Strona WWW i JSON ze stanu centrali, dla serwera HTTP na XIAO:
 * nagłówek stanu z rozgłoszenia 0x10 na magistrali (te same 220 B co na początku odpowiedzi na 0x24)
 * i raport panelu TPR (0x11). Pola: docs/stan-centrali.md, docs/protokol-ramek.md.
 * Tylko podgląd, bez żadnych poleceń.
 */

#include <stdint.h>

struct strona_stan {
    const uint8_t *status;  /* 220 B nagłówka stanu albo 0, gdy jeszcze go nie ma */
    const uint8_t *panel;   /* 10 B raportu panelu albo 0 */
    uint32_t age_ms;        /* wiek stanu */
    uint32_t bytes, frames, states, junk; /* bajty, ramki, rozgłoszenia stanu, bajty śmieci od startu */
};

/* Odpowiedź HTTP (nagłówki i treść) na zapytanie req; długość w out, najwyżej max bajtów. */
uint32_t strona_http(const struct strona_stan *s, const uint8_t *req, uint32_t len, uint8_t *out, uint32_t max);

#endif
