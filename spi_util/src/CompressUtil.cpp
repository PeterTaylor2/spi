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

#include <zlib/zlib.h>
#include "RuntimeError.hpp"
#include "Utils.hpp"

#undef SPI_UTIL_CLOCK_EVENTS
#include "ClockUtil.hpp"

#include "platform.h"
#include <string.h>

SPI_UTIL_NAMESPACE

DataBuffer::DataBuffer(const std::string& in)
    :
    data(NULL),
    size(0),
    str(in)
{
    size = str.size();
    if (size > 0)
        data = &str[0];
}

DataBuffer::DataBuffer()
    :
    data(NULL),
    size(0),
    str()
{}

void DataBuffer::Allocate(size_t newSize)
{
    str.resize(newSize);
    size = newSize;
    if (size > 0)
        data = &str[0];
    else
        data = NULL;
}

namespace
{
    /*
     * Functions for base64 encoding and decoding are based on b64.c by Bob Trower 2001/08/04
     *
     *
     * Copyright (c) Trantor Standard Systems Inc., 2001
     *
     * This source code may be used as you wish, subject to
     * the MIT license.  See the LICENCE section below.
     *
     * This little utility implements the Base64
     * Content-Transfer-Encoding standard described in
     * RFC1113 (http://www.faqs.org/rfcs/rfc1113.html).
     *
     * This is the coding scheme used by MIME to allow
     * binary data to be transferred by SMTP mail.
     *
     * Groups of 3 bytes from a binary stream are coded as
     * groups of 4 bytes in a text stream.
     *
     * The input stream is 'padded' with zeros to create
     * an input that is an even multiple of 3.
     *
     * A special character ('=') is used to denote padding so
     * that the stream can be decoded back to its exact size.
     *
     * Encoded output is formatted in lines which should
     * be a maximum of 72 characters to conform to the
     * specification.  This program defaults to 72 characters,
     * but will allow more or less through the use of a
     * switch.  The program enforces a minimum line size
     * of 4 characters.
     *
     * LICENCE:
     *
     * Copyright (c) 2001 Bob Trower, Trantor Standard Systems Inc.
     *
     * Permission is hereby granted, free of charge, to any person
     * obtaining a copy of this software and associated
     * documentation files (the "Software"), to deal in the
     * Software without restriction, including without limitation
     * the rights to use, copy, modify, merge, publish, distribute,
     * sublicense, and/or sell copies of the Software, and to
     * permit persons to whom the Software is furnished to do so,
     * subject to the following conditions:
     *
     * The above copyright notice and this permission notice shall
     * be included in all copies or substantial portions of the
     * Software.
     *
     * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
     * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
     * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
     * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
     * OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
     * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
     * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
     * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
     *******************************************************************
    */

/*
** Translation Table as described in RFC1113
*/
static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
** Translation Table to decode (created by author, i.e. Bob Trower)
*/
static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

/*
** encodeblock
**
** encode 3 8-bit binary bytes as 4 '6-bit' characters
**
** coded as is from b64.c
*/
static void encodeblock( unsigned char *in, unsigned char *out, size_t len )
{
    out[0] = (unsigned char) cb64[ (int)(in[0] >> 2) ];
    out[1] = (unsigned char) cb64[ (int)(((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)) ];
    out[2] = (unsigned char) (len > 1 ? cb64[ (int)(((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6)) ] : '=');
    out[3] = (unsigned char) (len > 2 ? cb64[ (int)(in[2] & 0x3f) ] : '=');
}

/*
** B64Encode
**
** base64 encode a stream adding padding and line breaks as per spec.
**
** modified from b64.c encode function to use data structures in this file
*/
std::string B64Encode(const DataBuffer& buf)
{
    int blocksout = 0;
    int maxblocks = 15;
    size_t pos = 0;
    std::ostringstream ostr;

    while (pos < buf.size)
    {
        unsigned char in[3];
        size_t len = 3;
        if (pos + 3 <= buf.size)
        {
            memcpy(&in[0], (char*)buf.data + pos, 3);
            len = 3;
        }
        else
        {
            memset(&in[0], 0, 3);
            len = buf.size - pos;
            memcpy(&in[0], (char*)buf.data + pos, len);
        }
        pos += len;

        char out[4];
        encodeblock( (unsigned char*)&in[0], (unsigned char*)&out[0], len );
        ostr.write(out, 4);

        ++blocksout;
        if (blocksout >= maxblocks)
        {
            ostr.put('\n');
            blocksout = 0;
        }
    }
    if (blocksout > 0)
        ostr.put('\n');

    return ostr.str();
}

/*
** decodeblock
**
** decode 4 '6-bit' characters into 3 8-bit binary bytes
**
** copied as is from b64.c
*/
static void decodeblock( unsigned char *in, unsigned char *out )
{
    out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
    out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
    out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
}

/*
** B64Decode
**
** decode a base64 encoded stream discarding padding, line breaks and noise
**
** modified from b64.c decode function to use data structures from this file
*/
void B64Decode(DataBuffer& buf, const char* str)
{
    if (!str || !*str)
    {
        throw RuntimeError("%s: Nothing to decode", __FUNCTION__);
    }

    size_t len = strlen(str);
    buf.Allocate(len); // too much
    buf.size = 0; // actual size used

    size_t strPos = 0;

    unsigned char in[4];
    memset(in, 0, 4);
    size_t inPos = 0;

    const char* ptr = str;
    while (*ptr)
    {
        int v = (int)(*ptr);
        ++ptr;

        if (v < 43 || v > 122)
            v = 0;
        else
            v = (int) cd64[ v - 43 ];

        if (v != 0)
        {
            v = ((v == (int)'$') ? 0 : v - 61);
        }

        if (v > 0)
        {
            in[inPos] = (unsigned char)(v-1);
            ++inPos;
        }

        if (inPos == 4)
        {
            unsigned char out[3];
            decodeblock( &in[0], &out[0] );

            memcpy((char*)buf.data + buf.size, &out[0], 3);
            buf.size += 3;

            memset(in, 0, 4);
            inPos = 0;
        }
    }

    // we might have a partially filled block of 4
    // each encoded byte corresponds to 3/4 of a binary byte,
    // hence one is not enough, two/three are OK
    if (inPos > 1)
    {
        unsigned char out[3];
        decodeblock( &in[0], &out[0] );

        memcpy((char*)buf.data + buf.size, &out[0], (inPos-1));
        buf.size += inPos-1;
    }
}




} // end of anonymous namespace


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
