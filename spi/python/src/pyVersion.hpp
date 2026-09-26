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
** pyVersion.hpp
**
** Compatibility layer for Python2 and Python3 and the Py_LIMITED_API
***************************************************************************
*/

#ifndef SPI_PY_VERSION_HPP
#define SPI_PY_VERSION_HPP

#include <spi/Namespace.hpp>

#include <Python.h>
#include <string>

#if PY_MAJOR_VERSION >= 3
#define PyInt_Check PyLong_Check
#define PyInt_AS_LONG PyLong_AS_LONG
#define PyInt_AsLong PyLong_AsLong
#define PyInt_FromLong PyLong_FromLong
#define PyNumber_Int PyNumber_Long
#endif

#ifdef Py_LIMITED_API

#define PyMethod_Check PyCallable_Check

#endif

SPI_BEGIN_NAMESPACE

std::string pyo_typename(PyObject* pyo);
std::string pyType_GetName(PyTypeObject* pyType);
std::string pyo_base_class_name(PyObject* pyo);

double pyFloat_AsDouble(PyObject* pyo);

template <typename T>
static inline void PYO_INCREF(T* op)
{
    Py_INCREF(reinterpret_cast<PyObject*>(op));
}

template <typename T>
static inline void PYO_DECREF(T* op)
{
    Py_DECREF(reinterpret_cast<PyObject*>(op));
}

SPI_END_NAMESPACE


#endif
