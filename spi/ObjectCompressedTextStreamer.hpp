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
/*
***************************************************************************
** ObjectTextStreamer.hpp
***************************************************************************
** Defines streaming interface for objects.
***************************************************************************
*/

#ifndef SPI_OBJECT_COMPRESSED_TEXT_STREAMER_HPP
#define SPI_OBJECT_COMPRESSED_TEXT_STREAMER_HPP

#include "IObjectStreamer.hpp"
#include "MapRefCache.hpp"

SPI_BEGIN_NAMESPACE

class Value;
SPI_DECLARE_RC_CLASS(Map);
SPI_DECLARE_RC_CLASS(ObjectTextStreamer);
SPI_DECLARE_RC_CLASS(ObjectRefCache);
SPI_DECLARE_RC_CLASS(ObjectCompressedTextStreamer);

/**
 * ObjectCompressedTextStreamer interface.
 *
 * Implements to_ostream and from_istream for an object using a text format.
 */
class SPI_IMPORT ObjectCompressedTextStreamer : public IObjectStreamer
{
public:
    // implementation of IObjectTextStreamer
    ObjectConstSP from_data(
        const std::string& streamName,
        const std::string& data,
        size_t offset,
        const MapConstSP& metaData);
    void to_stream(
        std::ostream& ostr,
        const ObjectConstSP& object,
        const MapConstSP& metaData,
        bool addObjectId);

    /**
     * If options is defined then it is a ';' delimited string with the
     * same options supported as ObjectTextStreamer.
     */
    static ObjectCompressedTextStreamerSP Make(
        const ServiceConstSP& service,
        const char* options=0);

    static void Register();

    ~ObjectCompressedTextStreamer();

private:
    // main constructor - accessed via static Make method
    ObjectCompressedTextStreamer(
        const ServiceConstSP& service,
        const char* options);

    // prevent compiler implementation of copy constructor and assignment
    ObjectCompressedTextStreamer(const ObjectCompressedTextStreamer&);
    ObjectCompressedTextStreamer& operator=(const ObjectCompressedTextStreamer&);

    ObjectTextStreamerSP m_textStreamer;
};

SPI_END_NAMESPACE

#endif

