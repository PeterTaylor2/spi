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
** MapRefCache.hpp
***************************************************************************
** Defines MapRefCache used by some streamers
***************************************************************************
*/

#ifndef SPI_MAP_REF_CACHE_HPP
#define SPI_MAP_REF_CACHE_HPP

#include "RefCounter.hpp"

#include <map>
#include <vector>

SPI_BEGIN_NAMESPACE

SPI_DECLARE_RC_CLASS(Object);

// the point of this class is to maintain a reference from id (part of
// each object - count starts at 1 each time we start the process) and
// mapRef (count starts at 1 each time we start a map or start to stream
// objects to file)
//
// we also find that we need to keep track of objects created - this is
// because sometimes when converting an object to map we find that we
// create new wrapper class instances and that we need to keep a handle on
// these in order to keep track of identical objects
class SPI_IMPORT MapRefCache
{
public:
    MapRefCache();
    size_t count(int id) const;
    size_t size() const;

    void insert(int mapRef, const ObjectConstSP& obj);
    int mapRef(int id) const;
    void clear();

private:
    std::map<int,int> m_indexIdMapRef;
    std::vector<ObjectConstSP> m_objects;

    MapRefCache(const MapRefCache&);
    MapRefCache& operator=(const MapRefCache&);
};

SPI_END_NAMESPACE

#endif

