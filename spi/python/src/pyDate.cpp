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

#include "../pyDate.hpp"
#include "../pyUtil.hpp"
#include "pyVersion.hpp"

// extra python headers
#include <datetime.h>

#include <spi/RuntimeError.hpp>
#include <spi/Map.hpp>

#ifdef Py_LIMITED_API

/* Define structure for C API. */

// note that DateTime is not part of the Py_LIMITED_API but in practice
// is considered to be fairly stable - we may need to revisit this in
// future if this status changes - hence we use the same name as the
// structure in the regular API so that if there is a change in status
// of the Py_LIMITED_API in the future we can catch the problem

typedef struct {
    /* type objects */
    PyTypeObject* DateType;
    PyTypeObject* DateTimeType;
    PyTypeObject* TimeType;
    PyTypeObject* DeltaType;
    PyTypeObject* TZInfoType;

    /* singletons */
    PyObject* TimeZone_UTC;

    /* constructors */
    PyObject* (*Date_FromDate)(int, int, int, PyTypeObject*);
    PyObject* (*DateTime_FromDateAndTime)(int, int, int, int, int, int, int,
        PyObject*, PyTypeObject*);
    PyObject* (*Time_FromTime)(int, int, int, int, PyObject*, PyTypeObject*);
    PyObject* (*Delta_FromDelta)(int, int, int, int, PyTypeObject*);
    PyObject* (*TimeZone_FromTimeZone)(PyObject* offset, PyObject* name);

    /* constructors for the DB API */
    PyObject* (*DateTime_FromTimestamp)(PyObject*, PyObject*, PyObject*);
    PyObject* (*Date_FromTimestamp)(PyObject*, PyObject*);

    /* PEP 495 constructors */
    PyObject* (*DateTime_FromDateAndTimeAndFold)(int, int, int, int, int, int, int,
        PyObject*, int, PyTypeObject*);
    PyObject* (*Time_FromTimeAndFold)(int, int, int, int, PyObject*, int, PyTypeObject*);

} PyDateTime_CAPI;

#define PyDateTime_CAPSULE_NAME "datetime.datetime_CAPI"

/* Define global variable for the C API and a macro for setting it. */

static PyDateTime_CAPI* PyDateTimeAPI = NULL;
#define PyDateTime_IMPORT \
    PyDateTimeAPI = (PyDateTime_CAPI *)PyCapsule_Import(PyDateTime_CAPSULE_NAME, 0)

#define PyDate_FromDate(year, month, day) \
    PyDateTimeAPI->Date_FromDate((year), (month), (day), PyDateTimeAPI->DateType)

#define PyDateTime_FromDateAndTime(year, month, day, hour, min, sec, usec) \
    PyDateTimeAPI->DateTime_FromDateAndTime((year), (month), (day), (hour), \
        (min), (sec), (usec), Py_None, PyDateTimeAPI->DateTimeType)

#define PyDate_Check(op) PyObject_TypeCheck((op), PyDateTimeAPI->DateType)

#define PyDateTime_Check(op) PyObject_TypeCheck((op), PyDateTimeAPI->DateTimeType)

#endif

SPI_BEGIN_NAMESPACE

namespace
{

#ifdef Py_LIMITED_API
    int pyoGetIntAttribute(PyObject* pyo, const char* attr)
    {
        PyObject* v = PyObject_GetAttrString(pyo, attr);
        int result = (int)PyLong_AsLong(v);
        PYO_DECREF(v);
        return result;
    }
#endif

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

    int pyDateTime_GetYear(PyObject* pyo)
    {
#ifdef Py_LIMITED_API
        return pyoGetIntAttribute(pyo, "year");
#else
        return PyDateTime_GET_YEAR(pyo);
#endif
    }

    int pyDateTime_GetMonth(PyObject* pyo)
    {
#ifdef Py_LIMITED_API
        return pyoGetIntAttribute(pyo, "month");
#else
        return PyDateTime_GET_MONTH(pyo);
#endif
    }

    int pyDateTime_GetDay(PyObject* pyo)
    {
#ifdef Py_LIMITED_API
        return pyoGetIntAttribute(pyo, "day");
#else
        return PyDateTime_GET_DAY(pyo);
#endif
    }

    int pyDateTime_GetSecond(PyObject* pyo)
    {
#ifdef Py_LIMITED_API
        return pyoGetIntAttribute(pyo, "second");
#else
        return PyDateTime_DATE_GET_SECOND(pyo);
#endif
    }

    int pyDateTime_GetMinute(PyObject* pyo)
    {
#ifdef Py_LIMITED_API
        return pyoGetIntAttribute(pyo, "minute");
#else
        return PyDateTime_DATE_GET_MINUTE(pyo);
#endif
    }

    int pyDateTime_GetHour(PyObject* pyo)
    {
#ifdef Py_LIMITED_API
        return pyoGetIntAttribute(pyo, "hour");
#else
        return PyDateTime_DATE_GET_HOUR(pyo);
#endif
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
#ifdef Py_LIMITED_API
        static PyObject* name = PyUnicode_InternFromString("toordinal");   // cached
        PyObjectSP r = pyoShare(PyObject_CallMethodObjArgs(pyo, name, nullptr));
        if (!r)
            throw PyException();
        long ord = PyLong_AsLong(r.get());
        if (ord == -1 && PyErr_Occurred())
            throw PyException();
        return Date(ord - 584389);
#else
        int year  = pyDateTime_GetYear(pyo);
        int month = pyDateTime_GetMonth(pyo);
        int day   = pyDateTime_GetDay(pyo);

        return Date(year, month, day);
#endif
    }

    throw RuntimeError("%s: Input is not a date", __FUNCTION__);
}

DateTime pyToDateTime(PyObject* pyo)
{
    if (pyIsDateTime(pyo))
    {
        int year  = pyDateTime_GetYear(pyo);
        int month = pyDateTime_GetMonth(pyo);
        int day   = pyDateTime_GetDay(pyo);

        Date date(year, month, day);

        int hours   = pyDateTime_GetHour(pyo);
        int minutes = pyDateTime_GetMinute(pyo);
        int seconds = pyDateTime_GetSecond(pyo);

        return DateTime(date, hours, minutes, seconds);
    }

    throw RuntimeError("%s: Input is not a dateTime", __FUNCTION__);
}

SPI_END_NAMESPACE

