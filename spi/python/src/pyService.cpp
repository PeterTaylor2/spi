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
#include "pyService.hpp"

#include "pyUtil.hpp"
#include "pyInput.hpp"
#include "pyOutput.hpp"
#include "pyValue.hpp"
#include "pyObject.h"
#include "pyObjectMap.hpp"

#include <spi_util/FileUtil.hpp>

/*
***************************************************************************
** pyService.cpp
**
** Defines the Python service class. This is linked to the Service class
** for the underlying C++ outer library.
***************************************************************************
*/

SPI_BEGIN_NAMESPACE

std::map<PyTypeObject*, ObjectTypeSP> PythonService::g_indexPythonObjectType;

class Object_python_delegate : public Object
{
public:

    // IMPLEMENTS Object
    ObjectType* get_object_type() const
    {
        return py_object_type.get();
    }

    // IMPLEMENTS Object
    void to_map(IObjectMap* objMap, bool publicOnly) const
    {
        pyObjectToMap(self, objMap, publicOnly);
    }

    static Object* Constructor(PyObject* pyo)
    {
        return new Object_python_delegate(pyo);
    }

    ~Object_python_delegate()
    {
        if (Py_IsInitialized())
        {
            PyInterpreterLock lock;
            Py_XDECREF(self);
        }
        else
        {   
            // if we are in this state then probably Python is closing down
            //
            // it is likely that the pointer is owned by some C++ object
            // which is currently being deleted - but calling significant
            // python functions will cause a crash on exit
            bool memoryInUse = true;
        }
    }

    // RE-IMPLEMENTS Object
    void* get_py_object() const
    {
        return (void*)self;
    }

private:
    PyObject* self;
    ObjectTypeSP py_object_type;

    Object_python_delegate(PyObject* pyo) : self(pyo), py_object_type()
    {
        if (!self)
            throw std::runtime_error("Attempt to wrap NULL pointer in 'Object_python_delegate'");

        py_object_type = PythonService::FindRegisteredPythonClass(Py_TYPE(self));

        Py_INCREF(self);
    }

};

PythonClass::PythonClass(
    const std::string& name,
    const std::string& objectName,
    PyTypeObject* pto)
    :
    m_name(name),
    m_objectName(objectName),
    m_pto(pto)
{}

PythonNamespace::PythonNamespace(const char* ns)
    :
    m_ns(ns),
    m_functions(),
    m_classes()
{}

PythonService::PythonService(const char* moduleName, const ServiceSP& service)
    :
    m_moduleName(moduleName),
    m_service(service),
    m_module(0),
    m_inputContext(InputContext::PythonContext()),
    m_functions(),
    m_classes(),
    m_baseObjectType(SpiPyObjectType()),
    m_namespaces(),
    m_currentNamespace(0)
{
    m_service->add_client("PYTHON");
}

#if 0
void PythonService::DebugModule(const char* state)
{
    PyObject* moduleDict = PyModule_GetDict(m_module);

    Py_ssize_t pos = 0;
    PyObject* key = NULL;
    PyObject* value = NULL;

    char* moduleName = PyModule_GetName(m_module);

    std::cout << "\n" << "DebugModule(" << state << ")\n";
    std::cout << "module: " << moduleName << "\n" << std::endl;
    while (PyDict_Next(moduleDict, &pos, &key, &value))
    {
        std::string skey = pyoToString(key);
        std::cout << pos << ": " << skey << std::endl;

        std::string vstr = PyString_AsString(PyObject_Str(value));
        std::cout << "\t" << vstr << std::endl;
    }
}
#endif

void PythonService::SetNamespace(const char* ns)
{
    if (!ns || strlen(ns) == 0)
    {
        if (m_currentNamespace)
        {
            m_currentNamespace = 0;
        }
    }
    else
    {
        if (!m_currentNamespace || strcmp(m_currentNamespace->m_ns, ns) != 0)
        {
            PythonNamespace pns(ns);
            m_namespaces.push_back(pns);
            m_currentNamespace = &m_namespaces[m_namespaces.size()-1];
        }
    }
}

void PythonService::AddFunction(
    const char* name,
    PyCFunction func,
    const char* doc,
    int flags)
{
    PyMethodDef methodDef;

    methodDef.ml_name  = name;
    methodDef.ml_meth  = func;
    methodDef.ml_flags = flags;
    methodDef.ml_doc   = doc;

    if (m_currentNamespace)
    {
        m_currentNamespace->m_functions.push_back(methodDef);
    }
    else
    {
        m_functions.push_back(methodDef);
    }
}

void PythonService::AddClass(
    const std::string& name,
    const std::string& objectName,
    PyTypeObject* pyo,
    const char* baseClassName)
{
    if (m_indexClasses.count(objectName) > 0)
        SPI_THROW_RUNTIME_ERROR("Duplicate object name " << objectName);

    if (baseClassName)
    {
        // we can sub-class from another module
        // hence we must look in the common service for the base class
        pyo->tp_base = FindCommonClass(baseClassName);
    }
    else
    {
        pyo->tp_base = m_baseObjectType;
    }

    if (PyType_Ready(pyo) < 0)
        throw spi::PyException();

    m_indexClasses[objectName] = pyoShare((PyObject*)pyo, true);
    if (m_currentNamespace)
    {
        m_currentNamespace->m_classes.push_back(PythonClass(name, objectName, pyo));
    }
    else
    {
        m_classes.push_back(PythonClass(name, objectName, pyo));
    }
    if (!IsCommonService())
    {
        AddCommonClass(objectName, pyo);
    }
}

PyTypeObject* PythonService::FindCommonClass(
    const std::string& className)
{
    PythonService* commonService = CommonService();
    auto iter = commonService->m_indexClasses.find(className);

    if (iter == commonService->m_indexClasses.end())
    {
        SPI_THROW_RUNTIME_ERROR("Class name " << className << " undefined");
    }

    if (!PyType_Check(iter->second.get()))
        SPI_THROW_RUNTIME_ERROR("Type registered for " << className << " is not a python type object");

    return (PyTypeObject*)(iter->second.get());
}

void PythonService::AddCommonClass(
    const std::string& objectName,
    PyTypeObject* pyo)
{
    PythonService* commonService = CommonService();

    // we allow these to be overwritten - this allows a Python module which registers
    // Python classes with SPI to be re-imported
    commonService->m_indexClasses[objectName] = pyoShare((PyObject*)pyo, true);
}

void PythonService::AddDelegateClass(
    const std::string& className,
    DelegateObjectConstructor* constructor)
{
    PythonService* commonService = CommonService();

    std::string fullClassName = m_service->get_namespace() + "." + className;

    if (commonService->m_indexDelegateClasses.count(fullClassName))
    {
        throw RuntimeError("Delegate class %s already defined",
            fullClassName.c_str());
    }
    commonService->m_indexDelegateClasses[fullClassName] = constructor;
}

ObjectConstSP PythonService::ConstructDelegate(PyObject* pyo) const
{
    PyTypeObject* pyBaseClass = pyo->ob_type->tp_base;
    SPI_POST_CONDITION(pyBaseClass);
    std::string baseClassName = pyBaseClass->tp_name;

    DelegateObjectConstructor* constructor;

    auto iter = m_indexDelegateClasses.find(baseClassName);

    if (iter == m_indexDelegateClasses.end())
    {
        // must be a sub-type of the Python equivalent of Object
        // however it might not be direct sub-class
        PyTypeObject* pyObjectType = SpiPyObjectType();
        if (!PyType_IsSubtype(pyo->ob_type, pyObjectType))
        {
            SPI_THROW_RUNTIME_ERROR(pyo->ob_type->tp_name << " is not a sub-class of " << pyObjectType->tp_name);
        }
        constructor = Object_python_delegate::Constructor;
    }
    else
    {
        // sub-class of a regular c++ class which supports delegation
        constructor = iter->second;
    }

    Object* obj = constructor(pyo);
    return ObjectConstSP(obj);
}

PyObject* PythonService::ObjectCopy(PyObject* pyo, bool deepCopy)
{
    spi::Value value = spi::pyoToValue(pyo);
    spi::Value copy = spi::ObjectCopy(value, spi::InputContext::PythonContext(), deepCopy);
    return MakeObjectOfType(copy);
}

// we can use the IObjectMap to get the classname and hence the PyTypeObject
ObjectConstSP PythonService::PythonMakeFromMap(
    IObjectMap* m,
    ValueToObject& v2o)
{
    PyInterpreterLock lock;

    std::string className = m->ClassName();
    PyTypeObject* pyType = FindCommonClass(className);

    PyObjectSP method = pyoShare(PyObject_GetAttrString((PyObject*)pyType, "from_dict"));
    if (!method)
        SPI_THROW_RUNTIME_ERROR("from_dict not implemented in " << pyType->tp_name);

    PyObjectSP dict = pyObjectMapToDict(m, v2o);
    PyObjectSP args = pyoShare(PyTuple_New(1));

    PyTuple_SetItem(args.get(), 0, dict.get());

    PyObjectSP output = pyoShare(PyObject_CallObject(method.get(), args.get()));
    if (!output)
        pyPythonExceptionHandler();

    // don't check the name of the Python class - we can register it with a different name

    return CommonService()->ConstructDelegate(output.get());
}

bool PythonService::PythonTypeIsInstance(
    const ObjectConstSP& o)
{
    // we don't write C++ functions to take instances of a specific Python class
    // they are written to take instances of a specific C++ class
    // hence IsInstance can always reasonably return false

    return false;
}


void PythonService::RegisterPythonClass(PyObject* args)
{
    // args should be a 2-tuple consisting of a Python type and an object name
    // the object name is optional - it defaults to "py.<tp_name>"
    std::vector<PyObject*> vargs = pyTupleToVector(args);
    std::string className;

    switch (vargs.size())
    {
    case 2:
        className = pyoToString(vargs[1]);
        // FALLTHRU
    case 1:
        if (!PyType_Check(vargs[0]))
            SPI_THROW_RUNTIME_ERROR("Type is not a Python type");
        break;
    default:
        if (vargs.size() != 2)
            SPI_THROW_RUNTIME_ERROR("Expecting type and (optional) className");
    }
    
    PyTypeObject* pyType = (PyTypeObject*)vargs[0];
    if (className.empty())
        className = StringFormat("py.%s", pyType->tp_name);

    RegisterPythonClassDetails(className, pyType);
}

ObjectTypeSP PythonService::FindRegisteredPythonClass(PyTypeObject * pto)
{
    auto iter = g_indexPythonObjectType.find(pto);
    if (iter == g_indexPythonObjectType.end())
    {
        // attempt to automatically register
        //
        // this means that you won't be able to de-serialize this type if you 
        // didn't register it explicitly and haven't used any instances of
        // this type before attempting to de-serialize
        std::string className = StringFormat("py.%s", pto->tp_name);
        return RegisterPythonClassDetails(className, pto);
    }

    return iter->second;
}

ObjectTypeSP PythonService::RegisterPythonClassDetails(
    const std::string& className,
    PyTypeObject* pyType)
{
    PythonService* commonService = CommonService();
    PyTypeObject* pyTypeRoot = SpiPyObjectType();

    if (!PyType_IsSubtype(pyType, pyTypeRoot))
    {
        SPI_THROW_RUNTIME_ERROR(pyType->tp_name << " is not a sub-class of " << pyTypeRoot->tp_name);
    }

    // normally ObjectType's are static data for each generated class
    //
    // since we don't know which Python classes will be created we have to store
    // them as shared pointers within the Python service (to provide persistance)
    // and then register them as regular pointers in the C++ service
    ObjectTypeSP objectType(new ObjectType(
        className.c_str(),
        PythonService::PythonMakeFromMap,
        PythonService::PythonTypeIsInstance,
        false));

    // we use g_indexPythonObjectType to get from pytype to objectType
    // this is when we are serializing an object in order to get the correct typename
    g_indexPythonObjectType[pyType] = objectType;

    // for the common service we allow object types of the same name to be overwritten
    // we don't allow it for regular services
    commonService->m_service->add_object_type(objectType.get());

    // we need to be able to get from the className back to the pyType
    commonService->m_indexClasses[className] = pyoShare((PyObject*)pyType, true);

    // we must assume that PyTypeObject is persistent data
    // what if we re-import it? perhaps it is not in that case
    AddCommonClass(className, pyType);

    return objectType;
}

ServiceSP PythonService::GetService() const
{
    return m_service;
}

namespace {

    struct module_state {
        PyObject *error;
    };

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
    static struct module_state _state;
#endif

}


/**
 * Register all the functions and classes from the library.
 */
void PythonService::RegisterFunctions()
{
#if _DEBUG
    for (size_t i = 0; i < m_functions.size(); ++i)
    {
        std::cout << m_functions[i].ml_name << std::endl;
    }
#endif
    m_currentNamespace = 0;

    PyMethodDef sentinel;
    memset(&sentinel, 0, sizeof(PyMethodDef));
    m_functions.push_back(sentinel);


#if PY_MAJOR_VERSION >= 3

    m_moduleDef = {
        PyModuleDef_HEAD_INIT,
        m_moduleName,
        NULL,
        -1,
        &m_functions[0] };

    m_module = PyModule_Create(&m_moduleDef);
#else
    m_module = Py_InitModule3(m_moduleName, &m_functions[0], NULL);
#endif

    Py_INCREF(m_baseObjectType);
    PyModule_AddObject(m_module, "Object", (PyObject*)m_baseObjectType);

    size_t nbClasses = m_classes.size();
    for (size_t i = 0; i < nbClasses; ++i)
    {
        const std::string& className = m_classes[i].m_name;
        PyTypeObject* typeObject     = m_classes[i].m_pto;
        Py_INCREF(typeObject);
        PyModule_AddObject(
            m_module, className.c_str(), (PyObject*)(typeObject));
    }

    for (size_t i = 0; i < m_namespaces.size(); ++i)
    {
        PythonNamespace& pns = m_namespaces[i];

        pns.m_functions.push_back(sentinel);

#if PY_MAJOR_VERSION >= 3
        pns.m_moduleDef =
        {
            PyModuleDef_HEAD_INIT,
            pns.m_ns,
            NULL,
            -1,
            &pns.m_functions[0]
        };
        PyObject* module = PyModule_Create(&pns.m_moduleDef);
#else
        PyObject* module = Py_InitModule3(pns.m_ns, &pns.m_functions[0], NULL);
#endif

        size_t nbClasses = pns.m_classes.size();
        for (size_t j = 0; j < nbClasses; ++j)
        {
            const std::string& className = pns.m_classes[j].m_name;
            PyTypeObject* typeObject     = pns.m_classes[j].m_pto;

            std::vector<std::string> splitClassName = StringSplit(className, '.');

            SPI_POST_CONDITION(splitClassName.size() == 2);
            SPI_POST_CONDITION(splitClassName[0] == pns.m_ns);

            Py_INCREF(typeObject);
            PyModule_AddObject(
                module, splitClassName[1].c_str(), (PyObject*)(typeObject));
        }

        Py_INCREF(module);
        PyModule_AddObject(m_module, pns.m_ns, module);
    }
}

// given a Value of type OBJECT this will return the correct
// PyObject type based on m_classes
PyObject* PythonService::MakeObjectOfType(const Value& value)
{
    ObjectConstSP obj = value.getObject();
    if (!obj)
        Py_RETURN_NONE;

    // is this object already a delegate (OuterType_python_delegate)
    // if it is then creating a new python object is bound to fail
    void* p = obj->get_py_object();
    if (p)
    {
        PyObject* self = (PyObject*)p;
        Py_INCREF(self);
        return self;
    }

    std::string className(obj->get_class_name());

    PyTypeObject* pto = FindCommonClass(className);

    SpiPyObject* pyo = PyObject_New(SpiPyObject, pto);
    if (!pyo)
    {
        PyErr_Format(PyExc_MemoryError, "Could not create Object.");
        throw spi::PyException();
    }

    pyo->obj = obj.get();
    pyo->obj->incRefCount();

    return (PyObject*)pyo;
}

PyObject* PythonService::GetPythonModule()
{
    return m_module;
}

/**
 * These methods need the service. You should create functions
 * that can call these methods using get_service() to get the
 * service object.
 */
PyObject* PythonService::StartLogging(PyObject* args)
{
    try
    {
        std::vector<Value> inputs = pyTupleToValueVector(
            "StartLogging", 2, args);

        Value output = spi::StartLogging(m_service, inputs[0], inputs[1],
            GetInputContext());

        return pyoFromValue(output);
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

PyObject* PythonService::StopLogging(void)
{
    try
    {
        Value output = spi::StopLogging(m_service);
        return pyoFromValue(output);
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

PyObject* PythonService::IsLogging(void)
{
    try
    {
        Value output = spi::IsLogging(m_service);
        return pyoFromValue(output);
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

PyObject* PythonService::HelpFunc(PyObject* args)
{
    try
    {
        Value name;
        if (args)
        {
            std::vector<Value> inputs = pyTupleToValueVector(
                "HelpFunc", 1, args);

            name = inputs[0];
        }
        Value output = spi::HelpFunc(m_service, name, GetInputContext());
        return pyoFromValue(output);
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

PyObject* PythonService::HelpEnum(PyObject* args)
{
    try
    {
        std::vector<Value> inputs = pyTupleToValueVector("HelpEnum", 1, args);

        Value output = spi::HelpEnum(m_service, inputs[0],
            GetInputContext());

        return pyoFromValue(output);
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

PyObject* PythonService::ObjectFromString(PyObject* args)
{
    try
    {
        std::vector<Value> inputs = pyTupleToValueVector("ObjectFromString", 1, args);

        Value output = spi::ObjectFromString(m_service, inputs[0],
            GetInputContext());

        return MakeObjectOfType(output);
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

PyObject* PythonService::ObjectFromFile(PyObject* args)
{
    try
    {
        std::vector<Value> inputs = pyTupleToValueVector("ObjectFromFile", 1, args);

        Value output = spi::ObjectFromFile(m_service, inputs[0],
            GetInputContext());

        return MakeObjectOfType(output);
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

PyObject* PythonService::ObjectToString(PyObject* self, PyObject* args)
{
    try
    {
        ObjectConstSP obj = pyoToObject(self, NULL);
        if (!obj)
            throw std::runtime_error("Object.to_string: self is None");

        std::vector<Value> inputs = pyTupleToValueVector(
            "ObjectToString", 4, args);

        Value output = spi::ObjectToString(obj, inputs[0], inputs[1], inputs[2],
            inputs[3], GetInputContext());

        return pyoFromValue(output);
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

PyObject* PythonService::ObjectToFile(PyObject* self, PyObject* args)
{
    try
    {
        ObjectConstSP obj = pyoToObject(self, NULL);
        if (!obj)
            throw std::runtime_error("Object.to_file: self is None");

        std::vector<Value> inputs = pyTupleToValueVector(
            "ObjectToFile", 5, args);

        Value output = spi::ObjectToFile(obj, inputs[0], inputs[1], inputs[2], inputs[3],
            inputs[4], GetInputContext());

        return pyoFromValue(output);
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

PyObject* PythonService::ObjectCoerce(const char* className, PyObject* args)
{
    // className provided by the class
    // args provided by the user
    try
    {
        std::string funcName = StringFormat("%s.Coerce", className);
        std::vector<Value> inputs = pyTupleToValueVector(funcName.c_str(), 1, args);

        Value output = spi::ObjectCoerce(m_service, className, inputs[0],
            GetInputContext());

        return MakeObjectOfType(output);
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

PyObject* PythonService::ObjectCoerce(PyObject* args)
{
    // args has className as first parameter - this is for the global function
    try
    {
        std::vector<Value> inputs = pyTupleToValueVector(
            "ObjectCoerce", 2, args);

        Value output = spi::ObjectCoerce(m_service, inputs[0], inputs[1],
            GetInputContext());

        return MakeObjectOfType(output);
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

PyObject * PythonService::ObjectSHA(PyObject* self, int version)
{
    try
    {
        ObjectConstSP obj = pyoToObject(self, NULL);
        if (!obj)
            throw std::runtime_error("Object.to_string: self is None");
        
        Value output = spi::ObjectSHA(obj, version, GetInputContext());

        return pyoFromValue(output);
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

PyObject* PythonService::ObjectGet(PyObject* args)
{
    try
    {
        std::vector<Value> inputs = pyTupleToValueVector(
            "ObjectGet", 2, args);

        Value output = spi::ObjectGet(inputs[0], inputs[1],
            GetInputContext());

        return pyoFromValue(output);
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

PyObject* PythonService::ObjectPut(PyObject* args)
{
    try
    {
        std::vector<Value> in = pyTupleToValueVector(
            "ObjectPut", 27, args);

        Value output = spi::ObjectPut(in[0], in[1], in[2], in[3], in[4],
            in[5], in[6], in[7], in[8], in[9], in[10], in[11], in[12],
            in[13], in[14], in[15], in[16], in[17], in[18], in[19],
            in[20], in[21], in[22], in[23], in[24], in[25], in[26],
            GetInputContext());

        return pyoFromValue(output);
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

PyObject* PythonService::ObjectPutMetaData(PyObject* self, PyObject* args)
{
    try
    {
        ObjectConstSP obj = pyoToObject(self, NULL);
        if (!obj)
            throw std::runtime_error("ObjectPutMetaData: self is None");

        std::vector<Value> in = pyTupleToValueVector(
            "ObjectPutMetaData", 26, args);

        Value output = spi::ObjectPutMetaData(obj, in[0], in[1], in[2],
            in[3],  in[4],  in[5], in[6], in[7], in[8], in[9], in[10], in[11],
            in[12], in[13], in[14], in[15], in[16], in[17], in[18], in[19],
            in[20], in[21], in[22], in[23], in[24], in[25],
            GetInputContext());

        return pyoFromValue(obj);
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

FunctionCaller* PythonService::GetFunctionCaller(const char* name)
{
    return m_service->get_function_caller(name);
}

InputContext* PythonService::GetInputContext()
{
    return m_inputContext;
}

bool PythonService::IsCommonService() const
{
    return m_service->is_common_service();
}

PythonService* PythonService::CommonService()
{
    static bool init = false;
    static PythonService theService("", Service::CommonService());
    static PyTypeObject MapObject_PyObjectType =
    {
        PyVarObject_HEAD_INIT(NULL, 0)
        "Map", /*tp_name*/
        sizeof(SpiPyObject), /*tp_basicsize*/
        0, /*tp_itemsize*/
        (destructor)spi_py_object_dealloc, /*tp_dealloc*/
        0, /*tp_print*/
        0, /*tp_getattr*/
        (setattrfunc)spi_py_object_map_setter, /*tp_setattr*/
        0, /*tp_compare*/
        0, /*tp_repr*/
        0, /*tp_as_number*/
        0, /*tp_as_sequence*/
        0, /*tp_as_mapping*/
        0, /*tp_hash */
        0, /*tp_call*/
        0, /*tp_str*/
        (getattrofunc)spi_py_object_getattro, /*tp_getattro*/
        0, /*tp_setattro*/
        0, /*tp_as_buffer*/
        Py_TPFLAGS_DEFAULT, /*tp_flags*/
        "Name/value pairs without a specific class.", /* tp_doc */
        0, /* tp_traverse */
        0, /* tp_clear */
        0, /* tp_richcompare */
        0, /* tp_weaklistoffset */
        0, /* tp_iter */
        0, /* tp_iternext */
        0, /* tp_methods */
        0, /* tp_members */
        0, /* tp_getset */
        0, /* tp_base */
        0, /* tp_dict */
        0, /* tp_descr_get */
        0, /* tp_descr_set */
        0, /* tp_dictoffset */
        0, /* tp_init */
        0, /* tp_alloc */
        PyType_GenericNew, /* tp_new */
    };

    if (!init)
    {
        init = true;
        theService.AddClass("Map", "Map", &MapObject_PyObjectType);
    }

    return &theService;
}

SPI_END_NAMESPACE
