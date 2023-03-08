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

#include "JSONValue.hpp"

#include "RuntimeError.hpp"
#include "StringUtil.hpp"
#include "JSONLexer.hpp"

#include <string.h>

SPI_UTIL_NAMESPACE

/*
***************************************************************************
** Implementation of JSONString
***************************************************************************
*/
JSONString::JSONString(const std::string& str)
    :
    m_string(str)
{}

JSONString::JSONString(const char* str)
    :
    m_string(str ? str : "")
{}

const std::string& JSONString::String() const
{
    return m_string;
}

/*
***************************************************************************
** Implementation of JSONArray
***************************************************************************
*/
JSONArray::JSONArray()
    :
    m_array()
{}

void JSONArray::Append(const JSONValue& value)
{
    m_array.push_back(value);
}

size_t JSONArray::Size() const
{
    return m_array.size();
}

const JSONValue& JSONArray::Item(size_t i) const
{
    if (i > m_array.size())
        throw std::runtime_error("JSONArray: Item out of range");

    return m_array[i];
}

/*
***************************************************************************
** Implementation of JSONMap
***************************************************************************
*/
JSONMap::JSONMap()
{}

const std::vector<std::string>& JSONMap::Names() const
{
    return m_names;
}

JSONValue JSONMap::Item(const std::string& key, bool optional) const
{
    std::map<std::string, JSONValue>::const_iterator iter = m_map.find(key);

    if (iter != m_map.end())
        return iter->second;

    if (!optional)
        SPI_UTIL_THROW_RUNTIME_ERROR("'" << key << "' not in JSONMap");

    return JSONValue();
}

void JSONMap::Insert(const std::string& key, const JSONValue& value)
{
    if (m_map.count(key) == 0)
        m_names.push_back(key);

    m_map[key] = value;
}

/*
***************************************************************************
** Implementation of JSONValue
***************************************************************************
*/
JSONValue::JSONValue()
    :
    type(JSONValue::Null),
    aNumber(0.0)
{}

JSONValue::JSONValue(double number)
    :
    type(JSONValue::Number),
    aNumber(number)
{}

JSONValue::JSONValue(const std::string& str)
    :
    type(JSONValue::String),
    aNumber(0.0)
{
    aString = new JSONString(str);
    incRefCount(aString);
}

JSONValue::JSONValue(const JSONArraySP& in)
    :
    type(JSONValue::Array),
    aNumber(0.0)
{
    anArray = in.get();
    incRefCount(anArray);
}

JSONValue::JSONValue(const JSONArrayConstSP& in)
    :
    type(JSONValue::Array),
    aNumber(0.0)
{
    anArray = in.get();
    incRefCount(anArray);
}

JSONValue::JSONValue(const JSONMapSP& in)
    :
    type(JSONValue::Map),
    aNumber(0.0)
{
    aMap = in.get();
    incRefCount(aMap);
}

JSONValue::JSONValue(const JSONMapConstSP& in)
    :
    type(JSONValue::Map),
    aNumber(0.0)
{
    aMap = in.get();
    incRefCount(aMap);
}

JSONValue::JSONValue(bool in)
    :
    type(JSONValue::Bool),
    aBool(in)
{}

JSONValue::Type JSONValue::GetType() const
{
    return type;
}

double JSONValue::GetNumber(bool optional, double defaultValue) const
{
    if (type == Number)
        return aNumber;

    if (optional && type == Null)
        return defaultValue;

    throw std::runtime_error("JSONValue is not a number");
}

std::string JSONValue::GetString(bool optional, const char* defaultValue) const
{
    if (type == String)
        return aString->String();

    if (optional && type == Null)
    {
        if (defaultValue)
            return std::string(defaultValue);
        return std::string();
    }

    throw std::runtime_error("JSONValue is not a string");
}

JSONArrayConstSP JSONValue::GetArray(bool optional) const
{
    if (type == Array)
        return JSONArrayConstSP(anArray);

    if (optional && type == Null)
        return JSONArrayConstSP(new JSONArray());

    throw std::runtime_error("JSONValue is not an array");
}

JSONMapConstSP JSONValue::GetMap(bool optional) const
{
    if (type == Map)
        return JSONMapConstSP(aMap);

    if (optional && type == Null)
        return JSONMapConstSP(new JSONMap());

    throw std::runtime_error("JSONValue is not a map");
}

bool JSONValue::GetBool(bool optional, bool defaultValue) const
{
    if (type == Bool)
        return aBool;

    if (optional && type == Null)
        return defaultValue;

    throw std::runtime_error("JSONValue is not a bool");
}

JSONValue::~JSONValue()
{
    switch(type)
    {
    case String:
        decRefCount(aString);
        break;
    case Array:
        decRefCount(anArray);
        break;
    case Map:
        decRefCount(aMap);
        break;
    case Number:
    case Bool:
    case Null:
        break;
    }

    type = Null;
}

JSONValue::JSONValue(const JSONValue& in)
    :
    type(in.type),
    aNumber(0.0)
{
    switch(type)
    {
    case String:
        aString = in.aString;
        incRefCount(aString);
        break;
    case Array:
        anArray = in.anArray;
        incRefCount(anArray);
        break;
    case Map:
        aMap = in.aMap;
        incRefCount(aMap);
        break;
    case Number:
        aNumber = in.aNumber;
        break;
    case Bool:
        aBool = in.aBool;
        break;
    case Null:
        break;
    }
}

JSONValue& JSONValue::operator=(const JSONValue& rhs)
{
    JSONValue tmp(rhs);

    Swap(tmp); // bitwise copy

    return *this;
}

JSONValue& JSONValue::Swap(JSONValue &value)
{
    // Simple bitwise swap
    char tmp[sizeof(JSONValue)];
    memcpy(tmp, this, sizeof(JSONValue));
    memcpy((void*)this, &value, sizeof(JSONValue));
    memcpy((void*)&value, tmp, sizeof(JSONValue));
    return *this;
}


SPI_UTIL_END_NAMESPACE

