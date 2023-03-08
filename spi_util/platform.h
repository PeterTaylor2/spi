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
** platform.h
***************************************************************************
** Some platform-specific definitions. This file should only depend upon
** macros defined by the compiler vendors.
***************************************************************************
*/

#ifndef SPI_UTIL_PLATFORM_H
#define SPI_UTIL_PLATFORM_H

#ifdef __GNUC__

/* gcc */

#include <inttypes.h>

#endif

#ifdef _MSC_VER

/* microsoft */

#if _MSC_VER <= 1600

/* Visual Studio 2010 or earlier */

typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;

#else

#include <inttypes.h>

#endif


#endif

#endif
