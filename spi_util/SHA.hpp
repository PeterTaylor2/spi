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
#ifndef SPI_UTIL_SHA_HPP
#define SPI_UTIL_SHA_HPP

/**
 * Defines common interface to the SHA classes
 */

#include "DeclSpec.h"
#include "Namespace.hpp"
#include <string>

#ifdef _MSC_VER
#if _MSC_VER < 1600
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#endif
#endif

SPI_UTIL_NAMESPACE

class SPI_UTIL_IMPORT SHA
{
public:
    virtual ~SHA();

    // Hash in binary data and strings
    virtual void Update(const unsigned char* pbData, uint32_t uLen) = 0;

    // Finalize hash and return hexadecimal hash string
    virtual std::string Final() = 0;

};

std::string SHADigest2Hex(const unsigned char* digest, size_t len);

SPI_UTIL_END_NAMESPACE

#endif // SHA1_H_A545E61D43E9404E8D736869AB3CBFE7
