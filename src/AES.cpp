//
// Created by HATBAN on 25. 7. 17.
//

#include "AES.h"

void AES::encrypt(uint8_t *m) const {
    add_round_key(m, 0);
    for (int round = 1; round< ROUND-1; round++) {
        substitute(m);
        shift_row(m);
        mix_column(m);
        add_round_key(m, round);
    }
    substitute(m);
    shift_row(m);
    add_round_key(m, ROUND-1);
}

void AES::decrypt(uint8_t *m) const {
    add_round_key(m, ROUND-1);
    for (int round = ROUND-2; round> 0; round--) {
        inv_shift_row(m);
        inv_substitute(m);
        add_round_key(m, round);
        inv_mix_column(m);
    }
    inv_shift_row(m);
    inv_substitute(m);
    add_round_key(m, 0);
}


void AES::shift_row(uint8_t *p) {
    uint8_t tmp, tmp2;
    tmp = p[1]; p[1] = p[5]; p[5] = p[9]; p[9] = p[13]; p[13] = tmp;
    tmp = p[2]; tmp2 = p[6]; p[2] = p[10]; p[6] = p[14]; p[10] = tmp; p[14] = tmp2;
    tmp = p[3]; p[3] = p[15]; p[15] = p[11]; p[11] = p[7]; p[7] = tmp;
}

void AES::inv_shift_row(uint8_t *p) {
    uint8_t tmp, tmp2;
    tmp = p[13]; p[13] = p[9]; p[9] = p[5]; p[5] = p[1]; p[1] = tmp;
    tmp = p[10]; tmp2 = p[14]; p[14] = p[6]; p[10] = p[2]; p[6] = tmp2; p[2] = tmp;
    tmp = p[7]; p[7] = p[11]; p[11] = p[15]; p[15] = p[3]; p[3] = tmp;
}

void AES::substitute(uint8_t *p) {
    for (int i = 0; i< 16; i++) {
        p[i] = sbox[p[i]];
    }
}

void AES::inv_substitute(uint8_t *p) {
    for (int i = 0; i< 16; i++) {
        p[i] = inv_sbox[p[i]];
    }
}

void AES::mix_column(uint8_t *p) {
    static const uint8_t mix[4][4] = {
        {2, 3, 1, 1}, {1, 2, 3, 1}, {1, 1, 2, 3}, {3, 1, 1, 2}
    };
    uint8_t c[4], d, result[16];
    for (int y = 0; y< 4; y++) for (int x = 0; x< 4; x++) {
        for (int i = 0; i< 4; i++) {
            d = p[4 * x + i];
            switch (mix[y][i]) {
                case 1: c[i] = d; break;
                case 2: c[i] = doub(d); break;
                case 3: c[i] = doub(d) ^ d; break;
            }
        }
        result[4 * x + y] = c[0] ^ c[1] ^ c[2] ^ c[3];
    }
    memcpy(p, result, 16);
}

void AES::inv_mix_column(uint8_t *p) {
    static const uint8_t mix[4][4] = {
        {14, 11, 13, 9}, {9, 14, 11, 13}, {13, 9, 14, 11}, {11, 13, 9, 14}
    };
    uint8_t c[40], d, result[16];
    for (int y = 0; y< 4; y++) for (int x = 0; x< 4; x++) {
        for (int i = 0; i< 4; i++) {
            d = p[4 * x + i];
            switch (mix[y][i]) {
                case 9: c[i] = doub(doub(doub(d))) ^ d; break;
                case 11: c[i] = doub(doub(doub(d)) ^ d) ^ d; break;
                case 13: c[i] = doub(doub(doub(d) ^ d)) ^ d; break;
                case 14: c[i] = doub(doub(doub(d) ^ d) ^ d); break;
            }
        }
        result[4 * x + y] = c[0] ^ c[1] ^ c[2] ^ c[3];
    }
    memcpy(p, result, 16);
}

void AES::key(const uint8_t *pk) {
    memcpy(schedule_[0], pk, 16);
    uint8_t *p = &schedule_[1][0];
    for (int i = 1; i< ROUND; i++) {
        for (int j = 0; j< 3; j++) *(p + j) = *(p + j - 3);
        *(p + 3) = *(p - 4);
        for (int j = 0; j< 4; j++) *(p + j) = sbox[*(p + j)];
        for (int j = 0; j< 4; j++, p++) {
            *p ^= rcon[(4 * i / N) - 1][j];
            *p ^= *(p - (4 * N));
        }
        for (int j = 0; j< 12; j++, p++) *p = *(p - (4 * N)) ^ *(p - 4);
    }
}

void AES::add_round_key(uint8_t *p, int round) const {
    for (int i = 0; i< 16; i++) p[i] ^= schedule_[round][i];
}
