#ifndef ROPAM_STRONA_H
#define ROPAM_STRONA_H

/*
 * Strona WWW i JSON ze stanu centrali, dla serwera HTTP na XIAO:
 * nagłówek stanu z rozgłoszenia 0x10 na magistrali (te same 220 B co na początku odpowiedzi na 0x24)
 * i raport panelu TPR (0x11). Pola: docs/stan-centrali.md, docs/protokol-ramek.md.
 * Tylko podgląd, bez żadnych poleceń.
 * Z profilem instalacji (niżej) z nazwami wejść, wyjść i stref.
 */

#include <stdint.h>

struct strona_stan {
    const uint8_t *status;  /* 220 B nagłówka stanu albo 0, gdy jeszcze go nie ma */
    const uint8_t *panel;   /* 10 B raportu panelu albo 0 */
    uint32_t age_ms;        /* wiek stanu */
    uint32_t bytes, frames, states, junk; /* bajty, ramki, rozgłoszenia stanu, bajty śmieci od startu */
    const struct strona_profil *profil;   /* nazwy z konfiguracji albo 0: same numery */
};

/*
 * Profil instalacji z konfiguracji centrali (tools/profil_bramki.py):
 * nazwy w UTF-8, 0 dla nieużywanych.
 * Strona pokazuje wtedy nazwy i wyróżnia stany, które coś znaczą.
 */
struct strona_profil {
    const char *inputs[48];
    uint8_t input_type[48];  /* typ wejścia (docs/konfiguracja.md) */
    uint8_t input_zones[48]; /* bity 0–3: strefy 1–4 */
    const char *outputs[32];
    const char *zones[4];
    uint64_t inputs_known, inputs_expected;   /* stany z pliku oczekiwań: które i jakie (1 = naruszone) */
    uint32_t outputs_known, outputs_expected; /* 1 = włączone */
};

/* Profil wkompilowany z profil.h obok strona.c (generuje go zbuduj.sh) albo 0. */
const struct strona_profil *strona_profil_instalacji(void);

/* Odpowiedź HTTP (nagłówki i treść) na zapytanie req; długość w out, najwyżej max bajtów. */
uint32_t strona_http(const struct strona_stan *s, const uint8_t *req, uint32_t len, uint8_t *out, uint32_t max);

#endif
