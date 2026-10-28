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
#include "ObjectCompressedTextStreamer.hpp"
#include "ObjectTextStreamer.hpp"
#include "RuntimeError.hpp"
#include "LexerReader.hpp"
#include "MapReader.hpp"
#include "SHA.hpp"

#include <spi_util/Lexer.hpp>
#include <spi_util/StringUtil.hpp>
#include <spi_util/StreamUtil.hpp>
#include <spi_util/Utils.hpp>
#include <spi_util/CompressUtil.hpp>

#include "Service.hpp"
#include "ObjectMap.hpp"
#include <string.h>
#include <float.h>
#include <math.h>
#include "platform.h"

#undef SPI_UTIL_CLOCK_EVENTS
#include <spi_util/ClockUtil.hpp>

#define BEGIN_ANONYMOUS_NAMESPACE namespace {
#define END_ANONYMOUS_NAMESPACE }

SPI_BEGIN_NAMESPACE

/*
 * Implementation of ObjectCompressedTextStreamer interface.
 */
ObjectConstSP ObjectCompressedTextStreamer::from_data(
    const std::string& streamName,
    const std::string& data,
    size_t offset,
    const MapConstSP& metaData)
{
    SPI_UTIL_CLOCK_FUNCTION();

    // consume entire stream
    //
    // then decompress it
    //
    // then invoke the text streamer

    std::string contents = data.substr(offset);
    spi_util::UncompressText(contents);

    return m_textStreamer->from_text(streamName, contents.c_str(), metaData);
}



void ObjectCompressedTextStreamer::to_stream(
    std::ostream& ostr,
    const ObjectConstSP& object,
    const MapConstSP& metaData,
    bool addObjectId)
{
    std::ostringstream oss;
    m_textStreamer->to_stream(oss, object, metaData, addObjectId);
    ostr << spi_util::CompressText(oss.str());
}

/**
 * If options is defined then it is a ';' delimited string with the
 * same options supported as ObjectTextStreamer.
 */
ObjectCompressedTextStreamerSP ObjectCompressedTextStreamer::Make(
    const ServiceConstSP& service,
    const char* options)
{
    return ObjectCompressedTextStreamerSP(new ObjectCompressedTextStreamer(
        service, options));
}

void ObjectCompressedTextStreamer::Register()
{
    IObjectStreamer::Register("Z",
        (IObjectStreamer::Maker*)ObjectCompressedTextStreamer::Make,
        false,
        "<z-spi>");
}

ObjectCompressedTextStreamer::~ObjectCompressedTextStreamer()
{}

ObjectCompressedTextStreamer::ObjectCompressedTextStreamer(
    const ServiceConstSP& service,
    const char* options)
    :
    m_textStreamer(ObjectTextStreamer::Make(service, options))
{}

SPI_END_NAMESPACE
