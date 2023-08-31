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

#ifdef PYTHON_HAS_FASTCALL
PyObject* py_spdoc_Constant_typeName(PyObject* self, PyObject* const* args, Py_ssize_t nargs, PyObject* kwargs)
#else
PyObject* py_spdoc_Constant_typeName(PyObject* self, PyObject* args, PyObject* kwargs)
#endif
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Constant.typeName");

#ifdef PYTHON_HAS_FASTCALL
        const spi::InputValues & iv = spi::pyGetInputValues(func, args, nargs, kwargs, self);
#else
        const spi::InputValues & iv = spi::pyGetInputValues(func, args, kwargs, self);
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
PyObject* py_spdoc_Constant_docString(PyObject* self, PyObject* const* args, Py_ssize_t nargs, PyObject* kwargs)
#else
PyObject* py_spdoc_Constant_docString(PyObject* self, PyObject* args, PyObject* kwargs)
#endif
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Constant.docString");

#ifdef PYTHON_HAS_FASTCALL
        const spi::InputValues & iv = spi::pyGetInputValues(func, args, nargs, kwargs, self);
#else
        const spi::InputValues & iv = spi::pyGetInputValues(func, args, kwargs, self);
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
PyObject* py_spdoc_Constant_getInt(PyObject* self, PyObject* const* args, Py_ssize_t nargs, PyObject* kwargs)
#else
PyObject* py_spdoc_Constant_getInt(PyObject* self, PyObject* args, PyObject* kwargs)
#endif
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Constant.getInt");

#ifdef PYTHON_HAS_FASTCALL
        const spi::InputValues & iv = spi::pyGetInputValues(func, args, nargs, kwargs, self);
#else
        const spi::InputValues & iv = spi::pyGetInputValues(func, args, kwargs, self);
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
PyObject* py_spdoc_Constant_getDate(PyObject* self, PyObject* const* args, Py_ssize_t nargs, PyObject* kwargs)
#else
PyObject* py_spdoc_Constant_getDate(PyObject* self, PyObject* args, PyObject* kwargs)
#endif
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Constant.getDate");

#ifdef PYTHON_HAS_FASTCALL
        const spi::InputValues & iv = spi::pyGetInputValues(func, args, nargs, kwargs, self);
#else
        const spi::InputValues & iv = spi::pyGetInputValues(func, args, kwargs, self);
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
PyObject* py_spdoc_Constant_getDateTime(PyObject* self, PyObject* const* args, Py_ssize_t nargs, PyObject* kwargs)
#else
PyObject* py_spdoc_Constant_getDateTime(PyObject* self, PyObject* args, PyObject* kwargs)
#endif
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Constant.getDateTime");

#ifdef PYTHON_HAS_FASTCALL
        const spi::InputValues & iv = spi::pyGetInputValues(func, args, nargs, kwargs, self);
#else
        const spi::InputValues & iv = spi::pyGetInputValues(func, args, kwargs, self);
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
PyObject* py_spdoc_Constant_getDouble(PyObject* self, PyObject* const* args, Py_ssize_t nargs, PyObject* kwargs)
#else
PyObject* py_spdoc_Constant_getDouble(PyObject* self, PyObject* args, PyObject* kwargs)
#endif
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Constant.getDouble");

#ifdef PYTHON_HAS_FASTCALL
        const spi::InputValues & iv = spi::pyGetInputValues(func, args, nargs, kwargs, self);
#else
        const spi::InputValues & iv = spi::pyGetInputValues(func, args, kwargs, self);
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
PyObject* py_spdoc_Constant_getChar(PyObject* self, PyObject* const* args, Py_ssize_t nargs, PyObject* kwargs)
#else
PyObject* py_spdoc_Constant_getChar(PyObject* self, PyObject* args, PyObject* kwargs)
#endif
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Constant.getChar");

#ifdef PYTHON_HAS_FASTCALL
        const spi::InputValues & iv = spi::pyGetInputValues(func, args, nargs, kwargs, self);
#else
        const spi::InputValues & iv = spi::pyGetInputValues(func, args, kwargs, self);
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
PyObject* py_spdoc_Constant_getString(PyObject* self, PyObject* const* args, Py_ssize_t nargs, PyObject* kwargs)
#else
PyObject* py_spdoc_Constant_getString(PyObject* self, PyObject* args, PyObject* kwargs)
#endif
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Constant.getString");

#ifdef PYTHON_HAS_FASTCALL
        const spi::InputValues & iv = spi::pyGetInputValues(func, args, nargs, kwargs, self);
#else
        const spi::InputValues & iv = spi::pyGetInputValues(func, args, kwargs, self);
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
PyObject* py_spdoc_Constant_getBool(PyObject* self, PyObject* const* args, Py_ssize_t nargs, PyObject* kwargs)
#else
PyObject* py_spdoc_Constant_getBool(PyObject* self, PyObject* args, PyObject* kwargs)
#endif
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Constant.getBool");

#ifdef PYTHON_HAS_FASTCALL
        const spi::InputValues & iv = spi::pyGetInputValues(func, args, nargs, kwargs, self);
#else
        const spi::InputValues & iv = spi::pyGetInputValues(func, args, kwargs, self);
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
static PyMethodDef Constant_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_Constant_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce Constant from arbitrary value"},
    {"typeName", (PyCFunction)py_spdoc_Constant_typeName,
#ifdef PYTHON_HAS_FASTCALL
        METH_FASTCALL | METH_KEYWORDS,
#else
        METH_VARARGS | METH_KEYWORDS,
#endif
        "typeName(self)\n\nreturns the data type name for the scalar"},
    {"docString", (PyCFunction)py_spdoc_Constant_docString,
#ifdef PYTHON_HAS_FASTCALL
        METH_FASTCALL | METH_KEYWORDS,
#else
        METH_VARARGS | METH_KEYWORDS,
#endif
        "docString(self)\n\nreturns the string which should appear in documentation"},
    {"getInt", (PyCFunction)py_spdoc_Constant_getInt,
#ifdef PYTHON_HAS_FASTCALL
        METH_FASTCALL | METH_KEYWORDS,
#else
        METH_VARARGS | METH_KEYWORDS,
#endif
        "getInt(self)\n\nreturns the integer value (where applicable) for the scalar"},
    {"getDate", (PyCFunction)py_spdoc_Constant_getDate,
#ifdef PYTHON_HAS_FASTCALL
        METH_FASTCALL | METH_KEYWORDS,
#else
        METH_VARARGS | METH_KEYWORDS,
#endif
        "getDate(self)\n\nreturns the date value (where applicable) for the scalar"},
    {"getDateTime", (PyCFunction)py_spdoc_Constant_getDateTime,
#ifdef PYTHON_HAS_FASTCALL
        METH_FASTCALL | METH_KEYWORDS,
#else
        METH_VARARGS | METH_KEYWORDS,
#endif
        "getDateTime(self)\n\nreturns the date time value (where applicable) for the scalar"},
    {"getDouble", (PyCFunction)py_spdoc_Constant_getDouble,
#ifdef PYTHON_HAS_FASTCALL
        METH_FASTCALL | METH_KEYWORDS,
#else
        METH_VARARGS | METH_KEYWORDS,
#endif
        "getDouble(self)\n\nreturns the double value (where applicable) for the scalar"},
    {"getChar", (PyCFunction)py_spdoc_Constant_getChar,
#ifdef PYTHON_HAS_FASTCALL
        METH_FASTCALL | METH_KEYWORDS,
#else
        METH_VARARGS | METH_KEYWORDS,
#endif
        "getChar(self)\n\nreturns the char value (where applicable) for the scalar"},
    {"getString", (PyCFunction)py_spdoc_Constant_getString,
#ifdef PYTHON_HAS_FASTCALL
        METH_FASTCALL | METH_KEYWORDS,
#else
        METH_VARARGS | METH_KEYWORDS,
#endif
        "getString(self)\n\nreturns the string value (where applicable) for the scalar"},
    {"getBool", (PyCFunction)py_spdoc_Constant_getBool,
#ifdef PYTHON_HAS_FASTCALL
        METH_FASTCALL | METH_KEYWORDS,
#else
        METH_VARARGS | METH_KEYWORDS,
#endif
        "getBool(self)\n\nreturns the bool value (where applicable) for the scalar"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject Constant_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.Constant", /*tp_name*/
    sizeof(SpiPyObject), /*tp_basicsize*/
    0, /*tp_itemsize*/
    (destructor)spi_py_object_dealloc, /*tp_dealloc*/
    0, /*tp_print*/
    0, /*tp_getattr*/
    0, /*tp_setattr*/
    0, /*tp_compare*/
    0, /*tp_repr*/
    0, /*tp_as_number*/
    0, /*tp_as_sequence*/
    0, /*tp_as_mapping*/
    0, /*tp_hash */
    0, /*tp_call*/
    0, /*tp_str*/
    0, /*tp_getattro*/
    0, /*tp_setattro*/
    0, /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT, /*tp_flags*/
    "Interface class defining a constant scalar value.", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    Constant_methods, /* tp_methods */
    0, /* tp_members */
    0, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_Constant_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_IntConstant_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("IntConstant");

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

static PyGetSetDef IntConstant_properties[] = {
    {"value", (getter)(spi_py_object_getter), NULL,
        "integer value",
        (void*) "value"},
    {NULL} // sentinel
};

PyObject* py_spdoc_IntConstant_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("IntConstant", args);
    return pyo;
}
static PyMethodDef IntConstant_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_IntConstant_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce IntConstant from arbitrary value"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject IntConstant_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.IntConstant", /*tp_name*/
    sizeof(SpiPyObject), /*tp_basicsize*/
    0, /*tp_itemsize*/
    (destructor)spi_py_object_dealloc, /*tp_dealloc*/
    0, /*tp_print*/
    0, /*tp_getattr*/
    0, /*tp_setattr*/
    0, /*tp_compare*/
    0, /*tp_repr*/
    0, /*tp_as_number*/
    0, /*tp_as_sequence*/
    0, /*tp_as_mapping*/
    0, /*tp_hash */
    0, /*tp_call*/
    0, /*tp_str*/
    0, /*tp_getattro*/
    0, /*tp_setattro*/
    0, /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT, /*tp_flags*/
    "Integer constant defined in the configuration file.\n\n__init__(self, value)", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    IntConstant_methods, /* tp_methods */
    0, /* tp_members */
    IntConstant_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_IntConstant_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_DateConstant_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("DateConstant");

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

static PyGetSetDef DateConstant_properties[] = {
    {"value", (getter)(spi_py_object_getter), NULL,
        "date value",
        (void*) "value"},
    {NULL} // sentinel
};

PyObject* py_spdoc_DateConstant_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("DateConstant", args);
    return pyo;
}
static PyMethodDef DateConstant_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_DateConstant_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce DateConstant from arbitrary value"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject DateConstant_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.DateConstant", /*tp_name*/
    sizeof(SpiPyObject), /*tp_basicsize*/
    0, /*tp_itemsize*/
    (destructor)spi_py_object_dealloc, /*tp_dealloc*/
    0, /*tp_print*/
    0, /*tp_getattr*/
    0, /*tp_setattr*/
    0, /*tp_compare*/
    0, /*tp_repr*/
    0, /*tp_as_number*/
    0, /*tp_as_sequence*/
    0, /*tp_as_mapping*/
    0, /*tp_hash */
    0, /*tp_call*/
    0, /*tp_str*/
    0, /*tp_getattro*/
    0, /*tp_setattro*/
    0, /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT, /*tp_flags*/
    "Date constant defined in the configuration file.\n\n__init__(self, value)", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    DateConstant_methods, /* tp_methods */
    0, /* tp_members */
    DateConstant_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_DateConstant_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_DateTimeConstant_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("DateTimeConstant");

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

static PyGetSetDef DateTimeConstant_properties[] = {
    {"value", (getter)(spi_py_object_getter), NULL,
        "datetime value",
        (void*) "value"},
    {NULL} // sentinel
};

PyObject* py_spdoc_DateTimeConstant_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("DateTimeConstant", args);
    return pyo;
}
static PyMethodDef DateTimeConstant_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_DateTimeConstant_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce DateTimeConstant from arbitrary value"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject DateTimeConstant_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.DateTimeConstant", /*tp_name*/
    sizeof(SpiPyObject), /*tp_basicsize*/
    0, /*tp_itemsize*/
    (destructor)spi_py_object_dealloc, /*tp_dealloc*/
    0, /*tp_print*/
    0, /*tp_getattr*/
    0, /*tp_setattr*/
    0, /*tp_compare*/
    0, /*tp_repr*/
    0, /*tp_as_number*/
    0, /*tp_as_sequence*/
    0, /*tp_as_mapping*/
    0, /*tp_hash */
    0, /*tp_call*/
    0, /*tp_str*/
    0, /*tp_getattro*/
    0, /*tp_setattro*/
    0, /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT, /*tp_flags*/
    "DateTime constant defined in the configuration file.\n\n__init__(self, value)", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    DateTimeConstant_methods, /* tp_methods */
    0, /* tp_members */
    DateTimeConstant_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_DateTimeConstant_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_DoubleConstant_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("DoubleConstant");

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

static PyGetSetDef DoubleConstant_properties[] = {
    {"value", (getter)(spi_py_object_getter), NULL,
        "double value",
        (void*) "value"},
    {NULL} // sentinel
};

PyObject* py_spdoc_DoubleConstant_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("DoubleConstant", args);
    return pyo;
}
static PyMethodDef DoubleConstant_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_DoubleConstant_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce DoubleConstant from arbitrary value"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject DoubleConstant_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.DoubleConstant", /*tp_name*/
    sizeof(SpiPyObject), /*tp_basicsize*/
    0, /*tp_itemsize*/
    (destructor)spi_py_object_dealloc, /*tp_dealloc*/
    0, /*tp_print*/
    0, /*tp_getattr*/
    0, /*tp_setattr*/
    0, /*tp_compare*/
    0, /*tp_repr*/
    0, /*tp_as_number*/
    0, /*tp_as_sequence*/
    0, /*tp_as_mapping*/
    0, /*tp_hash */
    0, /*tp_call*/
    0, /*tp_str*/
    0, /*tp_getattro*/
    0, /*tp_setattro*/
    0, /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT, /*tp_flags*/
    "Double constant defined in the configuration file.\n\n__init__(self, value)", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    DoubleConstant_methods, /* tp_methods */
    0, /* tp_members */
    DoubleConstant_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_DoubleConstant_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_CharConstant_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("CharConstant");

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

static PyGetSetDef CharConstant_properties[] = {
    {"value", (getter)(spi_py_object_getter), NULL,
        "char value",
        (void*) "value"},
    {NULL} // sentinel
};

PyObject* py_spdoc_CharConstant_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("CharConstant", args);
    return pyo;
}
static PyMethodDef CharConstant_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_CharConstant_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce CharConstant from arbitrary value"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject CharConstant_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.CharConstant", /*tp_name*/
    sizeof(SpiPyObject), /*tp_basicsize*/
    0, /*tp_itemsize*/
    (destructor)spi_py_object_dealloc, /*tp_dealloc*/
    0, /*tp_print*/
    0, /*tp_getattr*/
    0, /*tp_setattr*/
    0, /*tp_compare*/
    0, /*tp_repr*/
    0, /*tp_as_number*/
    0, /*tp_as_sequence*/
    0, /*tp_as_mapping*/
    0, /*tp_hash */
    0, /*tp_call*/
    0, /*tp_str*/
    0, /*tp_getattro*/
    0, /*tp_setattro*/
    0, /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT, /*tp_flags*/
    "Character constant defined in the configuration file.\n\n__init__(self, value)", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    CharConstant_methods, /* tp_methods */
    0, /* tp_members */
    CharConstant_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_CharConstant_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_StringConstant_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("StringConstant");

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

static PyGetSetDef StringConstant_properties[] = {
    {"value", (getter)(spi_py_object_getter), NULL,
        "string value",
        (void*) "value"},
    {NULL} // sentinel
};

PyObject* py_spdoc_StringConstant_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("StringConstant", args);
    return pyo;
}
static PyMethodDef StringConstant_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_StringConstant_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce StringConstant from arbitrary value"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject StringConstant_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.StringConstant", /*tp_name*/
    sizeof(SpiPyObject), /*tp_basicsize*/
    0, /*tp_itemsize*/
    (destructor)spi_py_object_dealloc, /*tp_dealloc*/
    0, /*tp_print*/
    0, /*tp_getattr*/
    0, /*tp_setattr*/
    0, /*tp_compare*/
    0, /*tp_repr*/
    0, /*tp_as_number*/
    0, /*tp_as_sequence*/
    0, /*tp_as_mapping*/
    0, /*tp_hash */
    0, /*tp_call*/
    0, /*tp_str*/
    0, /*tp_getattro*/
    0, /*tp_setattro*/
    0, /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT, /*tp_flags*/
    "String constant defined in the configuration file.\n\n__init__(self, value)", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    StringConstant_methods, /* tp_methods */
    0, /* tp_members */
    StringConstant_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_StringConstant_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_BoolConstant_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("BoolConstant");

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

static PyGetSetDef BoolConstant_properties[] = {
    {"value", (getter)(spi_py_object_getter), NULL,
        "bool value",
        (void*) "value"},
    {NULL} // sentinel
};

PyObject* py_spdoc_BoolConstant_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("BoolConstant", args);
    return pyo;
}
static PyMethodDef BoolConstant_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_BoolConstant_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce BoolConstant from arbitrary value"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject BoolConstant_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.BoolConstant", /*tp_name*/
    sizeof(SpiPyObject), /*tp_basicsize*/
    0, /*tp_itemsize*/
    (destructor)spi_py_object_dealloc, /*tp_dealloc*/
    0, /*tp_print*/
    0, /*tp_getattr*/
    0, /*tp_setattr*/
    0, /*tp_compare*/
    0, /*tp_repr*/
    0, /*tp_as_number*/
    0, /*tp_as_sequence*/
    0, /*tp_as_mapping*/
    0, /*tp_hash */
    0, /*tp_call*/
    0, /*tp_str*/
    0, /*tp_getattro*/
    0, /*tp_setattro*/
    0, /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT, /*tp_flags*/
    "Bool constant defined in the configuration file.\n\n__init__(self, value)", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    BoolConstant_methods, /* tp_methods */
    0, /* tp_members */
    BoolConstant_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_BoolConstant_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_UndefinedConstant_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("UndefinedConstant");

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
static PyMethodDef UndefinedConstant_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_UndefinedConstant_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce UndefinedConstant from arbitrary value"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject UndefinedConstant_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.UndefinedConstant", /*tp_name*/
    sizeof(SpiPyObject), /*tp_basicsize*/
    0, /*tp_itemsize*/
    (destructor)spi_py_object_dealloc, /*tp_dealloc*/
    0, /*tp_print*/
    0, /*tp_getattr*/
    0, /*tp_setattr*/
    0, /*tp_compare*/
    0, /*tp_repr*/
    0, /*tp_as_number*/
    0, /*tp_as_sequence*/
    0, /*tp_as_mapping*/
    0, /*tp_hash */
    0, /*tp_call*/
    0, /*tp_str*/
    0, /*tp_getattro*/
    0, /*tp_setattro*/
    0, /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT, /*tp_flags*/
    "__init__(self)", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    UndefinedConstant_methods, /* tp_methods */
    0, /* tp_members */
    0, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_UndefinedConstant_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

void py_spdoc_constant_update_functions(spi::PythonService* svc)
{
    svc->SetNamespace("");

    svc->AddClass("Constant", "Constant",
        &Constant_PyObjectType);

    svc->AddClass("IntConstant", "IntConstant",
        &IntConstant_PyObjectType, "Constant");

    svc->AddClass("DateConstant", "DateConstant",
        &DateConstant_PyObjectType, "Constant");

    svc->AddClass("DateTimeConstant", "DateTimeConstant",
        &DateTimeConstant_PyObjectType, "Constant");

    svc->AddClass("DoubleConstant", "DoubleConstant",
        &DoubleConstant_PyObjectType, "Constant");

    svc->AddClass("CharConstant", "CharConstant",
        &CharConstant_PyObjectType, "Constant");

    svc->AddClass("StringConstant", "StringConstant",
        &StringConstant_PyObjectType, "Constant");

    svc->AddClass("BoolConstant", "BoolConstant",
        &BoolConstant_PyObjectType, "Constant");

    svc->AddClass("UndefinedConstant", "UndefinedConstant",
        &UndefinedConstant_PyObjectType, "Constant");
}

