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
/*
***************************************************************************
** Map.cpp
***************************************************************************
** The Map class. This is a map of name/value pairs, with the order
** that the objects were inserted into the map preserved.
**
** The map also has a className (can be empty).
***************************************************************************
*/

#include "Map.hpp"

#include "RuntimeError.hpp"
#include "StringUtil.hpp"
#include "ValueToObject.hpp"

#undef SPI_UTIL_CLOCK_EVENTS
#include <spi_util/ClockUtil.hpp>

#include <algorithm>

SPI_BEGIN_NAMESPACE

bool Map::g_caseInsensitive(false);

Map::Map(const char* className, int ref, bool permissive)
    :
    m_dict(),
    m_fieldNames(),
    m_className(className),
    m_ref(ref),
    m_permissive(permissive),
    m_type(Map::NAMED)
{}

Map::~Map()
{}

Map::Map(Type type)
    :
    m_dict(),
    m_fieldNames(),
    m_className(),
    m_ref(0),
    m_permissive(false),
    m_type(type)
{}

const std::string& Map::ClassName() const
{
    return m_className;
}

const std::vector<std::string>& Map::FieldNames() const
{
    return m_fieldNames;
}

namespace
{
    std::string ValueTypeName(const Value& value)
    {
        switch(value.getType())
        {
        case Value::CHAR:
        case Value::SHORT_STRING:
        case Value::STRING:
            return "STRING";
        case Value::INT:
            return "INT";
        case Value::DOUBLE:
            return "DOUBLE";
        case Value::BOOL:
            return "BOOL";
        case Value::DATE:
            return "DATE";
        case Value::DATETIME:
            return "DATETIME";
        case Value::MAP:
        case Value::OBJECT:
            return "OBJECT";
        case Value::ARRAY:
            {
                IArrayConstSP values = value.getArray();
                if (values->size() > 0)
                {
                    std::string itemType = ValueTypeName(values->getItem(0));
                    return itemType + "[]"; // assume homogeneous
                }
            }
            return ""; // empty array is undefined type
        case Value::UNDEFINED:
        case Value::ERROR:
        default:
            return "";
        }
    }
} // end of anonymous namespace

std::vector<std::string> Map::FieldTypesAndNames() const
{
    std::vector<std::string> output;
    output.reserve(m_fieldNames.size());

    for (std::vector<std::string>::const_iterator iter = m_fieldNames.begin();
         iter != m_fieldNames.end(); ++iter)
    {
        const std::string& name = *iter;
        try
        {
            Value value = GetValue(name);
            std::string typeName = ValueTypeName(value);
            if (typeName.length() == 0)
                output.push_back(name);
            else
                output.push_back(typeName + " " + name);
        }
        catch (std::exception&)
        {
            output.push_back(name);
        }
    }
    return output;
}

Value Map::GetValue(const std::string& name) const
{
    SPI_UTIL_CLOCK_FUNCTION();

    DictType::const_iterator iter = m_dict.find(name);
    if (iter != m_dict.end())
        return iter->second;

    if (g_caseInsensitive)
    {
        for (iter = m_dict.begin(); iter != m_dict.end(); ++iter)
        {
            if (strcasecmp(name.c_str(), iter->first.c_str()) == 0)
                return iter->second;
        }
    }
    return Value();
}

void Map::SetClassName(const std::string& className)
{
    m_className = className;
}

void Map::SetValue(const std::string& name, const Value& value, bool excludeFromNames)
{
    SPI_UTIL_CLOCK_FUNCTION();

    DictType::iterator iter = m_dict.find(name);
    if (iter == m_dict.end())
    {
        if (!value.isUndefined())
        {
            if (!excludeFromNames)
                m_fieldNames.push_back(name);
            m_dict.insert(std::pair<std::string, Value>(name, value));
        }
    }
    else
    {
        if (value.isUndefined())
        {
            m_dict.erase(iter);
            // use erase/remove algorithm to remove name
            m_fieldNames.erase(
                std::remove(m_fieldNames.begin(), m_fieldNames.end(), name),
                m_fieldNames.end());
        }
        else
        {
            iter->second = value;
        }
    }
}

int Map::GetRef() const
{
    return m_ref;
}

bool Map::Exists(const std::string& name) const
{
    DictType::const_iterator iter = m_dict.find(name);
    bool exists = (iter != m_dict.end());
    return exists;
}

/**
 * Creates a one-level deep copy of the map.
 *
 * This means that the data values are shallow copied. Value class has
 * copy semantics, but when the Value is a complex class this usually
 * involves increasing a reference count.
 *
 * Currently there are no plans for DeepCopy.
 */
MapSP Map::Copy() const
{
    MapSP copy (new Map(m_className.c_str(), m_ref, m_permissive));

    for (size_t i = 0; i < m_fieldNames.size(); ++i)
    {
        const std::string& name = m_fieldNames[i];
        const Value& value      = GetValue(name);

        copy->SetValue(name, value);
    }
    return copy;
}

/**
 * Create a copy of the map resolving all objects from maps and references.
 */
MapSP Map::Copy(ValueToObject& valueToObject) const
{
    MapSP copy (new Map(m_className.c_str(), m_ref, m_permissive));

    for (size_t i = 0; i < m_fieldNames.size(); ++i)
    {
        const std::string& name = m_fieldNames[i];
        const Value& value      = GetValue(name);

        copy->SetValue(name, valueToObject.ResolveObject(value));
    }
    return copy;
}


bool Map::Permissive() const
{
    return m_permissive;
}

Map::Type Map::MapType() const
{
    return m_type;
}

/**
 * Sets case insensitivity flag for all maps.
 * The normal behaviour is that maps are case sensitive.
 * Case insensitivity is a performance overhead.
 */
bool Map::SetCaseInsensitive(bool caseInsensitive)
{
    std::swap(caseInsensitive, g_caseInsensitive);
    return caseInsensitive;
}

size_t Map::NumFields() const
{
    return m_fieldNames.size();
}

SPI_END_NAMESPACE


