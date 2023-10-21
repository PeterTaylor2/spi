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
** StreamUtil.hpp
***************************************************************************
** Stream utility functions.
***************************************************************************
*/

#ifndef SPI_UTIL_STREAM_UTIL_HPP
#define SPI_UTIL_STREAM_UTIL_HPP

#include "DeclSpec.h"
#include "Namespace.hpp"

#include <vector>
#include <string>
#include <iostream>

SPI_UTIL_NAMESPACE

SPI_UTIL_IMPORT
std::vector<std::string> StreamReadLines(std::istream& str);

SPI_UTIL_IMPORT
std::string StreamReadContents(std::istream& str);

SPI_UTIL_END_NAMESPACE

#endif

