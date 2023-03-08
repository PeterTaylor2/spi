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
#include "EnumHelper.hpp"

SPI_BEGIN_NAMESPACE

EnumInfo::EnumInfo()
    :
    m_name(),
    m_enumerands()
{}

void EnumInfo::Initialise(
    const std::string& name,
    const std::vector<std::string>& enumerands)
{
    m_name = name;
    m_enumerands = enumerands;
#if 0
    for (size_t i = 0; i < m_enumerands.size(); ++i)
    {
        std::cout << "adding " << m_name << "::" << m_enumerands[i] << "\n";
    }
#endif
}

bool EnumInfo::Initialised() const
{
    return !m_name.empty();
}

std::string EnumInfo::Name() const
{
    return m_name;
}

std::vector<std::string> EnumInfo::Enumerands() const
{
    return m_enumerands;
}

SPI_END_NAMESPACE
