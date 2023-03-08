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
#ifndef SPI_DATE_UTIL_HPP
#define SPI_DATE_UTIL_HPP

/**
***************************************************************************
** DateUtil.hpp
***************************************************************************
** Low level functions for converting from YMD to an integer date.
**
** We use the Gregorian calendar with date=0 chosen to be 1601,1,1
**
** This is because 1601,1,1 is the start of the 400-year cycle
** and we don't want to have negative dates, and we won't be interested
** in dates any further back in history.
**
** We ignore the fact that the Gregorian calendar was initially introduced
** in 1582 in Italy, but the rest of the world didn't follow suit for
** centuries (in some cases).
***************************************************************************
*/

#include "DeclSpec.h"
#include "Namespace.hpp"

#include <string>

#define SPI_UTIL_IS_LEAP(y) ((y)%4 == 0) && (((y)%100 != 0) || ((y)%400 == 0))

SPI_UTIL_NAMESPACE

SPI_UTIL_IMPORT
void DateToYMD(int date, int* year, int* month, int* day);

SPI_UTIL_IMPORT
int YMDToDate(int year, int month, int day);

SPI_UTIL_IMPORT
int DaysInMonth(int year, int month);

SPI_UTIL_IMPORT
int StringToDate(const std::string& str);

SPI_UTIL_IMPORT
std::string DateToString(int date);

extern const int SECONDS_PER_DAY;

void TimeToHMS(int time, int* hours, int* minutes, int* seconds);

SPI_UTIL_IMPORT
int HMSToTime(int hours, int minutes, int seconds);

SPI_UTIL_IMPORT
int StringToTime(const std::string& str);
std::string TimeToString(int time);
int DayFractionToTime(double frac);
double TimeToDayFraction(int time);

// note that we mostly require time >= 0 and time < SECONDS_PER_DAY
// however DayFractionToTime can exceptionally return time = SECONDS_PER_DAY

SPI_UTIL_END_NAMESPACE

#endif
