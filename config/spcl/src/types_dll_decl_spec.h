/*

    Sartorial Programming Interface (SPI) code generators
    Copyright (C) 2012-2023 Sartorial Programming Ltd.

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

#ifndef _TYPES_DLL_DECL_SPEC_H_
#define _TYPES_DLL_DECL_SPEC_H_


/*
****************************************************************************
** types_dll_decl_spec.h
**
** Defines import/export declaration specifications.
**
****************************************************************************
*/

/*
****************************************************************************
* Generated code - do not edit
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

#endif /* _TYPES_DLL_DECL_SPEC_H_ */

