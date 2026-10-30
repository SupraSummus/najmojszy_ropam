/*
 * Test, dokąd prowadzi D6 (GPIO16): nóżka jako wejście, bez nadawania.
 *
 * Dwie fazy po 3 s: wejście z podciąganiem w dół, potem w górę.
 * W każdej liczy próbki w stanie niskim i zmiany stanu D6 oraz D7 (dla porównania)
 * i wypisuje je na konsolę, potem się zatrzymuje; po nim trzeba wgrać z powrotem most.
 *
 * Jak czytać wynik dla D6:
 *   zmiany w obu fazach        - coś na linii nadaje (TX centrali, przełączany kabel);
 *   niski w dół, wysoki w górę - nóżka wisi w powietrzu;
 *   niski w obu fazach         - masa albo linia trzymana nisko;
 *   wysoki w obu fazach        - linia trzymana wysoko (np. zasilony nadajnik w spoczynku).
 */

#include <stdint.h>

#include "devices.h"
#include "pins.h"
#include "rvuos.h"

#define REGION_IO_MUX 2
#define REGION_GPIO   3

#define GPIO_IN       (GPIO_BASE + 0x3Cu)
#define IO_MUX_PULLDN (1u << 7)
#define IO_MUX_PULLUP (1u << 8)

#define REG(a) (*(volatile uint32_t *)(a))

static void expect(const char *what, uint32_t status)
{
    if (status != KERR_OK) {
        rv_puts(BOOT_CAP_PRINT, "ropam-d6in: ");
        rv_puts(BOOT_CAP_PRINT, what);
        rv_puts(BOOT_CAP_PRINT, ": FAILED\n");
        rv_halt(BOOT_CAP_HALT, 1);
    }
}

static void show(const char *name, uint32_t value)
{
    char dec[12];
    int i = 11;
    dec[i] = '\0';
    do {
        dec[--i] = (char)('0' + value % 10u);
        value /= 10u;
    } while (value != 0 && i > 0);
    rv_puts(BOOT_CAP_PRINT, name);
    rv_puts(BOOT_CAP_PRINT, " ");
    rv_puts(BOOT_CAP_PRINT, &dec[i]);
    rv_puts(BOOT_CAP_PRINT, "\n");
}

static uint64_t now(uint32_t *hz)
{
    uint64_t t;
    expect("read the clock", rv_clock_read(BOOT_CAP_CLOCK, &t, hz));
    return t;
}

static void phase(const char *name, uint32_t pull)
{
    REG(IO_MUX_PIN(16)) = IO_MUX_MATRIX(2) | IO_MUX_INPUT | pull;
    uint32_t hz;
    uint64_t start = now(&hz);
    uint32_t samples = 0, low16 = 0, low17 = 0, edges16 = 0, edges17 = 0;
    uint32_t prev = REG(GPIO_IN);
    while (now(&hz) - start < 3u * (uint64_t)hz) {
        for (int k = 0; k < 256; k++) {
            uint32_t in = REG(GPIO_IN);
            samples++;
            low16 += ((in >> 16) & 1u) == 0;
            low17 += ((in >> 17) & 1u) == 0;
            edges16 += ((in ^ prev) >> 16) & 1u;
            edges17 += ((in ^ prev) >> 17) & 1u;
            prev = in;
        }
    }
    rv_puts(BOOT_CAP_PRINT, name);
    rv_puts(BOOT_CAP_PRINT, "\n");
    show("  probek", samples);
    show("  D6 niski", low16);
    show("  D6 zmian", edges16);
    show("  D7 niski", low17);
    show("  D7 zmian", edges17);
}

int main(void);
int main(void)
{
    expect("map IO MUX", rv_process_install(BOOT_CAP_PROCESS, REGION_IO_MUX, BOOT_CAP_IO_MUX, RIGHT_R | RIGHT_W));
    expect("map GPIO", rv_process_install(BOOT_CAP_PROCESS, REGION_GPIO, BOOT_CAP_GPIO, RIGHT_R | RIGHT_W));

    /* D6 przestaje nadawać: wyjście z matrycy, bez zezwolenia na wyjście. */
    REG(GPIO_ENABLE_W1TC) = 1u << 16;
    REG(GPIO_OUT_SEL(16)) = SIG_GPIO_OUT | GPIO_OE_FROM_ENABLE;

    phase("podciaganie w dol", IO_MUX_PULLDN);
    phase("podciaganie w gore", IO_MUX_PULLUP);
    rv_halt(BOOT_CAP_HALT, 0);
}
