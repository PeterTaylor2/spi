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
**
** At present this is empty, but we will doubtless find things to add as
** we go along!
***************************************************************************
*/

#ifndef SPI_PLATFORM_H
#define SPI_PLATFORM_H

#include <spi_util/platform.h>

#ifdef __GNUC__

/* gcc */

#define GCC_VERSION (__GNUC__*10000 + __GNUC_MINOR__*100 + __GNUC_PATCHLEVEL__)

#else

#define GCC_VERSION 0

#endif

#ifdef _MSC_VER

/* microsoft */

// removes compiler warning about non-dll exported classes inside
// other classes where some of the functions might be inlined
// tried to fix the problem where it came up, but failed - so for
// the moment we will suppress the warning
#pragma warning(disable : 4251)

#define strdup _strdup
#define strcasecmp _strcmpi

#if _MSC_VER <= 1600

/* Visual Studio 2010 or earlier */

#define isnan _isnan
#define isinf !_finite

#endif

#endif

#endif
