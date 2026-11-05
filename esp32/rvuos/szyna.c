/*
 * Ramki magistrali RopamNET dla podsłuchu; zob. szyna.h.
 */

#include "szyna.h"

#define NO_DATA_1 0xAAu /* długości oznaczające ramkę bez danych */
#define NO_DATA_2 0xFFu

/* Jak w firmware'ze: bit 0 sprawdzany po przesunięciu, nie przed (to nie CRC-16/MODBUS). */
uint16_t szyna_crc(const uint8_t *p, uint32_t n)
{
    uint32_t crc = 0xFFFFu;
    while (n--) {
        crc ^= *p++;
        for (int i = 0; i < 8; i++) {
            crc >>= 1;
            if (crc & 1u) {
                crc ^= 0xA001u;
            }
        }
    }
    return (uint16_t)crc;
}

int szyna_ramka(const uint8_t *p, uint32_t n)
{
    if (n < 5) {
        return 0;
    }
    uint32_t len = p[2] == NO_DATA_1 || p[2] == NO_DATA_2 ? 0 : p[2];
    return n == 3u + len + 2u && szyna_crc(p, n - 2) == (uint16_t)(p[n - 2] << 8 | p[n - 1]);
}

void szyna_init(struct szyna_parser *p, szyna_wyjscie out, void *arg)
{
    p->n = 0;
    p->smieci = 0;
    p->out = out;
    p->arg = arg;
}

/* Wysyła k pierwszych bajtów bufora jako pozycję rodzaju r i usuwa je z bufora. */
static void wydaj(struct szyna_parser *p, char r, uint32_t k)
{
    uint32_t e = 0;
    for (uint32_t i = 0; i < k; i++) {
        e |= p->e[i];
    }
    p->out(p->arg, r, p->b, k, p->s[0], p->us[0], e);
    for (uint32_t i = k; i < p->n; i++) {
        p->b[i - k] = p->b[i];
        p->e[i - k] = p->e[i];
        p->s[i - k] = p->s[i];
        p->us[i - k] = p->us[i];
    }
    p->n -= k;
}

static void wydaj_smieci(struct szyna_parser *p)
{
    if (p->smieci != 0) {
        uint32_t k = p->smieci;
        p->smieci = 0;
        wydaj(p, SZYNA_INNE, k);
    }
}

/* Rozpoznaje, co się da; ostatecznie: bez czekania na dalsze bajty. */
static void krok(struct szyna_parser *p, int ostatecznie)
{
    for (;;) {
        uint32_t i = p->smieci, jest = p->n - i;
        if (jest == 0) {
            if (ostatecznie) {
                wydaj_smieci(p);
            }
            return;
        }
        if (jest >= 3) {
            uint32_t len = p->b[i + 2] == NO_DATA_1 || p->b[i + 2] == NO_DATA_2 ? 0 : p->b[i + 2];
            uint32_t trzeba = 3u + len + 2u;
            if (jest >= trzeba) {
                if (szyna_ramka(&p->b[i], trzeba)) {
                    wydaj_smieci(p);
                    wydaj(p, SZYNA_RAMKA, trzeba);
                    continue;
                }
            } else if (!ostatecznie) {
                return;
            }
        } else if (!ostatecznie) {
            return;
        }
        /* Tu nie zaczyna się ramka. */
        if (i == 0 && p->b[0] >= 0x01u && p->b[0] <= 0x03u) {
            wydaj(p, SZYNA_ACK, 1);
            continue;
        }
        p->smieci++;
        if (p->smieci >= SZYNA_SMIECI_MAX) {
            wydaj_smieci(p);
        }
    }
}

void szyna_bajt(struct szyna_parser *p, uint8_t b, uint32_t s, uint32_t us, uint32_t bledy)
{
    if (p->n == SZYNA_BUF) {
        /* Nie powinno się zdarzyć: śmieci i ramka za nimi mieszczą się w buforze. */
        krok(p, 1);
    }
    p->b[p->n] = b;
    p->e[p->n] = (uint8_t)bledy;
    p->s[p->n] = s;
    p->us[p->n] = us;
    p->n++;
    krok(p, 0);
}

void szyna_cisza(struct szyna_parser *p)
{
    krok(p, 1);
}

uint32_t szyna_dec(char *out, uint32_t v, uint32_t min)
{
    char tmp[10];
    uint32_t k = 0;
    do {
        tmp[k++] = (char)('0' + v % 10u);
        v /= 10u;
    } while (v != 0);
    while (k < min && k < sizeof(tmp)) {
        tmp[k++] = '0';
    }
    for (uint32_t i = 0; i < k; i++) {
        out[i] = tmp[k - 1 - i];
    }
    return k;
}

uint32_t szyna_linia(char *out, uint32_t cap, uint32_t s, uint32_t us, char rodzaj,
                     const uint8_t *p, uint32_t n, uint32_t bledy)
{
    static const char hex[] = "0123456789abcdef";
    if (cap < 48u + 3u * n) {
        return 0;
    }
    uint32_t k = szyna_dec(out, s, 1);
    out[k++] = '.';
    k += szyna_dec(out + k, us, 6);
    out[k++] = ' ';
    out[k++] = rodzaj;
    out[k++] = ' ';
    k += szyna_dec(out + k, n, 1);
    static const struct {
        uint32_t bit;
        char c;
    } flagi[] = { { SZYNA_E_RAMKA, 'R' }, { SZYNA_E_ZAKL, 'Z' }, { SZYNA_E_PRZEP, 'P' } };
    for (uint32_t i = 0; i < sizeof(flagi) / sizeof(flagi[0]); i++) {
        if (bledy & flagi[i].bit) {
            out[k++] = ' ';
            out[k++] = '!';
            out[k++] = flagi[i].c;
        }
    }
    out[k++] = ':';
    for (uint32_t i = 0; i < n; i++) {
        out[k++] = ' ';
        out[k++] = hex[p[i] >> 4];
        out[k++] = hex[p[i] & 15u];
    }
    out[k++] = '\n';
    return k;
}
