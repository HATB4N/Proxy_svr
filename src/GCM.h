//
// Created by HATBAN on 25. 7. 31.
//

#ifndef GCM_H
#define GCM_H

#include "AES.h"
#include <cstdint>
#include <array>
#include <vector>
#include <cstring>

class GCM { //aes128-gcm
public:
    GCM() = default;
    void iv(const uint8_t *p); // 12byte iv(static) + 4byte counter(dynamic)
    void key(const uint8_t *Ek); // aes encryption key
    void aad(uint8_t *p, int sz); // aad = seq_num + TLSCompressed.type + TLSCompressed.version + TLSCompressed.length;
    std::array<uint8_t, 16> encrypt(uint8_t *msg, int sz);
    std::array<uint8_t, 16> decrypt(uint8_t *msg, int sz);

protected:
    std::array<uint8_t, 16> iv_;
    std::vector<uint8_t> aad_;
    std::array<uint8_t, 16> lenAC_; // len(authdata) || len(ciphertext)
    std::array<uint8_t, 16> H = {0}; // initialized at key() (aes encryption with E_k)
    
private:
    AES aes;
    void process_block(uint8_t *block, uint32_t ctr, int sz);
    std::array<uint8_t, 16> gen_auth(uint8_t *msg, int sz);
    void mult_H(uint8_t *x);
};


#endif // GCM_H
