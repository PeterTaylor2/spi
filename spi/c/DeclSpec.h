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
** SPI_C_IMPORT is set for Microsoft compilations. If SPI_C_EXPORT is defined
** then it defines SPI_C_IMPORT as dllexport otherwise as dllimport.
**
** Thus when building define -DSPI_C_EXPORT. When consuming no definitions
** are needed.
***************************************************************************
*/

#ifndef SPI_C_DECLSPEC_H
#define SPI_C_DECLSPEC_H

#ifdef _MSC_VER

/* microsoft compiler */

#ifdef SPI_C_EXPORT
#define SPI_C_IMPORT __declspec(dllexport)
#else
#define SPI_C_IMPORT __declspec(dllimport)
#endif

#else

/* not using a microsoft compiler */
/* we should also implement something similar for modern gcc compilers */
#define SPI_C_IMPORT

#endif

#endif


