//
// Created by HATBAN on 25. 7. 31.
//

#include "crypto/GCM.h"

void GCM::iv(const uint8_t *p) {
    memcpy(this->iv_.data(), p, 12);
}

void GCM::key(const uint8_t *Ek) {
    this->aes.key(Ek);
    aes.encrypt(H.data()); // encrypt H with E_k
}

void GCM::aad(uint8_t *p, int sz) {
    // aad = seq_num + TLSCompressed.type + TLSCompressed.version + TLSCompressed.length;
    // <=> seq num + tls header (anyway external value)
    aad_.resize(sz);
    memcpy(aad_.data(), p, sz);

    // lenAC_ = len(ciphertext) || len(auth_data)
    // in this fn, the cipher part is filled. the auth_data part is located at gen_auth 
    uint64_t aad_bits = static_cast<uint64_t>(sz) * 8;
    for (int i = 0; i< 8; i++) {
        lenAC_[i] = static_cast<uint8_t>((aad_bits >> (56 - 8*i)) & 0xFF);
    }

    while (aad_.size() % 16) {
        aad_.push_back(0);
    }
}

void GCM::process_block(uint8_t *block, uint32_t ctr, int sz) {
    std::array<uint8_t, 16> nonce;
    memcpy(nonce.data(), this->iv_.data(), 12);
    std::array<uint8_t, 4> ctr_arr = {
        static_cast<uint8_t>((ctr >> 24) & 0xff), static_cast<uint8_t>((ctr >> 16) & 0xff), 
        static_cast<uint8_t>((ctr >> 8) & 0xff), static_cast<uint8_t>(ctr & 0xff)
    }; // for ctr>0 : ctr1: gen auth, ctr2, 3, ...: get nonce
    memcpy(nonce.data()+12, ctr_arr.data(), 4);
    this->aes.encrypt(nonce.data());

    for (int i = 0; i< sz; i++) {
        block[i] ^= nonce[i];
    }
}

std::array<uint8_t, 16> GCM::encrypt(uint8_t *msg, int sz) {
    for (int i = 0; i< sz; i+=16) {
        process_block((msg + i), i/16 + 2, (sz-i< 16 ? sz-i : 16));
        // ctr starts at 2
    }
    return gen_auth(msg, sz); // give ciphertext not plaintext
}

std::array<uint8_t, 16> GCM::decrypt(uint8_t *msg, int sz) {
    // input msg is ciphertext
    auto auth = gen_auth(msg, sz); // not plain
    for (int i = 0; i< sz; i+=16) {
        process_block((msg + i), i/16 + 2, (sz-i< 16 ? sz-i : 16));
        // ctr starts at 2
    } // cipher -> plain decrypted
    return auth;
}

std::array<uint8_t, 16>GCM::gen_auth(uint8_t *msg, int sz) {
    // use after when ciphertext, aad, key is ready
    std::array<uint8_t, 16> auth;
    std::vector<uint8_t> x = aad_;

    mult_H(&x[0]);
    for (size_t i = 0; i< x.size()-16; i+=16) {
        for (size_t j = 0; j< 16; j++) {
            x[i + 16 + j] ^= x[i + j];
        }
        mult_H(&x[i + 16]);
    }
    memcpy(auth.data(), (x.data()+x.size()-16), 16);

    for (int i = 0; i< sz; i+=16) {
        for (int j = 0; j< (sz - i< 16 ? sz - i : 16); j++) {
            auth[j] ^= msg[i+j];
        }
        mult_H(&auth[0]); // kinda hasing btw :v
    }

    // lenAC_ = len(ciphertext) || len(auth_data)
    // in this fn, the auth_data part is filled. cipher part is located at aad
    uint64_t cipher_bits = static_cast<uint64_t>(sz) * 8; // conv byte to bits
    for (int i = 0; i< 8; i++) {
        lenAC_[i + 8] = static_cast<uint8_t>((cipher_bits >> (56 - 8 * i)) & 0xFF);
    }

    for (int i = 0; i< 16; i++) {
        auth[i] ^= lenAC_[i];
    }
    mult_H(&auth[0]);
    process_block(&auth[0], 1, 16);
    return auth;
}

void GCM::mult_H(uint8_t *v) {
    // NIST SP 800-38D 11p~
    // v *= H in GF(2^128)/x^128+x^7+x^2+x+1
    // read first 16byte from ptr
    bool lsb;
    uint8_t z[16] = {0};
    for (int i = 0; i< 16; i++) {
        for (int j = 0; j< 8; j++) {
            if(H[i] & 1<<(7 - j)) { 
                for (int k = 0; k< 16; k++) z[k] ^= v[k];
            }
            lsb = v[15] & 0x01;
            for(int k = 15; k> 0; k--) v[i] = (v[i] >> 1) | (v[i - 1] << 7);
            v[0] >>= 1;
            if(lsb) v[0] ^= 0xe1; // 0b11100001
        }
    }
    memcpy(v, z, 16);
}

// fix:
// lenAC_ fragmentiaon (put all them into gen_auth or other fn)