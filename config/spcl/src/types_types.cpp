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

/**
****************************************************************************
* Source file: types_types.cpp
****************************************************************************
*/

/*
****************************************************************************
* Generated code - do not edit
****************************************************************************
*/

#include "types_types.hpp"
#include "types_types_helper.hpp"
#include "types_dll_service_manager.hpp"

#include "attribute.hpp"
#include "cppType.hpp"
#include "dataType.hpp"
#include "struct.hpp"
#include "innerClass.hpp"
#include "wrapperClass.hpp"
#include "serviceDefinition.hpp"
#include "verbatim.hpp"
#include "inputConverter.hpp"
#include "converter.hpp"
#include "enum.hpp"

TYPES_BEGIN_NAMESPACE

PublicType::PublicType(const spi::Value &v)
{
    switch(v.getType())
    {
    case spi::Value::INT:
        value = PublicType::from_int(v.getInt());
        break;
    case spi::Value::DOUBLE:
        value = PublicType::from_int(v.getInt(true));
        break;
    case spi::Value::SHORT_STRING:
    case spi::Value::STRING:
        {
            PublicType that(v.getString());
            value = that.value;
        }
        break;
    case spi::Value::UNDEFINED:
        {
            PublicType that;
            value = that.value;
        }
        break;
    default:
        SPI_THROW_RUNTIME_ERROR("Bad value type: " << spi::Value::TypeToString(v.getType()));
    }
}

PublicType::PublicType(int v)
{
    value = PublicType::from_int(v); 
}

PublicType::Enum PublicType::from_int(int value)
{
    if (value < 0 || value > (int)UNINITIALIZED_VALUE)
    {
        SPI_THROW_RUNTIME_ERROR("Input value out of range");
    }
    return (PublicType::Enum)value; 
}

spdoc::PublicType::Enum PublicType_convert_in(const PublicType& v_)
{
    switch((PublicType::Enum)v_)
    {
    case PublicType::BOOL:
        return spdoc::PublicType::BOOL;
    case PublicType::CHAR:
        return spdoc::PublicType::CHAR;
    case PublicType::INT:
        return spdoc::PublicType::INT;
    case PublicType::DOUBLE:
        return spdoc::PublicType::DOUBLE;
    case PublicType::STRING:
        return spdoc::PublicType::STRING;
    case PublicType::DATE:
        return spdoc::PublicType::DATE;
    case PublicType::DATETIME:
        return spdoc::PublicType::DATETIME;
    case PublicType::ENUM_AS_STRING:
        return spdoc::PublicType::ENUM_AS_STRING;
    case PublicType::ENUM_AS_INT:
        return spdoc::PublicType::ENUM_AS_INT;
    case PublicType::CLASS:
        return spdoc::PublicType::CLASS;
    case PublicType::OBJECT:
        return spdoc::PublicType::OBJECT;
    case PublicType::MAP:
        return spdoc::PublicType::MAP;
    case PublicType::VARIANT:
        return spdoc::PublicType::VARIANT;
    case PublicType::UNINITIALIZED_VALUE:
        throw std::runtime_error("Uninitialized value for PublicType");
    }
    throw spi::RuntimeError("Bad enumerated value");
}

PublicType PublicType_convert_out(spdoc::PublicType::Enum v_)
{
    if (v_ == spdoc::PublicType::BOOL)
        return PublicType::BOOL;
    if (v_ == spdoc::PublicType::CHAR)
        return PublicType::CHAR;
    if (v_ == spdoc::PublicType::INT)
        return PublicType::INT;
    if (v_ == spdoc::PublicType::DOUBLE)
        return PublicType::DOUBLE;
    if (v_ == spdoc::PublicType::STRING)
        return PublicType::STRING;
    if (v_ == spdoc::PublicType::DATE)
        return PublicType::DATE;
    if (v_ == spdoc::PublicType::DATETIME)
        return PublicType::DATETIME;
    if (v_ == spdoc::PublicType::ENUM_AS_STRING)
        return PublicType::ENUM_AS_STRING;
    if (v_ == spdoc::PublicType::ENUM_AS_INT)
        return PublicType::ENUM_AS_INT;
    if (v_ == spdoc::PublicType::CLASS)
        return PublicType::CLASS;
    if (v_ == spdoc::PublicType::OBJECT)
        return PublicType::OBJECT;
    if (v_ == spdoc::PublicType::MAP)
        return PublicType::MAP;
    if (v_ == spdoc::PublicType::VARIANT)
        return PublicType::VARIANT;
    throw spi::RuntimeError("Bad enumerated value");
}

/*
****************************************************************************
* Implementation of InputConverter
****************************************************************************
*/

InputConverter::inner_type InputConverter::get_inner() const
{
    return inner_value;
}

InputConverter::inner_type InputConverter::get_inner(const InputConverter::outer_type& o)
{
    if (!o)
        return inner_type();
    return o->get_inner();
}

void InputConverter::set_inner(const spi_boost::intrusive_ptr< ::InputConverter const >& self)
{
    SPI_PRE_CONDITION(!inner_value);
    inner_value = self;
}

InputConverter::InputConverter()
{}

InputConverter::~InputConverter()
{}

InputConverterConstSP InputConverter::Wrap(const inner_type& inner)
{
    if (!inner)
        return InputConverterConstSP();
    for (size_t i = 0; i < g_sub_class_wrappers.size(); ++i)
    {
        InputConverterConstSP o = g_sub_class_wrappers[i](inner);
        if (o)
            return o;
    }
    throw spi::RuntimeError("%s: Failed!", __FUNCTION__);
}

InputConverterConstSP InputConverter::Coerce(const std::string& format)
{
    return InputConverterStringFormat::Make(format);
}

/*
****************************************************************************
* Implementation of InputConverterStringFormat
****************************************************************************
*/
InputConverterStringFormatConstSP InputConverterStringFormat::Make(
    const std::string& format)
{
  SPI_PROFILE("types.InputConverterStringFormat.Make");
  types_check_permission();
  try
  {
    inner_type self = make_inner(format);
    return Wrap(self);
  }
  catch (std::exception& e)
  { SPI_THROW_RUNTIME_ERROR("Could not create InputConverterStringFormat object:\n" << e.what()); }
  catch (...)
  { SPI_THROW_RUNTIME_ERROR("Could not create InputConverterStringFormat object:\n" << "Unknown exception"); }
}

InputConverterStringFormat::inner_type InputConverterStringFormat::make_inner(
    const std::string& o_format)
{

    const std::string& format = o_format;

    return ::InputConverterStringFormat::Make(format);
}

InputConverterStringFormat::inner_type InputConverterStringFormat::get_inner() const
{
    return inner_value;
}

InputConverterStringFormat::inner_type InputConverterStringFormat::get_inner(const InputConverterStringFormat::outer_type& o)
{
    if (!o)
        return inner_type();
    return o->get_inner();
}

void InputConverterStringFormat::set_inner(const spi_boost::intrusive_ptr< ::InputConverterStringFormat const >& self)
{
    SPI_PRE_CONDITION(!inner_value);
    inner_value = self;
    InputConverter::set_inner(self);
}

InputConverterStringFormat::~InputConverterStringFormat()
{}

InputConverterConstSP InputConverterStringFormat::BaseWrap(const InputConverter::inner_type& baseInner)
{
    inner_type inner = spi_boost::dynamic_pointer_cast< ::InputConverterStringFormat const >(baseInner);
    if (!inner)
        return InputConverterConstSP();
    return Wrap(inner);
}

InputConverterStringFormatConstSP InputConverterStringFormat::Wrap(const inner_type& inner)
{
    if (!inner)
        return InputConverterStringFormatConstSP();
    return InputConverterStringFormatConstSP(new InputConverterStringFormat(inner));
}

InputConverterStringFormat::InputConverterStringFormat(const inner_type& inner)
{
    set_inner(inner);
}

std::string InputConverterStringFormat::format() const
{
    inner_type self = get_inner();

return self->format(); }

spi::Value InputConverterStringFormat::as_value() const
{
    return spi::Value(format());
}

/*
****************************************************************************
* Implementation of InputConverterClass
****************************************************************************
*/
InputConverterClassConstSP InputConverterClass::Make(
    const std::string& format)
{
  SPI_PROFILE("types.InputConverterClass.Make");
  types_check_permission();
  try
  {
    inner_type self = make_inner(format);
    return Wrap(self);
  }
  catch (std::exception& e)
  { SPI_THROW_RUNTIME_ERROR("Could not create InputConverterClass object:\n" << e.what()); }
  catch (...)
  { SPI_THROW_RUNTIME_ERROR("Could not create InputConverterClass object:\n" << "Unknown exception"); }
}

InputConverterClass::inner_type InputConverterClass::make_inner(
    const std::string& o_format)
{

    const std::string& format = o_format;

    return ::InputConverterClass::Make(format);
}

InputConverterClass::inner_type InputConverterClass::get_inner() const
{
    return inner_value;
}

InputConverterClass::inner_type InputConverterClass::get_inner(const InputConverterClass::outer_type& o)
{
    if (!o)
        return inner_type();
    return o->get_inner();
}

void InputConverterClass::set_inner(const spi_boost::intrusive_ptr< ::InputConverterClass const >& self)
{
    SPI_PRE_CONDITION(!inner_value);
    inner_value = self;
    InputConverter::set_inner(self);
}

InputConverterClass::~InputConverterClass()
{}

InputConverterConstSP InputConverterClass::BaseWrap(const InputConverter::inner_type& baseInner)
{
    inner_type inner = spi_boost::dynamic_pointer_cast< ::InputConverterClass const >(baseInner);
    if (!inner)
        return InputConverterConstSP();
    return Wrap(inner);
}

InputConverterClassConstSP InputConverterClass::Wrap(const inner_type& inner)
{
    if (!inner)
        return InputConverterClassConstSP();
    return InputConverterClassConstSP(new InputConverterClass(inner));
}

InputConverterClass::InputConverterClass(const inner_type& inner)
{
    set_inner(inner);
}

std::string InputConverterClass::format() const
{
    inner_type self = get_inner();

return self->format(); }

/*
****************************************************************************
* Implementation of DataType
****************************************************************************
*/
DataTypeConstSP DataType::Make(
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
    bool ignored)
{
  SPI_PROFILE("types.DataType.Make");
  types_check_permission();
  try
  {
    inner_type self = make_inner(name, nsService, cppName, outerType,
        innerType, innerRefType, publicType, objectName, isClosed, noDoc,
        convertIn, convertOut, copyInner, ignored);
    return Wrap(self);
  }
  catch (std::exception& e)
  { SPI_THROW_RUNTIME_ERROR("Could not create DataType object:\n" << e.what()); }
  catch (...)
  { SPI_THROW_RUNTIME_ERROR("Could not create DataType object:\n" << "Unknown exception"); }
}

DataType::inner_type DataType::make_inner(
    const std::string& o_name,
    const std::string& o_nsService,
    const std::string& o_cppName,
    const std::string& o_outerType,
    const std::string& o_innerType,
    const std::string& o_innerRefType,
    PublicType o_publicType,
    const std::string& o_objectName,
    bool o_isClosed,
    bool o_noDoc,
    const InputConverterConstSP& o_convertIn,
    const std::string& o_convertOut,
    const std::string& o_copyInner,
    bool o_ignored)
{
    spdoc::PublicType::Enum publicType;
    spi_boost::intrusive_ptr< ::InputConverter const > convertIn;

    publicType = PublicType_convert_in(o_publicType);
    convertIn = InputConverter::get_inner(o_convertIn);

    const std::string& name = o_name;
    const std::string& nsService = o_nsService;
    const std::string& cppName = o_cppName;
    const std::string& outerType = o_outerType;
    const std::string& innerType = o_innerType;
    const std::string& innerRefType = o_innerRefType;
    const std::string& objectName = o_objectName;
    const bool& isClosed = o_isClosed;
    const bool& noDoc = o_noDoc;
    const std::string& convertOut = o_convertOut;
    const std::string& copyInner = o_copyInner;
    const bool& ignored = o_ignored;

    // we don't need to split name since ::DataType::Make simply joins
    // them back together
    ::DataTypeConstSP self = ::DataType::Make(
        name, "", nsService, cppName, outerType, innerType, innerRefType, publicType,
        objectName, isClosed, noDoc, convertIn, convertOut, copyInner,
        ::DataTypeConstSP(), false, false, ignored);

    return self;
}

DataType::inner_type DataType::get_inner() const
{
    return inner_value;
}

DataType::inner_type DataType::get_inner(const DataType::outer_type& o)
{
    if (!o)
        return inner_type();
    return o->get_inner();
}

void DataType::set_inner(const spi_boost::intrusive_ptr< ::DataType const >& self)
{
    SPI_PRE_CONDITION(!inner_value);
    inner_value = self;
}

DataType::~DataType()
{}

DataTypeConstSP DataType::Wrap(const inner_type& inner)
{
    if (!inner)
        return DataTypeConstSP();
    return DataTypeConstSP(new DataType(inner));
}

DataType::DataType(const inner_type& inner)
{
    set_inner(inner);
}

std::string DataType::name() const
{
    inner_type self = get_inner();

    return self->name();
}

std::string DataType::nsService() const
{
    inner_type self = get_inner();

    return self->nsService();
}

std::string DataType::cppName() const
{
    inner_type self = get_inner();

    return self->cppName();
}

std::string DataType::outerType() const
{
    inner_type self = get_inner();

    return self->outerType();
}

std::string DataType::innerType() const
{
    inner_type self = get_inner();

    return self->innerType();
}

std::string DataType::innerRefType() const
{
    inner_type self = get_inner();

    return self->innerRefType();
}

PublicType DataType::publicType() const
{
    spdoc::PublicType::Enum i_value = DataType_Helper::get_publicType(this);
    return PublicType_convert_out(i_value);
}

spdoc::PublicType::Enum DataType_Helper::get_publicType(const DataType* o)
{
    DataType::inner_type self = o->get_inner();
    return self->publicType();
}

std::string DataType::objectName() const
{
    inner_type self = get_inner();

    return self->objectName();
}

bool DataType::isClosed() const
{
    inner_type self = get_inner();

    return self->isClosed();
}

bool DataType::noDoc() const
{
    inner_type self = get_inner();

    return self->noDoc();
}

InputConverterConstSP DataType::convertIn() const
{
    spi_boost::intrusive_ptr< ::InputConverter const > i_value = DataType_Helper::get_convertIn(this);
    return InputConverter::Wrap(i_value);
}

spi_boost::intrusive_ptr< ::InputConverter const > DataType_Helper::get_convertIn(const DataType* o)
{
    DataType::inner_type self = o->get_inner();
    return self->convertIn();
}

std::string DataType::convertOut() const
{
    inner_type self = get_inner();

    return self->convertOut();
}

std::string DataType::copyInner() const
{
    inner_type self = get_inner();

    return self->copyInner();
}

bool DataType::ignored() const
{
    inner_type self = get_inner();

    return self->ignored();
}

/*
****************************************************************************
* Implementation of Attribute
****************************************************************************
*/
AttributeConstSP Attribute::Make(
    const DataTypeConstSP& dataType,
    const std::string& name,
    int arrayDim)
{
  SPI_PROFILE("types.Attribute.Make");
  types_check_permission();
  try
  {
    inner_type self = make_inner(dataType, name, arrayDim);
    return Wrap(self);
  }
  catch (std::exception& e)
  { SPI_THROW_RUNTIME_ERROR("Could not create Attribute object:\n" << e.what()); }
  catch (...)
  { SPI_THROW_RUNTIME_ERROR("Could not create Attribute object:\n" << "Unknown exception"); }
}

Attribute::inner_type Attribute::make_inner(
    const DataTypeConstSP& o_dataType,
    const std::string& o_name,
    int o_arrayDim)
{
    spi_boost::intrusive_ptr< ::DataType const > dataType;

    SPI_PRE_CONDITION(o_dataType);
    dataType = DataType::get_inner(o_dataType);

    const std::string& name = o_name;
    const int& arrayDim = o_arrayDim;

    ::AttributeConstSP self = ::Attribute::Make(
        std::vector<std::string>(),
        dataType,
        name,
        arrayDim);

    return self;
}

Attribute::inner_type Attribute::get_inner() const
{
    return inner_value;
}

Attribute::inner_type Attribute::get_inner(const Attribute::outer_type& o)
{
    if (!o)
        return inner_type();
    return o->get_inner();
}

void Attribute::set_inner(const spi_boost::intrusive_ptr< ::Attribute const >& self)
{
    SPI_PRE_CONDITION(!inner_value);
    inner_value = self;
}

Attribute::~Attribute()
{}

AttributeConstSP Attribute::Wrap(const inner_type& inner)
{
    if (!inner)
        return AttributeConstSP();
    return AttributeConstSP(new Attribute(inner));
}

Attribute::Attribute(const inner_type& inner)
{
    set_inner(inner);
}

DataTypeConstSP Attribute::dataType() const
{
    spi_boost::intrusive_ptr< ::DataType const > i_value = Attribute_Helper::get_dataType(this);
    return DataType::Wrap(i_value);
}

spi_boost::intrusive_ptr< ::DataType const > Attribute_Helper::get_dataType(const Attribute* o)
{
    Attribute::inner_type self = o->get_inner();
    return self->dataType();
}

std::string Attribute::name() const
{
    inner_type self = get_inner();

    return self->name();
}

int Attribute::arrayDim() const
{
    inner_type self = get_inner();

    return self->arrayDim();
}

/*
****************************************************************************
* Implementation of ClassProperty
****************************************************************************
*/
ClassPropertyConstSP ClassProperty::Make(
    const AttributeConstSP& attribute)
{
  SPI_PROFILE("types.ClassProperty.Make");
  types_check_permission();
  try
  {
    inner_type self = make_inner(attribute);
    return Wrap(self);
  }
  catch (std::exception& e)
  { SPI_THROW_RUNTIME_ERROR("Could not create ClassProperty object:\n" << e.what()); }
  catch (...)
  { SPI_THROW_RUNTIME_ERROR("Could not create ClassProperty object:\n" << "Unknown exception"); }
}

ClassProperty::inner_type ClassProperty::make_inner(
    const AttributeConstSP& o_attribute)
{
    spi_boost::intrusive_ptr< ::Attribute const > attribute;

    SPI_PRE_CONDITION(o_attribute);
    attribute = Attribute::get_inner(o_attribute);

    return ::ClassAttribute::Make(attribute,
        ::ClassAttributeAccess::PROPERTY,
        ::VerbatimConstSP(),
        false,
        false,
        std::string(),
        false,
        ::ConverterConstSP());
}

ClassProperty::inner_type ClassProperty::get_inner() const
{
    return inner_value;
}

ClassProperty::inner_type ClassProperty::get_inner(const ClassProperty::outer_type& o)
{
    if (!o)
        return inner_type();
    return o->get_inner();
}

void ClassProperty::set_inner(const spi_boost::intrusive_ptr< ::ClassAttribute const >& self)
{
    SPI_PRE_CONDITION(!inner_value);
    inner_value = self;
}

ClassProperty::~ClassProperty()
{}

ClassPropertyConstSP ClassProperty::Wrap(const inner_type& inner)
{
    if (!inner)
        return ClassPropertyConstSP();
    return ClassPropertyConstSP(new ClassProperty(inner));
}

ClassProperty::ClassProperty(const inner_type& inner)
{
    set_inner(inner);
}

AttributeConstSP ClassProperty::attribute() const
{
    spi_boost::intrusive_ptr< ::Attribute const > i_value = ClassProperty_Helper::get_attribute(this);
    return Attribute::Wrap(i_value);
}

spi_boost::intrusive_ptr< ::Attribute const > ClassProperty_Helper::get_attribute(const ClassProperty* o)
{
    ClassProperty::inner_type self = o->get_inner();
return self->attribute(); }

/*
****************************************************************************
* Implementation of Enumerand
****************************************************************************
*/
EnumerandConstSP Enumerand::Make(
    const std::string& code,
    const std::vector<std::string>& strings)
{
  SPI_PROFILE("types.Enumerand.Make");
  types_check_permission();
  try
  {
    inner_type self = make_inner(code, strings);
    return Wrap(self);
  }
  catch (std::exception& e)
  { SPI_THROW_RUNTIME_ERROR("Could not create Enumerand object:\n" << e.what()); }
  catch (...)
  { SPI_THROW_RUNTIME_ERROR("Could not create Enumerand object:\n" << "Unknown exception"); }
}

Enumerand::inner_type Enumerand::make_inner(
    const std::string& o_code,
    const std::vector<std::string>& o_strings)
{

    const std::string& code = o_code;
    const std::vector<std::string>& strings = o_strings;

    std::vector<std::string> description;
    std::string value;
    std::vector<std::string> alternates;
    for (size_t i = 0; i < strings.size(); ++i)
    {
        if (i == 0)
            value = strings[i];
        else
            alternates.push_back(strings[i]);
    }

    return ::Enumerand::Make(description, code, value, alternates);
}

Enumerand::inner_type Enumerand::get_inner() const
{
    return inner_value;
}

Enumerand::inner_type Enumerand::get_inner(const Enumerand::outer_type& o)
{
    if (!o)
        return inner_type();
    return o->get_inner();
}

void Enumerand::set_inner(const spi_boost::intrusive_ptr< ::Enumerand const >& self)
{
    SPI_PRE_CONDITION(!inner_value);
    inner_value = self;
}

Enumerand::~Enumerand()
{}

EnumerandConstSP Enumerand::Wrap(const inner_type& inner)
{
    if (!inner)
        return EnumerandConstSP();
    return EnumerandConstSP(new Enumerand(inner));
}

Enumerand::Enumerand(const inner_type& inner)
    : spi::Object(true)
{
    set_inner(inner);
}

std::string Enumerand::code() const
{
    inner_type self = get_inner();

     return self->name();
}

std::vector<std::string> Enumerand::strings() const
{
    inner_type self = get_inner();

    std::vector<std::string> strings;
    strings.push_back(self->value());
    const std::vector<std::string> alternates = self->alternates();
    for (size_t i = 0; i < alternates.size(); ++i)
        strings.push_back(alternates[i]);
    return strings;
}

/*
****************************************************************************
* Implementation of Enum
****************************************************************************
*/
EnumConstSP Enum::Make(
    const std::string& name,
    const std::vector<EnumerandConstSP>& enumerands)
{
  SPI_PROFILE("types.Enum.Make");
  types_check_permission();
  try
  {
    inner_type self = make_inner(name, enumerands);
    return Wrap(self);
  }
  catch (std::exception& e)
  { SPI_THROW_RUNTIME_ERROR("Could not create Enum object:\n" << e.what()); }
  catch (...)
  { SPI_THROW_RUNTIME_ERROR("Could not create Enum object:\n" << "Unknown exception"); }
}

Enum::inner_type Enum::make_inner(
    const std::string& o_name,
    const std::vector<EnumerandConstSP>& o_enumerands)
{
    std::vector< spi_boost::intrusive_ptr< ::Enumerand const > > enumerands;

    for (size_t i_ = 0; i_ < o_enumerands.size(); ++i_)
    {
        SPI_PRE_CONDITION(o_enumerands[i_]);
        enumerands.push_back(Enumerand::get_inner(o_enumerands[i_]));
    }

    const std::string& name = o_name;

    // is there an issue with ns?
    std::vector<std::string> description;
    std::string ns;
    std::string innerName;
    std::string innerHeader;
    std::string enumTypedef;
    std::vector<EnumConstructorConstSP> constructors;
    EnumBitmaskConstSP bitmask;

    return ::Enum::Make(
        description, name, ns, innerName, innerHeader, enumTypedef, enumerands,
        constructors, bitmask);
}

Enum::inner_type Enum::get_inner() const
{
    return inner_value;
}

Enum::inner_type Enum::get_inner(const Enum::outer_type& o)
{
    if (!o)
        return inner_type();
    return o->get_inner();
}

void Enum::set_inner(const spi_boost::intrusive_ptr< ::Enum const >& self)
{
    SPI_PRE_CONDITION(!inner_value);
    inner_value = self;
}

Enum::~Enum()
{}

EnumConstSP Enum::Wrap(const inner_type& inner)
{
    if (!inner)
        return EnumConstSP();
    return EnumConstSP(new Enum(inner));
}

Enum::Enum(const inner_type& inner)
{
    set_inner(inner);
}

std::string Enum::name() const
{
    inner_type self = get_inner();

    return self->name();
}

std::vector<EnumerandConstSP> Enum::enumerands() const
{
    const std::vector< spi_boost::intrusive_ptr< ::Enumerand const > >& i_values = Enum_Helper::get_enumerands(this);

    std::vector<EnumerandConstSP> values;

    for (size_t i_ = 0; i_ < i_values.size(); ++i_)
        values.push_back(Enumerand::Wrap(i_values[i_]));

    return values;
}

std::vector< spi_boost::intrusive_ptr< ::Enumerand const > > Enum_Helper::get_enumerands(const Enum* o)
{
    Enum::inner_type self = o->get_inner();
    return self->enumerands();
}

/*
****************************************************************************
* Implementation of BaseClass
****************************************************************************
*/

BaseClass::inner_type BaseClass::get_inner() const
{
    return inner_value;
}

BaseClass::inner_type BaseClass::get_inner(const BaseClass::outer_type& o)
{
    if (!o)
        return inner_type();
    return o->get_inner();
}

void BaseClass::set_inner(const spi_boost::intrusive_ptr< ::Class const >& self)
{
    SPI_PRE_CONDITION(!inner_value);
    inner_value = self;
}

BaseClass::BaseClass()
{}

BaseClass::~BaseClass()
{}

BaseClassConstSP BaseClass::Wrap(const inner_type& inner)
{
    if (!inner)
        return BaseClassConstSP();
    for (size_t i = 0; i < g_sub_class_wrappers.size(); ++i)
    {
        BaseClassConstSP o = g_sub_class_wrappers[i](inner);
        if (o)
            return o;
    }
    throw spi::RuntimeError("%s: Failed!", __FUNCTION__);
}

/*
****************************************************************************
* Implementation of BaseStruct
****************************************************************************
*/
BaseStructConstSP BaseStruct::Make(
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
    bool useAccessors)
{
  SPI_PROFILE("types.BaseStruct.Make");
  types_check_permission();
  try
  {
    inner_type self = make_inner(description, name, ns, baseClass, noMake,
        objectName, canPut, noId, isVirtual, asValue, uuid, byValue,
        useAccessors);
    return Wrap(self);
  }
  catch (std::exception& e)
  { SPI_THROW_RUNTIME_ERROR("Could not create BaseStruct object:\n" << e.what()); }
  catch (...)
  { SPI_THROW_RUNTIME_ERROR("Could not create BaseStruct object:\n" << "Unknown exception"); }
}

BaseStruct::inner_type BaseStruct::make_inner(
    const std::vector<std::string>& o_description,
    const std::string& o_name,
    const std::string& o_ns,
    const BaseClassConstSP& o_baseClass,
    bool o_noMake,
    const std::string& o_objectName,
    bool o_canPut,
    bool o_noId,
    bool o_isVirtual,
    bool o_asValue,
    bool o_uuid,
    bool o_byValue,
    bool o_useAccessors)
{
    spi_boost::intrusive_ptr< ::Class const > baseClass;

    baseClass = BaseClass::get_inner(o_baseClass);

    const std::vector<std::string>& description = o_description;
    const std::string& name = o_name;
    const std::string& ns = o_ns;
    const bool& noMake = o_noMake;
    const std::string& objectName = o_objectName;
    const bool& canPut = o_canPut;
    const bool& noId = o_noId;
    const bool& isVirtual = o_isVirtual;
    const bool& asValue = o_asValue;
    const bool& uuid = o_uuid;
    const bool& byValue = o_byValue;
    const bool& useAccessors = o_useAccessors;

    bool incomplete = false;
    std::string constructor;

    ::StructSP self = ::Struct::Make(
        description, name, ns, baseClass, noMake, objectName, canPut, noId, isVirtual, asValue,
        uuid, byValue, useAccessors, incomplete, constructor);

    SPI_POST_CONDITION(self->isAbstract());

    return self;
}

BaseStruct::inner_type BaseStruct::get_inner() const
{
    return inner_value;
}

BaseStruct::inner_type BaseStruct::get_inner(const BaseStruct::outer_type& o)
{
    if (!o)
        return inner_type();
    return o->get_inner();
}

void BaseStruct::set_inner(const spi_boost::intrusive_ptr< ::Struct const >& self)
{
    SPI_PRE_CONDITION(!inner_value);
    inner_value = self;
    BaseClass::set_inner(self);
}

BaseStruct::~BaseStruct()
{}

BaseClassConstSP BaseStruct::BaseWrap(const BaseClass::inner_type& baseInner)
{
    inner_type inner = spi_boost::dynamic_pointer_cast< ::Struct const >(baseInner);
    if (!inner)
        return BaseClassConstSP();
    return Wrap(inner);
}

BaseStructConstSP BaseStruct::Wrap(const inner_type& inner)
{
    if (!inner)
        return BaseStructConstSP();
    return BaseStructConstSP(new BaseStruct(inner));
}

BaseStruct::BaseStruct(const inner_type& inner)
{
    set_inner(inner);
}

std::vector<std::string> BaseStruct::description() const
{
    inner_type self = get_inner();

    return self->description();
}

std::string BaseStruct::name() const
{
    inner_type self = get_inner();

    return self->name();
}

std::string BaseStruct::ns() const
{
    inner_type self = get_inner();

    return self->ns();
}

BaseClassConstSP BaseStruct::baseClass() const
{
    spi_boost::intrusive_ptr< ::Class const > i_value = BaseStruct_Helper::get_baseClass(this);
    return BaseClass::Wrap(i_value);
}

spi_boost::intrusive_ptr< ::Class const > BaseStruct_Helper::get_baseClass(const BaseStruct* o)
{
    BaseStruct::inner_type self = o->get_inner();
    return self->baseClass();
}

bool BaseStruct::noMake() const
{
    inner_type self = get_inner();

    return self->noMake();
}

std::string BaseStruct::objectName() const
{
    inner_type self = get_inner();

    return self->objectName();
}

bool BaseStruct::canPut() const
{
    inner_type self = get_inner();

    return self->canPut();
}

bool BaseStruct::noId() const
{
    inner_type self = get_inner();

    return self->noId();
}

bool BaseStruct::isVirtual() const
{
    inner_type self = get_inner();

    return self->isVirtual();
}

bool BaseStruct::asValue() const
{
    inner_type self = get_inner();

    return self->asValue();
}

bool BaseStruct::uuid() const
{
    inner_type self = get_inner();

    return self->uuid();
}

bool BaseStruct::byValue() const
{
    inner_type self = get_inner();

    return self->byValue();
}

bool BaseStruct::useAccessors() const
{
    inner_type self = get_inner();

    return self->useAccessors();
}

/*
****************************************************************************
* Implementation of InnerClass
****************************************************************************
*/
InnerClassConstSP InnerClass::Make(
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
    bool allowConst)
{
  SPI_PROFILE("types.InnerClass.Make");
  types_check_permission();
  try
  {
    inner_type self = make_inner(typeName, ns, freeFunc, copyFunc,
        preDeclaration, sharedPtr, isShared, isConst, isOpen, isStruct,
        isCached, isTemplate, byValue, boolTest, allowConst);
    return Wrap(self);
  }
  catch (std::exception& e)
  { SPI_THROW_RUNTIME_ERROR("Could not create InnerClass object:\n" << e.what()); }
  catch (...)
  { SPI_THROW_RUNTIME_ERROR("Could not create InnerClass object:\n" << "Unknown exception"); }
}

InnerClass::inner_type InnerClass::make_inner(
    const std::string& o_typeName,
    const std::string& o_ns,
    const std::string& o_freeFunc,
    const std::string& o_copyFunc,
    const std::string& o_preDeclaration,
    const std::string& o_sharedPtr,
    bool o_isShared,
    bool o_isConst,
    bool o_isOpen,
    bool o_isStruct,
    bool o_isCached,
    bool o_isTemplate,
    bool o_byValue,
    const std::string& o_boolTest,
    bool o_allowConst)
{

    const std::string& typeName = o_typeName;
    const std::string& ns = o_ns;
    const std::string& freeFunc = o_freeFunc;
    const std::string& copyFunc = o_copyFunc;
    const std::string& preDeclaration = o_preDeclaration;
    const std::string& sharedPtr = o_sharedPtr;
    const bool& isShared = o_isShared;
    const bool& isConst = o_isConst;
    const bool& isOpen = o_isOpen;
    const bool& isStruct = o_isStruct;
    const bool& isCached = o_isCached;
    const bool& isTemplate = o_isTemplate;
    const bool& byValue = o_byValue;
    const std::string& boolTest = o_boolTest;
    const bool& allowConst = o_allowConst;

    ::InnerClassConstSP self = ::InnerClass::Make(
        typeName, ns, freeFunc, copyFunc, preDeclaration, sharedPtr,
        isShared, isConst, isOpen, isStruct, isCached, isTemplate, byValue, boolTest, allowConst);
    return self;
}

InnerClass::inner_type InnerClass::get_inner() const
{
    return inner_value;
}

InnerClass::inner_type InnerClass::get_inner(const InnerClass::outer_type& o)
{
    if (!o)
        return inner_type();
    return o->get_inner();
}

void InnerClass::set_inner(const spi_boost::intrusive_ptr< ::InnerClass const >& self)
{
    SPI_PRE_CONDITION(!inner_value);
    inner_value = self;
}

InnerClass::~InnerClass()
{}

InnerClassConstSP InnerClass::Wrap(const inner_type& inner)
{
    if (!inner)
        return InnerClassConstSP();
    return InnerClassConstSP(new InnerClass(inner));
}

InnerClass::InnerClass(const inner_type& inner)
{
    set_inner(inner);
}

std::string InnerClass::typeName() const
{
    inner_type self = get_inner();

    return self->typeName();
}

std::string InnerClass::ns() const
{
    inner_type self = get_inner();

    return self->ns();
}

std::string InnerClass::freeFunc() const
{
    inner_type self = get_inner();

    return self->freeFunc();
}

std::string InnerClass::copyFunc() const
{
    inner_type self = get_inner();

    return self->copyFunc();
}

std::string InnerClass::preDeclaration() const
{
    inner_type self = get_inner();

    return self->preDeclaration();
}

std::string InnerClass::sharedPtr() const
{
    inner_type self = get_inner();

    return self->sharedPtr();
}

bool InnerClass::isShared() const
{
    inner_type self = get_inner();

    return self->isShared();
}

bool InnerClass::isConst() const
{
    inner_type self = get_inner();

    return self->isConst();
}

bool InnerClass::isOpen() const
{
    inner_type self = get_inner();

    return self->isOpen();
}

bool InnerClass::isStruct() const
{
    inner_type self = get_inner();

    return self->isStruct();
}

bool InnerClass::isCached() const
{
    inner_type self = get_inner();

    return self->isCached();
}

bool InnerClass::isTemplate() const
{
    inner_type self = get_inner();

    return self->isTemplate();
}

bool InnerClass::byValue() const
{
    inner_type self = get_inner();

    return self->byValue();
}

std::string InnerClass::boolTest() const
{
    inner_type self = get_inner();

    return self->boolTest();
}

bool InnerClass::allowConst() const
{
    inner_type self = get_inner();

    return self->allowConst();
}

/*
****************************************************************************
* Implementation of BaseWrapperClass
****************************************************************************
*/
BaseWrapperClassConstSP BaseWrapperClass::Make(
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
    bool uuid)
{
  SPI_PROFILE("types.BaseWrapperClass.Make");
  types_check_permission();
  try
  {
    inner_type self = make_inner(description, name, ns, innerClass, baseClass,
        isVirtual, noMake, objectName, isDelegate, canPut, noId, dataType,
        asValue, classProperties, uuid);
    return Wrap(self);
  }
  catch (std::exception& e)
  { SPI_THROW_RUNTIME_ERROR("Could not create BaseWrapperClass object:\n" << e.what()); }
  catch (...)
  { SPI_THROW_RUNTIME_ERROR("Could not create BaseWrapperClass object:\n" << "Unknown exception"); }
}

BaseWrapperClass::inner_type BaseWrapperClass::make_inner(
    const std::vector<std::string>& o_description,
    const std::string& o_name,
    const std::string& o_ns,
    const InnerClassConstSP& o_innerClass,
    const BaseWrapperClassConstSP& o_baseClass,
    bool o_isVirtual,
    bool o_noMake,
    const std::string& o_objectName,
    bool o_isDelegate,
    bool o_canPut,
    bool o_noId,
    const DataTypeConstSP& o_dataType,
    bool o_asValue,
    const std::vector<ClassPropertyConstSP>& o_classProperties,
    bool o_uuid)
{
    spi_boost::intrusive_ptr< ::InnerClass const > innerClass;
    spi_boost::intrusive_ptr< ::WrapperClass const > baseClass;
    spi_boost::intrusive_ptr< ::DataType const > dataType;
    std::vector< spi_boost::intrusive_ptr< ::ClassAttribute const > > classProperties;

    SPI_PRE_CONDITION(o_innerClass);
    innerClass = InnerClass::get_inner(o_innerClass);
    baseClass = BaseWrapperClass::get_inner(o_baseClass);
    SPI_PRE_CONDITION(o_dataType);
    dataType = DataType::get_inner(o_dataType);
    for (size_t i_ = 0; i_ < o_classProperties.size(); ++i_)
    {
        SPI_PRE_CONDITION(o_classProperties[i_]);
        classProperties.push_back(ClassProperty::get_inner(o_classProperties[i_]));
    }

    const std::vector<std::string>& description = o_description;
    const std::string& name = o_name;
    const std::string& ns = o_ns;
    const bool& isVirtual = o_isVirtual;
    const bool& noMake = o_noMake;
    const std::string& objectName = o_objectName;
    const bool& isDelegate = o_isDelegate;
    const bool& canPut = o_canPut;
    const bool& noId = o_noId;
    const bool& asValue = o_asValue;
    const bool& uuid = o_uuid;

    bool incomplete = false;
    std::string accessorFormat;
    std::string propertyFormat;
    std::string constructor;

    ::WrapperClassSP self = ::WrapperClass::Make(
        description, name, ns, innerClass, baseClass, isVirtual,
        noMake, objectName, isDelegate, canPut, noId, asValue, uuid,
        incomplete, accessorFormat, propertyFormat, constructor);

    SPI_POST_CONDITION(self->isAbstract());

    self->setDataType(dataType);

    for (size_t i = 0; i < classProperties.size(); ++i)
        self->addClassAttribute(classProperties[i]);

    return self;
}

BaseWrapperClass::inner_type BaseWrapperClass::get_inner() const
{
    return inner_value;
}

BaseWrapperClass::inner_type BaseWrapperClass::get_inner(const BaseWrapperClass::outer_type& o)
{
    if (!o)
        return inner_type();
    return o->get_inner();
}

void BaseWrapperClass::set_inner(const spi_boost::intrusive_ptr< ::WrapperClass const >& self)
{
    SPI_PRE_CONDITION(!inner_value);
    inner_value = self;
    BaseClass::set_inner(self);
}

BaseWrapperClass::~BaseWrapperClass()
{}

BaseClassConstSP BaseWrapperClass::BaseWrap(const BaseClass::inner_type& baseInner)
{
    inner_type inner = spi_boost::dynamic_pointer_cast< ::WrapperClass const >(baseInner);
    if (!inner)
        return BaseClassConstSP();
    return Wrap(inner);
}

BaseWrapperClassConstSP BaseWrapperClass::Wrap(const inner_type& inner)
{
    if (!inner)
        return BaseWrapperClassConstSP();
    return BaseWrapperClassConstSP(new BaseWrapperClass(inner));
}

BaseWrapperClass::BaseWrapperClass(const inner_type& inner)
{
    set_inner(inner);
}

std::vector<std::string> BaseWrapperClass::description() const
{
    inner_type self = get_inner();

    return self->description();
}

std::string BaseWrapperClass::name() const
{
    inner_type self = get_inner();

    return self->name();
}

std::string BaseWrapperClass::ns() const
{
    inner_type self = get_inner();

    return self->ns();
}

InnerClassConstSP BaseWrapperClass::innerClass() const
{
    spi_boost::intrusive_ptr< ::InnerClass const > i_value = BaseWrapperClass_Helper::get_innerClass(this);
    return InnerClass::Wrap(i_value);
}

spi_boost::intrusive_ptr< ::InnerClass const > BaseWrapperClass_Helper::get_innerClass(const BaseWrapperClass* o)
{
    BaseWrapperClass::inner_type self = o->get_inner();
    return self->innerClass();
}

BaseWrapperClassConstSP BaseWrapperClass::baseClass() const
{
    spi_boost::intrusive_ptr< ::WrapperClass const > i_value = BaseWrapperClass_Helper::get_baseClass(this);
    return BaseWrapperClass::Wrap(i_value);
}

spi_boost::intrusive_ptr< ::WrapperClass const > BaseWrapperClass_Helper::get_baseClass(const BaseWrapperClass* o)
{
    BaseWrapperClass::inner_type self = o->get_inner();
    return self->baseClass();
}

bool BaseWrapperClass::isVirtual() const
{
    inner_type self = get_inner();

    return self->isVirtual();
}

bool BaseWrapperClass::noMake() const
{
    inner_type self = get_inner();

    return self->noMake();
}

std::string BaseWrapperClass::objectName() const
{
    inner_type self = get_inner();

    return self->objectName();
}

bool BaseWrapperClass::isDelegate() const
{
    inner_type self = get_inner();

    return self->isDelegate();
}

bool BaseWrapperClass::canPut() const
{
    inner_type self = get_inner();

    return self->canPut();
}

bool BaseWrapperClass::noId() const
{
    inner_type self = get_inner();

    return self->noId();
}

DataTypeConstSP BaseWrapperClass::dataType() const
{
    spi_boost::intrusive_ptr< ::DataType const > i_value = BaseWrapperClass_Helper::get_dataType(this);
    return DataType::Wrap(i_value);
}

spi_boost::intrusive_ptr< ::DataType const > BaseWrapperClass_Helper::get_dataType(const BaseWrapperClass* o)
{
    BaseWrapperClass::inner_type self = o->get_inner();
    return self->dataType();
}

bool BaseWrapperClass::asValue() const
{
    inner_type self = get_inner();

    return self->asValue();
}

std::vector<ClassPropertyConstSP> BaseWrapperClass::classProperties() const
{
    const std::vector< spi_boost::intrusive_ptr< ::ClassAttribute const > >& i_values = BaseWrapperClass_Helper::get_classProperties(this);

    std::vector<ClassPropertyConstSP> values;

    for (size_t i_ = 0; i_ < i_values.size(); ++i_)
        values.push_back(ClassProperty::Wrap(i_values[i_]));

    return values;
}

std::vector< spi_boost::intrusive_ptr< ::ClassAttribute const > > BaseWrapperClass_Helper::get_classProperties(const BaseWrapperClass* o)
{
    BaseWrapperClass::inner_type self = o->get_inner();
    return self->classProperties();
}

bool BaseWrapperClass::uuid() const
{
    inner_type self = get_inner();

    return self->uuid();
}

/*
****************************************************************************
* Implementation of TypesLibrary
****************************************************************************
*/
TypesLibraryConstSP TypesLibrary::Make(
    const std::string& name,
    const std::string& ns,
    const std::string& version,
    const std::string& lastModuleName,
    const std::vector<DataTypeConstSP>& dataTypes,
    const std::vector<DataTypeConstSP>& publicDataTypes,
    const std::vector<BaseClassConstSP>& baseClasses,
    const std::vector<EnumConstSP>& enums)
{
  SPI_PROFILE("types.TypesLibrary.Make");
  types_check_permission();
  try
  {
    inner_type self = make_inner(name, ns, version, lastModuleName, dataTypes,
        publicDataTypes, baseClasses, enums);
    return Wrap(self);
  }
  catch (std::exception& e)
  { SPI_THROW_RUNTIME_ERROR("Could not create TypesLibrary object:\n" << e.what()); }
  catch (...)
  { SPI_THROW_RUNTIME_ERROR("Could not create TypesLibrary object:\n" << "Unknown exception"); }
}

TypesLibrary::inner_type TypesLibrary::make_inner(
    const std::string& o_name,
    const std::string& o_ns,
    const std::string& o_version,
    const std::string& o_lastModuleName,
    const std::vector<DataTypeConstSP>& o_dataTypes,
    const std::vector<DataTypeConstSP>& o_publicDataTypes,
    const std::vector<BaseClassConstSP>& o_baseClasses,
    const std::vector<EnumConstSP>& o_enums)
{
    std::vector< spi_boost::intrusive_ptr< ::DataType const > > dataTypes;
    std::vector< spi_boost::intrusive_ptr< ::DataType const > > publicDataTypes;
    std::vector< spi_boost::intrusive_ptr< ::Class const > > baseClasses;
    std::vector< spi_boost::intrusive_ptr< ::Enum const > > enums;

    for (size_t i_ = 0; i_ < o_dataTypes.size(); ++i_)
    {
        SPI_PRE_CONDITION(o_dataTypes[i_]);
        dataTypes.push_back(DataType::get_inner(o_dataTypes[i_]));
    }
    for (size_t i_ = 0; i_ < o_publicDataTypes.size(); ++i_)
    {
        SPI_PRE_CONDITION(o_publicDataTypes[i_]);
        publicDataTypes.push_back(DataType::get_inner(o_publicDataTypes[i_]));
    }
    for (size_t i_ = 0; i_ < o_baseClasses.size(); ++i_)
    {
        SPI_PRE_CONDITION(o_baseClasses[i_]);
        baseClasses.push_back(BaseClass::get_inner(o_baseClasses[i_]));
    }
    for (size_t i_ = 0; i_ < o_enums.size(); ++i_)
    {
        SPI_PRE_CONDITION(o_enums[i_]);
        enums.push_back(Enum::get_inner(o_enums[i_]));
    }

    const std::string& name = o_name;
    const std::string& ns = o_ns;
    const std::string& version = o_version;
    const std::string& lastModuleName = o_lastModuleName;

    ::TypesLibraryConstSP self = ::TypesLibrary::Make(
        name, ns, version, lastModuleName, dataTypes, publicDataTypes, baseClasses, enums);
    return self;
}

TypesLibrary::inner_type TypesLibrary::get_inner() const
{
    return inner_value;
}

TypesLibrary::inner_type TypesLibrary::get_inner(const TypesLibrary::outer_type& o)
{
    if (!o)
        return inner_type();
    return o->get_inner();
}

void TypesLibrary::set_inner(const spi_boost::intrusive_ptr< ::TypesLibrary const >& self)
{
    SPI_PRE_CONDITION(!inner_value);
    inner_value = self;
}

TypesLibrary::~TypesLibrary()
{}

TypesLibraryConstSP TypesLibrary::Wrap(const inner_type& inner)
{
    if (!inner)
        return TypesLibraryConstSP();
    return TypesLibraryConstSP(new TypesLibrary(inner));
}

TypesLibrary::TypesLibrary(const inner_type& inner)
{
    set_inner(inner);
}

std::string TypesLibrary::name() const
{
    inner_type self = get_inner();

    return self->name();
}

std::string TypesLibrary::ns() const
{
    inner_type self = get_inner();

    return self->ns();
}

std::string TypesLibrary::version() const
{
    inner_type self = get_inner();

    return self->version();
}

std::string TypesLibrary::lastModuleName() const
{
    inner_type self = get_inner();

    return self->lastModuleName();
}

std::vector<DataTypeConstSP> TypesLibrary::dataTypes() const
{
    const std::vector< spi_boost::intrusive_ptr< ::DataType const > >& i_values = TypesLibrary_Helper::get_dataTypes(this);

    std::vector<DataTypeConstSP> values;

    for (size_t i_ = 0; i_ < i_values.size(); ++i_)
        values.push_back(DataType::Wrap(i_values[i_]));

    return values;
}

std::vector< spi_boost::intrusive_ptr< ::DataType const > > TypesLibrary_Helper::get_dataTypes(const TypesLibrary* o)
{
    TypesLibrary::inner_type self = o->get_inner();
    return self->dataTypes();
}

std::vector<DataTypeConstSP> TypesLibrary::publicDataTypes() const
{
    const std::vector< spi_boost::intrusive_ptr< ::DataType const > >& i_values = TypesLibrary_Helper::get_publicDataTypes(this);

    std::vector<DataTypeConstSP> values;

    for (size_t i_ = 0; i_ < i_values.size(); ++i_)
        values.push_back(DataType::Wrap(i_values[i_]));

    return values;
}

std::vector< spi_boost::intrusive_ptr< ::DataType const > > TypesLibrary_Helper::get_publicDataTypes(const TypesLibrary* o)
{
    TypesLibrary::inner_type self = o->get_inner();
    return self->publicDataTypes();
}

std::vector<BaseClassConstSP> TypesLibrary::baseClasses() const
{
    const std::vector< spi_boost::intrusive_ptr< ::Class const > >& i_values = TypesLibrary_Helper::get_baseClasses(this);

    std::vector<BaseClassConstSP> values;

    for (size_t i_ = 0; i_ < i_values.size(); ++i_)
        values.push_back(BaseClass::Wrap(i_values[i_]));

    return values;
}

std::vector< spi_boost::intrusive_ptr< ::Class const > > TypesLibrary_Helper::get_baseClasses(const TypesLibrary* o)
{
    TypesLibrary::inner_type self = o->get_inner();
    return self->baseClasses();
}

std::vector<EnumConstSP> TypesLibrary::enums() const
{
    const std::vector< spi_boost::intrusive_ptr< ::Enum const > >& i_values = TypesLibrary_Helper::get_enums(this);

    std::vector<EnumConstSP> values;

    for (size_t i_ = 0; i_ < i_values.size(); ++i_)
        values.push_back(Enum::Wrap(i_values[i_]));

    return values;
}

std::vector< spi_boost::intrusive_ptr< ::Enum const > > TypesLibrary_Helper::get_enums(const TypesLibrary* o)
{
    TypesLibrary::inner_type self = o->get_inner();
    return self->enums();
}

TYPES_END_NAMESPACE

