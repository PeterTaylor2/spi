/*

    GENDEP.EXE - Generates header file dependencies for C++
    Copyright (C) 2013 Sartorial Programming Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef SPI_STRING_UTIL_HPP
#define SPI_STRING_UTIL_HPP

/**
***************************************************************************
** StringUtil.hpp
***************************************************************************
** String utility functions.
***************************************************************************
*/

#include <vector>
#include <string>
#include <stdarg.h>

/** Function for formatting values in vprintf style */
std::string StringFormatV(const char* format, va_list args);

/** Function for formatting values in printf style */
std::string StringFormat(const char* format, ...);

/**
 * Splits a string at the given separator and return a vector of
 * strings. Note that if the string is empty, you will get a vector
 * of size 1.
 */
std::vector<std::string> StringSplit(
    const std::string& in,
    char separator);

/**
 * Strips a string of leading and trailing white space characters.
 */
std::string StringStrip(
    const std::string& in,
    bool stripLeft = true,
    bool stripRight = true);

/**
 * Joins a vector of strings using the given separator.
 */
std::string StringJoin(
    const std::string& separator,
    const std::vector<std::string>& strings);

/**
 * Tests whether a string starts with another string.
 */
bool StringStartsWith(const std::string& str, const std::string& other);

/**
 * Tests whether a string ends with another string.
 */
bool StringEndsWith(const std::string& str, const std::string& other);

#endif


