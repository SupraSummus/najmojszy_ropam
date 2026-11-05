/*
 * Strona WWW i JSON ze stanu centrali; zob. strona.h.
 */

#include "strona.h"

struct sbuf {
    uint8_t *p;
    uint32_t len, max;
};

static void sput(struct sbuf *o, const char *s)
{
    for (; *s; s++) {
        if (o->len < o->max) {
            o->p[o->len] = (uint8_t)*s;
        }
        o->len++;
    }
}

static void snum(struct sbuf *o, uint32_t v)
{
    char s[11];
    int i = 10;
    s[i] = '\0';
    do {
        s[--i] = (char)('0' + v % 10u);
        v /= 10u;
    } while (v != 0);
    sput(o, &s[i]);
}

static uint64_t le64(const uint8_t *p)
{
    uint64_t v = 0;
    for (int i = 7; i >= 0; i--) {
        v = v << 8 | p[i];
    }
    return v;
}

static uint32_t le32(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

static const char *zone_state(uint32_t w)
{
    if ((w >> 11) & 1u) {
        return "czuwanie nocne";
    }
    if ((w >> 10) & 1u) {
        return "czuwanie";
    }
    switch (w & 0xFu) {
    case 3:
        return "rozbrojona";
    case 5:
        return "odliczanie czasu na wyjście";
    default:
        return "inny stan";
    }
}

/* Numery ustawionych bitów jako lista "I1, I5" albo "brak". */
static void list(struct sbuf *o, const char *prefix, uint64_t mask, uint32_t count)
{
    int any = 0;
    for (uint32_t i = 0; i < count; i++) {
        if ((mask >> i) & 1u) {
            sput(o, any ? ", " : "");
            sput(o, prefix);
            snum(o, i + 1);
            any = 1;
        }
    }
    if (!any) {
        sput(o, "brak");
    }
}

static void json_list(struct sbuf *o, uint64_t mask, uint32_t count)
{
    int any = 0;
    sput(o, "[");
    for (uint32_t i = 0; i < count; i++) {
        if ((mask >> i) & 1u) {
            sput(o, any ? "," : "");
            snum(o, i + 1);
            any = 1;
        }
    }
    sput(o, "]");
}

static uint32_t le16(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8;
}

/* Dwie cyfry z zerem z przodu. */
static void snum2(struct sbuf *o, uint32_t v)
{
    if (v < 10u) {
        sput(o, "0");
    }
    snum(o, v);
}

/* Zegar centrali z nagłówka stanu (0x0D0): rok − 2000, miesiąc, dzień, godzina, minuta, sekunda. */
static void clock_text(struct sbuf *o, const uint8_t *st)
{
    const uint8_t *c = st + 0xD0;
    sput(o, "20");
    snum2(o, c[0]);
    sput(o, "-");
    snum2(o, c[1]);
    sput(o, "-");
    snum2(o, c[2]);
    sput(o, " ");
    snum2(o, c[3]);
    sput(o, ":");
    snum2(o, c[4]);
    sput(o, ":");
    snum2(o, c[5]);
}

static int service_session(const uint8_t *st)
{
    return (le16(st + 0xCE) >> 1) & 1u;
}

/* Napięcie w 0,01 V jako „12.01”. */
static void volts(struct sbuf *o, uint32_t centi)
{
    snum(o, centi / 100u);
    sput(o, ".");
    snum2(o, centi % 100u);
}

static void html(struct sbuf *o, const struct strona_stan *s)
{
    sput(o, "<!doctype html><html lang=\"pl\"><head><meta charset=\"utf-8\">"
           "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
           "<meta http-equiv=\"refresh\" content=\"5\"><title>Centrala</title><style>"
           ":root{--bg:#fff;--fg:#222;--dim:#777;--line:#ddd;--on:#e6f2e6;--alarm:#f8e0e0}"
           "@media (prefers-color-scheme:dark){:root{--bg:#181818;--fg:#ddd;--dim:#999;--line:#333;"
           "--on:#1f3320;--alarm:#42201f}}"
           "body{font:15px/1.4 system-ui,sans-serif;background:var(--bg);color:var(--fg);"
           "margin:0 auto;padding:1em;max-width:40em}"
           "table{border-collapse:collapse;width:100%}td{padding:.35em .5em;border-bottom:1px solid var(--line)}"
           ".on{background:var(--on)}.alarm{background:var(--alarm)}.dim{color:var(--dim)}"
           "</style></head><body><h1>Stan centrali</h1>");
    if (s->status == 0) {
        sput(o, "<p>Jeszcze nie ma stanu z magistrali.</p>");
    } else {
        const uint8_t *st = s->status;
        sput(o, "<p class=\"dim\">Zegar centrali ");
        clock_text(o, st);
        sput(o, ", stan sprzed ");
        snum(o, s->age_ms / 1000u);
        sput(o, " s; odświeżanie co 5 s; tylko podgląd.</p>");
        if (service_session(st)) {
            sput(o, "<p class=\"alarm\">Centrala jest w sesji serwisowej (tryb ON-LINE).</p>");
        }
        sput(o, "<h2>Strefy</h2><table>");
        for (uint32_t z = 0; z < 4; z++) {
            uint32_t w = le32(st + 0x4C + 8 * z);
            sput(o, ((w >> 10) & 3u) ? "<tr class=\"on\"><td>S" : "<tr><td>S");
            snum(o, z + 1);
            sput(o, "</td><td>");
            sput(o, zone_state(w));
            sput(o, "</td></tr>");
        }
        uint64_t violated = le64(st), tamper = le64(st + 8);
        sput(o, "</table><h2>Wejścia</h2><table><tr");
        sput(o, violated ? " class=\"alarm\"" : "");
        sput(o, "><td>naruszone</td><td>");
        list(o, "I", violated, 48);
        sput(o, "</td></tr><tr");
        sput(o, tamper ? " class=\"alarm\"" : "");
        sput(o, "><td>stan 3 (sabotaż?)</td><td>");
        list(o, "I", tamper, 48);
        sput(o, "</td></tr></table><h2>Wyjścia</h2><table><tr><td>włączone</td><td>");
        list(o, "O", le32(st + 0x30), 32);
        sput(o, "</td></tr></table>");
    }
    if (s->panel != 0) {
        const uint8_t *pn = s->panel;
        sput(o, "<h2>Panel</h2><table><tr><td>zasilanie</td><td>");
        volts(o, le16(pn + 4));
        sput(o, " V</td></tr><tr");
        sput(o, (pn[9] & 1u) ? " class=\"on\"" : "");
        sput(o, "><td>klawisz funkcyjny (fn)</td><td>");
        sput(o, (pn[9] & 1u) ? "aktywny" : "nie");
        sput(o, "</td></tr></table>");
    }
    sput(o, "<p class=\"dim\">Magistrala: bajtów ");
    snum(o, s->bytes);
    sput(o, ", ramek ");
    snum(o, s->frames);
    sput(o, ", rozgłoszeń stanu ");
    snum(o, s->states);
    sput(o, ", bajtów śmieci ");
    snum(o, s->junk);
    sput(o, ".</p></body></html>");
}

static void json(struct sbuf *o, const struct strona_stan *s)
{
    sput(o, "{\"bajty\":");
    snum(o, s->bytes);
    sput(o, ",\"ramki\":");
    snum(o, s->frames);
    sput(o, ",\"stany\":");
    snum(o, s->states);
    sput(o, ",\"smieci\":");
    snum(o, s->junk);
    sput(o, ",\"wiek_ms\":");
    snum(o, s->age_ms);
    if (s->status != 0) {
        const uint8_t *st = s->status;
        sput(o, ",\"zegar\":\"");
        clock_text(o, st);
        sput(o, "\",\"sesja_serwisowa\":");
        sput(o, service_session(st) ? "true" : "false");
        sput(o, ",\"strefy\":[");
        for (uint32_t z = 0; z < 4; z++) {
            sput(o, z ? ",\"" : "\"");
            sput(o, zone_state(le32(st + 0x4C + 8 * z)));
            sput(o, "\"");
        }
        sput(o, "],\"naruszone\":");
        json_list(o, le64(st), 48);
        sput(o, ",\"sabotaz\":");
        json_list(o, le64(st + 8), 48);
        sput(o, ",\"wyjscia\":");
        json_list(o, le32(st + 0x30), 32);
    }
    if (s->panel != 0) {
        sput(o, ",\"panel\":{\"zasilanie_V\":");
        volts(o, le16(s->panel + 4));
        sput(o, ",\"fn\":");
        sput(o, (s->panel[9] & 1u) ? "true" : "false");
        sput(o, "}");
    }
    sput(o, "}");
}

static int starts(const uint8_t *req, uint32_t len, const char *p)
{
    uint32_t i = 0;
    for (; p[i]; i++) {
        if (i >= len || req[i] != (uint8_t)p[i]) {
            return 0;
        }
    }
    return 1;
}

uint32_t strona_http(const struct strona_stan *s, const uint8_t *req, uint32_t len, uint8_t *out, uint32_t max)
{
    /* Najpierw treść za miejscem na nagłówki, żeby znać jej długość. */
    enum { HEAD = 160 };
    struct sbuf body = { out + HEAD, 0, max > HEAD ? max - HEAD : 0 };
    const char *type, *status = "200 OK";
    if (starts(req, len, "GET / ") || starts(req, len, "GET /index")) {
        html(&body, s);
        type = "text/html; charset=utf-8";
    } else if (starts(req, len, "GET /stan.json")) {
        json(&body, s);
        type = "application/json; charset=utf-8";
    } else {
        sput(&body, "nie ma\n");
        type = "text/plain; charset=utf-8";
        status = starts(req, len, "GET ") ? "404 Not Found" : "405 Method Not Allowed";
    }
    if (body.len > body.max) {
        body.len = 0; /* za mały bufor: lepiej 500 niż ucięta strona */
        sput(&body, "za mały bufor odpowiedzi\n");
        type = "text/plain; charset=utf-8";
        status = "500 Internal Server Error";
    }
    uint8_t head[HEAD];
    struct sbuf h = { head, 0, HEAD };
    sput(&h, "HTTP/1.0 ");
    sput(&h, status);
    sput(&h, "\r\nContent-Type: ");
    sput(&h, type);
    sput(&h, "\r\nContent-Length: ");
    snum(&h, body.len);
    sput(&h, "\r\nCache-Control: no-store\r\nConnection: close\r\n\r\n");
    /* Nagłówki tuż przed treścią. */
    uint32_t total = h.len + body.len;
    if (total > max) {
        return 0;
    }
    for (uint32_t i = 0; i < body.len; i++) {
        out[h.len + i] = out[HEAD + i];
    }
    for (uint32_t i = 0; i < h.len; i++) {
        out[i] = head[i];
    }
    return total;
}
