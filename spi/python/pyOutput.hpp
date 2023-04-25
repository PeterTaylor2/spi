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
#ifndef SPI_PYOUTPUT_HPP
#define SPI_PYOUTPUT_HPP

/*
***************************************************************************
** pyOutput.hpp
**
** Functions with convert from various standard and spi-specific data types
** to the Python data type (PyObject).
**
** Each function has a scalar and a vector equivalent.
**
** The vector version will use the template pyoMakeArray, but is
** provided as a specific function for convenience.
**
** Note that all functions are in the spi namespace, but also use the
** prefix py (lower case). This is to distinguish them from the Python
** C-interface which uses the prefix Py (mixed case). Otherwise there
** might be name clashes which would require us to use :: before functions
** from the Python C-interface.
***************************************************************************
*/

#include "pyUtil.hpp"

SPI_BEGIN_NAMESPACE

/**
 * Converts from bool to PyObject.
 *
 * Returns True or False with the reference count incremented.
 */
PyObject* pyoFromBool(bool value);
PyObject* pyoFromBoolArray(const std::vector<bool>& values);

/**
 * Converts from int to PyObject.
 *
 * Returns an integer object with the reference count incremented.
 */
PyObject* pyoFromInt(int value);
PyObject* pyoFromIntArray(const std::vector<int>& values);

/**
 * Converts from long to PyObject.
 *
 * Returns an integer object with the reference count incremented.
 */
PyObject* pyoFromLong(long value);
PyObject* pyoFromLongArray(const std::vector<long>& values);

/**
 * Converts from size_t to PyObject.
 *
 * Validates that the value is in the range 0 to max signed long.
 * Returns an integer object with the reference count incremented.
 */
PyObject* pyoFromSizeT(size_t value);
PyObject* pyoFromSizeTArray(const std::vector<size_t>& values);

/**
 * Converts from double to PyObject.
 *
 * Returns a float object with the reference count incremented.
 */
PyObject* pyoFromDouble(double value);
PyObject* pyoFromDoubleArray(const std::vector<double>& values);

/**
 * Converts froms std::string to PyObject.
 *
 * Returns an (immutable) Python string object with the reference count
 * incremented.
 */
PyObject* pyoFromString(const std::string& str);
PyObject* pyoFromStringArray(const std::vector<std::string>& values);

/**
 * Converts from char to PyObject.
 *
 * Returns an (immutable) Python string object of length one with the reference
 * count incremented.
 */
PyObject* pyoFromChar(char value);
PyObject* pyoFromCharArray(const std::vector<char>& values);

/**
 * Converts from Date to PyObject.
 *
 * Returns a Python datetime.date object with reference count incremented.
 */
PyObject* pyoFromDate(Date value);
PyObject* pyoFromDateArray(const std::vector<Date>& values);

/**
 * Converts from DateTime to PyObject.
 *
 * Returns a Python datetime.datetime object with reference count incremented.
 */
PyObject* pyoFromDateTime(DateTime value);
//PyObject* pyoFromDateArray(const std::vector<Date>& values);


/**
 * Converts from Map to PyObject.
 * This is for specific types of Map - namely Variant, Matrix etc.
 */
PyObject* pyoFromMap(const MapConstSP& m);

/**
 * Converts from Object to PyObject.
 *
 * Returns an instance of SpiPyObject with reference count incremented.
 */
PyObject* pyoFromObject(const ObjectConstSP& value);
PyObject* pyoFromObjectArray(const std::vector<ObjectConstSP>& values);

/**
 * Template for converting an enumerated type T to a PyObject.
 * At the Python level this will be represented by a string object.
 */
template<class T>
PyObject* pyoFromEnum(const T& value)
{
    return pyoFromString(std::string(value));
}

/**
 * Template for converting an enumerated type T to a PyObject.
 * At the Python level this will be represented by an array of string object.
 */
template<class T>
PyObject* pyoFromEnumArray(const std::vector<T>& values)
{
    std::vector<std::string> strings;
    for (size_t i = 0; i < values.size(); ++i)
        strings.push_back(std::string(values[i]));
    return pyoFromStringArray(strings);
}

/**
 * Given a function that converts a scalar instance of T to PyObject*, this
 * template will implement the vector equivalent where the scalar function
 * takes inputs by value rather than by reference.
 */
template<typename T>
PyObject* pyoMakeArray(
    const std::vector<T>& values,
    PyObject* (*MakeScalar)(T))
{
    int size = (int)values.size();
    PyObjectSP pyo = pyoShare(PyList_New((int)size));
    if (!pyo)
        throw PyException();
    for (int i = 0; i < size; ++i)
        PyList_SET_ITEM(pyo.get(), i, MakeScalar(values[i]));
    return pyoRelease(pyo);
}

/**
 * Given a function that converts a scalar instance of T to PyObject*, this
 * template will implement the vector equivalent where the scalar function
 * takes inputs by reference rather than by value.
 */
template<typename T>
PyObject* pyoMakeArray(
    const std::vector<T>& values,
    PyObject* (*MakeScalar)(const T&))
{
    int size = (int)values.size();
    PyObjectSP pyo = pyoShare(PyList_New((int)size));
    if (!pyo)
        throw PyException();
    for (int i = 0; i < size; ++i)
        PyList_SET_ITEM(pyo.get(), i, MakeScalar(values[i]));
    return pyoRelease(pyo);
}

/**
 * Given a function that converts a scalar instance of T to PyObject*, this
 * template will implement the matrix equivalent where the scalar function
 * takes inputs by value rather than by reference.
 */
template<typename T>
PyObject* pyoMakeMatrix(
    size_t nr,
    size_t nc,
    const std::vector<T>& values,
    PyObject* (*MakeScalar)(T))
{
    PyObjectSP pyo = pyoShare(PyList_New((int)nr));
    if (!pyo)
        throw PyException();
    size_t i = 0;
    for (size_t r = 0; r < nr; ++r)
    {
        PyObjectSP pyr = pyoShare(PyList_New((int)nc));
        for (size_t c = 0; c < nc; ++c)
        {
            PyList_SET_ITEM(pyr.get(), c, MakeScalar(values[i]));
            ++i;
        }
        PyList_SET_ITEM(pyo.get(), r, pyoRelease(pyr));
    }
    return pyoRelease(pyo);
}

/**
 * Given a function that converts a scalar instance of T to PyObject*, this
 * template will implement the matrix equivalent where the scalar function
 * takes inputs by reference rather than by value.
 */
template<typename T>
PyObject* pyoMakeMatrix(
    size_t nr,
    size_t nc,
    const std::vector<T>& values,
    PyObject* (*MakeScalar)(const T&))
{
    PyObjectSP pyo = pyoShare(PyList_New((int)nr));
    if (!pyo)
        throw PyException();
    size_t i = 0;
    for (size_t r = 0; r < nr; ++r)
    {
        PyObjectSP pyr = pyoShare(PyList_New((int)nc));
        for (size_t c = 0; c < nc; ++c)
        {
            PyList_SET_ITEM(pyr.get(), c, MakeScalar(values[i]));
            ++i;
        }
        PyList_SET_ITEM(pyo.get(), r, pyoRelease(pyr));
    }
    return pyoRelease(pyo);
}

template<class T>
PyObject* pyoFromInstanceVector(
    const std::vector< spi_boost::intrusive_ptr<T> >& values)
{
    std::vector<ObjectConstSP> tmp;
    tmp.reserve(values.size());
    for (size_t i = 0; i < values.size(); ++i)
        tmp.push_back(ObjectConstSP(values[i].get()));
    return pyoFromObjectArray(tmp);
}

SPI_END_NAMESPACE

#endif
