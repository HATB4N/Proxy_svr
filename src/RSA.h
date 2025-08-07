//
// Created by HATBAN on 25. 8. 1.
//

#ifndef RSA_H
#define RSA_H

#include "mpz.h"
#include <math.h>

class RSA {
public:
    RSA(int key_size);
    RSA(mpz_class e, mpz_class d, mpz_class K);
    mpz_class sign(mpz_class m);
    mpz_class decode(mpz_class m);
    mpz_class encode(mpz_class m);
    mpz_class K, e;

protected:
    mpz_class p, q, d, phi;

private:

};

#endif // RSA_H