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
#include "SHA.hpp"

#include "RuntimeError.hpp"

SPI_UTIL_NAMESPACE

SHA::~SHA()
{}

std::string SHADigest2Hex(const unsigned char* digest, size_t len)
{
    SPI_UTIL_PRE_CONDITION(len <= 32);

    char hex[65];
    for (size_t i = 0; i < len; ++i)
    {
        sprintf(&hex[2 * i], "%02X", digest[i]);
    }
    hex[2*len] = 0;

    return std::string(&hex[0]);
}

SPI_UTIL_END_NAMESPACE

