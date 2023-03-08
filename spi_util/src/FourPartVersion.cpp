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
* FourPartVersion.hpp
***************************************************************************
* Class for dealing with 4-part version numbers.
* Numbers allowed are in the range 0..99
***************************************************************************
*/

#include "FourPartVersion.hpp"

#include "RuntimeError.hpp"
#include "StringUtil.hpp"

#include <string.h>

SPI_UTIL_NAMESPACE

FourPartVersion::FourPartVersion(
    int v1,
    int v2,
    int v3,
    int v4)
    :
    m_v1(),
    m_v2(),
    m_v3(),
    m_v4()
{
    init(v1, v2, v3, v4);
}

FourPartVersion::FourPartVersion(unsigned int version)
    :
    m_v1(),
    m_v2(),
    m_v3(),
    m_v4()
{
    int v[4];

    for (int i = 0; i < 4; ++i)
    {
        v[3-i] = (int)(version % 256);
        version /= 256;
    }

    init(v[0], v[1], v[2], v[3]);
}

FourPartVersion::FourPartVersion(const char* version)
    :
    m_v1(),
    m_v2(),
    m_v3(),
    m_v4()
{
    init(version);
}

FourPartVersion::FourPartVersion(const std::string& version)
    :
    m_v1(),
    m_v2(),
    m_v3(),
    m_v4()
{
    init(version.c_str());
}

void FourPartVersion::init(const char* version)
{
    SPI_UTIL_PRE_CONDITION(strlen(version) < 12);

    std::vector<std::string> fourParts = CStringParser(version, "...");
    SPI_UTIL_POST_CONDITION(fourParts.size() == 4);

    int v[4] = {0,0,0,0};

    for (size_t i = 0; i < 4; ++i)
    {
        if (fourParts[i].empty())
        {
            v[i] = 0;
        }
        else
        {
            try
            {
                v[i] = StringToInt(fourParts[i]);
                if (v[i] < 0 || v[i] > 255)
                    throw RuntimeError("out of range");
            }
            catch (std::exception&)
            {
                throw RuntimeError(
                    "%s: Invalid version string %s. Should be V1.V2.V3.V4 where "
                    "each element is in the range 0..255",
                    __FUNCTION__, version);
            }
        }
    }

    init((int)v[0], (int)v[1], (int)v[2], (int)v[3]);
}

void FourPartVersion::init(
    int v1,
    int v2,
    int v3,
    int v4)
{
    SPI_UTIL_PRE_CONDITION(v1 >= 0 && v1 < 256);
    SPI_UTIL_PRE_CONDITION(v2 >= 0 && v2 < 256);
    SPI_UTIL_PRE_CONDITION(v3 >= 0 && v3 < 256);
    SPI_UTIL_PRE_CONDITION(v4 >= 0 && v4 < 256);

    m_v1 = v1;
    m_v2 = v2;
    m_v3 = v3;
    m_v4 = v4;
}

unsigned int FourPartVersion::version() const
{
    unsigned int version = ((m_v1 * 256 + m_v2) * 256 + m_v3) * 256 + m_v4;
    return version;
}

int FourPartVersion::v1() const
{
    return m_v1;
}

int FourPartVersion::v2() const
{
    return m_v2;
}

int FourPartVersion::v3() const
{
    return m_v3;
}

int FourPartVersion::v4() const
{
    return m_v4;
}

std::string FourPartVersion::versionString() const
{
    std::ostringstream oss;

    oss << m_v1 << '.' << m_v2 << '.' << m_v3 << '.' << m_v4;

    return oss.str();
}

SPI_UTIL_END_NAMESPACE
