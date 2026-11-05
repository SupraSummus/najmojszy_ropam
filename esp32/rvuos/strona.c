/*
 * Strona WWW i JSON ze stanu centrali; zob. strona.h.
 */

#include "strona.h"

#if __has_include("profil.h")
#include "profil.h" /* zbuduj.sh: z PROFIL albo pusty */
#endif
#ifndef PROFIL
#define PROFIL 0
#endif

const struct strona_profil *strona_profil_instalacji(void)
{
    return PROFIL;
}

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

/* Tekst z profilu w HTML-u (json = 0) albo w napisie JSON. */
static void stext(struct sbuf *o, const char *s, int json)
{
    for (; *s; s++) {
        const char c[2] = { *s, '\0' };
        if (*s == '&' && !json) {
            sput(o, "&amp;");
        } else if (*s == '<' && !json) {
            sput(o, "&lt;");
        } else if (*s == '"') {
            sput(o, json ? "\\\"" : "&quot;");
        } else if (*s == '\\' && json) {
            sput(o, "\\\\");
        } else {
            sput(o, (uint8_t)*s < 0x20u ? " " : c);
        }
    }
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

/* Typy wejść, docs/konfiguracja.md. */
static const char *input_type(uint32_t t)
{
    static const char *const names[] = { "zwykła cicha", "24h", "zwykła", "info", "zał./wył. czuwanie",
                                         "opóźniona", "opóźniona war.", "licznikowa", "kasująca alarm",
                                         "zał. czuwanie", "wył. czuwanie" };
    return t < sizeof(names) / sizeof(names[0]) ? names[t] : "inny typ";
}

static uint32_t bit(uint64_t mask, uint32_t i)
{
    return (uint32_t)(mask >> i) & 1u;
}

/* Bity 10 i 11 słowa strefy: czuwanie i noc. */
static uint32_t zone_arming(const uint8_t *st, uint32_t z)
{
    return (le32(st + 0x4C + 8 * z) >> 10) & 3u;
}

/* Typy, które w czuwającej strefie wywołują alarm: cicha, zwykła, opóźnione, licznikowa. */
#define ZONE_ALARM_TYPES (1u << 0 | 1u << 2 | 1u << 5 | 1u << 6 | 1u << 7)

static int input_unexpected(const struct strona_profil *p, const uint8_t *st, uint32_t i)
{
    return bit(p->inputs_known, i) && bit(le64(st), i) != bit(p->inputs_expected, i);
}

static int output_unexpected(const struct strona_profil *p, const uint8_t *st, uint32_t i)
{
    return bit(p->outputs_known, i) && bit(le32(st + 0x30), i) != bit(p->outputs_expected, i);
}

/*
 * Czy wejście i (od 0) wymaga uwagi:
 * stan 3 albo stan inny niż w pliku oczekiwań;
 * bez oczekiwań także naruszone wejście 24h
 * i naruszone wejście alarmowe strefy w pełnym czuwaniu.
 * W czuwaniu nocnym nie wiemy, które wejścia czuwają ❓, więc go pomijamy.
 */
static int input_alert(const struct strona_profil *p, const uint8_t *st, uint32_t i)
{
    uint32_t type = p->input_type[i];
    if (bit(le64(st + 8), i) || input_unexpected(p, st, i)) {
        return 1;
    }
    if (bit(p->inputs_known, i) || !bit(le64(st), i)) {
        return 0;
    }
    if (type == 1u) {
        return 1;
    }
    for (uint32_t z = 0; z < 4; z++) {
        if (bit(p->input_zones[i], z) && zone_arming(st, z) == 1u && type < 32u && bit(ZONE_ALARM_TYPES, type)) {
            return 1;
        }
    }
    return 0;
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

static void row(struct sbuf *o, int alert, int on)
{
    sput(o, alert ? "<tr class=\"alarm\"><td>" : on ? "<tr class=\"on\"><td>" : "<tr><td>");
}

/* Wejścia i wyjścia bez profilu: same numery. */
static void plain_html(struct sbuf *o, const uint8_t *st)
{
    uint64_t violated = le64(st), tamper = le64(st + 8);
    sput(o, "<h2>Wejścia</h2><table><tr");
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

/*
 * Wejścia i wyjścia z nazwami z profilu.
 * Nienazwane wejścia pomijamy,
 * bo ich stały stan wynika z samej polaryzacji;
 * surowe maski są w JSON-ie.
 * Nienazwane włączone wyjścia pokazujemy zbiorczo.
 */
static void named_html(struct sbuf *o, const struct strona_profil *p, const uint8_t *st)
{
    uint64_t violated = le64(st), tamper = le64(st + 8);
    sput(o, "<h2>Wejścia</h2><table>");
    for (uint32_t i = 0; i < 48; i++) {
        if (p->inputs[i] == 0) {
            continue;
        }
        uint32_t v = bit(violated, i);
        row(o, input_alert(p, st, i), (int)v);
        stext(o, p->inputs[i], 0);
        sput(o, " <small>");
        sput(o, input_type(p->input_type[i]));
        for (uint32_t z = 0; z < 4; z++) {
            if ((p->input_zones[i] >> z) & 1u) {
                sput(o, ", S");
                snum(o, z + 1);
            }
        }
        sput(o, "</small></td><td>");
        sput(o, v ? "naruszone" : "w normie");
        sput(o, bit(tamper, i) ? ", stan 3" : "");
        if (input_unexpected(p, st, i)) {
            sput(o, v ? " (oczekiwane w normie)" : " (oczekiwane naruszone)");
        }
        sput(o, "</td></tr>");
    }
    uint32_t outputs = le32(st + 0x30), named_out = 0;
    sput(o, "</table><h2>Wyjścia</h2><table>");
    for (uint32_t i = 0; i < 32; i++) {
        if (p->outputs[i] == 0) {
            continue;
        }
        named_out |= 1u << i;
        uint32_t on = bit(outputs, i);
        int unexpected = output_unexpected(p, st, i);
        row(o, unexpected, (int)on);
        stext(o, p->outputs[i], 0);
        sput(o, "</td><td>");
        sput(o, on ? "włączone" : "wyłączone");
        if (unexpected) {
            sput(o, on ? " (oczekiwane wyłączone)" : " (oczekiwane włączone)");
        }
        sput(o, "</td></tr>");
    }
    if (outputs & ~named_out) {
        sput(o, "<tr><td>inne włączone</td><td>");
        list(o, "O", outputs & ~named_out, 32);
        sput(o, "</td></tr>");
    }
    sput(o, "</table>");
}

static void html(struct sbuf *o, const struct strona_stan *s, const struct strona_profil *p)
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
           ".on{background:var(--on)}.alarm{background:var(--alarm)}.dim,small{color:var(--dim)}"
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
            const char *name = p != 0 ? p->zones[z] : 0;
            if (p != 0 && name == 0 && (w & 0xFu) == 3u && zone_arming(st, z) == 0) {
                continue; /* bez wejść z profilu i rozbrojona */
            }
            row(o, 0, (int)zone_arming(st, z));
            sput(o, "S");
            snum(o, z + 1);
            if (name != 0) {
                sput(o, " <small>");
                stext(o, name, 0);
                sput(o, "</small>");
            }
            sput(o, "</td><td>");
            sput(o, zone_state(w));
            sput(o, "</td></tr>");
        }
        sput(o, "</table>");
        if (p != 0) {
            named_html(o, p, st);
        } else {
            plain_html(o, st);
        }
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

/* Napis JSON z tekstu profilu. */
static void jtext(struct sbuf *o, const char *s)
{
    sput(o, "\"");
    stext(o, s, 1);
    sput(o, "\"");
}

/* "opis": nazwy z profilu ze stanem; "uwaga" jak wyróżnienie na stronie. */
static void named_json(struct sbuf *o, const struct strona_profil *p, const uint8_t *st)
{
    sput(o, ",\"opis\":{\"strefy\":[");
    for (uint32_t z = 0; z < 4; z++) {
        sput(o, z ? "," : "");
        if (p->zones[z] == 0) {
            sput(o, "null");
        } else {
            jtext(o, p->zones[z]);
        }
    }
    sput(o, "],\"wejscia\":[");
    uint64_t violated = le64(st);
    const char *sep = "";
    for (uint32_t i = 0; i < 48; i++) {
        if (p->inputs[i] == 0) {
            continue;
        }
        uint32_t v = bit(violated, i);
        sput(o, sep);
        sep = ",";
        sput(o, "{\"nr\":");
        snum(o, i + 1);
        sput(o, ",\"nazwa\":");
        jtext(o, p->inputs[i]);
        sput(o, ",\"typ\":");
        jtext(o, input_type(p->input_type[i]));
        sput(o, v ? ",\"naruszone\":true" : ",\"naruszone\":false");
        sput(o, input_alert(p, st, i) ? ",\"uwaga\":true}" : ",\"uwaga\":false}");
    }
    sput(o, "],\"wyjscia\":[");
    uint32_t outputs = le32(st + 0x30);
    sep = "";
    for (uint32_t i = 0; i < 32; i++) {
        if (p->outputs[i] == 0) {
            continue;
        }
        uint32_t on = bit(outputs, i);
        sput(o, sep);
        sep = ",";
        sput(o, "{\"nr\":");
        snum(o, i + 1);
        sput(o, ",\"nazwa\":");
        jtext(o, p->outputs[i]);
        sput(o, on ? ",\"wlaczone\":true" : ",\"wlaczone\":false");
        sput(o, output_unexpected(p, st, i) ? ",\"uwaga\":true}" : ",\"uwaga\":false}");
    }
    sput(o, "]}");
}

static void json(struct sbuf *o, const struct strona_stan *s, const struct strona_profil *p)
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
        if (p != 0) {
            named_json(o, p, st);
        }
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
        html(&body, s, s->profil);
        type = "text/html; charset=utf-8";
    } else if (starts(req, len, "GET /stan.json")) {
        json(&body, s, s->profil);
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
