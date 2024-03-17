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

#ifndef _SPDOC_CONFIGTYPES_HPP_
#define _SPDOC_CONFIGTYPES_HPP_

/**
****************************************************************************
* Header file: spdoc_configTypes.hpp
****************************************************************************
*/

#include <spi/spi.hpp>
#include "spdoc_constant.hpp"

SPDOC_BEGIN_NAMESPACE

SPI_DECLARE_OBJECT_CLASS(DataType);
SPI_DECLARE_OBJECT_CLASS(Attribute);
SPI_DECLARE_OBJECT_CLASS(ClassAttribute);
SPI_DECLARE_OBJECT_CLASS(Construct);
SPI_DECLARE_OBJECT_CLASS(SimpleType);
SPI_DECLARE_OBJECT_CLASS(Function);
SPI_DECLARE_OBJECT_CLASS(Enumerand);
SPI_DECLARE_OBJECT_CLASS(Enum);
SPI_DECLARE_OBJECT_CLASS(ClassMethod);
SPI_DECLARE_OBJECT_CLASS(CoerceFrom);
SPI_DECLARE_OBJECT_CLASS(CoerceTo);
SPI_DECLARE_OBJECT_CLASS(Class);
SPI_DECLARE_OBJECT_CLASS(Module);
SPI_DECLARE_OBJECT_CLASS(Service);

/**
****************************************************************************
* Defines a data type.
*
* This involves things such as the internal name, plus information on how
* the type is declared and passed as an input to a function call etc.
****************************************************************************
*/
class SPI_IMPORT DataType : public spi::Object
{
public:
    static DataTypeConstSP Make(
        const std::string& name,
        bool passByReference,
        const std::string& refTypeName,
        const std::string& valueTypeName,
        PublicType publicType,
        bool noDoc,
        const std::string& nsService,
        bool objectAsValue);

    /**
    ************************************************************************
    * Returns the value type (includes the namespace)
    ************************************************************************
    */
    std::string ValueType() const;

    /**
    ************************************************************************
    * Returns the reference type (includes the namespace)
    ************************************************************************
    */
    std::string RefType() const;

    typedef spi::ObjectSmartPtr<DataType> outer_type;

    SPI_DECLARE_OBJECT_TYPE(DataType);

protected:

    DataType(
        const std::string& name,
        bool passByReference,
        const std::string& refTypeName,
        const std::string& valueTypeName,
        PublicType publicType,
        bool noDoc,
        const std::string& nsService,
        bool objectAsValue);

public:

    const std::string name;
    const bool passByReference;
    const std::string refTypeName;
    const std::string valueTypeName;
    const PublicType publicType;
    const bool noDoc;
    const std::string nsService;
    const bool objectAsValue;
};

/**
****************************************************************************
* Defines an attribute which is an input to a function call or output
* from a function call.
****************************************************************************
*/
class SPI_IMPORT Attribute : public spi::Object
{
public:
    static AttributeConstSP Make(
        const std::string& name,
        const std::vector<std::string>& description,
        const DataTypeConstSP& dataType,
        int arrayDim,
        bool isOptional,
        const ConstantConstSP& defaultValue);

    /**
    ************************************************************************
    * No description.
    *
    * @param isOutput
    ************************************************************************
    */
    std::string encoding(
        bool isOutput) const;

    typedef spi::ObjectSmartPtr<Attribute> outer_type;

    SPI_DECLARE_OBJECT_TYPE(Attribute);

protected:

    Attribute(
        const std::string& name,
        const std::vector<std::string>& description,
        const DataTypeConstSP& dataType,
        int arrayDim,
        bool isOptional,
        const ConstantConstSP& defaultValue);

public:

    const std::string name;
    const std::vector<std::string> description;
    const DataTypeConstSP dataType;
    const int arrayDim;
    const bool isOptional;
    const ConstantConstSP defaultValue;
    bool isArray() const;
};

/**
****************************************************************************
* Defines an attribute of a class.
****************************************************************************
*/
class SPI_IMPORT ClassAttribute : public spi::Object
{
public:
    static ClassAttributeConstSP Make(
        const std::string& name,
        const std::vector<std::string>& description,
        const DataTypeConstSP& dataType,
        int arrayDim,
        bool isOptional,
        const ConstantConstSP& defaultValue,
        bool accessible,
        const std::string& accessor);

    /**
    ************************************************************************
    * Converts to Attribute
    ************************************************************************
    */
    operator AttributeConstSP() const;

    typedef spi::ObjectSmartPtr<ClassAttribute> outer_type;

    SPI_DECLARE_OBJECT_TYPE(ClassAttribute);

    spi::ObjectConstSP coerce_to_object(
        const std::string& className) const;

protected:

    ClassAttribute(
        const std::string& name,
        const std::vector<std::string>& description,
        const DataTypeConstSP& dataType,
        int arrayDim,
        bool isOptional,
        const ConstantConstSP& defaultValue,
        bool accessible,
        const std::string& accessor);

public:

    const std::string name;
    const std::vector<std::string> description;
    const DataTypeConstSP dataType;
    const int arrayDim;
    const bool isOptional;
    const ConstantConstSP defaultValue;
    const bool accessible;
    const std::string accessor;
    bool isArray() const;
    std::string encoding() const;
};

/**
****************************************************************************
* Defines a construct - this is something that is defined by a module
* and can be either some sort of type definition or function.
****************************************************************************
*/
class SPI_IMPORT Construct : public spi::Object
{
public:

    /**
    ************************************************************************
    * Returns the type of the Construct - specific to each sub-class of
    * Construct.
    *
    * Inside the module we keep a list of constructs in the order that
    * they were defined, but we will often need to know what type of
    * Construct object we have in order to decide what to do with it!
    ************************************************************************
    */
    virtual std::string getType() const = 0;

    /**
    ************************************************************************
    * Returns a short summary of the construct (disregarding description)
    *
    * If the construct is undocumented then this will be a zero size array
    ************************************************************************
    */
    virtual std::vector<std::string> Summary() const = 0;

    typedef spi::ObjectSmartPtr<Construct> outer_type;

    SPI_DECLARE_OBJECT_TYPE(Construct);

protected:

    Construct();

public:
    virtual std::string getName() const = 0;
};

/**
****************************************************************************
* Defines a simple type.
****************************************************************************
*/
class SPI_IMPORT SimpleType : public Construct
{
public:
    static SimpleTypeConstSP Make(
        const std::string& name,
        const std::vector<std::string>& description,
        const std::string& typeName,
        bool noDoc);

    /**
    ************************************************************************
    * Returns "SIMPLE_TYPE"
    ************************************************************************
    */
    std::string getType() const;

    /**
    ************************************************************************
    * No description.
    ************************************************************************
    */
    std::vector<std::string> Summary() const;

    typedef spi::ObjectSmartPtr<SimpleType> outer_type;

    SPI_DECLARE_OBJECT_TYPE(SimpleType);

protected:

    SimpleType(
        const std::string& name,
        const std::vector<std::string>& description,
        const std::string& typeName,
        bool noDoc);

public:

    const std::string name;
    const std::vector<std::string> description;
    const std::string typeName;
    const bool noDoc;

public:
    std::string getName() const { return name; } 
};

/**
****************************************************************************
* Defines a function.
****************************************************************************
*/
class SPI_IMPORT Function : public Construct
{
public:
    static FunctionConstSP Make(
        const std::string& name,
        const std::vector<std::string>& description,
        const std::vector<std::string>& returnTypeDescription,
        const DataTypeConstSP& returnType,
        int returnArrayDim,
        const std::vector<AttributeConstSP>& inputs,
        const std::vector<AttributeConstSP>& outputs,
        const std::vector<std::string>& excelOptions);

    /**
    ************************************************************************
    * Does this Function return an object or not?
    ************************************************************************
    */
    bool returnsObject() const;

    /**
    ************************************************************************
    * How many objects does this function return?
    ************************************************************************
    */
    int objectCount() const;

    /**
    ************************************************************************
    * No description.
    ************************************************************************
    */
    std::vector<std::string> Summary() const;

    /**
    ************************************************************************
    * Returns "FUNCTION"
    ************************************************************************
    */
    std::string getType() const;

    typedef spi::ObjectSmartPtr<Function> outer_type;

    SPI_DECLARE_OBJECT_TYPE(Function);

protected:

    Function(
        const std::string& name,
        const std::vector<std::string>& description,
        const std::vector<std::string>& returnTypeDescription,
        const DataTypeConstSP& returnType,
        int returnArrayDim,
        const std::vector<AttributeConstSP>& inputs,
        const std::vector<AttributeConstSP>& outputs,
        const std::vector<std::string>& excelOptions);

public:

    const std::string name;
    const std::vector<std::string> description;
    const std::vector<std::string> returnTypeDescription;
    const DataTypeConstSP returnType;
    const int returnArrayDim;
    const std::vector<AttributeConstSP> inputs;
    const std::vector<AttributeConstSP> outputs;
    const std::vector<std::string> excelOptions;

public:
    std::string getName() const { return name; } 
};

/**
****************************************************************************
* Defines an enumerand.
****************************************************************************
*/
class SPI_IMPORT Enumerand : public spi::Object
{
public:
    static EnumerandConstSP Make(
        const std::string& code,
        const std::vector<std::string>& strings,
        const std::vector<std::string>& description);

    typedef spi::ObjectSmartPtr<Enumerand> outer_type;

    SPI_DECLARE_OBJECT_TYPE(Enumerand);

protected:

    Enumerand(
        const std::string& code,
        const std::vector<std::string>& strings,
        const std::vector<std::string>& description);

public:

    const std::string code;
    const std::vector<std::string> strings;
    const std::vector<std::string> description;
};

/**
****************************************************************************
* Defines an enumerated type.
****************************************************************************
*/
class SPI_IMPORT Enum : public Construct
{
public:
    static EnumConstSP Make(
        const std::string& name,
        const std::vector<std::string>& description,
        const std::vector<EnumerandConstSP>& enumerands);

    /**
    ************************************************************************
    * Returns "ENUM"
    ************************************************************************
    */
    std::string getType() const;

    /**
    ************************************************************************
    * No description.
    ************************************************************************
    */
    std::vector<std::string> Summary() const;

    typedef spi::ObjectSmartPtr<Enum> outer_type;

    SPI_DECLARE_OBJECT_TYPE(Enum);

protected:

    Enum(
        const std::string& name,
        const std::vector<std::string>& description,
        const std::vector<EnumerandConstSP>& enumerands);

public:

    const std::string name;
    const std::vector<std::string> description;
    const std::vector<EnumerandConstSP> enumerands;

public:
    std::string getName() const { return name; } 
};

/**
****************************************************************************
* Defines a class method.
****************************************************************************
*/
class SPI_IMPORT ClassMethod : public spi::Object
{
public:
    static ClassMethodConstSP Make(
        const FunctionConstSP& function,
        bool isConst,
        bool isVirtual,
        bool isStatic,
        bool isImplementation,
        const std::string& implements);

    /**
    ************************************************************************
    * No description.
    ************************************************************************
    */
    std::vector<std::string> Summary() const;

    typedef spi::ObjectSmartPtr<ClassMethod> outer_type;

    SPI_DECLARE_OBJECT_TYPE(ClassMethod);

protected:

    ClassMethod(
        const FunctionConstSP& function,
        bool isConst,
        bool isVirtual,
        bool isStatic,
        bool isImplementation,
        const std::string& implements);

public:

    const FunctionConstSP function;
    const bool isConst;
    const bool isVirtual;
    const bool isStatic;
    const bool isImplementation;
    const std::string implements;
};

/**
****************************************************************************
* Defines a CoerceFrom constructor for creating a class by coercion
* from a single input of some other type.
****************************************************************************
*/
class SPI_IMPORT CoerceFrom : public spi::Object
{
public:
    static CoerceFromConstSP Make(
        const std::vector<std::string>& description,
        const AttributeConstSP& coerceFrom);

    /**
    ************************************************************************
    * No description.
    ************************************************************************
    */
    std::vector<std::string> Summary() const;

    typedef spi::ObjectSmartPtr<CoerceFrom> outer_type;

    SPI_DECLARE_OBJECT_TYPE(CoerceFrom);

protected:

    CoerceFrom(
        const std::vector<std::string>& description,
        const AttributeConstSP& coerceFrom);

public:

    const std::vector<std::string> description;
    const AttributeConstSP coerceFrom;
};

/**
****************************************************************************
* Defines a CoerceTo operator for creating a class by coercion
* to a class from an instance of another class.
****************************************************************************
*/
class SPI_IMPORT CoerceTo : public spi::Object
{
public:
    static CoerceToConstSP Make(
        const std::vector<std::string>& description,
        const std::string& className,
        const DataTypeConstSP& classType);

    /**
    ************************************************************************
    * No description.
    ************************************************************************
    */
    std::vector<std::string> Summary() const;

    typedef spi::ObjectSmartPtr<CoerceTo> outer_type;

    SPI_DECLARE_OBJECT_TYPE(CoerceTo);

protected:

    CoerceTo(
        const std::vector<std::string>& description,
        const std::string& className,
        const DataTypeConstSP& classType);

public:

    const std::vector<std::string> description;
    const std::string className;
    const DataTypeConstSP classType;
};

/**
****************************************************************************
* Defines a class.
****************************************************************************
*/
class SPI_IMPORT Class : public Construct
{
public:
    static ClassConstSP Make(
        const std::string& name,
        const std::string& ns,
        const std::vector<std::string>& description,
        const std::string& baseClassName,
        const std::vector<ClassAttributeConstSP>& attributes,
        const std::vector<ClassAttributeConstSP>& properties,
        const std::vector<ClassMethodConstSP>& methods,
        const std::vector<CoerceFromConstSP>& coerceFrom,
        const std::vector<CoerceToConstSP>& coerceTo,
        bool isAbstract,
        bool noMake,
        const std::string& objectName,
        const DataTypeConstSP& dataType,
        bool isDelegate,
        bool canPut,
        bool hasDynamicAttributes,
        bool asValue,
        const std::string& funcPrefix,
        const std::string& instance);

    /**
    ************************************************************************
    * No description.
    ************************************************************************
    */
    std::vector<std::string> Summary() const;

    /**
    ************************************************************************
    * Returns "CLASS"
    ************************************************************************
    */
    std::string getType() const;

    /**
    ************************************************************************
    * Returns the object name. If objectName is defined, then that is the
    * object name. Otherwise name is the object name.
    ************************************************************************
    */
    std::string ObjectName() const;

    /**
    ************************************************************************
    * No description.
    ************************************************************************
    */
    std::string ServiceNamespace() const;

    typedef spi::ObjectSmartPtr<Class> outer_type;

    SPI_DECLARE_OBJECT_TYPE(Class);

protected:

    Class(
        const std::string& name,
        const std::string& ns,
        const std::vector<std::string>& description,
        const std::string& baseClassName,
        const std::vector<ClassAttributeConstSP>& attributes,
        const std::vector<ClassAttributeConstSP>& properties,
        const std::vector<ClassMethodConstSP>& methods,
        const std::vector<CoerceFromConstSP>& coerceFrom,
        const std::vector<CoerceToConstSP>& coerceTo,
        bool isAbstract,
        bool noMake,
        const std::string& objectName,
        const DataTypeConstSP& dataType,
        bool isDelegate,
        bool canPut,
        bool hasDynamicAttributes,
        bool asValue,
        const std::string& funcPrefix,
        const std::string& instance);

public:

    const std::string name;
    const std::string ns;
    const std::vector<std::string> description;
    const std::string baseClassName;
    const std::vector<ClassAttributeConstSP> attributes;
    const std::vector<ClassAttributeConstSP> properties;
    const std::vector<ClassMethodConstSP> methods;
    const std::vector<CoerceFromConstSP> coerceFrom;
    const std::vector<CoerceToConstSP> coerceTo;
    const bool isAbstract;
    const bool noMake;
    const std::string objectName;
    const DataTypeConstSP dataType;
    const bool isDelegate;
    const bool canPut;
    const bool hasDynamicAttributes;
    const bool asValue;
    const std::string funcPrefix;
    const std::string instance;

public:
    std::string getName() const { return name; } 
};

/**
****************************************************************************
* Defines a module.
****************************************************************************
*/
class SPI_IMPORT Module : public spi::Object
{
public:
    static ModuleConstSP Make(
        const std::string& name,
        const std::vector<std::string>& description,
        const std::string& ns,
        const std::vector<ConstructConstSP>& constructs);

    /**
    ************************************************************************
    * No description.
    *
    * @param modules
    * @param sort
    ************************************************************************
    */
    static std::vector<std::string> combineSummaries(
        const std::vector<ModuleConstSP>& modules,
        bool sort);

    typedef spi::ObjectSmartPtr<Module> outer_type;

    SPI_DECLARE_OBJECT_TYPE(Module);

protected:

    Module(
        const std::string& name,
        const std::vector<std::string>& description,
        const std::string& ns,
        const std::vector<ConstructConstSP>& constructs);

public:

    const std::string name;
    const std::vector<std::string> description;
    const std::string ns;
    const std::vector<ConstructConstSP> constructs;
};

/**
****************************************************************************
* Defines a service.
****************************************************************************
*/
class SPI_IMPORT Service : public spi::Object
{
public:
    static ServiceConstSP Make(
        const std::string& name,
        const std::vector<std::string>& description,
        const std::string& longName,
        const std::string& ns,
        const std::string& declSpec,
        const std::string& version,
        const std::vector<ModuleConstSP>& modules,
        const std::vector<ClassConstSP>& importedBaseClasses,
        const std::vector<EnumConstSP>& importedEnums,
        bool sharedService);

    /**
    ************************************************************************
    * No description.
    *
    * @param sort
    ************************************************************************
    */
    std::vector<std::string> Summary(
        bool sort) const;

    /**
    ************************************************************************
    * No description.
    *
    * @param services
    * @param sort
    ************************************************************************
    */
    static std::vector<std::string> combineSummaries(
        const std::vector<ServiceConstSP>& services,
        bool sort);

    /**
    ************************************************************************
    * Returns a sorted list of enumerated type names.
    ************************************************************************
    */
    std::vector<std::string> getEnums() const;

    /**
    ************************************************************************
    * Get an individual Enum by name.
    *
    * @param name
    ************************************************************************
    */
    EnumConstSP getEnum(
        const std::string& name) const;

    /**
    ************************************************************************
    * Returns the enumerands for a given enumerated type.
    *
    * @param name
    ************************************************************************
    */
    std::vector<std::string> getEnumerands(
        const std::string& name) const;

    /**
    ************************************************************************
    * Returns a sorted list of class names.
    ************************************************************************
    */
    std::vector<std::string> getClasses() const;

    /**
    ************************************************************************
    * Returns the class details for a class name.
    *
    * @param className
    ************************************************************************
    */
    ClassConstSP getClass(
        const std::string& className) const;

    /**
    ************************************************************************
    * Returns whether a given class is a sub-class of the data type of the given name.
    * Needs to be a method on the Service since otherwise we cannot find base class.
    *
    * @param cls
    * @param name
    ************************************************************************
    */
    bool isSubClass(
        const ClassConstSP& cls,
        const std::string& name) const;

    /**
    ************************************************************************
    * Returns the name of the class for which the given fieldName is a property.
    * If no such class exists then returns an empty string.
    *
    * @param baseClassName
    * @param fieldName
    ************************************************************************
    */
    std::string getPropertyClass(
        const std::string& baseClassName,
        const std::string& fieldName) const;

    typedef spi::ObjectSmartPtr<Service> outer_type;

    SPI_DECLARE_OBJECT_TYPE(Service);

protected:

    Service(
        const std::string& name,
        const std::vector<std::string>& description,
        const std::string& longName,
        const std::string& ns,
        const std::string& declSpec,
        const std::string& version,
        const std::vector<ModuleConstSP>& modules,
        const std::vector<ClassConstSP>& importedBaseClasses,
        const std::vector<EnumConstSP>& importedEnums,
        bool sharedService);

public:

    const std::string name;
    const std::vector<std::string> description;
    const std::string longName;
    const std::string ns;
    const std::string declSpec;
    const std::string version;
    const std::vector<ModuleConstSP> modules;
    const std::vector<ClassConstSP> importedBaseClasses;
    const std::vector<EnumConstSP> importedEnums;
    const bool sharedService;

private:
    friend class Service_Helper;

    mutable std::map<std::string, EnumConstSP> indexEnums;
    mutable std::map<std::string, ClassConstSP> indexClasses;
    void buildIndexEnums() const;
    void buildIndexClasses() const;

};

SPDOC_END_NAMESPACE

#endif /* _SPDOC_CONFIGTYPES_HPP_*/

