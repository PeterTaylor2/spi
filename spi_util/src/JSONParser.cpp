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

#include "JSONParser.hpp"

#include "RuntimeError.hpp"
#include "StringUtil.hpp"
#include "JSONLexer.hpp"
#include "JSONValue.hpp"

#include <math.h>

#define BEGIN_ANONYMOUS_NAMESPACE namespace {
#define END_ANONYMOUS_NAMESPACE }

SPI_UTIL_NAMESPACE

BEGIN_ANONYMOUS_NAMESPACE

void JSONArrayToStream(
    std::ostream& ostr,
    const JSONArrayConstSP& anArray,
    bool noNewLine,
    size_t indent)
{
    const char* sep = noNewLine ? "" : "\n";
    size_t nextIndent = noNewLine ? indent : indent + 2;
    std::string spaces(nextIndent, ' ');

    size_t numItems = anArray->Size();

    ostr << "[";
    for (size_t i = 0; i < numItems; ++i)
    {
        ostr << sep << spaces;
        JSONValueToStream(ostr, anArray->Item(i), noNewLine, nextIndent);
        sep = noNewLine ? "," : ",\n";
    }
    if (noNewLine)
        ostr << spaces << "]";
    else
        ostr << " ]";
}

void JSONMapToStream(
    std::ostream& ostr,
    const JSONMapConstSP& aMap,
    bool noNewLine,
    size_t indent)
{
    const char* sep = noNewLine ? "" : "\n";
    size_t nextIndent = noNewLine ? indent : indent + 2;
    std::string spaces(nextIndent, ' ');

    const std::vector<std::string>& names = aMap->Names();
    size_t numItems = names.size();

    ostr << "{";
    for (size_t i = 0; i < numItems; ++i)
    {
        const std::string& name = names[i];
        const JSONValue& item = aMap->Item(name);

        ostr << sep << spaces;

        ostr << '"' << StringEscape(name.c_str()) << '"' << " : ";
        JSONValueToStream(ostr, item, noNewLine, nextIndent);

        sep = noNewLine ? "," : ",\n";
    }

    if (!noNewLine)
        ostr << "\n";

    ostr << std::string(indent,' ') << "}";
}


JSONValue JSONParseValue(JSONLexer& lexer);

JSONArrayConstSP JSONParseArray(JSONLexer& lexer)
{
    // "[" is assumed
    // rule is value followed by , or ]

    JSONArraySP anArray(new JSONArray());

    JSONLexer::Token token = lexer.getToken();
    bool started = false;

    while (token.type != ']')
    {
        if (started)
        {
            if (token.type != ',')
                throw RuntimeError("Expecting , to separate values in JSONArray");
            token = lexer.getToken();
        }
        else
        {
            started = true;
        }
        lexer.returnToken(token); // need to return the current token for JSONParseValue
        JSONValue value = JSONParseValue(lexer);
        anArray->Append(value);

        token = lexer.getToken();
    }

    return anArray;
}

JSONMapConstSP JSONParseMap(JSONLexer& lexer)
{
    // "{" is assumed
    // rule is string : value followed by , or }

    JSONMapSP aMap(new JSONMap());

    JSONLexer::Token token = lexer.getToken();
    bool started = false;

    while (token.type != '}')
    {
        if (started)
        {
            if (token.type != ',')
                throw RuntimeError("Expecting , to separate name/value "
                                   "pairs in JSONMap");
            token = lexer.getToken();
        }
        else
        {
            started = true;
        }
        if (token.type != SPI_JSON_TOKEN_TYPE_STRING)
            throw RuntimeError("Expecting name string in JSONMap");

        std::string name = token.value.aString;
        token = lexer.getToken();
        if (token.type != ':')
            throw RuntimeError("Expecting token : for field value");
        JSONValue value = JSONParseValue(lexer);
        aMap->Insert(name, value);

        token = lexer.getToken();
    }

    return aMap;
}

JSONValue JSONParseValue(JSONLexer& lexer)
{
    JSONLexer::Token token = lexer.getToken();

    switch(token.type)
    {
    case '{': // a map
        return JSONValue(JSONParseMap(lexer));
    case '[': // an array
        return JSONValue(JSONParseArray(lexer));
    case SPI_JSON_TOKEN_TYPE_STRING:
        return JSONValue(std::string(token.value.aString));
    case SPI_JSON_TOKEN_TYPE_BOOL:
        return JSONValue(token.value.aBool);
    case SPI_JSON_TOKEN_TYPE_DOUBLE:
        return JSONValue(token.value.aDouble);
    case SPI_JSON_TOKEN_TYPE_NULL:
        return JSONValue();
    default:
        throw std::runtime_error("bad token");
    }
}

END_ANONYMOUS_NAMESPACE

void JSONValueToStream(
    std::ostream& ostr,
    const JSONValue& value,
    bool noNewLine,
    size_t indent)
{
    switch(value.GetType())
    {
    case JSONValue::String:
        ostr << '"' << StringEscape(value.GetString().c_str()) << '"';
        break;
    case JSONValue::Array:
        JSONArrayToStream(ostr, value.GetArray(), noNewLine, indent);
        break;
    case JSONValue::Map:
        JSONMapToStream(ostr, value.GetMap(), noNewLine, indent);
        break;
    case JSONValue::Number:
    {
        double d = value.GetNumber();
        if (isnan(d))
        {
            ostr << "NaN";
        }
        else if (isinf(d))
        {
            if (d < 0.0)
                ostr << "-";
            ostr << "Infinity";
        }
        else
        {
            ostr << StringFormat("%.15g", value.GetNumber());
        }
        break;
    }
    case JSONValue::Bool:
        if (value.GetBool())
            ostr << "true";
        else
            ostr << "false";
        break;
    case JSONValue::Null:
        ostr << "null";
        break;
    }
}

JSONValue JSONParseValue(
    std::istream& istr,
    const std::string& streamName)
{
    JSONLexer lexer(streamName.empty() ? "<<STREAM>>" : streamName, &istr);

    return JSONParseValue(lexer);
}

JSONValue JSONParseValue(
    const std::string& data,
    size_t offset,
    const std::string& streamName)
{
    // this might be an unfortunate inefficiency since we are trying to avoid
    // use of streams in parsing functions elsewhere
    std::istringstream istr(data);
    if (offset > 0)
        istr.seekg(offset, std::ios::beg);
    return JSONParseValue(istr, streamName);
}

JSONValue JSONValueFromString(
    const std::string& str)
{
    std::istringstream iss(str);
    return JSONParseValue(iss);
}

std::string JSONValueToString(const JSONValue& value)
{
    std::ostringstream oss;
    JSONValueToStream(oss, value, true, 1);
    return oss.str();
}


SPI_UTIL_END_NAMESPACE
