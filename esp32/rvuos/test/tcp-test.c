/*
 * Test tcp.c na komputerze: klient HTTP odegrany ramkami, stos net.c rvuos pod spodem.
 *
 * cc -std=c11 -g -Wall -Wextra -Werror -fsanitize=address,undefined \
 *    -I third_party/rvuos/user -I third_party/rvuos/user/wifi -I esp32/rvuos \
 *    esp32/rvuos/test/tcp-test.c esp32/rvuos/tcp.c third_party/rvuos/user/wifi/net.c
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "tcp.h"
#include "test/frames.h"

static uint8_t buf[NET_FRAME_MAX];
static uint8_t sent[64][NET_FRAME_MAX];
static uint32_t sent_len[64], nsent;

static uint8_t *give_buf(struct net *n)
{
    (void)n;
    return buf;
}

static void take(struct net *n, uint8_t *f, uint32_t len)
{
    (void)n;
    assert(nsent < 64);
    memcpy(sent[nsent], f, len);
    sent_len[nsent++] = len;
}

static uint8_t page[3000];

static uint32_t app(void *arg, const uint8_t *req, uint32_t len, uint8_t *out, uint32_t max)
{
    (void)arg;
    assert(len >= 4 && memcmp(req, "GET ", 4) == 0);
    assert(sizeof(page) <= max);
    memcpy(out, page, sizeof(page));
    return sizeof(page);
}

static uint32_t me, client;
static const uint8_t client_mac[6] = { 0x02, 0, 0, 0, 0, 0x42 };

static uint32_t get32(const uint8_t *p)
{
    return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3];
}

static void put32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);
    p[3] = (uint8_t)v;
}

/* Segment od klienta do serwera (port 80). */
static void client_send(struct net *n, uint32_t seq, uint32_t ack, uint8_t flags, const char *data)
{
    uint8_t f[NET_FRAME_MAX];
    uint32_t dlen = data ? (uint32_t)strlen(data) : 0;
    uint8_t *s = f + 34;
    put_be16(s, 40000);
    put_be16(s + 2, 80);
    put32(s + 4, seq);
    put32(s + 8, ack);
    s[12] = 5 << 4;
    s[13] = flags;
    put_be16(s + 14, 8192);
    put_be16(s + 16, 0);
    put_be16(s + 18, 0);
    if (dlen) {
        memcpy(s + 20, data, dlen);
    }
    const uint8_t *fr = (const uint8_t *)&client, *to = (const uint8_t *)&me;
    uint32_t sum = be16(fr) + be16(fr + 2) + be16(to) + be16(to + 2) + 6u + 20u + dlen;
    put_be16(s + 16, checksum(s, 20 + dlen, sum));
    uint32_t len = ip_frame(f, n->mac, client, me, 6, 20 + dlen);
    memcpy(f + 6, client_mac, 6);
    net_input(n, f, len);
}

struct seg {
    uint32_t seq, ack, flags, len;
    const uint8_t *data;
};

static struct seg parse(uint32_t i)
{
    const uint8_t *s = sent[i] + 34;
    struct seg g = { get32(s + 4), get32(s + 8), s[13], sent_len[i] - 34 - (uint32_t)(s[12] >> 4) * 4u,
                     s + (s[12] >> 4) * 4 };
    assert(sent[i][23] == 6);
    const uint8_t *fr = (const uint8_t *)&me, *to = (const uint8_t *)&client;
    uint32_t sum = be16(fr) + be16(fr + 2) + be16(to) + be16(to + 2) + 6u + sent_len[i] - 34;
    assert(checksum(s, sent_len[i] - 34, sum) == 0);
    return g;
}

int main(void)
{
    for (uint32_t i = 0; i < sizeof(page); i++) {
        page[i] = (uint8_t)('a' + i % 26);
    }
    struct net n;
    static uint8_t tx[TCP_CONNS * 4096];
    struct tcp t;
    net_init(&n, pico, give_buf, take, 0);
    me = net_ip(192, 168, 1, 50);
    client = net_ip(192, 168, 1, 7);
    n.ip = me;
    n.mask = net_ip(255, 255, 255, 0);
    n.gateway = net_ip(192, 168, 1, 1);
    memcpy(n.arp[0].mac, client_mac, 6);
    n.arp[0].ip = client;
    tcp_init(&t, &n, 80, app, 0, tx, 4096);

    /* uzgodnienie */
    client_send(&n, 1000, 0, 0x02, 0);
    assert(nsent == 1);
    struct seg g = parse(0);
    assert(g.flags == 0x12 && g.ack == 1001);
    uint32_t iss = g.seq;
    client_send(&n, 1001, iss + 1, 0x10, 0);
    assert(nsent == 1);

    /* zapytanie: odpowiedź w trzech segmentach, FIN przy ostatnim */
    const char *req = "GET / HTTP/1.0\r\nHost: x\r\n\r\n";
    client_send(&n, 1001, iss + 1, 0x18, req);
    uint32_t total = 0, fin = 0;
    for (uint32_t i = 1; i < nsent; i++) {
        g = parse(i);
        assert(g.ack == 1001 + strlen(req));
        assert(g.len <= TCP_MSS);
        assert(memcmp(g.data, page + (g.seq - iss - 1), g.len) == 0);
        total += g.len;
        fin |= g.flags & 0x01;
    }
    assert(total == sizeof(page) && fin);
    printf("odpowiedź: %u B w %u segmentach\n", total, nsent - 1);

    /* zgubione: klient potwierdza tylko pierwszy segment; po czasie serwer ponawia resztę */
    uint32_t first = parse(1).len;
    nsent = 0;
    client_send(&n, 1001 + strlen(req), iss + 1 + first, 0x10, 0);
    tcp_tick(&t, 100);
    assert(nsent == 0);
    n.now = 1000;
    tcp_tick(&t, 1000);
    assert(nsent >= 1);
    g = parse(0);
    assert(g.seq == iss + 1 + first);
    printf("ponowienie od bajtu %u: %u segmentów\n", first, nsent);

    /* klient potwierdza wszystko z FIN-em i sam zamyka: serwer potwierdza i zwalnia połączenie */
    nsent = 0;
    client_send(&n, 1001 + strlen(req), iss + 1 + sizeof(page) + 1, 0x11, 0);
    assert(nsent == 1);
    g = parse(0);
    assert(g.flags == 0x10 && g.ack == 1001 + strlen(req) + 1);
    assert(t.conns[0].state == TCP_FREE && t.conns[1].state == TCP_FREE);

    /* segment do nieistniejącego połączenia dostaje RST */
    nsent = 0;
    client_send(&n, 5000, 7, 0x10, 0);
    assert(nsent == 1 && (parse(0).flags & 0x04));
    printf("tcp-test: ok (przyjęte %u, odpowiedzi %u, ponowienia %u)\n", t.accepted, t.answered, t.retransmits);
    return 0;
}
