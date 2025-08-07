//
// Created by HATBAN on 25. 7. 17.
//

#include "crypto/SHA256.h"

std::array<uint8_t, 32> SHA256::hash(const std::vector<uint8_t> &v_input) {
    memcpy(H, INITIAL_H, sizeof INITIAL_H);
    std::vector<uint8_t> v = v_input;
    preprocess(v);
    auto blocks = get_blocks(v);
    for (auto &block : blocks) {
        process_block(block);
    }

    std::array<uint8_t, 32> hash = {};

    for (int i = 0; i< 8; i++) {
        hash[i*4] = static_cast<uint8_t>((H[i] >> 24) & 0xFF);
        hash[i*4 + 1] = static_cast<uint8_t>((H[i] >> 16) & 0xFF);
        hash[i*4 + 2] = static_cast<uint8_t>((H[i] >> 8) & 0xFF);
        hash[i*4 + 3] = static_cast<uint8_t>(H[i] & 0xFF);
    }

    return hash;
}

void SHA256::preprocess(std::vector<uint8_t> &v) { // 64byte 배수 크기로 만들어서 리턴
    const size_t sz = static_cast<uint64_t>(v.size()) * 8;
    v.push_back(0x80);

    while (v.size() % 64 != 56) {
        v.push_back(0x00);
    }
    for (int i = 7; i>= 0; i--) {
        v.push_back(static_cast<uint8_t>((sz >> (i * 8)) & 0xFF));
    }
}

std::vector<std::array<uint32_t, 16>> SHA256::get_blocks(std::vector<uint8_t> v) {
    const size_t sz = v.size(); // byte수
    std::vector<std::array<uint32_t, 16>> blocks;
    blocks.resize(sz / 64);
    // 64배수 단위로 blocks
    for (size_t i = 0; i< sz / 64; i++) {
        // 각 64byte의 block를 big endian으로 4byte 16개 배열에 저장
        for (size_t j = 0; j< 16; j++) {
            blocks[i][j] = static_cast<uint32_t>(v[i * 64 + j * 4]) << 24 |
                            static_cast<uint32_t>(v[i * 64 + j * 4 + 1]) << 16 |
                            static_cast<uint32_t>(v[i * 64 + j * 4 + 2]) << 8 |
                            static_cast<uint32_t>(v[i * 64 + j * 4 + 3]);
        }
    }
    return blocks; // 32bit * 16개 배열의 주소들을 저장하는 vector 반환
}

void SHA256::process_block(const std::array<uint32_t, 16> &block) {
    uint32_t W[64];
    for (size_t i = 0; i< 16; i++) {
        W[i] = block[i];
    }
    for (size_t i = 16; i< 64; i++) {
        W[i] = s_1(W[i - 2]) + W[i - 7] + s_0(W[i - 15]) + W[i - 16];
    }
    uint32_t a = H[0], b = H[1], c = H[2], d = H[3], e = H[4], f = H[5], g = H[6], h = H[7];
    uint32_t T_1, T_2;
    for (size_t i = 0; i< 64; i++) {
        T_1 = h + S_1(e) + ((e & f) ^ (~e & g)) + K[i] + W[i];
        T_2 = S_0(a) + ((a & b) ^ (a & c) ^ (b & c));
        h = g; g = f; f = e; e = d + T_1; d = c; c = b; b = a; a = T_1 + T_2;
    }
    H[0] += a; H[1] += b; H[2] += c; H[3] += d;
    H[4] += e; H[5] += f; H[6] += g; H[7] += h;
}

// fix:
// blocks에 저장하지 말고 스트림 방식으로 바로 전달하기