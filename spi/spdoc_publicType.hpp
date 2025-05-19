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

#ifndef _SPDOC_PUBLICTYPE_HPP_
#define _SPDOC_PUBLICTYPE_HPP_

/**
****************************************************************************
* Header file: spdoc_publicType.hpp
****************************************************************************
*/

#include <spi/spi.hpp>
#include "spdoc_dll_decl_spec.h"
#include "spdoc_namespace.hpp"

SPDOC_BEGIN_NAMESPACE

/**
****************************************************************************
* Class PublicType containing enumerated type PublicType::Enum.
* Whenever PublicType is expected you can use PublicType::Enum,
* and vice versa, because automatic type conversion is provided by
* the constructor and cast operator.
*
* The public type describes the type of data from the end-user perspective.
*
* There are the primitive types, and two generic types ENUM and OBJECT.
* At present ENUM is seen as STRING with no further information available,
* but this will change in the future.
*
* PublicType::BOOL
*     Bool type - can take the values true or false.
* PublicType::CHAR
*     Single character - ideally should be a printable character.
* PublicType::INT
*     Integer - standard 32-bit integer. This should be large enough for
*     most purposes - the maximum number is over one billion.
* PublicType::DOUBLE
*     Floating point with double precision.
* PublicType::STRING
*     String - represented as the c++ std::string class.
* PublicType::DATE
*     Date - represented externally by the spi::Date class which supports
*     the standard calendar.
* PublicType::DATETIME
*     DateTime - represented externally by the spi::DateTime class which
*     supports the standard calendar + time (without timezone information)
* PublicType::ENUM_AS_STRING
*     Any enumerated type. Representally externally via its string
*     equivalent.
* PublicType::ENUM_AS_INT
*     Any enumerated type. Representally externally via its integer
*     equivalent.
* PublicType::CLASS
*     An object of a specific class.
* PublicType::OBJECT
*     A generic object.
* PublicType::MAP
*     A map object which simply consists of name/value pairs.
* PublicType::VARIANT
*     A variant - any of the above with late binding.
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
* Given a c++ typename, this function will verify that this is a valid
* primitive type, and return the corresponding PublicType value.
*
* Not all public types are primitive types, but all primitive types have
* a corresponding public type.
*
* @param typeName
*     This is the name of the c++ data type that we are intending to
*     verify.
****************************************************************************
*/
SPI_IMPORT
PublicType verifyPrimitiveTypeName(
    const std::string& typeName);

SPDOC_END_NAMESPACE

#endif /* _SPDOC_PUBLICTYPE_HPP_*/

