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
** ObjectPut.hpp
***************************************************************************
** Defines functionality for ObjectPut.
**
** Two use modes:
** 1. Modifying an object of known type.
** 2. Creating a pure map without a known type.
**
** Start with the first use mode - we create a sub-class of IObjectMap
** which contains two maps - the map of the object to be modified plus an
** in context map of the changes.
**
** Second use case will have an in context map of the changes, but you will
** be expected to give clues as to the data types by your choice of names.
***************************************************************************
*/

#ifndef SPI_OBJECT_PUT_HPP
#define SPI_OBJECT_PUT_HPP

#include "Object.hpp"
#include "ObjectHelper.hpp"
#include "Value.hpp"

#include <string>
#include <vector>

SPI_BEGIN_NAMESPACE

SPI_DECLARE_RC_CLASS(Map);
SPI_DECLARE_RC_CLASS(MapObject);

SPI_IMPORT
ObjectConstSP ObjectPut(
    const ObjectConstSP& obj,
    const char* name,
    const Value& value,
    const InputContext*             context);

ObjectConstSP ObjectPut(
    const ObjectConstSP& obj,
    const std::vector<std::string>& names,
    const std::vector<Value>&       values,
    const InputContext*             context);

//class MapObject : public Object
//{
//public:
//    MapObject(
//        const std::vector<std::string>& names,
//        const std::vector<Value>&       values,
//        const InputContext*             context);
//
//    SPI_DECLARE_OBJECT_TYPE(MapObject);
//
//private:
//    MapSP m_map;
//};

SPI_END_NAMESPACE

#endif
