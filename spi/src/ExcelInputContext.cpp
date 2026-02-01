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
/**
***************************************************************************
** Implements ExcelInputContext class.
***************************************************************************
*/

#include "ExcelInputContext.hpp"
#include "StringUtil.hpp"
#include "ObjectHandle.hpp"

#define BEGIN_ANONYMOUS_NAMESPACE namespace {
#define END_ANONYMOUS_NAMESPACE }

SPI_BEGIN_NAMESPACE

// since Excel no longer works properly on MACs we might as well ignore
// the 1904 problem
int ExcelInputContext::DATE_OFFSET = 109205;
bool ExcelInputContext::allOptional = false;

BEGIN_ANONYMOUS_NAMESPACE

Date xlDoubleToDate(double in)
{
    int dt;
    if (in > 0)
    {
        dt = (int)in + ExcelInputContext::DATE_OFFSET;
    }
    else
    {
        dt = 0;
    }
    return Date(dt);
}

DateTime xlDoubleToDateTime(double in)
{
    if (in > 0)
    {
        if (in < 1)
            return DateTime(in);

        return DateTime(in + ExcelInputContext::DATE_OFFSET);
    }

    return DateTime(Date(0),0);
}

//double xlDateToDouble(Date in)
//{
//    double out = (int)in - ExcelInputContext::DATE_OFFSET;
//
//    if (out < 0.0)
//        out = 0.0;
//
//    return out;
//}

END_ANONYMOUS_NAMESPACE

const char* ExcelInputContext::Context() const
{
    return "EXCEL";
}

bool ExcelInputContext::ValueToBool(
    const Value& value,
    bool isOptional,
    bool defaultValue) const
{
    if ((isOptional || allOptional) && value.isUndefined())
        return defaultValue;

    switch(value.getType())
    {
    case Value::BOOL:
        return value.getBool();
    case Value::STRING:
        {
            std::string s = StringUpper(value.getString());
            if (s == "TRUE")
                return true;
            if (s == "FALSE")
                return false;
            if ((isOptional || allOptional) && s.empty())
                return defaultValue;
        }
        break;
    case Value::DOUBLE:
        {
            int i = ValueToInt(value, false, 0);
            return i != 0;
        }
        break;

    case Value::UNDEFINED:
    case Value::CHAR:
    case Value::SHORT_STRING:
    case Value::BYTES:
    case Value::INT:
    case Value::DATE:
    case Value::DATETIME:
    case Value::MAP:
    case Value::OBJECT:
    case Value::OBJECT_REF:
    case Value::ARRAY:
    case Value::ERROR:

        break;
    }

    SPI_THROW_RUNTIME_ERROR(value.toString() << " is not a bool");
}

char ExcelInputContext::ValueToChar(
    const Value& value,
    bool isOptional,
    char defaultValue) const
{
    if ((isOptional || allOptional) && value.isUndefined())
        return defaultValue;

    switch(value.getType())
    {
    case Value::STRING:
        {
            std::string s = value.getString();
            if (s.length() == 1)
                return s[0];
        }
        break;
    case Value::UNDEFINED:
    case Value::CHAR:
    case Value::SHORT_STRING:
    case Value::BYTES:
    case Value::INT:
    case Value::DOUBLE:
    case Value::BOOL:
    case Value::DATE:
    case Value::DATETIME:
    case Value::MAP:
    case Value::OBJECT:
    case Value::OBJECT_REF:
    case Value::ARRAY:
    case Value::ERROR:
        break;
    }

    SPI_THROW_RUNTIME_ERROR(value.toString() << " is not a char");
}

int ExcelInputContext::ValueToInt(const Value& value,
    bool isOptional,
    int defaultValue) const
{
    if ((isOptional || allOptional) && value.isUndefined())
        return defaultValue;

    switch(value.getType())
    {
    case Value::BOOL:
        return value.getBool() ? 1 : 0;
    case Value::STRING:
        return StringToInt(value.getString());
    case Value::DOUBLE:
        {
            double d = value.getDouble();
            int i = (int)d;

            // equality test of doubles which happen to be integers is correct
            if ((double)i == d)
                return i;
        }
        break;

    case Value::UNDEFINED:
    case Value::CHAR:
    case Value::SHORT_STRING:
    case Value::BYTES:
    case Value::INT:
    case Value::DATE:
    case Value::DATETIME:
    case Value::MAP:
    case Value::OBJECT:
    case Value::OBJECT_REF:
    case Value::ARRAY:
    case Value::ERROR:
        break;

    }

    SPI_THROW_RUNTIME_ERROR(value.toString() << " is not an integer");
}

double ExcelInputContext::ValueToDouble(const Value& value,
    bool isOptional,
    double defaultValue) const
{
    if ((isOptional || allOptional) && value.isUndefined())
        return defaultValue;

    switch(value.getType())
    {
    case Value::BOOL:
        return value.getBool() ? 1.0 : 0.0;
    case Value::STRING:
        return StringToDouble(value.getString());
    case Value::DOUBLE:
        return value.getDouble();
    case Value::UNDEFINED:
    case Value::CHAR:
    case Value::SHORT_STRING:
    case Value::BYTES:
    case Value::INT:
    case Value::DATE:
    case Value::DATETIME:
    case Value::MAP:
    case Value::OBJECT:
    case Value::OBJECT_REF:
    case Value::ARRAY:
    case Value::ERROR:
        break;
    }

    SPI_THROW_RUNTIME_ERROR(value.toString() << " is not a double");
}

std::string ExcelInputContext::ValueToString(
    const Value& value,
    bool isOptional,
    const char* defaultValue) const
{
    if ((isOptional || allOptional) && value.isUndefined())
        return defaultValue;

    switch(value.getType())
    {
    case Value::BOOL:
        return value.getBool() ? "TRUE" : "FALSE";
    case Value::SHORT_STRING:
    case Value::STRING:
        return value.getString();
    case Value::DOUBLE:
        return StringFormat("%.15g", value.getDouble());
    case Value::UNDEFINED:
    case Value::CHAR:
    case Value::BYTES:
    case Value::INT:
    case Value::DATE:
    case Value::DATETIME:
    case Value::MAP:
    case Value::OBJECT:
    case Value::OBJECT_REF:
    case Value::ARRAY:
    case Value::ERROR:
        break;
    }

    SPI_THROW_RUNTIME_ERROR(value.toString() << " is not a string");
}

Date ExcelInputContext::ValueToDate(
    const Value& value,
    bool isOptional,
    Date defaultValue) const
{
    if ((isOptional || allOptional) && value.isUndefined())
        return defaultValue;

    switch(value.getType())
    {
    case Value::STRING:
        {
            const std::string& str = value.getString();
            if ((isOptional || allOptional) && str.empty())
                return defaultValue;
            return Date::FromString(str);
        }
        break;
    case Value::DOUBLE:
        return xlDoubleToDate(value.getDouble());
    case Value::UNDEFINED:
    case Value::CHAR:
    case Value::SHORT_STRING:
    case Value::BYTES:
    case Value::INT:
    case Value::BOOL:
    case Value::DATE:
    case Value::DATETIME:
    case Value::MAP:
    case Value::OBJECT:
    case Value::OBJECT_REF:
    case Value::ARRAY:
    case Value::ERROR:
        break;
    }

    SPI_THROW_RUNTIME_ERROR(value.toString() << " is not a Date");
}

DateTime ExcelInputContext::ValueToDateTime(
    const Value& value,
    bool isOptional,
    DateTime defaultValue) const
{
    if ((isOptional || allOptional) && value.isUndefined())
        return defaultValue;

    switch(value.getType())
    {
    case Value::STRING:
        {
            const std::string& str = value.getString();
            if ((isOptional || allOptional) && str.empty())
                return defaultValue;
            return DateTime::FromString(str);
        }
        break;
    case Value::DOUBLE:
        return xlDoubleToDateTime(value.getDouble());
    case Value::UNDEFINED:
    case Value::CHAR:
    case Value::SHORT_STRING:
    case Value::BYTES:
    case Value::INT:
    case Value::BOOL:
    case Value::DATE:
    case Value::DATETIME:
    case Value::MAP:
    case Value::OBJECT:
    case Value::OBJECT_REF:
    case Value::ARRAY:
    case Value::ERROR:
        break;
    }

    SPI_THROW_RUNTIME_ERROR(value.toString() << " is not a Date");
}


ObjectConstSP ExcelInputContext::ValueToObject(
    const Value& value,
    ObjectType* objectType,
    bool optional) const
{
    // we see if we have a handle before attempting coercion from string
    if (value.getType() == Value::STRING)
    {
        std::string handle = StringStrip(value.getString());
        if (IsNullHandle(handle))
        {
            if (optional)
                return ObjectConstSP();
            throw RuntimeError("Object cannot be NULL or undefined");
        }

        try
        {
            ObjectConstSP obj = ObjectHandleFind(handle);
            // we don't check via is_instance at this point since we might
            // want to do a version_coercion instead
            //
            // note that before we feed this object into a function we will
            // be calling T::Coerce which supports version_coercion
            return obj;
        }
        catch(...) {}
    }

    if (objectType)
    {
        ObjectConstSP obj = objectType->coerce_from_value(value, this);
        if (obj)
            return obj;
    }

    switch(value.getType())
    {
    case Value::UNDEFINED:
        if (optional)
            return ObjectConstSP();
        throw RuntimeError("Object cannot be undefined");

    case Value::STRING:
    case Value::CHAR:
    case Value::SHORT_STRING:
    case Value::BYTES:
    case Value::INT:
    case Value::DOUBLE:
    case Value::BOOL:
    case Value::DATE:
    case Value::DATETIME:
    case Value::MAP:
    case Value::OBJECT:
    case Value::OBJECT_REF:
    case Value::ARRAY:
    case Value::ERROR:
        break;
    }

    SPI_THROW_RUNTIME_ERROR(value.toString() << " is not an Object");
}

bool ExcelInputContext::AcceptScalarForArray() const
{
	return true;
}

bool ExcelInputContext::StripArray() const
{
    return false; // slightly controversial one way or another
}

SPI_END_NAMESPACE
