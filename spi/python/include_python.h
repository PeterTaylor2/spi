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
** include_python.h
**
** In case we need some common definitions before including the python
** header file we isolate the include of Python.h via this single header
***************************************************************************
*/

#ifndef SPI_INCLUDE_PYTHON_H
#define SPI_INCLUDE_PYTHON_H

#include "Python.h"


#if PY_MAJOR_VERSION < 3

#undef PYTHON_HAS_FASTCALL

#elif PY_MAJOR_VERSION > 3

#define PYTHON_HAS_FASTCALL

#else

/* this will be major version 3 */

#if PY_MINOR_VERSION >= 7
#define PYTHON_HAS_FASTCALL
#else
#undef PYTHON_HAS_FASTCALL
#endif

#endif

#endif /* SPI_INCLUDE_PYTHON_H */

