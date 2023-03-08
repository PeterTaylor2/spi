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

#ifndef _SPI_REPLAY_MAP_HPP_
#define _SPI_REPLAY_MAP_HPP_

/**
****************************************************************************
* Header file: spi_replay_map.hpp
****************************************************************************
*/

#include "spi_replay_map_classes.hpp"

SPI_REPLAY_BEGIN_NAMESPACE

/**
****************************************************************************
* No description.
*
* @param m
****************************************************************************
*/
REPLAY_IMPORT
std::string MapClassName(
    const spi::MapObjectSP& m);

/**
****************************************************************************
* No description.
*
* @param m
****************************************************************************
*/
REPLAY_IMPORT
std::vector<std::string> MapFieldNames(
    const spi::MapObjectSP& m);

/**
****************************************************************************
* No description.
*
* @param m
****************************************************************************
*/
REPLAY_IMPORT
int MapRef(
    const spi::MapObjectSP& m);

/**
****************************************************************************
* Returns the value and the value type for a name in a Map.
*
* Sometimes the same primitive type is returned for different value types -
* hence the need to return the value type to distinguish.
*
* @param m
* @param name
*
* @output value
* @output valueType
****************************************************************************
*/
REPLAY_IMPORT
void MapGetValue(
    const spi::MapObjectSP& m,
    const std::string& name,
    spi::Variant& value,
    ValueType& valueType);

REPLAY_IMPORT
std::tuple< spi::Variant, ValueType > MapGetValue(
    const spi::MapObjectSP& m,
    const std::string& name);

SPI_REPLAY_END_NAMESPACE

#endif /* _SPI_REPLAY_MAP_HPP_*/

