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

SPI_UTIL_END_NAMESPACE

#endif


