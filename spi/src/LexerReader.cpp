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
#include "LexerReader.hpp"

#include <spi_util/Lexer.hpp>
#include "RuntimeError.hpp"
#include "Map.hpp"
#include "Service.hpp"
#include <float.h>
#include <string.h>

SPI_BEGIN_NAMESPACE

using spi_util::Lexer;

MapSP ReadMap(Lexer* lexer)
{
    MapSP m;

    Lexer::Token token = lexer->getToken();
    if (token.type == SPI_TOKEN_TYPE_NULL)
    {
        return m;
    }

    if (token.type == SPI_TOKEN_TYPE_NAME)
    {
        std::string className(token.value.aString);
        int ref = 0;

        token = lexer->getToken();
        if (token.type == SPI_TOKEN_TYPE_INT)
        {
            ref = token.value.anInt;
            token = lexer->getToken();
        }
        m.reset(new Map(className.c_str(), ref));
    }
    else
    {
        m.reset(new Map(""));
    }
    if (token.type != '{')
    {
        throw RuntimeError("Expecting '{' to begin map definition");
    }

    // now keep going until we hit the end of the map
    // we expect NAME = value until terminated by '}'
    token = lexer->getToken();
    while (token.type != '}')
    {
        if (token.type != SPI_TOKEN_TYPE_NAME)
        {
            throw RuntimeError("Expecting name within map definition");
        }
        std::string fieldName(token.value.aString);
        token = lexer->getToken();
        if (token.type != '=')
            throw RuntimeError("Expecting '=' to define value");
        Value value = ReadValue(lexer);
        m->SetValue(fieldName, value);
        token = lexer->getToken();
    }

    return m;
}

ValueArraySP ReadArray(Lexer* lexer)
{
    // we have already parsed the open '['
    // subsequently we have Values separated by white space until ']'

    //ValueArraySP output (new ValueArray());
    std::vector<Value> output;

    Lexer::Token token = lexer->getToken();
    while (token.type != ']')
    {
        lexer->returnToken(token);
        const Value& value = ReadValue(lexer);
        output.push_back(value);
        token = lexer->getToken();
    }

    return new ValueArray(output, output.size());
}

namespace {
    Value ReadReference(Lexer* lexer)
    {
        Lexer::Token token = lexer->getToken();
        if (token.type == SPI_TOKEN_TYPE_INT)
        {
            return Value(ObjectRef(token.value.anInt));
        }
        else
        {
            throw RuntimeError("Expecting integer after '*'");
        }
    }
} // end of anonymous namespace

Value ReadValue(Lexer* lexer)
{
    Lexer::Token token = lexer->getToken();
    switch (token.type)
    {
    case SPI_TOKEN_TYPE_INT:
        return Value(token.value.anInt);
    case SPI_TOKEN_TYPE_DATE:
        return Value(Date(token.value.aDate));
    case SPI_TOKEN_TYPE_DOUBLE:
        return Value(token.value.aDouble);
    case SPI_TOKEN_TYPE_CHAR:
        return Value(token.value.aChar);
    case SPI_TOKEN_TYPE_STRING:
        return Value(token.value.aString);
    case SPI_TOKEN_TYPE_BOOL:
        return Value(token.value.aBool);
    case SPI_TOKEN_TYPE_NAME:
    {
        if (strcmp(token.value.aName, "ERROR") == 0)
        {
            Lexer::Token nextToken = lexer->getToken();
            if (nextToken.type == ':')
            {
                // now we are committed - the next token must be the string
                // for an ERROR
                //
                // if we don't follow up with '=' then it is just possible
                // that we have a class called ERROR
                nextToken = lexer->getToken();
                if (nextToken.type != SPI_TOKEN_TYPE_STRING)
                    throw RuntimeError("Expecting string after 'ERROR:'");
                std::string error(nextToken.value.aString);
                return Value::Error(error);
            }
            else
            {
                lexer->returnToken(nextToken);
            }
        }
        Lexer::Token nextToken = lexer->getToken();
        if (nextToken.type == '*')
        {
            return ReadReference(lexer);
        }
        else
        {
            lexer->returnToken(nextToken);
            lexer->returnToken(token);
            MapSP m = ReadMap(lexer);
            return Value(MapConstSP(m));
        }
    }
    case '{':
    {
        lexer->returnToken(token);
        MapSP m = ReadMap(lexer);
        return Value(MapConstSP(m));
    }
    case '[':
    {
        ValueArraySP values = ReadArray(lexer);
        return Value(IArrayConstSP(values));
    }
    case '*':
        return ReadReference(lexer);
    case SPI_TOKEN_TYPE_NULL:
        return Value(MapConstSP());
    }
    throw RuntimeError("Unexpected token type '%c'", (char)token.type);
}


SPI_END_NAMESPACE
