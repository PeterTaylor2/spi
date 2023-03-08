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
#ifndef SPI_XL_INPUT_HPP
#define SPI_XL_INPUT_HPP

/**
 * Functions to convert from Excel inputs (XLOPER) to the fundamental types
 * of the SPI library.
 */

#include "DeclSpec.h"

#include <string>

#include <spi/Namespace.hpp>

typedef struct xloper XLOPER;

#if SPI_XL_VERSION >= 12
typedef struct xloper12 XLOPER12;
#endif

SPI_BEGIN_NAMESPACE

class Value;

class SPI_XL_IMPORT ExcelInputError
{
public:
    ExcelInputError();
    const char* what() const;
};

bool xloperToBool(
    XLOPER* oper,
    const char* name,
    bool optional=false,
    bool defaultValue=false);

std::string xloperToString(
    XLOPER* oper,
    const char* name,
    bool optional=false,
    const char* defaultValue="");

#if SPI_XL_VERSION >= 12

std::string xloper12ToString(
    XLOPER12* oper,
    const char* name,
    bool optional = false,
    const char* defaultValue = "");

#endif

SPI_END_NAMESPACE

#endif // SPI_XL_INPUT_HPP

