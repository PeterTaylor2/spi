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
** Or else c++ streams via the SPI_THROW_RUNTIME_ERROR macro.
***************************************************************************
*/

#ifndef SPI_RUNTIME_ERROR_HPP
#define SPI_RUNTIME_ERROR_HPP

#include "DeclSpec.h"
#include "Namespace.hpp"
#include <stdexcept>
#include <sstream>
#include <stdarg.h>

#undef SPI_PRE_CONDITION
#define SPI_PRE_CONDITION(cond) do { if(!(cond)) {\
throw spi::RuntimeError("%s: Required pre-condition (%s) fails", __FUNCTION__, #cond);\
}} while (0)

#undef SPI_POST_CONDITION
#define SPI_POST_CONDITION(cond) do { if(!(cond)) {\
throw spi::RuntimeError("%s: Required post-condition (%s) fails", __FUNCTION__, #cond);\
}} while (0)

SPI_BEGIN_NAMESPACE

SPI_IMPORT
std::runtime_error RuntimeError(const char* format, ...);

SPI_IMPORT
std::runtime_error RuntimeError(std::exception&e, const char* routine);

SPI_END_NAMESPACE

#define SPI_THROW_RUNTIME_ERROR(msg) { \
std::ostringstream oss_;\
oss_ << __FUNCTION__ << ": " << msg;\
throw std::runtime_error(oss_.str());\
}

#define SPI_NOT_IMPLEMENTED throw spi::RuntimeError("%s: Not implemented", __FUNCTION__)

#endif /* SPI_RUNTIME_ERROR_HPP */

