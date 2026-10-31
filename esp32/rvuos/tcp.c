/*
 * Najmniejszy serwer TCP; zob. tcp.h.
 */

#include "tcp.h"

#include <stddef.h>

#define IP_TCP 6u

#define T_FIN 0x01u
#define T_SYN 0x02u
#define T_RST 0x04u
#define T_PSH 0x08u
#define T_ACK 0x10u

#define T_HEADER     20u
#define RTO_FIRST  400u   /* ms */
#define RTO_LAST   6400u
#define TRIES      8u
#define IDLE_MS    15000u

static uint32_t be16(const uint8_t *p)
{
    return (uint32_t)p[0] << 8 | p[1];
}

static uint32_t be32(const uint8_t *p)
{
    return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3];
}

static void put16(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 8);
    p[1] = (uint8_t)v;
}

static void put32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);
    p[3] = (uint8_t)v;
}

static uint32_t sum16(uint32_t sum, const uint8_t *p, uint32_t len)
{
    for (uint32_t i = 0; i + 1 < len; i += 2) {
        sum += (uint32_t)p[i] << 8 | p[i + 1];
    }
    if (len & 1u) {
        sum += (uint32_t)p[len - 1] << 8;
    }
    return sum;
}

static uint16_t fold(uint32_t sum)
{
    while (sum >> 16) {
        sum = (sum & 0xffffu) + (sum >> 16);
    }
    return (uint16_t)~sum;
}

/* Suma kontrolna TCP z pseudonagłówkiem; adresy w kolejności sieciowej, jak trzyma je net.c. */
static uint16_t checksum(uint32_t from, uint32_t to, const uint8_t *seg, uint32_t len)
{
    uint8_t pseudo[12];
    const uint8_t *f = (const uint8_t *)&from, *t = (const uint8_t *)&to;
    for (int i = 0; i < 4; i++) {
        pseudo[i] = f[i];
        pseudo[4 + i] = t[i];
    }
    pseudo[8] = 0;
    pseudo[9] = IP_TCP;
    put16(pseudo + 10, len);
    return fold(sum16(sum16(0, pseudo, sizeof(pseudo)), seg, len));
}

static void copy(uint8_t *to, const uint8_t *from, uint32_t len)
{
    for (uint32_t i = 0; i < len; i++) {
        to[i] = from[i];
    }
}

static uint32_t window(const struct tcp_conn *c)
{
    return TCP_RX_MAX - c->rx_len;
}

/* Segment do rozmówcy połączenia c (albo, gdy c to 0, do ip:port jako T_RST). */
static void segment(struct tcp *t, uint32_t ip, uint16_t port, uint32_t seq, uint32_t ack, uint32_t flags,
                    uint32_t win, const uint8_t *data, uint32_t len)
{
    uint8_t s[T_HEADER + 4 + TCP_MSS];
    uint32_t head = T_HEADER + ((flags & T_SYN) ? 4u : 0u);
    put16(s, t->port);
    put16(s + 2, port);
    put32(s + 4, seq);
    put32(s + 8, ack);
    s[12] = (uint8_t)((head / 4u) << 4);
    s[13] = (uint8_t)flags;
    put16(s + 14, win);
    put16(s + 16, 0);
    put16(s + 18, 0);
    if (flags & T_SYN) {
        s[20] = 2; /* MSS */
        s[21] = 4;
        put16(s + 22, TCP_MSS);
    }
    copy(s + head, data, len);
    put16(s + 16, checksum(t->net->ip, ip, s, head + len));
    net_ip_send(t->net, ip, IP_TCP, s, head + len);
}

static void conn_free(struct tcp_conn *c)
{
    c->state = TCP_FREE;
}

static void reset(struct tcp *t, struct tcp_conn *c)
{
    segment(t, c->ip, c->port, c->snd_nxt, c->rcv_nxt, T_RST | T_ACK, 0, 0, 0);
    t->resets++;
    conn_free(c);
}

static void send_ack(struct tcp *t, struct tcp_conn *c)
{
    segment(t, c->ip, c->port, c->snd_nxt, c->rcv_nxt, T_ACK, window(c), 0, 0);
}

/* Wysyła z odpowiedzi tyle, ile pozwala okno, a za nią T_FIN; od snd_nxt. */
static void push(struct tcp *t, struct tcp_conn *c)
{
    if (c->state == TCP_SYN_RCVD) {
        segment(t, c->ip, c->port, c->iss, c->rcv_nxt, T_SYN | T_ACK, window(c), 0, 0);
        c->snd_nxt = c->iss + 1u;
    } else if (c->answered) {
        uint32_t base = c->iss + 1u, end = base + c->tx_len; /* T_FIN ma numer end */
        uint32_t limit = c->snd_una + (c->peer_window < TCP_INFLIGHT * TCP_MSS ? c->peer_window : TCP_INFLIGHT * TCP_MSS);
        while (c->snd_nxt - base < c->tx_len && c->snd_nxt - c->snd_una < limit - c->snd_una) {
            uint32_t off = c->snd_nxt - base, len = c->tx_len - off;
            if (len > TCP_MSS) {
                len = TCP_MSS;
            }
            if (len > limit - c->snd_nxt) {
                len = limit - c->snd_nxt;
            }
            uint32_t last = off + len == c->tx_len;
            segment(t, c->ip, c->port, c->snd_nxt, c->rcv_nxt, T_ACK | T_PSH | (last ? T_FIN : 0u), window(c),
                    c->tx + off, len);
            c->snd_nxt += len + (last ? 1u : 0u);
        }
        if (c->snd_nxt == end) {
            /* pusta odpowiedź albo ponowienie samego T_FIN-a */
            segment(t, c->ip, c->port, end, c->rcv_nxt, T_ACK | T_FIN, window(c), 0, 0);
            c->snd_nxt = end + 1u;
        }
        c->state = TCP_CLOSING;
    }
    if (c->snd_una != c->snd_nxt) {
        c->rto_at = t->net->now + c->rto;
    }
}

static struct tcp_conn *find(struct tcp *t, uint32_t ip, uint16_t port)
{
    for (uint32_t i = 0; i < TCP_CONNS; i++) {
        struct tcp_conn *c = &t->conns[i];
        if (c->state != TCP_FREE && c->ip == ip && c->port == port) {
            return c;
        }
    }
    return 0;
}

/* Czy w zapytaniu jest już pusta linia, czyli koniec nagłówków HTTP. */
static int complete(const struct tcp_conn *c)
{
    for (uint32_t i = 3; i < c->rx_len; i++) {
        if (c->rx[i - 3] == '\r' && c->rx[i - 2] == '\n' && c->rx[i - 1] == '\r' && c->rx[i] == '\n') {
            return 1;
        }
    }
    return c->rx_len == TCP_RX_MAX;
}

static void input(struct net *n, void *arg, uint32_t from, const uint8_t *s, uint32_t len)
{
    struct tcp *t = arg;
    (void)n;
    if (len < T_HEADER || checksum(from, t->net->ip, s, len) != 0) {
        return;
    }
    uint32_t head = (uint32_t)(s[12] >> 4) * 4u;
    if (head < T_HEADER || head > len || be16(s + 2) != t->port) {
        return;
    }
    uint16_t port = (uint16_t)be16(s);
    uint32_t seq = be32(s + 4), ack = be32(s + 8), flags = s[13], win = be16(s + 14);
    const uint8_t *data = s + head;
    uint32_t dlen = len - head;
    struct tcp_conn *c = find(t, from, port);

    if (c == 0) {
        if (flags & T_RST) {
            return;
        }
        if ((flags & (T_SYN | T_ACK)) != T_SYN) {
            segment(t, from, port, (flags & T_ACK) ? ack : 0u, seq + dlen + ((flags & T_FIN) ? 1u : 0u),
                    T_RST | ((flags & T_ACK) ? 0u : T_ACK), 0, 0, 0);
            return;
        }
        for (uint32_t i = 0; i < TCP_CONNS && c == 0; i++) {
            if (t->conns[i].state == TCP_FREE) {
                c = &t->conns[i];
            }
        }
        if (c == 0) {
            segment(t, from, port, 0, seq + 1u, T_RST | T_ACK, 0, 0, 0);
            return;
        }
        t->isn += 0x10001u + t->net->now * 250u;
        c->state = TCP_SYN_RCVD;
        c->ip = from;
        c->port = port;
        c->iss = t->isn;
        c->snd_una = c->snd_nxt = c->iss;
        c->rcv_nxt = seq + 1u;
        c->peer_window = (uint16_t)win;
        c->rto = RTO_FIRST;
        c->tries = 0;
        c->peer_fin = 0;
        c->rx_len = 0;
        c->tx_len = 0;
        c->answered = 0;
        c->tx = t->tx_buffers + (uint32_t)(c - t->conns) * t->tx_max;
        c->last = t->net->now;
        t->accepted++;
        push(t, c);
        return;
    }

    c->last = t->net->now;
    if (flags & T_RST) {
        conn_free(c);
        return;
    }
    if (flags & T_SYN) {
        if (c->state == TCP_SYN_RCVD && seq + 1u == c->rcv_nxt) {
            c->snd_nxt = c->iss;
            push(t, c); /* nasz T_SYN-T_ACK zginął */
        }
        return;
    }
    if (!(flags & T_ACK)) {
        return;
    }
    c->peer_window = (uint16_t)win;
    if (ack - c->snd_una - 1u < c->snd_nxt - c->snd_una) { /* snd_una < ack <= snd_nxt */
        c->snd_una = ack;
        c->rto = RTO_FIRST;
        c->tries = 0;
        if (c->state == TCP_SYN_RCVD) {
            c->state = TCP_OPEN;
        }
    }
    if (c->state == TCP_CLOSING && c->snd_una == c->iss + 1u + c->tx_len + 1u) {
        if (dlen != 0 || (flags & T_FIN)) {
            c->rcv_nxt = seq + dlen + ((flags & T_FIN) ? 1u : 0u);
            send_ack(t, c);
        }
        if (c->peer_fin || (flags & T_FIN)) {
            conn_free(c); /* obie strony zamknięte */
            return;
        }
    }
    int need_ack = 0;
    if (dlen != 0 || (flags & T_FIN)) {
        if (seq != c->rcv_nxt) {
            send_ack(t, c); /* nie po kolei: powtórzone potwierdzenie */
            return;
        }
        uint32_t take = dlen;
        if (take > TCP_RX_MAX - c->rx_len) {
            take = TCP_RX_MAX - c->rx_len;
        }
        copy(c->rx + c->rx_len, data, take);
        c->rx_len += take;
        c->rcv_nxt += take;
        if ((flags & T_FIN) && take == dlen) {
            c->rcv_nxt += 1u;
            c->peer_fin = 1;
        }
        need_ack = 1;
    }
    if (c->state == TCP_OPEN && !c->answered && (complete(c) || c->peer_fin)) {
        c->tx_len = t->app(t->arg, c->rx, c->rx_len, c->tx, t->tx_max);
        c->answered = 1;
        t->answered++;
        push(t, c); /* potwierdza też zapytanie */
        need_ack = 0;
    } else if (c->state == TCP_CLOSING) {
        push(t, c);
    }
    if (need_ack) {
        send_ack(t, c);
    }
}

void tcp_init(struct tcp *t, struct net *n, uint16_t port, tcp_app_fn app, void *arg, uint8_t *tx_buffers,
              uint32_t tx_max)
{
    for (uint32_t i = 0; i < TCP_CONNS; i++) {
        t->conns[i].state = TCP_FREE;
    }
    t->net = n;
    t->port = port;
    t->app = app;
    t->arg = arg;
    t->tx_buffers = tx_buffers;
    t->tx_max = tx_max;
    t->isn = 0x52504d00u; /* "RPM" */
    t->accepted = t->answered = t->resets = t->retransmits = 0;
    n->tcp_input = input;
    n->tcp_arg = t;
}

void tcp_tick(struct tcp *t, uint32_t now)
{
    for (uint32_t i = 0; i < TCP_CONNS; i++) {
        struct tcp_conn *c = &t->conns[i];
        if (c->state == TCP_FREE) {
            continue;
        }
        if (now - c->last > IDLE_MS) {
            reset(t, c);
            continue;
        }
        if (c->snd_una != c->snd_nxt && (int32_t)(now - c->rto_at) >= 0) {
            if (++c->tries > TRIES) {
                reset(t, c);
                continue;
            }
            t->retransmits++;
            c->rto = c->rto * 2u < RTO_LAST ? c->rto * 2u : RTO_LAST;
            c->snd_nxt = c->snd_una;
            if (c->state == TCP_CLOSING) {
                c->state = TCP_OPEN; /* push wyśle od snd_una i znów zamknie */
            }
            push(t, c);
        }
    }
}
