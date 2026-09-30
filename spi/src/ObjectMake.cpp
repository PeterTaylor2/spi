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
#include "../ObjectMake.hpp"

#include "ObjectPutMap.hpp"
#include "../InputContext.hpp"
#include "../ObjectMap.hpp"
#include "../StringUtil.hpp"
#include "../Service.hpp"

SPI_BEGIN_NAMESPACE

ObjectConstSP ObjectMake(
    const std::string& className,
    const std::vector<std::string>& names,
    const std::vector<Value>& values,
    const InputContext* context)
{
    if (!context)
        context = InputContext::NoContext();

    ObjectType* ot = Service::CommonService()->get_object_type(className);

    MapConstSP m(new Map(className.c_str()));
    ObjectMap om(m);

    ObjectPutMap opm(&om, names, values, context);

    ValueToObject valueToObject(ot->get_service(), new ObjectRefCache());
    ObjectConstSP obj = ot->make_from_map(&opm, valueToObject);

    // note that we do not see if there are any unused fields
    // this is similar to the rule that when we de-serialize an object
    // we allow fields to be ignored

    return obj;
}

SPI_END_NAMESPACE
