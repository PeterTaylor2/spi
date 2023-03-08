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

#ifndef _SPDOC_DLL_DECL_SPEC_H_
#define _SPDOC_DLL_DECL_SPEC_H_


/*
****************************************************************************
** spdoc_dll_decl_spec.h
**
** Defines import/export declaration specifications.
**
****************************************************************************
*/

#ifdef SPI_STATIC
#define SPI_IMPORT
#else

#include <spi_util/ImportExport.h>

#ifdef SPI_EXPORT
#define SPI_IMPORT SPI_UTIL_DECLSPEC_EXPORT
#else
#define SPI_IMPORT SPI_UTIL_DECLSPEC_IMPORT
#endif

#endif

#endif /* _SPDOC_DLL_DECL_SPEC_H_ */

