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
** DateTime.cpp
***************************************************************************
** Implementation of the DateTime and TimeDelta class.
***************************************************************************
*/

#include "DateTime.hpp"

#include "RuntimeError.hpp"
#include "StringUtil.hpp"

#include <time.h>
#include "../../spi_util/DateUtil.hpp"
#include "../../spi_util/Utils.hpp"

SPI_BEGIN_NAMESPACE

namespace
{
    const int SECONDS_PER_DAY = 24*60*60;
    void SplitTime(int time, int* hours, int* minutes, int* seconds)
    {
        bool negate = time < 0;
        if (negate)
            time = -time;

        SPI_PRE_CONDITION(time < SECONDS_PER_DAY);

        int h,m,s;

        s = time % 60;
        int hm = (time - s) / 60;
        m = hm % 60;
        h = (hm - m) / 60;

        SPI_POST_CONDITION(s >=0 && s < 60);
        SPI_POST_CONDITION(m >= 0 && m < 60);
        SPI_POST_CONDITION(h >= 0 && h < 24);

        if (hours)
            *hours = negate ? -h : h;

        if (minutes)
            *minutes = negate ? -m : m;

        if (seconds)
            *seconds = negate ? -s : s;
    }
}

/*
************************************************************************
* Implementation of TimeDelta
************************************************************************
*/
TimeDelta::TimeDelta(int days, int time)
    :
    m_days(days),
    m_time(time)
{
    if (days > 0 && time < 0)
        SPI_THROW_RUNTIME_ERROR("Cannot have days " << days
            << " < 0 and time " << time << " > 0");

    if (days < 0 && time > 0)
        SPI_THROW_RUNTIME_ERROR("Cannot have days " << days
            << " > 0 and time " << time << " < 0");

    if (time > 0 && time >= SECONDS_PER_DAY)
        SPI_THROW_RUNTIME_ERROR("More time " << time <<
            " than seconds in a day (" << SECONDS_PER_DAY << ")");

    if (time < 0 && -time >= SECONDS_PER_DAY)
        SPI_THROW_RUNTIME_ERROR("More negative time " << time <<
            " than seconds in a day (" << SECONDS_PER_DAY << ")");

}

TimeDelta::TimeDelta(double diff)
    :
    m_days(),
    m_time()
{
    if (diff >= 0.0)
    {
        m_days = (int)diff;
        m_time = spi::DateTime::DoubleToTime(diff - m_days);
        if (m_time == SECONDS_PER_DAY)
        {
            m_days += 1;
            m_time = 0;
        }
        SPI_POST_CONDITION(m_time >= 0 && m_time < SECONDS_PER_DAY);
    }
    else
    {
        double minusDiff = -diff;
        int minusDays = (int)minusDiff;
        int minusTime = spi::DateTime::DoubleToTime(
            minusDiff - minusDays);
        if (minusTime == SECONDS_PER_DAY)
        {
            minusDays += 1;
            minusTime = 0;
        }
        SPI_POST_CONDITION(minusTime >= 0 && minusTime < SECONDS_PER_DAY);

        m_days = -minusDays;
        m_time = -minusTime;
    }
}

int TimeDelta::Days() const
{
    return m_days;
}

int TimeDelta::Time() const
{
    return m_time;
}

TimeDelta TimeDelta::Negate() const
{
    return TimeDelta(-m_days, -m_time);
}

TimeDelta::operator double() const
{
    return m_days + (double)m_time / SECONDS_PER_DAY;
}

/*
**************************************************************************
** Implementation of DateTime
**************************************************************************
*/
DateTime::DateTime()
    :
    m_date(),
    m_time()
{}

DateTime::DateTime(const spi::Date& date, int time)
    :
    m_date(date),
    m_time(time)
{
    SPI_PRE_CONDITION(time >= 0);
    SPI_PRE_CONDITION(time < SECONDS_PER_DAY);
}

DateTime::DateTime(
    const spi::Date& date,
    int hours,
    int minutes,
    int seconds)
    :
    m_date(date),
    m_time()
{
    SPI_PRE_CONDITION(seconds >= 0 && seconds < 60);
    SPI_PRE_CONDITION(minutes >= 0 && minutes < 60);
    SPI_PRE_CONDITION(hours >= 0 && hours < 24);
    int time = seconds + 60*(minutes + 60*hours);

    SPI_POST_CONDITION(time >= 0 && time < SECONDS_PER_DAY);

    m_time = time;
}

DateTime::DateTime(double dateAndTime)
    :
    m_date(),
    m_time()
{
    SPI_PRE_CONDITION(dateAndTime >= 0.0);

    int date = (int)dateAndTime;
    int time = DoubleToTime(dateAndTime - date);
    if (time == SECONDS_PER_DAY)
    {
        date += 1;
        time = 0;
    }
    SPI_PRE_CONDITION(time >= 0);
    SPI_PRE_CONDITION(time < SECONDS_PER_DAY);

    m_date = date;
    m_time = time;
}

DateTime DateTime::FromString(const std::string& str)
{
    try
    {
        std::vector<std::string> parts = spi::StringSplit(str, ' ');
        if (parts.size() == 1) // time only - no seconds
        {
            std::vector<std::string> timeParts = spi::StringParser(parts[0], ":");
            SPI_POST_CONDITION(timeParts.size() == 2);
            return DateTime(
                spi::Date(),
                spi::StringToInt(timeParts[0]),
                spi::StringToInt(timeParts[1]),
                0);
        }
        else if (parts.size() == 2) // date & time (including seconds)
        {
            std::vector<std::string> dateParts = spi::StringParser(parts[0], "--");
            std::vector<std::string> timeParts = spi::StringParser(parts[1], "::");
            SPI_POST_CONDITION(dateParts.size() == 3);
            SPI_POST_CONDITION(timeParts.size() == 3);
            return DateTime(
                spi::Date(
                    spi::StringToInt(dateParts[0]),
                    spi::StringToInt(dateParts[1]),
                    spi::StringToInt(dateParts[2])),
                spi::StringToInt(timeParts[0]),
                spi::StringToInt(timeParts[1]),
                spi::StringToInt(timeParts[2]));
        }
    }
    catch (std::exception& e)
    {
        SPI_THROW_RUNTIME_ERROR("Could not parse '" << str << "' to DateTime. "
            << "Format should be YYYY-MM-DD HH:MM:SS. "
            << "Error was " << e.what());
    }

    SPI_THROW_RUNTIME_ERROR("Could not parse '" << str << "' to DateTime. "
        << "Format should be YYYY-MM-DD HH:MM:SS.");

}

DateTime DateTime::Now(bool universal)
{
    time_t t;
    time(&t);

#ifdef _MSC_VER
    struct tm* now = universal ? gmtime(&t) : localtime(&t);
    return DateTime(
        spi::Date(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday),
        now->tm_hour, now->tm_min, now->tm_sec);
#else
    struct tm now;
    if (universal)
        gmtime_r(&t, &now);
    else
        localtime_r(&t, &now);
    return DateTime(
        spi::Date(now.tm_year + 1900, now.tm_mon + 1, now.tm_mday),
        now.tm_hour, now.tm_min, now.tm_sec);
#endif
}

void DateTime::HMS(
    int* hours,
    int* minutes,
    int* seconds) const
{
    SplitTime(m_time, hours, minutes, seconds);
}

spi::Date DateTime::Date() const
{
    return m_date;
}

int DateTime::Time() const
{
    return m_time;
}

bool DateTime::IsDate() const
{
    return m_time == 0;
}

DateTime::operator spi::Date() const
{
    if (m_time != 0)
        SPI_THROW_RUNTIME_ERROR("Time must be zero");

    return m_date;
}

DateTime::operator bool() const
{
    return m_date || m_time;
}

DateTime::operator double() const
{
    return (int)(m_date) + (double)m_time / SECONDS_PER_DAY;
}

std::string DateTime::ToString() const
{
    int hours, minutes, seconds;

    HMS(&hours, &minutes, &seconds);

    if (!m_date && seconds == 0)
    {
        return spi_util::StringFormat("%02d:%02d", hours, minutes);
    }
    else
    {
        int years, months, days;
        m_date.YMD(&years, &months, &days);

        return spi_util::StringFormat("%04d-%02d-%02d %02d:%02d:%02d",
            years, months, days, hours, minutes, seconds);
    }
}

DateTime::operator std::string() const
{
    return ToString();
}

bool DateTime::Equals(const DateTime& other) const
{
    return m_date == other.m_date && m_time == other.m_time;
}

bool DateTime::Before(const DateTime& other) const
{
    if (m_date < other.m_date)
        return true;
    if (m_date == other.m_date)
        return m_time < other.m_time;
    return false;
}

DateTime DateTime::Add(const TimeDelta& diff) const
{
    spi::Date date = m_date + diff.Days();
    int time = m_time + diff.Time();

    // no need to loop - cannot get too far out of range
    if (time < 0)
    {
        time += SECONDS_PER_DAY;
        date -= 1;
    }
    if (time > SECONDS_PER_DAY)
    {
        time -= SECONDS_PER_DAY;
        date += 1;
    }

    return DateTime(date, time);
}

TimeDelta DateTime::Diff(const DateTime& other) const
{
    int days = m_date - other.m_date;
    int time = m_time - other.m_time;

    // no need to loop - cannot get too far out of range
    if (time < 0 && days > 0)
    {
        days -= 1;
        time += SECONDS_PER_DAY;
    }

    return TimeDelta(days, time);
}

double DateTime::TimeToDouble(int time)
{
    SPI_PRE_CONDITION(time >= 0 && time <= SECONDS_PER_DAY);
    return (double)time / SECONDS_PER_DAY;
}

int DateTime::DoubleToTime(double time)
{
    SPI_PRE_CONDITION(time >= 0.0 && time <= 1.0);
    int result = (int)(time * SECONDS_PER_DAY + 0.5);
    SPI_POST_CONDITION(result >= 0 && result <= SECONDS_PER_DAY);
    return result;
}

SPI_END_NAMESPACE

std::ostream& operator<<(std::ostream& ostr, const spi::DateTime& dt)
{
    ostr << dt.ToString();
    return ostr;
}

