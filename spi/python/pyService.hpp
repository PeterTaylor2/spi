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
#ifndef SPI_PY_SERVICE_HPP
#define SPI_PY_SERVICE_HPP

/*
***************************************************************************
** pyService.hpp
**
** Defines the Python service class. This is linked to the Service class
** for the underlying C++ outer library.
***************************************************************************
*/

#include "DeclSpec.h"
#include <spi/Service.hpp>
#include <spi/spdoc_configTypes.hpp>
#include <vector>
#include <string>

#include "Python.h"

#include "pyValue.hpp"

SPI_BEGIN_NAMESPACE

struct PythonClass
{
    PythonClass(
        const std::string& name,
        const std::string& objectName,
        PyTypeObject* pto);

    std::string   m_name;
    std::string   m_objectName;
    PyTypeObject* m_pto;
};

struct PythonNamespace
{
    PythonNamespace(const char* ns);

    const char*              m_ns;
    std::vector<PyMethodDef> m_functions;
    std::vector<PythonClass> m_classes;
#if PY_MAJOR_VERSION >= 3
    PyModuleDef              m_moduleDef;
#endif
};

typedef spi::Object* (DelegateObjectConstructor)(PyObject* pyo);

class SPI_PY_IMPORT PythonService
{
public:
    PythonService(const char* moduleName, const ServiceSP& service);

    static PythonService* CommonService();

    /**
     * Register all the functions and classes from the library.
    */
    void RegisterFunctions();

    // void DebugModule(const char* state);

    /**
     * These methods need the service. You should create functions
     * that can call these methods using get_service() to get the
     * service object.
     */
    PyObject* StartLogging(PyObject* args);
    PyObject* StopLogging(void);
    PyObject* IsLogging(void);
    PyObject* HelpFunc(PyObject* args);
    PyObject* HelpEnum(PyObject* args);
    PyObject* ObjectFromString(PyObject* args);
    PyObject* ObjectFromFile(PyObject* args);
    PyObject* ObjectToString(PyObject* self, PyObject* args);
    PyObject* ObjectToFile(PyObject* self, PyObject* args);
    PyObject* ObjectGet(PyObject* args);
    PyObject* ObjectPut(PyObject* args);
    // we need two version of ObjectCoerce
    // one called from within the class which knows the className
    // and a static version which has the class name as the first arg
    PyObject* ObjectCoerce(const char* className, PyObject* args);
    PyObject* ObjectCoerce(PyObject* args);
    PyObject* ObjectSHA(PyObject* self, int version);

    FunctionCaller* GetFunctionCaller(const char* name);
    InputContext* GetInputContext();
    void SetNamespace(const char* ns);
    void AddFunction(
        const char* name,
        PyCFunction func,
        const char* doc,
        int flags=METH_VARARGS);
    void AddClass(
        const std::string& name,
        const std::string& objectName,
        PyTypeObject* pyo,
        const char* baseClassName=0);
    void AddDelegateClass(
        const std::string& name,
        DelegateObjectConstructor* constructor);
    spi::ObjectConstSP ConstructDelegate(PyObject* pyo) const;

    // we use this function to register a Python class from Python code
    // args should be a 2-tuple (type,className)
    static void RegisterPythonClass(PyObject* args);
    static ObjectTypeSP FindRegisteredPythonClass(PyTypeObject* pto);

    //spdoc::ServiceConstSP getDoc();
    ServiceSP GetService() const;

    // given a Value of type OBJECT this will return the correct
    // PyObject type based on m_classes
    static PyObject* MakeObjectOfType(const Value& value);

    // gets the python module (not incremented)
    PyObject* GetPythonModule();

    static PyObject* ObjectCopy(PyObject* pyo, bool deepCopy);
    static ObjectConstSP PythonMakeFromMap(IObjectMap* m, ValueToObject& m2o);
    static bool PythonTypeIsInstance(const ObjectConstSP& o);

private:
    const char*                         m_moduleName;
    ServiceSP                           m_service;
    PyObject*                           m_module;
    InputContext*                       m_inputContext;
    std::vector<PyMethodDef>            m_functions;
    std::map<std::string,PyObjectSP>    m_indexClasses;
    std::vector<PythonClass>            m_classes;
    PyTypeObject*                       m_baseObjectType;
    std::vector<PythonNamespace>        m_namespaces;
    PythonNamespace*                    m_currentNamespace;
    std::map<std::string,DelegateObjectConstructor*> m_indexDelegateClasses;
    //std::string m_dirname;
    //spdoc::ServiceConstSP m_doc;

    bool IsCommonService() const;

    static ObjectTypeSP RegisterPythonClassDetails(
        const std::string& className,
        PyTypeObject* pyType);

    static void AddCommonClass(
        const std::string& objectName,
        PyTypeObject* pyo);

    static PyTypeObject* FindCommonClass(const std::string& objectName);
    static std::map<PyTypeObject*, ObjectTypeSP> g_indexPythonObjectType;
#if PY_MAJOR_VERSION >= 3
    PyModuleDef m_moduleDef;
#endif
};



SPI_END_NAMESPACE

#endif /* SPI_PY_SERVICE_HPP */
