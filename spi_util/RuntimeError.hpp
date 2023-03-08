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
** RuntimeError.hpp
***************************************************************************
** Defines the RuntimeError functions which can be used for convenience
** to throw std::runtime_error.
**
** We can use either printf-style formatting via the RuntimeError functions.
** Or else c++ streams via the SPI_UTIL_THROW_RUNTIME_ERROR macro.
***************************************************************************
*/

#ifndef SPI_UTIL_RUNTIME_ERROR_HPP
#define SPI_UTIL_RUNTIME_ERROR_HPP

#include "DeclSpec.h"
#include "Namespace.hpp"
#include <stdexcept>
#include <sstream>
#include <stdarg.h>

SPI_UTIL_NAMESPACE

SPI_UTIL_IMPORT
std::runtime_error RuntimeErrorV(const char* format, va_list args);

SPI_UTIL_IMPORT
std::runtime_error RuntimeError(const char* format, ...);

SPI_UTIL_IMPORT
std::runtime_error RuntimeError(std::exception&e, const char* routine);

SPI_UTIL_END_NAMESPACE

#if defined(_MSC_VER) && (_MSC_VER <= 1500)
#define __func__ __FUNCTION__
#endif

#define SPI_UTIL_THROW_RUNTIME_ERROR(msg) { \
std::ostringstream oss_;\
oss_ << __func__ << ": " << msg;\
throw std::runtime_error(oss_.str());\
}

#define SPI_UTIL_NOT_IMPLEMENTED SPI_UTIL_THROW_RUNTIME_ERROR("Not implemented")

#undef SPI_UTIL_PRE_CONDITION
#define SPI_UTIL_PRE_CONDITION(cond) do { if(!(cond)) {\
SPI_UTIL_THROW_RUNTIME_ERROR("Required pre-condition (" << #cond << ") fails");\
}} while (0)

#undef SPI_UTIL_POST_CONDITION
#define SPI_UTIL_POST_CONDITION(cond) do { if(!(cond)) {\
SPI_UTIL_THROW_RUNTIME_ERROR("Required post-condition (" << #cond << ") fails");\
}} while (0)

#endif /* SPI_UTIL_RUNTIME_ERROR_HPP */

