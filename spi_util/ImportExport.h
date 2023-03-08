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
#ifndef SPI_UTIL_IMPORT_EXPORT_H
#define SPI_UTIL_IMPORT_EXPORT_H

/*
******************************************************************************
* defines SPI_UTIL_DECLSPEC_EXPORT and SPI_UTIL_DECLSPEC_IMPORT
* use SPI_UTIL_DECLSPEC_EXPORT when compiling a DLL
* use SPI_UTIL_DECLSPEC_IMPORT when using the header files
* control by a compiler variable specific to the DLL build
*
* e.g.
*
* #ifdef XXX_EXPORT
* #define XXX_IMPORT SPI_UTIL_DECLSPEC_EXPORT
* #else
* #define XXX_IMPORT SPI_UTIL_DECLSPEC_IMPORT
* #endif
*
* Annotate the header files with XXX_IMPORT for functions/classes you wish
* to expose from a shared library.
*
* Note that XXX_IMPORT appears in front function declaration.
* Note that XXX_IMPORT appears after the class/struct keyword.
*
* Note that for gcc/g++ compilations you need to use -fvisibility=hidden
* in the compiler flags to hide everything that isn't default.
*
* Otherwise you would have to go the other way around and create something
* called XXX_NO_IMPORT as well with __attribute__((visibility("hidden"))),
* but then you would have to remember to use that for Windows compilations
* as well.
******************************************************************************
*/

#ifdef _MSC_VER

#define SPI_UTIL_DECLSPEC_EXPORT __declspec(dllexport)
#define SPI_UTIL_DECLSPEC_IMPORT __declspec(dllimport)

// this is needed because MSVC emits warnings about use of STL-types from DLLs
// we believe it to be meaningless as long as you use the same STL throughout
#pragma warning(disable:4251)

#elif defined(__GNUC__)

#define SPI_UTIL_DECLSPEC_EXPORT __attribute__((visibility("default")))
#define SPI_UTIL_DECLSPEC_IMPORT

#else

#define SPI_UTIL_DECLSPEC_EXPORT
#define SPI_UTIL_DECLSPEC_IMPORT

#endif

#endif
