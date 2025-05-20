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
* Source file: py_spdoc_configTypes.cpp
****************************************************************************
*/

#include "py_spdoc_configTypes.hpp"
#include "pyd_spdoc.hpp"

#include <spi/python/pyUtil.hpp>
#include <spi/python/pyInput.hpp>
#include <spi/python/pyObject.hpp>
#include <spi/python/pyObjectMap.hpp>
#include <spi/python/pyOutput.hpp>
#include <spi/python/pyService.hpp>
#include <spi/python/pyValue.hpp>

#include "spdoc_configTypes.hpp"

static int py_spdoc_DataType_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("DataType");

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::DataType::object_type);
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

static PyGetSetDef DataType_properties[] = {
    {"name", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "name"},
    {"passByReference", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "passByReference"},
    {"refTypeName", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "refTypeName"},
    {"valueTypeName", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "valueTypeName"},
    {"publicType", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "publicType"},
    {"noDoc", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "noDoc"},
    {"nsService", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "nsService"},
    {"objectAsValue", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "objectAsValue"},
    {NULL} // sentinel
};

PyObject* py_spdoc_DataType_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("DataType", args);
    return pyo;
}

PyObject* py_spdoc_DataType_ValueType(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("DataType.ValueType");

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

PyObject* py_spdoc_DataType_RefType(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("DataType.RefType");

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
static PyMethodDef DataType_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_DataType_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce DataType from arbitrary value"},
    {"ValueType", (PyCFunction)py_spdoc_DataType_ValueType, METH_VARARGS | METH_KEYWORDS,
        "ValueType(self)\n\nReturns the value type (includes the namespace)"},
    {"RefType", (PyCFunction)py_spdoc_DataType_RefType, METH_VARARGS | METH_KEYWORDS,
        "RefType(self)\n\nReturns the reference type (includes the namespace)"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject DataType_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.DataType", /*tp_name*/
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
    "Defines a data type.\n\n__init__(self, name, passByReference, refTypeName, valueTypeName, publicType, noDoc, nsService=None, objectAsValue=None)", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    DataType_methods, /* tp_methods */
    0, /* tp_members */
    DataType_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_DataType_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_Attribute_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Attribute");

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::Attribute::object_type);
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

static PyGetSetDef Attribute_properties[] = {
    {"name", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "name"},
    {"description", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "description"},
    {"dataType", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "dataType"},
    {"arrayDim", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "arrayDim"},
    {"isOptional", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "isOptional"},
    {"defaultValue", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "defaultValue"},
    {"isArray", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "isArray"},
    {NULL} // sentinel
};

PyObject* py_spdoc_Attribute_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("Attribute", args);
    return pyo;
}

PyObject* py_spdoc_Attribute_encoding(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Attribute.encoding");

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
static PyMethodDef Attribute_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_Attribute_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce Attribute from arbitrary value"},
    {"encoding", (PyCFunction)py_spdoc_Attribute_encoding, METH_VARARGS | METH_KEYWORDS,
        "encoding(self, isOutput=None, showDefault=None)"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject Attribute_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.Attribute", /*tp_name*/
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
    "Defines an attribute which is an input to a function call or output from a function call.\n\n__init__(self, name, description=[], dataType, arrayDim=None, isOptional=None, defaultValue=None)", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    Attribute_methods, /* tp_methods */
    0, /* tp_members */
    Attribute_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_Attribute_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_ClassAttribute_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("ClassAttribute");

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::ClassAttribute::object_type);
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

static PyGetSetDef ClassAttribute_properties[] = {
    {"name", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "name"},
    {"description", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "description"},
    {"dataType", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "dataType"},
    {"arrayDim", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "arrayDim"},
    {"isOptional", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "isOptional"},
    {"defaultValue", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "defaultValue"},
    {"accessible", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "accessible"},
    {"accessor", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "accessor"},
    {"isArray", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "isArray"},
    {NULL} // sentinel
};

PyObject* py_spdoc_ClassAttribute_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("ClassAttribute", args);
    return pyo;
}

PyObject* py_spdoc_ClassAttribute_encoding(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("ClassAttribute.encoding");

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
static PyMethodDef ClassAttribute_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_ClassAttribute_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce ClassAttribute from arbitrary value"},
    {"encoding", (PyCFunction)py_spdoc_ClassAttribute_encoding, METH_VARARGS | METH_KEYWORDS,
        "encoding(self, showDefault=None)"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject ClassAttribute_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.ClassAttribute", /*tp_name*/
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
    "Defines an attribute of a class.\n\n__init__(self, name, description=[], dataType, arrayDim=None, isOptional=None, defaultValue=None, accessible, accessor)", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    ClassAttribute_methods, /* tp_methods */
    0, /* tp_members */
    ClassAttribute_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_ClassAttribute_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_Construct_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    try
    {
        throw spi::RuntimeError("Cannot construct class of type %s", "Construct");
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

PyObject* py_spdoc_Construct_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("Construct", args);
    return pyo;
}

PyObject* py_spdoc_Construct_getType(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Construct.getType");

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

PyObject* py_spdoc_Construct_Summary(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Construct.Summary");

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
static PyMethodDef Construct_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_Construct_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce Construct from arbitrary value"},
    {"getType", (PyCFunction)py_spdoc_Construct_getType, METH_VARARGS | METH_KEYWORDS,
        "getType(self)\n\nReturns the type of the Construct - specific to each sub-class of Construct."},
    {"Summary", (PyCFunction)py_spdoc_Construct_Summary, METH_VARARGS | METH_KEYWORDS,
        "Summary(self, includeDescription=None)\n\nReturns a short summary of the construct (disregarding description)"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject Construct_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.Construct", /*tp_name*/
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
    "Defines a construct - this is something that is defined by a module and can be either some sort of type definition or function.", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    Construct_methods, /* tp_methods */
    0, /* tp_members */
    0, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_Construct_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_SimpleType_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("SimpleType");

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::SimpleType::object_type);
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

static PyGetSetDef SimpleType_properties[] = {
    {"name", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "name"},
    {"description", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "description"},
    {"typeName", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "typeName"},
    {"noDoc", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "noDoc"},
    {NULL} // sentinel
};

PyObject* py_spdoc_SimpleType_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("SimpleType", args);
    return pyo;
}
static PyMethodDef SimpleType_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_SimpleType_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce SimpleType from arbitrary value"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject SimpleType_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.SimpleType", /*tp_name*/
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
    "Defines a simple type.\n\n__init__(self, name, description=[], typeName, noDoc=None)", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    SimpleType_methods, /* tp_methods */
    0, /* tp_members */
    SimpleType_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_SimpleType_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_Function_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Function");

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::Function::object_type);
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

static PyGetSetDef Function_properties[] = {
    {"name", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "name"},
    {"description", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "description"},
    {"returnTypeDescription", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "returnTypeDescription"},
    {"returnType", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "returnType"},
    {"returnArrayDim", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "returnArrayDim"},
    {"inputs", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "inputs"},
    {"outputs", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "outputs"},
    {"excelOptions", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "excelOptions"},
    {"optionalReturnType", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "optionalReturnType"},
    {NULL} // sentinel
};

PyObject* py_spdoc_Function_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("Function", args);
    return pyo;
}

PyObject* py_spdoc_Function_returnsObject(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Function.returnsObject");

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

PyObject* py_spdoc_Function_returns(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Function.returns");

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

PyObject* py_spdoc_Function_objectCount(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Function.objectCount");

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
static PyMethodDef Function_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_Function_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce Function from arbitrary value"},
    {"returnsObject", (PyCFunction)py_spdoc_Function_returnsObject, METH_VARARGS | METH_KEYWORDS,
        "returnsObject(self)\n\nDoes this Function return an object or not?"},
    {"returns", (PyCFunction)py_spdoc_Function_returns, METH_VARARGS | METH_KEYWORDS,
        "returns(self)\n\nWhat does the function return (as an attribute)"},
    {"objectCount", (PyCFunction)py_spdoc_Function_objectCount, METH_VARARGS | METH_KEYWORDS,
        "objectCount(self)\n\nHow many objects does this function return?"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject Function_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.Function", /*tp_name*/
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
    "Defines a function.\n\n__init__(self, name, description=[], returnTypeDescription=[], returnType, returnArrayDim=None, inputs=[], outputs=[], excelOptions=[], optionalReturnType=None)", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    Function_methods, /* tp_methods */
    0, /* tp_members */
    Function_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_Function_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_Enumerand_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Enumerand");

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::Enumerand::object_type);
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

static PyGetSetDef Enumerand_properties[] = {
    {"code", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "code"},
    {"strings", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "strings"},
    {"description", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "description"},
    {NULL} // sentinel
};

PyObject* py_spdoc_Enumerand_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("Enumerand", args);
    return pyo;
}
static PyMethodDef Enumerand_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_Enumerand_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce Enumerand from arbitrary value"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject Enumerand_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.Enumerand", /*tp_name*/
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
    "Defines an enumerand.\n\n__init__(self, code, strings=[], description=[])", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    Enumerand_methods, /* tp_methods */
    0, /* tp_members */
    Enumerand_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_Enumerand_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_EnumConstructor_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("EnumConstructor");

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::EnumConstructor::object_type);
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

static PyGetSetDef EnumConstructor_properties[] = {
    {"constructorType", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "constructorType"},
    {"description", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "description"},
    {NULL} // sentinel
};

PyObject* py_spdoc_EnumConstructor_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("EnumConstructor", args);
    return pyo;
}
static PyMethodDef EnumConstructor_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_EnumConstructor_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce EnumConstructor from arbitrary value"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject EnumConstructor_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.EnumConstructor", /*tp_name*/
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
    "__init__(self, constructorType, description=[])", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    EnumConstructor_methods, /* tp_methods */
    0, /* tp_members */
    EnumConstructor_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_EnumConstructor_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_Enum_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Enum");

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::Enum::object_type);
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

static PyGetSetDef Enum_properties[] = {
    {"name", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "name"},
    {"description", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "description"},
    {"enumerands", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "enumerands"},
    {"constructors", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "constructors"},
    {NULL} // sentinel
};

PyObject* py_spdoc_Enum_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("Enum", args);
    return pyo;
}
static PyMethodDef Enum_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_Enum_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce Enum from arbitrary value"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject Enum_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.Enum", /*tp_name*/
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
    "Defines an enumerated type.\n\n__init__(self, name, description=[], enumerands=[], constructors=[])", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    Enum_methods, /* tp_methods */
    0, /* tp_members */
    Enum_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_Enum_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_ClassMethod_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("ClassMethod");

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::ClassMethod::object_type);
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

static PyGetSetDef ClassMethod_properties[] = {
    {"function", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "function"},
    {"isConst", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "isConst"},
    {"isVirtual", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "isVirtual"},
    {"isStatic", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "isStatic"},
    {"isImplementation", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "isImplementation"},
    {"implements", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "implements"},
    {NULL} // sentinel
};

PyObject* py_spdoc_ClassMethod_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("ClassMethod", args);
    return pyo;
}

PyObject* py_spdoc_ClassMethod_Summary(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("ClassMethod.Summary");

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
static PyMethodDef ClassMethod_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_ClassMethod_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce ClassMethod from arbitrary value"},
    {"Summary", (PyCFunction)py_spdoc_ClassMethod_Summary, METH_VARARGS | METH_KEYWORDS,
        "Summary(self, includeDescription=None)"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject ClassMethod_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.ClassMethod", /*tp_name*/
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
    "Defines a class method.\n\n__init__(self, function, isConst, isVirtual=None, isStatic=None, isImplementation=None, implements=None)", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    ClassMethod_methods, /* tp_methods */
    0, /* tp_members */
    ClassMethod_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_ClassMethod_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_CoerceFrom_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("CoerceFrom");

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::CoerceFrom::object_type);
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

static PyGetSetDef CoerceFrom_properties[] = {
    {"description", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "description"},
    {"coerceFrom", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "coerceFrom"},
    {NULL} // sentinel
};

PyObject* py_spdoc_CoerceFrom_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("CoerceFrom", args);
    return pyo;
}

PyObject* py_spdoc_CoerceFrom_Summary(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("CoerceFrom.Summary");

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
static PyMethodDef CoerceFrom_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_CoerceFrom_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce CoerceFrom from arbitrary value"},
    {"Summary", (PyCFunction)py_spdoc_CoerceFrom_Summary, METH_VARARGS | METH_KEYWORDS,
        "Summary(self, includeDescription=None)"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject CoerceFrom_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.CoerceFrom", /*tp_name*/
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
    "Defines a CoerceFrom constructor for creating a class by coercion from a single input of some other type.\n\n__init__(self, description=[], coerceFrom)", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    CoerceFrom_methods, /* tp_methods */
    0, /* tp_members */
    CoerceFrom_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_CoerceFrom_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_CoerceTo_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("CoerceTo");

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::CoerceTo::object_type);
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

static PyGetSetDef CoerceTo_properties[] = {
    {"description", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "description"},
    {"className", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "className"},
    {"classType", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "classType"},
    {NULL} // sentinel
};

PyObject* py_spdoc_CoerceTo_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("CoerceTo", args);
    return pyo;
}

PyObject* py_spdoc_CoerceTo_Summary(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("CoerceTo.Summary");

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
static PyMethodDef CoerceTo_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_CoerceTo_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce CoerceTo from arbitrary value"},
    {"Summary", (PyCFunction)py_spdoc_CoerceTo_Summary, METH_VARARGS | METH_KEYWORDS,
        "Summary(self, includeDescription=None)"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject CoerceTo_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.CoerceTo", /*tp_name*/
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
    "Defines a CoerceTo operator for creating a class by coercion to a class from an instance of another class.\n\n__init__(self, description=[], className, classType)", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    CoerceTo_methods, /* tp_methods */
    0, /* tp_members */
    CoerceTo_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_CoerceTo_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_Class_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Class");

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::Class::object_type);
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

static PyGetSetDef Class_properties[] = {
    {"name", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "name"},
    {"ns", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "ns"},
    {"description", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "description"},
    {"baseClassName", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "baseClassName"},
    {"attributes", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "attributes"},
    {"properties", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "properties"},
    {"methods", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "methods"},
    {"coerceFrom", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "coerceFrom"},
    {"coerceTo", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "coerceTo"},
    {"isAbstract", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "isAbstract"},
    {"noMake", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "noMake"},
    {"objectName", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "objectName"},
    {"dataType", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "dataType"},
    {"isDelegate", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "isDelegate"},
    {"canPut", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "canPut"},
    {"hasDynamicAttributes", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "hasDynamicAttributes"},
    {"asValue", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "asValue"},
    {"constructor", (getter)(spi_py_object_getter), (setter)(spi_py_object_setter), NULL,
        (void*) "constructor"},
    {NULL} // sentinel
};

PyObject* py_spdoc_Class_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("Class", args);
    return pyo;
}

PyObject* py_spdoc_Class_ObjectName(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Class.ObjectName");

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

PyObject* py_spdoc_Class_ServiceNamespace(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Class.ServiceNamespace");

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
static PyMethodDef Class_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_Class_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce Class from arbitrary value"},
    {"ObjectName", (PyCFunction)py_spdoc_Class_ObjectName, METH_VARARGS | METH_KEYWORDS,
        "ObjectName(self)\n\nReturns the object name. If objectName is defined, then that is the object name. Otherwise name is the object name."},
    {"ServiceNamespace", (PyCFunction)py_spdoc_Class_ServiceNamespace, METH_VARARGS | METH_KEYWORDS,
        "ServiceNamespace(self)"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject Class_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.Class", /*tp_name*/
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
    "Defines a class.\n\n__init__(self, name, ns=None, description=[], baseClassName=None, attributes=[], properties=[], methods=[], coerceFrom=[], coerceTo=[], isAbstract=None, noMake=None, objectName, dataType, isDelegate=None, canPut=None, hasDynamicAttributes=None, asValue=None, constructor=None)", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    Class_methods, /* tp_methods */
    0, /* tp_members */
    Class_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_Class_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_Module_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Module");

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::Module::object_type);
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

static PyGetSetDef Module_properties[] = {
    {"name", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "name"},
    {"description", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "description"},
    {"ns", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "ns"},
    {"constructs", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "constructs"},
    {NULL} // sentinel
};

PyObject* py_spdoc_Module_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("Module", args);
    return pyo;
}

PyObject* py_spdoc_Module_combineSummaries(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Module.combineSummaries");

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
static PyMethodDef Module_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_Module_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce Module from arbitrary value"},
    {"combineSummaries", (PyCFunction)py_spdoc_Module_combineSummaries, METH_VARARGS | METH_KEYWORDS | METH_STATIC,
        "combineSummaries(modules=[], sort)"},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject Module_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.Module", /*tp_name*/
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
    "Defines a module.\n\n__init__(self, name, description=[], ns=None, constructs=[])", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    Module_methods, /* tp_methods */
    0, /* tp_members */
    Module_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_Module_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

static int py_spdoc_Service_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Service");

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spdoc::Service::object_type);
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

static PyGetSetDef Service_properties[] = {
    {"name", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "name"},
    {"description", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "description"},
    {"longName", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "longName"},
    {"ns", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "ns"},
    {"declSpec", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "declSpec"},
    {"version", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "version"},
    {"modules", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "modules"},
    {"importedBaseClasses", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "importedBaseClasses"},
    {"importedEnums", (getter)(spi_py_object_getter), NULL, NULL,
        (void*) "importedEnums"},
    {"sharedService", (getter)(spi_py_object_getter), NULL,
        "Indicates that this service is shared with a previously defined service. As a result we will not implement some of the common service functions for the add-ins.",
        (void*) "sharedService"},
    {NULL} // sentinel
};

PyObject* py_spdoc_Service_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("Service", args);
    return pyo;
}

PyObject* py_spdoc_Service_CombineSharedServices(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Service.CombineSharedServices");

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

PyObject* py_spdoc_Service_Summary(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Service.Summary");

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

PyObject* py_spdoc_Service_combineSummaries(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Service.combineSummaries");

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

PyObject* py_spdoc_Service_getEnums(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Service.getEnums");

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

PyObject* py_spdoc_Service_getEnum(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Service.getEnum");

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

PyObject* py_spdoc_Service_getEnumerands(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Service.getEnumerands");

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

PyObject* py_spdoc_Service_getClasses(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Service.getClasses");

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

PyObject* py_spdoc_Service_getClass(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Service.getClass");

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

PyObject* py_spdoc_Service_isSubClass(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Service.isSubClass");

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

PyObject* py_spdoc_Service_getPropertyClass(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Service.getPropertyClass");

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

PyObject* py_spdoc_Service_getConstructs(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Service.getConstructs");

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

PyObject* py_spdoc_Service_getConstruct(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    try
    {
        if (!func)
            func = get_function_caller("Service.getConstruct");

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
static PyMethodDef Service_methods[] = {
    {"Coerce", (PyCFunction)py_spdoc_Service_Coerce, METH_VARARGS | METH_STATIC,
        "Coerce Service from arbitrary value"},
    {"CombineSharedServices", (PyCFunction)py_spdoc_Service_CombineSharedServices, METH_VARARGS | METH_KEYWORDS,
        "CombineSharedServices(self, sharedServices=[])"},
    {"Summary", (PyCFunction)py_spdoc_Service_Summary, METH_VARARGS | METH_KEYWORDS,
        "Summary(self, sort)"},
    {"combineSummaries", (PyCFunction)py_spdoc_Service_combineSummaries, METH_VARARGS | METH_KEYWORDS | METH_STATIC,
        "combineSummaries(services=[], sort)"},
    {"getEnums", (PyCFunction)py_spdoc_Service_getEnums, METH_VARARGS | METH_KEYWORDS,
        "getEnums(self)\n\nReturns a sorted list of enumerated type names."},
    {"getEnum", (PyCFunction)py_spdoc_Service_getEnum, METH_VARARGS | METH_KEYWORDS,
        "getEnum(self, name)\n\nGet an individual Enum by name."},
    {"getEnumerands", (PyCFunction)py_spdoc_Service_getEnumerands, METH_VARARGS | METH_KEYWORDS,
        "getEnumerands(self, name)\n\nReturns the enumerands for a given enumerated type."},
    {"getClasses", (PyCFunction)py_spdoc_Service_getClasses, METH_VARARGS | METH_KEYWORDS,
        "getClasses(self)\n\nReturns a sorted list of class names."},
    {"getClass", (PyCFunction)py_spdoc_Service_getClass, METH_VARARGS | METH_KEYWORDS,
        "getClass(self, className)\n\nReturns the class details for a class name."},
    {"isSubClass", (PyCFunction)py_spdoc_Service_isSubClass, METH_VARARGS | METH_KEYWORDS,
        "isSubClass(self, cls, name)\n\nReturns whether a given class is a sub-class of the data type of the given name. Needs to be a method on the Service since otherwise we cannot find base class."},
    {"getPropertyClass", (PyCFunction)py_spdoc_Service_getPropertyClass, METH_VARARGS | METH_KEYWORDS,
        "getPropertyClass(self, baseClassName, fieldName)\n\nReturns the name of the class for which the given fieldName is a property. If no such class exists then returns an empty string."},
    {"getConstructs", (PyCFunction)py_spdoc_Service_getConstructs, METH_VARARGS | METH_KEYWORDS,
        "getConstructs(self)\n\nReturns a sorted list of constructs defined by the service."},
    {"getConstruct", (PyCFunction)py_spdoc_Service_getConstruct, METH_VARARGS | METH_KEYWORDS,
        "getConstruct(self, name)\n\nReturns the construct details for a name."},
    {NULL, NULL, 0, NULL} // sentinel
};

static PyTypeObject Service_PyObjectType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "spdoc.Service", /*tp_name*/
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
    "Defines a service.\n\n__init__(self, name, description=[], longName, ns, declSpec, version, modules=[], importedBaseClasses=[], importedEnums=[], sharedService=None)", /* tp_doc */
    0, /* tp_traverse */
    0, /* tp_clear */
    0, /* tp_richcompare */
    0, /* tp_weaklistoffset */
    0, /* tp_iter */
    0, /* tp_iternext */
    Service_methods, /* tp_methods */
    0, /* tp_members */
    Service_properties, /* tp_getset */
    0, /* tp_base */
    0, /* tp_dict */
    0, /* tp_descr_get */
    0, /* tp_descr_set */
    0, /* tp_dictoffset */
    (initproc)py_spdoc_Service_init, /* tp_init */
    0, /* tp_alloc */
    PyType_GenericNew, /* tp_new */
};

void py_spdoc_configTypes_update_functions(spi::PythonService* svc)
{
    svc->SetNamespace("");

    svc->AddClass("DataType", "DataType",
        &DataType_PyObjectType);

    svc->AddClass("Attribute", "Attribute",
        &Attribute_PyObjectType);

    svc->AddClass("ClassAttribute", "ClassAttribute",
        &ClassAttribute_PyObjectType);

    svc->AddClass("Construct", "Construct",
        &Construct_PyObjectType);

    svc->AddClass("SimpleType", "SimpleType",
        &SimpleType_PyObjectType, "Construct");

    svc->AddClass("Function", "Function",
        &Function_PyObjectType, "Construct");

    svc->AddClass("Enumerand", "Enumerand",
        &Enumerand_PyObjectType);

    svc->AddClass("EnumConstructor", "EnumConstructor",
        &EnumConstructor_PyObjectType);

    svc->AddClass("Enum", "Enum",
        &Enum_PyObjectType, "Construct");

    svc->AddClass("ClassMethod", "ClassMethod",
        &ClassMethod_PyObjectType);

    svc->AddClass("CoerceFrom", "CoerceFrom",
        &CoerceFrom_PyObjectType);

    svc->AddClass("CoerceTo", "CoerceTo",
        &CoerceTo_PyObjectType);

    svc->AddClass("Class", "Class",
        &Class_PyObjectType, "Construct");

    svc->AddClass("Module", "Module",
        &Module_PyObjectType);

    svc->AddClass("Service", "Service",
        &Service_PyObjectType);
}

