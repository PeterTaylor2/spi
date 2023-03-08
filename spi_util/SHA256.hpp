#ifndef SPI_UTIL_SHA256_HPP
#define SPI_UTIL_SHA256_HPP

#include "SHA.hpp"

SPI_UTIL_NAMESPACE

struct sha256_state
{
    uint64_t length;
    uint32_t state[8], curlen;
    unsigned char buf[64];
};

class SPI_UTIL_IMPORT SHA256 : public SHA
{
public:
    SHA256();
    void Update(const unsigned char* in, uint32_t inlen);
    /// returns a 64-character string in hex format representing the 256-bits of the hash
    std::string Final();

private:
    sha256_state state;
};

SPI_UTIL_END_NAMESPACE

#endif
