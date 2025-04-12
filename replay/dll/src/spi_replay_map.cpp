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
* Source file: spi_replay_map.cpp
****************************************************************************
*/

#include "spi_replay_map.hpp"
#include "spi_replay_map_helper.hpp"
#include "replay_dll_service_manager.hpp"

#include <spi/Replay.hpp>

SPI_REPLAY_BEGIN_NAMESPACE

ValueType::ValueType(const spi::Value &v)
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

SPI_NAMESPACE::Value::Type ValueType_convert_in(const ValueType& v_)
{
    switch((ValueType::Enum)v_)
    {
    case ValueType::UNDEFINED:
        return SPI_NAMESPACE::Value::UNDEFINED;
    case ValueType::CHAR:
        return SPI_NAMESPACE::Value::CHAR;
    case ValueType::SHORT_STRING:
        return SPI_NAMESPACE::Value::SHORT_STRING;
    case ValueType::STRING:
        return SPI_NAMESPACE::Value::STRING;
    case ValueType::INT:
        return SPI_NAMESPACE::Value::INT;
    case ValueType::DOUBLE:
        return SPI_NAMESPACE::Value::DOUBLE;
    case ValueType::BOOL:
        return SPI_NAMESPACE::Value::BOOL;
    case ValueType::DATE:
        return SPI_NAMESPACE::Value::DATE;
    case ValueType::DATETIME:
        return SPI_NAMESPACE::Value::DATETIME;
    case ValueType::MAP:
        return SPI_NAMESPACE::Value::MAP;
    case ValueType::OBJECT:
        return SPI_NAMESPACE::Value::OBJECT;
    case ValueType::OBJECT_REF:
        return SPI_NAMESPACE::Value::OBJECT_REF;
    case ValueType::ARRAY:
        return SPI_NAMESPACE::Value::ARRAY;
    case ValueType::ERROR:
        return SPI_NAMESPACE::Value::ERROR;
    case ValueType::UNINITIALIZED_VALUE:
        throw std::runtime_error("Uninitialized value for ValueType");
    }
    throw spi::RuntimeError("Bad enumerated value");
}

ValueType ValueType_convert_out(SPI_NAMESPACE::Value::Type v_)
{
    if (v_ == SPI_NAMESPACE::Value::UNDEFINED)
        return ValueType::UNDEFINED;
    if (v_ == SPI_NAMESPACE::Value::CHAR)
        return ValueType::CHAR;
    if (v_ == SPI_NAMESPACE::Value::SHORT_STRING)
        return ValueType::SHORT_STRING;
    if (v_ == SPI_NAMESPACE::Value::STRING)
        return ValueType::STRING;
    if (v_ == SPI_NAMESPACE::Value::INT)
        return ValueType::INT;
    if (v_ == SPI_NAMESPACE::Value::DOUBLE)
        return ValueType::DOUBLE;
    if (v_ == SPI_NAMESPACE::Value::BOOL)
        return ValueType::BOOL;
    if (v_ == SPI_NAMESPACE::Value::DATE)
        return ValueType::DATE;
    if (v_ == SPI_NAMESPACE::Value::DATETIME)
        return ValueType::DATETIME;
    if (v_ == SPI_NAMESPACE::Value::MAP)
        return ValueType::MAP;
    if (v_ == SPI_NAMESPACE::Value::OBJECT)
        return ValueType::OBJECT;
    if (v_ == SPI_NAMESPACE::Value::OBJECT_REF)
        return ValueType::OBJECT_REF;
    if (v_ == SPI_NAMESPACE::Value::ARRAY)
        return ValueType::ARRAY;
    if (v_ == SPI_NAMESPACE::Value::ERROR)
        return ValueType::ERROR;
    throw spi::RuntimeError("Bad enumerated value");
}

std::string MapClassName(
    const spi::MapObjectSP& m)
{
  SPI_PROFILE("spi_replay.MapClassName");
  bool isLogging = replay_begin_function(true);
  try
  {
    spi::MapConstSP i_m;

    i_m = spi::MapObject::get_inner(m);
    const std::string& i_result = MapClassName_Helper(i_m);

    replay_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw replay_catch_exception(isLogging, "MapClassName", e); }
  catch (...)
  { throw replay_catch_all(isLogging, "MapClassName"); }
}

std::string MapClassName_Helper(
    const spi::MapConstSP& m)
{
    return m->ClassName();
}

std::vector<std::string> MapFieldNames(
    const spi::MapObjectSP& m)
{
  SPI_PROFILE("spi_replay.MapFieldNames");
  bool isLogging = replay_begin_function(true);
  try
  {
    spi::MapConstSP i_m;

    i_m = spi::MapObject::get_inner(m);
    const std::vector<std::string>& i_result = MapFieldNames_Helper(i_m);

    replay_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw replay_catch_exception(isLogging, "MapFieldNames", e); }
  catch (...)
  { throw replay_catch_all(isLogging, "MapFieldNames"); }
}

std::vector<std::string> MapFieldNames_Helper(
    const spi::MapConstSP& m)
{
    return m->FieldNames();
}

int MapRef(
    const spi::MapObjectSP& m)
{
  SPI_PROFILE("spi_replay.MapRef");
  bool isLogging = replay_begin_function(true);
  try
  {
    spi::MapConstSP i_m;

    i_m = spi::MapObject::get_inner(m);
    int i_result = MapRef_Helper(i_m);

    replay_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw replay_catch_exception(isLogging, "MapRef", e); }
  catch (...)
  { throw replay_catch_all(isLogging, "MapRef"); }
}

int MapRef_Helper(
    const spi::MapConstSP& m)
{
    return m->GetRef();
}

/*
****************************************************************************
* Returns the value and the value type for a name in a Map.
*
* Sometimes the same primitive type is returned for different value types -
* hence the need to return the value type to distinguish.
****************************************************************************
*/
void MapGetValue(
    const spi::MapObjectSP& m,
    const std::string& name,
    spi::Variant& value,
    ValueType& valueType)
{
  SPI_PROFILE("spi_replay.MapGetValue");
  bool isLogging = replay_begin_function(true);
  try
  {
    spi::MapConstSP i_m;

    i_m = spi::MapObject::get_inner(m);
    SPI_NAMESPACE::Value::Type i_valueType;

    MapGetValue_Helper(i_m, name, value, i_valueType);

    valueType = ValueType_convert_out(i_valueType);

    replay_end_function();
  }
  catch (std::exception& e)
  { throw replay_catch_exception(isLogging, "MapGetValue", e); }
  catch (...)
  { throw replay_catch_all(isLogging, "MapGetValue"); }
}

std::tuple< spi::Variant, ValueType > MapGetValue(
    const spi::MapObjectSP& m,
    const std::string& name)
{
    spi::Variant value;
    ValueType valueType;

    MapGetValue(m, name, value, valueType);

    return std::tie(value, valueType);
}

void MapGetValue_Helper(
    const spi::MapConstSP& m,
    const std::string& name,
    spi::Variant& value,
    SPI_NAMESPACE::Value::Type& valueType)
{
    spi::Value v = m->GetValue(name);

    value = v;
    valueType = v.getType();
}

SPI_REPLAY_END_NAMESPACE

