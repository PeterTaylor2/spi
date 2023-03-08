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

/**
***************************************************************************
** StringUtil.cpp
***************************************************************************
** String utility functions.
***************************************************************************
*/

#include "StringUtil.hpp"
#include "RuntimeError.hpp"

#include <sstream>

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace
{
/**
 * Splits a string at the given separator and return a vector of
 * strings. Note that if the string is empty, you will get a vector
 * of size 1.
 */
std::vector<std::string> CStringSplit(
    const char* in,
    char separator)
{
    PRE_CONDITION(in != NULL);

    const char* remainder = in;
    const char* pos;

    std::vector<std::string> out;

    while ((pos = strchr(remainder, separator)) != NULL)
    {
        out.push_back(std::string(remainder, pos-remainder));
        remainder = pos+1;
    }
    out.push_back(std::string(remainder));

    return out;
}

/**
 * Strips a string of leading and trailing white space characters.
 */
std::string CStringStrip(
    const char* in,
    bool stripLeft,
    bool stripRight)
{
    PRE_CONDITION(in != NULL);

    const char* begin = in;
    const char* end = in + strlen(in) - 1;

    if (stripLeft)
    {
        while (*begin && isspace(*begin))
            ++begin;
    }

    if (stripRight)
    {
        while (end >= begin && isspace(*end))
            --end;
    }

    if (begin > end)
        return std::string();

    return std::string(begin, end+1);
}

} // end of anonymous namespace

std::string StringFormat(const char* format, ...)
{
    va_list args;
    va_start (args, format);

    std::string result = StringFormatV(format, args);

    va_end (args);
    return result;
}

std::string StringFormatV(const char* format, va_list args)
{
    char buf[256];
    int  size;

    std::string result;

    size = vsnprintf (buf, sizeof(buf), format, args);

    // allegedly vsnprintf returns the required size if there is an
    // overflow, but on some systems it returns -1 instead
    //
    // in that case we go to a bigger size and hope for the best
    if (size == -1)
    {
        char *tmp = (char*)malloc(8192);
        vsprintf (tmp, format, args);
        result = std::string(tmp);
        free(tmp);
    }
    else
    {
        if (size >= (int)(sizeof(buf)))
        {
            char *tmp = (char*)malloc(size+1);
            vsprintf (tmp, format, args);
            result = std::string(tmp);
            free(tmp);
        }
        else
        {
            result = std::string(buf);
        }
    }
    return result;
}

/**
 * Splits a string at the given separator and return a vector of
 * strings. Note that if the string is empty, you will get a vector
 * of size 1.
 */
std::vector<std::string> StringSplit(
    const std::string& in,
    char separator)
{
    return CStringSplit(in.c_str(), separator);
}

/**
 * Strips a string of leading and trailing white space characters.
 */
std::string StringStrip(
    const std::string& in,
    bool stripLeft,
    bool stripRight)
{
    return CStringStrip(in.c_str(), stripLeft, stripRight);
}

/**
 * Joins a vector of strings using the given separator.
 */
std::string StringJoin(
    const std::string& separator,
    const std::vector<std::string>& strings)
{
    std::ostringstream oss;
    for (size_t i = 0; i < strings.size(); ++i)
    {
        if (i > 0)
            oss << separator;

        oss << strings[i];
    }
    return oss.str();
}

/**
 * Tests whether a string starts with another string.
 */
bool StringStartsWith(const std::string& str, const std::string& other)
{
    if (other.length() > str.length())
        return false;

    std::string substr = str.substr(0, other.length());

    return substr == other;
}

/**
 * Tests whether a string ends with another string.
 */
bool StringEndsWith(const std::string& str, const std::string& other)
{
    if (other.length() > str.length())
        return false;

    std::string substr = str.substr(str.length() - other.length(),
                                    other.length());

    return substr == other;
}
