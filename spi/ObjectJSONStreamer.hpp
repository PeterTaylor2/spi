/*
***************************************************************************
** ObjectJSONStreamer.hpp
***************************************************************************
** Defines streaming interface for objects using JSON.
**
** Note that we don't try to catch duplicate objects using JSON.
** Hence we won't be including the object references.
***************************************************************************
*/

#ifndef SPI_OBJECT_JSON_STREAMER_HPP
#define SPI_OBJECT_JSON_STREAMER_HPP

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

#include "IObjectStreamer.hpp"

SPI_BEGIN_NAMESPACE

class Value;
SPI_DECLARE_RC_CLASS(ObjectJSONStreamer);

/**
 * ObjectJSONStreamer interface.
 *
 * Implements to_ostream and from_istream for an object using a text format.
 */
class SPI_IMPORT ObjectJSONStreamer : public IObjectStreamer
{
public:
    // implementation of IObjectStreamer
    ObjectConstSP from_stream(
        const std::string& streamName,
        std::istream& istr,
        const MapConstSP& metaData);
    void to_stream(
        std::ostream& ostr,
        const ObjectConstSP& object,
        const MapConstSP& metaData,
        bool addObjectId);

    // writes a field to an output stream in the form of a single value Map
    // without a class name or reference number
    void to_stream(std::ostream& ostr, const char* name, const Value& value);

    static ObjectJSONStreamerSP Make(
        const ServiceConstSP& service,
        const char* options=0);

    static void Register();

    // re-implements IObjectStreamer
    bool uses_recognizer() const;

    ~ObjectJSONStreamer();

private:
    // main constructor - accessed via static Make method
    ObjectJSONStreamer(
        const ServiceConstSP& service,
        const char* options);

    // prevent compiler implementation of copy constructor and assignment
    ObjectJSONStreamer(const ObjectJSONStreamer&);
    ObjectJSONStreamer& operator=(const ObjectJSONStreamer&);

    ServiceConstSP m_service;
    bool m_noNewLine;
    bool m_objectId;
    bool m_noObjectId;

};

SPI_END_NAMESPACE

#endif

