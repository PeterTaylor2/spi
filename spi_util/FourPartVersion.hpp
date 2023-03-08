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
#ifndef SPI_FOUR_PART_VERSION_HPP
#define SPI_FOUR_PART_VERSION_HPP

/*
***************************************************************************
* FourPartVersion.hpp
***************************************************************************
* Class for dealing with 4-part version numbers.
* Numbers allowed are in the range 0..99
***************************************************************************
*/

#include "DeclSpec.h"
#include "Namespace.hpp"

#include <string>

SPI_UTIL_NAMESPACE

class SPI_UTIL_IMPORT FourPartVersion
{
public:
    FourPartVersion(int v1, int v2, int v3, int v4);
    FourPartVersion(unsigned int version);
    FourPartVersion(const char* version);
    FourPartVersion(const std::string& version);

    unsigned int version() const;
    int v1() const;
    int v2() const;
    int v3() const;
    int v4() const;
    std::string versionString() const;

private:
    int m_v1;
    int m_v2;
    int m_v3;
    int m_v4;

    void init(int v1, int v2, int v3, int v4);
    void init(const char* version);
};

SPI_UTIL_END_NAMESPACE

#endif
