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
** DeclSpec.h
**
** Defines various declaration specifications. Windows specific.
**
** SPI_PY_IMPORT is set for Microsoft compilations. If SPI_PY_EXPORT is defined
** then it defines SPI_PY_IMPORT as dllexport otherwise as dllimport.
**
** Thus when building define -DSPI_PY_EXPORT. When consuming no definitions
** are needed.
***************************************************************************
*/

#ifndef SPI_PY_DECLSPEC_H
#define SPI_PY_DECLSPEC_H

#include <spi_util/ImportExport.h>

#ifdef SPI_PY_EXPORT
#define SPI_PY_IMPORT SPI_UTIL_DECLSPEC_EXPORT
#else
#define SPI_PY_IMPORT SPI_UTIL_DECLSPEC_IMPORT
#endif

#endif


