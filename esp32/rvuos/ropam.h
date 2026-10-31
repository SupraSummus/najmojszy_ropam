#ifndef ROPAM_ROPAM_H
#define ROPAM_ROPAM_H

/*
 * Klient protokołu serwisowego OptimaGSM (XMODEM na porcie RS232TTL), jak tools/ropam_serwis.py.
 *
 * Tylko odczyt: identyfikacja (0x01), stan (0x24), dziennik (0x04)
 * i zamknięcie sesji serwisowej trzema przerwaniami CAN CAN; zob. docs/protokol-serwisowy.md.
 * Wejście i wyjście bajtów oraz zegar daje wywołujący, więc biblioteka nie zależy od rvuos.
 */

#include <stdint.h>

struct ropam_io {
    /* Bajt z linii albo -1, gdy w ciągu ms milisekund nic nie przyszło. */
    int (*get)(void *arg, uint32_t ms);
    void (*put)(void *arg, const uint8_t *data, uint32_t len);
    uint32_t (*now)(void *arg); /* ms */
    void *arg;
};

enum ropam_result {
    ROPAM_OK = 0,
    ROPAM_NO_INVITE = -1,  /* centrala nie nadała 'C' */
    ROPAM_NOT_ACKED = -2,  /* centrala nie potwierdziła bloku albo EOT */
    ROPAM_NO_DATA = -3,    /* nie przyszła odpowiedź */
    ROPAM_CANCELLED = -4,  /* centrala przerwała */
    ROPAM_ECHO = -5,       /* centrala nadaje bez przerwy: słyszy własne echo */
};

#define ROPAM_STATUS_SIZE 1100u

/* Wysyła komendę (dane pakietu) XMODEM-em; ROPAM_OK albo błąd. */
int ropam_send(const struct ropam_io *io, const uint8_t *data, uint32_t len);
/* Odbiera jedną transmisję XMODEM do out (najwyżej max bajtów, z dopełnieniem); długość albo błąd. */
int ropam_receive(const struct ropam_io *io, uint8_t *out, uint32_t max, uint32_t timeout_ms);
/* Gasi flagę sesji serwisowej. */
int ropam_end_session(const struct ropam_io *io);
/* Stan centrali (0x24) do out[ROPAM_STATUS_SIZE], potem zamknięcie sesji. */
int ropam_status(const struct ropam_io *io, uint8_t *out);

#endif
