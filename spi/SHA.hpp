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
#ifndef SPI_SHA_HPP
#define SPI_SHA_HPP

#include "Map.hpp"
#include "Object.hpp"

SPI_BEGIN_NAMESPACE

SPI_IMPORT
std::string SHA1(const MapConstSP& map);

SPI_IMPORT
std::string SHA1(const ObjectConstSP& object);

SPI_IMPORT
std::string SHA256(const MapConstSP& map);

SPI_IMPORT
std::string SHA256(const ObjectConstSP& object);

SPI_END_NAMESPACE

#endif /* SPI_SHA256_HPP */


