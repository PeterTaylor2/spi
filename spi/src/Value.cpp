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
#include "Value.hpp"

#include <sstream>

#include "Map.hpp"
#include "RuntimeError.hpp"
#include "StringUtil.hpp"
#include "Object.hpp"
#include "Array.hpp"
#include "MatrixData.hpp"

#include <string.h>

SPI_BEGIN_NAMESPACE

namespace
{

class ValueString : public IString
{
public:
    ValueString(const char* str)
    {
        if (!str)
            throw std::runtime_error("Null inputs");
        m_str = strdup(str);
        if (!m_str)
            throw std::runtime_error("Allocation failure");
    }

    ~ValueString()
    {
        if (m_str)
        {
            free((void*)m_str);
            m_str = 0;
        }
    }

    const char* str() const
    {
        return m_str;
    }

private:
    const char* m_str;

    ValueString(const ValueString&);
    ValueString& operator=(const ValueString&);
};

} // end of private namespace within SPI_NAMESPACE

/*
***************************************************************************
** Implementation of non-virtual IArray methods.
***************************************************************************
*/

Value IArray::operator[] (size_t i) const
{
    return getItem(i);
}

std::vector<Value> IArray::getVector() const
{
    size_t numItems = size();
    std::vector<Value> out;
    out.reserve(numItems);
    for (size_t i = 0; i < numItems; ++i)
    {
        out.push_back(getItem(i));
    }
    return out;
}


Value::Type IArray::elementType() const
{
    return commonElementType();
}

Value::Type IArray::commonElementType() const
{
    size_t N = size();
    if (N == 0)
        return Value::UNDEFINED;

    Value::Type type = getItem(0).getType();
    for (size_t i = 1; i < N; ++i)
    {
        if (getItem(i).getType() != type)
            return Value::UNDEFINED;
    }
    return type;
}


/*
***************************************************************************
** Implementation of helper methods for Value::Type class.
***************************************************************************
*/
Value::Type Value::StringToType(const char* str)
{
    char   buf[16];

    if (str == NULL || *str == '\0')
    {
        return Value::UNDEFINED;
    }

    // convert the string to upper case and remove leading white space
    CStringPreProcess(str, sizeof(buf)-1, &buf[0]);

    switch (buf[0])
    {
    case 'U':
        if (strcmp (buf, "UNDEFINED") == 0) return Value::UNDEFINED;
        break;
    case 'C':
        if (strcmp (buf, "CHAR") == 0) return Value::CHAR;
        break;
    case 'S':
        if (strcmp (buf, "STRING") == 0) return Value::STRING;
        break;
    case 'D':
        if (strcmp (buf, "DOUBLE") == 0) return Value::DOUBLE;
        if (strcmp (buf, "DATE") == 0) return Value::DATE;
        if (strcmp (buf, "DATETIME") == 0) return Value::DATETIME;
        break;
    case 'B':
        if (strcmp (buf, "BOOL") == 0) return Value::BOOL;
        break;
    case 'A':
        if (strcmp (buf, "ARRAY") == 0) return Value::ARRAY;
        break;
    case 'O':
        if (strcmp (buf, "OBJECT") == 0) return Value::OBJECT;
        if (strcmp (buf, "OBJECTREF") == 0) return Value::OBJECT_REF;
        break;
    case 'M':
        if (strcmp (buf, "MAP") == 0) return Value::MAP;
        break;
    case 'E':
        if (strcmp (buf, "ERROR") == 0) return Value::ERROR;
        break;
    default:
        break;
    }

    throw RuntimeError(
        "%s: Unknown type '%s'\nPossible values: %s",
        __FUNCTION__, str,
        "Undefined, Char, String, Int, Double, Date, DateTime, Bool, Map, Object, Array, Error");
}

const char* Value::TypeToString(Value::Type type)
{
    switch (type)
    {
    case Value::UNDEFINED:
        return "Undefined";
    case Value::CHAR:
        return "Char";
    case Value::SHORT_STRING:
    case Value::STRING:
        return "String";
    case Value::INT:
        return "Int";
    case Value::DOUBLE:
        return "Double";
    case Value::BOOL:
        return "Bool";
    case Value::DATE:
        return "Date";
    case Value::DATETIME:
        return "DateTime";
    case Value::MAP:
        return "Map";
    case Value::ARRAY:
        return "Array";
    case Value::OBJECT:
        return "Object";
    case Value::OBJECT_REF:
        return "ObjectRef";
    case Value::ERROR:
        return "Error";
    }
    throw RuntimeError("%s: Unknown type %d\n", __FUNCTION__, (int)type);
}

/*
***************************************************************************
** Implementation of Value class
***************************************************************************
*/

Value::Type Value::getType() const
{
    if (type == Value::SHORT_STRING)
        return Value::STRING;
    return type;
}

/*
***************************************************************************
** Constructors for Value
***************************************************************************
*/
Value::Value () : type(Value::UNDEFINED)
{
    memset(aShortString, 0, sizeof(double));
}

Value::Value (char value) : type(Value::CHAR)
{
    aChar = value;
}

Value::Value (const char* value)
{
    setString(value);
}

Value::Value (const std::string &value)
{
    setString (value.c_str());
}

Value::Value (int value) : type(Value::INT)
{
    anInt = value;
}

Value::Value (double value) : type(Value::DOUBLE)
{
    aDouble = value;
}

Value::Value (bool value) : type(Value::BOOL)
{
    aBool = value;
}

Value::Value (const Date &value) : type(Value::DATE)
{
    aDate = (int)value;
}

Value::Value (const DateTime &value) : type(Value::DATETIME)
{
    aDateTime[0] = (int)value.Date();
    aDateTime[1] = value.Time();
}

Value::Value (const MapConstSP& value) : type(Value::MAP)
{
    aMap = value.get();
    incRefCount(aMap);
}

Value::Value (const ObjectConstSP& value) : type(Value::OBJECT)
{
    anObject = value.get();
    incRefCount(anObject);
}

Value::Value (const ObjectRef& value) : type(Value::OBJECT_REF)
{
    anObjectRef = (int)value;
}

Value::Value (const IArrayConstSP& value) : type(Value::ARRAY)
{
    anArray = value.get();
    incRefCount(anArray);
}

Value::Value (const std::exception &e)
{
    setString(e.what(), true);

    // we used the fact that we have a union to set the string value
    // now we must change the type
    type = Value::ERROR;
}

Value::Value (const char* value, bool isError)
{
    setString(value, true);

    // we used the fact that we have a union to set the string value
    // now we must change the type
    if (isError)
        type = Value::ERROR;
    else
        type = Value::STRING;
}

Value::Value(const std::vector<Value>& values)
{
    type = Value::ARRAY;
    anArray = new ValueArray(values, values.size());
    anArray->incRefCount();
}

Value::Value(const std::vector<Value>& values,
    const std::vector<size_t>& dimensions)
{
    type = Value::ARRAY;
    anArray = new ValueArray(values, dimensions);
    anArray->incRefCount();
}

/*
***************************************************************************
** Some of the more complex memory management functions for Value.
***************************************************************************
*/
Value::~Value ()
{
    freeContents();
}

Value& Value::operator= (const Value &value)
{
    Value tmp(value);
    swap(tmp);
    return *this;
}

Value::Value (const Value &value)
{
    type = value.type;
    switch (type)
    {
    case Value::UNDEFINED:
        break;
    case Value::CHAR:
        aChar = value.aChar;
        break;
    case Value::SHORT_STRING:
        strcpy(aShortString, value.aShortString);
        break;
    case Value::STRING:
    case Value::ERROR:
        aString = value.aString;
        incRefCount(aString);
        break;
    case Value::INT:
        anInt = value.anInt;
        break;
    case Value::DOUBLE:
        aDouble = value.aDouble;
        break;
    case Value::BOOL:
        aBool = value.aBool;
        break;
    case Value::DATE:
        aDate = value.aDate;
        break;
    case Value::DATETIME:
        aDateTime[0] = value.aDateTime[0];
        aDateTime[1] = value.aDateTime[1];
        break;
    case Value::MAP:
        aMap = value.aMap;
        incRefCount(aMap);
        break;
    case Value::OBJECT:
        anObject = value.anObject;
        incRefCount(anObject);
        break;
    case Value::OBJECT_REF:
        anObjectRef = value.anObjectRef;
        break;
    case Value::ARRAY:
        anArray = value.anArray;
        incRefCount(anArray);
        break;
    }
}

void Value::freeContents()
{
    switch (type)
    {
    case Value::UNDEFINED:
    case Value::CHAR:
    case Value::SHORT_STRING:
    case Value::INT:
    case Value::DOUBLE:
    case Value::BOOL:
    case Value::DATE:
    case Value::DATETIME:
    case Value::OBJECT_REF:
        break;
    case Value::STRING:
    case Value::ERROR:
        decRefCount(aString);
        break;
    case Value::MAP:
        decRefCount(aMap);
        break;
    case Value::OBJECT:
        decRefCount(anObject);
        break;
    case Value::ARRAY:
        decRefCount(anArray);
        break;
    }
    memset(aShortString, 0, sizeof(double));
}

void Value::setString(const char* value, bool neverShort)
{
    if (!neverShort && strlen(value) < sizeof(double))
    {
        strcpy(aShortString, value);
        type = Value::SHORT_STRING;
    }
    else
    {
        aString = new ValueString(value);
        incRefCount(aString);
        type = Value::STRING;
    }
}

Value& Value::swap(Value &value)
{
    // Simple bitwise swap
    char tmp[sizeof(Value)];
    memcpy(tmp, this, sizeof(Value));
    memcpy((void*)this, &value, sizeof(Value));
    memcpy((void*)&value, tmp, sizeof(Value));
    return *this;
}

bool Value::isUndefined() const
{
    return type == Value::UNDEFINED;
}

char Value::getChar(bool permissive) const
{
    switch(type)
    {
    case Value::CHAR:
        return aChar;
    case Value::SHORT_STRING:
        return aShortString[0];
    case Value::STRING:
        return aString->str()[0];
    case Value::INT:
        if (anInt == (int)((char)anInt))
            return (char)anInt;
    default:
        break;
    }

    if (permissive && type == Value::ARRAY)
        return convertArrayToScalar().getChar(true);

    SPI_THROW_RUNTIME_ERROR(toString() << " is not a char");
}

std::string Value::getString(bool permissive) const
{
    switch(type)
    {
    case Value::CHAR:
    {
        char buf[2];
        buf[0] = aChar;
        buf[1] = '\0';
        return std::string(buf);
    }
    case Value::SHORT_STRING:
        return std::string(aShortString);
    case Value::STRING:
        return std::string(aString->str());
    default:
        break;
    }

    if (permissive)
    {
        switch(type)
        {
        case Value::UNDEFINED:
            return std::string();
        case Value::INT:
            return StringFormat("%d", anInt);
        case Value::DOUBLE:
            return StringFormat("%.15g", aDouble);
        case Value::DATE:
            return std::string(Date(aDate));
        case Value::DATETIME:
            return std::string(DateTime(Date(aDateTime[0]), aDateTime[1]));
        case Value::ARRAY:
            return convertArrayToScalar().getString(true);
        default:
            break;
        }
    }

    SPI_THROW_RUNTIME_ERROR(toString() << " is not a string");
}

std::string Value::getError() const
{
    switch(type)
    {
    case Value::ERROR:
        return std::string(aString->str());
    default:
        break;
    }
    SPI_THROW_RUNTIME_ERROR(toString() << " is not an error");
}

int Value::getInt(bool permissive) const
{
    switch(type)
    {
    case Value::CHAR:
        return (int)(aShortString[0]);
    case Value::INT:
        return anInt;
    default:
        break;
    }

    if (permissive)
    {
        switch(type)
        {
        case Value::SHORT_STRING:
        case Value::STRING:
            return StringToInt(getString());
        case Value::DOUBLE:
            if (aDouble == (double)((int)aDouble))
                return (int)aDouble;
            break;
        case Value::BOOL:
            return aBool ? 1 : 0;
        case Value::ARRAY:
            return convertArrayToScalar().getInt(true);
        default:
            break;
        }
    }

    SPI_THROW_RUNTIME_ERROR(toString() << " is not an int");
}

bool Value::getBool(bool permissive) const
{
    switch(type)
    {
    case Value::CHAR:
        if (aShortString[0] == 0)
            return false;
        if (aShortString[0] == 1)
            return true;
    case Value::INT:
        if (anInt == 0)
            return false;
        if (anInt == 1)
            return true;
    case Value::BOOL:
        return aBool;
    default:
        break;
    }

    if (permissive && type == Value::ARRAY)
        return convertArrayToScalar().getBool(true);

    SPI_THROW_RUNTIME_ERROR(toString() << " is not a bool");
}

double Value::getDouble(bool permissive) const
{
    switch(type)
    {
    case Value::INT:
        return (double)anInt;
    case Value::DOUBLE:
        return aDouble;
    default:
        break;
    }

    if (permissive)
    {
        switch(type)
        {
        case Value::SHORT_STRING:
        case Value::STRING:
            return StringToDouble(getString());
        case Value::BOOL:
            return aBool ? 1.0 : 0.0;
        case Value::ARRAY:
            return convertArrayToScalar().getDouble(true);
        default:
            break;
        }
    }

    SPI_THROW_RUNTIME_ERROR(toString() << " is not a double");
}

Date Value::getDate(bool permissive) const
{
    switch(type)
    {
    case Value::DATE:
        return Date(aDate);
    // always allow DateTime since in many other languages
    // we may not have a simple Date, or else it will consider
    // (e.g. Python) that a DateTime is actually a sub-class
    // of Date
    case Value::DATETIME:
        return Date(aDateTime[0]);
    default:
        break;
    }

    if (permissive)
    {
        switch(type)
        {
        case Value::SHORT_STRING:
        case Value::STRING:
            return Date::FromString(getString());
        case Value::ARRAY:
            return convertArrayToScalar().getDate(true);
        case Value::INT:
            if (anInt == 0)
            {
                return Date();
            }
            else
            {
                int yyyymmdd = anInt;
                int year     = yyyymmdd / 10000;
                int month    = yyyymmdd / 100 % 100;
                int day      = yyyymmdd % 100;
                return Date(year, month, day);
            }
            break;
        default:
            break;
        }
    }

    SPI_THROW_RUNTIME_ERROR(toString() << " is not a Date");
}

DateTime Value::getDateTime(bool permissive) const
{
    switch(type)
    {
    case Value::DATETIME:
        return DateTime(Date(aDateTime[0]), aDateTime[1]);
    default:
        break;
    }

    if (permissive)
    {
        switch(type)
        {
        case Value::SHORT_STRING:
        case Value::STRING:
            return DateTime::FromString(getString());
        case Value::ARRAY:
            return convertArrayToScalar().getDateTime(true);
        case Value::INT:
        case Value::DATE:
            return DateTime(getDate(true), 0);
        default:
            break;
        }
    }

    SPI_THROW_RUNTIME_ERROR(toString() << " is not a DateTime");
}

MapConstSP Value::getMap() const
{
    switch(type)
    {
    case Value::MAP:
        if (!aMap)
            return MapConstSP();
        return MapConstSP(aMap);
    default:
        break;
    }
    SPI_THROW_RUNTIME_ERROR(toString() << " is not a map");
}

ObjectConstSP Value::getObject() const
{
    switch(type)
    {
    case Value::OBJECT:
        if (!anObject)
            return ObjectConstSP();
        return ObjectConstSP(anObject);
    default:
        break;
    }
    SPI_THROW_RUNTIME_ERROR(toString() << " is not an object");
}

ObjectRef Value::getObjectRef() const
{
    switch(type)
    {
    case Value::OBJECT_REF:
        return ObjectRef(anObjectRef);
        break;
    default:
        break;
    }
    SPI_THROW_RUNTIME_ERROR(toString() << " is not an object reference");
}

IArrayConstSP Value::getArray(bool permissive) const
{
    switch(type)
    {
    case Value::ARRAY:
        if (!anArray)
            return IArrayConstSP();
        return IArrayConstSP(anArray);
    default:
        break;
    }

    if (permissive)
    {
        switch(type)
        {
        case Value::UNDEFINED:
            // undefined => array of size zero
            return IArrayConstSP(new ValueArray(0));
        default:
            // any scalar is treated as an array of size 1
            return IArrayConstSP(new ValueArray(
                std::vector<Value>(1, *this), 1));
        }
    }

    SPI_THROW_RUNTIME_ERROR(toString() << " is not an array");
}

namespace
{

template <typename T>
std::vector<T> ValueGetVector(const Value& v, T getScalar(const Value&v))
{
    std::vector<T> result;

    // this of course will fail if the Value is not an array
    IArrayConstSP anArray = v.getArray();
    size_t arraySize = anArray->size();
    result.reserve(arraySize);
    for (size_t i = 0; i < arraySize; ++i)
    {
        const Value& item = anArray->getItem(i);
        result.push_back(getScalar(item));
    }
    return result;
}

template <typename T>
std::vector<T> ValueGetVector(const Value& v, bool permissive,
                              T getScalar(const Value&v, bool permissive))
{
    std::vector<T> result;

    // if permissive then this succeeds even when the input is a scalar
    IArrayConstSP anArray = v.getArray(permissive);
    size_t arraySize = anArray->size();
    result.reserve(arraySize);
    for (size_t i = 0; i < arraySize; ++i)
    {
        const Value& item = anArray->getItem(i);
        result.push_back(getScalar(item, permissive));
    }
    return result;
}

} // end of private namespace

std::vector<std::string>
Value::getStringVector(bool permissive) const
{
    return ValueGetVector<std::string>(*this, permissive, Value::ToString);
}

std::vector<double>
Value::getDoubleVector(bool permissive) const
{
    return ValueGetVector<double>(*this, permissive, Value::ToDouble);
}

std::vector<int>
Value::getIntVector(bool permissive) const
{
    return ValueGetVector<int>(*this, permissive, Value::ToInt);
}

std::vector<bool>
Value::getBoolVector(bool permissive) const
{
    return ValueGetVector<bool>(*this, permissive, Value::ToBool);
}

std::vector<Date>
Value::getDateVector(bool permissive) const
{
    return ValueGetVector<Date>(*this, permissive, Value::ToDate);
}

std::vector<DateTime>
Value::getDateTimeVector(bool permissive) const
{
    return ValueGetVector<DateTime>(*this, permissive, Value::ToDateTime);
}

std::vector<MapConstSP>
Value::getMapVector() const
{
    return ValueGetVector<MapConstSP>(*this, Value::ToMap);
}

std::vector<ObjectConstSP>
Value::getObjectVector() const
{
    return ValueGetVector<ObjectConstSP>(*this, Value::ToObject);
}

MatrixData<Value> Value::getMatrix() const
{
    IArrayConstSP anArray = getArray();

    const std::vector<size_t>& dims = anArray->dimensions();

    // easy case - we have a 2D array already
    if (dims.size() == 2)
    {
        MatrixData<Value> out(dims[0], dims[1], anArray->getVector());
        return out;
    }

    if (dims.size() != 1)
    {
        throw RuntimeError("Cannot convert array with dimension %d to Matrix",
            (int)dims.size());
    }

    // harder case - we have a 1D-array which might be an
    // array of arrays
    const std::vector<Value>& rows = anArray->getVector();
    std::vector<Value> all;
    size_t nr = rows.size();
    size_t nc = 0;
    for (size_t i = 0; i < nr; ++i)
    {
        if (rows[i].getType() != Value::ARRAY)
        {
            throw RuntimeError("Array is not an array of arrays");
        }
        const std::vector<Value>& row = rows[i].getArray()->getVector();
        if (i == 0)
        {
            nc = row.size();
        }
        else
        {
            if (nc != row.size())
                throw RuntimeError("Array is not an array of equal size arrays");
        }
        all.insert(all.end(), row.begin(), row.end());
    }
    return MatrixData<Value>(nr,nc,all);
}

std::string Value::toString (const char* indent) const
{
    std::string output;
    switch (type)
    {
    case Value::UNDEFINED:
        return "UNDEFINED";
    case Value::CHAR:
        return StringFormat("'%c'", aShortString[0]);
    case Value::SHORT_STRING:
        return StringFormat("\"%s\"", aShortString);
    case Value::STRING:
        return StringFormat("\"%s\"", aString->str());
    case Value::ERROR:
        return StringFormat("ERROR");
    case Value::INT:
        return StringFormat("%d", anInt);
    case Value::DOUBLE:
        output = StringFormat("%.17g", aDouble);
        if (strchr(output.c_str(), 'e') == NULL &&
            strchr(output.c_str(), '.') == NULL)
        {
            output += ".0";
        }
        return output;
    case Value::BOOL:
        return std::string(aBool ? "true" : "false");
    case Value::DATE:
        return std::string(Date(aDate));
    case Value::DATETIME:
        return std::string(DateTime(Date(aDateTime[0]), aDateTime[1]));
    default:
        return StringFormat("Complex value of type '%s'", TypeToString(type));
    }
}

Value Value::convertArrayToScalar() const
{
    switch (type)
    {
    case Value::UNDEFINED:
        return Value();
    case Value::ARRAY:
    {
        size_t arraySize = anArray->size();
        if (arraySize == 0)
            return Value();
        if (arraySize == 1)
            return anArray->getItem(0);
        throw RuntimeError(
            "%s: Cannot convert array of size %d to scalar",
            __FUNCTION__, (int)arraySize);
        break;
    }
    default:
        throw RuntimeError("%s: Input is not an array", __FUNCTION__);
    }
}

char Value::ToChar(const Value &v, bool permissive)
{
    return v.getChar(permissive);
}

std::string Value::ToString(const Value &v, bool permissive)
{
    return v.getString(permissive);
}

int Value::ToInt(const Value &v, bool permissive)
{
    return v.getInt(permissive);
}

bool Value::ToBool(const Value &v, bool permissive)
{
    return v.getBool(permissive);
}

double Value::ToDouble(const Value &v, bool permissive)
{
    return v.getDouble(permissive);
}

Date Value::ToDate(const Value &v, bool permissive)
{
    return v.getDate(permissive);
}

DateTime Value::ToDateTime(const Value &v, bool permissive)
{
    return v.getDateTime(permissive);
}

MapConstSP Value::ToMap(const Value &v)
{
    return v.getMap();
}

ObjectConstSP Value::ToObject(const Value &v)
{
    return v.getObject();
}

IArrayConstSP Value::ToArray(const Value &v)
{
    return v.getArray();
}

Value::operator char() const
{
    return getChar();
}

Value::operator std::string() const
{
    return getString();
}

Value::operator int() const
{
    return getInt();
}

Value::operator bool() const
{
    return getBool();
}

Value::operator double() const
{
    return getDouble();
}

Value::operator Date() const
{
    return getDate();
}

Value::operator DateTime() const
{
    return getDateTime();
}

Value::operator MapConstSP() const
{
    return getMap();
}

Value::operator ObjectConstSP() const
{
    return getObject();
}

void Value::Cast(char& t) const
{
    t = getChar();
}

void Value::Cast(bool& t) const
{
    t = getBool();
}

void Value::Cast(std::string& t) const
{
    t = getString();
}

void Value::Cast(int& t) const
{
    t = getInt();
}

void Value::Cast(double& t) const
{
    t = getDouble();
}

void Value::Cast(Date& t) const
{
    t = getDate();
}

void Value::Cast(DateTime& t) const
{
    t = getDateTime();
}

void Value::Cast(MapConstSP& t) const
{
    t = getMap();
}

void Value::Cast(ObjectConstSP& t) const
{
    t = getObject();
}

void Value::Cast(Value& t) const
{
    t = *this;
}

void Value::Translate(char& t, bool permissive) const
{
    t = getChar(permissive);
}

void Value::Translate(bool& t, bool permissive) const
{
    t = getBool(permissive);
}

void Value::Translate(std::string& t, bool permissive) const
{
    t = getString(permissive);
}

void Value::Translate(int& t, bool permissive) const
{
    t = getInt(permissive);
}

void Value::Translate(double& t, bool permissive) const
{
    t = getDouble(permissive);
}

void Value::Translate(Date& t, bool permissive) const
{
    t = getDate(permissive);
}

void Value::Translate(DateTime& t, bool permissive) const
{
    t = getDateTime(permissive);
}

void Value::Translate(Value& t, bool permissive) const
{
    t = *this; // ignoring permissive
}

SPI_END_NAMESPACE

