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
#include "DateUtil.hpp"
#include "RuntimeError.hpp"
#include "StringUtil.hpp"

SPI_UTIL_NAMESPACE

static int  endOfMonthLeapYear[] = {
    -1, 30, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

static int  endOfMonthNoLeapYear[] = {
    -1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364};

static int daysInMonthLeapYear[] = {
    0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static int daysInMonthNoLeapYear[] = {
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


static const int ONE_YEAR       = 365;
static const int FOUR_YEARS     = ONE_YEAR * 4 + 1;
static const int ONE_CENTURY    = ONE_YEAR * 100 + 24; // 24 leap years
static const int FOUR_CENTURIES = ONE_CENTURY * 4 + 1; // 97 leap years

static void DateToYear(int date, int* pYear, int* pDayInYear)
{
    SPI_UTIL_PRE_CONDITION(date >= 0);

    int work = date;
    int year = 1601;

    while (work >= FOUR_CENTURIES)
    {
        work -= FOUR_CENTURIES;
        year += 400;
    }

    // deal with special case of 31st December 2000 (etc) by
    // only going through this loop at most three times
    for (int i = 0; work >= ONE_CENTURY && i < 3; ++i)
    {
        work -= ONE_CENTURY;
        year += 100;
    }

    // rather than stepping through 4 years at a time we will just
    // divide to get the number of blocks of 4 years
    if (work >= FOUR_YEARS)
    {
        int numFourYears = work / FOUR_YEARS;

        work -= numFourYears * FOUR_YEARS;
        year += (numFourYears << 2);
    }

    // deal with special case of 31st December in a leap year by
    // only going through this loop at most three times
    for (int i = 0; work >= ONE_YEAR && i < 3; ++i)
    {
        work -= ONE_YEAR;
        year += 1;
    }

    *pYear = year;
    *pDayInYear = work;
}

void DateToYMD(int date, int* pYear, int* pMonth, int* pDay)
{
    int year;
    int month;
    int day;

    int daysInYear;

    DateToYear(date, &year, &daysInYear);

    // now get the month and the day

    bool isLeapYear = SPI_UTIL_IS_LEAP(year);

    // dividing by 32 will give us a lower bound for the month and is cheap
    month = (daysInYear >> 5) + 1;

    int *endOfMonth = isLeapYear ? endOfMonthLeapYear : endOfMonthNoLeapYear;

    endOfMonth += month;

    while (daysInYear > *endOfMonth)
    {
        ++month;
        ++endOfMonth;
    }

    SPI_UTIL_POST_CONDITION(daysInYear <= *endOfMonth);

    day = daysInYear - *(endOfMonth-1);

    if (pYear)
        *pYear  = year;
    if (pMonth)
        *pMonth = month;
    if (pDay)
        *pDay   = day;
}

int YMDToDate(int year, int month, int day)
{
    int  date = 0;
    bool isLeapYear = SPI_UTIL_IS_LEAP(year);

    SPI_UTIL_PRE_CONDITION(year >= 1601);
    SPI_UTIL_PRE_CONDITION(month >= 1 && month <= 12);
    SPI_UTIL_PRE_CONDITION(day >= 1 && day <= 31);

    int* daysInMonth = isLeapYear ? daysInMonthLeapYear : daysInMonthNoLeapYear;
    int* endOfMonth  = isLeapYear ? endOfMonthLeapYear : endOfMonthNoLeapYear;

    SPI_UTIL_PRE_CONDITION(day <= daysInMonth[month]);

    // rebase the year - 1601 is year 0
    year -= 1601;

    // now we can deal in blocks of 400 / 100 / 4 to handle the leap years
    while (year >= 400)
    {
        year -= 400;
        date += FOUR_CENTURIES;
    }
    while (year >= 100)
    {
        year -= 100;
        date += ONE_CENTURY;
    }
    if (year >= 4)
    {
        int numFourYears = year >> 2;
        year -= (numFourYears << 2);
        date += FOUR_YEARS * numFourYears;
    }

    SPI_UTIL_POST_CONDITION(year >= 0 && year < 4);

    while (year >= 1)
    {
        --year;
        date += ONE_YEAR;
    }

    date += endOfMonth[month-1] + day;

    return date;
}

int DaysInMonth(int year, int month)
{
    if (SPI_UTIL_IS_LEAP(year))
        return daysInMonthLeapYear[month];
    else
        return daysInMonthNoLeapYear[month];
}


int StringToDate(const std::string& str)
{
    std::vector<std::string> parts = StringSplit(str, '-');
    if (parts.size() == 3)
    {
        return YMDToDate(StringToInt(parts[0]),
                         StringToInt(parts[1]),
                         StringToInt(parts[2]));
    }

    try
    {
        int yyyymmdd = StringToInt(str);
        int year  = yyyymmdd / 10000;
        int month = (yyyymmdd - year * 10000) / 100;
        int day   = yyyymmdd % 100;
        return YMDToDate(year, month, day);
    }
    catch (std::exception&)
    {
        throw RuntimeError("Cannot convert '%s' to date - expecting YYYY-MM-DD or YYYYMMDD",
            str.c_str());
    }
}

std::string DateToString(int date)
{
    int year;
    int month;
    int day;

    DateToYMD(date, &year, &month, &day);

    return StringFormat("%04d-%02d-%02d", year, month, day);
}

const int SECONDS_PER_DAY = 24 * 60 * 60;

void TimeToHMS(int time, int * hours, int * minutes, int * seconds)
{
    SPI_UTIL_PRE_CONDITION(time >= 0 && time < SECONDS_PER_DAY);

    int h, m, s;

    s = time % 60;
    int hm = (time - s) / 60;
    m = hm % 60;
    h = (hm - m) / 60;

    SPI_UTIL_POST_CONDITION(s >= 0 && s < 60);
    SPI_UTIL_POST_CONDITION(m >= 0 && m < 60);
    SPI_UTIL_POST_CONDITION(h >= 0 && h < 24);

    if (hours)
        *hours = h;

    if (minutes)
        *minutes = m;

    if (seconds)
        *seconds = s;
}

int HMSToTime(int hours, int minutes, int seconds)
{
    SPI_UTIL_PRE_CONDITION(seconds >= 0 && seconds < 60);
    SPI_UTIL_PRE_CONDITION(minutes >= 0 && minutes < 60);
    SPI_UTIL_PRE_CONDITION(hours >= 0 && hours < 24);
    int time = seconds + 60 * (minutes + 60 * hours);

    SPI_UTIL_POST_CONDITION(time >= 0 && time < SECONDS_PER_DAY);

    return time;
}

int StringToTime(const std::string & str)
{
    try
    {
        std::vector<std::string> parts = StringSplit(str, ':');
        if (parts.size() == 2)
        {
            return HMSToTime(StringToInt(parts[0]),
                StringToInt(parts[1]),
                0);
        }
        else if (parts.size() == 3)
        {
            return HMSToTime(StringToInt(parts[0]),
                StringToInt(parts[1]),
                StringToInt(parts[2]));
        }
    }
    catch (std::exception& e)
    {
        SPI_UTIL_THROW_RUNTIME_ERROR("Cannot convert '" << str
            << "' to time - expecting HH:MM or HH:MM:SS. "
            << "ERROR: " << e.what());
    }
    SPI_UTIL_THROW_RUNTIME_ERROR("Cannot convert '" << str
        << "' to time - expecting HH:MM or HH:MM:SS");
}

std::string TimeToString(int time)
{
    int h, m, s;
    TimeToHMS(time, &h, &m, &s);

    if (s != 0)
        return StringFormat("%02d:%02d:%02d", h, m, s);

    return StringFormat("%02d:%02d", h, m);
}

int DayFractionToTime(double frac)
{
    SPI_UTIL_PRE_CONDITION(frac >= 0.0 && frac < 1.0);
    int result = (int)(frac * SECONDS_PER_DAY + 0.5);
    SPI_UTIL_POST_CONDITION(result >= 0 && result < SECONDS_PER_DAY);
    return result;
}

double TimeToDayFraction(int time)
{
    SPI_UTIL_PRE_CONDITION(time >= 0 && time < SECONDS_PER_DAY);
    return (double)time / SECONDS_PER_DAY;
}

SPI_UTIL_END_NAMESPACE


