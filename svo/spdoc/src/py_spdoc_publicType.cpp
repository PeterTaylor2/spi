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
* Source file: py_spdoc_publicType.cpp
****************************************************************************
*/

#include "py_spdoc_publicType.hpp"
#include "pyd_spdoc.hpp"

#include <spi/python/pyUtil.hpp>
#include <spi/python/pyInput.hpp>
#include <spi/python/pyObject.hpp>
#include <spi/python/pyObjectMap.hpp>
#include <spi/python/pyOutput.hpp>
#include <spi/python/pyService.hpp>
#include <spi/python/pyValue.hpp>

#include "spdoc_publicType.hpp"

PyObject* py_spdoc_verifyPrimitiveTypeName(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("verifyPrimitiveTypeName");

        const spi::InputValues& iv = spi::pyGetInputValues(func, args, kwargs);
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

void py_spdoc_publicType_update_functions(spi::PythonService* svc)
{
    svc->SetNamespace("");

    /* verifyPrimitiveTypeName */
    svc->AddFunction("verifyPrimitiveTypeName",
        (PyCFunction)py_spdoc_verifyPrimitiveTypeName,
        "verifyPrimitiveTypeName(typeName)\n\nGiven a c++ typename, this function will verify that this is a valid primitive type, and return the corresponding PublicType value.",
        METH_VARARGS | METH_KEYWORDS); 
}

