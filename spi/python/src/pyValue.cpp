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
/*
***************************************************************************
** pyValue.hpp
**
** Mapping between spi::Value and PyObject
***************************************************************************
*/

#include <spi/python/pyValue.hpp>
#include <spi/python/pyUtil.hpp>
#include <spi/python/pyInput.hpp>
#include <spi/python/pyOutput.hpp>
#include <spi/python/pyDate.hpp>
#include <spi/python/pyService.hpp>
#include <spi/python/pyObject.hpp>
#include <spi/python/pyObjectMap.hpp>

#include <spi/python/pyObject.h>

#include <spi/Array.hpp>
#include <spi/ObjectMap.hpp>
#include <spi_util/Utils.hpp>

#include "py2to3.hpp"

SPI_BEGIN_NAMESPACE

using spi_util::IntegerCast;

PyObject* pyoFromMultiValue(const Value& value, size_t numVars)
{
    SPI_PRE_CONDITION(numVars >= 1);

    if (numVars > 1)
    {
        IArrayConstSP values = value.getArray();
        if (values->size() != numVars)
            throw RuntimeError("array size mismatch");

        PyObjectSP tuple = pyTupleFromValueVector(values->getVector());

        return pyoRelease(tuple);
    }

    return pyoFromValue(value);
}

PyObject* pyoFromValue(const Value& value)
{
    switch(value.getType())
    {
    case Value::UNDEFINED:
        Py_RETURN_NONE;
    case Value::CHAR:
        return pyoFromChar(value.getChar());
    case Value::SHORT_STRING:
    case Value::STRING:
        return pyoFromString(value.getString());
    case Value::INT:
        return pyoFromInt(value.getInt());
    case Value::DOUBLE:
        return pyoFromDouble(value.getDouble());
    case Value::BOOL:
        return pyoFromBool(value.getBool());
    case Value::DATE:
        return pyoFromDate(value.getDate());
    case Value::DATETIME:
        return pyoFromDateTime(value.getDateTime());
    case Value::MAP:
        return pyoFromMap(value.getMap());
    case Value::OBJECT:
        return PythonService::MakeObjectOfType(value);
    case Value::OBJECT_REF:
        return pyoFromInt(value.getObjectRef());
    case Value::ARRAY:
    {
        IArrayConstSP a = value.getArray();
        std::vector<size_t> dims = a->dimensions();
        if (dims.size() == 1)
        {
            return pyoMakeArray(a->getVector(), pyoFromValue);
        }
        else if (dims.size() == 2)
        {
            return pyoMakeMatrix(dims[0], dims[1], a->getVector(), pyoFromValue);
        }
        else
        {
            SPI_THROW_RUNTIME_ERROR("Dimensions of array must be 1 or 2");
        }
    }
    case Value::ERROR:
        PyErr_SetString(PyExc_Exception, value.getError().c_str());
        throw PyException();
    default:
        break;
    }
    PyErr_Format(PyExc_TypeError,
        "Cannot convert %s to PyObject", value.toString().c_str());
    throw PyException();
}

Value pyoToValue(PyObject* pyo)
{
    static char routine[] = "PyObjectToValue";

    if (pyo == Py_None)
    {
        return Value();
    }
    else if (pyo == Py_False)
    {
        return Value (false);
    }
    else if (pyo == Py_True)
    {
        return Value (true);
    }
    else if (PyInt_Check(pyo))
    {
        long value = PyInt_AsLong(pyo);
        return Value(IntegerCast<int>(value));
    }
    else if (PyFloat_Check(pyo))
    {
        return Value (PyFloat_AsDouble(pyo));
    }
    else if (pyoIsString(pyo))
    {
        return Value (pyoToString(pyo));
    }
#if PY_MAJOR_VERSION < 3
    else if (PyUnicode_Check(pyo))
    {
        PyObject* tmp = PyUnicode_AsLatin1String(pyo);
        if (!tmp)
            throw PyException();
        spi::Value value(PyString_AsString(tmp));
        Py_DECREF(tmp);
        return value;
    }
#endif
    // we must check for DateTime first since Python appears
    // to define that datetime.datetime is a sub-class of datetime.date
    else if (pyIsDateTime(pyo))
    {
        return Value(pyToDateTime(pyo));
    }
    else if (pyIsDate(pyo))
    {
        return Value (pyToDate(pyo));
    }
    else if (PyObject_TypeCheck(pyo, SpiPyObjectType()))
    {
        spi::ObjectConstSP obj = spiPyObjectGetObject((SpiPyObject*)pyo);
        return Value(obj);
    }
    else if (PyList_Check(pyo))
    {
        Py_ssize_t size = PyList_Size(pyo);
        std::vector<Value> values;
        for (Py_ssize_t i = 0; i < size; ++i)
        {
            PyObject* item  = PyList_GetItem(pyo, i);
            if (!item)
                throw PyException();
            values.push_back(pyoToValue(item));
        }
        return Value(IArrayConstSP(new ValueArray(values, values.size())));
    }
    else if (PyTuple_Check(pyo))
    {
        Py_ssize_t size = PyTuple_Size(pyo);
        std::vector<Value> values;
        for (Py_ssize_t i = 0; i < size; ++i)
        {
            PyObject* item  = PyTuple_GetItem(pyo, i);
            if (!item)
                throw PyException();
            values.push_back(pyoToValue(item));
        }
        return Value(IArrayConstSP(new ValueArray(values, values.size())));
    }
    else if (PyDict_Check(pyo))
    {
        MapSP m(new Map("Map"));
        ObjectMap om(m);

        pyDictToObjectMap(pyo, &om);

        return Value(MapObject::Make(m));
    }
    else
    {
        double v = PyFloat_AsDouble(pyo);
        // this method returns -1.0 on failure so need to test for errors
        //
        // PyErr_Occurred returns a borrowed reference - hence only need
        // to test its output for NULL and do need to Py_DECREF non-NULLs
        if (PyErr_Occurred() == NULL)
            return v;
        PyErr_Format(PyExc_TypeError,
                     "Cannot convert python type '%s' to value",
                     pyo->ob_type->tp_name);
        throw PyException();
    }
}

InputValues pyGetInputValues(
    FunctionCaller* func,
    PyObject* args,
    PyObject* kwargs,
    PyObject* self)
{
    InputValues iv(func->name);

    std::vector<PyObject*> vargs = pyTupleToVector(args);
    if (kwargs)
    {
        throw RuntimeError("keyword arguments not supported at this time");
    }

    if (self)
    {
        std::vector<PyObject*> vargsWithSelf;
        vargsWithSelf.push_back(self);
        vargsWithSelf.insert(vargsWithSelf.end(), vargs.begin(), vargs.end());
        vargs.swap(vargsWithSelf);
    }

    if (vargs.size() > func->nbArgs)
    {
        throw RuntimeError("Too many parameters provided for function %s",
            func->name);
    }

    for (size_t i = 0; i < func->nbArgs; ++i)
    {
        if (i < vargs.size())
        {
            iv.AddValue(func->args[i], pyoToValue(vargs[i]));
        }
        else
        {
            iv.AddValue(func->args[i], Value());
        }
    }

    return iv;
}

std::vector<Value> pyTupleToValueVector(
    const char* name,
    size_t nbArgs,
    PyObject* args)
{
    std::vector<PyObject*> vargs = pyTupleToVector(args);

    if (vargs.size() > nbArgs)
    {
        throw RuntimeError("Too many parameters provided for function %s",
            name);
    }

    std::vector<Value> values;
    for (size_t i = 0; i < nbArgs; ++i)
    {
        if (i < vargs.size())
        {
            values.push_back(pyoToValue(vargs[i]));
        }
        else
        {
            values.push_back(Value());
        }
    }

    return values;
}

PyObjectSP pyTupleFromValueVector(
    const std::vector<Value>& values)
{
    PyObject* pyTuple = PyTuple_New((Py_ssize_t)values.size());
    PyObjectSP output = pyoShare(pyTuple);

    for (Py_ssize_t i = 0; i < (Py_ssize_t)values.size(); ++i)
    {
        PyObject* pyo = pyoFromValue(values[i]);
        SPI_POST_CONDITION(pyo != NULL);
        // PyTuple_SET_ITEM steals a reference to pyo
        // which means that we no longer need to care about pyo
        PyTuple_SET_ITEM(pyTuple, i, pyo);
    }
    return output;
}

SPI_END_NAMESPACE
