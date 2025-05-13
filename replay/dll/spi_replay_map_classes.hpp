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

#ifndef _SPI_REPLAY_MAP_CLASSES_HPP_
#define _SPI_REPLAY_MAP_CLASSES_HPP_

/**
****************************************************************************
* Header file: spi_replay_map_classes.hpp
****************************************************************************
*/

#include <spi/spi.hpp>
#include "replay_dll_decl_spec.h"
#include "replay_namespace.hpp"

SPI_REPLAY_BEGIN_NAMESPACE

/**
****************************************************************************
* Class ValueType containing enumerated type ValueType::Enum.
* Whenever ValueType is expected you can use ValueType::Enum,
* and vice versa, because automatic type conversion is provided by
* the constructor and cast operator.
*
* ValueType::UNDEFINED
* ValueType::CHAR
* ValueType::SHORT_STRING
* ValueType::STRING
* ValueType::INT
* ValueType::DOUBLE
* ValueType::BOOL
* ValueType::DATE
* ValueType::DATETIME
* ValueType::MAP
* ValueType::OBJECT
* ValueType::OBJECT_REF
* ValueType::ARRAY
* ValueType::ERROR
****************************************************************************
*/
class REPLAY_IMPORT ValueType
{
public:
    enum Enum
    {
        UNDEFINED,
        CHAR,
        SHORT_STRING,
        STRING,
        INT,
        DOUBLE,
        BOOL,
        DATE,
        DATETIME,
        MAP,
        OBJECT,
        OBJECT_REF,
        ARRAY,
        ERROR,
        UNINITIALIZED_VALUE
    };

    static spi::EnumInfo* get_enum_info();

    ValueType() : value(UNINITIALIZED_VALUE) {}
    ValueType(ValueType::Enum value) : value(value) {}
    ValueType(const char* str) : value(ValueType::from_string(str)) {}
    ValueType(const std::string& str) : value(ValueType::from_string(str.c_str())) {}
    ValueType(const spi::Value& value);
    ValueType(int value);

    operator ValueType::Enum() const { return value; }
    operator std::string() const { return to_string(); }
    operator spi::Value() const { return to_value(); }
    std::string to_string() const { return std::string(ValueType::to_string(value)); }
    spi::Value to_value() const { return spi::Value(to_string()); }

    static ValueType::Enum from_int(int);
    static ValueType::Enum from_string(const char*);
    static const char* to_string(ValueType::Enum);

private:
    ValueType::Enum value;
};

SPI_REPLAY_END_NAMESPACE

#endif /* _SPI_REPLAY_MAP_CLASSES_HPP_*/

