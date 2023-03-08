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

#ifndef SPI_C_HELPER_HPP_
#define SPI_C_HELPER_HPP_

/**
****************************************************************************
* Header file: Helper.hpp
*
* Private C++ classes and functions for spcs DLL
****************************************************************************
*/

#include "c_spi.hpp"
#include <spi_util/StringUtil.hpp>

#ifdef _MSC_VER

#define g_malloc CoTaskMemAlloc
#define g_free CoTaskMemFree

#include <combaseapi.h>

#else

#define g_malloc malloc
#define g_free free

/* Allegedly the above are defined in <glib.h> but I can't find it */

#endif

#define to_size_t(i) spi_util::IntegerCast<size_t>(i)
#define to_int(i) spi_util::IntegerCast<int>(i)


#endif

