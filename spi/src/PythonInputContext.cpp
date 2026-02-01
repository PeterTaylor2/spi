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
** Implements PythonInputContext class.
***************************************************************************
*/

#include "PythonInputContext.hpp"
#include "StringUtil.hpp"

SPI_BEGIN_NAMESPACE

bool PythonInputContext::allOptional = false;

PythonInputContext::PythonInputContext()
{}

const char* PythonInputContext::Context() const
{
    return "PYTHON";
}

bool PythonInputContext::ValueToBool(
    const Value& value,
    bool isOptional,
    bool defaultValue) const
{
    if ((isOptional || allOptional) && value.isUndefined())
        return defaultValue;

    bool permissive = !value.isUndefined();
    return value.getBool(permissive);
}

char PythonInputContext::ValueToChar(
    const Value& value,
    bool isOptional,
    char defaultValue) const
{
    if ((isOptional || allOptional) && value.isUndefined())
        return defaultValue;

    bool permissive = !value.isUndefined();
    std::string s = value.getString(permissive);

    if (s.length() == 1)
        return s[0];

    SPI_THROW_RUNTIME_ERROR(value.toString() << " is not a char");
}

int PythonInputContext::ValueToInt(const Value& value,
    bool isOptional,
    int defaultValue) const
{
    if ((isOptional || allOptional) && value.isUndefined())
        return defaultValue;

    bool permissive = !value.isUndefined();
    return value.getInt(permissive);
}

double PythonInputContext::ValueToDouble(const Value& value,
    bool isOptional,
    double defaultValue) const
{
    if ((isOptional || allOptional) && value.isUndefined())
        return defaultValue;

    bool permissive = !value.isUndefined();
    return value.getDouble(permissive);
}

std::string PythonInputContext::ValueToString(
    const Value& value,
    bool isOptional,
    const char* defaultValue) const
{
    if ((isOptional || allOptional) && value.isUndefined())
        return defaultValue;

    bool permissive = !value.isUndefined();
    return value.getString(permissive);
}

Date PythonInputContext::ValueToDate(
    const Value& value,
    bool isOptional,
    Date defaultValue) const
{
    if ((isOptional || allOptional) && value.isUndefined())
        return defaultValue;

    bool permissive = !value.isUndefined();
    return value.getDate(permissive);
}

DateTime PythonInputContext::ValueToDateTime(
    const Value& value,
    bool isOptional,
    DateTime defaultValue) const
{
    if ((isOptional || allOptional) && value.isUndefined())
        return defaultValue;

    bool permissive = !value.isUndefined();
    return value.getDateTime(permissive);
}

ObjectConstSP PythonInputContext::ValueToObject(
    const Value& value,
    ObjectType* objectType,
    bool optional) const
{
    switch(value.getType())
    {
    case Value::UNDEFINED:
        if (optional)
            return ObjectConstSP();
        if (objectType)
        {
            throw RuntimeError("Object of type %s cannot be undefined",
                objectType->get_class_name());
        }
        throw RuntimeError("Object cannot be undefined");
    case Value::OBJECT:
        // before we feed this object into a function we will be calling
        // T::Coerce which supports version_coercion
        return value.getObject();
    case Value::CHAR:
    case Value::SHORT_STRING:
    case Value::STRING:
    case Value::BYTES:
    case Value::INT:
    case Value::DOUBLE:
    case Value::BOOL:
    case Value::DATE:
    case Value::DATETIME:
    case Value::MAP:
    case Value::OBJECT_REF:
    case Value::ARRAY:
    case Value::ERROR:
        break;
    }

    if (objectType)
    {
        ObjectConstSP obj = objectType->coerce_from_value(value, this);
        if (obj)
            return obj;
    }

    if (objectType)
        SPI_THROW_RUNTIME_ERROR(value.toString()
            << " could not be converted to '" << objectType->get_class_name()
            << "'");

    SPI_THROW_RUNTIME_ERROR(value.toString() << " is not an Object");
}

bool PythonInputContext::AcceptScalarForArray() const
{
	return true;
}

bool PythonInputContext::StripArray() const
{
    return false;
}

SPI_END_NAMESPACE
