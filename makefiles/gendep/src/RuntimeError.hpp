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

#ifndef SPI_RUNTIME_ERROR_HPP
#define SPI_RUNTIME_ERROR_HPP

/*
***************************************************************************
** RuntimeError.hpp
***************************************************************************
** Defines the RuntimeError functions which can be used for convenience
** to throw std::runtime_error.
***************************************************************************
*/

#include <stdexcept>
#include <sstream>
#include <stdarg.h>

#undef PRE_CONDITION
#define PRE_CONDITION(cond) do { if(!(cond)) {\
throw RuntimeError("%s: Required pre-condition (%s) fails", __FUNCTION__, #cond);\
}} while (0)

#undef POST_CONDITION
#define POST_CONDITION(cond) do { if(!(cond)) {\
throw RuntimeError("%s: Required post-condition (%s) fails", __FUNCTION__, #cond);\
}} while (0)

std::runtime_error RuntimeErrorV(const char* format, va_list args);

std::runtime_error RuntimeError(const char* format, ...);

#define NOT_IMPLEMENTED throw RuntimeError("%s: Not implemented", __FUNCTION__)

#endif /* SPI_RUNTIME_ERROR_HPP */

