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

#include "ObjectJSONStreamer.hpp"
#include "RuntimeError.hpp"

#include <spi_util/StringUtil.hpp>
#include <spi_util/JSON.hpp>

#include "Service.hpp"
#include "ObjectMap.hpp"
#include <string.h>
#include <float.h>
#include <math.h>
#include "platform.h"
#include "SHA.hpp"

#define BEGIN_ANONYMOUS_NAMESPACE namespace {
#define END_ANONYMOUS_NAMESPACE }

SPI_BEGIN_NAMESPACE

BEGIN_ANONYMOUS_NAMESPACE

Value ToValue(const spi_util::JSONValue& jv);
MapConstSP ToMap(const spi_util::JSONMapConstSP& jv);



MapConstSP ToMap(const spi_util::JSONValue& value)
{
    if (value.GetType() != spi_util::JSONValue::Map)
        throw RuntimeError("JSON value is not a map");

    return ToMap(value.GetMap());
}

MapConstSP ToMap(const spi_util::JSONMapConstSP& jv)
{
    const std::vector<std::string>& names = jv->Names();

    // map created from JSON must be permissive
    // i.e. we can convert strings to dates, doubles to ints etc
    MapSP m = new Map("Map", 0, true);
    for (size_t i = 0; i < names.size(); ++i)
    {
        const std::string& name = names[i];
        if (name == "class")
        {
            std::string className = jv->Item("class").GetString();
            m->SetClassName(className);
        }
        else
        {
            m->SetValue(name, ToValue(jv->Item(name)));
        }
    }

    return m;
}

Value ToArray(const spi_util::JSONArrayConstSP& jv)
{
    size_t size = jv->Size();
    std::vector<Value> values;

    for (size_t i = 0; i < size; ++i)
    {
        values.push_back(ToValue(jv->Item(i)));
    }

    return Value(IArrayConstSP(new ValueArray(values, values.size())));
}

Value ToValue(const spi_util::JSONValue& jv)
{
    // the problem we have here is that whereas input contexts are permissive
    // values are not very permissive at all
    //
    // thus if you ask a Value whether it is a Date then it really has to be a Date
    //
    // perhaps the way around this is to create an instance of IObjectMap which is
    // permissive when using a Map of Value
    switch(jv.GetType())
    {
    case spi_util::JSONValue::Number:
        return Value(jv.GetNumber());
    case spi_util::JSONValue::String:
        return Value(jv.GetString());
    case spi_util::JSONValue::Array:
        return ToArray(jv.GetArray());
    case spi_util::JSONValue::Map:
        return Value(ToMap(jv.GetMap()));
    case spi_util::JSONValue::Bool:
        return Value(jv.GetBool());
    case spi_util::JSONValue::Null:
        return Value();
    default:
        throw RuntimeError("PROGRAM_BUG");
    }
}

spi_util::JSONValue FromValue(const Value& value);

spi_util::JSONValue FromMap(const MapConstSP& m)
{
    spi_util::JSONMapSP jm(new spi_util::JSONMap);

    const Map* mp = m.get();
    const std::string& className = mp->ClassName();
    if (!className.empty() && className != "Map")
    {
        jm->Insert("class", className);
    }

    const std::vector<std::string>& fieldNames = mp->FieldNames();
    size_t nbFields = fieldNames.size();
    for (size_t i = 0; i < nbFields; ++i)
    {
        const std::string& name  = fieldNames[i];
        const Value&       value = mp->GetValue(name);

        jm->Insert(name, FromValue(value));
    }

    return spi_util::JSONValue(jm);
}

spi_util::JSONValue FromObject(const ObjectConstSP& obj, const MapConstSP& metaData = MapConstSP(),
    bool addObjectId=false)
{
    if (!obj)
        return spi_util::JSONValue(); // indicates null

    // we don't do object caching for JSON
    MapSP aMap(new Map(obj->get_class_name()));

    if (metaData)
        aMap->SetValue("meta_data", Value(metaData));

    if (addObjectId)
        aMap->SetValue("object_id", obj->get_object_id());

    // this is where we use the abstract Object interface
    ObjectMap om(aMap);
    obj->to_map(&om, false);

    return FromMap(aMap);
}

spi_util::JSONValue FromArray(const IArrayConstSP& anArray)
{
    //spi_util::JSONArraySP ja(new spi_util::JSONArray);
    size_t size = anArray->size();
    const std::vector<size_t>& dimensions = anArray->dimensions();
    SPI_POST_CONDITION(dimensions.size() >= 1);

    size_t numDims = dimensions.size();
    size_t size2 = 1;
    for (size_t i = 0; i < numDims; ++i)
    {
        size2 *= dimensions[i];
    }
    SPI_POST_CONDITION(size == size2);
    size_t blockSize = dimensions.back();

    std::vector<size_t> count(numDims, 0);
    std::vector<spi_util::JSONArraySP> jas;

    for (size_t i = 0; i < numDims; ++i)
        jas.push_back(new spi_util::JSONArray);

    for (size_t i = 0; i < size; ++i)
    {
        const Value& value = anArray->getItem(i);
        jas[numDims-1]->Append(FromValue(value));
        count[numDims - 1] += 1;
        for (size_t j = numDims; j > 1; --j)
        {
            if (count[j - 1] < dimensions[j - 1])
                break;

            // we have a full array - need to push it out
            // the loop deals with 3-dimensional arrays which we don't support yet
            spi_util::JSONArraySP ja = jas[j - 1];
            jas[j - 2]->Append(spi_util::JSONValue(ja));
            jas[j - 1] = new spi_util::JSONArray();
            count[j - 1] = 0;
            count[j - 2] += 1;
        }
    }
    return spi_util::JSONValue(jas[0]);
}

spi_util::JSONValue FromValue(const Value& value)
{
    switch (value.getType())
    {
    case Value::UNDEFINED:
        throw RuntimeError("Cannot convert UNDEFINED to JSONValue");
    case Value::CHAR:
    case Value::SHORT_STRING:
    case Value::STRING:
        return spi_util::JSONValue(value.getString());
    case Value::INT:
    case Value::DOUBLE:
        return spi_util::JSONValue(value.getDouble());
    case Value::BOOL:
        return spi_util::JSONValue(value.getBool());
    case Value::DATE:
        return spi_util::JSONValue(std::string(value.getDate()));
    case Value::DATETIME:
        return spi_util::JSONValue(std::string(value.getDateTime()));
    case Value::MAP:
        return FromMap(value.getMap());
    case Value::OBJECT:
        return FromObject(value.getObject());
    case Value::ARRAY:
        return FromArray(value.getArray());
    default:
        SPI_THROW_RUNTIME_ERROR("Cannot convert value of type " <<
            Value::TypeToString(value.getType()) << " to JSONValue");
    }
}

END_ANONYMOUS_NAMESPACE

ObjectConstSP ObjectJSONStreamer::from_data(
    const std::string& streamName,
    const std::string& data,
    size_t offset,
    const MapConstSP& metaData)
{
    spi_util::JSONValue value = spi_util::JSONParseValue(data, offset, streamName);

    return object_from_json(value, m_service, metaData);
}

void ObjectJSONStreamer::to_stream(
    std::ostream& ostr,
    const ObjectConstSP& object,
    const MapConstSP& metaData,
    bool addObjectId)
{
    if (m_noObjectId)
        addObjectId = false;
    else if (m_objectId)
        addObjectId = true;

    spi_util::JSONValue jv = FromObject(object, metaData, addObjectId);

    JSONValueToStream(ostr, jv, m_noNewLine);
}

void ObjectJSONStreamer::to_stream(
    std::ostream& ostr,
    const char* name,
    const Value& value)
{
    spi_util::JSONValue jv = FromValue(value);

    JSONValueToStream(ostr, jv, m_noNewLine);
}

ObjectJSONStreamerSP ObjectJSONStreamer::Make(
    const ServiceConstSP& service,
    const char* options)
{
    return ObjectJSONStreamerSP(new ObjectJSONStreamer(service, options));
}

void ObjectJSONStreamer::Register()
{
    IObjectStreamer::Register("JSON",
        (IObjectStreamer::Maker*)ObjectJSONStreamer::Make,
        false,
        "{");
}

bool ObjectJSONStreamer::uses_recognizer() const
{
    return true;
}

ObjectJSONStreamer::~ObjectJSONStreamer()
{}

ObjectJSONStreamer::ObjectJSONStreamer(
    const ServiceConstSP& service,
    const char* options)
    :
    m_service(service),
    m_noNewLine(false),
    m_objectId(false),
    m_noObjectId(false)
{
    if (options)
    {
        std::vector<std::string> parts = spi_util::CStringSplit(options, ';');
        for (std::vector<std::string>::const_iterator iter = parts.begin();
            iter != parts.end(); ++iter)
        {
            if (iter->empty())
                continue;

            const std::string& part = spi_util::StringUpper(*iter);
            const char* buf = part.c_str();

            switch (*buf)
            {
            case 'N':
                if (strcmp(buf, "NONEWLINE") == 0)
                {
                    m_noNewLine = true;
                    continue;
                }
                if (strcmp(buf, "NO_OBJECT_ID") == 0)
                {
                    m_noObjectId = true;
                    continue;
                }
                break;
            case 'O':
                if (strcmp(buf, "OBJECT_ID") == 0)
                {
                    m_objectId = true;
                    continue;
                }
                break;
            default:
                break;
            }

            throw RuntimeError("%s: Unknown option string '%s'",
                __FUNCTION__, iter->c_str());
        }
    }
}

ObjectConstSP object_from_json(
    const spi_util::JSONValue& jv,
    const ServiceConstSP& svc,
    const MapConstSP& metaData)
{
    MapConstSP m = ToMap(jv);

    ObjectMap om(m);

    return svc->object_from_map(&om, ObjectRefCacheSP(), metaData);
}

SPI_END_NAMESPACE

