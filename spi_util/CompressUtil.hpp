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

/**
***************************************************************************
** CompressUtil.hpp
***************************************************************************
** Compression functions implemented using the industry standard zlib.
***************************************************************************
*/

#ifndef SPI_UTIL_COMPRESS_UTIL_HPP
#define SPI_UTIL_COMPRESS_UTIL_HPP

#include "DeclSpec.h"
#include "Namespace.hpp"

#include <string>

SPI_UTIL_NAMESPACE

/*
 * Defines a data buffer for use with the compression routines.
 *
 * You can construct from a pointer and size (in which case it is
 * your pointer and it won't be freed when the DataBuffer goes out
 * of scope).
 *
 * After construction, you can subsequently allocate data - in which
 * case the data is owned by the DataBuffer and will be freed when the
 * DataBuffer goes out of scope.
 */
struct DataBuffer
{
    DataBuffer(const std::string& data);
    DataBuffer();

    void Allocate(size_t newSize);

    void* data; // &str[0]
    size_t size;
    std::string str; // actual data storage

private:
    DataBuffer(const DataBuffer&);
    DataBuffer& operator=(const DataBuffer&);
};

/**
 * Uses ZLIB to compress the input buffer and return allocated output.
 *
 * Note that the output will contain the length of the original input at
 * the start (a requirement for UncompressData function below).
 */
void CompressData(DataBuffer& out, const DataBuffer& input, int level);

/**
 * Uses ZLIB to uncompress the input buffer and return allocated output.
 *
 * Assumes that the compressed buffer contains the length of the original
 * input at the start in order to correctly allocate the size of the output.
 */
void UncompressData(DataBuffer& out, const DataBuffer& input);

/**
 * Compresses the input and then performs base64 encoding to convert
 * the result to a string.
 */
std::string CompressDataAndEncode(const DataBuffer& in, int level);

/**
 * Performs base64 decoding on the input string, and then uncompresses
 * the resulting data to create the output.
 */
void DecodeTextAndUncompress(DataBuffer& out, const std::string& input);

/**
 * Compresses a text string using ZLIB and then base64 encodes the result.
 */
SPI_UTIL_IMPORT
std::string CompressText(const std::string& str);

/**
 * Performs base64 decoding on the string and then uncompresses the
 * resulting data.
 *
 * The string provided is modified to reduce copying of strings.
 *
 * If the output is not a null-terminated string then an exception is thrown.
 */
SPI_UTIL_IMPORT
void UncompressText(std::string& str);

/*
 * Functions for base64 encoding and decoding are based on b64.c by Bob Trower 2001/08/04
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

/// <summary>
/// Converts a binary string to a text string using Base64 method.
/// Will expand the string by around 35%.
/// </summary>
/// <param name="bytes"></param>
/// <returns></returns>
SPI_UTIL_IMPORT
std::string Base64Encode(const std::string& bytes);

/// <summary>
/// Backs out the original binary string from the encoded bytes.
/// </summary>
/// <param name="encodedBytes"></param>
/// <returns></returns>
SPI_UTIL_IMPORT
std::string Base64Decode(const std::string& encodedBytes);

SPI_UTIL_END_NAMESPACE

#endif


