#ifndef MPZ_H
#define MPZ_H

#include <array>
#include <cstdint>
#include <gmpxx.h>
#include <iomanip>
#include <random>
#include <vector>
#include <sstream>
#include <string>

mpz_class nextprime(mpz_class n);

mpz_class powm(mpz_class base, mpz_class exp, mpz_class mod);

template <typename It>
void mpz2bnd(mpz_class n, It begin, It end);

template <typename It>
mpz_class bnd2mpz(It begin, It end);

mpz_class random_prime(unsigned byte);

template <class C>
std::string hexprint(const char *p, const C &c);

#endif //MPZ_H