/*

    GENDEP.EXE - Generates header file dependencies for C++
    Copyright (C) 2013 Sartorial Programming Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

/*
***************************************************************************
** RuntimeError.cpp
***************************************************************************
** Defines the RuntimeError functions which can be used for convenience
** to throw std::runtime_error.
***************************************************************************
*/

#include "RuntimeError.hpp"
#include "StringUtil.hpp"

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

