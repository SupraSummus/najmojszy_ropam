/*
 * Diagnostyka nóżek mostu: jak ROM zostawił GPIO16 (D6, TX) i GPIO17 (D7, RX).
 *
 * Wypisuje konfigurację IO MUX, matrycy GPIO i UART0,
 * potem przez ok. 50 ms próbkuje poziomy obu wyprowadzeń
 * i mówi, ile razy każde zmieniło stan.
 * Niczego nie zmienia i niczego nie nadaje.
 */

#include <stdint.h>

#include "devices.h"
#include "pins.h"
#include "rvuos.h"

#define REGION_IO_MUX 2
#define REGION_GPIO   3
#define REGION_UART0  4

#define GPIO_OUT    (GPIO_BASE + 0x04u)
#define GPIO_ENABLE (GPIO_BASE + 0x20u)
#define GPIO_IN     (GPIO_BASE + 0x3Cu)
#define SIG_U0RXD   6u

#define REG(a) (*(volatile uint32_t *)(a))

static void expect(const char *what, uint32_t status)
{
    if (status != KERR_OK) {
        rv_puts(BOOT_CAP_PRINT, "ropam-piny: ");
        rv_puts(BOOT_CAP_PRINT, what);
        rv_puts(BOOT_CAP_PRINT, ": FAILED\n");
        rv_halt(BOOT_CAP_HALT, 1);
    }
}

static void show(const char *name, uint32_t value)
{
    char hex[12] = "0x";
    for (int i = 0; i < 8; i++) {
        hex[2 + i] = "0123456789abcdef"[(value >> (28 - 4 * i)) & 0xFu];
    }
    hex[10] = '\n';
    hex[11] = '\0';
    rv_puts(BOOT_CAP_PRINT, name);
    rv_puts(BOOT_CAP_PRINT, " ");
    rv_puts(BOOT_CAP_PRINT, hex);
}

int main(void);
int main(void)
{
    expect("map IO MUX", rv_process_install(BOOT_CAP_PROCESS, REGION_IO_MUX, BOOT_CAP_IO_MUX, RIGHT_R));
    expect("map GPIO", rv_process_install(BOOT_CAP_PROCESS, REGION_GPIO, BOOT_CAP_GPIO, RIGHT_R));
    expect("map UART0", rv_process_install(BOOT_CAP_PROCESS, REGION_UART0, BOOT_CAP_UART0, RIGHT_R));

    show("IO_MUX_GPIO16", REG(IO_MUX_PIN(16)));
    show("IO_MUX_GPIO17", REG(IO_MUX_PIN(17)));
    show("OUT_SEL16", REG(GPIO_OUT_SEL(16)));
    show("OUT_SEL17", REG(GPIO_OUT_SEL(17)));
    show("IN_SEL_U0RXD", REG(GPIO_IN_SEL(SIG_U0RXD)));
    show("GPIO_OUT", REG(GPIO_OUT));
    show("GPIO_ENABLE", REG(GPIO_ENABLE));
    show("GPIO_IN", REG(GPIO_IN));
    show("UART0_CONF0", REG(UART0_BASE + 0x20u));
    show("UART0_STATUS", REG(UART0_BASE + 0x1Cu));

    uint32_t prev = REG(GPIO_IN), edges16 = 0, edges17 = 0, low16 = 0, low17 = 0;
    for (uint32_t n = 0; n < 200000u; n++) {
        uint32_t in = REG(GPIO_IN);
        edges16 += ((in ^ prev) >> 16) & 1u;
        edges17 += ((in ^ prev) >> 17) & 1u;
        low16 += ((in >> 16) & 1u) == 0;
        low17 += ((in >> 17) & 1u) == 0;
        prev = in;
    }
    show("zmiany_D6", edges16);
    show("zmiany_D7", edges17);
    show("niskie_D6", low16);
    show("niskie_D7", low17);
    rv_halt(BOOT_CAP_HALT, 0);
}
