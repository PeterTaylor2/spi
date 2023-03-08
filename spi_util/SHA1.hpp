/*
  100% free public domain implementation of the SHA-1 algorithm
  by Dominik Reichl <dominik.reichl@t-online.de>
  Web: http://www.dominik-reichl.de/

  Subsequently tidied-up - we don't need various utility functions etc.
  Also changed the interface for the SHA1 class to match the interface for the SHA256 class
*/

#ifndef SHA1_H_A545E61D43E9404E8D736869AB3CBFE7
#define SHA1_H_A545E61D43E9404E8D736869AB3CBFE7

#include "SHA.hpp"

// You can define the endian mode in your files without modifying the SHA-1
// source files. Just #define SHA1_LITTLE_ENDIAN or #define SHA1_BIG_ENDIAN
// in your files, before including the SHA1.h header file. If you don't
// define anything, the class defaults to little endian.
#if !defined(SHA1_LITTLE_ENDIAN) && !defined(SHA1_BIG_ENDIAN)
#define SHA1_LITTLE_ENDIAN
#endif

///////////////////////////////////////////////////////////////////////////
// Declare SHA-1 workspace

SPI_UTIL_NAMESPACE

typedef union
{
    unsigned char c[64];
    uint32_t l[16];
} SHA1_WORKSPACE_BLOCK;

class SPI_UTIL_IMPORT SHA1 : public SHA
{
public:
    // Constructor and destructor
    SHA1();

    // Hash in binary data and strings
    void Update(const unsigned char* pbData, uint32_t uLen);

    // Finalize hash and return hexadecimal hash string
    std::string Final();

private:
    // Get the raw message digest (20 bytes)
    bool GetHash(unsigned char* pbDest20) const;
    void Reset();
    // Private SHA-1 transformation
    void Transform(uint32_t* pState, const unsigned char* pBuffer);

    // Member variables
    uint32_t m_state[5];
    uint32_t m_count[2];
    uint32_t m_reserved0[1]; // Memory alignment padding
    unsigned char m_buffer[64];

    unsigned char m_workspace[64];
    SHA1_WORKSPACE_BLOCK* m_block; // SHA1 pointer to the byte array above
};

SPI_UTIL_END_NAMESPACE

#endif // SHA1_H_A545E61D43E9404E8D736869AB3CBFE7
