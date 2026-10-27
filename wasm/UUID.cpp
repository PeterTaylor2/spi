#include <spi_util/UUID.hpp>
#include <emscripten.h>

// UUIDs use browser entropy rather than a native process/thread identifier.
EM_JS(void, random_bytes, (unsigned char* bytes), {
    globalThis.crypto.getRandomValues(HEAPU8.subarray(bytes, bytes + 16));
});

SPI_UTIL_NAMESPACE
std::string uuid_string()
{
    unsigned char bytes[16];
    random_bytes(bytes);
    bytes[6] = (bytes[6] & 0x0f) | 0x40;
    bytes[8] = (bytes[8] & 0x3f) | 0x80;
    const char* hex = "0123456789abcdef";
    std::string result;
    for (int i = 0; i < 16; ++i)
    {
        if (i == 4 || i == 6 || i == 8 || i == 10)
            result += '-';
        result += hex[bytes[i] >> 4];
        result += hex[bytes[i] & 15];
    }
    return result;
}
SPI_UTIL_END_NAMESPACE
