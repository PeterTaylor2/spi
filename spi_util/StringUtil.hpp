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
** StringUtil.hpp
***************************************************************************
** String utility functions.
***************************************************************************
*/

#ifndef SPI_UTIL_STRING_UTIL_HPP
#define SPI_UTIL_STRING_UTIL_HPP

#include "DeclSpec.h"
#include "Namespace.hpp"

#include <string>
#include <map>
#include <vector>
#include <stdarg.h>

SPI_UTIL_NAMESPACE

/** Function for formatting values in vprintf style */
SPI_UTIL_IMPORT
std::string StringFormatV(const char* format, va_list args);

/** Function for formatting values in printf style */
SPI_UTIL_IMPORT
std::string StringFormat(const char* format, ...);

/** Function for replacing values in the input string with values from
    the provided map of values. */
SPI_UTIL_IMPORT
std::string StringMapFormat(
    const char* format,
    const std::map<std::string, std::string>& values,
    char delim='%');

/**
 * Splits a string into its constituent components using a number of
 * separators.
 *
 * Whether the separators are mandatory or not is a parameter. If the
 * separators are not mandatory, then if a separator is missing then
 * all remaining components are returned as empty.
 */
SPI_UTIL_IMPORT
std::vector<std::string> StringParser(
    const std::string& str,
    const std::string& separators,
    bool optional=false);

/**
 * Splits a string at the given separator and return a vector of
 * strings. Note that if the string is empty, you will get a vector
 * of size 1.
 */
SPI_UTIL_IMPORT
std::vector<std::string> StringSplit(
    const std::string& in,
    char separator);

/**
 * Splits a string at the given separator and return a vector of
 * strings. Note that if the string is empty, you will get a vector
 * of size 1.
 */
SPI_UTIL_IMPORT
std::vector<std::string> StringSplit(
    const std::string& in,
    const char* separator);

/**
 * Replaces all instances of one string within a string within a second string.
 */
SPI_UTIL_IMPORT
std::string StringReplace(
    const std::string& in,
    const char* oldString,
    const char* newString);

/**
 * Strips a string of leading and trailing white space characters.
 */
SPI_UTIL_IMPORT
std::string StringStrip(
    const std::string& in,
    bool stripLeft = true,
    bool stripRight = true);

/**
 * Joins a vector of strings using the given separator.
 */
SPI_UTIL_IMPORT
std::string StringJoin(
    const std::string& separator,
    const std::vector<std::string>& strings,
    size_t startIndex=0);

/**
 * Converts a string to upper case.
 */
SPI_UTIL_IMPORT
std::string StringUpper(const std::string& in);

/**
 * Converts a string to lower case.
 */
SPI_UTIL_IMPORT
std::string StringLower(const std::string& in);

/**
 * Removes unprintable characters from a string.
 */
SPI_UTIL_IMPORT
std::string StringClean(const std::string& in, char replacement);

/**
 * Converts a string so that its first character is in upper case and
 * the rest is unchanged.
 */
std::string StringCap(const std::string& in);


/**
 * Converts a string to long - throwing an exception if str is not valid.
 */
SPI_UTIL_IMPORT
long StringToLong (const std::string& str);

/**
 * Converts a string to int - throwing an exception if str is not valid.
 */
SPI_UTIL_IMPORT
int StringToInt (const std::string& str);

/**
 * Converts a string to double - throwing an exception if str is not valid.
 */
SPI_UTIL_IMPORT
double StringToDouble (const std::string& str);

// c-string equivalent of some of the above functions (where appropriate)
// the inputs are c-strings, but the outputs will sometimes be std::string

/**
 * Splits a string into its constituent components using a number of
 * separators.
 *
 * Whether the separators are mandatory or not is a parameter. If the
 * separators are not mandatory, then if a separator is missing then
 * all remaining components are returned as empty.
 */
std::vector<std::string> CStringParser(
    const char* str,
    const char* separators,
    bool optional=false);


/**
 * Splits a string at the given separator and return a vector of
 * strings. Note that if the string is empty, you will get a vector
 * of size 1.
 */
SPI_UTIL_IMPORT
std::vector<std::string> CStringSplit(
    const char* in,
    char separator);

/**
 * Splits a string at the given separator and return a vector of
 * strings. Note that if the string is empty, you will get a vector
 * of size 1.
 */
SPI_UTIL_IMPORT
std::vector<std::string> CStringSplit(
    const char* in,
    const char* separator);

/**
 * Strips a string of leading and trailing white space characters.
 */
SPI_UTIL_IMPORT
std::string CStringStrip(
    const char* in,
    bool stripLeft = true,
    bool stripRight = true);

/**
 * Copies from the input string to a buffer removing leading whitespace
 * and converting to upper case.
 *
 * No memory allocation is performed and char* out must be of size outlen+1
 * or greater.
 */
SPI_UTIL_IMPORT
void CStringPreProcess(
    const char* in,
    size_t      outlen,
    char*       out);

/**
 * Converts a c-string to upper case.
 */
SPI_UTIL_IMPORT
std::string CStringUpper(const char* in);

/**
 * Converts a c-string to lower case.
 */
std::string CStringLower(const char* in);

/**
 * Capitalises the first character of a c-string.
 */
std::string CStringCap(const char* in);

/**
 * Converts a c-string to long.
 */
long CStringToLong (const char* in);

/**
 * Converts a c-string to int.
 */
int CStringToInt (const char* in);

/**
 * Converts a c-string to double.
 */
double CStringToDouble (const char* in);

/**
 * When preparing a string for output to file this function will escape out
 * unprintable characters. In addition any quotation marks will be escaped
 * out.
 */
SPI_UTIL_IMPORT
std::string StringEscape(const char* str);

/**
 * When reading a string from file this function will replace escape
 * sequences with the corresponding character. Hence it is the reverse
 * function for StringEscape - hence the name.
 */
SPI_UTIL_IMPORT
std::string StringCapture(const char* str);

/**
 * Tests whether a string starts with another string.
 */
SPI_UTIL_IMPORT
bool StringStartsWith(const std::string& str, const std::string& other);

/**
 * Tests whether a string ends with another string.
 */
SPI_UTIL_IMPORT
bool StringEndsWith(const std::string& str, const std::string& other);

SPI_UTIL_END_NAMESPACE

#endif


