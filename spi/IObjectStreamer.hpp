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
** IObjectStreamer.hpp
***************************************************************************
** Defines streaming interface for objects.
***************************************************************************
*/

#ifndef SPI_IOBJECT_STREAMER_HPP
#define SPI_IOBJECT_STREAMER_HPP

#include "RefCounter.hpp"
#include "Map.hpp"

#include <map>
#include <istream>
#include <ostream>
#include <string>
#include <vector>

SPI_BEGIN_NAMESPACE

SPI_DECLARE_RC_CLASS(IObjectStreamer);
SPI_DECLARE_RC_CLASS(Object);
SPI_DECLARE_RC_CLASS(Service);
SPI_DECLARE_RC_CLASS(Map);

/**
 * IObjectStreamer interface.
 *
 * An object streamer describes the way that objects are written to and from
 * C++ stream-like classes.
 */
class SPI_IMPORT IObjectStreamer : public RefCounter
{
public:
    /**
     * Defines the constructors of the various types of IObjectStream
     * available in the library.
     *
     * Initially we will support an easy to read text format.
     *
     * Subsequently we expect to add binary formats with the field names
     * excluded.
     */
    typedef IObjectStreamerSP (Maker) (const ServiceConstSP& service,
                                       const char* options);

    virtual ObjectConstSP from_data(
        const std::string& streamName, // for error messages only
        const std::string& data, // can be binary or text
        size_t offset, // skipped text (for recognizer)
        const MapConstSP& metaData = MapConstSP()) = 0;
    virtual void to_stream(
        std::ostream& ostr,
        const ObjectConstSP& object,
        const MapConstSP& metaData = MapConstSP(),
        bool addObjectId = false) = 0;

    // by default the streamer does not use the recognizer string
    virtual bool uses_recognizer() const;
    virtual bool is_binary() const;

    static IObjectStreamerSP Make(
        const ServiceConstSP& service,
        const std::string& key,
        const char* options=0);

    static const char* Recognizer(const std::string& key);

    static IObjectStreamerSP MakeWriter(
        const std::string& key,
        const char* options=0);

    static void Register(const std::string& key, Maker* maker,
        bool isBinary, const char* recognizer);
    static std::vector<std::string> Formats(bool allowBinary);

private:
    struct RegInfo
    {
        RegInfo(Maker*, bool, const char*);
        RegInfo();

        Maker*      maker;
        bool        isBinary;
        const char* recognizer;
    };

    static std::map<std::string, RegInfo> g_registry;
    static std::vector<std::string> g_formats;

    static RegInfo FindRegInfo(const std::string& key);
};

SPI_END_NAMESPACE

#endif

