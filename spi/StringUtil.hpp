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
** StringUtil.hpp
***************************************************************************
** String utility functions.
***************************************************************************
*/

#ifndef SPI_STRING_UTIL_HPP
#define SPI_STRING_UTIL_HPP

#include "Namespace.hpp"

#include "../spi_util/StringUtil.hpp"

SPI_BEGIN_NAMESPACE

/*
 * We don't redeclare or redirect all the useful functions from spi_util.
 *
 * Instead we rename them so that you can call spi::StringFormat (for
 * example) and you will actually be calling spi_util::StringFormat (in
 * that case).
 */

using spi_util::StringFormatV;
using spi_util::StringFormat;
using spi_util::StringMapFormat;
using spi_util::StringParser;
using spi_util::StringSplit;
using spi_util::StringJoin;
using spi_util::StringReplace;
using spi_util::StringStrip;
using spi_util::StringUpper;
using spi_util::StringLower;
using spi_util::StringCap;
using spi_util::StringToLong;
using spi_util::StringToInt;
using spi_util::StringToDouble;
using spi_util::StringEscape;
using spi_util::StringCapture;
using spi_util::StringStartsWith;
using spi_util::StringEndsWith;
using spi_util::StringFromBytes;
using spi_util::StringToBytes;

using spi_util::CStringPreProcess;

SPI_END_NAMESPACE

#endif


