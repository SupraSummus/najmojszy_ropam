/*
 * Klient protokołu serwisowego OptimaGSM; zob. ropam.h i tools/ropam_serwis.py, który robi to samo.
 */

#include "ropam.h"

#define SOH 0x01
#define STX 0x02
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18
#define BLK64 0x19 /* rozszerzenie Ropamu: blok 64 B */
#define CRC_REQ 'C'

static uint16_t crc16(const uint8_t *p, uint32_t len)
{
    uint32_t crc = 0;
    while (len--) {
        crc ^= (uint32_t)*p++ << 8;
        for (int i = 0; i < 8; i++) {
            crc = (crc & 0x8000u) ? (crc << 1) ^ 0x1021u : crc << 1;
        }
    }
    return (uint16_t)crc;
}

/* Czeka na któryś z bajtów a, b (b < 0: tylko a); zwraca go albo -1. */
static int wait_for(const struct ropam_io *io, int a, int b, uint32_t ms)
{
    uint32_t start = io->now(io->arg);
    for (;;) {
        uint32_t gone = io->now(io->arg) - start;
        if (gone >= ms) {
            return -1;
        }
        int c = io->get(io->arg, ms - gone);
        if (c == a || (b >= 0 && c == b)) {
            return c;
        }
    }
}

/*
 * Czeka na 'C' poprzedzone ciszą (0,5 s): zaległe bajty z przerwanych sesji
 * potrafią jeszcze dochodzić, a prawdziwe zaproszenie przychodzi co 2 s.
 */
static int fresh_invite(const struct ropam_io *io, uint32_t ms)
{
    uint32_t start = io->now(io->arg), last = start, seen = 0;
    while (io->now(io->arg) - start < ms) {
        int c = io->get(io->arg, 100);
        uint32_t t = io->now(io->arg);
        if (c < 0) {
            continue;
        }
        if (c == CRC_REQ && t - last >= 500u) {
            return ROPAM_OK;
        }
        last = t;
        seen++;
    }
    return seen > 10u * (ms / 1000u) ? ROPAM_ECHO : ROPAM_NO_INVITE;
}

int ropam_send(const struct ropam_io *io, const uint8_t *data, uint32_t len)
{
    uint8_t packet[3 + 1024 + 2];
    int r = fresh_invite(io, 10000);
    if (r != ROPAM_OK) {
        return r;
    }
    uint8_t head = len >= 0x400u ? STX : len > 0x40u ? SOH : BLK64;
    uint32_t size = head == STX ? 1024u : head == SOH ? 128u : 64u;
    uint32_t n = 1;
    for (uint32_t off = 0; off < len || off == 0; off += size, n++) {
        uint32_t chunk = len - off < size ? len - off : size;
        packet[0] = head;
        packet[1] = (uint8_t)n;
        packet[2] = (uint8_t)~n;
        for (uint32_t i = 0; i < size; i++) {
            packet[3 + i] = i < chunk ? data[off + i] : (i == chunk ? 0x1a : 0);
        }
        uint16_t crc = crc16(packet + 3, size);
        packet[3 + size] = (uint8_t)(crc >> 8);
        packet[4 + size] = (uint8_t)crc;
        int tries = 0;
        do {
            if (++tries > 10) {
                return ROPAM_NOT_ACKED;
            }
            io->put(io->arg, packet, size + 5);
        } while (wait_for(io, ACK, NAK, 2000) != ACK);
        if (chunk < size) {
            break;
        }
    }
    for (int tries = 0; tries < 10; tries++) {
        uint8_t eot = EOT;
        io->put(io->arg, &eot, 1);
        if (wait_for(io, ACK, -1, 2000) == ACK) {
            return ROPAM_OK;
        }
    }
    return ROPAM_NOT_ACKED;
}

/* Bajty do pełnego bloku: true, jeśli przyszły w czasie. */
static int read_n(const struct ropam_io *io, uint8_t *to, uint32_t n, uint32_t ms)
{
    for (uint32_t i = 0; i < n; i++) {
        int c = io->get(io->arg, ms);
        if (c < 0) {
            return 0;
        }
        to[i] = (uint8_t)c;
    }
    return 1;
}

int ropam_receive(const struct ropam_io *io, uint8_t *out, uint32_t max, uint32_t timeout_ms)
{
    uint8_t block[2 + 1024 + 2];
    uint32_t got = 0, expected = 1;
    uint32_t start = io->now(io->arg);
    uint8_t b = CRC_REQ;
    io->put(io->arg, &b, 1);
    for (;;) {
        int head = io->get(io->arg, 2000);
        if (head < 0) {
            if (io->now(io->arg) - start > timeout_ms) {
                return ROPAM_NO_DATA;
            }
            if (got == 0) {
                io->put(io->arg, &b, 1);
            }
            continue;
        }
        if (head == EOT) {
            uint8_t ack = ACK;
            io->put(io->arg, &ack, 1);
            return (int)got;
        }
        if (head == CAN) {
            return ROPAM_CANCELLED;
        }
        uint32_t size = head == STX ? 1024u : head == SOH ? 128u : head == BLK64 ? 64u : 0u;
        if (size == 0) {
            continue;
        }
        uint8_t reply = NAK;
        if (read_n(io, block, size + 4, 2000) && block[0] == (uint8_t)~block[1] &&
            crc16(block + 2, size) == ((uint16_t)block[2 + size] << 8 | block[3 + size])) {
            if (block[0] == (uint8_t)expected) {
                for (uint32_t i = 0; i < size && got < max; i++) {
                    out[got++] = block[2 + i];
                }
                expected++;
            }
            reply = ACK;
        }
        io->put(io->arg, &reply, 1);
    }
}

int ropam_end_session(const struct ropam_io *io)
{
    int r = fresh_invite(io, 10000);
    if (r != ROPAM_OK) {
        return r;
    }
    for (int i = 0; i < 3; i++) {
        if (i != 0 && wait_for(io, CRC_REQ, -1, 3000) < 0) {
            return ROPAM_NO_INVITE;
        }
        const uint8_t cancel[2] = { CAN, CAN };
        io->put(io->arg, cancel, 2);
        if (wait_for(io, ACK, -1, 2000) != ACK) {
            return ROPAM_NOT_ACKED;
        }
    }
    return ROPAM_OK;
}

int ropam_status(const struct ropam_io *io, uint8_t *out)
{
    static uint8_t buf[ROPAM_STATUS_SIZE + 1024];
    const uint8_t cmd = 0x24;
    int r = ropam_send(io, &cmd, 1);
    if (r != ROPAM_OK) {
        return r;
    }
    r = ropam_receive(io, buf, sizeof(buf), 10000);
    if (r < 0) {
        return r;
    }
    if ((uint32_t)r < ROPAM_STATUS_SIZE) {
        return ROPAM_NO_DATA;
    }
    for (uint32_t i = 0; i < ROPAM_STATUS_SIZE; i++) {
        out[i] = buf[i];
    }
    /* Opcjonalna druga transmisja (odpowiedź modemu): centrala czeka wtedy na nasze 'C' zamiast nadawać swoje. */
    if (wait_for(io, CRC_REQ, -1, 1500) < 0) {
        (void)ropam_receive(io, buf, sizeof(buf), 10000);
    }
    return ropam_end_session(io);
}
