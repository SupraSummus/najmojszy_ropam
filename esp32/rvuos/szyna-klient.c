/*
 * Klient magistrali RopamNET w systemie Wi-Fi rvuos: proces obok procesu sieciowego, z UART0 od procesu głównego.
 *
 * Tylko słucha (jak ropam-podsluch.c, piny w rs485.c); do UART-u nic nie pisze.
 * Strumień dzieli na ramki parserem z szyna.c i publikuje migawkę (bramka.h)
 * z ostatniego rozgłoszenia stanu (0x10) i ostatniego raportu panelu (0x11).
 * Na bajty czeka jednorazowym budzikiem po 2 ms; kolejka odbiorcza UART0 mieści 128 B,
 * czyli ok. 11 ms przy 115200 b/s.
 *
 * Piny i polaryzację ustawia proces główny zaraz po starcie (ropam_pins):
 * ten proces ma już wszystkie osiem regionów, więc rejestrów IO MUX i GPIO nie dostaje.
 *
 * Dołączany do obrazu systemu Wi-Fi przez netproc.c (#include), bo budowanie rvuos kompiluje jego pliki.
 */

#include "ropam/bramka.h"
#include "ropam/rs485.c"
#include "ropam/szyna.c"

#define SK_UART_FIFO    (UART0_BASE + 0x00u)
#define SK_UART_RX_FILT (UART0_BASE + 0x18u)
#define SK_UART_STATUS  (UART0_BASE + 0x1Cu)
#define SK_UART_UPDATE  (UART0_BASE + 0x98u)
#define SK_GLITCH_FILT  (64u | (1u << 8)) /* impulsy krótsze niż 1,6 µs; bit trwa 8,7 µs */
#define SK_RXFIFO_CNT(s) ((s) & 0xFFu)
#define SK_WAIT_US      2000u
#define SK_CISZA_MS     3000u /* zob. CISZA_US w ropam-podsluch.c */
#define SK_REG(a) (*(volatile uint32_t *)(a))

/* Woła proces główny z zamapowanymi IO MUX i GPIO (wifi.h); 1, gdy wejście odwrócone. */
uint32_t ropam_pins(void)
{
    rs485_receive_only();
    uint32_t inv = rs485_idle_high_permille() < 500u;
    rs485_set_inverted(inv);
    return inv;
}

struct sk {
    struct ropam_page *page;
    struct ropam_snapshot snap;
    uint32_t per_ms;     /* takty licznika na milisekundę */
    uint32_t last, rest; /* ostatni odczyt licznika i takty jeszcze niezliczone w ms */
    uint32_t ms;         /* milisekundy od startu klienta */
    int dirty;           /* migawka zmieniona od ostatniej publikacji */
};

/* Milisekundy z licznika, przyrostowo na 32 bitach (bez dzielenia 64-bitowego). */
static uint32_t sk_now(struct sk *k)
{
    uint32_t t = SK_REG(k->page->counter);
    k->rest += t - k->last;
    k->last = t;
    k->ms += k->rest / k->per_ms;
    k->rest %= k->per_ms;
    return k->ms;
}

/* Pozycja z parsera: ramki stanu i panelu do migawki, reszta tylko do liczników. */
static void sk_item(void *arg, char rodzaj, const uint8_t *p, uint32_t n, uint32_t s, uint32_t us, uint32_t bledy)
{
    struct sk *k = arg;
    (void)s;
    (void)us;
    (void)bledy;
    if (rodzaj == SZYNA_INNE) {
        k->snap.junk += n;
        return;
    }
    if (rodzaj != SZYNA_RAMKA) {
        return;
    }
    k->snap.frames++;
    if (p[0] == 0xFEu && p[1] == 0x10u && n == 5u + ROPAM_HEADER_SIZE) {
        memcpy(k->snap.status, p + 3, ROPAM_HEADER_SIZE);
        k->snap.have = 1;
        k->snap.state_ms = k->ms;
        k->snap.states++;
        k->dirty = 1;
    } else if (p[0] == 0xAAu && p[1] == 0x11u && n == 5u + ROPAM_PANEL_SIZE) {
        /* Jedyny panel z adresem w obiegu; przy kilku panelach trzeba by brać adres z zapytania. */
        memcpy(k->snap.panel, p + 3, ROPAM_PANEL_SIZE);
        k->snap.panel_have = 1;
        k->dirty = 1;
    }
}

/* Śpi ok. 2 ms (albo krócej, gdy coś obudzi) i odpowiada procesowi głównemu. */
static void sk_nap(struct sk *k)
{
    uint32_t bits;
    rv_timer_set(CHILD_TIMER, CHILD_BIT_TIMER, SK_WAIT_US);
    if (rv_wait(CHILD_INBOX, &bits) != KERR_OK) {
        rv_breakpoint();
    }
    child_answer(&k->page->c.c);
}

__attribute__((noreturn)) void ropam_main(struct child_page *page)
{
    struct ropam_page *p = (struct ropam_page *)page;
    struct out out = child_out();
    struct sk k;
    struct szyna_parser parser;
    memset(&k, 0, sizeof(k));
    k.page = p;
    k.per_ms = p->counter_hz / 1000u;
    k.last = SK_REG(p->counter);
    szyna_init(&parser, sk_item, &k);
    child_report(page, CHILD_RUNNING);

    SK_REG(SK_UART_RX_FILT) = SK_GLITCH_FILT;
    SK_REG(SK_UART_UPDATE) = 1u;
    for (uint32_t i = 0; i < 1000u && (SK_REG(SK_UART_UPDATE) & 1u); i++) {
    }
    while (SK_RXFIFO_CNT(SK_REG(SK_UART_STATUS)) != 0) {
        (void)SK_REG(SK_UART_FIFO);
    }
    say(&out, "ropam: up, magistrala, tylko odbior\n");

    uint32_t last_byte = sk_now(&k), reported = last_byte, announced = 0, published = 0;
    for (;;) {
        uint32_t now = sk_now(&k);
        if (SK_RXFIFO_CNT(SK_REG(SK_UART_STATUS)) != 0) {
            while (SK_RXFIFO_CNT(SK_REG(SK_UART_STATUS)) != 0) {
                szyna_bajt(&parser, (uint8_t)SK_REG(SK_UART_FIFO), now / 1000u, now % 1000u * 1000u, 0);
                k.snap.bytes++;
            }
            last_byte = now;
        } else if (parser.n != 0 && now - last_byte >= SK_CISZA_MS) {
            szyna_cisza(&parser);
        }
        /* Co sekundę także bez nowej ramki: liczniki i wiek stanu idą dalej, nawet gdy magistrala milczy. */
        if (k.dirty || now - published >= 1000u) {
            published = now;
            k.snap.now_ms = now;
            if (k.snap.have && !announced) {
                announced = 1;
                child_report(page, ROPAM_READING);
                say(&out, "ropam: pierwszy stan z magistrali po %u ms\n", now);
            }
            seqlock_write(&p->shared, &k.snap);
            k.dirty = 0;
        }
        if (now - reported >= 600000u) {
            reported = now;
            say(&out, "ropam: bajty %u, ramki %u, stany %u, bajty smieci %u\n", k.snap.bytes, k.snap.frames,
                k.snap.states, k.snap.junk);
        }
        sk_nap(&k);
    }
}
