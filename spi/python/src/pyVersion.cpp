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

#include "pyVersion.hpp"

#include "../pyUtil.hpp"
#include "../pyInput.hpp"

#include <spi/Map.hpp>

/*
***************************************************************************
** pyVersion.cpp
**
** Compatibility layer for Python2 and Python3 and the Py_LIMITED_API
***************************************************************************
*/

SPI_BEGIN_NAMESPACE

std::string pyo_typename(PyObject* pyo)
{
#ifdef Py_LIMITED_API
    return pyType_GetName(Py_TYPE(pyo));
#else
    return pyType_GetName(pyo->ob_type);
#endif
}

std::string pyType_GetName(PyTypeObject* pyType)
{
#ifdef Py_LIMITED_API
    PyObject* pyName = PyType_GetName(pyType);   // new reference, holds a str
    if (!pyName)
        throw PyException();

    std::string name = pyoToString(pyName);

    PYO_DECREF(pyName);

    return name;
#else
    return std::string(pyType->tp_name);
#endif
}

double pyFloat_AsDouble(PyObject* pyo)
{
    double d = PyFloat_AsDouble(pyo);
    if (d == -1.0 && PyErr_Occurred())
    {
        throw PyException();
    }
    return d;
}

SPI_END_NAMESPACE
