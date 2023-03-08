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
** pyUtil.cpp
**
** General purpose Python utility functions.
***************************************************************************
*/

#include "pyUtil.hpp"
#include "pyObject.h"
#include "pyInput.hpp"

SPI_BEGIN_NAMESPACE

/**
 * Shares a newly allocated PyObject but does not increase the reference
 * count.
 *
 * The idea is that you call this function after you have created PyObject,
 * and if you need to return it from the function as PyObject* then you
 * call the related function PyObjectRelease.
 */
PyObjectSP pyoShare(PyObject* pyo, bool add_ref)
{
    if (!pyo)
        return PyObjectSP();

    return PyObjectSP(pyo, add_ref);
}

/**
 * Returns the underlying PyObject from the shared pointer, and increases the
 * reference count of the PyObject.
 *
 * The PyObject remains inside the PyObjectSP, and when that goes out of
 * scope the PyObject is therefore not automatically deleted.
 *
 * See also PyObjectShare.
 */
PyObject* pyoRelease(const PyObjectSP& pyosp)
{
    PyObject* pyo = pyosp.get();
    Py_XINCREF(pyo);
    return pyo;
}

/**
 * When a C++ exception is detected, call this function to exit the
 * function at the top-level of the Python/C++ interface. For usage see
 * also PyException class.
 */
PyObject* pyExceptionHandler(const char* errmsg)
{
    PyErr_SetString(PyExc_Exception, (char*)errmsg);
    return NULL;
}

/**
* When a Python exception is detected, call this function to raise
* a C++ exception.
*/
void pyPythonExceptionHandler()
{
    PyObject* py_exc;
    PyObject* py_val;
    PyObject* py_tb;
    PyErr_Fetch(&py_exc, &py_val, &py_tb);
    PyErr_NormalizeException(&py_exc, &py_val, &py_tb);

    PyObject* py_err = PyObject_Str(py_val);

    Py_XDECREF(py_exc);
    Py_XDECREF(py_val);
    Py_XDECREF(py_tb);

    std::string err = pyoToString(py_err);

    Py_XDECREF(py_err);

    throw std::runtime_error(err);
}

/**
 * Converts a tuple object to a vector of objects.
 *
 * The outputs remain owned by the tuple, and hence you should not increment
 * or decrement the reference count of the outputs.
 *
 * If the input is not a tuple, then returns a vector of size one containing
 * the input value.
 */
std::vector<PyObject*> pyTupleToVector(PyObject* in)
{
    std::vector<PyObject*> out;
    if (PyTuple_Check(in))
    {
        Py_ssize_t size = PyTuple_Size(in);
        for (Py_ssize_t i = 0; i < size; ++i)
            out.push_back(PyTuple_GetItem(in, i));
    }
    else
    {
        out.push_back(in);
    }
    return out;
}

PyInterpreterLock::PyInterpreterLock()
{
    gstate = PyGILState_Ensure();
}

PyInterpreterLock::~PyInterpreterLock()
{
    PyGILState_Release(gstate);
}

SPI_END_NAMESPACE
