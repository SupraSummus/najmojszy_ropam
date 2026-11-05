#ifndef ROPAM_SZYNA_H
#define ROPAM_SZYNA_H

/*
 * Ramki magistrali RopamNET dla podsłuchu (ropam-podsluch.c);
 * format i CRC jak w docs/protokol-ramek.md i tools/ropamnet.py.
 *
 * Ramki idą jedna za drugą razem z potwierdzeniami, często bez przerwy,
 * a w rozgłoszeniu 0x10 bywa przerwa do ok. 12 ms 🔌, więc ciszą ich się nie oddzieli.
 * Parser robi to co parse_stream w ropamnet.py: od każdej pozycji próbuje ramki
 * (długość z trzeciego bajtu, potem CRC), a gdy jej nie ma, bierze bajt za potwierdzenie albo śmieć.
 * Na resztę ramki czeka, póki nie zapadnie cisza (szyna_cisza).
 */

#include <stdint.h>

#define SZYNA_RAMKA_MAX  260u /* adres, komenda, długość, 255 B danych, CRC */
#define SZYNA_SMIECI_MAX 64u  /* śmieci wychodzą kawałkami najwyżej tej długości */
#define SZYNA_BUF        512u /* mieści śmieci i najdłuższą ramkę za nimi */
/* Linia na pozycję: nagłówek i po 3 znaki na bajt. */
#define SZYNA_LINIA_MAX (48u + 3u * SZYNA_RAMKA_MAX)

/* Co UART zgłosił przy bajtach pozycji. */
#define SZYNA_E_RAMKA (1u << 0) /* błąd ramki: zły bit stopu (zła prędkość albo zamienione A i B) */
#define SZYNA_E_ZAKL  (1u << 1) /* zakłócenie krótsze niż filtr */
#define SZYNA_E_PRZEP (1u << 2) /* przepełniona kolejka odbiorcza, zgubione bajty */

/* Rodzaje pozycji. */
#define SZYNA_RAMKA 'R' /* ramka z poprawnym CRC */
#define SZYNA_ACK   'A' /* bajt potwierdzenia: 0x01, 0x02 albo 0x03 */
#define SZYNA_INNE  '?'

/*
 * CRC jak w firmware'ze (docs/protokol-ramek.md): bit 0 sprawdzany po przesunięciu,
 * więc CRC nie zależy od bitu 0 żadnego bajtu; mimo to tak liczą centrala i moduły 🔌.
 */
uint16_t szyna_crc(const uint8_t *p, uint32_t n);
/* Czy p[0..n) to dokładnie jedna ramka z poprawnym CRC. */
int szyna_ramka(const uint8_t *p, uint32_t n);

/* Rozpoznana pozycja: rodzaj, bajty, czas pierwszego bajtu i błędy UART-u przy jej bajtach. */
typedef void (*szyna_wyjscie)(void *arg, char rodzaj, const uint8_t *p, uint32_t n,
                              uint32_t s, uint32_t us, uint32_t bledy);

struct szyna_parser {
    uint8_t b[SZYNA_BUF];
    uint8_t e[SZYNA_BUF];
    uint32_t s[SZYNA_BUF], us[SZYNA_BUF];
    uint32_t n;      /* bajtów w buforze */
    uint32_t smieci; /* tyle bajtów z początku bufora to już uznane śmieci */
    szyna_wyjscie out;
    void *arg;
};

void szyna_init(struct szyna_parser *p, szyna_wyjscie out, void *arg);
/* Kolejny bajt z magistrali, z czasem odbioru i błędami UART-u. */
void szyna_bajt(struct szyna_parser *p, uint8_t b, uint32_t s, uint32_t us, uint32_t bledy);
/* Cisza na magistrali: niedokończone ramki nie dojdą, wszystko w buforze wychodzi. */
void szyna_cisza(struct szyna_parser *p);

/*
 * Linia tekstu o pozycji, zakończona '\n':
 *   "S.UUUUUU K N[ !R!Z!P]: xx xx …"
 * czas (sekundy, mikrosekundy), rodzaj, liczba bajtów, błędy UART-u i bajty.
 * Zwraca długość albo 0, gdy linia nie mieści się w cap.
 */
uint32_t szyna_linia(char *out, uint32_t cap, uint32_t s, uint32_t us, char rodzaj,
                     const uint8_t *p, uint32_t n, uint32_t bledy);

/* Liczba dziesiętnie, co najmniej min cyfr (z zerami z przodu); zwraca liczbę znaków. */
uint32_t szyna_dec(char *out, uint32_t v, uint32_t min);

#endif
