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
/**
***************************************************************************
** DateTime.hpp
***************************************************************************
** DateTime class which internally uses the number of days since some
** arbitrary base date (1601-01-01 in the Gregorian calendar) as the date
** and the number of seconds since midnight as the time.
**
** Currently we do not support timezones - so essentially DateTime should
** either be defined in universal time or local time and the context should
** know which is being used.
***************************************************************************
*/

#ifndef SPI_DATETIME_HPP
#define SPI_DATETIME_HPP

#include "Date.hpp"

SPI_BEGIN_NAMESPACE

class SPI_IMPORT TimeDelta
{
public:
    TimeDelta(int days, int time=0);
    TimeDelta(double diff);

    int Days() const;
    int Time() const;
    TimeDelta Negate() const;
    operator double() const;

private:
    int m_days;
    int m_time;
};

/**
 * DateTime class contains no virtual functions - hence the size of the
 * class is exactly the same as the size of its data fields - hence we go
 * for minimality, so that a vector of DateTime is as cheap as possible
 * (for example).
 */
class SPI_IMPORT DateTime
{
public:
    // constructors
    DateTime();
    DateTime(const spi::Date& date, int time);
    DateTime(const spi::Date& date, int hours, int minutes, int seconds);
    DateTime(double dateAndTime);
    static DateTime FromString(const std::string& str);
    static DateTime Now(bool universal=false);

    // access methods
    void HMS(int* hours, int* minutes, int* seconds) const;
    spi::Date Date() const;
    int Time() const;
    bool IsDate() const;
    operator spi::Date() const;
    operator bool() const;
    operator double() const;
    std::string ToString() const;
    operator std::string() const;

    bool Equals(const DateTime& other) const;
    bool Before(const DateTime& other) const;
    DateTime Add(const TimeDelta& diff) const;
    TimeDelta Diff(const DateTime& other) const;

    static double TimeToDouble(int time);
    static int DoubleToTime(double time);
private:
    spi::Date m_date;
    int m_time;
};

SPI_END_NAMESPACE

// simple DateTime arithmetic
// it is possible that we don't really need all the comparison methods here
// however it should help to avoid ambiguity

inline bool operator == (spi::DateTime d1, spi::DateTime d2)
{
    return d1.Equals(d2);
}

inline bool operator != (spi::DateTime d1, spi::DateTime d2)
{
    return !d1.Equals(d2);
}

inline bool operator < (spi::DateTime d1, spi::DateTime d2)
{
    return d1.Before(d2);
}

inline bool operator > (spi::DateTime d1, spi::DateTime d2)
{
    return d2.Before(d1);
}

inline bool operator <= (spi::DateTime d1, spi::DateTime d2)
{
    if (d1.Before(d2))
        return true;
    return d1.Equals(d2);
}

inline bool operator >= (spi::DateTime d1, spi::DateTime d2)
{
    if (d2.Before(d1))
        return true;
    return d1.Equals(d2);
}

// difference between two date times is returned as TimeDelta

inline spi::TimeDelta operator - (spi::DateTime d1, spi::DateTime d2)
{
    return d1.Diff(d2);
}

// we can a TimeDelta and get another DateTime

inline spi::DateTime operator + (spi::DateTime dt, spi::TimeDelta delta)
{
    return dt.Add(delta);
}

// we can subtract a TimeDelta and get another DateTime

inline spi::DateTime operator - (spi::DateTime dt, spi::TimeDelta delta)
{
    return dt.Add(delta.Negate());
}

#endif


