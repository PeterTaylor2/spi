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

#ifndef _SPI_REPLAY_MAP_HELPER_HPP_
#define _SPI_REPLAY_MAP_HELPER_HPP_

/**
****************************************************************************
* Header file: spi_replay_map_helper.hpp
****************************************************************************
*/

#include "spi_replay_map_classes.hpp"
#include "replay_dll_type_converters.hpp"

SPI_REPLAY_BEGIN_NAMESPACE

std::string MapClassName_Helper(
    const spi::MapConstSP& m);

std::vector<std::string> MapFieldNames_Helper(
    const spi::MapConstSP& m);

int MapRef_Helper(
    const spi::MapConstSP& m);

void MapGetValue_Helper(
    const spi::MapConstSP& m,
    const std::string& name,
    spi::Variant& value,
    SPI_NAMESPACE::Value::Type& valueType);

void map_register_object_types(const spi::ServiceSP& svc);

SPI_REPLAY_END_NAMESPACE

#endif /* _SPI_REPLAY_MAP_HELPER_HPP_*/

