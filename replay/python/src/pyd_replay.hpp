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

#ifndef _PYD_REPLAY_HPP_
#define _PYD_REPLAY_HPP_

/**
****************************************************************************
* Header file: pyd_replay.hpp
****************************************************************************
*/

/*
****************************************************************************
** pyd_replay.hpp
**
** Top level PYD functions for replay
****************************************************************************
*/

#include "pyd_replay_decl_spec.h"
#include <spi/Namespace.hpp>

#include "Python.h"

SPI_BEGIN_NAMESPACE
class PythonService;
struct FunctionCaller;
class InputContext;
SPI_END_NAMESPACE

spi::FunctionCaller* get_function_caller(const char* name);
spi::InputContext* get_input_context();
spi::PythonService* get_python_service();

extern "C"
{

/*
***************************************************************************
** Function called by Python to initialise the shared library.
***************************************************************************
*/
PY_REPLAY_IMPORT
#if PY_MAJOR_VERSION >= 3
PyObject* PyInit_py_replay(void);
#else
void initpy_replay(void);
#endif

} /* end of extern "C" */

#endif /* _PYD_REPLAY_HPP_*/

