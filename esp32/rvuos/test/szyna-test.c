/*
 * Test szyna.c na komputerze; ramki wzorcowe z tools/ropamnet.py.
 *
 * cc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -I esp32/rvuos \
 *    esp32/rvuos/test/szyna-test.c esp32/rvuos/szyna.c
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "szyna.h"

/* Co wypisał parser: rodzaje, długości i czasy pozycji po kolei. */
static char kinds[64];
static uint32_t lens[64], times[64], items;

static void collect(void *arg, char rodzaj, const uint8_t *p, uint32_t n, uint32_t s, uint32_t us, uint32_t bledy)
{
    (void)arg;
    (void)p;
    (void)s;
    (void)bledy;
    assert(items < 64);
    kinds[items] = rodzaj;
    lens[items] = n;
    times[items] = us;
    items++;
}

/* Bajty po kolei, każdy 1 µs po poprzednim. */
static void feed(struct szyna_parser *ps, const uint8_t *p, uint32_t n, uint32_t *t)
{
    for (uint32_t i = 0; i < n; i++) {
        szyna_bajt(ps, p[i], 0, (*t)++, 0);
    }
}

static void expect_items(const char *want_kinds, const uint32_t *want_lens)
{
    assert(items == strlen(want_kinds));
    for (uint32_t i = 0; i < items; i++) {
        assert(kinds[i] == want_kinds[i] && lens[i] == want_lens[i]);
    }
}

int main(void)
{
    /* ropamnet.py 0xB0 0xF0 0102030405060708 i ropamnet.py 0x01 0x2B */
    uint8_t reply[] = { 0xb0, 0xf0, 0x08, 1, 2, 3, 4, 5, 6, 7, 8, 0xc6, 0x04 };
    const uint8_t poll[] = { 0x01, 0x2b, 0x00, 0x28, 0x08 };
    assert(szyna_ramka(reply, sizeof(reply)) && szyna_ramka(poll, sizeof(poll)));
    reply[5] ^= 2;
    assert(!szyna_ramka(reply, sizeof(reply)));
    reply[5] ^= 3; /* bit 0 nie wpływa na CRC, tak jak w firmware'ze */
    assert(szyna_ramka(reply, sizeof(reply)));
    reply[5] ^= 1;

    /* Rozgłoszenie stanu: 220 B danych 0..219, CRC z ropamnet.py. */
    uint8_t state[225] = { 0xfe, 0x10, 220 };
    for (int i = 0; i < 220; i++) {
        state[3 + i] = (uint8_t)i;
    }
    state[223] = 0x6a;
    state[224] = 0xdc;

    /*
     * Ciąg jak na magistrali: śmieci, odpytanie, potwierdzenie, odpowiedź, potwierdzenie,
     * rozgłoszenie, potwierdzenie tuż przed ramką (parser musi wtedy poczekać na dalsze bajty),
     * a na końcu urwana ramka, którą wypuszcza dopiero cisza.
     */
    static struct szyna_parser ps;
    szyna_init(&ps, collect, NULL);
    const uint8_t junk[] = { 0x55, 0x66 }, ack = 0x03;
    uint32_t t = 0;
    feed(&ps, junk, sizeof(junk), &t);
    feed(&ps, poll, sizeof(poll), &t);
    feed(&ps, &ack, 1, &t);
    feed(&ps, reply, sizeof(reply), &t);
    feed(&ps, &ack, 1, &t);
    feed(&ps, state, sizeof(state), &t);
    feed(&ps, &ack, 1, &t);
    feed(&ps, poll, sizeof(poll), &t);
    feed(&ps, reply, 6, &t);
    assert(items == 6); /* potwierdzenie i za nim reszta czekają w buforze */
    szyna_cisza(&ps);
    expect_items("?RARARAR?", (const uint32_t[]){ 2, 5, 1, 13, 1, 225, 1, 5, 6 });
    assert(times[1] == 2 && times[5] == 2 + 5 + 1 + 13 + 1 && ps.n == 0);

    /* Długie śmieci wychodzą kawałkami po SZYNA_SMIECI_MAX. */
    items = 0;
    t = 0;
    for (uint32_t i = 0; i < 2 * SZYNA_SMIECI_MAX + 10; i++) {
        feed(&ps, junk, 1, &t);
    }
    szyna_cisza(&ps);
    expect_items("???", (const uint32_t[]){ SZYNA_SMIECI_MAX, SZYNA_SMIECI_MAX, 10 });

    char line[SZYNA_LINIA_MAX];
    uint32_t n = szyna_linia(line, sizeof(line), 12, 3456, SZYNA_RAMKA, poll, sizeof(poll), SZYNA_E_RAMKA);
    assert(n == 33 && memcmp(line, "12.003456 R 5 !R: 01 2b 00 28 08\n", n) == 0);
    assert(szyna_linia(line, 40, 0, 0, SZYNA_RAMKA, state, sizeof(state), 0) == 0);
    printf("szyna: OK\n");
    return 0;
}
