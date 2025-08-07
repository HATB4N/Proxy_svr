#include "crypto/RSA.h"

RSA::RSA(int key_size) { // gen
    p = random_prime(key_size/2);
    q = random_prime(key_size/2);
    K = p * q;
    phi = lcm(p-q, q-1);
    for(e = 0x10001; gcd(e, phi) != 1; e = nextprime(e));
    mpz_invert(d.get_mpz_t(), e.get_mpz_t(), phi.get_mpz_t());
}

RSA::RSA(mpz_class e, mpz_class d, mpz_class K) { // read
    this->e = e;
    this->d = d;
    this->K = K;
}

mpz_class RSA::encode(mpz_class m) {
    return powm(m, e, K);
}

mpz_class RSA::decode(mpz_class m) {
    return powm(m, d, K);
}

mpz_class RSA::sign(mpz_class m) {
    return decode(m);
}