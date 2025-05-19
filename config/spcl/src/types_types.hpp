/*

    Sartorial Programming Interface (SPI) code generators
    Copyright (C) 2012-2023 Sartorial Programming Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef _TYPES_TYPES_HPP_
#define _TYPES_TYPES_HPP_

/**
****************************************************************************
* Header file: types_types.hpp
****************************************************************************
*/

#include <spi/spi.hpp>
#include "types_dll_decl_spec.h"
#include "types_namespace.hpp"
class InputConverter;
class InputConverterStringFormat;
class InputConverterClass;
class DataType;
class Attribute;
class ClassAttribute;
class Enumerand;
class Enum;
class Class;
class Struct;
class InnerClass;
class WrapperClass;
class TypesLibrary;

TYPES_BEGIN_NAMESPACE

SPI_DECLARE_OBJECT_CLASS(InputConverter);
SPI_DECLARE_OBJECT_CLASS(InputConverterStringFormat);
SPI_DECLARE_OBJECT_CLASS(InputConverterClass);
SPI_DECLARE_OBJECT_CLASS(DataType);
SPI_DECLARE_OBJECT_CLASS(Attribute);
SPI_DECLARE_OBJECT_CLASS(ClassProperty);
SPI_DECLARE_OBJECT_CLASS(Enumerand);
SPI_DECLARE_OBJECT_CLASS(Enum);
SPI_DECLARE_OBJECT_CLASS(BaseClass);
SPI_DECLARE_OBJECT_CLASS(BaseStruct);
SPI_DECLARE_OBJECT_CLASS(InnerClass);
SPI_DECLARE_OBJECT_CLASS(BaseWrapperClass);
SPI_DECLARE_OBJECT_CLASS(TypesLibrary);

/**
****************************************************************************
* Class PublicType containing enumerated type PublicType::Enum.
* Whenever PublicType is expected you can use PublicType::Enum,
* and vice versa, because automatic type conversion is provided by
* the constructor and cast operator.
*
* PublicType::BOOL
* PublicType::CHAR
* PublicType::INT
* PublicType::DOUBLE
* PublicType::STRING
* PublicType::DATE
* PublicType::DATETIME
* PublicType::ENUM_AS_STRING
* PublicType::ENUM_AS_INT
* PublicType::CLASS
* PublicType::OBJECT
* PublicType::MAP
* PublicType::VARIANT
****************************************************************************
*/
class SPI_IMPORT PublicType
{
public:
    enum Enum
    {
        BOOL,
        CHAR,
        INT,
        DOUBLE,
        STRING,
        DATE,
        DATETIME,
        ENUM_AS_STRING,
        ENUM_AS_INT,
        CLASS,
        OBJECT,
        MAP,
        VARIANT,
        UNINITIALIZED_VALUE
    };

    static spi::EnumInfo* get_enum_info();

    PublicType() : value(UNINITIALIZED_VALUE) {}
    PublicType(PublicType::Enum value) : value(value) {}
    PublicType(const char* str) : value(PublicType::from_string(str)) {}
    PublicType(const std::string& str) : value(PublicType::from_string(str.c_str())) {}
    PublicType(const spi::Value& value);
    PublicType(int value);

    operator PublicType::Enum() const { return value; }
    operator std::string() const { return to_string(); }
    operator spi::Value() const { return to_value(); }
    std::string to_string() const { return std::string(PublicType::to_string(value)); }
    spi::Value to_value() const { return spi::Value(to_string()); }

    static PublicType::Enum from_int(int);
    static PublicType::Enum from_string(const char*);
    static const char* to_string(PublicType::Enum);

private:
    PublicType::Enum value;
};

/**
****************************************************************************
* No description.
****************************************************************************
*/
class SPI_IMPORT InputConverter : public spi::Object
{
public:

    /**
    ************************************************************************
    * Creates instance of InputConverter by coercion from string.
    *
    * @param format
    ************************************************************************
    */
    static InputConverterConstSP Coerce(const std::string& format);

    typedef spi::ObjectSmartPtr<InputConverter> outer_type; 

    SPI_DECLARE_OBJECT_TYPE(InputConverter);

    typedef spi_boost::intrusive_ptr< ::InputConverter const > inner_type;
    inner_type get_inner() const;
    static inner_type get_inner(const outer_type& o);

    ~InputConverter();

    static InputConverterConstSP Wrap(const inner_type& inner);

    typedef InputConverterConstSP (sub_class_wrapper)(const inner_type& inner);

    static std::vector<sub_class_wrapper*> g_sub_class_wrappers;

protected:

    void set_inner(const spi_boost::intrusive_ptr< ::InputConverter const >&);
    InputConverter();

private:

    /* shared pointer to implementation */
    spi_boost::intrusive_ptr< ::InputConverter const > inner_value;
};

/**
****************************************************************************
* No description.
****************************************************************************
*/
class SPI_IMPORT InputConverterStringFormat : public InputConverter
{
public:
    static InputConverterStringFormatConstSP Make(
        const std::string& format);

    /* public accessor methods */
    std::string format() const;

    spi::Value as_value() const;

    typedef spi::ObjectSmartPtr<InputConverterStringFormat> outer_type; 

    SPI_DECLARE_OBJECT_TYPE(InputConverterStringFormat);

    typedef spi_boost::intrusive_ptr< ::InputConverterStringFormat const > inner_type;
    inner_type get_inner() const;
    static inner_type get_inner(const outer_type& o);

    ~InputConverterStringFormat();

    static InputConverterStringFormatConstSP Wrap(const inner_type& inner);
    static InputConverterConstSP BaseWrap(const InputConverter::inner_type& inner);

protected:

    void set_inner(const spi_boost::intrusive_ptr< ::InputConverterStringFormat const >&);

    InputConverterStringFormat(const inner_type& inner);

private:

    static inner_type make_inner(
        const std::string& format);

    /* shared pointer to implementation */
    spi_boost::intrusive_ptr< ::InputConverterStringFormat const > inner_value;
};

/**
****************************************************************************
* No description.
****************************************************************************
*/
class SPI_IMPORT InputConverterClass : public InputConverter
{
public:
    static InputConverterClassConstSP Make(
        const std::string& format);

    /* public accessor methods */
    std::string format() const;

    typedef spi::ObjectSmartPtr<InputConverterClass> outer_type; 

    SPI_DECLARE_OBJECT_TYPE(InputConverterClass);

    typedef spi_boost::intrusive_ptr< ::InputConverterClass const > inner_type;
    inner_type get_inner() const;
    static inner_type get_inner(const outer_type& o);

    ~InputConverterClass();

    static InputConverterClassConstSP Wrap(const inner_type& inner);
    static InputConverterConstSP BaseWrap(const InputConverter::inner_type& inner);

protected:

    void set_inner(const spi_boost::intrusive_ptr< ::InputConverterClass const >&);

    InputConverterClass(const inner_type& inner);

private:

    static inner_type make_inner(
        const std::string& format);

    /* shared pointer to implementation */
    spi_boost::intrusive_ptr< ::InputConverterClass const > inner_value;
};

/**
****************************************************************************
* No description.
****************************************************************************
*/
class SPI_IMPORT DataType : public spi::Object
{
public:
    static DataTypeConstSP Make(
        const std::string& name,
        const std::string& nsService,
        const std::string& cppName,
        const std::string& outerType,
        const std::string& innerType,
        const std::string& innerRefType,
        PublicType publicType,
        const std::string& objectName,
        bool isClosed,
        bool noDoc,
        const InputConverterConstSP& convertIn,
        const std::string& convertOut,
        const std::string& copyInner,
        bool ignored);

    typedef spi::ObjectSmartPtr<DataType> outer_type; 

    SPI_DECLARE_OBJECT_TYPE(DataType);

    typedef spi_boost::intrusive_ptr< ::DataType const > inner_type;
    inner_type get_inner() const;
    static inner_type get_inner(const outer_type& o);

    ~DataType();

    static DataTypeConstSP Wrap(const inner_type& inner);

protected:

    void set_inner(const spi_boost::intrusive_ptr< ::DataType const >&);

    DataType(const inner_type& inner);

private:

    static inner_type make_inner(
        const std::string& name,
        const std::string& nsService,
        const std::string& cppName,
        const std::string& outerType,
        const std::string& innerType,
        const std::string& innerRefType,
        PublicType publicType,
        const std::string& objectName,
        bool isClosed,
        bool noDoc,
        const InputConverterConstSP& convertIn,
        const std::string& convertOut,
        const std::string& copyInner,
        bool ignored);

    /* private accessor methods */
    std::string name() const;
    std::string nsService() const;
    std::string cppName() const;
    std::string outerType() const;
    std::string innerType() const;
    std::string innerRefType() const;
    PublicType publicType() const;
    std::string objectName() const;
    bool isClosed() const;
    bool noDoc() const;
    InputConverterConstSP convertIn() const;
    std::string convertOut() const;
    std::string copyInner() const;
    bool ignored() const;

    /* shared pointer to implementation */
    spi_boost::intrusive_ptr< ::DataType const > inner_value;
};

/**
****************************************************************************
* No description.
****************************************************************************
*/
class SPI_IMPORT Attribute : public spi::Object
{
public:
    static AttributeConstSP Make(
        const DataTypeConstSP& dataType,
        const std::string& name,
        int arrayDim);

    typedef spi::ObjectSmartPtr<Attribute> outer_type; 

    SPI_DECLARE_OBJECT_TYPE(Attribute);

    typedef spi_boost::intrusive_ptr< ::Attribute const > inner_type;
    inner_type get_inner() const;
    static inner_type get_inner(const outer_type& o);

    ~Attribute();

    static AttributeConstSP Wrap(const inner_type& inner);

protected:

    void set_inner(const spi_boost::intrusive_ptr< ::Attribute const >&);

    Attribute(const inner_type& inner);

private:

    static inner_type make_inner(
        const DataTypeConstSP& dataType,
        const std::string& name,
        int arrayDim);

    /* private accessor methods */
    DataTypeConstSP dataType() const;
    std::string name() const;
    int arrayDim() const;

    /* shared pointer to implementation */
    spi_boost::intrusive_ptr< ::Attribute const > inner_value;
};

/**
****************************************************************************
* No description.
****************************************************************************
*/
class SPI_IMPORT ClassProperty : public spi::Object
{
public:
    static ClassPropertyConstSP Make(
        const AttributeConstSP& attribute);

    typedef spi::ObjectSmartPtr<ClassProperty> outer_type; 

    SPI_DECLARE_OBJECT_TYPE(ClassProperty);

    typedef spi_boost::intrusive_ptr< ::ClassAttribute const > inner_type;
    inner_type get_inner() const;
    static inner_type get_inner(const outer_type& o);

    ~ClassProperty();

    static ClassPropertyConstSP Wrap(const inner_type& inner);

protected:

    void set_inner(const spi_boost::intrusive_ptr< ::ClassAttribute const >&);

    ClassProperty(const inner_type& inner);

private:

    static inner_type make_inner(
        const AttributeConstSP& attribute);

    /* private accessor methods */
    AttributeConstSP attribute() const;

    /* shared pointer to implementation */
    spi_boost::intrusive_ptr< ::ClassAttribute const > inner_value;
};

/**
****************************************************************************
* No description.
****************************************************************************
*/
class SPI_IMPORT Enumerand : public spi::Object
{
public:
    static EnumerandConstSP Make(
        const std::string& code,
        const std::vector<std::string>& strings);

    /* public accessor methods */
    std::string code() const;
    std::vector<std::string> strings() const;

    typedef spi::ObjectSmartPtr<Enumerand> outer_type; 

    SPI_DECLARE_OBJECT_TYPE(Enumerand);

    typedef spi_boost::intrusive_ptr< ::Enumerand const > inner_type;
    inner_type get_inner() const;
    static inner_type get_inner(const outer_type& o);

    ~Enumerand();

    static EnumerandConstSP Wrap(const inner_type& inner);

protected:

    void set_inner(const spi_boost::intrusive_ptr< ::Enumerand const >&);

    Enumerand(const inner_type& inner);

private:

    static inner_type make_inner(
        const std::string& code,
        const std::vector<std::string>& strings);

    /* shared pointer to implementation */
    spi_boost::intrusive_ptr< ::Enumerand const > inner_value;
};

/**
****************************************************************************
* No description.
****************************************************************************
*/
class SPI_IMPORT Enum : public spi::Object
{
public:
    static EnumConstSP Make(
        const std::string& name,
        const std::vector<EnumerandConstSP>& enumerands);

    /* public accessor methods */
    std::string name() const;
    std::vector<EnumerandConstSP> enumerands() const;

    typedef spi::ObjectSmartPtr<Enum> outer_type; 

    SPI_DECLARE_OBJECT_TYPE(Enum);

    typedef spi_boost::intrusive_ptr< ::Enum const > inner_type;
    inner_type get_inner() const;
    static inner_type get_inner(const outer_type& o);

    ~Enum();

    static EnumConstSP Wrap(const inner_type& inner);

protected:

    void set_inner(const spi_boost::intrusive_ptr< ::Enum const >&);

    Enum(const inner_type& inner);

private:

    static inner_type make_inner(
        const std::string& name,
        const std::vector<EnumerandConstSP>& enumerands);

    /* shared pointer to implementation */
    spi_boost::intrusive_ptr< ::Enum const > inner_value;
};

/**
****************************************************************************
* No description.
****************************************************************************
*/
class SPI_IMPORT BaseClass : public spi::Object
{
public:

    typedef spi::ObjectSmartPtr<BaseClass> outer_type; 

    SPI_DECLARE_OBJECT_TYPE(BaseClass);

    typedef spi_boost::intrusive_ptr< ::Class const > inner_type;
    inner_type get_inner() const;
    static inner_type get_inner(const outer_type& o);

    ~BaseClass();

    static BaseClassConstSP Wrap(const inner_type& inner);

    typedef BaseClassConstSP (sub_class_wrapper)(const inner_type& inner);

    static std::vector<sub_class_wrapper*> g_sub_class_wrappers;

protected:

    void set_inner(const spi_boost::intrusive_ptr< ::Class const >&);
    BaseClass();

private:

    /* shared pointer to implementation */
    spi_boost::intrusive_ptr< ::Class const > inner_value;
};

/**
****************************************************************************
* No description.
****************************************************************************
*/
class SPI_IMPORT BaseStruct : public BaseClass
{
public:
    static BaseStructConstSP Make(
        const std::vector<std::string>& description,
        const std::string& name,
        const std::string& ns,
        const BaseClassConstSP& baseClass,
        bool noMake,
        const std::string& objectName,
        bool canPut,
        bool noId,
        bool isVirtual,
        bool asValue,
        bool uuid,
        bool byValue,
        bool useAccessors);

    typedef spi::ObjectSmartPtr<BaseStruct> outer_type; 

    SPI_DECLARE_OBJECT_TYPE(BaseStruct);

    typedef spi_boost::intrusive_ptr< ::Struct const > inner_type;
    inner_type get_inner() const;
    static inner_type get_inner(const outer_type& o);

    ~BaseStruct();

    static BaseStructConstSP Wrap(const inner_type& inner);
    static BaseClassConstSP BaseWrap(const BaseClass::inner_type& inner);

protected:

    void set_inner(const spi_boost::intrusive_ptr< ::Struct const >&);

    BaseStruct(const inner_type& inner);

private:

    static inner_type make_inner(
        const std::vector<std::string>& description,
        const std::string& name,
        const std::string& ns,
        const BaseClassConstSP& baseClass,
        bool noMake,
        const std::string& objectName,
        bool canPut,
        bool noId,
        bool isVirtual,
        bool asValue,
        bool uuid,
        bool byValue,
        bool useAccessors);

    /* private accessor methods */
    std::vector<std::string> description() const;
    std::string name() const;
    std::string ns() const;
    BaseClassConstSP baseClass() const;
    bool noMake() const;
    std::string objectName() const;
    bool canPut() const;
    bool noId() const;
    bool isVirtual() const;
    bool asValue() const;
    bool uuid() const;
    bool byValue() const;
    bool useAccessors() const;

    /* shared pointer to implementation */
    spi_boost::intrusive_ptr< ::Struct const > inner_value;
};

/**
****************************************************************************
* No description.
****************************************************************************
*/
class SPI_IMPORT InnerClass : public spi::Object
{
public:
    static InnerClassConstSP Make(
        const std::string& typeName,
        const std::string& ns,
        const std::string& freeFunc,
        const std::string& copyFunc,
        const std::string& preDeclaration,
        const std::string& sharedPtr,
        bool isShared,
        bool isConst,
        bool isOpen,
        bool isStruct,
        bool isCached,
        bool isTemplate,
        bool byValue,
        const std::string& boolTest,
        bool allowConst);

    typedef spi::ObjectSmartPtr<InnerClass> outer_type; 

    SPI_DECLARE_OBJECT_TYPE(InnerClass);

    typedef spi_boost::intrusive_ptr< ::InnerClass const > inner_type;
    inner_type get_inner() const;
    static inner_type get_inner(const outer_type& o);

    ~InnerClass();

    static InnerClassConstSP Wrap(const inner_type& inner);

protected:

    void set_inner(const spi_boost::intrusive_ptr< ::InnerClass const >&);

    InnerClass(const inner_type& inner);

private:

    static inner_type make_inner(
        const std::string& typeName,
        const std::string& ns,
        const std::string& freeFunc,
        const std::string& copyFunc,
        const std::string& preDeclaration,
        const std::string& sharedPtr,
        bool isShared,
        bool isConst,
        bool isOpen,
        bool isStruct,
        bool isCached,
        bool isTemplate,
        bool byValue,
        const std::string& boolTest,
        bool allowConst);

    /* private accessor methods */
    std::string typeName() const;
    std::string ns() const;
    std::string freeFunc() const;
    std::string copyFunc() const;
    std::string preDeclaration() const;
    std::string sharedPtr() const;
    bool isShared() const;
    bool isConst() const;
    bool isOpen() const;
    bool isStruct() const;
    bool isCached() const;
    bool isTemplate() const;
    bool byValue() const;
    std::string boolTest() const;
    bool allowConst() const;

    /* shared pointer to implementation */
    spi_boost::intrusive_ptr< ::InnerClass const > inner_value;
};

/**
****************************************************************************
* No description.
****************************************************************************
*/
class SPI_IMPORT BaseWrapperClass : public BaseClass
{
public:
    static BaseWrapperClassConstSP Make(
        const std::vector<std::string>& description,
        const std::string& name,
        const std::string& ns,
        const InnerClassConstSP& innerClass,
        const BaseWrapperClassConstSP& baseClass,
        bool isVirtual,
        bool noMake,
        const std::string& objectName,
        bool isDelegate,
        bool canPut,
        bool noId,
        const DataTypeConstSP& dataType,
        bool asValue,
        const std::vector<ClassPropertyConstSP>& classProperties,
        bool uuid);

    typedef spi::ObjectSmartPtr<BaseWrapperClass> outer_type; 

    SPI_DECLARE_OBJECT_TYPE(BaseWrapperClass);

    typedef spi_boost::intrusive_ptr< ::WrapperClass const > inner_type;
    inner_type get_inner() const;
    static inner_type get_inner(const outer_type& o);

    ~BaseWrapperClass();

    static BaseWrapperClassConstSP Wrap(const inner_type& inner);
    static BaseClassConstSP BaseWrap(const BaseClass::inner_type& inner);

protected:

    void set_inner(const spi_boost::intrusive_ptr< ::WrapperClass const >&);

    BaseWrapperClass(const inner_type& inner);

private:

    static inner_type make_inner(
        const std::vector<std::string>& description,
        const std::string& name,
        const std::string& ns,
        const InnerClassConstSP& innerClass,
        const BaseWrapperClassConstSP& baseClass,
        bool isVirtual,
        bool noMake,
        const std::string& objectName,
        bool isDelegate,
        bool canPut,
        bool noId,
        const DataTypeConstSP& dataType,
        bool asValue,
        const std::vector<ClassPropertyConstSP>& classProperties,
        bool uuid);

    /* private accessor methods */
    std::vector<std::string> description() const;
    std::string name() const;
    std::string ns() const;
    InnerClassConstSP innerClass() const;
    BaseWrapperClassConstSP baseClass() const;
    bool isVirtual() const;
    bool noMake() const;
    std::string objectName() const;
    bool isDelegate() const;
    bool canPut() const;
    bool noId() const;
    DataTypeConstSP dataType() const;
    bool asValue() const;
    std::vector<ClassPropertyConstSP> classProperties() const;
    bool uuid() const;

    /* shared pointer to implementation */
    spi_boost::intrusive_ptr< ::WrapperClass const > inner_value;
};

/**
****************************************************************************
* No description.
****************************************************************************
*/
class SPI_IMPORT TypesLibrary : public spi::Object
{
public:
    static TypesLibraryConstSP Make(
        const std::string& name,
        const std::string& ns,
        const std::string& version,
        const std::string& lastModuleName,
        const std::vector<DataTypeConstSP>& dataTypes,
        const std::vector<DataTypeConstSP>& publicDataTypes,
        const std::vector<BaseClassConstSP>& baseClasses,
        const std::vector<EnumConstSP>& enums);

    typedef spi::ObjectSmartPtr<TypesLibrary> outer_type; 

    SPI_DECLARE_OBJECT_TYPE(TypesLibrary);

    typedef spi_boost::intrusive_ptr< ::TypesLibrary const > inner_type;
    inner_type get_inner() const;
    static inner_type get_inner(const outer_type& o);

    ~TypesLibrary();

    static TypesLibraryConstSP Wrap(const inner_type& inner);

protected:

    void set_inner(const spi_boost::intrusive_ptr< ::TypesLibrary const >&);

    TypesLibrary(const inner_type& inner);

private:

    static inner_type make_inner(
        const std::string& name,
        const std::string& ns,
        const std::string& version,
        const std::string& lastModuleName,
        const std::vector<DataTypeConstSP>& dataTypes,
        const std::vector<DataTypeConstSP>& publicDataTypes,
        const std::vector<BaseClassConstSP>& baseClasses,
        const std::vector<EnumConstSP>& enums);

    /* private accessor methods */
    std::string name() const;
    std::string ns() const;
    std::string version() const;
    std::string lastModuleName() const;
    std::vector<DataTypeConstSP> dataTypes() const;
    std::vector<DataTypeConstSP> publicDataTypes() const;
    std::vector<BaseClassConstSP> baseClasses() const;
    std::vector<EnumConstSP> enums() const;

    /* shared pointer to implementation */
    spi_boost::intrusive_ptr< ::TypesLibrary const > inner_value;
};

TYPES_END_NAMESPACE

#endif /* _TYPES_TYPES_HPP_*/

