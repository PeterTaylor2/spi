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
#include "Object.hpp"

#include "RuntimeError.hpp"
#include "Map.hpp"
#include "ObjectHelper.hpp"
#include "ObjectMap.hpp"
#include "IObjectStreamer.hpp"
#include "InputValues.hpp"
#include "Function.hpp"

#include "MapObject.hpp"

#include <string.h>

#ifdef TRACE_WRAPPER_CACHE
#include <iostream>
#endif


#undef SPI_UTIL_CLOCK_EVENTS
#include <spi_util/ClockUtil.hpp>

#define CREATE_OBJECT_ID_ON_REQUEST
#include <spi_util/UUID.hpp>

SPI_BEGIN_NAMESPACE

bool Object::g_add_object_id_file = false;
bool Object::g_add_object_id_string = false;

ObjectType::ObjectType(
    const char*       className,
    MakeFromMap*      makeFromMap,
    IsInstance*       isInstance,
    bool              canPut,
    CoerceFromObject* coerceFromObject,
    CoerceFromValue*  coerceFromValue)
    :
    m_className(className),
    m_makeFromMap(makeFromMap),
    m_isInstance(isInstance),
    m_canPut(canPut),
    m_coerceFromObject(coerceFromObject),
    m_coerceFromValue(coerceFromValue),
    m_service(0)
{
    SPI_PRE_CONDITION(className != NULL);
    SPI_PRE_CONDITION(isInstance != NULL);
}

const char* ObjectType::get_class_name() const
{
    return m_className.c_str();
}

ObjectConstSP ObjectType::make_from_map(
    IObjectMap* objectMap,
    ValueToObject& mapToObject,
    const MapConstSP& metaData) const
{
    SPI_UTIL_CLOCK_FUNCTION();

    std::string className = objectMap->ClassName();

    if (className != m_className)
        throw RuntimeError("Mismatch in map name (%s) and object (%s)",
                           className.c_str(), m_className.c_str());

    ObjectConstSP o = m_makeFromMap(objectMap, mapToObject);
    MapObjectConstSP omd = objectMap->GetInstance<MapObject>("meta_data", mapToObject, true);
    std::string object_id = objectMap->GetString("object_id", true);

    if (!object_id.empty())
        o->set_object_id(object_id);

    if (metaData || omd)
    {
        // this is really part of the construction
        //
        // getting the non-const metaData from the object and changing it outside
        // is a sneaky way of getting around the const state of the object
        //
        // perhaps m_makeFromMap should return non-const object
        MapSP objMetaData = o->get_meta_data();

        if (omd)
        {
            MapConstSP m = omd->get_inner();
            const std::vector<std::string> fieldNames = m->FieldNames();
            for (std::vector<std::string>::const_iterator iter = fieldNames.begin();
                iter != fieldNames.end(); ++iter)
            {
                objMetaData->SetValue(*iter, m->GetValue(*iter));
            }
        }

        if (metaData)
        {
            SPI_PRE_CONDITION(metaData->ClassName().empty());
            const std::vector<std::string> fieldNames = metaData->FieldNames();
            for (std::vector<std::string>::const_iterator iter = fieldNames.begin();
                iter != fieldNames.end(); ++iter)
            {
                objMetaData->SetValue(*iter, metaData->GetValue(*iter));
            }
        }
        objMetaData->SetClassName("Map");
    }

    SPI_UTIL_CLOCK_EVENTS_LOG(m_className);

    return o;
}

bool ObjectType::is_instance(const ObjectConstSP& o) const
{
    return m_isInstance(o);
}

bool ObjectType::can_put() const
{
    return m_canPut;
}

ObjectConstSP ObjectType::coerce_from_value(
    const Value& value,
    const InputContext* context) const
{
    // this function is intended to be used when the value is not an object
    // subsequently we use the static method Coerce (via the static method
    // coerce_from_object) to complete the transition to the correct object type
    if (m_coerceFromValue && !value.isUndefined())
    {
        static NoInputContext noInputContext;
        if (!context)
            context = &noInputContext;

        ObjectConstSP obj = m_coerceFromValue(value, context);
        if (obj)
            return obj;
    }

    return ObjectConstSP();
}

ObjectConstSP ObjectType::coerce_from_object(const ObjectConstSP& o) const
{
    // the generated coerce_from_object for each class is essentially
    // the same as below except that in addition it supports coercion
    // from other classes
    if (m_coerceFromObject)
    {
        ObjectConstSP p = m_coerceFromObject(o);
        return p;
    }

    if (!o)
        return ObjectConstSP();

    if (is_instance(o))
        return o;

    // we need to do the service coercion before coerceTo
    //
    // suppose we are trying to see if we have an instance of some base class
    // there won't be any coerceTo to the base class and we won't have a clue
    // whether we can coerceTo some intermediate class in the class hierarchy

    ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (get_service() != o->get_service())
    {
        o2 = o->service_coercion(get_service());
        if (!o2)
            o2 = o;
        else if (is_instance(o2))
            return o2;
    }
    else
    {
        o2 = o;
    }

    // coerceTo if the input object has a different name
    if (strcmp(get_class_name(), o2->get_class_name()) != 0)
    {
        ObjectConstSP o3 = o2->coerce_to_object(get_class_name());
        if (o3 && is_instance(o3))
            return o3;
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name() << " is neither "
        << get_class_name() << " nor a sub-class of " << get_class_name());
}

void ObjectType::set_service(const Service* svc)
{
    m_service = svc;
}

const Service* ObjectType::get_service() const
{
    return m_service;
}

ObjectType::Registry::Registry()
    :
    m_dict()
{}

void ObjectType::Registry::import_types(const Registry& other)
{
    std::map<std::string,ObjectType*>::const_iterator iter;
    for (iter = other.m_dict.begin(); iter != other.m_dict.end(); ++iter)
    {
        add_type(iter->second);
    }
}

void ObjectType::Registry::add_type(ObjectType* type, bool overwrite)
{
    std::string className = type->get_class_name();
    std::map<std::string,ObjectType*>::iterator iter = m_dict.find(className);

    if (iter != m_dict.end())
    {
        if (!overwrite)
            return; // first seen wins
    }

    m_dict[className] = type;
}

ObjectType* ObjectType::Registry::get_type(const char* className) const
{
    return get_type(std::string(className));
}

ObjectType* ObjectType::Registry::get_type(
    const std::string& className) const
{
    std::map<std::string,ObjectType*>::const_iterator iter = m_dict.find(className);

    if (iter != m_dict.end())
        return iter->second;

    if (className == "Map")
        return &MapObject::object_type;

    throw RuntimeError("%s: className %s is not defined",
                       __FUNCTION__, className.c_str());
}

size_t ObjectType::Registry::size() const
{
    return m_dict.size();
}

void ObjectType::Registry::clear()
{
    m_dict.clear();
}

const char* Object::get_class_name() const
{
    return get_object_type()->get_class_name();
}

MapConstSP Object::get_attributes() const
{
    MapSP aMap(new Map(get_class_name()));

    ObjectMap objectMap(aMap);
    to_map(&objectMap, false);

    return aMap;
}

const MapConstSP& Object::get_public_map() const
{
    if (!m_public_map)
    {
        // create the public map - which is why m_public_map is mutable
        MapSP aMap(new Map(get_class_name()));

        if (!m_object_id.empty())
            aMap->SetValue("object_id", m_object_id, true);

        ObjectMap objectMap(aMap);
        to_map(&objectMap, true);

        if (m_meta_data && m_meta_data->NumFields() > 0)
        {
            // skip objectMap.setObject and go straight to aMap->setValue
            // this is because we don't want meta_data appearing in the list of names
            // SetValue has a parameter (excludeFromNames) that can only be used
            // for a direct calls instead of via ObjectMap::SetObject
            aMap->SetValue("meta_data", MapObject::Make(m_meta_data)->to_value(), true);
        }

        m_public_map = aMap;
    }
    return m_public_map;
}

Value Object::get_value(const std::string & name) const
{
    const MapConstSP& m = get_public_map();
    if (name.empty())
    {
        return m->FieldNames();
    }
    return m->GetValue(name);
}

Value Object::as_value() const
{
    return Value();
}

const std::string& Object::get_object_id() const
{
#ifdef CREATE_OBJECT_ID_ON_REQUEST
    if (m_object_id.empty())
    {
        std::ostringstream oss;
        DateTime now = DateTime::Now(true);
        oss << spi_util::uuid_string() << " " << now.ToString();
        m_object_id = oss.str();
    }
#endif
    return m_object_id;
}

Value Object::to_value(const ObjectConstSP& o)
{
    if (!o)
        return Value(ObjectConstSP());
    return o->to_value();
}

Value Object::to_value() const
{
    Value value = as_value();
    if (value.isUndefined())
        value = Value(ObjectConstSP(this));
    return value;
}

Value Object::to_value(const std::vector<ObjectConstSP>& objects)
{
    std::vector<Value> values;
    for (size_t i = 0; i < objects.size(); ++i)
        values.push_back(Object::to_value(objects[i]));
    return Value(values);
}

void Object::clear_public_map() const
{
    // m_public_map is mutable - clear_public_map should only be called from a non-const method
    // however we declare all object types as const shared pointers - so this method itself is const
    m_public_map.reset();
}

ObjectConstSP Object::coerce_to_object(const std::string& name) const
{
    return ObjectConstSP();
}

bool Object::to_stream(
    std::ostream& ostr,
    const char* format,
    const char* options,
    const MapConstSP& metaData,
    bool addObjectId) const
{
    std::string key = format ? std::string(format) : std::string();
    IObjectStreamerSP streamer = IObjectStreamer::MakeWriter(key, options);
    bool isBinary = streamer->is_binary();

    if (!streamer->uses_recognizer())
    {
        ostr << IObjectStreamer::Recognizer(key);
        if (!isBinary)
            ostr << '\n';
    }
    streamer->to_stream(ostr, this, metaData, addObjectId);

    return isBinary;
}

std::string Object::to_string(
    const char* format,
    const char* options,
    const MapConstSP& metaData) const
{
    std::ostringstream oss;
    bool isBinary = to_stream(oss, format, options, metaData, g_add_object_id_string);

    if (isBinary)
        throw RuntimeError("Object::to_string: Format requested is binary");

    return oss.str();
}

void Object::to_file(
    const char* filename,
    const char* format,
    const char* options,
    const MapConstSP& metaData) const
{
    // we stream in memory and then write the file afterwards in one hit
    //
    // hence if we have a problem with the streaming we don't write an
    // incomplete file
    //
    // also on slow file systems it is best to keep the file open for
    // as short a time as possible
    std::stringstream oss;
    bool isBinary = to_stream(oss, format, options, metaData, g_add_object_id_file);

    std::ios_base::openmode mode = std::ios_base::out;
    if (isBinary)
        mode |= std::ios_base::binary;

    std::ofstream ofs(filename, mode);
    if (!ofs)
        throw RuntimeError("Object::to_file: Could not open output file %s",
            filename);

    ofs << oss.rdbuf();

    if (!isBinary)
        ofs << std::endl;

    ofs.close();
}

/**
 * Given an object created by a different service can we translate it via its
 * map to an object of the required service.
 */
ObjectConstSP Object::service_coercion(const Service* svc) const
{
    SPI_PRE_CONDITION(svc);

    if (m_serviceCoercion.count(svc))
        return m_serviceCoercion[svc];

    ObjectType* ot;
    try
    {
        ot = svc->get_object_type(get_class_name());
    }
    catch (std::exception&)
    {
        return ObjectConstSP();
    }

    // actually the object types have the same reference (usually ObjectType is
    // pointer to static)
    if (ot == get_object_type())
        return ObjectConstSP(this);

    try
    {
        // create an objectMap from this object
        ObjectMapSP objectMap(new ObjectMap(get_attributes()));

        // convert the objectMap to another object using the service
        // associated with the given object type rather than the service
        // associated with this object
        //
        // note that objectTypes ultimately sit inside services, so we
        // cannot put a shared pointer to the service inside the objectTypes
        // without creating a cycle
        //
        // however we can get the service associated with the objectType
        // and make a temporary shared pointer (since we are using intrusive
        // pointers) for the purpose of creating the valueToObject which is
        // needed for translating nested types
        ValueToObject valueToObject(ot->get_service(), new ObjectRefCache());
        ObjectConstSP obj = ot->make_from_map(objectMap.get(), valueToObject, MapSP());
        // since this is time-consuming we keep track of the fact that
        // we have performed this coercion so that next time around it
        // will be quicker
        m_serviceCoercion[svc] = obj;
        return obj;
    }
    catch (std::exception&)
    {
        SPI_THROW_RUNTIME_ERROR("Could not convert object of type '"
            << get_class_name()
            << "' from service (" << get_service()->get_name() << ")"
            << " to service (" << svc->get_name() << ")");
    }
}

const Service* Object::get_service() const
{
    return get_object_type()->get_service();
}

void* Object::get_py_object() const
{
    return NULL;
}

const MapSP& Object::get_meta_data() const
{
    return m_meta_data;
}

int Object::get_id() const
{
    return m_id;
}

void Object::set_tracking(bool tracking)
{
    if (!tracking)
        g_tracked.clear();

    g_tracking = tracking;
}

size_t Object::tracked_object_count()
{
    return g_tracked.size();
}

std::map<std::string, size_t> Object::tracked_object_by_class_name()
{
    std::map<std::string, size_t> out;
    if (g_tracking)
    {
        for (std::set<const Object*>::const_iterator iter = g_tracked.begin();
            iter != g_tracked.end(); ++iter)
        {
            const std::string& class_name = (*iter)->get_class_name();
            std::map<std::string,size_t>::iterator iter2 = out.find(class_name);
            if (iter2 == out.end())
                out.insert(std::pair<std::string,size_t>(class_name,1));
            else
                iter2->second += 1;
        }
    }
    return out;
}

Object::Object(bool no_id)
{
    if (no_id)
        m_id = 0;
    else
        m_id = ++g_count;

    m_meta_data.reset(new Map(""));

    if (g_tracking)
    {
        g_tracked.insert(this);
    }
}

Object::~Object()
{
    if (g_tracking)
    {
        g_tracked.erase(this);
    }
}

void Object::set_constructor(const FunctionConstSP& constructor) const
{
    if (constructor)
        m_constructor = constructor;
    else
        m_constructor = 0;
}

bool Object::has_constructor() const
{
    return !!m_constructor;
}

void Object::constructor_to_map(IObjectMap* objectMap) const
{
    SPI_PRE_CONDITION(has_constructor());
    objectMap->SetObject("_constructor", m_constructor);
}

ObjectConstSP Object::constructor_from_map(
    IObjectMap*    objectMap,
    ValueToObject& valueToObject)
{
    // optionally get a generic object from the map
    // if _constructor is missing then we get null and return
    // if _constructor exists then it must be a functor
    ObjectConstSP obj = objectMap->GetObject(
        "_constructor", 0, valueToObject, true);

    if (!obj)
        return ObjectConstSP();

    const Function* func = dynamic_cast<const Function*>(obj.get());
    if (!func)
        throw spi::RuntimeError("%s is not a Function",
            obj->get_class_name());

    Value value = func->call();

    // value must be an object of some type

    ObjectConstSP object = value.getObject();
    return object;
}

void Object::set_object_id(const std::string& object_id) const
{
#ifndef CREATE_OBJECT_ID_ON_REQUEST
    if (object_id.empty())
    {
        std::ostringstream oss;
        DateTime now = DateTime::Now(true);
        oss << spi_util::uuid_string() << " " << now.ToString();
        m_object_id = oss.str();
    }
    else
    {
        m_object_id = object_id;
    }
#else
    m_object_id = object_id;
#endif
}

int Object::g_count = 0;
bool Object::g_tracking = false;
std::set<const Object*> Object::g_tracked;

Value ObjectGet(const ObjectConstSP& obj, const char* name)
{
    return ObjectGet(obj.get(), name);
}

Value ObjectGet(const Object* obj, const char* name)
{
    const char* classname = obj->get_class_name();

    // since attributes of classes need to be c++ variable names
    // we can safely use typename as a request for the classname
    // since typename is a keyword in c++ and cannot be a variable name
    if (strcmp(name, "typename") == 0)
        return Value(classname);

    Value value = obj->get_value(name);
    if (value.isUndefined())
    {
        throw RuntimeError("ObjectGet: '%s' is undefined in object of type %s",
            name, classname);
    }
    return value;
}

ObjectWrapperCache::ObjectWrapperCache(const char* name)
    :
    m_name(!name ? "(NULL)" : name),
    m_count(0),
    m_maxSize(0),
    m_found(0)
{}

ObjectWrapperCache::~ObjectWrapperCache()
{
#ifdef TRACE_WRAPPER_CACHE
    std::cout << "deleting ObjectWrapperCache " << m_name << ": " << m_map.size()
        << " : " << m_count << " : " << m_maxSize << " : " << m_found << std::endl;
#endif
}

Object* ObjectWrapperCache::Find(const void* inner) const
{
    std::map<const void*, const Object*>::const_iterator iter = m_map.find(inner);

    if (iter == m_map.end())
        return 0;

#ifdef TRACE_WRAPPER_CACHE
    ++m_found;
#endif

    return (Object*)iter->second;
}

void ObjectWrapperCache::Insert(const void* inner, const Object* outer)
{
    m_map.insert(std::pair<const void*,const Object*>(inner,outer));
#ifdef TRACE_WRAPPER_CACHE
    ++m_count;
    if (m_map.size() > m_maxSize)
        m_maxSize = m_map.size();
#endif
}

void ObjectWrapperCache::Delete(const void* inner, const Object* outer)
{
    std::map<const void*, const Object*>::iterator iter = m_map.find(inner);
    if (iter != m_map.end())
    {
        SPI_POST_CONDITION(iter->second == outer);
        m_map.erase(iter);
    }
}

size_t ObjectWrapperCache::Size() const
{
    return m_map.size();
}

SPI_END_NAMESPACE

