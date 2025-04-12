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
* Source file: spdoc_publicType.cpp
****************************************************************************
*/

#include "spdoc_publicType.hpp"
#include "spdoc_publicType_helper.hpp"
#include "spdoc_dll_service_manager.hpp"


SPDOC_BEGIN_NAMESPACE

PublicType::PublicType(const spi::Value &v)
{
    switch(v.getType())
    {
    case spi::Value::INT:
        value = (Enum)v.getInt();
        break;
    case spi::Value::SHORT_STRING:
    case spi::Value::STRING:
        value = from_string(v.getString().c_str());
        break;
    case spi::Value::UNDEFINED:
        value = from_string("");
        break;
    default:
        SPI_THROW_RUNTIME_ERROR("Bad value type: " << spi::Value::TypeToString(v.getType()));
    }
}

/*
****************************************************************************
* Given a c++ typename, this function will verify that this is a valid
* primitive type, and return the corresponding PublicType value.
*
* Not all public types are primitive types, but all primitive types have
* a corresponding public type.
****************************************************************************
*/
PublicType verifyPrimitiveTypeName(
    const std::string& typeName)
{
  SPI_PROFILE("spdoc.verifyPrimitiveTypeName");
  bool isLogging = spdoc_begin_function(true);
  try
  {
    PublicType i_result = verifyPrimitiveTypeName_Helper(typeName);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "verifyPrimitiveTypeName", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "verifyPrimitiveTypeName"); }
}

PublicType verifyPrimitiveTypeName_Helper(
    const std::string& typeName)
{
    static std::map<std::string, PublicType::Enum> goodTypes;

    if (goodTypes.empty())
    {
        goodTypes["bool"] = PublicType::BOOL;
        goodTypes["char"] = PublicType::CHAR;
        goodTypes["int"] = PublicType::INT;
        goodTypes["double"] = PublicType::DOUBLE;
        goodTypes["std::string&"] = PublicType::STRING;
        goodTypes["spi::Date"] = PublicType::DATE;
        goodTypes["spi::DateTime"] = PublicType::DATETIME;
        goodTypes["spi::ObjectConstSP&"] = PublicType::OBJECT;
        goodTypes["spi::Variant&"] = PublicType::VARIANT;
        goodTypes["spi::MapObjectSP&"] = PublicType::MAP;
    }

    std::map<std::string, PublicType::Enum>::iterator iter =
        goodTypes.find(typeName);

    if (iter == goodTypes.end())
    {
        throw spi::RuntimeError("%s is not a valid primitive type name",
                                typeName.c_str());
    }
    return iter->second;
}

SPDOC_END_NAMESPACE

