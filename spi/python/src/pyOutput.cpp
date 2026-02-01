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
** pyOutput.cpp
**
** Functions for converting from C++ data types to PyObject.
***************************************************************************
*/

#include <spi/python/pyOutput.hpp>
#include <spi/python/pyDate.hpp>
#include <spi/python/pyObject.h>
#include <spi/python/pyValue.hpp>
#include <spi/python/pyService.hpp>

#include <spi_util/Utils.hpp>

#include <spi/Variant.hpp>
#include <spi/MapObject.hpp>
#include <spi/MatrixData.hpp>
#include "py2to3.hpp"

SPI_BEGIN_NAMESPACE

using spi_util::IntegerCast;

/**
 * Converts from bool to PyObject.
 *
 * Returns True or False with the reference count incremented.
 */
PyObject* pyoFromBool(bool value)
{
    PyObject* pyo = value ? Py_True : Py_False;

    Py_INCREF(pyo);
    return pyo;
}

PyObject* pyoFromBoolArray(const std::vector<bool>& values)
{
    return pyoMakeArray(values, pyoFromBool);
}

/**
 * Converts from int to PyObject.
 *
 * Returns an integer object with the reference count incremented.
 */
PyObject* pyoFromInt(int value)
{
    return PyInt_FromLong(value);
}

PyObject* pyoFromIntArray(const std::vector<int>& values)
{
    return pyoMakeArray(values, pyoFromInt);
}

/**
 * Converts from long to PyObject.
 *
 * Returns an integer object with the reference count incremented.
 */
PyObject* pyoFromLong(long value)
{
    return PyInt_FromLong(value);
}

PyObject* pyoFromLongArray(const std::vector<long>& values)
{
    return pyoMakeArray(values, PyInt_FromLong);
}

/**
 * Converts from size_t to PyObject.
 *
 * Validates that the value is in the range 0 to max signed long.
 * Returns an integer object with the reference count incremented.
 */
PyObject* pyoFromSizeT(size_t value)
{
    return PyInt_FromLong(IntegerCast<long>(value));
}

PyObject* pyoFromSizeTArray(const std::vector<size_t>& values)
{
    return pyoMakeArray(values, pyoFromSizeT);
}

/**
 * Converts from double to PyObject.
 *
 * Returns a float object with the reference count incremented.
 */
PyObject* pyoFromDouble(double value)
{
    return PyFloat_FromDouble(value);
}

PyObject* pyoFromDoubleArray(const std::vector<double>& values)
{
    return pyoMakeArray(values, PyFloat_FromDouble);
}

/**
 * Converts froms std::string to PyObject.
 *
 * Returns an (immutable) Python string object with the reference count
 * incremented.
 */
PyObject* pyoFromString(const std::string& str)
{
#if PY_MAJOR_VERSION < 3
    return PyString_FromString(str.c_str());
#else
    return PyUnicode_FromKindAndData(
        PyUnicode_1BYTE_KIND,
        str.c_str(),
        str.length());
#endif
}

PyObject* pyoFromStringArray(const std::vector<std::string>& values)
{
    return pyoMakeArray(values, pyoFromString);
}

PyObject* pyoFromBytes(const std::string& str)
{
    return PyBytes_FromStringAndSize(str.data(), str.length());
}

PyObject* pyoFromBytesArray(const std::vector<std::string>& values)
{
    return pyoMakeArray(values, pyoFromBytes);
}

/**
 * Converts from char to PyObject.
 *
 * Returns an (immutable) Python string object of length one with the reference
 * count incremented.
 */
PyObject* pyoFromChar(char value)
{
    char buf[2];
    buf[0] = value;
    buf[1] = '\0';

    return pyoFromString(std::string(buf));
}

PyObject* pyoFromCharArray(const std::vector<char>& values)
{
    return pyoMakeArray(values, pyoFromChar);
}

/**
 * Converts from Date to PyObject.
 *
 * Returns a Python datetime.date object with reference count incremented.
 */
PyObject* pyoFromDate(Date value)
{
    if (!value)
        Py_RETURN_NONE;

    int year;
    int month;
    int day;

    value.YMD(&year, &month, &day);

    return pyMakeDate(year, month, day);
}

PyObject* pyoFromDateArray(const std::vector<Date>& values)
{
    return pyoMakeArray(values, pyoFromDate);
}

/**
 * Converts from DateTime to PyObject.
 *
 * Returns a Python datetime.datetime object with reference count incremented.
 */
PyObject* pyoFromDateTime(DateTime value)
{
    int year, month, day;
    int hours, minutes, seconds;

    value.Date().YMD(&year, &month, &day);
    value.HMS(&hours, &minutes, &seconds);

    return pyMakeDateTime(year, month, day, hours, minutes, seconds);
}


/**
 * Converts from Map to PyObject.
 * This is for specific types of Map - namely Variant, Matrix etc.
 */
PyObject* pyoFromMap(const MapConstSP& m)
{
    if (!m)
        Py_RETURN_NONE;

    switch(m->MapType())
    {
    case Map::VARIANT:
    {
        Variant var(m);
        return pyoFromValue(var.GetValue());
    }
    case Map::MATRIX:
    {
        MatrixData<Value> matrix = MatrixData<Value>::FromMap(m);
        int rows = IntegerCast<int>(matrix.Rows());
        int cols = IntegerCast<int>(matrix.Cols());
        const std::vector<Value>& data = matrix.Data();
        size_t k = 0;

        PyObjectSP pyo = pyoShare(PyList_New(rows));
        if (!pyo)
            throw PyException();

        for (int i = 0; i < rows; ++i)
        {
            PyObjectSP pyc = pyoShare(PyList_New(cols));
            if (!pyc)
                throw PyException();
            for (int j = 0; j < cols; ++j)
            {
                PyList_SET_ITEM(pyc.get(), j, pyoFromValue(data[k]));
                ++k;
            }
            PyList_SET_ITEM(pyo.get(), i, pyoRelease(pyc));
        }
        return pyoRelease(pyo);
    }
    case Map::NAMED:
    {
        return PythonService::MakeObjectOfType(
            spi::Object::to_value(MapObject::Make(m))); 
    }
    }

    PyErr_SetString(PyExc_TypeError, "Cannot convert Map to PyObject");
    throw PyException();
}


/**
 * Converts from Object to PyObject.
 *
 * Returns an instance of SpiPyObject with reference count incremented.
 * SpiPyObject allows the underlying spi::Object to be changed.
 */
PyObject* pyoFromObject(const ObjectConstSP& value)
{
    if (!value)
        Py_RETURN_NONE;

    SpiPyObject* pyo = PyObject_New(SpiPyObject, SpiPyObjectType());
    if (!pyo)
    {
        PyErr_Format(PyExc_MemoryError, "Could not create Object.");
        throw PyException();
    }

    pyo->obj = value.get();
    pyo->obj->incRefCount();

    return (PyObject*)pyo;
}

PyObject* pyoFromObjectArray(const std::vector<ObjectConstSP>& values)
{
    return pyoMakeArray(values, pyoFromObject);
}

SPI_END_NAMESPACE
