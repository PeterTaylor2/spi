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
#include "ObjectRef.hpp"
#include "Object.hpp"

#include "RuntimeError.hpp"
#include "Map.hpp"

SPI_BEGIN_NAMESPACE

ObjectRef::ObjectRef(int id)
    :
    m_id(id)
{
    SPI_PRE_CONDITION(id > 0);
}

ObjectRef::operator int() const
{
    return m_id;
}

ObjectRefCache::ObjectRefCache()
    :
    m_dict()
{}

bool ObjectRefCache::add_object(int ref, const ObjectConstSP& obj)
{
    if (!obj)
        return false;

    bool exists = m_dict.count(ref) > 0;
    m_dict[ref] = obj; // overwrite regardless of whether it exists already
    return exists;
}

ObjectConstSP ObjectRefCache::find_object(const ObjectRef& id) const
{
    return find_object(int(id));
}

ObjectConstSP ObjectRefCache::find_object(int ref) const
{
    std::map<int,ObjectConstSP>::const_iterator iter = m_dict.find(ref);

    if (iter == m_dict.end())
        return ObjectConstSP();

    return iter->second;
}

SPI_END_NAMESPACE
