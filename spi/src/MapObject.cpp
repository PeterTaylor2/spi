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

#include "MapObject.hpp"
#include "RuntimeError.hpp"
#include "IObjectMap.hpp"
#include "Map.hpp"
#include "Service.hpp"
#include "ValueToObject.hpp"

SPI_BEGIN_NAMESPACE

IMapObject::IMapObject(const MapSP& aMap)
:
m_map(aMap)
{}

void IMapObject::to_map(IObjectMap* objectMap, bool publicOnly) const
{
    // we can ignore publicOnly parameter in this context
    objectMap->ImportMap(m_map.get());
}

Value IMapObject::GetValue(const std::string& name) const
{
    return m_map->GetValue(name);
}

void IMapObject::SetValue(const std::string& name, const Value& value)
{
    m_map->SetValue(name, value);
}

const std::vector<std::string>& IMapObject::GetNames() const
{
    return m_map->FieldNames();
}

MapObjectSP MapObject::Make(const MapSP& aMap)
{
    return MapObjectSP(new MapObject(aMap));
}

MapObjectSP MapObject::Make(const MapConstSP& aConstMap)
{
    if (!aConstMap)
        return MapObjectSP();
    return MapObjectSP(new MapObject(aConstMap->Copy()));
}

MapObjectSP MapObject::Make(const MapConstSP& aConstMap,
                            ValueToObject& valueToObject)
{
    if (!aConstMap)
        return MapObjectSP();
    return MapObjectSP(new MapObject(aConstMap->Copy(valueToObject)));
}

MapObject::MapObject(const MapSP& aMap)
    :
IMapObject(aMap)
{}

MapConstSP MapObject::get_inner() const
{
    return m_map;
}

MapConstSP MapObject::get_inner(const MapObject::outer_type& o)
{
    if (!o)
        return MapConstSP();
    return o->get_inner();
}

SPI_IMPLEMENT_MAP_OBJECT_TYPE(MapObject, "Map", Service::CommonService, true);

SPI_END_NAMESPACE
