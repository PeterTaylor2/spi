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
** pyInput.cpp
**
** Functions for converting from PyObject to C++ data types.
***************************************************************************
*/

#include <spi/python/pyInput.hpp>
#include <spi/python/pyUtil.hpp>
#include <spi/python/pyDate.hpp>
#include <spi/python/pyObject.h>
#include <spi/python/pyService.hpp>
#include <spi/python/pyObject.hpp>

#include <spi_util/Utils.hpp>
#include <spi/StringUtil.hpp>
#include <spi/RuntimeError.hpp>

#include "py2to3.hpp"

SPI_BEGIN_NAMESPACE

using spi_util::IntegerCast;

/**
 * Converts from PyObject to vector.
 */
template <typename T>
std::vector<T> pyoToVector(
    PyObject* pyo,
    T (*ToScalar)(PyObject*))
{
    PyObjectSP pyoArray = pyoShare(
        PySequence_Fast(pyo, "Not a sequence"));
    if (!pyoArray)
        throw PyException();
    Py_ssize_t size = PySequence_Fast_GET_SIZE(pyoArray.get());
    std::vector<T> out;
    for (Py_ssize_t i = 0; i < size; ++i)
    {
        PyObject* item = PySequence_Fast_GET_ITEM(pyoArray.get(), i);
        out.push_back(ToScalar(item));
    }
    return out;
}

template <typename T>
std::vector<T> pyoToVector(
    PyObject* pyo,
    ObjectType* ot,
    T (*ToScalar)(PyObject*, ObjectType*))
{
    PyObjectSP pyoArray = pyoShare(
        PySequence_Fast(pyo, "Not a sequence"));
    if (!pyoArray)
        throw PyException();
    Py_ssize_t size = PySequence_Fast_GET_SIZE(pyoArray.get());
    std::vector<T> out;
    for (Py_ssize_t i = 0; i < size; ++i)
    {
        PyObject* item = PySequence_Fast_GET_ITEM(pyoArray.get(), i);
        out.push_back(ToScalar(item, ot));
    }
    return out;
}

/**
 * Converts from PyObject to bool.
 *
 * Accepts False, True and integers.
 */
bool pyoToBool(PyObject* pyo)
{
    if (pyo == Py_False)
        return false;
    if (pyo == Py_True)
        return true;
    if (PyInt_Check(pyo))
        return (PyInt_AS_LONG(pyo)) != 0;

    PyErr_Format(PyExc_TypeError, "Cannot convert %s to bool",
                 pyo->ob_type->tp_name);
    throw PyException();
}

std::vector<bool> pyoToBoolVector(PyObject* pyo)
{
    return pyoToVector<bool>(pyo, pyoToBool);
}

/**
 * Converts from PyObject to int.
 *
 * We do the equivalent of the Python int function, and then validate that
 * resulting integer is a valid C int (since Python integers are represented
 * by long in C).
 */
int pyoToInt(PyObject* pyo)
{
    PyObjectSP int_pyo = pyoShare(PyNumber_Int(pyo));

    if (!int_pyo)
        throw PyException();

    return IntegerCast<int>(PyInt_AS_LONG(int_pyo.get()));
}

std::vector<int> pyoToIntVector(PyObject* pyo)
{
    return pyoToVector<int>(pyo, pyoToInt);
}

/**
 * Converts from PyObject to long.
 *
 * We do the equivalent of the Python int function. The resulting integer
 * will be a long and is returned.
 */
long pyoToLong(PyObject* pyo)
{
    PyObjectSP int_pyo = pyoShare(PyNumber_Int(pyo));

    if (!int_pyo)
        throw PyException();

    return PyInt_AS_LONG(int_pyo.get());
}

std::vector<long> pyoToLongVector(PyObject* pyo)
{
    return pyoToVector<long>(pyo, pyoToLong);
}

/**
 * Converts from PyObject to size_t.
 *
 * We do the equivalent of the Python int function, and then validate that
 * resulting integer is positive.
 */
size_t pyoToSizeT(PyObject* pyo)
{
    PyObjectSP int_pyo = pyoShare(PyNumber_Int(pyo));

    if (!int_pyo)
        throw PyException();

    return IntegerCast<size_t>(PyInt_AS_LONG(int_pyo.get()));
}

std::vector<size_t> pyoToSizeTVector(PyObject* pyo)
{
    return pyoToVector<size_t>(pyo, pyoToSizeT);
}

/**
 * Converts from PyObject to double.
 *
 * We do the equivalent of the Python float function. Since Python represents
 * float as double in C, the resulting value is simply returned.
 */
double pyoToDouble(PyObject* pyo)
{
    PyObjectSP float_pyo = pyoShare(PyNumber_Float(pyo));

    if (!float_pyo)
        throw PyException();

    return PyFloat_AS_DOUBLE(float_pyo.get());
}

std::vector<double> pyoToDoubleVector(PyObject* pyo)
{
    return pyoToVector<double>(pyo, pyoToDouble);
}


// checks whether an input object is a string
bool pyoIsString(PyObject* pyo)
{
#if PY_MAJOR_VERSION < 3
    if (PyString_Check(pyo))
        return true;
    return false;
#else
    if (PyUnicode_Check(pyo) && PyUnicode_IS_READY(pyo))
    {
        int kind = PyUnicode_KIND(pyo);
        return kind == PyUnicode_1BYTE_KIND;
    }
    return false;
#endif
}


/**
 * Converts from PyObject to std::string.
 *
 * The input must be a Python string.
 */
std::string pyoToString(PyObject* pyo)
{
#if PY_MAJOR_VERSION < 3
    char* str = PyString_AsString(pyo);
    // documentation of PyString_AsString tells us that we must not de-allocate the
    // output returned - if it is NULL then a TypeError will have been raised
    if (!str)
        throw PyException();

    return std::string(str);
#else
    if (!pyoIsString(pyo))
    {
        PyErr_SetNone(PyExc_TypeError);
        throw PyException();
    }

    // pyoIsString guarantees that PyUnicode_DATA returns 1-byte array
    void* str = PyUnicode_DATA(pyo);
    if (!str)
        throw PyException();

    return std::string((const char*)str);
#endif
}

std::vector<std::string> pyoToStringVector(PyObject* pyo)
{
    return pyoToVector<std::string>(pyo, pyoToString);
}

/**
 * Converts from PyObject to char.
 *
 * The input must be a Python string of length 1.
 */

char pyoToChar(PyObject* pyo)
{
    std::string str = pyoToString(pyo);
    if (str.length() != 1)
    {
        PyErr_Format(PyExc_TypeError, "%s: '%s' must be of size 1",
            __FUNCTION__, str.c_str());
    }

    return str[0];
}

std::vector<char> pyoToCharVector(PyObject* pyo)
{
    return pyoToVector<char>(pyo, pyoToChar);
}

/**
 * Converts from PyObject to Date.
 *
 * Accepts datetime.date as input, and also integers of the format YYYYMMDD.
 */
Date pyoToDate(PyObject* pyo)
{
    if (pyIsDate(pyo))
    {
        return pyToDate(pyo);
    }

    if (PyInt_Check(pyo))
    {
        long ymd = PyInt_AS_LONG(pyo);

        long year  = ymd / 10000;
        long month = ymd / 100 % 100;
        long day   = ymd % 100;

        return Date(year, month, day);
    }

    PyErr_Format(PyExc_TypeError, "%s: Cannot convert %s to Date",
                 __FUNCTION__, pyo->ob_type->tp_name);
    throw PyException();
}

std::vector<Date> pyoToDateVector(PyObject* pyo)
{
    return pyoToVector<Date>(pyo, pyoToDate);
}

/**
 * Converts from PyObject to ObjectSP.
 *
 * We provide the object type so that we can validate that we have the correct
 * class, and in the future to provide object coercion.
 *
 * Returns None as an empty object.
 */
ObjectConstSP pyoToObject(PyObject* pyo, ObjectType* ot)
{
    if (pyo == Py_None)
        return ObjectConstSP();

    if (PyObject_TypeCheck(pyo, SpiPyObjectType()))
    {
        ObjectConstSP obj = spiPyObjectGetObject((SpiPyObject*)pyo);
 
        if (!ot)
        {
            // no type checking required
            return obj;
        }
        if (ot->is_instance(obj))
        {
            return obj;
        }
    }

    const char* srcName = pyo->ob_type->tp_name;
    const char* dstName = ot ? ot->get_class_name() : "Object";
    PyErr_Format(PyExc_TypeError, "Cannot convert %s to %s",
                 srcName, dstName);
    throw PyException();
}

std::vector<ObjectConstSP> pyoToObjectVector(PyObject* pyo, ObjectType* ot)
{
    return pyoToVector<ObjectConstSP>(pyo, ot, pyoToObject);
}

SPI_END_NAMESPACE
