/*
 * Klient centrali w systemie Wi-Fi rvuos: proces obok procesu sieciowego, z UART0 od procesu głównego.
 *
 * Co ROPAM_EVERY_MS czyta stan centrali (ropam.c), zamyka sesję serwisową
 * i publikuje migawkę (bramka.h) w obszarze, który czyta proces sieciowy.
 * Na bajty z UART-u czeka jednorazowym budzikiem po 2 ms, a nie w pętli bez przerwy,
 * żeby nie zabierać czasu radiu; przy każdym obudzeniu odpowiada procesowi głównemu, że żyje.
 * Kolejka odbiorcza UART0 mieści 128 B, czyli ok. 11 ms przy 115200 b/s.
 * Czas bierze z licznika (BOOT_CAP_COUNTER), który proces główny mapuje mu razem z UART0.
 *
 * Piny ustawia proces główny zaraz po starcie (ropam_pins), zanim system dołączy do sieci:
 * póki D8 nie nadaje, wejście RX centrali wisi w powietrzu, a centrala bierze to za śmieci.
 *
 * Dołączany do obrazu systemu Wi-Fi przez netproc.c (#include), bo budowanie rvuos kompiluje jego pliki.
 */

#include "ropam/bramka.h"
#include "ropam/ropam.c"

#define RK_UART_FIFO   (UART0_BASE + 0x00u)
#define RK_UART_STATUS (UART0_BASE + 0x1Cu)
#define RK_RXFIFO_CNT(s) ((s) & 0xFFu)
#define RK_TXFIFO_CNT(s) (((s) >> 16) & 0xFFu)
#define RK_PIN_TX_EXTRA 19u /* D8 */
#define RK_SIG_U0TXD    6u
#define RK_WAIT_US      2000u
#define RK_REG(a) (*(volatile uint32_t *)(a))

struct rk {
    struct ropam_page *page;
    uint32_t per_ms;    /* takty licznika na milisekundę */
    uint32_t last, rest; /* ostatni odczyt licznika i takty jeszcze niezliczone w ms */
    uint32_t ms;        /* milisekundy od startu klienta */
};

/* TX UART0 także na D8, przez matrycę GPIO; woła proces główny z zamapowanymi IO MUX i GPIO. */
void ropam_pins(void)
{
    RK_REG(GPIO_OUT_SEL(RK_PIN_TX_EXTRA)) = RK_SIG_U0TXD | GPIO_OE_FROM_ENABLE;
    RK_REG(GPIO_ENABLE_W1TS) = 1u << RK_PIN_TX_EXTRA;
    RK_REG(IO_MUX_PIN(RK_PIN_TX_EXTRA)) = IO_MUX_MATRIX(2);
}

/* Milisekundy z licznika, przyrostowo na 32 bitach (bez dzielenia 64-bitowego). */
static uint32_t rk_now(void *arg)
{
    struct rk *k = arg;
    uint32_t t = RK_REG(k->page->counter);
    k->rest += t - k->last;
    k->last = t;
    k->ms += k->rest / k->per_ms;
    k->rest %= k->per_ms;
    return k->ms;
}

/* Śpi ok. 2 ms (albo krócej, gdy coś obudzi) i odpowiada procesowi głównemu. */
static void rk_nap(struct rk *k)
{
    uint32_t bits;
    rv_timer_set(CHILD_TIMER, CHILD_BIT_TIMER, RK_WAIT_US);
    if (rv_wait(CHILD_INBOX, &bits) != KERR_OK) {
        rv_breakpoint();
    }
    child_answer(&k->page->c.c);
}

static int rk_get(void *arg, uint32_t ms)
{
    struct rk *k = arg;
    uint32_t start = rk_now(k);
    for (;;) {
        if (RK_RXFIFO_CNT(RK_REG(RK_UART_STATUS)) != 0) {
            return (int)(RK_REG(RK_UART_FIFO) & 0xFFu);
        }
        if (rk_now(k) - start >= ms) {
            return -1;
        }
        rk_nap(k);
    }
}

static void rk_put(void *arg, const uint8_t *data, uint32_t len)
{
    struct rk *k = arg;
    for (uint32_t i = 0; i < len; i++) {
        while (RK_TXFIFO_CNT(RK_REG(RK_UART_STATUS)) >= 127u) {
            rk_nap(k);
        }
        RK_REG(RK_UART_FIFO) = data[i];
    }
}

__attribute__((noreturn)) void ropam_main(struct child_page *page)
{
    struct ropam_page *p = (struct ropam_page *)page;
    struct out out = child_out();
    struct rk k;
    memset(&k, 0, sizeof(k));
    k.page = p;
    k.per_ms = p->counter_hz / 1000u;
    k.last = RK_REG(p->counter);
    child_report(page, CHILD_RUNNING);

    while (RK_RXFIFO_CNT(RK_REG(RK_UART_STATUS)) != 0) {
        (void)RK_REG(RK_UART_FIFO);
    }
    say(&out, "ropam: up\n");

    const struct ropam_io io = { rk_get, rk_put, rk_now, &k };
    struct ropam_snapshot snap;
    uint8_t buf[ROPAM_STATUS_BUF];
    memset(&snap, 0, sizeof(snap));
    for (;;) {
        uint32_t start = rk_now(&k);
        int r = ropam_status(&io, buf);
        if (r == ROPAM_OK) {
            snap.reads++;
            snap.have = 1;
            memcpy(snap.status, buf, ROPAM_STATUS_SIZE);
            if (snap.reads == 1) {
                child_report(page, ROPAM_READING);
                say(&out, "ropam: pierwszy odczyt w %u ms\n", rk_now(&k) - start);
            }
        } else {
            snap.errors++;
            say(&out, "ropam: odczyt nieudany, kod -%u\n", (uint32_t)-r);
        }
        snap.last_error = r;
        seqlock_write(&p->shared, &snap);
        while (rk_now(&k) - start < ROPAM_EVERY_MS) {
            rk_nap(&k);
        }
    }
}
