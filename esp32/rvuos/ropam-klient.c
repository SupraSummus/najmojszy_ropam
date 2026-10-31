/*
 * Klient centrali w systemie Wi-Fi rvuos: proces obok procesu sieciowego, z UART0 i pinami od procesu głównego.
 *
 * Co ROPAM_EVERY_MS czyta stan centrali (ropam.c), zamyka sesję serwisową
 * i publikuje migawkę (bramka.h) w obszarze, który czyta proces sieciowy.
 * Na bajty z UART-u czeka jednorazowym budzikiem po 2 ms, a nie w pętli bez przerwy,
 * żeby nie zabierać czasu radiu; przy każdym obudzeniu odpowiada procesowi głównemu, że żyje.
 * Kolejka odbiorcza UART0 mieści 128 B, czyli ok. 11 ms przy 115200 b/s.
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
    uint32_t ms; /* czas klienta: suma przespanych odcinków, przybliżona */
};

/* Śpi ok. 2 ms (albo krócej, gdy coś obudzi) i odpowiada procesowi głównemu. */
static void rk_nap(struct rk *k)
{
    uint32_t bits;
    rv_timer_set(CHILD_TIMER, CHILD_BIT_TIMER, RK_WAIT_US);
    if (rv_wait(CHILD_INBOX, &bits) != KERR_OK) {
        rv_breakpoint();
    }
    child_answer(&k->page->c.c);
    k->ms += RK_WAIT_US / 1000u;
}

static uint32_t rk_now(void *arg)
{
    return ((struct rk *)arg)->ms;
}

static int rk_get(void *arg, uint32_t ms)
{
    struct rk *k = arg;
    uint32_t start = k->ms;
    for (;;) {
        if (RK_RXFIFO_CNT(RK_REG(RK_UART_STATUS)) != 0) {
            return (int)(RK_REG(RK_UART_FIFO) & 0xFFu);
        }
        if (k->ms - start >= ms) {
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
    struct rk k = { p, 0 };
    child_report(page, CHILD_RUNNING);

    /* TX także na D8, przez matrycę GPIO, jak w ropam-most.c. */
    RK_REG(GPIO_OUT_SEL(RK_PIN_TX_EXTRA)) = RK_SIG_U0TXD | GPIO_OE_FROM_ENABLE;
    RK_REG(GPIO_ENABLE_W1TS) = 1u << RK_PIN_TX_EXTRA;
    RK_REG(IO_MUX_PIN(RK_PIN_TX_EXTRA)) = IO_MUX_MATRIX(2);
    while (RK_RXFIFO_CNT(RK_REG(RK_UART_STATUS)) != 0) {
        (void)RK_REG(RK_UART_FIFO);
    }
    say(&out, "ropam: up\n");

    const struct ropam_io io = { rk_get, rk_put, rk_now, &k };
    struct ropam_snapshot snap;
    uint8_t buf[ROPAM_STATUS_BUF];
    memset(&snap, 0, sizeof(snap));
    for (;;) {
        uint32_t start = k.ms;
        int r = ropam_status(&io, buf);
        if (r == ROPAM_OK) {
            snap.reads++;
            snap.have = 1;
            memcpy(snap.status, buf, ROPAM_STATUS_SIZE);
            if (snap.reads == 1) {
                child_report(page, ROPAM_READING);
            }
        } else {
            snap.errors++;
            say(&out, "ropam: odczyt nieudany, kod -%u\n", (uint32_t)-r);
        }
        snap.last_error = r;
        seqlock_write(&p->shared, &snap);
        while (k.ms - start < ROPAM_EVERY_MS) {
            rk_nap(&k);
        }
    }
}
