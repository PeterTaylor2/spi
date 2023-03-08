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
#include "xlInput.hpp"

#include <spi/RuntimeError.hpp>
#include <spi/InputContext.hpp>

#include "xlValue.hpp"

SPI_BEGIN_NAMESPACE

/*
***************************************************************************
** Implemention of ExcelInputError.
***************************************************************************
*/
ExcelInputError::ExcelInputError()
{
}

const char* ExcelInputError::what() const
{
    return "Excel input error";
}

bool xloperToBool(
    XLOPER* oper,
    const char* name,
    bool optional,
    bool defaultValue)
{
    static InputContext* context = InputContext::ExcelContext();
    Value value = xloperToValue(oper);

    try
    {
        return context->ValueToBool(value, optional, defaultValue);
    }
    catch (std::exception &e)
    {
        throw RuntimeError(e, name);
    }
}

std::string xloperToString(
    XLOPER* oper,
    const char* name,
    bool optional,
    const char* defaultValue)
{
    static InputContext* context = InputContext::ExcelContext();
    Value value = xloperToValue(oper);

    try
    {
        return context->ValueToString(value, optional, defaultValue);
    }
    catch (std::exception &e)
    {
        throw RuntimeError(e, name);
    }
}

#if SPI_XL_VERSION>=12

std::string xloper12ToString(
    XLOPER12 * oper,
    const char * name,
    bool optional,
    const char * defaultValue)
{
    static InputContext* context = InputContext::ExcelContext();
    Value value = xloper12ToValue(oper);

    try
    {
        return context->ValueToString(value, optional, defaultValue);
    }
    catch (std::exception &e)
    {
        throw RuntimeError(e, name);
    }
    return std::string();
}

#endif

SPI_END_NAMESPACE
