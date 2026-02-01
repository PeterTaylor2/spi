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
#include "ValueToObject.hpp"
#include "RuntimeError.hpp"

#include <spi_util/Lexer.hpp>
#include <spi_util/StringUtil.hpp>

#include "Service.hpp"
#include "Value.hpp"
#include "ObjectMap.hpp"

SPI_BEGIN_NAMESPACE

// when reading from a map we need the map and the service
ValueToObject::ValueToObject(
    const ServiceConstSP& svc,
    const ObjectRefCacheSP& objectCache)
    :
    m_service(svc),
    m_objectCache(objectCache)
{}

ValueToObject::~ValueToObject()
{}

ServiceConstSP ValueToObject::Service() const
{
    return m_service;
}

ObjectConstSP ValueToObject::ObjectFromValue(
    const Value& value,
    ObjectType* objectType)
{
    // logic needed here to convert map to object
    // or just take the object
    // or extract the object from an object reference
    switch(value.getType())
    {
    case Value::MAP:
    {
        MapConstSP aMap = value.getMap();

        if (!aMap)
        {
            // NULL map => NULL object
            return ObjectConstSP();
        }

        int ref = aMap->GetRef();

        ObjectMap om(aMap);//, m_service, m_objectCache);

        // it is possible that we have already translated this map
        if (ref != 0 && m_objectCache)
        {
            ObjectConstSP obj = m_objectCache->find_object(ref);
            if (obj)
                return obj;
        }

        ObjectConstSP obj = m_service->object_from_map(&om, m_objectCache);

        if (ref != 0 && m_objectCache)
        {
            m_objectCache->add_object(ref, obj);
        }

        return obj;
    }
    case Value::OBJECT_REF:
    {
        int ref = value.getObjectRef();
        if (!m_objectCache)
        {
            throw RuntimeError("Could not find *%d since there is no cache",
                ref);
        }
        std::map<int, ObjectConstSP>::const_iterator iter;
        ObjectConstSP obj = m_objectCache->find_object(ref);
        if (!obj)
            throw RuntimeError("Could not find *%d in cache", ref);

        return obj;
    }
    case Value::OBJECT:
    {
        ObjectConstSP obj = value.getObject();
        return obj;
    }
    case Value::UNDEFINED:
        // probably should be caught earlier on
        return ObjectConstSP();
    default:
    {
        ObjectConstSP obj = objectType->coerce_from_value(value);
        if (obj)
            return obj;
        // coerce_from_value only throws exceptions if the coercion could
        // be attempted but actually failed - in other cases it returns
        // an empty object which is interpreted below as a failure
        break;
    }
    }
    throw RuntimeError("Cannot convert %s to object", value.toString().c_str());
}

// converts a Value by resolving maps and references to objects
// note that object coercion is not attempted so we do not need objectType
Value ValueToObject::ResolveObject(const Value& value)
{
    switch(value.getType())
    {
    case Value::UNDEFINED:
    case Value::CHAR:
    case Value::SHORT_STRING:
    case Value::STRING:
    case Value::BYTES:
    case Value::INT:
    case Value::DOUBLE:
    case Value::BOOL:
    case Value::DATE:
    case Value::DATETIME:
    case Value::OBJECT:
    case Value::ERROR:
        return value;
    case Value::MAP:
    case Value::OBJECT_REF:
        return Value(ObjectFromValue(value, NULL));
    case Value::ARRAY:
        {
            IArrayConstSP items = value.getArray();
            std::vector<Value> resolvedItems;
            size_t count = items->size();
            resolvedItems.reserve(count);
            for (size_t i = 0; i < count; ++i)
            {
                resolvedItems.push_back(ResolveObject(items->getItem(i)));
            }
            return Value(resolvedItems, items->dimensions());
        }
        break;
    }

    throw RuntimeError("Unknown value type");
}


SPI_END_NAMESPACE
