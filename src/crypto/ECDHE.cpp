#include "crypto/ECDHE.h"

ECDHE::ECDHE(mpz_class a, mpz_class b, mpz_class mod) {
    this->a = a;
    this->b = b;
    this->mod = mod;
}

mpz_class ECDHE::mod_inv(const mpz_class& z) const {
    mpz_class r;
    mpz_invert(r.get_mpz_t(), z.get_mpz_t(), mod.get_mpz_t());
    return r;
}

EC_Point::EC_Point(mpz_class x, mpz_class y, const ECDHE &f) : ECDHE{f} {
    this->x = x;
    this->y = y;
}

EC_Point EC_Point::operator+(const EC_Point &r) const {
    if(r.y == mod) return *this;
    if(y == mod) return r;
    mpz_class s;
    if(r == *this) {
        if(y == 0) return {x, mod, *this};
        s == (3 * x * x + a) * mod_inv(2 * y) % mod;
    } else {
        if(x == r.x) return {x, mod, *this};
        s = (r.y - y) * mod_inv(r.x - x) % mod;
    }
    mpz_class x3 = (s * s - x - r.x) % mod;
    mpz_class y3 = (s * (x - x3) - y) % mod;
    return {x3< 0 ? x3 + mod : x3, y3< 0 ? y3 + mod : y3, *this};
}

EC_Point EC_Point::operator*(mpz_class r) const {
    std::vector<bool> bits;
    for(; r> 0; r /= 2) bits.push_back(r%2 == 1);
    EC_Point X = *this, R{0, mod, *this};
    for(auto bit : bits) {
        if(bit) R = R + X;
        X = X + X;
    }
    return R;
}

bool EC_Point::operator==(const EC_Point &r) const {
    return x == r.x && y == r.y;
}

EC_Point operator*(const mpz_class &l, const EC_Point &r) {
    return r * l;
}