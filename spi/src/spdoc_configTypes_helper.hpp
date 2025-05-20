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

#ifndef _SPDOC_CONFIGTYPES_HELPER_HPP_
#define _SPDOC_CONFIGTYPES_HELPER_HPP_

/**
****************************************************************************
* Header file: spdoc_configTypes_helper.hpp
****************************************************************************
*/

#include "spdoc_configTypes.hpp"
#include "spdoc_dll_type_converters.hpp"

SPDOC_BEGIN_NAMESPACE

class DataType_Helper
{
public:
    static std::string ValueType(
        const DataTypeConstSP& self);
    static std::string RefType(
        const DataTypeConstSP& self);
};

class Attribute_Helper
{
public:
    static std::string encoding(
        const AttributeConstSP& self,
        bool isOutput,
        bool showDefault);
};

class ClassAttribute_Helper
{
public:
    static std::string encoding(
        const ClassAttributeConstSP& self,
        bool showDefault);
};

class Construct_Helper
{
public:

    virtual ~Construct_Helper() {}
    static std::string getType(
        const ConstructConstSP& self);
    static std::vector<std::string> Summary(
        const ConstructConstSP& self,
        bool includeDescription);
};

class SimpleType_Helper
{
public:
    static std::string getType(
        const SimpleTypeConstSP& self);
    static std::vector<std::string> Summary(
        const SimpleTypeConstSP& self,
        bool includeDescription);
};

class Function_Helper
{
public:
    static bool returnsObject(
        const FunctionConstSP& self);
    static AttributeConstSP returns(
        const FunctionConstSP& self);
    static int objectCount(
        const FunctionConstSP& self);
    static std::vector<std::string> Summary(
        const FunctionConstSP& self,
        bool includeDescription);
    static std::string getType(
        const FunctionConstSP& self);
};

class Enum_Helper
{
public:
    static std::string getType(
        const EnumConstSP& self);
    static std::vector<std::string> Summary(
        const EnumConstSP& self,
        bool includeDescription);
};

class ClassMethod_Helper
{
public:
    static std::vector<std::string> Summary(
        const ClassMethodConstSP& self,
        bool includeDescription);
};

class CoerceFrom_Helper
{
public:
    static std::vector<std::string> Summary(
        const CoerceFromConstSP& self,
        bool includeDescription);
};

class CoerceTo_Helper
{
public:
    static std::vector<std::string> Summary(
        const CoerceToConstSP& self,
        bool includeDescription);
};

class Class_Helper
{
public:
    static std::vector<std::string> Summary(
        const ClassConstSP& self,
        bool includeDescription);
    static std::string getType(
        const ClassConstSP& self);
    static std::string ObjectName(
        const ClassConstSP& self);
    static std::string ServiceNamespace(
        const ClassConstSP& self);
};

class Module_Helper
{
public:
    static std::vector<std::string> combineSummaries(
        const std::vector<ModuleConstSP>& modules,
        bool sort);
};

class Service_Helper
{
public:
    static ServiceConstSP CombineSharedServices(
        const ServiceConstSP& self,
        const std::vector<ServiceConstSP>& sharedServices);
    static std::vector<std::string> Summary(
        const ServiceConstSP& self,
        bool sort);
    static std::vector<std::string> combineSummaries(
        const std::vector<ServiceConstSP>& services,
        bool sort);
    static std::vector<std::string> getEnums(
        const ServiceConstSP& self);
    static EnumConstSP getEnum(
        const ServiceConstSP& self,
        const std::string& name);
    static std::vector<std::string> getEnumerands(
        const ServiceConstSP& self,
        const std::string& name);
    static std::vector<std::string> getClasses(
        const ServiceConstSP& self);
    static ClassConstSP getClass(
        const ServiceConstSP& self,
        const std::string& className);
    static bool isSubClass(
        const ServiceConstSP& self,
        const ClassConstSP& cls,
        const std::string& name);
    static std::string getPropertyClass(
        const ServiceConstSP& self,
        const std::string& baseClassName,
        const std::string& fieldName);
    static std::vector<std::string> getConstructs(
        const ServiceConstSP& self);
    static ConstructConstSP getConstruct(
        const ServiceConstSP& self,
        const std::string& name);
};

void configTypes_register_object_types(const spi::ServiceSP& svc);

SPDOC_END_NAMESPACE

#endif /* _SPDOC_CONFIGTYPES_HELPER_HPP_*/

