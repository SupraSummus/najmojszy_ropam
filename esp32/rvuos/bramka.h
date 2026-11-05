#ifndef ROPAM_BRAMKA_H
#define ROPAM_BRAMKA_H

/*
 * Bramka na XIAO w systemie Wi-Fi rvuos: co dzielą klient magistrali (szyna-klient.c)
 * i proces sieciowy, który podaje stan przez HTTP (strona.c nad tcp.c).
 *
 * Klient pisze migawkę przez lib/seqlock.h do obszaru wyciętego ze strony procesu sieciowego,
 * a proces sieciowy ją czyta przy każdym zapytaniu.
 */

#include <stdint.h>

/* Nagłówek stanu, który centrala rozgłasza ramką 0x10 (docs/stan-centrali.md). */
#define ROPAM_HEADER_SIZE 220u
/* Dane raportu panelu TPR, odpowiedź 0x11 (docs/protokol-ramek.md). */
#define ROPAM_PANEL_SIZE 10u

struct ropam_snapshot {
    uint32_t bytes;    /* bajty odebrane z magistrali od startu */
    uint32_t frames;   /* ramki z poprawnym CRC od startu */
    uint32_t junk;     /* bajty, które nie złożyły się w ramkę ani potwierdzenie */
    uint32_t states;   /* odebrane rozgłoszenia stanu */
    uint32_t have;     /* czy status jest już z jakiegoś rozgłoszenia */
    uint32_t panel_have;
    uint32_t state_ms, now_ms; /* czas klienta (ms): ostatniego stanu i tej publikacji */
    uint8_t status[ROPAM_HEADER_SIZE];
    uint8_t panel[ROPAM_PANEL_SIZE];
    uint8_t pad[2];
};

/* Wiek stanu w chwili odczytu migawki, gdy od jej publikacji minęło since_ms. */
static inline uint32_t ropam_state_age(const struct ropam_snapshot *s, uint32_t since_ms)
{
    return s->now_ms - s->state_ms + since_ms;
}

/* Obszar migawki w stronie procesu sieciowego: osobny region, więc potęga dwójki. */
#define ROPAM_AREA_SIZE 4096u

#define HTTP_PORT 80u
#define HTTP_TX_MAX 3072u /* bufor odpowiedzi na połączenie */

#endif
