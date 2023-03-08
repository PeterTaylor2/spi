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
#include "ObjectTextStreamer.hpp"
#include "RuntimeError.hpp"
#include "LexerReader.hpp"
#include "MapReader.hpp"
#include "SHA.hpp"

#include <spi_util/Lexer.hpp>
#include <spi_util/StringUtil.hpp>
#include <spi_util/Utils.hpp>
#include <spi_util/CompressUtil.hpp>

#include "Service.hpp"
#include "ObjectMap.hpp"
#include <string.h>
#include <float.h>
#include <math.h>
#include "platform.h"

#ifdef __GNUC__
using std::isnan;
using std::isinf;
#endif

#undef SPI_UTIL_CLOCK_EVENTS
#include <spi_util/ClockUtil.hpp>

#define BEGIN_ANONYMOUS_NAMESPACE namespace {
#define END_ANONYMOUS_NAMESPACE }

SPI_BEGIN_NAMESPACE

BEGIN_ANONYMOUS_NAMESPACE

const char* NumFormat(
    bool maxAccuracy,
    bool minAccuracy)
{
    if (maxAccuracy)
        return "%.17g";

    if (minAccuracy)
        return "%.8g";

    return "%.15g";
}

void WriteValue(std::ostream& ostr,
                const Value&  value,
                MapRefCache&  mapRefCache,
                const std::string& indent,
                const std::string& addIndent,
                const char* numFormat,
                bool noMapRef);

void WriteMap(std::ostream&      ostr,
              const MapConstSP&  m,
              int                ref,
              MapRefCache&       mapRefCache,
              const std::string& indent,
              const std::string& addIndent,
              const char*        numFormat,
              bool               noMapRef)
{
    const Map* mp = m.get();
    if (!mp)
    {
        ostr << "NULL";
    }
    else
    {
        const std::string& className = mp->ClassName();
        if (!className.empty())
        {
            ostr << className << " ";
            if (ref != 0)
                ostr << ref << " ";
        }
        ostr << "{" << std::endl;

        std::string newIndent = indent + addIndent;
        const std::vector<std::string>& fieldNames = mp->FieldNames();
        size_t nbFields = fieldNames.size();
        for (size_t i = 0; i < nbFields; ++i)
        {
            const std::string& name = fieldNames[i];
            const Value&       value = mp->GetValue(name);
            ostr << newIndent << name << " = ";
            WriteValue(ostr, value, mapRefCache, newIndent, addIndent,
                numFormat, noMapRef);
            ostr << std::endl;
        }
        ostr << indent << "}";
    }
}

void WriteObject(
    std::ostream&        ostr,
    const ObjectConstSP& obj,
    MapRefCache&         mapRefCache,
    const std::string&   indent,
    const std::string&   addIndent,
    const char*          numFormat,
    bool                 noMapRef,
    const MapConstSP&    metaData = MapConstSP(),
    bool                 addObjectId = false)
{
    if (!obj)
    {
        ostr << "NULL";
    }
    else
    {
        int id = noMapRef ? 0 : obj->get_id();
        if (id == 0 || mapRefCache.count(id) == 0)
        {
            // we haven't seen this object before
            // however id == 0 indicates that it is a probably a functor
            int mapRef = id == 0 ?
                0 : spi_util::IntegerCast<int>(mapRefCache.size()) + 1;
            MapSP aMap(new Map(obj->get_class_name(), mapRef));
            if (mapRef != 0)
                mapRefCache.insert(mapRef, obj);

            if (metaData)
                aMap->SetValue("meta_data", metaData);

            if (addObjectId)
                aMap->SetValue("object_id", obj->get_object_id());

            // this is where we use the abstract Object interface
            ObjectMap om(aMap);
            obj->to_map(&om, false);

            WriteMap(ostr, aMap, mapRef, mapRefCache,
                indent, addIndent, numFormat, noMapRef);
        }
        else
        {
            // we have seen this object before
            // the class name is not strictly necessary but looks clearer
            ostr << obj->get_class_name() << " *" << mapRefCache.mapRef(id);
        }
    }
}

void WriteArray(std::ostream& ostr,
                const IArray* array,
                MapRefCache&  mapRefCache,
                const std::string& indent,
                const std::string& addIndent,
                const char* numFormat,
                bool noMapRef)
{
    std::string newIndent = indent + addIndent;
    std::string sep("\n" + newIndent);

    const std::vector<size_t> dimensions = array->dimensions();
    size_t numDims = dimensions.size();
    size_t size = array->size();

    SPI_POST_CONDITION(numDims >= 1);

    ostr << "[";
    size_t size2 = 1;
    for (size_t i = 0; i < numDims; ++i)
    {
        if (i > 0)
            ostr << ",";
        ostr << dimensions[i];
        size2 *= dimensions[i];
    }
    ostr << "]";
    SPI_POST_CONDITION(size == size2);

    size_t blockSize = dimensions.back();
    for (size_t i = 0; i < size; i += blockSize)
    {
        ostr << "\n" << indent << "{";
        for (size_t j = 0; j < blockSize; ++j)
        {
            const Value& value = array->getItem(i+j);
            // FIXME: what if value is undefined?
            ostr << sep;
            WriteValue(ostr, value, mapRefCache,
                newIndent, addIndent, numFormat, noMapRef);
        }
        ostr << "\n" << indent << "}";
    }
}

void WriteValue(std::ostream& ostr,
                const Value&  value,
                MapRefCache&  mapRefCache,
                const std::string& indent,
                const std::string& addIndent,
                const char* numFormat,
                bool noMapRef)
{
    switch (value.getType())
    {
    case Value::UNDEFINED:
        ostr << "None";
        break;
    case Value::CHAR:
        ostr << "'" << value.getChar() << "'";
        break;
    case Value::SHORT_STRING:
    case Value::STRING:
        ostr << "\"" << spi_util::StringEscape(value.getString().c_str()) << "\"";
        break;
    case Value::INT:
        ostr << value.getInt();
        break;
    case Value::DOUBLE:
    {
        double d = value.getDouble();
        char   buf[64];
        if (isnan(d))
            strcpy(buf, "NAN");
        else if (isinf(d))
            strcpy(buf, "INF");
        else
        {
            sprintf(buf, numFormat, d);
            if (strchr(buf, 'e') == NULL && strchr(buf, '.') == NULL)
            {
                strcat(buf, ".0");
            }
        }
        ostr << buf;
        break;
    }
    case Value::BOOL:
        if (value.getBool())
            ostr << "true";
        else
            ostr << "false";
        break;
    case Value::DATE:
    {
        Date dt = value.getDate();
        int year, month, day;
        char buf[16];
        dt.YMD(&year, &month, &day);
        sprintf(buf, "%04d-%02d-%02d", year, month, day);
        ostr << buf;
        break;
    }
    case Value::DATETIME:
    {
        DateTime dt = value.getDateTime();
        int hours, minutes, seconds;
        char buf[32];
        dt.HMS(&hours, &minutes, &seconds);
        Date date = dt.Date();
        if (!date && !seconds)
        {
            sprintf(buf, "%02d:%02d", hours, minutes);
        }
        else
        {
            int year, month, day;
            date.YMD(&year, &month, &day);
            sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
                year, month, day, hours, minutes, seconds);
        }
        ostr << buf;
        break;
    }
    case Value::MAP:
    {
        MapConstSP m = value.getMap();
        WriteMap(ostr, m, 0, mapRefCache, indent, addIndent,
            numFormat, noMapRef);
        break;
    }
    case Value::OBJECT:
    {
        const ObjectConstSP& obj = value.getObject();
        WriteObject(ostr, obj, mapRefCache, indent, addIndent,
            numFormat, noMapRef);
        break;
    }
    case Value::OBJECT_REF:
    {
        if (noMapRef)
            throw RuntimeError("No object references allowed");
        int ref = value.getObjectRef();
        //if (ref <= 0 || ref > m_refCount)
        //    throw RuntimeError("Reference to object %d not seen before", ref);
        ostr << '*' << ref;
        break;
    }
    case Value::ARRAY:
    {
        IArrayConstSP a = value.getArray();
        WriteArray(ostr, a.get(), mapRefCache, indent, addIndent,
            numFormat, noMapRef);
        break;
    }
    case Value::ERROR:
        ostr << "ERROR \""
             << spi_util::StringEscape(value.getError().c_str())
             << "\"";
        break;
    default:
        SPI_THROW_RUNTIME_ERROR("Cannot write value of type " <<
            Value::TypeToString(value.getType()) << " to map");
    }
}

END_ANONYMOUS_NAMESPACE

MapRefCache::MapRefCache()
:
m_indexIdMapRef(),
m_objects()
{}

size_t MapRefCache::count(int id) const
{
    return m_indexIdMapRef.count(id);
}

size_t MapRefCache::size() const
{
    return m_indexIdMapRef.size();
}

void MapRefCache::insert(int mapRef, const ObjectConstSP& obj)
{
    int id = obj->get_id();

    m_indexIdMapRef[id] = mapRef;
    m_objects.push_back(obj);
}

int MapRefCache::mapRef(int id) const
{
    std::map<int,int>::const_iterator iter = m_indexIdMapRef.find(id);
    if (iter == m_indexIdMapRef.end())
        throw RuntimeError("Could not find %d in idMapRef cache", id);
    return iter->second;
}

void MapRefCache::clear()
{
    m_indexIdMapRef.clear();
    m_objects.clear();
}

ObjectConstSP ObjectTextStreamer::from_stream(
    const std::string& streamName,
    std::istream& istr,
    const MapConstSP& metaData)
{
    SPI_UTIL_CLOCK_FUNCTION();

    // the hand crafted parser (via MapReader) appears to be at least 30%
    // faster than the lexer parser (via Lexer)
#if 0
    spi_util::Lexer lexer(streamName, &istr);
    MapSP m = ReadMap(&lexer);

    ObjectMap om(m);
    return m_service->object_from_map(&om, m_objectCache);
#else
    // this method appears to be faster than using the streambuf iterator
    std::stringstream contents;
    {
        SPI_UTIL_CLOCK_BLOCK("read entire uncompressed stream");
        contents << istr.rdbuf();
    }
    return from_text(streamName, contents.str(), metaData);
#endif
}

ObjectConstSP ObjectTextStreamer::from_text(
    const std::string& streamName,
    const std::string& contents,
    const MapConstSP& metaData)
{
    SPI_UTIL_CLOCK_FUNCTION();

    MapSP m;

    // try formats in descending order from the most recently coded
    // on failure we continue until we get one which works, or else
    // we throw an exception for the first format failure
    std::string firstError;
    const int latestFormat = 2;
    const int earliestFormat = 1;

    for (int format = latestFormat; format >= earliestFormat; --format)
    {
        MapReader reader(streamName, contents.c_str());
        try
        {
            m = ReadMap(&reader, format);
            break;
        }
        catch (std::exception& e)
        {
            if (format == latestFormat)
            {
                std::ostringstream oss;
                oss << streamName << ": " << reader.lineno()
                    << ": Map read error: " << e.what();
                firstError = oss.str();
            }
            if (format == earliestFormat)
            {
                throw std::runtime_error(firstError);
            }
        }
    }

    ObjectMap om(m);
    return m_service->object_from_map(&om, m_objectCache, metaData);
}

void ObjectTextStreamer::to_stream(
    std::ostream& ostr,
    const ObjectConstSP& object,
    const MapConstSP& metaData,
    bool addObjectId)
{
    if (m_noObjectId)
        addObjectId = false;
    else if (m_objectId)
        addObjectId = true;

    WriteObject(ostr, object, m_mapRefCache, "", m_noIndent ? "" : " ",
        NumFormat(m_maxAccuracy, m_minAccuracy), m_noMapRef, metaData, addObjectId);
}

void ObjectTextStreamer::to_stream(
    std::ostream& ostr,
    const char* name,
    const Value& value)
{
    MapSP aMap(new Map(""));
    aMap->SetValue(name, value);
    WriteMap(ostr, aMap, 0, m_mapRefCache, "", m_noIndent ? "" : " ",
        NumFormat(m_maxAccuracy, m_minAccuracy), m_noMapRef);
}

void ObjectTextStreamer::to_stream(
    std::ostream& ostr,
    const MapConstSP& aMap)
{
    WriteMap(ostr, aMap, 0, m_mapRefCache, "", m_noIndent ? "" : " ",
        NumFormat(m_maxAccuracy, m_minAccuracy), m_noMapRef);
}


ObjectTextStreamerSP ObjectTextStreamer::Make(
    const ServiceConstSP& service,
    const char* options)
{
    return ObjectTextStreamerSP(new ObjectTextStreamer(service, options));
}

void ObjectTextStreamer::Register()
{
    // as long as this is registered first it will be used when format = ""
    IObjectStreamer::Register("TEXT",
        (IObjectStreamer::Maker*)ObjectTextStreamer::Make,
        false,
        "<spi>");
}

ObjectTextStreamer::~ObjectTextStreamer()
{}

void ObjectTextStreamer::clear_cache()
{
    m_mapRefCache.clear();
    m_objectCache.reset(new ObjectRefCache());
}

ObjectTextStreamer::ObjectTextStreamer(
    const ServiceConstSP& service,
    const char* options)
    :
    m_service(service),
    m_objectCache(new ObjectRefCache()),
    m_mapRefCache(),
    m_maxAccuracy(false),
    m_minAccuracy(false),
    m_noMapRef(false),
    m_noIndent(false),
    m_objectId(false),
    m_noObjectId(false)
{
    if (options)
    {
        std::vector<std::string> parts = spi_util::CStringSplit(options, ';');
        for (std::vector<std::string>::const_iterator iter = parts.begin();
             iter != parts.end(); ++iter)
        {
            if (iter->empty())
                continue;

            const std::string& part = spi_util::StringUpper(*iter);
            const char* buf = part.c_str();

            switch (*buf)
            {
            case 'A':
                if (strcmp(buf, "ACC") == 0)
                {
                    m_maxAccuracy = true;
                    continue;
                }
                break;
            case 'N':
                if (strcmp(buf, "NOREF") == 0)
                {
                    m_noMapRef = true;
                    continue;
                }
                if (strcmp(buf, "NOINDENT") == 0)
                {
                    m_noIndent = true;
                    continue;
                }
                if (strcmp(buf, "NO_OBJECT_ID") == 0)
                {
                    m_noObjectId = true;
                    continue;
                }
                break;
            case 'L':
                if (strcmp(buf, "LOACC") == 0)
                {
                    m_minAccuracy = true;
                    continue;
                }
                break;
            case 'O':
                if (strcmp(buf, "OBJECT_ID") == 0)
                {
                    m_objectId = true;
                    continue;
                }
                break;
            }
            throw RuntimeError("%s: Unknown option string %s",
                __FUNCTION__, iter->c_str());
        }
    }
}

/*
 * Implementation of ObjectCompressedTextStreamer interface.
 */
ObjectConstSP ObjectCompressedTextStreamer::from_stream(
    const std::string& streamName,
    std::istream& istr,
    const MapConstSP& metaData)
{
    SPI_UTIL_CLOCK_FUNCTION();

    // consume entire stream
    //
    // then decompress it
    //
    // then invoke the text streamer

    std::stringstream sstr;
    {
        SPI_UTIL_CLOCK_BLOCK("read entire compressed stream");
        sstr << istr.rdbuf();
    }

    std::string contents = sstr.str();
    spi_util::UncompressText(contents);

    return m_textStreamer->from_text(streamName, contents, metaData);
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
