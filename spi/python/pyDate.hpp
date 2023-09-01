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
** pyDate.hpp
**
** Primitive Python date conversion routines.
**
** Only this module accesses the Python DateTime module.
***************************************************************************
*/

#ifndef SPI_PYDATE_HPP
#define SPI_PYDATE_HPP

#include "Python.h"

#include <spi/Date.hpp>
#include <spi/DateTime.hpp>

SPI_BEGIN_NAMESPACE

bool pyIsDate(PyObject* pyo);
bool pyIsDateTime(PyObject* pyo);
PyObject* pyMakeDate(int year, int month, int day);
PyObject* pyMakeDateTime(int year, int month, int day,
    int hours, int minutes, int seconds);
Date pyToDate(PyObject* pyo);
DateTime pyToDateTime(PyObject* pyo);

SPI_END_NAMESPACE

#endif
