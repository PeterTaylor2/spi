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
** pyObject.cpp
**
** Python equivalent to spi::Object
***************************************************************************
*/

#include <spi/python/pyObject.h>
#include <spi/python/pyObject.hpp>

#include <spi/python/pyValue.hpp>
#include <spi/python/pyUtil.hpp>
#include <spi/python/pyOutput.hpp>
#include <spi/python/pyService.hpp>
#include <spi/Value.hpp>
#include <spi/Object.hpp>
#include <spi/ObjectPut.hpp>
#include <spi/ObjectHelper.hpp>
#include <spi/InputContext.hpp>

#include "py2to3.hpp"
#include "pyInput.hpp"

/*
***************************************************************************
** Implementation of SpiPyObject and SpiPyConstObject.
***************************************************************************
*/

/* tp_dealloc function */
void spi_py_object_dealloc (SpiPyObject *pyo)
{
    if (pyo)
    {
        if (pyo->obj)
        {
            decRefCount(pyo->obj);
            PyObject_Del(pyo);
        }
    }
}

PyObject* spi_py_object_getter (SpiPyObject* pyo, char* name)
{
    // note that if we have sub-classed spi.Object (directly or indirectly) in Python
    // then we only get as far as this routine if the name is not defined in the
    // object's dictionary
    try
    {
        spi::ObjectConstSP obj = spi::spiPyObjectGetObject(pyo);
        spi::Value value = spi::ObjectGet(obj, name);
        return pyoFromValue(value);
    }
    catch (spi::PyException&)
    {
        return NULL;
    }
    catch (std::exception &e)
    {
        PyErr_SetString(PyExc_AttributeError, e.what());
        return NULL;
    }
}

int spi_py_object_setter (SpiPyObject* pyo, PyObject* value, char* name)
{
    try
    {
        spi::PythonService* svc = spi::PythonService::CommonService();

        if (pyo->obj)
        {
            // this operation creates a new spi::Object on success
            spi::ObjectConstSP obj = spi::ObjectPut(
                spi::ObjectConstSP(pyo->obj), name,
                spi::pyoToValue(value), svc->GetInputContext());

            // we put the newly created spi::Object back into the SpiPyObject
            // but since we are dealing with raw pointers we need to increment
            // and decrement the reference counts explicitly
            const spi::Object* pobj = spi::incRefCount(obj.get());
            spi::decRefCount(pyo->obj);
            pyo->obj = pobj;
        }
        else
        {
            // we don't seem to get to here if we originally subclassed
            // spi.Object - it seems that the dictionary of the Python
            // object is used instead
            //
            // this would otherwise have been very difficult to handle
            //
            // however it also notes that 'typename' is defined as an
            // attribute of spi.Object so it doesn't let you override
            // 'typename' reporting an AttributeError Exception as follows:
            // attribute 'typename' of 'spi.Object' objects is not writable
            //
            // this is also very re-assuring behaviour
            SPI_THROW_RUNTIME_ERROR("pyo->obj undefined");
        }

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
}

// here we are expecting to be editing Map
// hence we do it in place rather than via ObjectPut which is quite an overhead
int spi_py_object_map_setter(SpiPyObject* pyo, PyObject* value, char* name)
{
    try
    {
        SPI_NOT_IMPLEMENTED;

        //spi::PythonService* svc = spi::PythonService::CommonService();

        //// this operation creates a new spi::Object on success
        //spi::ObjectConstSP obj = spi::ObjectPut(
        //    spi::ObjectConstSP(pyo->obj), name,
        //    spi::pyoToValue(value), svc->GetInputContext());

        //// we put the newly created spi::Object back into the SpiPyObject
        //// but since we are dealing with raw pointers we need to increment
        //// and decrement the reference counts explicitly
        //const spi::Object* pobj = spi::incRefCount(obj.get());
        //spi::decRefCount(pyo->obj);
        //pyo->obj = pobj;

        //return 0;
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
}

PyObject* spi_py_object_getattro(SpiPyObject* pyo, PyObject* name)
{
    try
    {
        // use generic method (will find methods and static attributes)
        PyObject* attr = PyObject_GenericGetAttr((PyObject *)pyo, name);
        if (attr)
            return attr;

        // failing that we can try dynamic attributes
        // this function is only invoked if there are dynamic attributes
        PyErr_Clear(); // PyObject_GenericGetAttr will have set AttributeError

        // cName will be a copy of the buffer inside the PyObject* name
        // returns NULL if name is not a string
        std::string cName = spi::pyoToString(name);

        spi::Value value = spi::ObjectGet(pyo->obj, cName.c_str());
        return pyoFromValue(value);
    }
    catch (spi::PyException&)
    {
        return NULL;
    }
    catch (std::exception &e)
    {
        PyErr_SetString(PyExc_AttributeError, e.what());
        return NULL;
    }
}

PyObject* spi_py_object_get (PyObject* self, PyObject* args)
{
    try
    {
        spi::PythonService* svc = spi::PythonService::CommonService();

        return svc->ObjectGet(args);
    }
    catch (spi::PyException&)
    {
        return NULL;
    }
    catch (std::exception &e)
    {
        return spi::pyExceptionHandler(e.what());
    }
}

PyObject* spi_py_object_put (PyObject* self, PyObject* args)
{
    try
    {
        spi::PythonService* svc = spi::PythonService::CommonService();

        return svc->ObjectPut(args);
    }
    catch (spi::PyException&)
    {
        return NULL;
    }
    catch (std::exception &e)
    {
        return spi::pyExceptionHandler(e.what());
    }
}

PyObject* spi_py_object_update_meta_data(PyObject* self, PyObject* args)
{
    try
    {
        spi::PythonService* svc = spi::PythonService::CommonService();

        return svc->ObjectUpdateMetaData(args);
    }
    catch (spi::PyException&)
    {
        return NULL;
    }
    catch (std::exception& e)
    {
        return spi::pyExceptionHandler(e.what());
    }
}

PyObject* spi_py_object_copy(PyObject* self)
{
    try
    {
        return spi::PythonService::ObjectCopy(self, false);
    }
    catch (spi::PyException&)
    {
        return NULL;
    }
    catch (std::exception& e)
    {
        return spi::pyExceptionHandler(e.what());
    }
}

PyObject* spi_py_object_deepcopy(PyObject* self, PyObject* ignored)
{
    try
    {
        return spi::PythonService::ObjectCopy(self, true);
    }
    catch (spi::PyException&)
    {
        return NULL;
    }
    catch (std::exception& e)
    {
        return spi::pyExceptionHandler(e.what());
    }

}

void investigate_null_object(PyObject* pyo)
{
    PyObjectSP dir = spi::pyoShare(PyObject_Dir(pyo));
    if (PySequence_Check(dir.get()) == 1)
    {
        Py_ssize_t nbItems = PySequence_Size(dir.get());
        for (Py_ssize_t i = 0; i < nbItems; ++i)
        {
            PyObjectSP name(PySequence_GetItem(dir.get(), i));
            PyObjectSP value(PyObject_GetAttr(pyo, name.get()));
            std::string sname = spi::pyoToString(name.get());
            int is_method = PyMethod_Check(value.get());
            printf("dir: %s = %s (%d)\n", sname.c_str(), value->ob_type->tp_name,
                is_method);
        }
    }
    else
    {
        throw spi::RuntimeError("PyObject_Dir did not return sequence");
    }

    throw spi::RuntimeError("NULL spi::Object");
}

PyObject* spi_py_object_to_string(PyObject* self, PyObject* args)
{
    try
    {
        spi::PythonService* svc = spi::PythonService::CommonService();
        return svc->ObjectToString(self, args);
    }
    catch (spi::PyException&)
    {
        return NULL;
    }
    catch (std::exception &e)
    {
        return spi::pyExceptionHandler(e.what());
    }
}

PyObject* spi_py_object_to_file(PyObject* self, PyObject* args)
{
    try
    {
        spi::PythonService* svc = spi::PythonService::CommonService();
        return svc->ObjectToFile(self, args);
    }
    catch (spi::PyException&)
    {
        return NULL;
    }
    catch (std::exception &e)
    {
        return spi::pyExceptionHandler(e.what());
    }
}

PyObject* spi_py_object_from_string(PyObject* self, PyObject* args)
{
    try
    {
        spi::PythonService* svc = spi::PythonService::CommonService();
        return svc->ObjectFromString(args);
    }
    catch (spi::PyException&)
    {
        return NULL;
    }
    catch (std::exception &e)
    {
        return spi::pyExceptionHandler(e.what());
    }
}

PyObject* spi_py_object_from_file(PyObject* self, PyObject* args)
{
    try
    {
        spi::PythonService* svc = spi::PythonService::CommonService();
        return svc->ObjectFromFile(args);
    }
    catch (spi::PyException&)
    {
        return NULL;
    }
    catch (std::exception &e)
    {
        return spi::pyExceptionHandler(e.what());
    }
}

PyObject* spi_py_object_sha_1(PyObject* self)
{
    try
    {
        spi::PythonService* svc = spi::PythonService::CommonService();
        return svc->ObjectSHA(self, 1);
    }
    catch (spi::PyException&)
    {
        return NULL;
    }
    catch (std::exception &e)
    {
        return spi::pyExceptionHandler(e.what());
    }
}

PyObject* spi_py_object_sha_256(PyObject* self)
{
    try
    {
        spi::PythonService* svc = spi::PythonService::CommonService();
        return svc->ObjectSHA(self, 256);
    }
    catch (spi::PyException&)
    {
        return NULL;
    }
    catch (std::exception &e)
    {
        return spi::pyExceptionHandler(e.what());
    }
}

PyObject* spi_py_object_register_class(PyObject* self, PyObject* args)
{
    try
    {
        spi::PythonService* svc = spi::PythonService::CommonService();
        svc->RegisterPythonClass(args);
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
}

static PyObject* spi_py_object_reduce(SpiPyObject* self)
{
    try
    {
        if (!self->obj)
            investigate_null_object((PyObject*)self);

        spi::ObjectConstSP object(self->obj);

        std::string output = object->to_string("", "ACC;NOINDENT");
        const spi::Service* svc = object->get_service();
        if (!svc)
        {
            SPI_THROW_RUNTIME_ERROR("No service for object of type '" << object->get_class_name() << "'");
        }

        PyObject* stringRep = spi::pyoFromString(output);

        // we need to return a 2-tuple
        // element 1 is the constructor that takes the arguments of element 2
        //
        // we want element 1 to be Object.from_string which is a static method
        // we want element 2 to be the string

        PyObject* args = PyTuple_New(1);
        PyTuple_SetItem(args, 0, stringRep);

        PyObject* out = PyTuple_New(2);

        // we need to be able to map from the object to its service
        // then we must ensure that each service has object_from_string function
        PyObject* module = PyImport_ImportModule(svc->get_namespace().c_str());
        PyObject* func = PyObject_GetAttrString(module, "object_from_string");

        PyTuple_SetItem(out, 0, func);
        PyTuple_SetItem(out, 1, args);

        // all other python references have been stolen by PyTuple_SetItem
        Py_DECREF(module);

        return out;
    }
    catch (spi::PyException&)
    {
        return NULL;
    }
    catch (std::exception &e)
    {
        return spi::pyExceptionHandler(e.what());
    }
}

static PyObject* spi_py_object_as_value(SpiPyObject* self)
{
    try
    {
        if (!self->obj)
            investigate_null_object((PyObject*)self);

        spi::ObjectConstSP object(self->obj);
        spi::Value value = object->as_value();

        if (value.getType() == spi::Value::UNDEFINED)
        {
            SPI_THROW_RUNTIME_ERROR("Object of type " << object->get_class_name()
                << " cannot be represented as a simple value");
        }

        return spi::pyoFromValue(value);
    }
    catch (spi::PyException&)
    {
        return NULL;
    }
    catch (std::exception &e)
    {
        return spi::pyExceptionHandler(e.what());
    }
}

PyTypeObject* SpiPyObjectType(/*const char* ns*/)
{
    static char* className = "spi.Object";
    static PyGetSetDef properties[] = {
        {(char*)"typename", (getter)spi_py_object_getter, NULL,
            (char*)"The typename of the Object.",
            (void*)"typename"},
        {(char*)"meta_data", (getter)spi_py_object_getter, NULL,
            (char*)"Meta data for this Object (not always present)",
            (void*)"meta_data"},
        {NULL}
    };

    // note the initial implementation used &PyType_Type but only for python3
    // when we moved to using PyVarObject_HEAD_INIT we lost this feature
    static PyTypeObject typeObject = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "IObject",
        sizeof(SpiPyObject),
        0,
        (destructor)spi_py_object_dealloc
    };
    static PyMethodDef methods[] = {
        {"to_string", (PyCFunction)spi_py_object_to_string, METH_VARARGS,
            "to_string(self, format=None, options=None, meta_data=None, merge_meta_data=False)\n\n"
            "Return the serialised object as a single string."
        },
        {"to_file", (PyCFunction)spi_py_object_to_file, METH_VARARGS,
            "to_file(self, filename, format=None, options=None, meta_data=None, merge_meta_data=False)\n\n"
            "Write the serialised object to file."
        },
        {"from_string", (PyCFunction)spi_py_object_from_string,
            METH_VARARGS | METH_STATIC,
            "from_string(string)\n\n"
            "Create an object from the serialised string format."
        },
        {"from_file", (PyCFunction)spi_py_object_from_file,
            METH_VARARGS | METH_STATIC,
            "from_file(filename)\n\n"
            "Create an object from the serialised string format."
        },
        {"__copy__", (PyCFunction)spi_py_object_copy, METH_NOARGS,
            "__copy__(self)\n\n"
            "Shallow copy of the object - internal objects are not copied."
        },
        {"__deepcopy__", (PyCFunction)spi_py_object_deepcopy, METH_VARARGS,
            "__deepcopy__(self, memo(ignored))\n\n"
            "Deep copy of the object - internal objects are also copied."
        },
        {"sha_1", (PyCFunction)spi_py_object_sha_1, METH_NOARGS,
            "sha_1(self)\n\n"
            "Return the SHA1 hash for an object as a string."
        },
        {"sha_256", (PyCFunction)spi_py_object_sha_256, METH_NOARGS,
            "sha_256(self)\n\n"
            "Return the SHA256 hash for an object as a string."
        },
        { "register_class", (PyCFunction)spi_py_object_register_class,
            METH_VARARGS | METH_STATIC,
            "register_class(type, objectName)\n\n"
            "Registers a class sub-classed from spi.Object in order to support de-serialization."
        },
        {"__reduce__", (PyCFunction)spi_py_object_reduce, METH_NOARGS,
            "For pickle/unpickle"
        },
        {"as_value", (PyCFunction)spi_py_object_as_value, METH_NOARGS,
            "For objects with a simple representation"
        },

        {NULL}  /* Sentinel */
    };

    static bool initialised = false;

    if (!initialised)
    {
#if PY_MAJOR_VERSION > 3 || (PY_MAJOR_VERSION == 3 && PY_MINOR_VERSION >= 9)
        Py_SET_TYPE(&typeObject, &PyType_Type);
#else
        Py_TYPE(&typeObject)  = &PyType_Type;
#endif
        typeObject.tp_name    = className;
        typeObject.tp_flags   = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
        typeObject.tp_methods = methods;
        typeObject.tp_getset  = properties;
        typeObject.tp_new     = PyType_GenericNew;

        if (PyType_Ready(&typeObject) < 0)
            return NULL;

        initialised = true;
    }

    return &typeObject;
}

SPI_BEGIN_NAMESPACE

const Object* pyInitConstObject(
    PyObject* args,
    PyObject* kwargs,
    FunctionCaller* func,
    ObjectType* type)
{
    static InputContext* context = InputContext::PythonContext();

    const InputValues& iv = pyGetInputValues(func, args, kwargs);
    Value              ov = CallInContext(func, iv, context);
    ObjectConstSP      output;

    if (ov.getType() == Value::OBJECT)
    {
        output = ov.getObject();
    }
    else
    {
        // use case is when the object is returned by the function caller as a value
        // however here we are insisting that we must return an Object
        output = type->coerce_from_value(ov);
    }

    if (!type->is_instance(output))
    {
        throw RuntimeError(
           "Constructor for %s has not returned object of that type",
            type->get_class_name());
    }

    return spi::incRefCount(output.get());
}

const Object* pyInitConstObject(PyObject* args, FunctionCaller* func, ObjectType* type)
{
    return pyInitConstObject(args, 0, func, type);
}

ObjectConstSP spiPyObjectGetObject(SpiPyObject* pyo)
{
    // usual case is that pyo->obj is defined
    if (pyo->obj)
    {
        return ObjectConstSP(pyo->obj);
    }

    // this is a temporary object - it might own the PyObject but it will go
    // out of scope before we return to the calling environment at which point
    // it will release the PyObject

    ObjectConstSP obj = PythonService::CommonService()->ConstructDelegate(
        (PyObject*)pyo);

    return obj;
}

SPI_END_NAMESPACE
