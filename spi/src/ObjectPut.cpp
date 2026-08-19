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
#include "../ObjectPut.hpp"

#include "ObjectPutMap.hpp"
#include "../InputContext.hpp"
#include "../MapObject.hpp"
#include "../ObjectMap.hpp"
#include "../StringUtil.hpp"

SPI_BEGIN_NAMESPACE

ObjectConstSP ObjectPut(
    const ObjectConstSP& obj,
    const char*          name,
    const Value&         value,
    const InputContext*  context)
{
    std::vector<std::string> names;
    std::vector<Value>       values;

    names.push_back(name);
    values.push_back(value);

    return ObjectPut(obj, names, values, context);
}

ObjectConstSP ObjectPut(
    const ObjectConstSP& obj,
    const std::vector<std::string>& names,
    const std::vector<Value>&       values,
    const InputContext*             context)
{
    if (!context)
        context = InputContext::NoContext();

    ObjectType* ot;
    MapConstSP attributes;

    if (!obj)
    {
        ot = &MapObject::object_type;
        attributes.reset(new Map(ot->get_class_name()));
    }
    else
    {
        ot = obj->get_object_type();
        attributes = obj->get_attributes();
    }

    bool meta_data_only = names.size() == 1 && names[0] == "meta_data";

    if (!ot->can_put() && !meta_data_only)
        throw RuntimeError("Object of type '%s' does not support ObjectPut",
            ot->get_class_name());

    ObjectMap om(attributes);
    ObjectPutMap opm(&om, names, values, context);

    ValueToObject valueToObject(ot->get_service(), new ObjectRefCache());
    ObjectConstSP modifiedObject = ot->make_from_map(&opm, valueToObject);

    const std::vector<std::string>& unused = opm.Unused();
    if (unused.size() > 0)
        throw RuntimeError("Object of type '%s' does not support names '%s'",
            ot->get_class_name(),
            StringJoin(",", unused).c_str());

    return modifiedObject;
}

void ObjectPutMetaData(
    const ObjectConstSP& obj,
    const char* name,
    const Value& value,
    const InputContext* context)
{
    std::vector<std::string> names(1, name);
    std::vector<Value> values(1, value);

    ObjectPutMetaData(obj, names, values, context);
}

void ObjectPutMetaData(
    const ObjectConstSP& obj,
    const std::vector<std::string>& names,
    const std::vector<Value>& values,
    const InputContext* context)
{
    if (!context)
        context = InputContext::NoContext();

    const MapSP& omd = obj->get_meta_data();

    SPI_PRE_CONDITION(names.size() == values.size());

    ObjectMap om(omd);
    ObjectPutMap opm(&om, names, values, context);

    const MapSP& combined = opm.ExportMap();

    obj->update_meta_data(combined);
}

SPI_END_NAMESPACE
