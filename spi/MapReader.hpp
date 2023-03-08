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
#ifndef SPI_MAP_READER_HPP
#define SPI_MAP_READER_HPP

/*
***************************************************************************
** MapReader.hpp
***************************************************************************
** Functions for reading various standard types from an input stream.
***************************************************************************
*/

#include "Value.hpp"
#include "Array.hpp"

SPI_BEGIN_NAMESPACE

class MapReader
{
public:

    enum TokenType
    {
        STRING,
        CHAR,
        SEPARATOR,
        SYMBOL,
        NONE
    };

    MapReader (const std::string& filename,
        const char* contents);

    // returns false if there are no more tokens
    bool read(std::string& token, TokenType& type);
    int lineno() const;
    const std::string& filename() const;

    void returnToken(const std::string& token, TokenType type);

private:

    MapReader(const MapReader&);
    MapReader& operator=(const MapReader&);

    std::string m_filename;
    const char* m_contents; // original
    const char* m_ptr; // current pointer
    int m_lineno;
    std::string m_line; // remainder of the current line
    std::string m_returnedToken;
    TokenType m_returnedType;

    void skipWhiteSpace();
    void skipToEOL();
};

SPI_DECLARE_RC_CLASS(Map);
SPI_DECLARE_RC_CLASS(ValueArray);
SPI_DECLARE_RC_CLASS(ObjectReader);

MapSP ReadMap(MapReader* reader, int format=2);
Value ReadValue(MapReader* reader, int format);
ValueArraySP ReadArray(MapReader* reader, int format);

SPI_END_NAMESPACE

#endif

