#ifndef ROPAM_STRONA_H
#define ROPAM_STRONA_H

/*
 * Strona WWW i JSON ze stanu centrali (1100 B odpowiedzi na 0x24), dla serwera HTTP na XIAO.
 * Pola stanu: docs/stan-centrali.md. Tylko podgląd, bez żadnych poleceń.
 */

#include <stdint.h>

struct strona_stan {
    const uint8_t *status;   /* ROPAM_STATUS_SIZE bajtów albo 0, gdy jeszcze nie ma odczytu */
    uint32_t age_ms;         /* ile temu był odczyt */
    uint32_t reads, errors;  /* udane i nieudane odczyty od startu */
    int last_error;          /* ostatni błąd z ropam.h, 0 gdy ostatni odczyt się udał */
};

/* Odpowiedź HTTP (nagłówki i treść) na zapytanie req; długość w out, najwyżej max bajtów. */
uint32_t strona_http(const struct strona_stan *s, const uint8_t *req, uint32_t len, uint8_t *out, uint32_t max);

#endif
