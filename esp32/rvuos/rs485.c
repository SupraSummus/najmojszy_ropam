/*
 * Transceiver RS-485 na XIAO ESP32-C6, tylko do odbioru: RO na D7 (U0RXD), RE/DE na D8, DI na D9.
 * Wspólne dla ropam-podsluch.c i szyna-klient.c; wymaga zamapowanych IO MUX i GPIO.
 */

#define RS485_RX   17u /* D7 */
#define RS485_REDE 19u /* D8 */
#define RS485_DI   20u /* D9 */
#define RS485_GPIO_IN    (GPIO_BASE + 0x3Cu)
#define RS485_SIG_U0RXD  6u        /* sygnał wejściowy U0RXD w matrycy GPIO */
#define RS485_IN_INV     (1u << 6) /* negacja sygnału w GPIO_IN_SEL */
#define RS485_REG(a) (*(volatile uint32_t *)(a))

/* RE/DE nisko (odbiór, nadajnik wyłączony), DI wysoko; poziom przed zezwoleniem na wyjście. */
static void rs485_receive_only(void)
{
    RS485_REG(GPIO_OUT_W1TC) = 1u << RS485_REDE;
    RS485_REG(GPIO_OUT_W1TS) = 1u << RS485_DI;
    RS485_REG(GPIO_OUT_SEL(RS485_REDE)) = SIG_GPIO_OUT | GPIO_OE_FROM_ENABLE;
    RS485_REG(GPIO_OUT_SEL(RS485_DI)) = SIG_GPIO_OUT | GPIO_OE_FROM_ENABLE;
    RS485_REG(GPIO_ENABLE_W1TS) = (1u << RS485_REDE) | (1u << RS485_DI);
    RS485_REG(IO_MUX_PIN(RS485_REDE)) = IO_MUX_MATRIX(0) | IO_MUX_INPUT;
    RS485_REG(IO_MUX_PIN(RS485_DI)) = IO_MUX_MATRIX(0) | IO_MUX_INPUT;
}

/* U0RXD z RO przez matrycę GPIO, z negacją albo bez. */
static void rs485_set_inverted(uint32_t inv)
{
    RS485_REG(GPIO_IN_SEL(RS485_SIG_U0RXD)) = RS485_RX | GPIO_IN_MATRIX | (inv ? RS485_IN_INV : 0);
}

/*
 * Magistrala w spoczynku to stan wysoki na RO; głównie niski znaczy zamienione A i B,
 * co przy modułach z transceiverem jest częste.
 * Ok. 0,2 s próbek, bez zegara. Zwraca promile czasu w stanie wysokim.
 */
static uint32_t rs485_idle_high_permille(void)
{
    uint32_t high = 0, all = 1u << 21;
    for (uint32_t i = 0; i < all; i++) {
        high += (RS485_REG(RS485_GPIO_IN) >> RS485_RX) & 1u;
    }
    return high / (all / 1000u);
}
