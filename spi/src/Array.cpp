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
** Array.cpp
***************************************************************************
** Concrete implementations of IArray interface.
***************************************************************************
*/

#include "Array.hpp"

SPI_BEGIN_NAMESPACE

namespace {
    size_t DimensionsSize(const std::vector<size_t>& dimensions)
    {
        if (dimensions.size() == 0)
            return 0;

        size_t size = 1;
        for (size_t i = 0; i < dimensions.size(); ++i)
            size *= dimensions[i];

        return size;
    }
}

ValueArray::ValueArray(size_t size)
:
m_vec(size),
m_dimensions(1,size)
{}

ValueArray::ValueArray(const std::vector<size_t>& dimensions)
:
m_vec(),
m_dimensions(dimensions)
{
    m_vec.resize(DimensionsSize(dimensions));
}


ValueArray::ValueArray(const std::vector<Value>& vec, size_t size)
    :
    m_vec(vec)
{
    if (size != vec.size())
    {
        throw RuntimeError("Bad size %d - should be %d",
            (int)size, (int)vec.size());
    }

    m_dimensions.resize(1, size);
}

ValueArray::ValueArray(
    const std::vector<Value>& vec,
    const std::vector<size_t>& dimensions)
    :
    m_vec(vec),
    m_dimensions(dimensions)
{
    if (m_vec.size() != DimensionsSize(dimensions))
        throw RuntimeError("Array size mismatch between dimensions and data");
}

size_t ValueArray::size() const
{
    return m_vec.size();
}

Value ValueArray::getItem(size_t i) const
{
    if (i >= m_vec.size())
    {
        throw RuntimeError("Index %ld out of range", (long)i);
    }
    return m_vec[i];
}

std::vector<Value> ValueArray::getVector() const
{
    return m_vec;
}

void ValueArray::assign(size_t i, const Value& value)
{
    if (i >= m_vec.size())
        throw RuntimeError("Out of range");
    m_vec[i] = value;
}

std::vector<size_t> ValueArray::dimensions() const
{
    return m_dimensions;
}

SPI_END_NAMESPACE


