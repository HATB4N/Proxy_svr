//
// Created by HATBAN on 25. 8. 1.
//

#ifndef ECDHE_H
#define ECDHE_H

#include "mpz.h"
#include <vector>

class ECDHE {
public:
    ECDHE(mpz_class a, mpz_class b, mpz_class mod);

protected:
    mpz_class a, b, mod;
    mpz_class mod_inv(const mpz_class& z) const;

private:

};

struct EC_Point : ECDHE {
public:
    EC_Point(mpz_class x, mpz_class y, const ECDHE &f);
    mpz_class x, y;
    EC_Point operator+(const EC_Point &r) const;
    EC_Point operator*(mpz_class r) const;
    bool operator==(const EC_Point &r) const;
};

std::ostream& operator<<(std::ostream &is, const EC_Point &r);
EC_Point operator*(const mpz_class &l, const EC_Point &r);

#endif // ECDHE_H