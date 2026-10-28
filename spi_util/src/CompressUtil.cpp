/*

    Sartorial Programming Interface (SPI) runtime libraries
    Copyright (C) 2012-2023 Sartorial Programming Ltd.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA

*/

#include "CompressUtil.hpp"

#include "Base64.hpp"
#include "DataBuffer.hpp"
#include <zlib/zlib.h>
#include "RuntimeError.hpp"
#include "Utils.hpp"

#undef SPI_UTIL_CLOCK_EVENTS
#include "ClockUtil.hpp"

#include "platform.h"
#include <string.h>

SPI_UTIL_NAMESPACE

/**
 * Uses ZLIB to compress the input buffer and return allocated output.
 *
 * Note that the output contains the length of the original input at the start.
 */
void CompressData(DataBuffer& out, const DataBuffer& input, int level)
{
    if (!input.data || !input.size)
    {
        throw RuntimeError("%s: NULL inputs", __FUNCTION__);
    }

    size_t sizeNeeded = compressBound(IntegerCast<z_uLong>(input.size)) + sizeof(uint32_t);
    out.Allocate(sizeNeeded);

    // ignore big/little endian issues
    *(uint32_t*)out.data = IntegerCast<uint32_t>(input.size);

    uLongf actualSize = IntegerCast<uLongf>(sizeNeeded - sizeof(uint32_t));

    // call to ZLIB
    int result = compress2((z_Bytef*)out.data+sizeof(uint32_t),
                           &actualSize,
                           (z_Bytef*)input.data,
                           IntegerCast<z_uLong>(input.size),
                           level);


    /*
     compress2 returns Z_OK if success, Z_MEM_ERROR if there was not enough
     memory, Z_BUF_ERROR if there was not enough room in the output buffer,
     Z_STREAM_ERROR if the level parameter is invalid.
    */

    switch(result)
    {
    case Z_OK:
        break;
    case Z_MEM_ERROR:
        throw RuntimeError("%s: Out of memory", __FUNCTION__);
    case Z_BUF_ERROR:
        throw RuntimeError("%s: Not enough room in the output buffer", __FUNCTION__);
    case Z_STREAM_ERROR:
        throw RuntimeError("%s: Bad compression level %d", __FUNCTION__, level);
    default:
        throw RuntimeError("%s: Unexpected ZLIB error %d", __FUNCTION__, result);
    }

    out.size = actualSize + sizeof(uint32_t);
    return;
}

/**
 * Uses ZLIB to uncompress the input buffer and return allocated output.
 *
 * Assumes that the compressed buffer contains the length of the original
 * input at the start in order to correctly allocate the size of the output.
 *
 * out.data is allocated using malloc and needs to be freed using free.
 */
void UncompressData(DataBuffer& out, const DataBuffer& input)
{
    if (!input.data || !input.size)
    {
        throw RuntimeError("%s: NULL inputs", __FUNCTION__);
    }

    if (input.size < sizeof(uint32_t))
    {
        throw RuntimeError("%s: Input buffer too small to determine original size",
            __FUNCTION__);
    }

    // ignore big/little endian issues
    uint32_t originalSize = *(uint32_t*)input.data;

    try
    {
        out.Allocate(IntegerCast<size_t>(originalSize));
    }
    catch (...)
    {
        throw RuntimeError(
            "Unable to allocate %ld bytes during de-compression of buffer of size %ld",
            (long)originalSize, (long)input.size);
    }

    uLongf actualSize = IntegerCast<uLongf>(originalSize);
    int result = uncompress((z_Bytef*)out.data,
                            &actualSize,
                            (z_Bytef*)input.data + sizeof(uint32_t),
                            IntegerCast<z_uLong>(input.size - sizeof(uint32_t)));

    /*
        uncompress returns Z_OK if success, Z_MEM_ERROR if there was not
        enough memory, Z_BUF_ERROR if there was not enough room in the output
        buffer, or Z_DATA_ERROR if the input data was corrupted or incomplete.  In
        the case where there is not enough room, uncompress() will fill the output
        buffer with the uncompressed data up to that point.
    */


    switch(result)
    {
    case Z_OK:
        break;
    case Z_MEM_ERROR:
        throw RuntimeError("%s: Out of memory", __FUNCTION__);
    case Z_BUF_ERROR:
        throw RuntimeError("%s: Not enough room in the output buffer", __FUNCTION__);
    case Z_DATA_ERROR:
        throw RuntimeError("%s: Corrupted or incomplete input data", __FUNCTION__);
    default:
        throw RuntimeError("%s: Unexpected ZLIB error %d", __FUNCTION__, result);
    }

    out.size = actualSize;
    return;
}

/**
 * Compresses the input and then uuencodes the result to create a string.
 */
std::string CompressDataAndEncode(const DataBuffer& in, int level)
{
    DataBuffer out;
    CompressData(out, in, level);
    return B64Encode(out);
}

/**
 * uudecodes the input string and then uncompresses to create output.
 *
 * out.data is allocated using malloc and needs to be freed using free.
 */
void DecodeTextAndUncompress(DataBuffer& out, const std::string& input)
{
    DataBuffer in;
    B64Decode(in, input.c_str());
    UncompressData(out, in);
}

/**
 * Compresses a text string using ZLIB and then uuencodes the result.
 */
std::string CompressText(const std::string& str)
{
    SPI_UTIL_CLOCK_FUNCTION();

    DataBuffer in(str);
    return CompressDataAndEncode(in, -1);
}

/**
* Performs base64 decoding on the string and then uncompresses the
* resulting data.
*
* The string provided is modified to reduce copying of strings.
*
* If the output is not a null-terminated string then an exception is thrown.
*/
void UncompressText(std::string& str)
{
    SPI_UTIL_CLOCK_FUNCTION();

    DataBuffer out;
    DecodeTextAndUncompress(out, str);

    if (out.size == 0)
        throw RuntimeError("Nothing decoded");

    const char* buf = (const char*)out.data;
    size_t size = out.size;
    if (buf[size] != '\0')
        throw RuntimeError("%s: Uncompressed output is not a string",
            __FUNCTION__);

    str.swap(out.str);
    out.data = NULL;
    out.size = 0;
}

SPI_UTIL_END_NAMESPACE
