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
* Source file: pyd_spdoc.cpp
****************************************************************************
*/

#include "pyd_spdoc.hpp"

#include "py_spdoc_publicType.hpp"
#include "py_spdoc_constant.hpp"
#include "py_spdoc_configTypes.hpp"

#include <spi/CommonRuntime.hpp>
#include <spi/ObjectHandle.hpp>
#include <spi/python/pyService.hpp>
#include <spi/python/pyUtil.hpp>
#include <spi/python/pyObject.h>

#include "spdoc_dll_service.hpp"

static spi_boost::shared_ptr<spi::PythonService> theService;

spi::FunctionCaller* get_function_caller(const char* name)
{
    return theService->GetFunctionCaller(name);
}

spi::InputContext* get_input_context()
{
    return theService->GetInputContext();
}

spi::PythonService* get_python_service()
{
    return theService.get();
}

extern "C"
{

PyObject* py_spdoc_help_func(
    PyObject* self, PyObject* args)
{
    return theService->HelpFunc(args);
}

PyObject* py_spdoc_help_enum(
    PyObject* self, PyObject* args)
{
    return theService->HelpEnum(args);
}

PyObject* py_spdoc_object_coerce(
    PyObject* self, PyObject* args)
{
    return theService->ObjectCoerce(args);
}

PyObject* py_spdoc_start_logging(
    PyObject* self, PyObject* args)
{
    return theService->StartLogging(args);
}

PyObject* py_spdoc_stop_logging(
    PyObject*self, PyObject* args)
{
    return theService->StopLogging();
}

} /* end of extern "C" */

/*
***************************************************************************
** Standard Python initialisation function.
***************************************************************************
*/
#if PY_MAJOR_VERSION >= 3
PyObject* PyInit_py_spdoc(void)
#else
void initpy_spdoc(void)
#endif
{
  try {
    spi::CommonRuntime::AddContext("PYTHON");
    theService.reset(new spi::PythonService("py_spdoc", spdoc::spdoc_exported_service()));
    spi::PythonService* svc = theService.get();

    /* function registration */
    /* help_func */
    svc->AddFunction("help_func", py_spdoc_help_func,
        "help_func(funcName?)\n\n"
        "Lists functions or function arguments.");

    /* help_enum */
    svc->AddFunction("help_enum", py_spdoc_help_enum,
        "help_enum(enumName?)\n\n"
        "Find enumerated types or enumerands.");

    /* object_from_string */
    svc->AddFunction("object_from_string", spi_py_object_from_string,
        "object_from_string(str)\n\n"
        "Create object from string - identical to Object.from_string.\n"
        "Needs to be a top-level function for pickle/unpickle purposes.");

    /* object_get */
    svc->AddFunction("object_get", spi_py_object_get,
        "object_get(obj,name?)\n\n"
        "Gets a value by name from an object.");

    /* object_put */
    svc->AddFunction("object_put", spi_py_object_put,
        "object_put(obj,names[],value,...)\n\n"
        "Sets a value by name within an object.");

    /* start_logging */
    svc->AddFunction("start_logging", py_spdoc_start_logging,
        "start_logging(filename,options?)\n\n"
        "Start function logging - returns filename.");

    /* stop_logging */
    svc->AddFunction("stop_logging", py_spdoc_stop_logging,
        "stop_logging()\n\nStops function logging"
        " - returns whether logging was on previously.");

    py_spdoc_publicType_update_functions(svc);
    py_spdoc_constant_update_functions(svc);
    py_spdoc_configTypes_update_functions(svc);

    svc->RegisterFunctions();
#if PY_MAJOR_VERSION >= 3
    PyObject* module = svc->GetPythonModule();
    return module;
#endif
  }
  catch (std::exception& e)
  {
    spi::pyExceptionHandler(e.what());
#if PY_MAJOR_VERSION >= 3
    return NULL;
#endif
  }
  catch (...)
  {
    spi::pyExceptionHandler("Unknown exception");
#if PY_MAJOR_VERSION >= 3
    return NULL;
#endif
  }
}


