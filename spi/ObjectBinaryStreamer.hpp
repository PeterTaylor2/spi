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
** ObjectBinaryStreamer.hpp
***************************************************************************
** Defines binary streaming interface for objects.
***************************************************************************
*/

#ifndef SPI_OBJECT_BINARY_STREAMER_HPP
#define SPI_OBJECT_BINARY_STREAMER_HPP

#include "IObjectStreamer.hpp"
#include "MapRefCache.hpp"

SPI_BEGIN_NAMESPACE

class Value;
SPI_DECLARE_RC_CLASS(Map);
SPI_DECLARE_RC_CLASS(ObjectBinaryStreamer);
SPI_DECLARE_RC_CLASS(ObjectRefCache);

/**
 * ObjectBinaryStreamer interface.
 *
 * Implements to_ostream and from_istream for an object using a text format.
 */
class SPI_IMPORT ObjectBinaryStreamer : public IObjectStreamer
{
public:
    // functions to implement IObjectStreamer
    ObjectConstSP from_data(
        const std::string& streamName,
        const std::string& data,
        size_t offset,
        const MapConstSP& metaData) override;
    void to_stream(
        std::ostream& ostr,
        const ObjectConstSP& object,
        const MapConstSP& metaData = MapConstSP(),
        bool addObjectId = false) override;

    bool is_binary() const override;

    // functions specific to ObjectTextStreamer

    // converts text to object - called after extracting the text from a
    // stream (which might have been a compressed stream)
    ObjectConstSP from_data(
        const std::string& streamName,
        size_t size,
        const char* contents,
        const MapConstSP& metaData,
        size_t* sizeUsed
    );

    // writes a field to an output stream in the form of a single value Map
    // without a class name or reference number
    void to_stream(std::ostream& ostr, const char* name, const Value& value);
    void to_stream(std::ostream& ostr, const MapConstSP& m);

    /**
     * If options is defined then it is a ';' delimited string with the
     * following options supported (case independent):
     *
     * NAMES: stream the names for a class - but only the first time that
     *        we see that class
     */
    static ObjectBinaryStreamerSP Make(
        const ServiceConstSP& service,
        const char* options=0);

    static void Register();

    ~ObjectBinaryStreamer();

    void clear_cache();

private:
    // main constructor - accessed via static Make method
    ObjectBinaryStreamer(
        const ServiceConstSP& service,
        const char* options);

    // prevent compiler implementation of copy constructor and assignment
    ObjectBinaryStreamer(const ObjectBinaryStreamer&);
    ObjectBinaryStreamer& operator=(const ObjectBinaryStreamer&);

    ServiceConstSP   m_service;
    ObjectRefCacheSP m_objectCache; // used when reading from file
    MapRefCache      m_mapRefCache; // used when writing to file

    std::map<std::string, std::vector<std::string>> m_indexClassNames;
};

SPI_END_NAMESPACE

#endif

