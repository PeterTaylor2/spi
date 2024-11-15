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

#include "Error.h"

#include "Helper.hpp"
#include "CString.h"

#include <stdarg.h>
#include <spi_util/StringUtil.hpp>

static std::string g_last_error;

/*
**************************************************************************
* Implementation of spi_Error functions
**************************************************************************
*/
void spi_Error_set(const char* err)
{
    if (err)
        g_last_error = err;
    else
        g_last_error.clear();
}

void spi_Error_set_function(const char* func, const char* err)
{
    if (func && err)
    {
        spi_Error_set_format("%s: %s", func, err);
    }
    else
    {
        g_last_error.clear();
    }
}

void spi_Error_set_format(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    std::string result = spi_util::StringFormatV(format, args);

    va_end(args);

    g_last_error = result;
}

char* spi_Error_get()
{
    return spi_String_copy(g_last_error.c_str());
}

