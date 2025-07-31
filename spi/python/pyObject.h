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
** pyObject.h
**
** Python object equivalent to spi::Object
***************************************************************************
*/

#ifndef SPI_PYOBJECT_H
#define SPI_PYOBJECT_H

#include "Python.h"

#ifndef __cplusplus

typedef struct _SpiObject SpiObject;

#else

#include "pyUtil.hpp"
typedef spi::Object SpiObject;

extern "C"
{
#endif

    typedef struct _SpiPyObject
    {
        PyObject_HEAD
        const SpiObject* obj;
    } SpiPyObject;

    SPI_PY_IMPORT
    void spi_py_object_dealloc (SpiPyObject *pyo);

    SPI_PY_IMPORT
    PyObject* spi_py_object_getter (SpiPyObject* pyo, char* name);

    SPI_PY_IMPORT
    int spi_py_object_setter (SpiPyObject* pyo, PyObject* value, char* name);

    SPI_PY_IMPORT
    int spi_py_object_map_setter(SpiPyObject* pyo, PyObject* value, char* name);

    SPI_PY_IMPORT
    PyObject* spi_py_object_getattro(SpiPyObject* pyo, PyObject* name);

    SPI_PY_IMPORT
    PyObject* spi_py_object_get(PyObject* self, PyObject* args);

    SPI_PY_IMPORT
    PyObject* spi_py_object_put(PyObject* self, PyObject* args);

    SPI_PY_IMPORT
    PyObject* spi_py_object_set_meta_data(SpiPyObject* self, PyObject* args);

    SPI_PY_IMPORT
    PyObject* spi_py_object_copy(PyObject* pyo);

    SPI_PY_IMPORT
    PyObject* spi_py_object_deepcopy(PyObject* pyo, PyObject* ignored);

    SPI_PY_IMPORT
    PyObject* spi_py_object_to_string (PyObject* pyo, PyObject* args);

    SPI_PY_IMPORT
    PyObject* spi_py_object_to_file (PyObject* pyo, PyObject* args);

    SPI_PY_IMPORT
    PyObject* spi_py_object_from_string (PyObject* self, PyObject* args);

    SPI_PY_IMPORT
    PyObject* spi_py_object_from_file (PyObject* self, PyObject* args);

    PyTypeObject* SpiPyObjectType();

#ifdef __cplusplus
}
#endif

#endif

