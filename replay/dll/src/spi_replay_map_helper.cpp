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

/**
****************************************************************************
* Source file: spi_replay_map_helper.cpp
****************************************************************************
*/

#include "spi_replay_map.hpp"
#include "spi_replay_map_helper.hpp"
#include "replay_dll_service_manager.hpp"

SPI_REPLAY_BEGIN_NAMESPACE

spi::EnumInfo* ValueType::get_enum_info()
{
    static spi::EnumInfo the_info;
    if (!the_info.Initialised())
    {
        std::vector<std::string> enumerands;
        enumerands.push_back("UNDEFINED");
        enumerands.push_back("CHAR");
        enumerands.push_back("SHORT_STRING");
        enumerands.push_back("STRING");
        enumerands.push_back("INT");
        enumerands.push_back("DOUBLE");
        enumerands.push_back("BOOL");
        enumerands.push_back("DATE");
        enumerands.push_back("DATETIME");
        enumerands.push_back("MAP");
        enumerands.push_back("OBJECT");
        enumerands.push_back("OBJECT_REF");
        enumerands.push_back("ARRAY");
        enumerands.push_back("ERROR");
        the_info.Initialise("ValueType", enumerands);
    }
    return &the_info;
}

ValueType::Enum ValueType::from_string(const char* str)
{
    std::string uc_ = spi::StringUpper(str);
    if (uc_ == "")
        return ValueType::UNINITIALIZED_VALUE;
    switch(uc_[0])
    {
    case 'A':
        if (uc_ == "ARRAY")
            return ValueType::ARRAY;
        break;
    case 'B':
        if (uc_ == "BOOL")
            return ValueType::BOOL;
        break;
    case 'C':
        if (uc_ == "CHAR")
            return ValueType::CHAR;
        break;
    case 'D':
        if (uc_ == "DATE")
            return ValueType::DATE;
        if (uc_ == "DATETIME")
            return ValueType::DATETIME;
        if (uc_ == "DOUBLE")
            return ValueType::DOUBLE;
        break;
    case 'E':
        if (uc_ == "ERROR")
            return ValueType::ERROR;
        break;
    case 'I':
        if (uc_ == "INT")
            return ValueType::INT;
        break;
    case 'M':
        if (uc_ == "MAP")
            return ValueType::MAP;
        break;
    case 'O':
        if (uc_ == "OBJECT")
            return ValueType::OBJECT;
        if (uc_ == "OBJECT_REF")
            return ValueType::OBJECT_REF;
        break;
    case 'S':
        if (uc_ == "SHORT_STRING")
            return ValueType::SHORT_STRING;
        if (uc_ == "STRING")
            return ValueType::STRING;
        break;
    case 'U':
        if (uc_ == "UNDEFINED")
            return ValueType::UNDEFINED;
        break;
    }
    throw spi::RuntimeError("Cannot convert '%s' to ValueType. Possible values:\n"
       "'UNDEFINED', 'CHAR', 'SHORT_STRING', 'STRING', 'INT', 'DOUBLE', 'BOOL', 'DATE', 'DATETIME', 'MAP', 'OBJECT', 'OBJECT_REF', 'ARRAY', 'ERROR'",
        str);
}

const char* ValueType::to_string(ValueType::Enum v_)
{
    switch(v_)
    {
    case ValueType::UNDEFINED:
        return "UNDEFINED";
    case ValueType::CHAR:
        return "CHAR";
    case ValueType::SHORT_STRING:
        return "SHORT_STRING";
    case ValueType::STRING:
        return "STRING";
    case ValueType::INT:
        return "INT";
    case ValueType::DOUBLE:
        return "DOUBLE";
    case ValueType::BOOL:
        return "BOOL";
    case ValueType::DATE:
        return "DATE";
    case ValueType::DATETIME:
        return "DATETIME";
    case ValueType::MAP:
        return "MAP";
    case ValueType::OBJECT:
        return "OBJECT";
    case ValueType::OBJECT_REF:
        return "OBJECT_REF";
    case ValueType::ARRAY:
        return "ARRAY";
    case ValueType::ERROR:
        return "ERROR";
    case ValueType::UNINITIALIZED_VALUE:
        throw std::runtime_error("Uninitialized value for ValueType");
    default:
        throw std::runtime_error("Bad enumerated value");
    }
}

spi::Value MapClassName_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const spi::MapObjectSP& m =
        in_context->ValueToInstance<spi::MapObject>(in_values[0]);

    const std::string& o_result = spi_replay::MapClassName(m);
    return o_result;
}

spi::FunctionCaller MapClassName_FunctionCaller = {
    "MapClassName",
    1,
    {
        {"m", spi::ArgType::OBJECT, "Map", false, false, true}
    },
    MapClassName_caller
};

spi::ObjectType MapClassName_FunctionObjectType =
    spi::FunctionObjectType("spi_replay.MapClassName");

spi::Value MapFieldNames_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const spi::MapObjectSP& m =
        in_context->ValueToInstance<spi::MapObject>(in_values[0]);

    const std::vector<std::string>& o_result = spi_replay::MapFieldNames(m);
    return o_result;
}

spi::FunctionCaller MapFieldNames_FunctionCaller = {
    "MapFieldNames",
    1,
    {
        {"m", spi::ArgType::OBJECT, "Map", false, false, false}
    },
    MapFieldNames_caller
};

spi::ObjectType MapFieldNames_FunctionObjectType =
    spi::FunctionObjectType("spi_replay.MapFieldNames");

spi::Value MapRef_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const spi::MapObjectSP& m =
        in_context->ValueToInstance<spi::MapObject>(in_values[0]);

    int o_result = spi_replay::MapRef(m);
    return o_result;
}

spi::FunctionCaller MapRef_FunctionCaller = {
    "MapRef",
    1,
    {
        {"m", spi::ArgType::OBJECT, "Map", false, false, true}
    },
    MapRef_caller
};

spi::ObjectType MapRef_FunctionObjectType =
    spi::FunctionObjectType("spi_replay.MapRef");

spi::Value MapGetValue_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const spi::MapObjectSP& m =
        in_context->ValueToInstance<spi::MapObject>(in_values[0]);
    const std::string& name =
        in_context->ValueToString(in_values[1]);

    spi::Variant value;
    ValueType valueType;

    spi_replay::MapGetValue(m, name, value, valueType);

    std::vector<spi::Value> values_;
    values_.push_back(value.GetValue());
    values_.push_back(valueType);

    return spi::Value(values_);
}

spi::FunctionCaller MapGetValue_FunctionCaller = {
    "MapGetValue",
    2,
    {
        {"m", spi::ArgType::OBJECT, "Map", false, false, true},
        {"name", spi::ArgType::STRING, "string", false, false, true}
    },
    MapGetValue_caller
};

spi::ObjectType MapGetValue_FunctionObjectType =
    spi::FunctionObjectType("spi_replay.MapGetValue");

void map_register_object_types(const spi::ServiceSP& svc)
{
    svc->add_enum_info(ValueType::get_enum_info());
    svc->add_object_type(&MapClassName_FunctionObjectType);
    svc->add_function_caller(&MapClassName_FunctionCaller);
    svc->add_object_type(&MapFieldNames_FunctionObjectType);
    svc->add_function_caller(&MapFieldNames_FunctionCaller);
    svc->add_object_type(&MapRef_FunctionObjectType);
    svc->add_function_caller(&MapRef_FunctionCaller);
    svc->add_object_type(&MapGetValue_FunctionObjectType);
    svc->add_function_caller(&MapGetValue_FunctionCaller);
}

SPI_REPLAY_END_NAMESPACE

