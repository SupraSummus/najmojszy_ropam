#ifndef ROPAM_TCP_H
#define ROPAM_TCP_H

/*
 * Najmniejszy serwer TCP nad stosem net.c rvuos, dla serwera HTTP na XIAO.
 *
 * Słucha na jednym porcie i obsługuje TCP_CONNS połączeń naraz.
 * Zbiera zapytanie do pustej linii ("\r\n\r\n"), oddaje je aplikacji,
 * wysyła jej odpowiedź i zamyka połączenie (FIN), jak HTTP/1.0.
 * Ponawia niepotwierdzone segmenty od pierwszego niepotwierdzonego bajtu
 * (go-back-N) z rosnącym czasem oczekiwania.
 * Nie wywołuje kernela i nie ma zmiennych globalnych, jak net.c,
 * więc da się go testować na komputerze.
 */

#include <stdint.h>

#include "wifi/net.h"

#define TCP_CONNS   2u
#define TCP_RX_MAX  1024u /* najdłuższe zapytanie */
#define TCP_MSS     1400u
#define TCP_INFLIGHT 4u   /* najwięcej segmentów w drodze */

/*
 * Aplikacja: odpowiedź na zapytanie req o długości len, zapisana do out (najwyżej max bajtów);
 * zwraca jej długość. Bufor należy do połączenia aż do jego zamknięcia, więc ponowienia wysyłają to samo.
 */
typedef uint32_t (*tcp_app_fn)(void *arg, const uint8_t *req, uint32_t len, uint8_t *out, uint32_t max);

enum tcp_state { TCP_FREE, TCP_SYN_RCVD, TCP_OPEN, TCP_CLOSING };

struct tcp_conn {
    uint32_t state;
    uint32_t ip;
    uint16_t port, peer_window;
    uint32_t iss, snd_una, snd_nxt, rcv_nxt;
    uint32_t last;    /* ms, ostatnia aktywność rozmówcy */
    uint32_t rto, rto_at, tries;
    uint32_t peer_fin; /* rozmówca zamknął swoją stronę */
    uint32_t rx_len, tx_len, answered;
    uint8_t rx[TCP_RX_MAX];
    uint8_t *tx;      /* odpowiedź, tx_len bajtów; FIN idzie za nią */
};

struct tcp {
    struct net *net;
    uint16_t port;
    tcp_app_fn app;
    void *arg;
    uint8_t *tx_buffers; /* TCP_CONNS buforów po tx_max bajtów */
    uint32_t tx_max;
    uint32_t isn;
    struct tcp_conn conns[TCP_CONNS];
    uint32_t accepted, answered, resets, retransmits;
};

/* Serwer na porcie port; tx_buffers to TCP_CONNS * tx_max bajtów na odpowiedzi. */
void tcp_init(struct tcp *t, struct net *n, uint16_t port, tcp_app_fn app, void *arg, uint8_t *tx_buffers,
              uint32_t tx_max);
/* Czas: ponowienia i porzucanie połączeń bez życia. */
void tcp_tick(struct tcp *t, uint32_t now);

#endif
