#include "mpz.h"

mpz_class nextprime(mpz_class n) {
    mpz_class r;
    mpz_nextprime(r.get_mpz_t(), n.get_mpz_t());
    return r;
}

mpz_class powm(mpz_class base, mpz_class exp, mpz_class mod) {
    mpz_class r;
    mpz_powm(r.get_mpz_t(), base.get_mpz_t(), exp.get_mpz_t(), mod.get_mpz_t());
    return r;
}

template <typename It>
void mpz2bnd(mpz_class n, It begin, It end) {
    for (It i = end; i != begin; n /= 0x100) {
        i--;
        *i = mpz_class{n % 0x100}.get_ui();
    }
}

template <typename It>
mpz_class bnd2mpz(It begin, It end) {
    std::stringstream ss;
    ss << "0x";
    for (It i = begin; i != end; i++) {
        ss << std::hex << std::setfill('0') << std::setw(2) << +*i;
    }
    return mpz_class{ss.str()};
}

mpz_class random_prime(unsigned byte) {
    std::vector<uint8_t> arr(byte);
    std::uniform_int_distribution<> di(0, 0xff);
    std::random_device rd;
    for (size_t i = 0; i < byte; i++)
        arr[i] = di(rd);
    auto z = nextprime(bnd2mpz(arr.begin(), arr.end()));
    for (size_t i = 0; i < byte; i++)
        arr[i] = 0xff;
    if (z > bnd2mpz(arr.begin(), arr.end()))
        return random_prime(byte);
    else
        return z;
}

template <class C>
std::string hexprint(const char *p, const C &c) {
    std::stringstream ss;
    ss << p << " : 0x";
    for (uint8_t a : c)
        ss << std::hex << std::setw(2) << std::setfill('0') << +a;
    return ss.str();
}

template void mpz2bnd<unsigned char*>(mpz_class, unsigned char*, unsigned char*);
template mpz_class bnd2mpz<unsigned char*>(unsigned char*, unsigned char*);

template void mpz2bnd<std::vector<uint8_t>::iterator>(mpz_class, std::vector<uint8_t>::iterator, std::vector<uint8_t>::iterator);
template mpz_class bnd2mpz<std::vector<uint8_t>::iterator>(std::vector<uint8_t>::iterator, std::vector<uint8_t>::iterator);

template std::string hexprint<std::vector<uint8_t>>(const char*, const std::vector<uint8_t>&);
