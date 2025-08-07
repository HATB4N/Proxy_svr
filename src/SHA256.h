//
// Created by HATBAN on 25. 7. 17.
//

#ifndef SHA256_H
#define SHA256_H

#include <array>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

class SHA256 {
    // 256bit 고정 길이 해쉬 반환
public:
    SHA256() = default; // big endian 관련 추가
    std::array<uint8_t, 32> hash(const std::vector<uint8_t> &v);
    template <class It>
    std::array<uint8_t, 32> hash(const It begin, const It end) {
        return hash(std::vector<uint8_t>(begin, end));
    }
    std::array<uint8_t, 32> hash(const std::string &s) {
        return hash(s.begin(), s.end());
    }

private:
    static void preprocess(std::vector<uint8_t> &v);
    static std::vector<std::array<uint32_t, 16>>
    get_blocks(std::vector<uint8_t> v);
    void process_block(const std::array<uint32_t, 16> &block);

    uint32_t H[8] = {};

    // inline functions

    static uint32_t rot_r(uint32_t x, size_t bits) {
        return x << bits | x >> (32 - bits);
    }

    static uint32_t S_0(uint32_t x) {
        return rot_r(x, 2) ^ rot_r(x, 13) ^ rot_r(x, 22);
    }

    static uint32_t S_1(uint32_t x) {
        return rot_r(x, 6) ^ rot_r(x, 11) ^ rot_r(x, 25);
    }

    static uint32_t s_0(uint32_t x) {
        return rot_r(x, 7) ^ rot_r(x, 18) ^ x >> 3;
    }

    static uint32_t s_1(uint32_t x) {
        return rot_r(x, 17) ^ rot_r(x, 19) ^ x >> 10;
    }

    // constant or initialize values
    static constexpr uint32_t INITIAL_H[8] = { // 생성자에서 Init하기
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    static constexpr uint32_t K[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,0x923f82a4, 0xab1c5ed5, 
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,0x06ca6351, 0x14292967, 
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
    };

#endif // SHA256_H
