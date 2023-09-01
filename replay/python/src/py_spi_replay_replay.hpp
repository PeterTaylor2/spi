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

#ifndef _PY_SPI_REPLAY_REPLAY_HPP_
#define _PY_SPI_REPLAY_REPLAY_HPP_

/**
****************************************************************************
* Header file: py_spi_replay_replay.hpp
****************************************************************************
*/

#include "pyd_replay_decl_spec.h"
#include <spi/Namespace.hpp>

#include "Python.h"
#include <vector>

SPI_BEGIN_NAMESPACE
class PythonService;
SPI_END_NAMESPACE

void py_spi_replay_replay_update_functions(spi::PythonService* svc);

extern "C"
{

PyObject* py_spi_replay_ReplayAction_Coerce(PyObject* self, PyObject* args);

PyObject* py_spi_replay_ReplayFunctionAction_Coerce(PyObject* self, PyObject* args);

PyObject* py_spi_replay_ReplayObjectAction_Coerce(PyObject* self, PyObject* args);

PyObject* py_spi_replay_ReplayCodeGenerator_Coerce(PyObject* self, PyObject* args);

PyObject* py_spi_replay_ReplayCodeGenerator_GenerateFunction(PyObject* self, PyObject* args, PyObject* kwargs);

PyObject* py_spi_replay_ReplayCodeGenerator_GenerateObject(PyObject* self, PyObject* args, PyObject* kwargs);

PyObject* py_spi_replay_ReplayLog_Coerce(PyObject* self, PyObject* args);

PyObject* py_spi_replay_ReplayLog_generateCode(PyObject* self, PyObject* args, PyObject* kwargs);

PyObject* py_spi_replay_ReplayLog_Read(PyObject* self, PyObject* args, PyObject* kwargs);

} /* end of extern "C" */

#endif /* _PY_SPI_REPLAY_REPLAY_HPP_*/

