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

#ifndef _PY_SPDOC_CONFIGTYPES_HPP_
#define _PY_SPDOC_CONFIGTYPES_HPP_

/**
****************************************************************************
* Header file: py_spdoc_configTypes.hpp
****************************************************************************
*/

#include "pyd_spdoc_decl_spec.h"
#include <spi/Namespace.hpp>

#include "Python.h"
#include <vector>

SPI_BEGIN_NAMESPACE
class PythonService;
SPI_END_NAMESPACE

void py_spdoc_configTypes_update_functions(spi::PythonService* svc);

extern "C"
{

PyObject* py_spdoc_DataType_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_DataType_ValueType(PyObject* self, PyObject* args);

PyObject* py_spdoc_DataType_RefType(PyObject* self, PyObject* args);

PyObject* py_spdoc_Attribute_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_Attribute_encoding(PyObject* self, PyObject* args);

PyObject* py_spdoc_ClassAttribute_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_Construct_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_Construct_getType(PyObject* self, PyObject* args);

PyObject* py_spdoc_Construct_Summary(PyObject* self, PyObject* args);

PyObject* py_spdoc_SimpleType_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_Function_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_Function_returnsObject(PyObject* self, PyObject* args);

PyObject* py_spdoc_Function_objectCount(PyObject* self, PyObject* args);

PyObject* py_spdoc_Enumerand_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_Enum_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_ClassMethod_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_ClassMethod_Summary(PyObject* self, PyObject* args);

PyObject* py_spdoc_CoerceFrom_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_CoerceFrom_Summary(PyObject* self, PyObject* args);

PyObject* py_spdoc_CoerceTo_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_CoerceTo_Summary(PyObject* self, PyObject* args);

PyObject* py_spdoc_Class_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_Class_ObjectName(PyObject* self, PyObject* args);

PyObject* py_spdoc_Class_ServiceNamespace(PyObject* self, PyObject* args);

PyObject* py_spdoc_Module_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_Module_combineSummaries(PyObject* self, PyObject* args);

PyObject* py_spdoc_Service_Coerce(PyObject* self, PyObject* args);

PyObject* py_spdoc_Service_Summary(PyObject* self, PyObject* args);

PyObject* py_spdoc_Service_combineSummaries(PyObject* self, PyObject* args);

PyObject* py_spdoc_Service_getEnums(PyObject* self, PyObject* args);

PyObject* py_spdoc_Service_getEnum(PyObject* self, PyObject* args);

PyObject* py_spdoc_Service_getEnumerands(PyObject* self, PyObject* args);

PyObject* py_spdoc_Service_getClasses(PyObject* self, PyObject* args);

PyObject* py_spdoc_Service_getClass(PyObject* self, PyObject* args);

PyObject* py_spdoc_Service_getPropertyClass(PyObject* self, PyObject* args);

} /* end of extern "C" */

#endif /* _PY_SPDOC_CONFIGTYPES_HPP_*/

