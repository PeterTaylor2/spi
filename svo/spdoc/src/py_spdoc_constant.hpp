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

#ifndef _PY_SPDOC_CONSTANT_HPP_
#define _PY_SPDOC_CONSTANT_HPP_

/**
****************************************************************************
* Header file: py_spdoc_constant.hpp
****************************************************************************
*/

#include "pyd_spdoc_decl_spec.h"
#include <spi/Namespace.hpp>

#include "Python.h"
#include <vector>

SPI_BEGIN_NAMESPACE
class PythonService;
SPI_END_NAMESPACE

void py_spdoc_constant_update_functions(spi::PythonService* svc);

extern "C"
{

PyObject* py_spdoc_Constant_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_Constant_typeName(PyObject* self, PyObject* args, PyObject* kwargs);

PyObject* py_spdoc_Constant_docString(PyObject* self, PyObject* args, PyObject* kwargs);

PyObject* py_spdoc_Constant_getInt(PyObject* self, PyObject* args, PyObject* kwargs);

PyObject* py_spdoc_Constant_getDate(PyObject* self, PyObject* args, PyObject* kwargs);

PyObject* py_spdoc_Constant_getDateTime(PyObject* self, PyObject* args, PyObject* kwargs);

PyObject* py_spdoc_Constant_getDouble(PyObject* self, PyObject* args, PyObject* kwargs);

PyObject* py_spdoc_Constant_getChar(PyObject* self, PyObject* args, PyObject* kwargs);

PyObject* py_spdoc_Constant_getString(PyObject* self, PyObject* args, PyObject* kwargs);

PyObject* py_spdoc_Constant_getBool(PyObject* self, PyObject* args, PyObject* kwargs);

PyObject* py_spdoc_IntConstant_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_DateConstant_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_DateTimeConstant_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_DoubleConstant_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_CharConstant_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_StringConstant_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_BoolConstant_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_UndefinedConstant_Coerce(PyObject* self, PyObject* args);

} /* end of extern "C" */

#endif /* _PY_SPDOC_CONSTANT_HPP_*/

