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

#ifndef _TYPES_TYPES_HELPER_HPP_
#define _TYPES_TYPES_HELPER_HPP_

/**
****************************************************************************
* Header file: types_types_helper.hpp
****************************************************************************
*/

#include "types_types.hpp"
#include "types_dll_type_converters.hpp"

TYPES_BEGIN_NAMESPACE

class InputConverter_Helper
{
public:
};

class InputConverterStringFormat_Helper
{
public:
};

class InputConverterClass_Helper
{
public:
};

class DataType_Helper
{
public:
    static spdoc::PublicType::Enum get_publicType(const DataType* o);
    static spi_boost::intrusive_ptr< ::InputConverter const > get_convertIn(const DataType* o);
};

class Attribute_Helper
{
public:
    static spi_boost::intrusive_ptr< ::DataType const > get_dataType(const Attribute* o);
};

class ClassProperty_Helper
{
public:
    static spi_boost::intrusive_ptr< ::Attribute const > get_attribute(const ClassProperty* o);
};

class Enumerand_Helper
{
public:
};

class Enum_Helper
{
public:
    static std::vector< spi_boost::intrusive_ptr< ::Enumerand const > > get_enumerands(const Enum* o);
};

class BaseClass_Helper
{
public:
};

class BaseStruct_Helper
{
public:
    static spi_boost::intrusive_ptr< ::Class const > get_baseClass(const BaseStruct* o);
};

class InnerClass_Helper
{
public:
};

class BaseWrapperClass_Helper
{
public:
    static spi_boost::intrusive_ptr< ::InnerClass const > get_innerClass(const BaseWrapperClass* o);
    static spi_boost::intrusive_ptr< ::WrapperClass const > get_baseClass(const BaseWrapperClass* o);
    static spi_boost::intrusive_ptr< ::DataType const > get_dataType(const BaseWrapperClass* o);
    static std::vector< spi_boost::intrusive_ptr< ::ClassAttribute const > > get_classProperties(const BaseWrapperClass* o);
};

class TypesLibrary_Helper
{
public:
    static std::vector< spi_boost::intrusive_ptr< ::DataType const > > get_dataTypes(const TypesLibrary* o);
    static std::vector< spi_boost::intrusive_ptr< ::DataType const > > get_publicDataTypes(const TypesLibrary* o);
    static std::vector< spi_boost::intrusive_ptr< ::Class const > > get_baseClasses(const TypesLibrary* o);
    static std::vector< spi_boost::intrusive_ptr< ::Enum const > > get_enums(const TypesLibrary* o);
};

void types_register_object_types(const spi::ServiceSP& svc);

TYPES_END_NAMESPACE

#endif /* _TYPES_TYPES_HELPER_HPP_*/

