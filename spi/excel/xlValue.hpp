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
#ifndef SPI_XL_VALUE_HPP
#define SPI_XL_VALUE_HPP

/**
***************************************************************************
** spi/excel/xlValue.hpp
***************************************************************************
** Functions to convert from XLOPER to/from spi::Value.
**
** XLOPER is rather limited in its data representations - everything is
** a number or a string or an array. Hence we need to use another
** module to convert from an Excel defined spi::Value to basic data types.
**
** However going first to spi::Value will enable us to handle coercion,
** and make decisions on the data type later rather than immediately.
***************************************************************************
*/

#include "DeclSpec.h"

#include <spi/Value.hpp>
#include <spi/InputValues.hpp>

typedef struct xloper XLOPER;

#if SPI_XL_VERSION >= 12
typedef struct xloper12 XLOPER12;
#endif

SPI_BEGIN_NAMESPACE

struct XLInputValues
{
    XLInputValues(const char* name);

    InputValues iv;
    Value baseName;
    bool noCallInWizard;
};

/**
 * Convert xloper to Value. Returns ExcelInputError if the input XLOPER
 * is an error type. This is a sub-class of std::exception, but you can
 * catch it independently (recommended) and return #N/A without reporting
 * an error.
 */
SPI_XL_IMPORT /* only needed for export by excel-test executables */
spi::Value xloperToValue(XLOPER* oper);

#if SPI_XL_VERSION >= 12
SPI_XL_IMPORT /* only needed for export by excel-test executables */
spi::Value xloper12ToValue(XLOPER12* oper);
#endif

/**
 * Creates an XLOPER from a Value.
 */
SPI_XL_IMPORT
XLOPER* xloperMakeFromValue(
    const spi::Value& in,
    bool expandArrays = false,
    size_t numVars = 1,
    const spi::Value& baseNameValue = spi::Value(),
    bool mandatoryBaseName = false,
    const std::string& baseNamePrefix = std::string(),
    bool fillBlank = false);

/**
 * Set the value in an xloper to the given Value.
 */
void xloperSetFromValue(
    XLOPER* oper,
    const spi::Value& in,
    bool expandArrays,
    const std::string& baseName = std::string(),
    int baseNameIndex = 0,
    bool mandatoryBaseName = false,
    bool fillBlank = false);

// returns input values and object base name(s)
//
// baseNamePos = -1 if last input is the base name
// baseNamePos = 0 if there are no objects returned
// baseNamePos = specific for a specific base name position (start at 1)
SPI_XL_IMPORT
XLInputValues xlGetInputValues(
    FunctionCaller* func,
    int baseNamePos,
    ...);

SPI_END_NAMESPACE

#endif // SPI_XL_VALUE_HPP

