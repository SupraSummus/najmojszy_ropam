/*
 * Test strona.c na komputerze, ze zmyślonym stanem.
 *
 * cc -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined -I esp32/rvuos \
 *    esp32/rvuos/test/strona-test.c esp32/rvuos/strona.c
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "strona.h"

int main(void)
{
    static uint8_t st[220], pn[10], out[8192];
    st[0] = 0x05;               /* naruszone I1 i I3 */
    st[0x30] = 0x02;            /* włączone O2 */
    for (int z = 0; z < 4; z++) {
        st[0x4C + 8 * z] = 0x03; /* rozbrojona */
    }
    st[0x4C + 1] = 0x04;        /* S1: bit 10, czuwanie */
    const uint8_t clock[6] = { 26, 1, 15, 12, 5, 9 };
    memcpy(st + 0xD0, clock, sizeof(clock));
    pn[4] = 0xb1;               /* 12,01 V */
    pn[5] = 0x04;
    pn[9] = 1;                  /* fn */
    struct strona_stan s = { st, pn, 2500, 900, 70, 7, 0 };
    const char *req = "GET / HTTP/1.1\r\nHost: x\r\n\r\n";
    uint32_t n = strona_http(&s, (const uint8_t *)req, (uint32_t)strlen(req), out, sizeof(out));
    out[n] = 0;
    assert(n > 0 && strstr((char *)out, "HTTP/1.0 200 OK\r\n") == (char *)out);
    assert(strstr((char *)out, "I1, I3") && strstr((char *)out, "<td>O2</td>"));
    assert(strstr((char *)out, "czuwanie") && strstr((char *)out, "stan sprzed 2 s"));
    assert(strstr((char *)out, "2026-01-15 12:05:09") && strstr((char *)out, "12.01 V"));
    assert(strstr((char *)out, "aktywny") && !strstr((char *)out, "sesji serwisowej"));
    const char *body = strstr((char *)out, "\r\n\r\n") + 4;
    char want[32];
    snprintf(want, sizeof(want), "Content-Length: %u\r\n", (unsigned)strlen(body));
    assert(strstr((char *)out, want));

    st[0xCE] = 0x02;            /* sesja serwisowa */
    req = "GET /stan.json HTTP/1.1\r\n\r\n";
    n = strona_http(&s, (const uint8_t *)req, (uint32_t)strlen(req), out, sizeof(out));
    out[n] = 0;
    body = strstr((char *)out, "\r\n\r\n") + 4;
    assert(strstr(body, "\"sesja_serwisowa\":true") && strstr(body, "\"fn\":true"));
    assert(strstr(body, "\"zasilanie_V\":12.01") && strstr(body, "\"naruszone\":[1,3]"));

    struct strona_stan empty = { 0, 0, 0, 0, 0, 0, 0 };
    n = strona_http(&empty, (const uint8_t *)req, (uint32_t)strlen(req), out, sizeof(out));
    out[n] = 0;
    assert(strstr((char *)out, "{\"bajty\":0,\"ramki\":0,"));

    req = "GET /x HTTP/1.1\r\n\r\n";
    strona_http(&s, (const uint8_t *)req, (uint32_t)strlen(req), out, sizeof(out));
    assert(memcmp(out, "HTTP/1.0 404", 12) == 0);
    n = strona_http(&s, (const uint8_t *)"GET / ", 6, out, 300);
    assert(n > 0 && memcmp(out, "HTTP/1.0 500", 12) == 0); /* za mały bufor: 500, nie ucięta strona */
    printf("strona: OK\n");
    return 0;
}
