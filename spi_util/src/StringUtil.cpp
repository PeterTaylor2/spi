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
#include "StringUtil.hpp"
#include "Utils.hpp"

#include <spi_boost/shared_array.hpp>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#undef SPI_UTIL_CLOCK_EVENTS
#include "ClockUtil.hpp"

SPI_UTIL_NAMESPACE

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

/** Function for replacing values in the input string with values from
    the provided map of values. */
std::string StringMapFormat(
    const char* format,
    const std::map<std::string, std::string>& values,
    char delim)
{
    std::vector<std::string> parts = CStringSplit(format, delim);
    std::ostringstream oss;

    if (parts.size() % 2 != 1)
    {
        throw RuntimeError(
            "Expecting an even number of delimiters '%c' in \"%s\"",
            delim, format);
    }

    oss << parts[0];

    for (size_t i = 1; i < parts.size(); i+=2)
    {
        const std::string& part = parts[i];
        if (part == "")
            oss << '%';
        else
        {
            std::map<std::string, std::string>::const_iterator iter =
                values.find(part);

            if (iter == values.end())
                throw RuntimeError("No value defined for \"%s\"",
                                   part.c_str());

            oss << iter->second;
        }
        oss << parts[i+1];
    }

    return oss.str();
}

/**
 * Splits a string into its constituent components using a number of
 * separators.
 *
 * Whether the separators are mandatory or not is a parameter. If the
 * separators are not mandatory, then if a separator is missing then
 * all remaining components are returned as empty.
 */
std::vector<std::string> StringParser(
    const std::string& str,
    const std::string& separators,
    bool optional)
{
    return CStringParser(str.c_str(), separators.c_str(), optional);
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
 * Splits a string at the given separator and return a vector of
 * strings. Note that if the string is empty, you will get a vector
 * of size 1.
 */
std::vector<std::string> StringSplit(
    const std::string& in,
    const char* separator)
{
    return CStringSplit(in.c_str(), separator);
}

/**
 * Replaces all instances of one string within a string within a second string.
 */
std::string StringReplace(
    const std::string& in,
    const char* oldString,
    const char* newString)
{
    return StringJoin(newString, StringSplit(in, oldString));
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
    const std::vector<std::string>& strings,
    size_t startIndex)
{
    std::ostringstream oss;
    for (size_t i = startIndex; i < strings.size(); ++i)
    {
        if (i > startIndex)
            oss << separator;

        oss << strings[i];
    }
    return oss.str();
}



/**
 * Converts a string to upper case.
 */
std::string StringUpper(const std::string& in)
{
    return CStringUpper(in.c_str());
}

/**
 * Converts a string to lower case.
 */
std::string StringLower(const std::string& in)
{
    return CStringLower(in.c_str());
}

/**
 * Converts the first character of a string to upper case.
 */
std::string StringCap(const std::string& in)
{
    return CStringCap(in.c_str());
}

/**
 * Converts a string to long.
 */
long StringToLong (const std::string& in)
{
    return CStringToLong(in.c_str());
}

/**
 * Converts a string to int.
 */
int StringToInt (const std::string& in)
{
    return CStringToInt(in.c_str());
}

/**
 * Converts a string to double.
 */
double StringToDouble (const std::string& in)
{
    return CStringToDouble(in.c_str());
}

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
    bool optional)
{
    SPI_UTIL_PRE_CONDITION(str != NULL);
    SPI_UTIL_PRE_CONDITION(separators != NULL);

    size_t nbSeparators = strlen(separators);
    std::vector<std::string> output(nbSeparators+1);
    const char* remainder = str;

    for (size_t i = 0; i < nbSeparators; ++i)
    {
        if (remainder)
        {
            char sep = separators[i];
            const char* pos = strchr(remainder, sep);
            if (pos)
            {
                output[i] = std::string(remainder, pos-remainder);
                remainder = pos + 1;
            }
            else
            {
                if (!optional)
                {
                    throw RuntimeError("Could not find separator %c in %s",
                                       sep, remainder);
                }
                output[i] = std::string(remainder);
                remainder = 0;
            }
        }
    }
    if (remainder)
    {
        output[nbSeparators] = std::string(remainder);
    }

    return output;
}

/**
 * Splits a string at the given separator and return a vector of
 * strings. Note that if the string is empty, you will get a vector
 * of size 1.
 */
std::vector<std::string> CStringSplit(
    const char* in,
    char separator)
{
    SPI_UTIL_PRE_CONDITION(in != NULL);

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
 * Splits a string at the given separator and return a vector of
 * strings. Note that if the string is empty, you will get a vector
 * of size 1.
 */
std::vector<std::string> CStringSplit(
    const char* in,
    const char* separator)
{
    SPI_UTIL_PRE_CONDITION(in != NULL);
    SPI_UTIL_PRE_CONDITION(separator != NULL);

    const char* remainder = in;
    const char* pos;

    size_t lenSep = strlen(separator);

    std::vector<std::string> out;

    while ((pos = strstr(remainder, separator)) != NULL)
    {
        out.push_back(std::string(remainder, pos-remainder));
        remainder = pos+lenSep;
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
    SPI_UTIL_PRE_CONDITION(in != NULL);

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

/**
 * Copies from the input string to a buffer removing leading whitespace
 * and converting to upper case.
 *
 * No memory allocation is performed and char* out must be of size outlen+1
 * or greater.
 */
void CStringPreProcess(
    const char* in,
    size_t      outlen,
    char*       out)
{
    SPI_UTIL_PRE_CONDITION(in != NULL);
    SPI_UTIL_PRE_CONDITION(outlen > 0);
    SPI_UTIL_PRE_CONDITION(out != NULL);

    memset(out, 0, outlen+1);

    while( *in && isspace(*in) )
        ++in;

    strncpy(out, in, outlen);
    outlen = strlen(out);

    for (size_t i = 0; i < outlen; ++i)
        out[i] = (char)toupper(out[i]);
}


/**
 * Converts a c-string to upper case.
 */
std::string CStringUpper(const char* in)
{
    SPI_UTIL_CLOCK_FUNCTION();

    SPI_UTIL_PRE_CONDITION(in != NULL);

    std::string out(in);
    for (size_t i = 0; i < out.length(); ++i)
        out[i] = toupper(out[i]);
    return out;
}

/**
 * Converts a c-string to lower case.
 */
std::string CStringLower(const char* in)
{
    SPI_UTIL_CLOCK_FUNCTION();

    SPI_UTIL_PRE_CONDITION(in != NULL);

    std::string out(in);
    for (size_t i = 0; i < out.length(); ++i)
        out[i] = tolower(out[i]);
    return out;
}

/**
 * Converts first character of a c-string to upper case.
 */
std::string CStringCap(const char* in)
{
    SPI_UTIL_PRE_CONDITION(in != NULL);

    std::string out(in);
    out[0] = toupper(out[0]);

    return out;
}


/**
 * Converts a c-string to long.
 */
long CStringToLong (const char* in)
{
    SPI_UTIL_PRE_CONDITION(in != NULL);

    char *ep;
    long  out = strtol(in, &ep, 10);

    if (*ep)
        throw RuntimeError("Could not convert %s to integer", in);

    return out;
}

/**
 * Converts a c-string to int.
 */
int CStringToInt (const char* in)
{
    long out = CStringToLong(in);
    return IntegerCast<int>(out);
}

/**
 * Converts a c-string to double.
 */
double CStringToDouble (const char* in)
{
    SPI_UTIL_PRE_CONDITION(in != NULL);

    char  *ep;
    double out = strtod(in, &ep);

    if (*ep)
        throw RuntimeError("Could not convert %s to double", in);

    return out;
}

std::string StringEscape(const char* str)
{
    // this is the sort of routine that could be heavily optimised
    // by writing in assembler or using some clever system function
    SPI_UTIL_PRE_CONDITION(str != NULL);

    size_t len = strlen(str);
    size_t buflen = 2*len;

    char* buf = new char[buflen+1];
    spi_boost::shared_array<char> spbuf(buf); // memory management

    size_t pos = 0; // insertion position within buf

    for (size_t i = 0; i < len; ++i)
    {
        char c = str[i];
        char escape = '\0';
        switch (c)
        {
        case '"':
        case '\\':
            escape = c;
            break;
        case '\n':
            escape = 'n';
            break;
        case '\t':
            escape = 't';
            break;
        default:
            if((int)(unsigned char)c < 32)
            {
                throw RuntimeError("Cannot escape char(%d)\n", (int)c);
            }
            break;
        }
        if (escape)
        {
            buf[pos] = '\\';
            buf[pos+1] = escape;
            pos += 2;
        }
        else
        {
            buf[pos] = c;
            pos += 1;
        }
    }
    SPI_UTIL_POST_CONDITION(pos <= buflen);
    buf[pos] = '\0';

    return std::string(buf);
}

std::string StringCapture(const char* str)
{
    // this is the sort of routine that could be heavily optimised
    // by writing in assembler or using some clever system function
    SPI_UTIL_PRE_CONDITION(str != NULL);

    size_t len = strlen(str);

    char* buf = new char[len+1];
    spi_boost::shared_array<char> spbuf(buf); // memory management

    size_t pos = 0; // current position within the input str
    size_t ins = 0; // insert position within the output buf
    const char* pc;

    while (pc = strchr(str+pos, '\\'), pc != NULL)
    {
        char nc = *(pc+1);

        switch (nc)
        {
        case 'n':
            nc = '\n';
            break;
        case 't':
            nc = '\t';
            break;
        case '"':
        case '\\':
            break; // nc unchanged
        default:
            throw RuntimeError("Invalid escape sequence \\%c", nc);
        }

        size_t copy = pc - (str + pos);
        if (copy > 0)
        {
            strncpy(buf+ins, str+pos, copy);
            ins += copy;
            pos += copy;
        }

        buf[ins] = nc;
        ins += 1;
        pos += 2;
    }
    strcpy(buf+ins, str+pos);
    SPI_UTIL_POST_CONDITION(strlen(buf) <= len);

    return std::string(buf);
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



SPI_UTIL_END_NAMESPACE
