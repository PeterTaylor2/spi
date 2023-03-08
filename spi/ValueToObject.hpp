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
** ValueToObject.hpp
***************************************************************************
** Class for converting values to objects.
***************************************************************************
*/

#ifndef SPI_VALUE_TO_OBJECT_HPP
#define SPI_VALUE_TO_OBJECT_HPP

#include "Object.hpp"

#include <map>
#include <istream>
#include <ostream>

SPI_BEGIN_NAMESPACE

SPI_DECLARE_RC_CLASS(Service);
SPI_DECLARE_RC_CLASS(ObjectRefCache);

class Value;

/**
 * Provides the information to help with converting a map to an object.
 *
 * The issues are that the we need to find the object constructor (for
 * which we use the service), and that we need to be able to get from
 * an object reference to an object for objects we have seen before.
 */
class SPI_IMPORT ValueToObject
{
public:
    ValueToObject(
        const ServiceConstSP& svc,
        const ObjectRefCacheSP& objectCache);
    ~ValueToObject();

    ObjectConstSP ObjectFromValue(
        const Value& value,
        ObjectType* objectType);

    // converts a Value by resolving maps and references to objects
    Value ResolveObject(const Value& value);
    ServiceConstSP Service() const;

private:
    ServiceConstSP   m_service;
    ObjectRefCacheSP m_objectCache;

    ValueToObject(const ValueToObject&);
    ValueToObject& operator=(const ValueToObject&);
};

SPI_END_NAMESPACE

#endif
