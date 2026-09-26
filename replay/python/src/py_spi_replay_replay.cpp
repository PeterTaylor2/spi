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
* Source file: py_spi_replay_replay.cpp
****************************************************************************
*/

#include "py_spi_replay_replay.hpp"
#include "pyd_replay.hpp"

#include <spi/python/pyUtil.hpp>
#include <spi/python/pyInput.hpp>
#include <spi/python/pyObject.hpp>
#include <spi/python/pyObjectMap.hpp>
#include <spi/python/pyOutput.hpp>
#include <spi/python/pyService.hpp>
#include <spi/python/pyValue.hpp>

#include "spi_replay_replay.hpp"

static int py_spi_replay_ReplayAction_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    try
    {
        throw spi::RuntimeError("Cannot construct class of type %s", "ReplayAction");
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

PyObject* py_spi_replay_ReplayAction_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("ReplayAction", args);
    return pyo;
}

static PyTypeObject* ReplayAction_PyObjectType()
{
    static PyTypeObject* typeObject = NULL;
    if (typeObject)
        return typeObject;

    static PyMethodDef methods[] =
    {
        {"Coerce", (PyCFunction)py_spi_replay_ReplayAction_Coerce, METH_VARARGS | METH_STATIC,
            "Coerce ReplayAction from arbitrary value"},
        {NULL, NULL, 0, NULL} // sentinel
    };

    typeObject = spi::pyMakeTypeObject(
        "spi_replay.ReplayAction",
        0,
        methods,
        true,
        "",
        nullptr,
        Py_tp_init, (void*)py_spi_replay_ReplayAction_init,
        0);

    return typeObject;
}


static int py_spi_replay_ReplayFunctionAction_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "ReplayFunctionAction");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spi_replay::ReplayFunctionAction::object_type);
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

PyObject* py_spi_replay_ReplayFunctionAction_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("ReplayFunctionAction", args);
    return pyo;
}

static PyTypeObject* ReplayFunctionAction_PyObjectType()
{
    static PyTypeObject* typeObject = NULL;
    if (typeObject)
        return typeObject;

    static PyGetSetDef properties[] =
    {
        {"inputs", (getter)(spi_py_object_getter), NULL, NULL,
            (void*) "inputs"},
        {"output", (getter)(spi_py_object_getter), NULL, NULL,
            (void*) "output"},
        {NULL} // sentinel
    };

    static PyMethodDef methods[] =
    {
        {"Coerce", (PyCFunction)py_spi_replay_ReplayFunctionAction_Coerce, METH_VARARGS | METH_STATIC,
            "Coerce ReplayFunctionAction from arbitrary value"},
        {NULL, NULL, 0, NULL} // sentinel
    };

    typeObject = spi::pyMakeTypeObject(
        "spi_replay.ReplayFunctionAction",
        properties,
        methods,
        false,
        "__init__(self, inputs, output)",
        "ReplayAction",
        Py_tp_init, (void*)py_spi_replay_ReplayFunctionAction_init,
        0);

    return typeObject;
}


static int py_spi_replay_ReplayObjectAction_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "ReplayObjectAction");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        self->obj = spi::pyInitConstObject(args, kwargs, func, &spi_replay::ReplayObjectAction::object_type);
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

PyObject* py_spi_replay_ReplayObjectAction_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("ReplayObjectAction", args);
    return pyo;
}

static PyTypeObject* ReplayObjectAction_PyObjectType()
{
    static PyTypeObject* typeObject = NULL;
    if (typeObject)
        return typeObject;

    static PyGetSetDef properties[] =
    {
        {"inputs", (getter)(spi_py_object_getter), NULL, NULL,
            (void*) "inputs"},
        {NULL} // sentinel
    };

    static PyMethodDef methods[] =
    {
        {"Coerce", (PyCFunction)py_spi_replay_ReplayObjectAction_Coerce, METH_VARARGS | METH_STATIC,
            "Coerce ReplayObjectAction from arbitrary value"},
        {NULL, NULL, 0, NULL} // sentinel
    };

    typeObject = spi::pyMakeTypeObject(
        "spi_replay.ReplayObjectAction",
        properties,
        methods,
        false,
        "__init__(self, inputs)",
        "ReplayAction",
        Py_tp_init, (void*)py_spi_replay_ReplayObjectAction_init,
        0);

    return typeObject;
}


SPI_DECLARE_OBJECT_CLASS(ReplayCodeGenerator_python_delegate);

class ReplayCodeGenerator_python_delegate : public spi_replay::ReplayCodeGenerator
{
public:
    // IMPLEMENTS spi::Object
    spi::ObjectType* get_object_type() const
    {
        return py_object_type.get();
    }

    // IMPLEMENTS spi::Object
    void to_map(spi::IObjectMap* objMap, bool publicOnly) const
    {
        spi::pyObjectToMap(self, objMap, publicOnly);
    }

    static spi::Object* Constructor(PyObject* pyo)
    {
        return new ReplayCodeGenerator_python_delegate(pyo);
    }

    ~ReplayCodeGenerator_python_delegate()
    {
        if (Py_IsInitialized())
        {
            spi::PyInterpreterLock lock;
            Py_XDECREF(self);
        }
    }

    void GenerateFunction(
        const spi_replay::ReplayFunctionActionConstSP& action) const
    {
        spi::PyInterpreterLock lock;
        PyObjectSP the_method = spi::pyoShare(PyObject_GetAttrString(self, "GenerateFunction"));

        std::vector<spi::Value> input_values;
        input_values.push_back(spi::Object::to_value(action));
        PyObjectSP method_args = pyTupleFromValueVector(input_values);

        PyObject* pyo_output = PyObject_CallObject(the_method.get(), method_args.get());
        if (!pyo_output)
            spi::pyPythonExceptionHandler();
        PyObjectSP pyo_output_sp = spi::pyoShare(pyo_output);
        return;
    }

    void GenerateObject(
        const spi_replay::ReplayObjectActionConstSP& action) const
    {
        spi::PyInterpreterLock lock;
        PyObjectSP the_method = spi::pyoShare(PyObject_GetAttrString(self, "GenerateObject"));

        std::vector<spi::Value> input_values;
        input_values.push_back(spi::Object::to_value(action));
        PyObjectSP method_args = pyTupleFromValueVector(input_values);

        PyObject* pyo_output = PyObject_CallObject(the_method.get(), method_args.get());
        if (!pyo_output)
            spi::pyPythonExceptionHandler();
        PyObjectSP pyo_output_sp = spi::pyoShare(pyo_output);
        return;
    }

    // RE-IMPLEMENTS spi::Object
    void* get_py_object() const
    {
        return (void*)self;
    }

private:
    PyObject* self;
    spi::ObjectTypeSP py_object_type;

    ReplayCodeGenerator_python_delegate(PyObject* pyo) : self(pyo), py_object_type()
    {
        if (!self)
            throw std::runtime_error("Attempt to wrap NULL pointer in 'ReplayCodeGenerator_python_delegate'");
        py_object_type = spi::PythonService::FindRegisteredPythonClass(Py_TYPE(self));
        Py_INCREF(self);
    }

};


static int py_spi_replay_ReplayCodeGenerator_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    try
    {
        throw spi::RuntimeError("Cannot construct class of type %s", "ReplayCodeGenerator");
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

PyObject* py_spi_replay_ReplayCodeGenerator_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("ReplayCodeGenerator", args);
    return pyo;
}

PyObject* py_spi_replay_ReplayCodeGenerator_GenerateFunction(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "ReplayCodeGenerator.GenerateFunction");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        const spi::InputValues& iv = spi::pyGetInputValues(func, args, kwargs, self);
        spi::Value output = spi::CallInContext(func, iv, get_input_context());
        Py_RETURN_NONE;
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

PyObject* py_spi_replay_ReplayCodeGenerator_GenerateObject(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "ReplayCodeGenerator.GenerateObject");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        const spi::InputValues& iv = spi::pyGetInputValues(func, args, kwargs, self);
        spi::Value output = spi::CallInContext(func, iv, get_input_context());
        Py_RETURN_NONE;
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

static PyTypeObject* ReplayCodeGenerator_PyObjectType()
{
    static PyTypeObject* typeObject = NULL;
    if (typeObject)
        return typeObject;

    static PyMethodDef methods[] =
    {
        {"Coerce", (PyCFunction)py_spi_replay_ReplayCodeGenerator_Coerce, METH_VARARGS | METH_STATIC,
            "Coerce ReplayCodeGenerator from arbitrary value"},
        {"GenerateFunction", (PyCFunction)py_spi_replay_ReplayCodeGenerator_GenerateFunction, METH_VARARGS | METH_KEYWORDS,
            "GenerateFunction(self, action)"},
        {"GenerateObject", (PyCFunction)py_spi_replay_ReplayCodeGenerator_GenerateObject, METH_VARARGS | METH_KEYWORDS,
            "GenerateObject(self, action)"},
        {NULL, NULL, 0, NULL} // sentinel
    };

    typeObject = spi::pyMakeTypeObject(
        "spi_replay.ReplayCodeGenerator",
        0,
        methods,
        true,
        "",
        nullptr,
        Py_tp_init, (void*)py_spi_replay_ReplayCodeGenerator_init,
        0);

    return typeObject;
}


static int py_spi_replay_ReplayLog_init(SpiPyObject* self, PyObject* args, PyObject* kwargs)
{
    try
    {
        throw spi::RuntimeError("Cannot construct class of type %s", "ReplayLog");
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

PyObject* py_spi_replay_ReplayLog_Coerce(PyObject* self, PyObject* args)
{
    PyObject* pyo = get_python_service()->ObjectCoerce("ReplayLog", args);
    return pyo;
}

PyObject* py_spi_replay_ReplayLog_generateCode(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "ReplayLog.generateCode");
    try
    {
        if (!func)
            func = get_function_caller(py_timer.Name());

        const spi::InputValues& iv = spi::pyGetInputValues(func, args, kwargs, self);
        spi::Value output = spi::CallInContext(func, iv, get_input_context());
        Py_RETURN_NONE;
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

PyObject* py_spi_replay_ReplayLog_Read(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static spi::FunctionCaller* func = 0;
    spi::PythonTimer py_timer(get_python_service(), "ReplayLog.Read");
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

static PyTypeObject* ReplayLog_PyObjectType()
{
    static PyTypeObject* typeObject = NULL;
    if (typeObject)
        return typeObject;

    static PyGetSetDef properties[] =
    {
        {"items", (getter)(spi_py_object_getter), NULL, NULL,
            (void*) "items"},
        {NULL} // sentinel
    };

    static PyMethodDef methods[] =
    {
        {"Coerce", (PyCFunction)py_spi_replay_ReplayLog_Coerce, METH_VARARGS | METH_STATIC,
            "Coerce ReplayLog from arbitrary value"},
        {"generateCode", (PyCFunction)py_spi_replay_ReplayLog_generateCode, METH_VARARGS | METH_KEYWORDS,
            "generateCode(self, generator)"},
        {"Read", (PyCFunction)py_spi_replay_ReplayLog_Read, METH_VARARGS | METH_KEYWORDS | METH_STATIC,
            "Read(infilename)"},
        {NULL, NULL, 0, NULL} // sentinel
    };

    typeObject = spi::pyMakeTypeObject(
        "spi_replay.ReplayLog",
        properties,
        methods,
        false,
        "",
        nullptr,
        Py_tp_init, (void*)py_spi_replay_ReplayLog_init,
        0);

    return typeObject;
}


void py_spi_replay_replay_update_functions(spi::PythonService* svc)
{
    svc->SetNamespace("");

    svc->AddClass("ReplayAction", "ReplayAction",
        ReplayAction_PyObjectType());

    svc->AddClass("ReplayFunctionAction", "ReplayFunctionAction",
        ReplayFunctionAction_PyObjectType());

    svc->AddClass("ReplayObjectAction", "ReplayObjectAction",
        ReplayObjectAction_PyObjectType());

    svc->AddClass("ReplayCodeGenerator", "ReplayCodeGenerator",
        ReplayCodeGenerator_PyObjectType());

    svc->AddDelegateClass("ReplayCodeGenerator", ReplayCodeGenerator_python_delegate::Constructor);

    svc->AddClass("ReplayLog", "ReplayLog",
        ReplayLog_PyObjectType());
}

