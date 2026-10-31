/*
 * Test ropam.c na komputerze, z udawaną centralą i udawanym czasem.
 *
 * cc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -I esp32/rvuos \
 *    esp32/rvuos/test/ropam-test.c esp32/rvuos/ropam.c
 *
 * Centrala zaprasza 'C' co 2 s, przyjmuje blok komendy (ACK) i EOT (ACK),
 * po czym ustawia flagę sesji i odsyła stan (1100 B w dwóch blokach po 1024 B), gdy odbiorca wyśle 'C'.
 * Przerwany odbiór (CAN CAN) potwierdza ACK-iem i od razu zaprasza znowu;
 * trzy takie puste cykle z rzędu gaszą flagę, jak w task_service.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "ropam.h"

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define B64  0x19
#define CRC_REQ 'C'

enum mode { INVITING, SENDING_WAIT_C, SENDING, SILENT };

struct panel {
    uint32_t now;
    /* bajty od centrali, każdy z czasem, od którego jest do odebrania */
    uint8_t q[4096];
    uint32_t q_at[4096], q_head, q_tail;
    enum mode mode;
    uint32_t next_invite;
    uint8_t in[2048]; /* bajty od odbiorcy, jeszcze nie złożone w całość */
    uint32_t in_len;
    int block_ok;     /* przyszedł dobry blok komendy, czekamy na EOT */
    uint8_t cmd;
    uint32_t sent_blocks, cancels;
    int flag;
    uint32_t flag_set_at, flag_ms; /* łączny czas z ustawioną flagą */
    int silent_after_cmd;          /* awaria: po komendzie milknie na tyle ms */
    int reject_blocks;             /* awaria: tyle bloków komendy kwituje nowym 'C' zamiast ACK */
    uint32_t damage_block, damage_at; /* awaria: w tym bloku stanu (1, 2) psuje bajt danych nr damage_at */
    int damage_drop;               /* ... gubiąc go zamiast przekłamać, albo (-n) wstawiając przed nim n śmieci */
    uint32_t naks;
    uint32_t silent_until;
    uint32_t commands;
};

static uint16_t crc16(const uint8_t *p, uint32_t len)
{
    uint32_t crc = 0;
    while (len--) {
        crc ^= (uint32_t)*p++ << 8;
        for (int i = 0; i < 8; i++) {
            crc = (crc & 0x8000u) ? (crc << 1) ^ 0x1021u : crc << 1;
        }
    }
    return (uint16_t)crc;
}

static void emit(struct panel *p, uint8_t b, uint32_t delay)
{
    p->q[p->q_tail % 4096] = b;
    p->q_at[p->q_tail % 4096] = p->now + delay;
    p->q_tail++;
}

static void flag_on(struct panel *p)
{
    if (!p->flag) {
        p->flag = 1;
        p->flag_set_at = p->now;
    }
    p->cancels = 0;
}

static void flag_off(struct panel *p)
{
    if (p->flag) {
        p->flag = 0;
        p->flag_ms += p->now - p->flag_set_at;
    }
}

/* Blok stanu nr n (1 albo 2): 1024 B, w drugim reszta 1100 B i wypełnienie. */
static void send_block(struct panel *p, uint32_t n)
{
    uint8_t data[1024];
    for (uint32_t i = 0; i < 1024; i++) {
        uint32_t at = (n - 1) * 1024 + i;
        data[i] = at < ROPAM_STATUS_SIZE ? (uint8_t)(at * 7u + 1u) : 0x1A;
    }
    uint16_t crc = crc16(data, 1024);
    emit(p, STX, 1);
    emit(p, (uint8_t)n, 1);
    emit(p, (uint8_t)~n, 1);
    for (uint32_t i = 0; i < 1024; i++) {
        if (n == p->damage_block && i == p->damage_at) {
            p->damage_block = 0; /* tylko raz */
            for (int g = 0; g < -p->damage_drop; g++) {
                emit(p, 0xFF, 1 + i / 12u);
            }
            if (p->damage_drop < 0) {
                emit(p, data[i], 1 + i / 12u);
                continue;
            }
            if (p->damage_drop) {
                continue;
            }
            emit(p, (uint8_t)(data[i] ^ 0x10), 1 + i / 12u);
            continue;
        }
        emit(p, data[i], 1 + i / 12u); /* ok. 87 µs na bajt */
    }
    emit(p, (uint8_t)(crc >> 8), 90);
    emit(p, (uint8_t)crc, 90);
}

static void on_byte(struct panel *p, uint8_t b)
{
    switch (p->mode) {
    case SILENT:
        return;
    case SENDING_WAIT_C:
        if (b == CRC_REQ) {
            p->mode = SENDING;
            p->sent_blocks = 1;
            send_block(p, 1);
        }
        return;
    case SENDING:
        if (b == NAK && p->sent_blocks <= 2) {
            if (++p->naks > 5) {
                emit(p, CAN, 1);
                emit(p, CAN, 1);
                p->mode = INVITING;
                p->next_invite = p->now + 100;
                return;
            }
            send_block(p, p->sent_blocks);
            return;
        }
        if (b == ACK && p->sent_blocks == 1) {
            p->sent_blocks = 2;
            send_block(p, 2);
        } else if (b == ACK && p->sent_blocks == 2) {
            p->sent_blocks = 3;
            emit(p, EOT, 1);
        } else if (b == ACK && p->sent_blocks == 3) {
            p->mode = INVITING;
            emit(p, CRC_REQ, 12); /* następne zaproszenie zaraz po transmisji */
            p->next_invite = p->now + 12 + 2000;
        } else if (b == CAN) {
            p->mode = INVITING;
            p->next_invite = p->now + 100;
        }
        return;
    case INVITING:
        break;
    }
    p->in[p->in_len++] = b;
    if (p->in_len == 2 && p->in[0] == CAN && p->in[1] == CAN) {
        p->in_len = 0;
        emit(p, ACK, 1);
        emit(p, CRC_REQ, 2);
        p->next_invite = p->now + 2 + 2000;
        if (++p->cancels >= 3) {
            flag_off(p);
        }
        return;
    }
    if (p->in[0] == EOT && p->block_ok) {
        p->in_len = 0;
        p->block_ok = 0;
        emit(p, ACK, 1);
        p->commands++;
        flag_on(p);
        if (p->silent_after_cmd) {
            p->mode = SILENT;
            p->silent_until = p->now + (uint32_t)p->silent_after_cmd;
        } else {
            p->mode = SENDING_WAIT_C;
        }
        return;
    }
    if (p->in[0] == B64 && p->in_len == 3 + 64 + 2) {
        p->in_len = 0;
        uint16_t crc = crc16(p->in + 3, 64);
        int ok = p->in[1] == 1 && p->in[2] == 0xFE && crc == ((uint16_t)p->in[67] << 8 | p->in[68]);
        p->cmd = p->in[3];
        if (ok && p->reject_blocks > 0) {
            p->reject_blocks--;
            p->block_ok = 0;
            emit(p, CRC_REQ, 8); /* jak przy linii RX wiszącej w powietrzu */
            p->next_invite = p->now + 8 + 2000;
            return;
        }
        p->block_ok = ok;
        emit(p, ok ? ACK : NAK, 8);
        return;
    }
    if (p->in[0] != B64 && p->in[0] != CAN && p->in[0] != EOT) {
        p->in_len = 0; /* śmieci */
    }
    if (p->in_len >= sizeof(p->in)) {
        p->in_len = 0;
    }
}

/* Udawany czas płynie tylko tutaj: do najbliższego bajtu centrali albo do końca czekania. */
static int p_get(void *arg, uint32_t ms)
{
    struct panel *p = arg;
    uint32_t until = p->now + ms;
    for (;;) {
        if (p->mode == SILENT && p->now >= p->silent_until) {
            p->mode = INVITING;
            p->next_invite = p->now;
        }
        if (p->mode == INVITING && p->q_head == p->q_tail && p->next_invite <= until) {
            uint32_t at = p->next_invite > p->now ? p->next_invite : p->now;
            uint32_t was = p->now;
            p->now = at;
            emit(p, CRC_REQ, 0);
            p->now = was;
            p->next_invite = at + 2000;
        }
        if (p->q_head != p->q_tail && p->q_at[p->q_head % 4096] <= until) {
            uint32_t at = p->q_at[p->q_head % 4096];
            if (at > p->now) {
                p->now = at;
            }
            return p->q[p->q_head++ % 4096];
        }
        if (p->mode == SILENT && p->silent_until <= until) {
            p->now = p->silent_until;
            continue;
        }
        p->now = until;
        return -1;
    }
}

static void p_put(void *arg, const uint8_t *data, uint32_t len)
{
    struct panel *p = arg;
    for (uint32_t i = 0; i < len; i++) {
        on_byte(p, data[i]);
    }
}

static uint32_t p_now(void *arg)
{
    return ((struct panel *)arg)->now;
}

static void start(struct panel *p)
{
    memset(p, 0, sizeof(*p));
    p->mode = INVITING;
    p->next_invite = 700;
}

int main(void)
{
    static struct panel p;
    static uint8_t buf[ROPAM_STATUS_BUF];
    const struct ropam_io io = { p_get, p_put, p_now, &p };

    /* Zwykły odczyt: stan się zgadza, flaga zgaszona, i to szybko. */
    start(&p);
    int r = ropam_status(&io, buf);
    assert(r == ROPAM_OK);
    for (uint32_t i = 0; i < ROPAM_STATUS_SIZE; i++) {
        assert(buf[i] == (uint8_t)(i * 7u + 1u));
    }
    assert(p.commands == 1 && p.cmd == 0x24 && !p.flag);
    printf("odczyt: %u ms, flaga sesji przez %u ms\n", p.now, p.flag_ms);
    assert(p.flag_ms < 500);

    /* Kilka odczytów pod rząd: za każdym razem to samo. */
    for (int i = 0; i < 5; i++) {
        assert(ropam_status(&io, buf) == ROPAM_OK && !p.flag);
    }
    printf("6 odczytów: %u ms, flaga sesji łącznie %u ms\n", p.now, p.flag_ms);

    /* Centrala odrzuca dwa pierwsze bloki nowym 'C': klient ponawia od razu, nie po 2 s. */
    start(&p);
    p.reject_blocks = 2;
    r = ropam_status(&io, buf);
    printf("dwa bloki odrzucone: kod %d, %u ms\n", r, p.now);
    assert(r == ROPAM_OK && p.commands == 1 && !p.flag && p.now < 1000);

    /* Przekłamany albo zgubiony bajt w bloku stanu: jedno ponowienie bloku, odczyt udany. */
    static const struct { uint32_t block, at; int drop; } damage[] = {
        { 1, 10, 0 }, { 1, 500, 1 }, { 2, 3, 1 }, { 2, 1000, 0 }, { 1, 1023, 1 },
        /* Dołożone bajty: blok kończy się wcześniej, a jego ogon (tu z 0x18 i 0x04 w danych) jest jeszcze w drodze. */
        { 1, 10, -150 }, { 1, 700, -400 }, { 2, 5, -100 },
    };
    for (uint32_t i = 0; i < sizeof(damage) / sizeof(damage[0]); i++) {
        start(&p);
        p.damage_block = damage[i].block;
        p.damage_at = damage[i].at;
        p.damage_drop = damage[i].drop;
        r = ropam_status(&io, buf);
        printf("blok %u, bajt %u %s: kod %d, %u ms, NAK %u\n", damage[i].block, damage[i].at,
               damage[i].drop > 0 ? "zgubiony" : damage[i].drop < 0 ? "ze śmieciami" : "przekłamany", r, p.now, p.naks);
        assert(r == ROPAM_OK && p.naks == 1 && !p.flag);
        for (uint32_t j = 0; j < ROPAM_STATUS_SIZE; j++) {
            assert(buf[j] == (uint8_t)(j * 7u + 1u));
        }
    }

    /* Centrala przyjmuje komendę i milknie na 15 s: błąd, ale flaga i tak zgaszona. */
    start(&p);
    p.silent_after_cmd = 15000;
    r = ropam_status(&io, buf);
    printf("milczenie po komendzie: kod %d, %u ms, flaga %d\n", r, p.now, p.flag);
    assert(r == ROPAM_NO_DATA && p.commands == 1 && !p.flag);

    /* Centrala w ogóle nie zaprasza: brak zaproszenia, bez prób kończenia sesji. */
    start(&p);
    p.mode = SILENT;
    p.silent_until = 0xFFFFFFFFu;
    r = ropam_status(&io, buf);
    printf("cisza: kod %d po %u ms\n", r, p.now);
    assert(r == ROPAM_NO_INVITE && p.commands == 0 && p.now <= 10100);

    printf("ropam-test: ok\n");
    return 0;
}
