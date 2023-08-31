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

#ifndef _PY_SPDOC_PUBLICTYPE_HPP_
#define _PY_SPDOC_PUBLICTYPE_HPP_

/**
****************************************************************************
* Header file: py_spdoc_publicType.hpp
****************************************************************************
*/

#include "pyd_spdoc_decl_spec.h"
#include <spi/Namespace.hpp>

#include <spi/python/include_python.h>
#include <vector>

SPI_BEGIN_NAMESPACE
class PythonService;
SPI_END_NAMESPACE

void py_spdoc_publicType_update_functions(spi::PythonService* svc);

extern "C"
{

#ifdef PYTHON_HAS_FASTCALL
PyObject* py_spdoc_verifyPrimitiveTypeName(PyObject* self, PyObject* const* args, Py_ssize_t nargs, PyObject* kwargs);
#else
PyObject* py_spdoc_verifyPrimitiveTypeName(PyObject* self, PyObject* args, PyObject* kwargs);
#endif

} /* end of extern "C" */

#endif /* _PY_SPDOC_PUBLICTYPE_HPP_*/

