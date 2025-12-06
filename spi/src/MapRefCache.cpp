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
#include "MapRefCache.hpp"
#include "RuntimeError.hpp"
#include "LexerReader.hpp"
#include "MapReader.hpp"
#include "SHA.hpp"

#include <spi_util/Lexer.hpp>
#include <spi_util/StringUtil.hpp>
#include <spi_util/StreamUtil.hpp>
#include <spi_util/Utils.hpp>
#include <spi_util/CompressUtil.hpp>

#include "Service.hpp"
#include "ObjectMap.hpp"
#include <string.h>
#include <float.h>
#include <math.h>
#include "platform.h"

#undef SPI_UTIL_CLOCK_EVENTS
#include <spi_util/ClockUtil.hpp>

SPI_BEGIN_NAMESPACE

MapRefCache::MapRefCache()
:
m_indexIdMapRef(),
m_objects()
{}

size_t MapRefCache::count(int id) const
{
    return m_indexIdMapRef.count(id);
}

size_t MapRefCache::size() const
{
    return m_indexIdMapRef.size();
}

void MapRefCache::insert(int mapRef, const ObjectConstSP& obj)
{
    int id = obj->get_id();

    m_indexIdMapRef[id] = mapRef;
    m_objects.push_back(obj);
}

int MapRefCache::mapRef(int id) const
{
    std::map<int,int>::const_iterator iter = m_indexIdMapRef.find(id);
    if (iter == m_indexIdMapRef.end())
        throw RuntimeError("Could not find %d in idMapRef cache", id);
    return iter->second;
}

void MapRefCache::clear()
{
    m_indexIdMapRef.clear();
    m_objects.clear();
}

SPI_END_NAMESPACE

