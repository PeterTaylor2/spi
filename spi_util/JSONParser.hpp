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

#ifndef SPI_UTIL_JSON_PARSER_HPP
#define SPI_UTIL_JSON_PARSER_HPP

/*
***************************************************************************
** JSONParser.hpp
***************************************************************************
** Parses a stream and returns a JSON-style map.
***************************************************************************
*/

#include "DeclSpec.h"
#include "Namespace.hpp"
#include "RefCounter.hpp"

#include <string>
#include <map>
#include <vector>
#include <iostream>

SPI_UTIL_NAMESPACE

class JSONValue;

SPI_UTIL_IMPORT
void JSONValueToStream(
    std::ostream& str,
    const JSONValue& value,
    bool noNewLine = false,
    size_t indent = 0);

SPI_UTIL_IMPORT
JSONValue JSONParseValue(
    std::istream& istr,
    const std::string& streamName=std::string());

//SPI_UTIL_IMPORT
JSONValue JSONValueFromString(
    std::string& str);

//SPI_UTIL_IMPORT
std::string JSONValueToString(const JSONValue& value);

SPI_UTIL_END_NAMESPACE

#endif
