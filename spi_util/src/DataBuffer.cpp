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

#include "DataBuffer.hpp"

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

SPI_UTIL_END_NAMESPACE
