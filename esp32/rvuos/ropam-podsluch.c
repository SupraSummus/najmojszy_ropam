/*
 * Bierny podsłuch magistrali RopamNET przez transceiver RS-485 na XIAO ESP32-C6, program główny rvuos.
 *
 * Piny i polaryzacja: rs485.c. Program nigdy nie podnosi RE/DE i nie pisze do UART-u,
 * czyli niczego nie nadaje na magistralę (CLAUDE.md, zasady bezpieczeństwa).
 * UART0 zostaje taki, jak ustawił go ROM: 115200 8N1, jak magistrala.
 *
 * Pętla bez budzika (jak w ropam-most.c) dzieli strumień na ramki parserem z szyna.c
 * i każdą pozycję wypisuje na konsolę USB linią "S.UUUUUU K N[ !R!Z!P]: xx xx …";
 * co 10 s linia "# …" z licznikami.
 * Odbiór nigdy nie czeka na konsolę: gdy nikt jej nie czyta, linie po zapełnieniu bufora giną (i są liczone).
 *
 * Polecenia z komputera (znaki na konsoli USB), tylko do odbioru:
 *   p  przez 2 s mierzy szerokości stanów na RO: z tego widać czas bitu i poziom spoczynkowy;
 *   i  odwraca wejście RX, jak zamiana A z B.
 */

#include <stdint.h>

#include "console.h"
#include "devices.h"
#include "pins.h"
#include "rvuos.h"
#include "ropam/rs485.c"
#include "ropam/szyna.c"

/* Regiony procesu za kodem i danymi. */
#define REGION_IO_MUX  2
#define REGION_GPIO    3
#define REGION_CONSOLE 4
#define REGION_UART0   5
#define REGION_COUNTER 6

/* Rejestry UART0 (ESP-IDF, soc/esp32c6/register/soc/uart_reg.h). */
#define UART_FIFO    (UART0_BASE + 0x00u)
#define UART_INT_RAW (UART0_BASE + 0x04u)
#define UART_INT_CLR (UART0_BASE + 0x10u)
#define UART_RX_FILT (UART0_BASE + 0x18u)
#define UART_STATUS  (UART0_BASE + 0x1Cu)
#define UART_UPDATE  (UART0_BASE + 0x98u)
#define RXFIFO_CNT(s) ((s) & 0xFFu)
#define INT_FRM_ERR  (1u << 3)
#define INT_OVF      (1u << 4)
#define INT_GLITCH   (1u << 11)
#define INT_ERRORS   (INT_FRM_ERR | INT_OVF | INT_GLITCH)
#define GLITCH_FILT  (64u | (1u << 8)) /* impulsy krótsze niż 1,6 µs; bit trwa 8,7 µs */

#define STATS_S  10u
#define CISZA_US 3000000u /* ramki rozpoznaje CRC, więc cisza tylko wypuszcza resztki z bufora */
#define OUT_SIZE 8192u    /* potęga dwójki */

#define REG(a) (*(volatile uint32_t *)(a))

static void expect(const char *what, uint32_t status)
{
    if (status != KERR_OK) {
        rv_puts(BOOT_CAP_PRINT, "ropam-podsluch: ");
        rv_puts(BOOT_CAP_PRINT, what);
        rv_puts(BOOT_CAP_PRINT, ": FAILED\n");
        rv_halt(BOOT_CAP_HALT, 1);
    }
}

/* Bufor konsoli; do niego tylko całe linie, żeby przy zapełnieniu ginęły całe. */
static char out[OUT_SIZE];
static uint32_t out_head, out_tail, lost_lines;
static char stage[256]; /* linia składana z kawałków (emit_str, emit_num) */
static uint32_t stage_n;

static void emit(const char *s, uint32_t n)
{
    if (OUT_SIZE - (out_head - out_tail) < n) {
        lost_lines++;
        return;
    }
    for (uint32_t i = 0; i < n; i++) {
        out[out_head++ & (OUT_SIZE - 1u)] = s[i];
    }
}

static void emit_str(const char *s)
{
    for (; *s; s++) {
        if (stage_n < sizeof(stage)) {
            stage[stage_n++] = *s;
        }
        if (*s == '\n') {
            emit(stage, stage_n);
            stage_n = 0;
        }
    }
}

static void emit_num(const char *name, uint32_t v)
{
    char d[12];
    emit_str(name);
    d[szyna_dec(d, v, 1)] = '\0';
    emit_str(d);
}

/*
 * Do konsoli tyle, ile przyjmie jej FIFO, bez czekania.
 * Pakiet USB idzie, gdy FIFO jest pełne albo dane czekają w nim 2 ms:
 * drobne pakiety po każdej linii nie nadążały za magistralą.
 * Pełne FIFO z czekającymi danymi dostaje WR_DONE co 1 ms (jak console_put_polled):
 * pakiet wysłany, zanim komputer otworzył port, inaczej potrafi utknąć na dobre.
 */
static uint32_t pump_retry_at, pump_first_at, pump_unsent;

static void pump(uint32_t now_us)
{
    while (out_tail != out_head && (console_regs[USJ_EP1_CONF] & USJ_FREE) != 0) {
        console_regs[USJ_EP1] = (uint8_t)out[out_tail++ & (OUT_SIZE - 1u)];
        if (!pump_unsent) {
            pump_first_at = now_us;
            pump_unsent = 1;
        }
    }
    int full = (console_regs[USJ_EP1_CONF] & USJ_FREE) == 0;
    if (pump_unsent ? (full || now_us - pump_first_at >= 2000u)
                    : (full && out_tail != out_head && now_us - pump_retry_at >= 1000u)) {
        console_flush();
        pump_unsent = 0;
        pump_retry_at = now_us;
    }
}

/*
 * Czas z licznika, przyrostowo na 32 bitach (bez dzielenia 64-bitowego).
 * Licznik nie tyka w pełnych MHz (ok. 160,05 MHz), więc mikrosekundy są przybliżone.
 */
static volatile uint32_t *counter;
static uint32_t hz, per_us, last_tick, rest_ticks, sec, usec;

static void clock_tick(void)
{
    uint32_t t = *counter;
    rest_ticks += t - last_tick;
    last_tick = t;
    while (rest_ticks >= hz) {
        rest_ticks -= hz;
        sec++;
    }
    usec = rest_ticks / per_us;
    if (usec > 999999u) {
        usec = 999999u;
    }
}

static uint32_t rx_inverted;

static void set_inverted(uint32_t inv)
{
    rx_inverted = inv;
    rs485_set_inverted(inv);
    emit_num("# wejscie RX odwrocone: ", inv);
    emit_str("\n");
}

/*
 * Pomiar na RO przez 2 s, bez czytania UART-u: zbocza, udział stanu wysokiego
 * i najkrótszy impuls każdego stanu, czyli czas bitu (8,68 µs przy 115200 b/s).
 */
static void probe(void)
{
    uint32_t min_w[2] = { ~0u, ~0u }, ticks[2] = { 0, 0 }, edges = 0;
    uint32_t start = *counter, t = start, edge = start;
    uint32_t level = (REG(RS485_GPIO_IN) >> RS485_RX) & 1u;
    while (t - start < 2000000u * per_us) {
        uint32_t in = (REG(RS485_GPIO_IN) >> RS485_RX) & 1u;
        t = *counter;
        if (in != level) {
            uint32_t w = t - edge;
            ticks[level] += w;
            min_w[level] = w < min_w[level] ? w : min_w[level];
            edges++;
            level = in;
            edge = t;
        }
    }
    ticks[level] += t - edge;
    emit_num("# pomiar RO: zbocza ", edges);
    emit_num(", wysoki promile ", ticks[1] / ((ticks[0] + ticks[1]) / 1000u + 1u));
    emit_num(", najkrotszy niski ns ", min_w[0] == ~0u ? 0 : min_w[0] * 1000u / per_us);
    emit_num(", najkrotszy wysoki ns ", min_w[1] == ~0u ? 0 : min_w[1] * 1000u / per_us);
    emit_str("\n");
}

/* Liczniki od ostatniej linii "#". */
struct stats {
    uint32_t bytes, frames, acks, junk, err_frm, err_glitch, err_ovf;
};

/* Pozycja z parsera: do liczników i linią na konsolę. */
static void on_item(void *arg, char rodzaj, const uint8_t *p, uint32_t n, uint32_t s, uint32_t us, uint32_t bledy)
{
    static char line[SZYNA_LINIA_MAX];
    struct stats *st = arg;
    st->frames += rodzaj == SZYNA_RAMKA;
    st->acks += rodzaj == SZYNA_ACK;
    st->junk += rodzaj == SZYNA_INNE ? n : 0;
    emit(line, szyna_linia(line, sizeof(line), s, us, rodzaj, p, n, bledy));
}

static void show_stats(const struct stats *st)
{
    emit_num("# t=", sec);
    emit_num(" bajty ", st->bytes);
    emit_num(" ramki ", st->frames);
    emit_num(" ack ", st->acks);
    emit_num(" smieci_bajty ", st->junk);
    emit_num(" bledy_ramki ", st->err_frm);
    emit_num(" zaklocenia ", st->err_glitch);
    emit_num(" przepelnienia ", st->err_ovf);
    emit_num(" zgubione_linie ", lost_lines);
    emit_str("\n");
    if (st->bytes == 0) {
        emit_str("# cisza: sprawdz A, B, GND i zasilanie transceivera\n");
    } else if (st->frames == 0 && st->err_frm >= 20u) {
        /* Polaryzacja przy starcie bywa zła, np. gdy centrala była wtedy wyłączona. */
        emit_str("# same bledy ramki, zmieniam polaryzacje\n");
        set_inverted(!rx_inverted);
    }
}

int main(void);
int main(void)
{
    /* Najpierw transceiver w tryb tylko odbioru, potem cała reszta. */
    expect("map IO MUX", rv_process_install(BOOT_CAP_PROCESS, REGION_IO_MUX, BOOT_CAP_IO_MUX, RIGHT_R | RIGHT_W));
    expect("map GPIO", rv_process_install(BOOT_CAP_PROCESS, REGION_GPIO, BOOT_CAP_GPIO, RIGHT_R | RIGHT_W));
    rs485_receive_only();

    uint32_t base, size;
    expect("console info", rv_frame_info(BOOT_CAP_UART, &base, &size));
    expect("map the console", rv_process_install(BOOT_CAP_PROCESS, REGION_CONSOLE, BOOT_CAP_UART, RIGHT_R | RIGHT_W));
    console_init(base);
    expect("map UART0", rv_process_install(BOOT_CAP_PROCESS, REGION_UART0, BOOT_CAP_UART0, RIGHT_R | RIGHT_W));
    expect("counter info", rv_frame_info(BOOT_CAP_COUNTER, &base, &size));
    expect("map the counter", rv_process_install(BOOT_CAP_PROCESS, REGION_COUNTER, BOOT_CAP_COUNTER, RIGHT_R));
    counter = (volatile uint32_t *)(base + COUNTER_OFFSET);
    uint64_t now;
    expect("read the clock", rv_clock_read(BOOT_CAP_CLOCK, &now, &hz));
    per_us = hz / 1000000u;
    last_tick = *counter;

    REG(UART_RX_FILT) = GLITCH_FILT;
    REG(UART_UPDATE) = 1u;
    for (uint32_t i = 0; i < 1000u && (REG(UART_UPDATE) & 1u); i++) {
    }

    uint32_t in = REG(RS485_GPIO_IN), high = rs485_idle_high_permille();
    emit_num("# ropam-podsluch: tylko odbior, RE/DE (D8)=", (in >> RS485_REDE) & 1u);
    emit_num(", DI (D9)=", (in >> RS485_DI) & 1u);
    emit_num(", RO (D7) w spoczynku wysoki promile ", high);
    emit_str("\n");
    set_inverted(high < 500u);
    while (RXFIFO_CNT(REG(UART_STATUS)) != 0) {
        (void)REG(UART_FIFO);
    }
    REG(UART_INT_CLR) = INT_ERRORS;

    struct stats st = { 0 };
    static struct szyna_parser parser;
    szyna_init(&parser, on_item, &st);
    uint32_t last_byte = *counter, next_stats = STATS_S;

    for (;;) {
        clock_tick();
        uint32_t t = *counter;
        if (RXFIFO_CNT(REG(UART_STATUS)) != 0) {
            /* Błędy UART-u przypisujemy bajtom z tej samej porcji. */
            uint32_t raw = REG(UART_INT_RAW) & INT_ERRORS, errs = 0;
            if (raw) {
                REG(UART_INT_CLR) = raw;
                errs = ((raw & INT_FRM_ERR) ? SZYNA_E_RAMKA : 0) | ((raw & INT_GLITCH) ? SZYNA_E_ZAKL : 0) |
                       ((raw & INT_OVF) ? SZYNA_E_PRZEP : 0);
                st.err_frm += (raw & INT_FRM_ERR) != 0;
                st.err_glitch += (raw & INT_GLITCH) != 0;
                st.err_ovf += (raw & INT_OVF) != 0;
            }
            while (RXFIFO_CNT(REG(UART_STATUS)) != 0) {
                szyna_bajt(&parser, (uint8_t)REG(UART_FIFO), sec, usec, errs);
                st.bytes++;
            }
            last_byte = t;
        } else if (parser.n != 0 && t - last_byte >= CISZA_US * per_us) {
            szyna_cisza(&parser);
        }
        if (sec >= next_stats) {
            next_stats = sec + STATS_S;
            show_stats(&st);
            struct stats zero = { 0 };
            st = zero;
        }
        char c;
        if (console_get(&c) && (c == 'i' || c == 'p')) {
            if (c == 'i') {
                set_inverted(!rx_inverted);
            } else {
                probe();
                while (RXFIFO_CNT(REG(UART_STATUS)) != 0) {
                    (void)REG(UART_FIFO);
                }
                szyna_cisza(&parser);
                clock_tick();
                last_byte = *counter;
            }
        }
        pump(sec * 1000000u + usec);
    }
}
