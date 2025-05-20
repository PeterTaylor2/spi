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
* Source file: spdoc_publicType_helper.cpp
****************************************************************************
*/

#include "spdoc_publicType.hpp"
#include "spdoc_publicType_helper.hpp"
#include "spdoc_dll_service_manager.hpp"

SPDOC_BEGIN_NAMESPACE

spi::EnumInfo* PublicType::get_enum_info()
{
    static spi::EnumInfo the_info;
    if (!the_info.Initialised())
    {
        std::vector<std::string> enumerands;
        enumerands.push_back("BOOL");
        enumerands.push_back("CHAR");
        enumerands.push_back("INT");
        enumerands.push_back("DOUBLE");
        enumerands.push_back("STRING");
        enumerands.push_back("DATE");
        enumerands.push_back("DATETIME");
        enumerands.push_back("ENUM_AS_STRING");
        enumerands.push_back("ENUM_AS_INT");
        enumerands.push_back("CLASS");
        enumerands.push_back("OBJECT");
        enumerands.push_back("MAP");
        enumerands.push_back("VARIANT");
        the_info.Initialise("PublicType", enumerands);
    }
    return &the_info;
}

PublicType::Enum PublicType::from_string(const char* str)
{
    std::string uc_ = spi::StringUpper(str);
    if (uc_ == "")
        return PublicType::UNINITIALIZED_VALUE;
    switch(uc_[0])
    {
    case 'B':
        if (uc_ == "BOOL")
            return PublicType::BOOL;
        break;
    case 'C':
        if (uc_ == "CHAR")
            return PublicType::CHAR;
        if (uc_ == "CLASS")
            return PublicType::CLASS;
        break;
    case 'D':
        if (uc_ == "DATE")
            return PublicType::DATE;
        if (uc_ == "DATETIME")
            return PublicType::DATETIME;
        if (uc_ == "DOUBLE")
            return PublicType::DOUBLE;
        break;
    case 'E':
        if (uc_ == "ENUM_AS_INT")
            return PublicType::ENUM_AS_INT;
        if (uc_ == "ENUM_AS_STRING")
            return PublicType::ENUM_AS_STRING;
        break;
    case 'I':
        if (uc_ == "INT")
            return PublicType::INT;
        break;
    case 'M':
        if (uc_ == "MAP")
            return PublicType::MAP;
        break;
    case 'O':
        if (uc_ == "OBJECT")
            return PublicType::OBJECT;
        break;
    case 'S':
        if (uc_ == "STRING")
            return PublicType::STRING;
        break;
    case 'V':
        if (uc_ == "VARIANT")
            return PublicType::VARIANT;
        break;
    }
    throw spi::RuntimeError("Cannot convert '%s' to PublicType. Possible values:\n"
       "'BOOL', 'CHAR', 'INT', 'DOUBLE', 'STRING', 'DATE', 'DATETIME', 'ENUM_AS_STRING', 'ENUM_AS_INT', 'CLASS', 'OBJECT', 'MAP', 'VARIANT'",
        str);
}

const char* PublicType::to_string(PublicType::Enum v_)
{
    switch(v_)
    {
    case PublicType::BOOL:
        return "BOOL";
    case PublicType::CHAR:
        return "CHAR";
    case PublicType::INT:
        return "INT";
    case PublicType::DOUBLE:
        return "DOUBLE";
    case PublicType::STRING:
        return "STRING";
    case PublicType::DATE:
        return "DATE";
    case PublicType::DATETIME:
        return "DATETIME";
    case PublicType::ENUM_AS_STRING:
        return "ENUM_AS_STRING";
    case PublicType::ENUM_AS_INT:
        return "ENUM_AS_INT";
    case PublicType::CLASS:
        return "CLASS";
    case PublicType::OBJECT:
        return "OBJECT";
    case PublicType::MAP:
        return "MAP";
    case PublicType::VARIANT:
        return "VARIANT";
    case PublicType::UNINITIALIZED_VALUE:
        throw std::runtime_error("Uninitialized value for PublicType");
    default:
        throw std::runtime_error("Bad enumerated value");
    }
}

spi::Value verifyPrimitiveTypeName_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& typeName =
        in_context->ValueToString(in_values[0]);

    PublicType o_result = spdoc::verifyPrimitiveTypeName(typeName);
    return o_result;
}

spi::FunctionCaller verifyPrimitiveTypeName_FunctionCaller = {
    "verifyPrimitiveTypeName",
    1,
    {
        {"typeName", spi::ArgType::STRING, "string", false, false, true}
    },
    verifyPrimitiveTypeName_caller
};

spi::ObjectType verifyPrimitiveTypeName_FunctionObjectType =
    spi::FunctionObjectType("spdoc.verifyPrimitiveTypeName");

void publicType_register_object_types(const spi::ServiceSP& svc)
{
    svc->add_enum_info(PublicType::get_enum_info());
    svc->add_object_type(&verifyPrimitiveTypeName_FunctionObjectType);
    svc->add_function_caller(&verifyPrimitiveTypeName_FunctionCaller);
}

SPDOC_END_NAMESPACE

