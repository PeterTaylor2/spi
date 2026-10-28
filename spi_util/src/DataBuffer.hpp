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

#ifndef SPI_UTIL_DATA_BUFFER_HPP
#define SPI_UTIL_DATA_BUFFER_HPP

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

SPI_UTIL_END_NAMESPACE

#endif


