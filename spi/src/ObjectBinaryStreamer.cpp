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
#include "ObjectBinaryStreamer.hpp"
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

BEGIN_ANONYMOUS_NAMESPACE

// whenever we write a field we must first write its type
// if we have an array of a specific element type that is a separate type

// once we have defined this order we cannot change it - new entries must be
// added to the end of the list
//
// this is on the assumption that we want to be able to use the binary format
// for long term persistence
enum class Type
{
    UNDEFINED,
    INT,
    INT_ARRAY,
    DOUBLE,
    DOUBLE_ARRAY,
    STRING,
    STRING_ARRAY,
    DATE,
    DATE_ARRAY,
    DATETIME,
    DATETIME_ARRAY,
    SIZE,
    SIZE_ARRAY,
    BOOL,
    BOOL_ARRAY,
    CHAR,
    MAP,
    OBJECT,
    NULL_MAP,
    NULL_OBJECT,
    MAP_NAMES,
    OBJECT_REF,
    OBJECT_ID,
    META_DATA,
    CLASS_NAME,
    ERROR,
    ARRAY
};

void WriteValue(
	std::ostream& ostr,
	const Value& value,
	MapRefCache& mapRefCache,
    std::map<std::string, std::vector<std::string>>& indexClassNames);

// currently we are assuming that all systems we support have the same
// endian nature so we don't have to convert these values to network byte
// order - for scalars this isn't a big deal - but if we want to encode
// a vector in one hit then this would become a nuisance

void WriteInt(std::ostream& ostr, int n)
{
    ostr.write((const char*)&n, sizeof(int));
}

void WriteIntVector(std::ostream& ostr, const std::vector<int>& vec)
{
    if (vec.size() > 0)
        ostr.write((const char*)vec.data(), vec.size() * sizeof(int));
}

void WriteDouble(std::ostream& ostr, double n)
{
    ostr.write((const char*)&n, sizeof(double));
}

void WriteDoubleVector(std::ostream& ostr, const std::vector<double>& vec)
{
    if (vec.size() > 0)
        ostr.write((const char*)vec.data(), vec.size() * sizeof(double));
}

void WriteDate(std::ostream& ostr, Date dt)
{
    ostr.write((const char*)&dt, sizeof(Date));
}

void WriteDateVector(std::ostream& ostr, const std::vector<Date>& vec)
{
    if (vec.size() > 0)
        ostr.write((const char*)vec.data(), vec.size() * sizeof(Date));
}

void WriteDateTime(std::ostream& ostr, DateTime dt)
{
    ostr.write((const char*)&dt, sizeof(DateTime));
}

void WriteDateTimeVector(std::ostream& ostr, const std::vector<DateTime>& vec)
{
    if (vec.size() > 0)
        ostr.write((const char*)vec.data(), vec.size() * sizeof(DateTime));
}

void WriteChar(std::ostream& ostr, char c)
{
    ostr.write((const char*)&c, 1);
}

void WriteBool(std::ostream& ostr, bool b)
{
    WriteChar(ostr, b ? 'T' : 'F');
}

void WriteSize(std::ostream& ostr, size_t size)
{
    // size_t is really too big for the size of an array
    // we will limit ourselves to uint32_t instead when encoding
    uint32_t size_32 = spi_util::IntegerCast<uint32_t>(size);
    ostr.write((const char*)&size_32, sizeof(uint32_t));
}

void WriteSizeVector(std::ostream& ostr, const std::vector<size_t>& vec)
{
    size_t size = vec.size();
    WriteSize(ostr, size);

    // since we are downsizing the size_t to uint32_t we need to write
    // elements one-by-one

    for (size_t i = 0; i < size; ++i)
        WriteSize(ostr, vec.at(i));

#if false
    if (size > 0)
        ostr.write((const char*)vec.data(), size * sizeof(size_t));
#endif
}

void WriteType(std::ostream& ostr, Type type)
{
    WriteChar(ostr, (char)type);
}

void WriteString(std::ostream& ostr, const std::string& str)
{
    // we can save a lot of space by not writing the length of the string
    // and relying on the zero delimiter

    const char* cstr = str.c_str();
    size_t length = strlen(cstr);
    if (length > 0)
        ostr.write(cstr, length+1); // we must write the zero delimiter
}

void WriteStringVector(std::ostream& ostr, const std::vector<std::string>& vec)
{
    WriteType(ostr, Type::STRING_ARRAY);
    size_t size = vec.size();
    WriteSizeVector(ostr, { size });

    // we can't write strings en bloc since they are variable length
    for (const auto& str : vec)
    {
        WriteString(ostr, str);
    }
}

void WriteFieldNames(std::ostream& ostr, const std::vector<std::string>& fieldNames)
{
    WriteType(ostr, Type::MAP_NAMES);
    WriteSize(ostr, fieldNames.size());
    for (const auto& fieldName : fieldNames)
        WriteString(ostr, fieldName);
}

void WriteMap(
    std::ostream& ostr,
    const MapConstSP& m,
    MapRefCache& mapRefCache,
    std::map<std::string, std::vector<std::string>>& indexClassNames)
{
    const Map* mp = m.get();
    if (!mp)
    {
        WriteType(ostr, Type::NULL_MAP);
    }
    else
    {
        WriteType(ostr, Type::MAP);
        const std::string& className = mp->ClassName();
        SPI_POST_CONDITION(className.empty()); // or Map perhaps

        const std::vector<std::string>& fieldNames = mp->FieldNames();

        WriteFieldNames(ostr, fieldNames);

        for (const auto& fieldName : fieldNames)
        {
            const Value& value = mp->GetValue(fieldName);
            WriteValue(ostr, value, mapRefCache, indexClassNames);
        }
    }
}

void WriteObject(
	std::ostream& ostr,
	const ObjectConstSP& obj,
	MapRefCache& mapRefCache,
	std::map<std::string, std::vector<std::string>>& indexClassNames,
	const MapConstSP& metaData = MapConstSP(),
	bool addObjectId = false)
{
    if (!obj)
    {
        WriteType(ostr, Type::NULL_OBJECT);
    }
    else
    {
        int id = obj->get_id();
        if (id == 0 || mapRefCache.count(id) == 0)
        {
            // we haven't seen this object before
            // however id == 0 indicates that it is a probably a functor
            int mapRef = id == 0 ?
                0 : spi_util::IntegerCast<int>(mapRefCache.size()) + 1;

            const std::string& className = obj->get_class_name();
            MapSP aMap(new Map(className.c_str(), mapRef));
            if (mapRef != 0)
                mapRefCache.insert(mapRef, obj);

            // we need to encode META_DATA and OBJECT_ID separately
            // otherwise we are messing up the field names of the class

            WriteType(ostr, Type::OBJECT);
            // we always print the reference number for a named map
            WriteInt(ostr, mapRef);

            if (metaData)
            {
                WriteType(ostr, Type::META_DATA);
                WriteMap(ostr, metaData, mapRefCache, indexClassNames);
            }

            // typically only the top-level object has object_id recorded
            if (addObjectId)
            {
                WriteType(ostr, Type::OBJECT_ID);
                WriteString(ostr, obj->get_object_id());
            }

            // this is where we use the abstract Object interface
            //
            // we must use to_map without any fields being hidden
            // since every time we use a class it must have the same
            // field names since we only record the class field names
            // the first time we see an instance of that class
            const bool noHiding = true;
            ObjectMap om(aMap, noHiding);
            obj->to_map(&om, false);

            WriteType(ostr, Type::CLASS_NAME);
			WriteString(ostr, className);

			const std::vector<std::string>& fieldNames = aMap->FieldNames();

			// first time we see the class name we print the field names
			// this means we must use noHiding when converting object to map
			auto iter = indexClassNames.find(className);
			if (iter == indexClassNames.end())
			{
                WriteFieldNames(ostr, fieldNames);
				indexClassNames.insert({ className, fieldNames });
			}

			for (const auto& fieldName : fieldNames)
			{
				const Value& value = aMap->GetValue(fieldName);
				WriteValue(ostr, value, mapRefCache, indexClassNames);
			}
        }
        else
        {
            WriteType(ostr, Type::OBJECT_REF);
            WriteInt(ostr, mapRefCache.mapRef(id));
        }
    }
}

void WriteArray(
    std::ostream& ostr,
    const IArray* array,
    MapRefCache&  mapRefCache,
    std::map<std::string, std::vector<std::string>>& indexClassNames)
{
    SPI_PRE_CONDITION(array);

    const std::vector<size_t>& dimensions = array->dimensions();
    Value::Type elementType = array->commonElementType();

    // having a common element type means IArray was likely created using Array<T>
    // so we test for that - in which case we can encode the block of values for
    // some of the primitive types
    //
    // this assumes that there are no endian considerations
    //
    // if we need to use network byte order then this entire section needs
    // to be removed and we have to go element by element
    switch (elementType)
    {
    case Value::DATE:
    {
        const Array<Date>* array2 = dynamic_cast<const Array<Date>*>(array);
        if (array2)
        {
            WriteType(ostr, Type::DATE_ARRAY);
            WriteSizeVector(ostr, dimensions);
            const std::vector<Date>& vec = array2->getVectorOfType();
            WriteDateVector(ostr, vec);
            return;
        }
        break;
    }
    case Value::DATETIME:
    {
        const Array<DateTime>* array2 = dynamic_cast<const Array<DateTime>*>(array);
        if (array2)
        {
            WriteType(ostr, Type::DATETIME_ARRAY);
            WriteSizeVector(ostr, dimensions);
            const std::vector<DateTime>& vec = array2->getVectorOfType();
            WriteDateTimeVector(ostr, vec);
            return;
        }
        break;
    }
    case Value::DOUBLE:
    {
        const Array<double>* array2 = dynamic_cast<const Array<double>*>(array);
        if (array2)
        {
            WriteType(ostr, Type::DOUBLE_ARRAY);
            WriteSizeVector(ostr, dimensions);
            const std::vector<double>& vec = array2->getVectorOfType();
            WriteDoubleVector(ostr, vec);
            return;
        }
        break;
    }
    case Value::INT:
    {
        const Array<int>* array2 = dynamic_cast<const Array<int>*>(array);
        if (array2)
        {
            WriteType(ostr, Type::INT_ARRAY);
            WriteSizeVector(ostr, dimensions);
            const std::vector<int>& vec = array2->getVectorOfType();
            WriteIntVector(ostr, vec);
            return;
        }
        break;
    }
    }

    // if we have a common element type but cannot use Array<T> then we still
    // encode as the given type array - otherwise we have to encode the type of
    // each element of the array

    size_t size = array->size();

	switch (elementType)
	{
	case Value::DATE:
		WriteType(ostr, Type::DATE_ARRAY);
		WriteSizeVector(ostr, dimensions);
		for (size_t i = 0; i < size; ++i)
			WriteDate(ostr, array->getItem(i));
		return;
	case Value::DATETIME:
		WriteType(ostr, Type::DATETIME_ARRAY);
		WriteSizeVector(ostr, dimensions);
		for (size_t i = 0; i < size; ++i)
			WriteDateTime(ostr, array->getItem(i));
		return;
	case Value::DOUBLE:
		WriteType(ostr, Type::DOUBLE_ARRAY);
		WriteSizeVector(ostr, dimensions);
		for (size_t i = 0; i < size; ++i)
			WriteDouble(ostr, array->getItem(i));
		return;
	case Value::INT:
		WriteType(ostr, Type::INT_ARRAY);
		WriteSizeVector(ostr, dimensions);
		for (size_t i = 0; i < size; ++i)
			WriteInt(ostr, array->getItem(i));
		return;
    case Value::STRING:
    case Value::SHORT_STRING: // is this a thing in this context?
        WriteType(ostr, Type::STRING_ARRAY);
        WriteSizeVector(ostr, dimensions);
        for (size_t i = 0; i < size; ++i)
            WriteString(ostr, array->getItem(i));
        return;
    case Value::BOOL:
        WriteType(ostr, Type::BOOL_ARRAY);
        WriteSizeVector(ostr, dimensions);
        for (size_t i = 0; i < size; ++i)
            WriteBool(ostr, array->getItem(i));
        return;
    }

    // otherwise we write an ARRAY and have to write the type of each element
    WriteType(ostr, Type::ARRAY);
    WriteSizeVector(ostr, dimensions);
    for (size_t i = 0; i < size; ++i)
        WriteValue(ostr, array->getItem(i), mapRefCache, indexClassNames);
}

void WriteValue(
    std::ostream& ostr,
    const Value& value,
    MapRefCache& mapRefCache,
    std::map<std::string, std::vector<std::string>>& indexClassNames)
{
    switch (value.getType())
    {
    case Value::UNDEFINED:
        WriteType(ostr, Type::UNDEFINED);
        break;
    case Value::CHAR:
        WriteType(ostr, Type::CHAR);
        WriteChar(ostr, value);
        break;
    case Value::SHORT_STRING:
    case Value::STRING:
        WriteType(ostr, Type::STRING);
        WriteString(ostr, value);
        break;
    case Value::INT:
        WriteType(ostr, Type::INT);
        WriteInt(ostr, value);
        break;
    case Value::DOUBLE:
        WriteType(ostr, Type::DOUBLE);
        WriteDouble(ostr, value);
        break;
    case Value::BOOL:
        WriteType(ostr, Type::BOOL);
        WriteBool(ostr, value);
        break;
    case Value::DATE:
        WriteType(ostr, Type::DATE);
        WriteDate(ostr, value);
        break;
    case Value::DATETIME:
        WriteType(ostr, Type::DATETIME);
        WriteDateTime(ostr, value);
        break;
    case Value::MAP:
        WriteMap(ostr, value.getMap(), mapRefCache, indexClassNames);
        break;
    case Value::OBJECT:
        WriteObject(ostr, value.getObject(), mapRefCache, indexClassNames);
        break;
    case Value::OBJECT_REF:
        WriteType(ostr, Type::OBJECT_REF);
        WriteInt(ostr, value.getObjectRef());
        break;
    case Value::ARRAY:
    {
        IArrayConstSP a = value.getArray();
        WriteArray(ostr, a.get(), mapRefCache, indexClassNames);
        break;
    }
    case Value::ERROR:
        WriteType(ostr, Type::ERROR);
        WriteString(ostr, value.getError());
        break;
    default:
        SPI_THROW_RUNTIME_ERROR("Cannot write value of type " <<
            Value::TypeToString(value.getType()) << " to map");
    }
}

END_ANONYMOUS_NAMESPACE

ObjectConstSP ObjectBinaryStreamer::from_data(
    const std::string& streamName,
    const std::string& data,
    size_t offset,
    const MapConstSP& metaData)
{
    SPI_UTIL_CLOCK_FUNCTION();

    size_t data_size = data.size();
    if (data_size < offset)
        SPI_THROW_RUNTIME_ERROR("data_size (" << data_size << ") less than offset(" << offset << ")");

    size_t sizeUsed;

    return from_data(
        streamName,
        data_size-offset,
        data.data() + offset ,
        metaData,
        &sizeUsed);
}

BEGIN_ANONYMOUS_NAMESPACE

typedef std::map<std::string, std::vector<std::string>> IndexClassNames;

class BinaryReader
{
public:
    BinaryReader(const char* contents, size_t size)
        :
        m_work(contents),
        m_size(size),
        m_used(0)
    {
    }

    size_t used() const
    {
        return m_used;
    }

    template<typename T>
    std::vector<T> ReadVector(uint32_t size)
    {
        size_t needed = size * sizeof(T);
        check_needed(needed);

        const T* data = (const T*)m_work;
        m_work += needed;
        m_size -= needed;

        return std::vector<T>(data, data + size);
    }

    template<>
    std::vector<bool> ReadVector<bool>(uint32_t size)
    {
        size_t needed = size;
        check_needed(needed);

        const char* data = m_work;
        m_work += needed;
        m_size -= needed;

        std::vector<bool> vec;
        vec.reserve(size);

        for (size_t i = 0; i < size; ++i)
            vec.push_back(data[i]);
        
        return vec;
    }

    template<>
    std::vector<std::string> ReadVector<std::string>(uint32_t size)
    {
        std::vector<std::string> vec;
        vec.reserve(size);

        for (size_t i = 0; i < size; ++i)
            vec.push_back(Read<std::string>());

        return vec;
    }

    std::vector<size_t> ReadDimensions()
    {
        // we encode dimensions using uint32_t rather than size_t
        // size_t is really too large for an array
        // even uint32_t is too large
        uint32_t size = Read<uint32_t>();
        std::vector<uint32_t> dims = ReadVector<uint32_t>(size);

        return std::vector<size_t>(dims.begin(), dims.end());
    }

    uint32_t DimensionsSize(const std::vector<size_t>& dims)
    {
        if (dims.size() == 0)
            return 0;

        size_t size = 1;
        for (auto dim : dims)
            size *= dim;

        return spi_util::IntegerCast<uint32_t>(size);
    }

    template<typename T>
    T Read()
    {
        constexpr size_t needed = sizeof(T);
        check_needed(needed);
        T value = *(const T*)m_work;

        m_size -= needed;
        m_work += needed;

        return value;
    }

    template<>
    std::string Read<std::string>()
    {
        size_t length = strlen(m_work);
        check_needed(length + 1); 

        std::string out(m_work);

        m_size -= (length + 1);
        m_work += (length + 1);

        return out;
    }

    bool CharToBool(char c)
    {
        if (c == 'T')
            return true;
        if (c == 'F')
            return false;

        SPI_THROW_RUNTIME_ERROR("Expect 'T' or 'F' for bool");
    }

    template<>
    bool Read<bool>()
    {
        return CharToBool(Read<char>());
    }

    template<>
    Type Read<Type>()
    {
        char c = Read<char>();
        return (Type)c;
    }

    template<>
    Value Read<Value>() = delete;

    Value ReadValue(IndexClassNames& indexClassNames)
    {
        Type type = Read<Type>();

        switch (type)
        {
        case Type::UNDEFINED:
            return Value();
        case Type::INT:
            return Read<int>();
        case Type::DOUBLE:
            return Read<double>();
        case Type::STRING:
            return Read<std::string>();
        case Type::DATE:
            return Read<Date>();
        case Type::DATETIME:
            return Read<DateTime>();
        case Type::BOOL:
            return Read<bool>();
        case Type::CHAR:
            return Read<char>();
        case Type::MAP:
            return ReadMap(indexClassNames);
        case Type::OBJECT:
            return ReadClassMap(indexClassNames);
        case Type::OBJECT_REF:
            return ObjectRef(Read<int>());
        case Type::ERROR:
        {
            std::string error = Read<std::string>();
            return Value(error.c_str(), true);
        }
        case Type::NULL_MAP:
            return MapConstSP();
        case Type::NULL_OBJECT:
            return ObjectConstSP();
        case Type::INT_ARRAY:
        {
            const std::vector<size_t>& dims = ReadDimensions();
            const std::vector<int>& vec = ReadVector<int>(DimensionsSize(dims));
            return IArrayConstSP(new Array<int>(vec, dims));
        }
        case Type::DATE_ARRAY:
        {
            const std::vector<size_t>& dims = ReadDimensions();
            const std::vector<Date>& vec = ReadVector<Date>(DimensionsSize(dims));
            return IArrayConstSP(new Array<Date>(vec, dims));
        }
        case Type::DATETIME_ARRAY:
        {
            const std::vector<size_t>& dims = ReadDimensions();
            const std::vector<DateTime>& vec = ReadVector<DateTime>(DimensionsSize(dims));
            return IArrayConstSP(new Array<DateTime>(vec, dims));
        }
        case Type::DOUBLE_ARRAY:
        {
            const std::vector<size_t>& dims = ReadDimensions();
            const std::vector<double>& vec = ReadVector<double>(DimensionsSize(dims));
            return IArrayConstSP(new Array<double>(vec, dims));
        }
        case Type::STRING_ARRAY:
        {
            const std::vector<size_t>& dims = ReadDimensions();
            const std::vector<std::string>& vec = ReadVector<std::string>(DimensionsSize(dims));
            return IArrayConstSP(new Array<std::string>(vec, dims));
        }
        case Type::BOOL_ARRAY:
        {
            const std::vector<size_t>& dims = ReadDimensions();
            const std::vector<bool>& vec = ReadVector<bool>(DimensionsSize(dims));
            return IArrayConstSP(new Array<bool>(vec, dims));
        }
        case Type::ARRAY:
        {
            const std::vector<size_t>& dims = ReadDimensions();
            size_t size = DimensionsSize(dims);
            std::vector<Value> vec;
            vec.reserve(size);
            for (size_t i = 0; i < size; ++i)
                vec.push_back(ReadValue(indexClassNames));
            return IArrayConstSP(new ValueArray(vec, dims));
        }
        default:
            // none of the remaining types should be used for field values
            SPI_THROW_RUNTIME_ERROR("Cannot read value of type " << (int)type);
        }
    }

    std::vector<std::string> ReadFieldNames()
    {
        Type type = Read<Type>();
        if (type != Type::MAP_NAMES)
            SPI_THROW_RUNTIME_ERROR("Expecting MAP_NAMES to be defined");

        uint32_t size = Read<uint32_t>();

        std::vector<std::string> fieldNames;
        fieldNames.reserve(size);

        for (size_t i = 0; i < size; ++i)
            fieldNames.push_back(Read<std::string>());

        return fieldNames;
    }

    MapSP ReadMap(IndexClassNames& indexClassNames)
    {
        const std::vector<std::string>& fieldNames = ReadFieldNames();
        MapSP m(new Map(""));

        for (const auto& fieldName : fieldNames)
        {
            const Value& value = ReadValue(indexClassNames);
            m->SetValue(fieldName, value);
        }

        return m;
    }

    // reads the map for the class - does not convert to Object yet
    // we pre-suppose that Type::OBJECT has been read already
    MapSP ReadClassMap(IndexClassNames& indexClassNames)
    {
        int ref = Read<int>();

        Type type = Read<Type>();

        MapConstSP meta_data;
        if (type == Type::META_DATA)
        {
            meta_data = ReadMap(indexClassNames);
            type = Read<Type>();
        }

        std::string object_id;
        if (type == Type::OBJECT_ID)
        {
            object_id = Read<std::string>();
            type = Read<Type>();
        }

        if (type != Type::CLASS_NAME)
        {
            SPI_THROW_RUNTIME_ERROR("No className provided for OBJECT");
        }

        std::string className = Read<std::string>();

        auto iter = indexClassNames.find(className);
        if (iter == indexClassNames.end())
        {
            const std::vector<std::string>& fieldNames = ReadFieldNames();
            indexClassNames.insert({ className, fieldNames });
            iter = indexClassNames.find(className);
        }
        const std::vector<std::string>& fieldNames = iter->second;

        MapSP m(new Map(className.c_str(), ref));

        for (const auto& fieldName : fieldNames)
        {
            const Value& value = ReadValue(indexClassNames);
            m->SetValue(fieldName, value);
        }

        if (meta_data)
            m->SetValue("meta_data", meta_data, true);
        if (!object_id.empty())
            m->SetValue("object_id", object_id, true);

        return m;
    }

private:
    const char* m_work;
    size_t m_size;
    size_t m_used;

    void check_needed(size_t needed)
    {
        if (needed > m_size)
        {
            SPI_THROW_RUNTIME_ERROR("Available size " << m_size <<
                " less than needed " << needed);
        }
    }
};

END_ANONYMOUS_NAMESPACE

ObjectConstSP ObjectBinaryStreamer::from_data(
    const std::string& streamName,
    size_t size,
    const char* contents,
    const MapConstSP& metaData,
    size_t* sizeUsed)
{
    SPI_UTIL_CLOCK_FUNCTION();

    size_t totalUsed = 0;

    BinaryReader reader(contents, size);

    Type type = reader.Read<Type>();
    if (type != Type::OBJECT)
        SPI_THROW_RUNTIME_ERROR("contents does not represent an OBJECT");

    MapSP m = reader.ReadClassMap(m_indexClassNames);

    if (sizeUsed)
        *sizeUsed = totalUsed;

    ObjectMap om(m);
    return m_service->object_from_map(&om, m_objectCache, metaData);
}

// top-level object
void ObjectBinaryStreamer::to_stream(
    std::ostream& ostr,
    const ObjectConstSP& object,
    const MapConstSP& metaData,
    bool addObjectId)
{
    WriteObject(ostr, object, m_mapRefCache, m_indexClassNames, metaData, addObjectId);
}

bool ObjectBinaryStreamer::is_binary() const
{
    return true;
}

void ObjectBinaryStreamer::to_stream(
    std::ostream& ostr,
    const char* name,
    const Value& value)
{
    MapSP aMap(new Map(""));
    aMap->SetValue(name, value);
    WriteMap(ostr, aMap, m_mapRefCache, m_indexClassNames);
}

void ObjectBinaryStreamer::to_stream(
    std::ostream& ostr,
    const MapConstSP& aMap)
{
    WriteMap(ostr, aMap, m_mapRefCache, m_indexClassNames);
}

ObjectBinaryStreamerSP ObjectBinaryStreamer::Make(
    const ServiceConstSP& service,
    const char* options)
{
    return ObjectBinaryStreamerSP(new ObjectBinaryStreamer(service, options));
}

void ObjectBinaryStreamer::Register()
{
    // as long as this is registered first it will be used when format = ""
    IObjectStreamer::Register("BIN",
        (IObjectStreamer::Maker*)ObjectBinaryStreamer::Make,
        false,
        "spi-bin:");
}

ObjectBinaryStreamer::~ObjectBinaryStreamer()
{}

void ObjectBinaryStreamer::clear_cache()
{
    m_mapRefCache.clear();
    m_objectCache.reset(new ObjectRefCache());
}

ObjectBinaryStreamer::ObjectBinaryStreamer(
    const ServiceConstSP& service,
    const char* options)
    :
    m_service(service),
    m_objectCache(new ObjectRefCache()),
    m_mapRefCache(),
    m_indexClassNames()
{
    if (options && strlen(options) > 0)
    {
        SPI_THROW_RUNTIME_ERROR("No options supported for binary streamer");
    }

    SPI_PRE_CONDITION(sizeof(int) == sizeof(int32_t));
    SPI_PRE_CONDITION(sizeof(Date) == sizeof(int32_t));
    SPI_PRE_CONDITION(sizeof(DateTime) == 2 * sizeof(int32_t));
    SPI_PRE_CONDITION(sizeof(size_t) == sizeof(uint64_t));

}

SPI_END_NAMESPACE

