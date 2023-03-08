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
** Date.cpp
***************************************************************************
** Implementation of the Date class.
***************************************************************************
*/

#include "Date.hpp"

#include "RuntimeError.hpp"
#include "StringUtil.hpp"

#include <time.h>
#include "../../spi_util/DateUtil.hpp"
#include "../../spi_util/Utils.hpp"

SPI_BEGIN_NAMESPACE

Date::Date() : m_dt(0)
{}

Date::Date(int dt) : m_dt(dt)
{
    SPI_PRE_CONDITION(dt >= 0);
}

Date::Date(long dt) : m_dt(spi_util::IntegerCast<int>(dt))
{
    SPI_PRE_CONDITION(dt >= 0);
}

Date::Date(int year, int month, int day) : m_dt(0)
{
    SPI_PRE_CONDITION(year > 1600);
    SPI_PRE_CONDITION(month >= 1 && month <= 12);
    SPI_PRE_CONDITION(day >= 1 && day <= 31);

    m_dt = spi_util::YMDToDate(year, month, day);
}

Date Date::FromString(const std::string& str)
{
    return Date(spi_util::StringToDate(str));
}

Date Date::Today()
{
    time_t t = time(0);

#ifdef _MSC_VER
    struct tm* now = localtime(&t);
    return Date(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
#else
    struct tm now;
    localtime_r(&t, &now);
    return Date(now.tm_year + 1900, now.tm_mon + 1, now.tm_mday);
#endif
}

void Date::YMD(int* year, int* month, int* day) const
{
    spi_util::DateToYMD((int)m_dt, year, month, day);
}

int Date::Year() const
{
    int year;
    spi_util::DateToYMD(m_dt, &year, NULL, NULL);
    return year;
}

int Date::Month() const
{
    int month;
    spi_util::DateToYMD(m_dt, NULL, &month, NULL);
    return month;
}

int Date::Day() const
{
    int day;
    spi_util::DateToYMD(m_dt, NULL, NULL, &day);
    return day;
}

Date::operator int() const
{
    return m_dt;
}

Date::operator long() const
{
    return m_dt;
}

Date::operator bool() const
{
    return m_dt != 0;
}

Date::operator std::string() const
{
    return spi_util::DateToString((int)m_dt);
}

std::string Date::ToString() const
{
    return spi_util::DateToString((int)m_dt);
}

Date& Date::operator += (int days)
{
    int dt = m_dt + days;
    if (dt < 0)
        throw RuntimeError("Adding %d days to %s causes date overflow",
                           days, ToString().c_str());
    m_dt = dt;
    return *this;
}

Date& Date::operator -= (int days)
{
    int dt = m_dt - days;
    if (dt < 0)
        throw RuntimeError("Subtracting %d days from %s causes date overflow",
                           days, ToString().c_str());
    m_dt = dt;
    return *this;
}

SPI_END_NAMESPACE
