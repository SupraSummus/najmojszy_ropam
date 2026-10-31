/*
 * Odczyt stanu centrali przez XIAO, bez komputera po drodze: program główny rvuos.
 *
 * Co 10 s czyta stan (komenda 0x24, ropam.c), zamyka sesję serwisową
 * i wypisuje na konsolę USB stan stref, maski wejść i wyjść.
 * To pierwszy krok bramki: ten sam klient pójdzie potem do procesu obok sieci.
 *
 * UART0 jak w ropam-most.c: 115200 8N1 z ROM-u, RX na D7, TX na D6 i D8.
 */

#include <stdint.h>

#include "console.h"
#include "devices.h"
#include "pins.h"
#include "rvuos.h"
#include "ropam/ropam.c"

#define REGION_CONSOLE 2
#define REGION_UART0   3
#define REGION_IO_MUX  4
#define REGION_GPIO    5

#define UART_FIFO   (UART0_BASE + 0x00u)
#define UART_STATUS (UART0_BASE + 0x1Cu)
#define RXFIFO_CNT(s) ((s) & 0xFFu)
#define TXFIFO_CNT(s) (((s) >> 16) & 0xFFu)
#define PIN_TX_EXTRA 19u
#define SIG_U0TXD    6u

#define REG(a) (*(volatile uint32_t *)(a))

static void expect(const char *what, uint32_t status)
{
    if (status != KERR_OK) {
        rv_puts(BOOT_CAP_PRINT, "ropam-stan: ");
        rv_puts(BOOT_CAP_PRINT, what);
        rv_puts(BOOT_CAP_PRINT, ": FAILED\n");
        rv_halt(BOOT_CAP_HALT, 1);
    }
}

/* Milisekundy od startu, liczone przyrostowo na 32 bitach (bez dzielenia 64-bitowego). */
static uint32_t ms_now(void *arg)
{
    static uint32_t last, rest, ms;
    (void)arg;
    uint64_t t;
    uint32_t hz;
    expect("read the clock", rv_clock_read(BOOT_CAP_CLOCK, &t, &hz));
    uint32_t per_ms = hz / 1000u;
    rest += (uint32_t)t - last;
    last = (uint32_t)t;
    ms += rest / per_ms;
    rest %= per_ms;
    return ms;
}

static int uart_get(void *arg, uint32_t ms)
{
    uint32_t start = ms_now(arg);
    do {
        if (RXFIFO_CNT(REG(UART_STATUS)) != 0) {
            return (int)(REG(UART_FIFO) & 0xFFu);
        }
    } while (ms_now(arg) - start < ms);
    return -1;
}

static void uart_put(void *arg, const uint8_t *data, uint32_t len)
{
    (void)arg;
    for (uint32_t i = 0; i < len; i++) {
        while (TXFIFO_CNT(REG(UART_STATUS)) >= 127u) {
        }
        REG(UART_FIFO) = data[i];
    }
}

/* Na konsolę USB od razu (rv_puts pisze do dziennika kernela, widocznego dopiero przy zatrzymaniu). */
static void say(const char *s)
{
    for (; *s; s++) {
        console_put_polled(*s);
    }
    console_flush();
}

static void put_num(const char *label, uint32_t v, int hex)
{
    char s[12];
    int i = 11;
    s[i] = '\0';
    do {
        uint32_t d = hex ? v % 16u : v % 10u;
        s[--i] = (char)(d < 10 ? '0' + d : 'a' + d - 10);
        v = hex ? v / 16u : v / 10u;
    } while (v != 0 && i > 0);
    say(label);
    if (hex) {
        say("0x");
    }
    say(&s[i]);
}

static uint32_t le32(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

static void show(const uint8_t *st)
{
    put_num("wejscia naruszone ", le32(st + 4), 1);
    put_num(":", le32(st), 1);
    put_num("  sabotaz ", le32(st + 12), 1);
    put_num(":", le32(st + 8), 1);
    put_num("  wyjscia ", le32(st + 0x30), 1);
    say("\n");
    for (uint32_t z = 0; z < 4; z++) {
        uint32_t w = le32(st + 0x4C + 8 * z);
        put_num("  strefa ", z + 1, 0);
        say((w >> 11) & 1u ? " czuwanie nocne" : (w >> 10) & 1u ? " czuwanie" :
                                (w & 0xFu) == 3u ? " rozbrojona" : " inny stan");
        put_num(" ", w, 1);
        say("\n");
    }
}

int main(void);
int main(void)
{
    uint32_t base, size;
    expect("console info", rv_frame_info(BOOT_CAP_UART, &base, &size));
    expect("map the console", rv_process_install(BOOT_CAP_PROCESS, REGION_CONSOLE, BOOT_CAP_UART, RIGHT_R | RIGHT_W));
    console_init(base);
    expect("map UART0", rv_process_install(BOOT_CAP_PROCESS, REGION_UART0, BOOT_CAP_UART0, RIGHT_R | RIGHT_W));
    expect("map IO MUX", rv_process_install(BOOT_CAP_PROCESS, REGION_IO_MUX, BOOT_CAP_IO_MUX, RIGHT_R | RIGHT_W));
    expect("map GPIO", rv_process_install(BOOT_CAP_PROCESS, REGION_GPIO, BOOT_CAP_GPIO, RIGHT_R | RIGHT_W));
    REG(GPIO_OUT_SEL(PIN_TX_EXTRA)) = SIG_U0TXD | GPIO_OE_FROM_ENABLE;
    REG(GPIO_ENABLE_W1TS) = 1u << PIN_TX_EXTRA;
    REG(IO_MUX_PIN(PIN_TX_EXTRA)) = IO_MUX_MATRIX(2);
    while (RXFIFO_CNT(REG(UART_STATUS)) != 0) {
        (void)REG(UART_FIFO);
    }

    const struct ropam_io io = { uart_get, uart_put, ms_now, 0 };
    static uint8_t status[ROPAM_STATUS_BUF];
    say("ropam-stan: start\n");
    for (uint32_t n = 1;; n++) {
        uint32_t t0 = ms_now(0);
        int r = ropam_status(&io, status);
        put_num("odczyt ", n, 0);
        put_num(" w ", ms_now(0) - t0, 0);
        say(" ms: ");
        if (r == ROPAM_OK) {
            say("ok\n");
            show(status);
        } else {
            put_num("blad ", (uint32_t)-r, 0);
            say("\n");
        }
        uint32_t t1 = ms_now(0);
        while (ms_now(0) - t1 < 10000u) {
        }
    }
}
