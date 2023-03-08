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
#include "MapReader.hpp"

#include <spi_util/StringUtil.hpp>
#include <spi_util/Utils.hpp>
#include <spi_util/DateUtil.hpp>
#include "RuntimeError.hpp"
#include "Map.hpp"
#include "Service.hpp"

#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#undef SPI_UTIL_CLOCK_EVENTS
#include <spi_util/ClockUtil.hpp>
#include <spi_util/FileUtil.hpp>

#ifdef _MSC_VER
/* this disables the warning message about dividing by zero */
#pragma warning( disable : 4723 )
#endif

SPI_BEGIN_NAMESPACE

#define IS_NAME(token,type) (type == MapReader::SYMBOL)
#define IS_SEPARATOR(token, type, sep) (type == MapReader::SEPARATOR && \
    token[0] == sep)

// strtod is notoriously slow in one of the Microsoft implementations
//
// however we have found that the home written parser is not 100%
// accurate
//
// so we are going to have to trade accuracy for speed
//
// parsing objects is an important operation - so slowing it down
// could cause problems
#define USE_DG_STRTOD

#ifdef USE_DG_STRTOD
#include "dg_strtod.h"
#endif



namespace {

    struct NumericValue
    {
        enum Type
        {
            UNDEFINED,
            DOUBLE,
            DATE,
            INT,
            HMS,
            HM
        };

        Type type;
        union
        {
            double d;
            int dt;
            int i;
            int time;
        };

        NumericValue()
            :
            type(UNDEFINED)
        {}

        static NumericValue Double(double d)
        {
            NumericValue nv;
            nv.type = DOUBLE;
            nv.d = d;
            return nv;
        }
        static NumericValue Date(Date dt)
        {
            NumericValue nv;
            nv.type = DATE;
            nv.dt = dt;
            return nv;
        }
        static NumericValue Int(int i)
        {
            NumericValue nv;
            nv.type = INT;
            nv.i = i;
            return nv;
        }
        static NumericValue Time(Type type, int time)
        {
            NumericValue nv;
            if (type != HMS && type != HM)
                SPI_THROW_RUNTIME_ERROR("Type must be HMS or HM");
            nv.type = type;
            nv.time = time;
            return nv;
        }

        int getInt() const
        {
            if (type == INT)
                return i;

            SPI_THROW_RUNTIME_ERROR("Numeric value is not an integer");
        }
    };

    // if the token begins with '-' or a digit from '0' to '9' then
    // call this function - it will return a value which is either
    // an integer, a double or a date
    NumericValue NumericValueFromToken(const char* str)
    {
        SPI_UTIL_CLOCK_FUNCTION();

        char* ep;
        // note that strtol call is partially wasted if str is a double
        // however strtod is slower than strtol
        long value = strtol(str, &ep, 10);
        if (*ep == '\0')
        {
            int ival = (int)value;
            if ((long)ival == value)
                return NumericValue::Int(ival);
        }
        else if (*ep == '-') // might be a date of format yyyy-mm-dd
        {
            long year = value;
            long month = strtol(ep + 1, &ep, 10);
            if (*ep == '-')
            {
                long day = strtol(ep + 1, &ep, 10);
                if (*ep == '\0')
                {
                    int iyear = (int)year;
                    int imonth = (int)month;
                    int iday = (int)day;

                    if ((year == (long)iyear) &&
                        (month == (long)imonth) &&
                        (day == (long)iday))
                    {
                        return NumericValue::Date(Date(iyear, imonth, iday));
                    }
                }
            }
        }
        else if (*ep == ':') // might be a time of format HH:MM:SS or HH:MM
        {
            long hours = value;
            long minutes = strtol(ep + 1, &ep, 10);
            if (*ep == ':')
            {
                long seconds = strtol(ep + 1, &ep, 10);
                if (*ep == '\0')
                {
                    int ihours = (int)hours;
                    int iminutes = (int)minutes;
                    int iseconds = (int)seconds;

                    if ((hours == (long)ihours) &&
                        (minutes == (long)iminutes) &&
                        (seconds == (long)iseconds))
                    {
                        return NumericValue::Time(NumericValue::HMS, spi_util::HMSToTime(hours, minutes, seconds));
                    }
                }
            }
            else if (*ep == '\0')
            {
                int ihours = (int)hours;
                int iminutes = (int)minutes;

                if ((hours == (long)ihours) &&
                    (minutes == (long)iminutes))
                {
                    return NumericValue::Time(NumericValue::HM, spi_util::HMSToTime(hours, minutes, 0));
                }
            }
        }
        else if (*ep == '.' || *ep == 'e' || *ep == 'E')
        {
            SPI_UTIL_CLOCK_BLOCK("strtod");

#ifdef USE_DG_STRTOD
            double value = dg_strtod(str, &ep);
#else
            double value = strtod(str, &ep);
#endif
            if (*ep == '\0')
                return NumericValue::Double(value);
        }

        SPI_THROW_RUNTIME_ERROR("'" << str << "' is not a numeric value");
    }

    size_t GetDimension(const std::string& token, MapReader::TokenType type)
    {
        if (type == MapReader::SYMBOL)
        {
            try
            {
                int dim = NumericValueFromToken(token.c_str()).getInt();
                if (dim >= 0)
                    return (size_t) dim;
            }
            catch (...)
            {}
        }

        throw RuntimeError("'%s' is not a dimension (non-negative integer expected)",
            token.c_str());
    }

} // end of anonymous namespace

MapReader::MapReader(
    const std::string& filename,
    const char* contents)
    :
    m_filename(filename),
    m_contents(contents),
    m_ptr(contents),
    m_lineno(0),
    m_line(),
    m_returnedToken(),
    m_returnedType(NONE)
{
    SPI_PRE_CONDITION(contents);
}

void MapReader::returnToken(const std::string& token, TokenType type)
{
    if (m_returnedType != NONE)
        throw RuntimeError("Can only return one token");

    m_returnedType = type;
    m_returnedToken = token;
}

bool MapReader::read(std::string& token, TokenType& type)
{
    SPI_UTIL_CLOCK_FUNCTION();

    if (m_returnedType != NONE)
    {
        token = m_returnedToken;
        type  = m_returnedType;

        m_returnedToken.clear();
        m_returnedType = NONE;

        return true;
    }

    // skip white space
    skipWhiteSpace();
    while (*m_ptr == '#')
    {
        skipToEOL();
        skipWhiteSpace();
    }

    if (!*m_ptr)
    {
        type = NONE;
        return false; // no more tokens
    }

    const char* buf = m_ptr;
    const char* ptr = buf;

    const char* separators = "[]{},*="; // these are the separators in addition to spaces

    if (*ptr == '"')
    {
        type = STRING;

        // look for the end of the string
        // allow backslashes to escape out the next character
        bool escaped = false;
        bool nextEscaped = false;

        ++ptr;
        while ((*ptr != '"' || nextEscaped) && *ptr != '\0')
        {
            if (*ptr == '\\')
            {
                escaped = true;
                nextEscaped = !nextEscaped;
            }
            else
            {
                if (*ptr == '\n')
                    ++m_lineno;
                nextEscaped = false;
            }
            ++ptr;
        }

        // the above loop can end with *ptr == '\0' which is an error
        // or with *ptr = '"' which is good news

        if (!*ptr)
            throw RuntimeError("Unterminated string");

        SPI_POST_CONDITION(*ptr == '"');

        m_ptr = ptr + 1;
        token = std::string(buf+1, ptr-buf-1);
        if (escaped)
            token = spi_util::StringCapture(token.c_str());
    }
    else
    // if the buffer begins with "'" then we have a char
    // we only allow a single char followed by "'"
    // anything else is an error - and we don't escape out
    if (*ptr == '\'')
    {
        type = CHAR;

        ++ptr;
        if (!*ptr)
            throw RuntimeError("Unterminated character");

        if (*ptr == '\n')
            ++m_lineno;

        token = std::string(1, *ptr);

        ++ptr;
        if (!*ptr)
            throw RuntimeError("Unterminated character");

        if (*ptr != '\'')
            throw RuntimeError("Expected \"'\" - actual was \"%c\"", *ptr);

        m_ptr = ptr + 1;
    }
    else
    if (strchr(separators, *ptr) != NULL)
    {
        // indicates that we have started with a separator
        token = std::string(1, *ptr);
        type = SEPARATOR;

        m_ptr = ptr + 1;
    }
    else
    {
        ++ptr;
        while (*ptr && !isspace(*ptr) && strchr(separators, *ptr) == NULL)
            ++ptr;

        m_ptr = ptr;
        token = std::string(buf, ptr-buf);
        type = SYMBOL;
    }
    return true;
}

int MapReader::lineno() const
{
    return m_lineno;
}

const std::string& MapReader::filename() const
{
    return m_filename;
}

void MapReader::skipWhiteSpace()
{
    while(*m_ptr && isspace(*m_ptr))
    {
        if (*m_ptr == '\n')
            ++m_lineno;

        ++m_ptr;
    }
}

void MapReader::skipToEOL()
{
    while(*m_ptr && *m_ptr != '\n')
        ++m_ptr;

    if (*m_ptr)
    {
        SPI_POST_CONDITION(*m_ptr == '\n');
        ++m_lineno;
        ++m_ptr;
    }
}

namespace
{
    // we have already constructed the map - now populate it from { until }
    void PopulateMap(MapReader* reader, Map* m, int format)
    {
        SPI_UTIL_CLOCK_FUNCTION();

        std::string token;
        MapReader::TokenType type;

        reader->read(token, type);
        if (!IS_SEPARATOR(token, type, '{'))
        {
            throw RuntimeError("Expecting '{' to begin map definition");
        }

        // now keep going until we hit the end of the map
        // we expect NAME = value until terminated by '}'
        reader->read(token, type);
        while (!IS_SEPARATOR(token, type, '}'))
        {
            if (!IS_NAME(token, type))
            {
                throw RuntimeError("Expecting name within map definition");
            }
            std::string fieldName = token;

            reader->read(token, type);
            if (!IS_SEPARATOR(token, type, '='))
            {
                throw RuntimeError("Expecting '=' to define value");
            }

            Value value = ReadValue(reader, format);
            m->SetValue(fieldName, value);

            reader->read(token, type);
        }
    }

    MapSP BeginClassMap(MapReader* reader, const std::string& className)
    {
        SPI_UTIL_CLOCK_FUNCTION();

        std::string token;
        MapReader::TokenType type;

        // expecting either an integer or {
        reader->read(token, type);
        int ref = 0;
        if (type == MapReader::SYMBOL)
        {
            try
            {
                ref = spi_util::StringToInt(token);
            }
            catch (...)
            {
                throw RuntimeError(
                    "Expect reference number or '{' after className \"%s\"",
                    className.c_str());
            }
        }
        else
        {
            reader->returnToken(token, type);
        }
        return MapSP(new Map(className.c_str(), ref));
    }
}

MapSP ReadMap(MapReader* reader, int format)
{
    SPI_UTIL_CLOCK_FUNCTION();

    MapSP m;

    std::string token;
    MapReader::TokenType type;

    if (!reader->read(token, type))
        return m;

    // we expect a map to begin either with a name or {
    // we will simply insist that a name begins with a letter or _

    if (IS_NAME(token, type))
    {
        m = BeginClassMap(reader, token);
    }
    else
    {
        m.reset(new Map(""));
        // PopulateMap is expecting the initial {
        reader->returnToken(token, type);
    }

    PopulateMap(reader, m.get(), format);
    return m;
}

ValueArraySP ReadArray(MapReader* reader, int format)
{
    SPI_UTIL_CLOCK_FUNCTION();

    // we have already parsed the open '['
    if (format == 2)
    {
        // format is [dimensions] {values}

        std::vector<size_t> dimensions;

        std::string token;
        MapReader::TokenType type;

        reader->read(token, type);
        size_t dim = GetDimension(token, type);
        size_t size = dim;
        dimensions.push_back(dim);

        reader->read(token, type);
        while(IS_SEPARATOR(token, type, ','))
        {
            reader->read(token, type);
            dim = GetDimension(token, type);
            dimensions.push_back(dim);
            size *= dim;
            reader->read(token, type);
        }

        if (!IS_SEPARATOR(token, type, ']'))
            throw RuntimeError("Expecting ']' to end dimensions of array");

        // now we have an array of known size which is expected to be in {..}
        // with the array split into blocks of size dimensions[0]
        //
        // if the array is empty then we don't expect anything at all

        std::vector<Value> output;
        output.reserve(size);

        if (size > 0)
        {
            size_t count = 0;
            size_t blockSize = dimensions.back();
            SPI_POST_CONDITION(blockSize > 0);
            while (count < size)
            {
                reader->read(token, type);
                if (!IS_SEPARATOR(token, type, '{'))
                    throw RuntimeError("Expecting '{' to start the array");

                for (size_t i = 0; i < blockSize; ++i)
                {
                    const Value& value = ReadValue(reader, format);
                    output.push_back(value);
                }
                reader->read(token, type);
                if (!IS_SEPARATOR(token, type, '}'))
                    throw RuntimeError("Expecting '}' to end the array");

                count += blockSize;
            }
            SPI_POST_CONDITION(count == size);
        }
        return new ValueArray(output, dimensions);
    }
    else if (format == 1)
    {
        // subsequently we have Values separated by white space until ']'

        //ValueArraySP output (new ValueArray());
        std::vector<Value> output;

        std::string token;
        MapReader::TokenType type;

        reader->read(token, type);
        while (!IS_SEPARATOR(token, type, ']'))
        {
            if (type == MapReader::NONE)
                throw RuntimeError("Unterminated array");
            reader->returnToken(token, type);
            const Value& value = ReadValue(reader, format);
            output.push_back(value);
            reader->read(token, type);
        }

        return new ValueArray(output, output.size());
    }
    else
    {
        throw RuntimeError("%s: Unknown format %d", __FUNCTION__, format);
    }
}

namespace {
    Value ReadReference(MapReader* reader)
    {
        SPI_UTIL_CLOCK_FUNCTION();

        std::string token;
        MapReader::TokenType type;

        reader->read(token, type);
        if (type == MapReader::SYMBOL)
        {
            try
            {
                int ref = spi_util::StringToInt(token);
                return Value(ObjectRef(ref));
            }
            catch (...)
            {
                // ignore in favour of generic error below
            }
        }
        throw RuntimeError("Expecting integer after '*'");
    }

    Value ValueFromSymbol(const char* symbol, MapReader* reader, int format)
    {
        SPI_UTIL_CLOCK_FUNCTION();

        // symbols can be INT, DATE, DOUBLE, BOOL, NAME, NULL
        //
        // these are the patterns that were supported by the lexer:
        // integer     -?[0-9]+
        // date        [0-9]+\-[0-9]+\-[0-9]+
        // double      -?((([0-9]+)|([0-9]*\.[0-9]+))([eE][-+]?[0-9]+)?)
        // name        [a-zA-Z_][a-zA-Z0-9_\.]*
        // NAN, INF, NULL, true, false
        //
        // note:
        // 1. only a double has '.'
        // 2. only a date has interior minus signs
        // 3. doubles and integers can start with a minus sign
        try
        {
            switch(symbol[0])
            {
            case '-':
            case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9': case '0':
            {
                NumericValue nv = NumericValueFromToken(symbol);
                switch (nv.type)
                {
                case NumericValue::INT:
                    return Value(nv.i);
                case NumericValue::DOUBLE:
                    return Value(nv.d);
                case NumericValue::DATE:
                {
                    std::string token;
                    MapReader::TokenType type;

                    // look ahead to see if the next token is a time
                    // in that case we have a DateTime instead of a Date
                    reader->read(token, type);
                    if (type == MapReader::SYMBOL
                        && (token[0] == '0' || token[0] == '1' || token[0] == '2'))
                    {
                        // minimal lookahead to see if we have ":" in the next symbol
                        long hours;
                        char* ep;
                        hours = strtol(token.c_str(), &ep, 10);
                        if (*ep == ':')
                        {
                            NumericValue nvt = NumericValueFromToken(token.c_str());
                            if (nvt.type == NumericValue::HMS)
                            {
                                return Value(DateTime(nv.dt, nvt.time));
                            }
                        }
                    }
                    reader->returnToken(token, type);
                    return Value(Date(nv.dt));
                }
                case NumericValue::HM:
                    // we allow Time without Date if there are no seconds
                    // it is designed to suggest a regular time of day for particular events
                    // e.g. opening and closing times
                    return Value(DateTime(Date(), nv.time));
                case NumericValue::HMS:
                case NumericValue::UNDEFINED:
                    break;
                }
                break;
            }
            case 'N':
                if (strcmp(symbol, "NAN") == 0)
                {
                    double i = sqrt(-1.0);
                    return Value(i);
                }
                if (strcmp(symbol, "NULL") == 0)
                {
                    return Value(MapConstSP());
                }
                if (strcmp(symbol, "None") == 0)
                {
                    return Value();
                }
                break;
            case 'I':
                if (strcmp(symbol, "INF") == 0)
                {
                    double one  = 1.0;
                    double zero = 0.0;
                    double infinity = one / zero;

                    return Value(infinity);
                }
                break;
            case 't':
                if (strcmp(symbol, "true") == 0)
                {
                    return Value(true);
                }
                break;
            case 'f':
                if (strcmp(symbol, "false") == 0)
                {
                    return Value(false);
                }
                break;
            case 'E':
                if (strcmp(symbol, "ERROR") == 0)
                {
                    std::string nextToken;
                    MapReader::TokenType nextType;
                    reader->read(nextToken, nextType);
                    if (nextType == MapReader::STRING)
                    {
                        return Value(nextToken.c_str(), true /* isError */);
                    }
                    else
                    {
                        reader->returnToken(nextToken, nextType);
                    }
                }
                break;
            }
            // anything else can only be a name and hence the start of a map
            if (IS_NAME(symbol, MapReader::SYMBOL))
            {
                std::string nextToken;
                MapReader::TokenType nextType;
                reader->read(nextToken, nextType);
                if (IS_SEPARATOR(nextToken, nextType, '*'))
                {
                    return ReadReference(reader); // no type checking!
                }

                reader->returnToken(nextToken, nextType);
                MapSP m = BeginClassMap(reader, symbol);
                PopulateMap(reader, m.get(), format);
                return Value(MapConstSP(m));
            }
        }
        catch(std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("Error for symbol '" << symbol << "':\n\t" << e.what());
        }
        SPI_THROW_RUNTIME_ERROR("Bad symbol '" << symbol << "'");
    }
} // end of anonymous namespace

Value ReadValue(MapReader* reader, int format)
{
    SPI_UTIL_CLOCK_FUNCTION();

    std::string token;
    MapReader::TokenType type;

    reader->read(token, type);
    switch (type)
    {
    case MapReader::NONE:
        throw RuntimeError("Unexpected end of file");
    case MapReader::STRING:
        return Value(token);
    case MapReader::CHAR:
        return Value(token[0]);
    case MapReader::SEPARATOR:
        switch(token[0])
        {
        case '{':
            reader->returnToken(token, type);
            return Value(MapConstSP(ReadMap(reader, format)));
        case '[':
            return Value(IArrayConstSP(ReadArray(reader, format)));
        case '*':
            return ReadReference(reader);
        }
        throw RuntimeError("Unexpected separator '%c'", token[0]);
    case MapReader::SYMBOL:
        return ValueFromSymbol(token.c_str(), reader, format);
    }
    throw RuntimeError("Unexpected token type %d", (int)type);
}

SPI_END_NAMESPACE
