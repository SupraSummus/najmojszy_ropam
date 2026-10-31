#ifndef ROPAM_BRAMKA_H
#define ROPAM_BRAMKA_H

/*
 * Bramka na XIAO w systemie Wi-Fi rvuos: co dzielą klient centrali (ropam-klient.c)
 * i proces sieciowy, który podaje stan przez HTTP (strona.c nad tcp.c).
 *
 * Klient pisze migawkę przez lib/seqlock.h do obszaru wyciętego ze strony procesu sieciowego,
 * a proces sieciowy ją czyta przy każdym zapytaniu.
 */

#include <stdint.h>

#include "ropam/ropam.h"

struct ropam_snapshot {
    uint32_t reads, errors;  /* udane i nieudane odczyty od startu */
    int32_t last_error;      /* ropam.h, 0 gdy ostatni się udał */
    uint32_t have;           /* czy status jest już z jakiegoś odczytu */
    uint8_t status[ROPAM_STATUS_SIZE];
};

/* Obszar migawki w stronie procesu sieciowego: osobny region, więc potęga dwójki. */
#define ROPAM_AREA_SIZE 4096u

#define HTTP_PORT 80u
#define HTTP_TX_MAX 3072u /* bufor odpowiedzi na połączenie */

/* Co ile klient czyta stan centrali. */
#define ROPAM_EVERY_MS 5000u

#endif
