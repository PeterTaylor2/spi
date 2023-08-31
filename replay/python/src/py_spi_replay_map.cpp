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

/**
****************************************************************************
* Source file: py_spi_replay_map.cpp
****************************************************************************
*/

#include "py_spi_replay_map.hpp"
#include "pyd_replay.hpp"

#include <spi/python/pyUtil.hpp>
#include <spi/python/pyInput.hpp>
#include <spi/python/pyObject.hpp>
#include <spi/python/pyObjectMap.hpp>
#include <spi/python/pyOutput.hpp>
#include <spi/python/pyService.hpp>
#include <spi/python/pyValue.hpp>

#include "spi_replay_map.hpp"

#ifdef PYTHON_HAS_FASTCALL
PyObject* py_spi_replay_MapClassName(PyObject* self, PyObject* const* args, Py_ssize_t nargs, PyObject* kwargs)
#else
PyObject* py_spi_replay_MapClassName(PyObject* self, PyObject* args, PyObject* kwargs)
#endif
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("MapClassName");

#ifdef PYTHON_HAS_FASTCALL
        const spi::InputValues& iv = spi::pyGetInputValues(func, args, nargs, kwargs);
#else
        const spi::InputValues& iv = spi::pyGetInputValues(func, args, kwargs);
#endif
        spi::Value output = spi::CallInContext(func, iv, get_input_context());
        return spi::pyoFromValue(output);
    }
    catch (spi::PyException&)
    {
        return NULL;
    }
    catch (std::exception &e)
    {
        return spi::pyExceptionHandler(e.what());
    }
    catch (...)
    {
        return spi::pyExceptionHandler("Unknown exception");
    }
}

#ifdef PYTHON_HAS_FASTCALL
PyObject* py_spi_replay_MapFieldNames(PyObject* self, PyObject* const* args, Py_ssize_t nargs, PyObject* kwargs)
#else
PyObject* py_spi_replay_MapFieldNames(PyObject* self, PyObject* args, PyObject* kwargs)
#endif
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("MapFieldNames");

#ifdef PYTHON_HAS_FASTCALL
        const spi::InputValues& iv = spi::pyGetInputValues(func, args, nargs, kwargs);
#else
        const spi::InputValues& iv = spi::pyGetInputValues(func, args, kwargs);
#endif
        spi::Value output = spi::CallInContext(func, iv, get_input_context());
        return spi::pyoFromValue(output);
    }
    catch (spi::PyException&)
    {
        return NULL;
    }
    catch (std::exception &e)
    {
        return spi::pyExceptionHandler(e.what());
    }
    catch (...)
    {
        return spi::pyExceptionHandler("Unknown exception");
    }
}

#ifdef PYTHON_HAS_FASTCALL
PyObject* py_spi_replay_MapRef(PyObject* self, PyObject* const* args, Py_ssize_t nargs, PyObject* kwargs)
#else
PyObject* py_spi_replay_MapRef(PyObject* self, PyObject* args, PyObject* kwargs)
#endif
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("MapRef");

#ifdef PYTHON_HAS_FASTCALL
        const spi::InputValues& iv = spi::pyGetInputValues(func, args, nargs, kwargs);
#else
        const spi::InputValues& iv = spi::pyGetInputValues(func, args, kwargs);
#endif
        spi::Value output = spi::CallInContext(func, iv, get_input_context());
        return spi::pyoFromValue(output);
    }
    catch (spi::PyException&)
    {
        return NULL;
    }
    catch (std::exception &e)
    {
        return spi::pyExceptionHandler(e.what());
    }
    catch (...)
    {
        return spi::pyExceptionHandler("Unknown exception");
    }
}

#ifdef PYTHON_HAS_FASTCALL
PyObject* py_spi_replay_MapGetValue(PyObject* self, PyObject* const* args, Py_ssize_t nargs, PyObject* kwargs)
#else
PyObject* py_spi_replay_MapGetValue(PyObject* self, PyObject* args, PyObject* kwargs)
#endif
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("MapGetValue");

#ifdef PYTHON_HAS_FASTCALL
        const spi::InputValues& iv = spi::pyGetInputValues(func, args, nargs, kwargs);
#else
        const spi::InputValues& iv = spi::pyGetInputValues(func, args, kwargs);
#endif
        spi::Value output = spi::CallInContext(func, iv, get_input_context());
        return spi::pyoFromMultiValue(output, 2);
    }
    catch (spi::PyException&)
    {
        return NULL;
    }
    catch (std::exception &e)
    {
        return spi::pyExceptionHandler(e.what());
    }
    catch (...)
    {
        return spi::pyExceptionHandler("Unknown exception");
    }
}

void py_spi_replay_map_update_functions(spi::PythonService* svc)
{
    svc->SetNamespace("");

    /* MapClassName */
    svc->AddFunction("MapClassName",
        (PyCFunction)py_spi_replay_MapClassName,
        "MapClassName(m)",
#ifdef PYTHON_HAS_FASTCALL
        METH_FASTCALL | METH_KEYWORDS); 
#else
        METH_VARARGS | METH_KEYWORDS); 
#endif

    /* MapFieldNames */
    svc->AddFunction("MapFieldNames",
        (PyCFunction)py_spi_replay_MapFieldNames,
        "MapFieldNames(m)",
#ifdef PYTHON_HAS_FASTCALL
        METH_FASTCALL | METH_KEYWORDS); 
#else
        METH_VARARGS | METH_KEYWORDS); 
#endif

    /* MapRef */
    svc->AddFunction("MapRef",
        (PyCFunction)py_spi_replay_MapRef,
        "MapRef(m)",
#ifdef PYTHON_HAS_FASTCALL
        METH_FASTCALL | METH_KEYWORDS); 
#else
        METH_VARARGS | METH_KEYWORDS); 
#endif

    /* MapGetValue */
    svc->AddFunction("MapGetValue",
        (PyCFunction)py_spi_replay_MapGetValue,
        "MapGetValue(m,name)\n\nReturns the value and the value type for a name in a Map.",
#ifdef PYTHON_HAS_FASTCALL
        METH_FASTCALL | METH_KEYWORDS); 
#else
        METH_VARARGS | METH_KEYWORDS); 
#endif
}

