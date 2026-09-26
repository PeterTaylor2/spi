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
* Source file: py_spdoc_constant.cpp
****************************************************************************
*/

#include "py_spdoc_constant.hpp"
#include "pyd_spdoc.hpp"

#include <spi/python/pyUtil.hpp>
#include <spi/python/pyInput.hpp>
#include <spi/python/pyObject.hpp>
#include <spi/python/pyObjectMap.hpp>
#include <spi/python/pyOutput.hpp>
#include <spi/python/pyService.hpp>
#include <spi/python/pyValue.hpp>

#include "spdoc_constant.hpp"

static int py_spdoc_Constant_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    try
    {
        throw spi::RuntimeError("Cannot construct class of type %s", "Constant");
    }
    catch (spi::PyException&)
    {
        return -1;
    }
    catch (std::exception &e)
    {
        spi::pyExceptionHandler(e.what());
        return -1;
    }
    catch (...)
    {
        spi::pyExceptionHandler("Unknown exception");
        return -1;
    }
}

PyObject* py_spdoc_Constant_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("Constant", args);
    return pyo;
}

PyObject* py_spdoc_Constant_typeName(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "Constant.typeName");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        const spi::InputValues& iv = spi::pyGetInputValues(func, args, kwargs, self);
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

PyObject* py_spdoc_Constant_docString(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "Constant.docString");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        const spi::InputValues& iv = spi::pyGetInputValues(func, args, kwargs, self);
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

PyObject* py_spdoc_Constant_getInt(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "Constant.getInt");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        const spi::InputValues& iv = spi::pyGetInputValues(func, args, kwargs, self);
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

PyObject* py_spdoc_Constant_getDate(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "Constant.getDate");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        const spi::InputValues& iv = spi::pyGetInputValues(func, args, kwargs, self);
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

PyObject* py_spdoc_Constant_getDateTime(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "Constant.getDateTime");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        const spi::InputValues& iv = spi::pyGetInputValues(func, args, kwargs, self);
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

PyObject* py_spdoc_Constant_getDouble(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "Constant.getDouble");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        const spi::InputValues& iv = spi::pyGetInputValues(func, args, kwargs, self);
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

PyObject* py_spdoc_Constant_getChar(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "Constant.getChar");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        const spi::InputValues& iv = spi::pyGetInputValues(func, args, kwargs, self);
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

PyObject* py_spdoc_Constant_getString(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "Constant.getString");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        const spi::InputValues& iv = spi::pyGetInputValues(func, args, kwargs, self);
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

PyObject* py_spdoc_Constant_getBool(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "Constant.getBool");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        const spi::InputValues& iv = spi::pyGetInputValues(func, args, kwargs, self);
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

static PyTypeObject* Constant_PyObjectType()
{
    static PyTypeObject* typeObject = NULL;
    if (typeObject)
        return typeObject;

    static PyMethodDef methods[] =
    {
        {"Coerce", (PyCFunction)py_spdoc_Constant_Coerce, METH_VARARGS | METH_STATIC,
            "Coerce Constant from arbitrary value"},
        {"typeName", (PyCFunction)py_spdoc_Constant_typeName, METH_VARARGS | METH_KEYWORDS,
            "typeName(self)\n\nreturns the data type name for the scalar"},
        {"docString", (PyCFunction)py_spdoc_Constant_docString, METH_VARARGS | METH_KEYWORDS,
            "docString(self)\n\nreturns the string which should appear in documentation"},
        {"getInt", (PyCFunction)py_spdoc_Constant_getInt, METH_VARARGS | METH_KEYWORDS,
            "getInt(self)\n\nreturns the integer value (where applicable) for the scalar"},
        {"getDate", (PyCFunction)py_spdoc_Constant_getDate, METH_VARARGS | METH_KEYWORDS,
            "getDate(self)\n\nreturns the date value (where applicable) for the scalar"},
        {"getDateTime", (PyCFunction)py_spdoc_Constant_getDateTime, METH_VARARGS | METH_KEYWORDS,
            "getDateTime(self)\n\nreturns the date time value (where applicable) for the scalar"},
        {"getDouble", (PyCFunction)py_spdoc_Constant_getDouble, METH_VARARGS | METH_KEYWORDS,
            "getDouble(self)\n\nreturns the double value (where applicable) for the scalar"},
        {"getChar", (PyCFunction)py_spdoc_Constant_getChar, METH_VARARGS | METH_KEYWORDS,
            "getChar(self)\n\nreturns the char value (where applicable) for the scalar"},
        {"getString", (PyCFunction)py_spdoc_Constant_getString, METH_VARARGS | METH_KEYWORDS,
            "getString(self)\n\nreturns the string value (where applicable) for the scalar"},
        {"getBool", (PyCFunction)py_spdoc_Constant_getBool, METH_VARARGS | METH_KEYWORDS,
            "getBool(self)\n\nreturns the bool value (where applicable) for the scalar"},
        {NULL, NULL, 0, NULL} // sentinel
    };

    typeObject = spi::pyMakeTypeObject(
        "spdoc.Constant",
        0,
        methods,
        true,
        "Interface class defining a constant scalar value.",
        nullptr,
        Py_tp_init, (void*)py_spdoc_Constant_init,
        0);

    return typeObject;
}


static int py_spdoc_IntConstant_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "IntConstant");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::IntConstant::object_type);
        return 0;
    }
    catch (spi::PyException&)
    {
        return -1;
    }
    catch (std::exception &e)
    {
        spi::pyExceptionHandler(e.what());
        return -1;
    }
    catch (...)
    {
        spi::pyExceptionHandler("Unknown exception");
        return -1;
    }
}

PyObject* py_spdoc_IntConstant_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("IntConstant", args);
    return pyo;
}

static PyTypeObject* IntConstant_PyObjectType()
{
    static PyTypeObject* typeObject = NULL;
    if (typeObject)
        return typeObject;

    static PyGetSetDef properties[] =
    {
        {"value", (getter)(spi_py_object_getter), NULL,
            "integer value",
            (void*) "value"},
        {NULL} // sentinel
    };

    static PyMethodDef methods[] =
    {
        {"Coerce", (PyCFunction)py_spdoc_IntConstant_Coerce, METH_VARARGS | METH_STATIC,
            "Coerce IntConstant from arbitrary value"},
        {NULL, NULL, 0, NULL} // sentinel
    };

    typeObject = spi::pyMakeTypeObject(
        "spdoc.IntConstant",
        properties,
        methods,
        false,
        "Integer constant defined in the configuration file.\n\n__init__(self, value)",
        "Constant",
        Py_tp_init, (void*)py_spdoc_IntConstant_init,
        0);

    return typeObject;
}


static int py_spdoc_DateConstant_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "DateConstant");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::DateConstant::object_type);
        return 0;
    }
    catch (spi::PyException&)
    {
        return -1;
    }
    catch (std::exception &e)
    {
        spi::pyExceptionHandler(e.what());
        return -1;
    }
    catch (...)
    {
        spi::pyExceptionHandler("Unknown exception");
        return -1;
    }
}

PyObject* py_spdoc_DateConstant_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("DateConstant", args);
    return pyo;
}

static PyTypeObject* DateConstant_PyObjectType()
{
    static PyTypeObject* typeObject = NULL;
    if (typeObject)
        return typeObject;

    static PyGetSetDef properties[] =
    {
        {"value", (getter)(spi_py_object_getter), NULL,
            "date value",
            (void*) "value"},
        {NULL} // sentinel
    };

    static PyMethodDef methods[] =
    {
        {"Coerce", (PyCFunction)py_spdoc_DateConstant_Coerce, METH_VARARGS | METH_STATIC,
            "Coerce DateConstant from arbitrary value"},
        {NULL, NULL, 0, NULL} // sentinel
    };

    typeObject = spi::pyMakeTypeObject(
        "spdoc.DateConstant",
        properties,
        methods,
        false,
        "Date constant defined in the configuration file.\n\n__init__(self, value)",
        "Constant",
        Py_tp_init, (void*)py_spdoc_DateConstant_init,
        0);

    return typeObject;
}


static int py_spdoc_DateTimeConstant_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "DateTimeConstant");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::DateTimeConstant::object_type);
        return 0;
    }
    catch (spi::PyException&)
    {
        return -1;
    }
    catch (std::exception &e)
    {
        spi::pyExceptionHandler(e.what());
        return -1;
    }
    catch (...)
    {
        spi::pyExceptionHandler("Unknown exception");
        return -1;
    }
}

PyObject* py_spdoc_DateTimeConstant_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("DateTimeConstant", args);
    return pyo;
}

static PyTypeObject* DateTimeConstant_PyObjectType()
{
    static PyTypeObject* typeObject = NULL;
    if (typeObject)
        return typeObject;

    static PyGetSetDef properties[] =
    {
        {"value", (getter)(spi_py_object_getter), NULL,
            "datetime value",
            (void*) "value"},
        {NULL} // sentinel
    };

    static PyMethodDef methods[] =
    {
        {"Coerce", (PyCFunction)py_spdoc_DateTimeConstant_Coerce, METH_VARARGS | METH_STATIC,
            "Coerce DateTimeConstant from arbitrary value"},
        {NULL, NULL, 0, NULL} // sentinel
    };

    typeObject = spi::pyMakeTypeObject(
        "spdoc.DateTimeConstant",
        properties,
        methods,
        false,
        "DateTime constant defined in the configuration file.\n\n__init__(self, value)",
        "Constant",
        Py_tp_init, (void*)py_spdoc_DateTimeConstant_init,
        0);

    return typeObject;
}


static int py_spdoc_DoubleConstant_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "DoubleConstant");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::DoubleConstant::object_type);
        return 0;
    }
    catch (spi::PyException&)
    {
        return -1;
    }
    catch (std::exception &e)
    {
        spi::pyExceptionHandler(e.what());
        return -1;
    }
    catch (...)
    {
        spi::pyExceptionHandler("Unknown exception");
        return -1;
    }
}

PyObject* py_spdoc_DoubleConstant_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("DoubleConstant", args);
    return pyo;
}

static PyTypeObject* DoubleConstant_PyObjectType()
{
    static PyTypeObject* typeObject = NULL;
    if (typeObject)
        return typeObject;

    static PyGetSetDef properties[] =
    {
        {"value", (getter)(spi_py_object_getter), NULL,
            "double value",
            (void*) "value"},
        {NULL} // sentinel
    };

    static PyMethodDef methods[] =
    {
        {"Coerce", (PyCFunction)py_spdoc_DoubleConstant_Coerce, METH_VARARGS | METH_STATIC,
            "Coerce DoubleConstant from arbitrary value"},
        {NULL, NULL, 0, NULL} // sentinel
    };

    typeObject = spi::pyMakeTypeObject(
        "spdoc.DoubleConstant",
        properties,
        methods,
        false,
        "Double constant defined in the configuration file.\n\n__init__(self, value)",
        "Constant",
        Py_tp_init, (void*)py_spdoc_DoubleConstant_init,
        0);

    return typeObject;
}


static int py_spdoc_CharConstant_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "CharConstant");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::CharConstant::object_type);
        return 0;
    }
    catch (spi::PyException&)
    {
        return -1;
    }
    catch (std::exception &e)
    {
        spi::pyExceptionHandler(e.what());
        return -1;
    }
    catch (...)
    {
        spi::pyExceptionHandler("Unknown exception");
        return -1;
    }
}

PyObject* py_spdoc_CharConstant_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("CharConstant", args);
    return pyo;
}

static PyTypeObject* CharConstant_PyObjectType()
{
    static PyTypeObject* typeObject = NULL;
    if (typeObject)
        return typeObject;

    static PyGetSetDef properties[] =
    {
        {"value", (getter)(spi_py_object_getter), NULL,
            "char value",
            (void*) "value"},
        {NULL} // sentinel
    };

    static PyMethodDef methods[] =
    {
        {"Coerce", (PyCFunction)py_spdoc_CharConstant_Coerce, METH_VARARGS | METH_STATIC,
            "Coerce CharConstant from arbitrary value"},
        {NULL, NULL, 0, NULL} // sentinel
    };

    typeObject = spi::pyMakeTypeObject(
        "spdoc.CharConstant",
        properties,
        methods,
        false,
        "Character constant defined in the configuration file.\n\n__init__(self, value)",
        "Constant",
        Py_tp_init, (void*)py_spdoc_CharConstant_init,
        0);

    return typeObject;
}


static int py_spdoc_StringConstant_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "StringConstant");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::StringConstant::object_type);
        return 0;
    }
    catch (spi::PyException&)
    {
        return -1;
    }
    catch (std::exception &e)
    {
        spi::pyExceptionHandler(e.what());
        return -1;
    }
    catch (...)
    {
        spi::pyExceptionHandler("Unknown exception");
        return -1;
    }
}

PyObject* py_spdoc_StringConstant_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("StringConstant", args);
    return pyo;
}

static PyTypeObject* StringConstant_PyObjectType()
{
    static PyTypeObject* typeObject = NULL;
    if (typeObject)
        return typeObject;

    static PyGetSetDef properties[] =
    {
        {"value", (getter)(spi_py_object_getter), NULL,
            "string value",
            (void*) "value"},
        {NULL} // sentinel
    };

    static PyMethodDef methods[] =
    {
        {"Coerce", (PyCFunction)py_spdoc_StringConstant_Coerce, METH_VARARGS | METH_STATIC,
            "Coerce StringConstant from arbitrary value"},
        {NULL, NULL, 0, NULL} // sentinel
    };

    typeObject = spi::pyMakeTypeObject(
        "spdoc.StringConstant",
        properties,
        methods,
        false,
        "String constant defined in the configuration file.\n\n__init__(self, value)",
        "Constant",
        Py_tp_init, (void*)py_spdoc_StringConstant_init,
        0);

    return typeObject;
}


static int py_spdoc_BoolConstant_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "BoolConstant");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::BoolConstant::object_type);
        return 0;
    }
    catch (spi::PyException&)
    {
        return -1;
    }
    catch (std::exception &e)
    {
        spi::pyExceptionHandler(e.what());
        return -1;
    }
    catch (...)
    {
        spi::pyExceptionHandler("Unknown exception");
        return -1;
    }
}

PyObject* py_spdoc_BoolConstant_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("BoolConstant", args);
    return pyo;
}

static PyTypeObject* BoolConstant_PyObjectType()
{
    static PyTypeObject* typeObject = NULL;
    if (typeObject)
        return typeObject;

    static PyGetSetDef properties[] =
    {
        {"value", (getter)(spi_py_object_getter), NULL,
            "bool value",
            (void*) "value"},
        {NULL} // sentinel
    };

    static PyMethodDef methods[] =
    {
        {"Coerce", (PyCFunction)py_spdoc_BoolConstant_Coerce, METH_VARARGS | METH_STATIC,
            "Coerce BoolConstant from arbitrary value"},
        {NULL, NULL, 0, NULL} // sentinel
    };

    typeObject = spi::pyMakeTypeObject(
        "spdoc.BoolConstant",
        properties,
        methods,
        false,
        "Bool constant defined in the configuration file.\n\n__init__(self, value)",
        "Constant",
        Py_tp_init, (void*)py_spdoc_BoolConstant_init,
        0);

    return typeObject;
}


static int py_spdoc_UndefinedConstant_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "UndefinedConstant");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::UndefinedConstant::object_type);
        return 0;
    }
    catch (spi::PyException&)
    {
        return -1;
    }
    catch (std::exception &e)
    {
        spi::pyExceptionHandler(e.what());
        return -1;
    }
    catch (...)
    {
        spi::pyExceptionHandler("Unknown exception");
        return -1;
    }
}

PyObject* py_spdoc_UndefinedConstant_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("UndefinedConstant", args);
    return pyo;
}

static PyTypeObject* UndefinedConstant_PyObjectType()
{
    static PyTypeObject* typeObject = NULL;
    if (typeObject)
        return typeObject;

    static PyMethodDef methods[] =
    {
        {"Coerce", (PyCFunction)py_spdoc_UndefinedConstant_Coerce, METH_VARARGS | METH_STATIC,
            "Coerce UndefinedConstant from arbitrary value"},
        {NULL, NULL, 0, NULL} // sentinel
    };

    typeObject = spi::pyMakeTypeObject(
        "spdoc.UndefinedConstant",
        0,
        methods,
        false,
        "__init__(self)",
        "Constant",
        Py_tp_init, (void*)py_spdoc_UndefinedConstant_init,
        0);

    return typeObject;
}


void py_spdoc_constant_update_functions(spi::PythonService* svc)
{
    svc->SetNamespace("");

    svc->AddClass("Constant", "Constant",
        Constant_PyObjectType());

    svc->AddClass("IntConstant", "IntConstant",
        IntConstant_PyObjectType());

    svc->AddClass("DateConstant", "DateConstant",
        DateConstant_PyObjectType());

    svc->AddClass("DateTimeConstant", "DateTimeConstant",
        DateTimeConstant_PyObjectType());

    svc->AddClass("DoubleConstant", "DoubleConstant",
        DoubleConstant_PyObjectType());

    svc->AddClass("CharConstant", "CharConstant",
        CharConstant_PyObjectType());

    svc->AddClass("StringConstant", "StringConstant",
        StringConstant_PyObjectType());

    svc->AddClass("BoolConstant", "BoolConstant",
        BoolConstant_PyObjectType());

    svc->AddClass("UndefinedConstant", "UndefinedConstant",
        UndefinedConstant_PyObjectType());
}

