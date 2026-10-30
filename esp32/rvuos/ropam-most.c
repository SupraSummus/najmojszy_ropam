/*
 * Most USB <-> port serwisowy centrali Ropam, program główny rvuos na XIAO ESP32-C6.
 *
 * Bajty od komputera (konsola USB Serial/JTAG, /dev/ttyACM0) idą na UART0,
 * a bajty z UART0 do komputera, bez żadnych zmian;
 * dla komputera XIAO jest więc zwykłym portem szeregowym jak kabel USB-MGSM,
 * i tools/ropam_serwis.py działa z nim bez zmian.
 *
 * UART0 zostaje taki, jak ustawił go ROM: 115200 8N1,
 * TX na GPIO16 (D6) do RX centrali, RX na GPIO17 (D7) od TX centrali.
 * Wymaga rvuos z przydziałem UART0 (BOOT_CAP_UART0), zob. esp32/rvuos/README.md.
 *
 * Program nie korzysta z przerwań: co STEP_US przepycha to, co czeka w obu FIFO.
 * Przy 115200 b/s w ciągu 500 µs przychodzi ok. 6 bajtów,
 * a FIFO odbiorcze UART0 mieści 128.
 */

#include <stdint.h>

#include "console.h"
#include "devices.h"
#include "rvuos.h"

enum {
    SLOT_NTFN = BOOT_CAP_COUNT, /* powiadomienie od timera */
    SLOT_LINE,                  /* linia timera */
    SLOT_TIMER,                 /* Irq na tej linii */
};

/* Regiony procesu za kodem i danymi. */
#define REGION_CONSOLE 2
#define REGION_UART0   3

#define BIT_TIMER 1u
#define STEP_US   500u

/* Rejestry UART0 (ESP-IDF, soc/esp32c6/register/soc/uart_reg.h). */
#define UART_FIFO        0x00u /* odczyt: bajt z RX FIFO, zapis: bajt do TX FIFO */
#define UART_STATUS      0x1Cu
#define UART_RXFIFO_CNT(s) ((s) & 0xFFu)
#define UART_TXFIFO_CNT(s) (((s) >> 16) & 0xFFu)
#define UART_FIFO_SIZE   128u

#define REG(off) (*(volatile uint32_t *)(UART0_BASE + (off)))

static void expect(const char *what, uint32_t status)
{
    if (status != KERR_OK) {
        rv_puts(BOOT_CAP_PRINT, "ropam-most: ");
        rv_puts(BOOT_CAP_PRINT, what);
        rv_puts(BOOT_CAP_PRINT, ": FAILED\n");
        rv_halt(BOOT_CAP_HALT, 1);
    }
}

/* Z UART0 do komputera, póki jest co i gdzie; czy coś przeszło. */
static int uart_to_host(void)
{
    int moved = 0;
    while (UART_RXFIFO_CNT(REG(UART_STATUS)) != 0 &&
           (console_regs[USJ_EP1_CONF] & USJ_FREE) != 0) {
        console_regs[USJ_EP1] = (uint8_t)REG(UART_FIFO);
        moved = 1;
    }
    return moved;
}

/* Od komputera do UART0, póki jest co i jest miejsce w TX FIFO. */
static void host_to_uart(void)
{
    char c;
    while (UART_TXFIFO_CNT(REG(UART_STATUS)) < UART_FIFO_SIZE - 1 && console_get(&c)) {
        REG(UART_FIFO) = (uint8_t)c;
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

    expect("allocate the timer's notification", rv_pool_alloc(BOOT_CAP_POOL, CAP_NOTIFICATION, SLOT_NTFN, 0));
    expect("carve a timer line", rv_irq_carve(BOOT_CAP_TIMER_LINES, 0, 1, SLOT_LINE));
    expect("bind the timer line", rv_irq_bind(SLOT_LINE, BOOT_CAP_POOL, SLOT_NTFN, SLOT_TIMER));

    /* Stare bajty z czasu startu (np. echo tekstu ROM) nie należą do żadnej sesji. */
    while (UART_RXFIFO_CNT(REG(UART_STATUS)) != 0) {
        (void)REG(UART_FIFO);
    }

    for (;;) {
        if (uart_to_host()) {
            console_flush();
        }
        host_to_uart();
        uint32_t skipped, bits;
        expect("keep the step", rv_timer_period(SLOT_TIMER, BIT_TIMER, STEP_US, &skipped));
        expect("wait for it", rv_wait(SLOT_NTFN, &bits));
    }
}
