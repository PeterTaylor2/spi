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
#include "xlOutput.hpp"
#include "xlValue.hpp"
#include "xlUtil.hpp"

#include <spi/ObjectHandle.hpp>
#include <spi/StringUtil.hpp>

#include "xlcall.h"

SPI_BEGIN_NAMESPACE

XLOPER* xloperOutput(XLOPER* xlo)
{
    if (!xlo)
        return xloperError(xlerrNull);

    xlo->xltype |= xlbitDLLFree;
    return xlo;
}

XLOPER* xloperInFunctionWizard()
{
    // for the moment we return #N/A
    return xloperError(xlerrNA);
}

XLOPER* xloperInputError()
{
    return xloperError(xlerrNA);
}

XLOPER* xloperError(int errorCode)
{
    static XLOPER xlo;

    xlo.xltype  = xltypeErr;
    xlo.val.err = errorCode;

    return &xlo;
}

XLOPER* xloperFromBool(bool value)
{
    return xloperMakeFromValue(Value(value));
}

SPI_END_NAMESPACE
