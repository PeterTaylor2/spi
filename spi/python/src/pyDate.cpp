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
** pyDate.cpp
**
** Primitive Python date conversion routines.
**
** Only this module accesses the Python DateTime module.
***************************************************************************
*/

#include "pyDate.hpp"

// standard python headers
#include "datetime.h"

#include <spi/RuntimeError.hpp>

SPI_BEGIN_NAMESPACE

namespace
{

/**
 * Ensures that the datetime module gets imported.
 */
void pyImportDateTime(void)
{
    static bool imported = false;
    if (!imported)
    {
        PyDateTime_IMPORT;
        imported = true;
    }
}

} // end of anonymous namespace



bool pyIsDate(PyObject* pyo)
{
    pyImportDateTime();

    if (PyDate_Check(pyo))
        return true;

    return false;
}

bool pyIsDateTime(PyObject* pyo)
{
    pyImportDateTime();

    if (PyDateTime_Check(pyo))
        return true;

    return false;
}

PyObject* pyMakeDate(int year, int month, int day)
{
    pyImportDateTime();

    return PyDate_FromDate(year, month, day);
}

PyObject* pyMakeDateTime(int year, int month, int day,
    int hours, int minutes, int seconds)
{
    pyImportDateTime();

    return PyDateTime_FromDateAndTime(year, month, day,
        hours, minutes, seconds, 0);
}

Date pyToDate(PyObject* pyo)
{
    if (pyIsDate(pyo))
    {
        int year  = PyDateTime_GET_YEAR(pyo);
        int month = PyDateTime_GET_MONTH(pyo);
        int day   = PyDateTime_GET_DAY(pyo);

        return Date(year, month, day);
    }

    throw RuntimeError("%s: Input is not a date", __FUNCTION__);
}

DateTime pyToDateTime(PyObject* pyo)
{
    if (pyIsDateTime(pyo))
    {
        int year  = PyDateTime_GET_YEAR(pyo);
        int month = PyDateTime_GET_MONTH(pyo);
        int day   = PyDateTime_GET_DAY(pyo);

        Date date(year, month, day);

        int hours   = PyDateTime_DATE_GET_HOUR(pyo);
        int minutes = PyDateTime_DATE_GET_MINUTE(pyo);
        int seconds = PyDateTime_DATE_GET_SECOND(pyo);

        return DateTime(date, hours, minutes, seconds);
    }

    throw RuntimeError("%s: Input is not a dateTime", __FUNCTION__);
}

SPI_END_NAMESPACE

