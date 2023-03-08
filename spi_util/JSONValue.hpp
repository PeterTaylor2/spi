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
#ifndef SPI_UTIL_JSON_VALUE_HPP
#define SPI_UTIL_JSON_VALUE_HPP

/*
***************************************************************************
** JSONValue.hpp
***************************************************************************
** Defines various values that will be understood by JSON.
***************************************************************************
*/

#include "DeclSpec.h"
#include "Namespace.hpp"
#include "RefCounter.hpp"

#include <string>
#include <map>
#include <vector>

SPI_UTIL_NAMESPACE

class JSONValue;
SPI_UTIL_DECLARE_RC_CLASS(JSONString);
SPI_UTIL_DECLARE_RC_CLASS(JSONArray);
SPI_UTIL_DECLARE_RC_CLASS(JSONMap);

class SPI_UTIL_IMPORT JSONString : public RefCounter
{
public:
    friend class JSONValue;

    JSONString(const std::string& str);
    JSONString(const char* str);
    const std::string& String() const;

private:
    std::string m_string;
};


class SPI_UTIL_IMPORT JSONArray : public RefCounter
{
public:
    friend class JSONValue;

    JSONArray();
    void Append(const JSONValue& value);
    size_t Size() const;
    const JSONValue& Item(size_t i) const;

private:
    std::vector<JSONValue> m_array;

};

class SPI_UTIL_IMPORT JSONMap : public RefCounter
{
public:
    friend class JSONValue;

    JSONMap();

    const std::vector<std::string>& Names() const;
    JSONValue Item(const std::string& key, bool optional=false) const;
    void Insert(const std::string& key, const JSONValue& value);

private:

    std::map<std::string, JSONValue> m_map;
    std::vector<std::string> m_names;

};

class SPI_UTIL_IMPORT JSONValue
{
public:
    enum Type
    {
        Number,
        String,
        Array,
        Map,
        Bool,
        Null
    };

    JSONValue();
    JSONValue(double);
    JSONValue(const std::string&);
    JSONValue(const JSONArraySP&);
    JSONValue(const JSONArrayConstSP&);
    JSONValue(const JSONMapSP&);
    JSONValue(const JSONMapConstSP&);
    JSONValue(bool);

    Type GetType() const;

    double           GetNumber(bool optional=false, double defaultValue=0.0) const;
    std::string      GetString(bool optional = false, const char* defaultValue = "") const;
    JSONArrayConstSP GetArray(bool optional=false) const;
    JSONMapConstSP   GetMap(bool optional=false) const;
    bool             GetBool(bool optional=false, bool defaultValue=false) const;

    ~JSONValue();
    JSONValue(const JSONValue&);
    JSONValue& operator=(const JSONValue&);
    JSONValue& Swap(JSONValue&);

private:

    Type type;
    union
    {
        double            aNumber;
        const JSONString* aString;
        const JSONArray*  anArray;
        const JSONMap*    aMap;
        bool              aBool;
    };

};

SPI_UTIL_END_NAMESPACE

#endif
