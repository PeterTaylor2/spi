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
#ifndef SPI_MAP_OBJECT_HPP
#define SPI_MAP_OBJECT_HPP

/*
***************************************************************************
** MapObject.hpp
***************************************************************************
** Defines an object implemently entirely as a map of data.
** This can be sub-classed for specific object classes.
***************************************************************************
*/

#include "Map.hpp"
#include "Object.hpp"
#include "MatrixData.hpp"
#include "ObjectSmartPtr.hpp"

#include <map>
#include <string>
#include <vector>


#define SPI_DECLARE_MAP_OBJECT_TYPE(T)\
    typedef spi_boost::intrusive_ptr<T> outer_type;\
    spi::ObjectType* get_object_type() const; \
    static spi::ObjectConstSP object_from_map(spi::IObjectMap* objMap, spi::ValueToObject& valueToObject); \
    static T::outer_type Coerce(const spi::ObjectConstSP& o);       \
    static T::outer_type from_data(const std::string& data, const std::string& streamName, bool isBinary=false); \
    static T::outer_type from_string(const std::string& str);\
    static T::outer_type from_file(const char* filename);\
    static spi::ObjectType object_type


#define SPI_IMPLEMENT_MAP_OBJECT_TYPE(T,objectName,serviceFunc,canPut)\
spi::ObjectType* T::get_object_type() const      \
{\
    return &T::object_type;\
}\
spi::ObjectConstSP T::object_from_map(spi::IObjectMap* objMap, spi::ValueToObject& valueToObject)\
{\
 if (!objMap)\
   return T::outer_type();\
 return T::Make(objMap->ExportMap(), valueToObject);\
}\
T::outer_type T::Coerce(const spi::ObjectConstSP& o)    \
{\
    if (!o) return T::outer_type();\
    T* p = dynamic_cast<T*>((spi::Object*)o.get());\
    if (p) return T::outer_type(p);\
    throw spi::RuntimeError("%s is not instance of %s\n", o->get_class_name(),\
        object_type.get_class_name());\
}\
T::outer_type T::from_data(const std::string& data, const std::string& streamName, bool isBinary)  \
{\
    spi::ObjectConstSP o = serviceFunc()->object_from_data(data, streamName, isBinary); \
    return T::Coerce(o); \
}\
T::outer_type T::from_string(const std::string& str)\
{\
    spi::ObjectConstSP o = serviceFunc()->object_from_string(str);\
    return T::Coerce(o);\
}\
T::outer_type T::from_file(const char* filename)\
{\
    spi::ObjectConstSP o = serviceFunc()->object_from_file(filename);\
    return T::Coerce(o);\
}\
spi::ObjectType T::object_type = spi::ObjectType(\
  objectName, T::object_from_map, spi::IsInstance<T>, canPut, NULL)

SPI_BEGIN_NAMESPACE

SPI_DECLARE_RC_CLASS(MapObject);

class SPI_IMPORT IMapObject : public Object
{
public:
    void to_map(IObjectMap* objectMap, bool publicOnly) const;
    Value GetValue(const std::string& name) const;
    void SetValue(const std::string& name, const Value& value);
    const std::vector<std::string>& GetNames() const;
    bool Exists(const std::string& name) const;

    // convenience methods for getting values of particular types
    // implemented inline in terms of GetValue and Value access methods
    char GetChar(const std::string& name, bool permissive=false) const
    {
        return GetValue(name).getChar(permissive);
    }

    std::string GetString(const std::string& name, bool permissive=false) const
    {
        return GetValue(name).getString(permissive);
    }

    int GetInt(const std::string& name, bool permissive=false) const
    {
        return GetValue(name).getInt(permissive);
    }

    bool GetBool(const std::string& name, bool permissive=false) const
    {
        return GetValue(name).getBool(permissive);
    }

    double GetDouble(const std::string& name, bool permissive=false) const
    {
        return GetValue(name).getDouble(permissive);
    }

    Date GetDate(const std::string& name, bool permissive=false) const
    {
        return GetValue(name).getDate(permissive);
    }

    MapConstSP GetMap(const std::string& name) const
    {
        return GetValue(name).getMap();
    }

    ObjectConstSP GetObject(const std::string& name) const
    {
        return GetValue(name).getObject();
    }

    template<class T>
    typename T::outer_type GetInstance(const std::string& name) const
    {
        return T::Coerce(GetObject(name));
    }

    std::vector<std::string>
    GetStringVector(const std::string& name, bool permissive=false) const
    {
        return GetValue(name).getStringVector(permissive);
    }

    std::vector<double>
    GetDoubleVector(const std::string& name, bool permissive=false) const
    {
        return GetValue(name).getDoubleVector(permissive);
    }

    std::vector<int>
    GetIntVector(const std::string& name, bool permissive=false) const
    {
        return GetValue(name).getIntVector(permissive);
    }

    std::vector<bool>
    GetBoolVector(const std::string& name, bool permissive=false) const
    {
        return GetValue(name).getBoolVector(permissive);
    }

    std::vector<Date>
    GetDateVector(const std::string& name, bool permissive=false) const
    {
        return GetValue(name).getDateVector(permissive);
    }

    std::vector<MapConstSP>
    GetMapVector(const std::string& name) const
    {
        return GetValue(name).getMapVector();
    }

    std::vector<ObjectConstSP>
    GetObjectVector(const std::string& name) const
    {
        return GetValue(name).getObjectVector();
    }

    template<class T>
    std::vector< typename T::outer_type >
    GetInstanceVector(const std::string& name) const
    {
        return CoerceObjectVector<T>(GetObjectVector(name));
    }

    spi::MatrixData<Value>
    GetMatrix(const std::string& name) const
    {
        return GetValue(name).getMatrix();
    }

protected:
    IMapObject(const MapSP& aMap);
    MapSP m_map;
};

class SPI_IMPORT MapObject : public IMapObject
{
public:
    SPI_DECLARE_MAP_OBJECT_TYPE(MapObject);
    static MapObjectSP Make(const char* className);
    static MapObjectSP Make(const MapSP& aMap);
    static MapObjectSP Make(const MapConstSP& aConstMap);
    static MapObjectSP Make(const MapConstSP& aConstMap,
        ValueToObject& valueToObject);

    typedef MapConstSP inner_type;
    inner_type get_inner() const;
    static inner_type get_inner(const outer_type& o);
protected:
    MapObject(const MapSP& aMap);
};

SPI_END_NAMESPACE

#endif /* SPI_MAP_OBJECT_HPP */


