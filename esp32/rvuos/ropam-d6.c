/*
 * Test, czy D6 (GPIO16) w ogóle steruje linią RX centrali.
 *
 * Działa jak ropam-most w jedną stronę (z UART0 do komputera, wejście z komputera pomija),
 * a w drugiej fazie przez 2 s trzyma D6 w stanie niskim przez matrycę GPIO.
 * Fazy: 2 s spoczynku, 2 s stanu niskiego, potem spoczynek bez końca;
 * na początku każdej komputer dostaje znacznik "\n#D6=1\n" albo "\n#D6=0\n".
 * Stan niski centrala odbiera jako bajty 0x00 z błędem ramki,
 * które jej odbiornik XMODEM pomija.
 */

#include <stdint.h>

#include "console.h"
#include "devices.h"
#include "pins.h"
#include "rvuos.h"

enum {
    SLOT_NTFN = BOOT_CAP_COUNT,
    SLOT_LINE,
    SLOT_TIMER,
};

#define REGION_CONSOLE 2
#define REGION_UART0   3
#define REGION_IO_MUX  4
#define REGION_GPIO    5

#define BIT_TIMER 1u
#define STEP_US   500u
#define PHASE     4000u /* kroków po 500 µs, czyli 2 s */

#define UART_FIFO   (UART0_BASE + 0x00u)
#define UART_STATUS (UART0_BASE + 0x1Cu)
#define REG(a) (*(volatile uint32_t *)(a))

static void expect(const char *what, uint32_t status)
{
    if (status != KERR_OK) {
        rv_puts(BOOT_CAP_PRINT, "ropam-d6: ");
        rv_puts(BOOT_CAP_PRINT, what);
        rv_puts(BOOT_CAP_PRINT, ": FAILED\n");
        rv_halt(BOOT_CAP_HALT, 1);
    }
}

static void mark(const char *text)
{
    for (; *text; text++) {
        while ((console_regs[USJ_EP1_CONF] & USJ_FREE) == 0) {
            console_flush();
        }
        console_regs[USJ_EP1] = (uint8_t)*text;
    }
    console_flush();
}

static void d6_low(void)
{
    REG(GPIO_OUT_W1TC) = 1u << 16;
    REG(GPIO_OUT_SEL(16)) = SIG_GPIO_OUT | GPIO_OE_FROM_ENABLE;
    REG(GPIO_ENABLE_W1TS) = 1u << 16;
    REG(IO_MUX_PIN(16)) = IO_MUX_MATRIX(2) | IO_MUX_INPUT;
}

static void d6_uart(uint32_t mux)
{
    REG(IO_MUX_PIN(16)) = mux;
    REG(GPIO_ENABLE_W1TC) = 1u << 16;
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

    expect("allocate the timer's notification", rv_pool_alloc(BOOT_CAP_POOL, CAP_NOTIFICATION, SLOT_NTFN, 0));
    expect("carve a timer line", rv_irq_carve(BOOT_CAP_TIMER_LINES, 0, 1, SLOT_LINE));
    expect("bind the timer line", rv_irq_bind(SLOT_LINE, BOOT_CAP_POOL, SLOT_NTFN, SLOT_TIMER));

    uint32_t mux = REG(IO_MUX_PIN(16));
    for (uint32_t step = 0;; step++) {
        if (step == 0) {
            mark("\n#D6=1\n");
        } else if (step == PHASE) {
            d6_low();
            mark("\n#D6=0\n");
        } else if (step == 2 * PHASE) {
            d6_uart(mux);
            mark("\n#D6=1\n");
        }
        int moved = 0;
        while ((REG(UART_STATUS) & 0xFFu) != 0 && (console_regs[USJ_EP1_CONF] & USJ_FREE) != 0) {
            console_regs[USJ_EP1] = (uint8_t)REG(UART_FIFO);
            moved = 1;
        }
        if (moved) {
            console_flush();
        }
        char c;
        while (console_get(&c)) {
        }
        uint32_t skipped, bits;
        expect("keep the step", rv_timer_period(SLOT_TIMER, BIT_TIMER, STEP_US, &skipped));
        expect("wait for it", rv_wait(SLOT_NTFN, &bits));
    }
}
