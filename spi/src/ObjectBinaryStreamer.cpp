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
#include "MapObject.hpp"
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

bool FieldNamesEqual(
    const std::vector<std::string>& one,
    const std::vector<std::string>& two)
{
    if (one.size() != two.size())
        return false;

    size_t N = one.size();

    for (size_t i = 0; i < N; ++i)
    {
        if (one[i] != two[i])
            return false;
    }

    return true;
}

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
    FIELD_NAMES,
    OBJECT_REF,
    OBJECT_ID,
    META_DATA,
    MAP_REF,
    ERROR,
    ARRAY
};

/*
 * case statements for completing a switch statement rather than using default
    case Type::UNDEFINED:
    case Type::INT:
    case Type::INT_ARRAY:
    case Type::DOUBLE:
    case Type::DOUBLE_ARRAY:
    case Type::STRING:
    case Type::STRING_ARRAY:
    case Type::DATE:
    case Type::DATE_ARRAY:
    case Type::DATETIME:
    case Type::DATETIME_ARRAY:
    case Type::SIZE:
    case Type::SIZE_ARRAY:
    case Type::BOOL:
    case Type::BOOL_ARRAY:
    case Type::CHAR:
    case Type::MAP:
    case Type::OBJECT:
    case Type::NULL_MAP:
    case Type::NULL_OBJECT:
    case Type::FIELD_NAMES:
    case Type::OBJECT_REF:
    case Type::OBJECT_ID:
    case Type::META_DATA:
    case Type::CLASS_NAME:
    case Type::ERROR:
    case Type::ARRAY:
*/


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
    ostr.write(cstr, length + 1); // we must write the zero delimiter
}

void WriteFieldNames(std::ostream& ostr, const std::vector<std::string>& fieldNames)
{
    WriteType(ostr, Type::FIELD_NAMES);
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
        SPI_POST_CONDITION(className.empty());

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

            // we haven't seen this object before
            const char* className = obj->get_class_name();

            if (mapRef != 0)
                mapRefCache.insert(mapRef, obj);

            // we need to encode META_DATA and OBJECT_ID separately
            // otherwise we are messing up the field names of the class

            WriteType(ostr, Type::OBJECT);
            WriteString(ostr, className);

            if (mapRef > 0)
            {
                WriteType(ostr, Type::MAP_REF);
                WriteInt(ostr, mapRef);
            }

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
            MapSP m(new Map(className, mapRef));
            const bool noHiding = true;
            ObjectMap om(m, noHiding);
            obj->to_map(&om, false);

            const std::vector<std::string>& fieldNames = m->FieldNames();
            
            auto iter = indexClassNames.find(className);
            if (iter == indexClassNames.end())
            {
                WriteFieldNames(ostr, fieldNames);
                indexClassNames.insert({ className, fieldNames });
            }
            else
            {
                if (!FieldNamesEqual(fieldNames, iter->second))
                {
                    // this case will usually just be dealing with Map
                    WriteFieldNames(ostr, fieldNames);
                    iter->second = fieldNames;
                }
            }

            for (const auto& fieldName : fieldNames)
            {
                const Value& value = m->GetValue(fieldName);
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
    MapRefCache& mapRefCache,
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
    default:
        break;
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
    default:
        break;
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

    return from_data_offset(
        streamName,
        data_size - offset,
        data.data() + offset,
        metaData,
        &sizeUsed);

    // ideally the <= would be an equality
    // in other words we used the entire stream
    SPI_POST_CONDITION(sizeUsed <= data_size - offset);
}

BEGIN_ANONYMOUS_NAMESPACE

typedef std::map<std::string, std::vector<std::string>> IndexClassNames;

class BinaryReader
{
public:
    BinaryReader(const char* contents, size_t size);

    size_t used() const;

    std::vector<size_t> ReadDimensions();
    static uint32_t DimensionsSize(const std::vector<size_t>& dims);
    static bool CharToBool(char c);

    Value ReadValue(IndexClassNames& indexClassNames);

    std::vector<std::string> ReadFieldNames();
    MapConstSP ReadMap(IndexClassNames& indexClassNames);

    // reads the map for the class - does not convert to Object yet
    // we pre-suppose that Type::OBJECT has been read already
    MapConstSP ReadClassMap(IndexClassNames& indexClassNames);

    const char* work();
    void check_needed(size_t needed) const;
    void use_work(size_t used);

    /// <summary>
    /// Returns true if the next entry in work is the requested Type.
    /// In that case the Type is extracted from work.
    /// Otherwise work is left unchanged.
    /// </summary>
    /// <param name="expectedType"></param>
    /// <returns></returns>
    bool extract_expected_type(Type expectedType);

private:
    const char* m_work;
    size_t m_size;
    size_t m_used;
};

// as a workaround for gcc bugs we will put our templates in the global
// namespace (or at least the anonymous namespace) and provide the
// BinaryReader as a pointer
//
// we start with the Read() templates since these are used by ReadVector()
// then the ReadVector() templates
// finally the class implementation functions

template<typename T>
T Read(BinaryReader* reader)
{
    constexpr size_t needed = sizeof(T);
    reader->check_needed(needed);
    T value = *(const T*)reader->work();

    reader->use_work(needed);

    return value;
}

template<>
std::string Read<std::string>(BinaryReader* reader)
{
    size_t length = strlen(reader->work());
    reader->check_needed(length + 1);

    std::string out(reader->work());
    reader->use_work(length + 1);
    return out;
}

template<>
bool Read<bool>(BinaryReader* reader)
{
    return BinaryReader::CharToBool(Read<char>(reader));
}

template<>
Type Read<Type>(BinaryReader* reader)
{
    char c = Read<char>(reader);
    return (Type)c;
}

template<>
Value Read<Value>(BinaryReader*) = delete;


template<typename T>
std::vector<T> ReadVector(BinaryReader* reader, uint32_t size)
{
    size_t needed = size * sizeof(T);
    reader->check_needed(needed);

    const T* data = (const T*)(reader->work());
    reader->use_work(needed);

    return std::vector<T>(data, data + size);
}

template<>
std::vector<bool> ReadVector<bool>(BinaryReader* reader, uint32_t size)
{
    size_t needed = size;
    reader->check_needed(needed);

    const char* data = reader->work();
    reader->use_work(needed);

    std::vector<bool> vec;
    vec.reserve(size);

    for (size_t i = 0; i < size; ++i)
        vec.push_back(BinaryReader::CharToBool(data[i]));

    return vec;
}

template<>
std::vector<std::string> ReadVector<std::string>(
    BinaryReader* reader,
    uint32_t size)
{
    std::vector<std::string> vec;
    vec.reserve(size);

    for (size_t i = 0; i < size; ++i)
        vec.push_back(Read<std::string>(reader));

    return vec;
}

BinaryReader::BinaryReader(const char* contents, size_t size)
    :
    m_work(contents),
    m_size(size),
    m_used(0)
{
}

size_t BinaryReader::used() const
{
    return m_used;
}

std::vector<size_t> BinaryReader::ReadDimensions()
{
    // we encode dimensions using uint32_t rather than size_t
    // size_t is really too large for an array
    // even uint32_t is too large
    uint32_t size = Read<uint32_t>(this);
    std::vector<uint32_t> dims = ReadVector<uint32_t>(this, size);

    return std::vector<size_t>(dims.begin(), dims.end());
}

uint32_t BinaryReader::DimensionsSize(const std::vector<size_t>& dims)
{
    if (dims.size() == 0)
        return 0;

    size_t size = 1;
    for (auto dim : dims)
        size *= dim;

    return spi_util::IntegerCast<uint32_t>(size);
}

bool BinaryReader::CharToBool(char c)
{
    if (c == 'T')
        return true;
    if (c == 'F')
        return false;

    SPI_THROW_RUNTIME_ERROR("Expect 'T' or 'F' for bool");
}


Value BinaryReader::ReadValue(IndexClassNames& indexClassNames)
{
    Type type = Read<Type>(this);

    switch (type)
    {
    case Type::UNDEFINED:
        return Value();
    case Type::INT:
        return Read<int>(this);
    case Type::DOUBLE:
        return Read<double>(this);
    case Type::STRING:
        return Read<std::string>(this);
    case Type::DATE:
        return Read<Date>(this);
    case Type::DATETIME:
        return Read<DateTime>(this);
    case Type::BOOL:
        return Read<bool>(this);
    case Type::CHAR:
        return Read<char>(this);
    case Type::MAP:
        return Value(ReadMap(indexClassNames));
    case Type::OBJECT:
        return ReadClassMap(indexClassNames);
    case Type::OBJECT_REF:
        return ObjectRef(Read<int>(this));
    case Type::ERROR:
    {
        std::string error = Read<std::string>(this);
        return Value(error.c_str(), true);
    }
    case Type::NULL_MAP:
        return MapConstSP();
    case Type::NULL_OBJECT:
        return ObjectConstSP();
    case Type::INT_ARRAY:
    {
        const std::vector<size_t>& dims = ReadDimensions();
        const std::vector<int>& vec = ReadVector<int>(this, DimensionsSize(dims));
        return IArrayConstSP(new Array<int>(vec, dims));
    }
    case Type::DATE_ARRAY:
    {
        const std::vector<size_t>& dims = ReadDimensions();
        const std::vector<Date>& vec = ReadVector<Date>(this, DimensionsSize(dims));
        return IArrayConstSP(new Array<Date>(vec, dims));
    }
    case Type::DATETIME_ARRAY:
    {
        const std::vector<size_t>& dims = ReadDimensions();
        const std::vector<DateTime>& vec = ReadVector<DateTime>(this, DimensionsSize(dims));
        return IArrayConstSP(new Array<DateTime>(vec, dims));
    }
    case Type::DOUBLE_ARRAY:
    {
        const std::vector<size_t>& dims = ReadDimensions();
        const std::vector<double>& vec = ReadVector<double>(this, DimensionsSize(dims));
        return IArrayConstSP(new Array<double>(vec, dims));
    }
    case Type::STRING_ARRAY:
    {
        const std::vector<size_t>& dims = ReadDimensions();
        const std::vector<std::string>& vec = ReadVector<std::string>(this, DimensionsSize(dims));
        return IArrayConstSP(new Array<std::string>(vec, dims));
    }
    case Type::BOOL_ARRAY:
    {
        const std::vector<size_t>& dims = ReadDimensions();
        const std::vector<bool>& vec = ReadVector<bool>(this, DimensionsSize(dims));
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
    case Type::SIZE:
        SPI_THROW_RUNTIME_ERROR("Cannot read value of type SIZE");
    case Type::SIZE_ARRAY:
        SPI_THROW_RUNTIME_ERROR("Cannot read value of type SIZE_ARRAY");
    case Type::FIELD_NAMES:
        SPI_THROW_RUNTIME_ERROR("Cannot read value of type FIELD_NAMES");
    case Type::OBJECT_ID:
        SPI_THROW_RUNTIME_ERROR("Cannot read value of type OBJECT_ID");
    case Type::META_DATA:
        SPI_THROW_RUNTIME_ERROR("Cannot read value of type META_DATA");
    case Type::MAP_REF:
        SPI_THROW_RUNTIME_ERROR("Cannot read value of type MAP_REF");
    default:
        // none of the remaining types should be used for field values
        SPI_THROW_RUNTIME_ERROR("Cannot read value of type " << (int)type);
    }
}

std::vector<std::string> BinaryReader::ReadFieldNames()
{
    // we expect FIELD_NAMES to be tested before calling this function
    uint32_t size = Read<uint32_t>(this);

    std::vector<std::string> fieldNames;
    fieldNames.reserve(size);

    for (size_t i = 0; i < size; ++i)
        fieldNames.push_back(Read<std::string>(this));

    return fieldNames;
}

MapConstSP BinaryReader::ReadMap(IndexClassNames& indexClassNames)
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
MapConstSP BinaryReader::ReadClassMap(IndexClassNames& indexClassNames)
{
    std::string className = Read<std::string>(this);

    int ref;
    if (extract_expected_type(Type::MAP_REF))
        ref = Read<int>(this);
    else
        ref = 0;

    MapConstSP meta_data;
    if (extract_expected_type(Type::META_DATA))
        meta_data = ReadClassMap(indexClassNames);

    std::string object_id;
    if (extract_expected_type(Type::OBJECT_ID))
        object_id = Read<std::string>(this);

    std::vector<std::string> fieldNames;
    if (extract_expected_type(Type::FIELD_NAMES))
    {
        fieldNames = ReadFieldNames();
        indexClassNames[className] = fieldNames;
    }
    else
    {
        auto iter = indexClassNames.find(className);
        if (iter == indexClassNames.end())
            SPI_THROW_RUNTIME_ERROR("Undefined fieldNames for class " << className);
        fieldNames = iter->second;
    }

    MapSP m(new Map(className.c_str(), ref));

    if (meta_data)
        m->SetValue("meta_data", meta_data);
    if (!object_id.empty())
        m->SetValue("object_id", object_id);

    for (const auto& fieldName : fieldNames)
    {
        const Value& value = ReadValue(indexClassNames);
        m->SetValue(fieldName, value);
    }

    return m;
}

void BinaryReader::check_needed(size_t needed) const
{
    if (needed > m_size)
    {
        SPI_THROW_RUNTIME_ERROR("Available size " << m_size <<
            " less than needed " << needed);
    }
}

const char* BinaryReader::work()
{
    return m_work;
}

void BinaryReader::use_work(size_t used)
{
    m_work += used;
    m_size -= used;
    m_used += used;
}

bool BinaryReader::extract_expected_type(Type expectedType)
{
    if ((char)expectedType == *m_work)
    {
        use_work(1);
        return true;
    }
    return false;
}

END_ANONYMOUS_NAMESPACE

ObjectConstSP ObjectBinaryStreamer::from_data_offset(
    const std::string& streamName,
    size_t size,
    const char* contents,
    const MapConstSP& metaData,
    size_t* sizeUsed)
{
    SPI_UTIL_CLOCK_FUNCTION();

    size_t totalUsed = 0;

    BinaryReader reader(contents, size);

    ObjectConstSP obj;

    if (!reader.extract_expected_type(Type::OBJECT))
        SPI_THROW_RUNTIME_ERROR("contents does not represent an OBJECT");

    MapConstSP m = reader.ReadClassMap(m_indexClassNames);

    ObjectMap om(m);
    obj = m_service->object_from_map(&om, m_objectCache, metaData);

    if (sizeUsed)
        *sizeUsed = reader.used();

    return obj;
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

