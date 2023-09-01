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
** pyValue.hpp
**
** Mapping between spi::Value and PyObject
**
** Note that all functions are in the spi namespace, but also use the
** prefix py (lower case). This is to distinguish them from the Python
** C-interface which uses the prefix Py (mixed case). Otherwise there
** might be name clashes which would require us to use :: before functions
** from the Python C-interface.
***************************************************************************
*/

#ifndef SPI_PYVALUE_HPP
#define SPI_PYVALUE_HPP

#include "Python.h"

#include "pyUtil.hpp"
#include <spi/Value.hpp>
#include <spi/InputValues.hpp>

#include <exception>
#include <string>
#include <vector>

SPI_BEGIN_NAMESPACE

SPI_PY_IMPORT
PyObject* pyoFromValue(const Value& value);

SPI_PY_IMPORT
PyObject* pyoFromMultiValue(const Value& value, size_t numVars);

SPI_PY_IMPORT
Value pyoToValue(PyObject* pyo);

SPI_PY_IMPORT
InputValues pyGetInputValues(
    FunctionCaller* func,
    PyObject* args,
    PyObject* kwargs=0, // needed for functions which can take named arguments
    PyObject* self=0); // needed for class methods

SPI_PY_IMPORT
InputValues pyGetInputValues(
    FunctionCaller* func,
    PyObject* const* args,
    Py_ssize_t nargs,
    PyObject* kwargs = 0, // needed for functions which can take named arguments
    PyObject* self = 0); // needed for class methods

SPI_PY_IMPORT
std::vector<Value> pyTupleToValueVector(
    const char* name,
    size_t nbArgs,
    PyObject* args);

SPI_PY_IMPORT
std::vector<Value> pyArrayToValueVector(
    const char* name,
    size_t nbArgs,
    PyObject* const* args,
    Py_ssize_t nargs);

// to be used as part of the grand scheme of allowing Python to sub-class
// certain object types
//
// each method in the class will translate its inputs to a vector of values
// and then call this function to convert these into PyTuple which we can
// then pass into the PyCFunction once we have extracted it from the the
// PyObject
SPI_PY_IMPORT
PyObjectSP pyTupleFromValueVector(
    const std::vector<Value>& values);

SPI_END_NAMESPACE

#endif
