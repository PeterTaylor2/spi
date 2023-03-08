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
/*
***************************************************************************
** RuntimeError.cpp
***************************************************************************
*/

#include "RuntimeError.hpp"

#include "StringUtil.hpp"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>

SPI_UTIL_NAMESPACE

std::runtime_error RuntimeErrorV(const char* format, va_list args)
{
    std::string error = StringFormatV(format, args);
    return std::runtime_error(error);
}

std::runtime_error RuntimeError(const char* format, ...)
{
    va_list args;
    va_start (args, format);

    std::runtime_error e = RuntimeErrorV(format, args);

    va_end (args);

    return e;
}

std::runtime_error RuntimeError(std::exception& e, const char* routine)
{
    std::ostringstream oss;
    oss << e.what() << "\n" << routine << ": Failed";
    return std::runtime_error(oss.str());
}

SPI_UTIL_END_NAMESPACE
