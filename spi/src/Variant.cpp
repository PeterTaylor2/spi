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
** Variant.cpp
***************************************************************************
** Polymorphic class with value semantics, arbitrary type and late binding.
**
** Value semantics means that Variant can be assigned by value and will
** correctly handle all memory issues.
**
** Arbitrary type means any of the built-in types.
**
** Late binding means that sometimes you only discover the type in the
** context of the interface. For example, when using Excel you don't
** know whether a number should be treated as an int, a double or a date.
** Variant class won't know, but in context the correct type will be
** requested.
***************************************************************************
*/

#include "Variant.hpp"

#include "InputValues.hpp"
#include "RuntimeError.hpp"
#include "StringUtil.hpp"
#include "Service.hpp"
#include "ObjectMap.hpp"
#include "ValueToObject.hpp"

SPI_BEGIN_NAMESPACE


/*
***************************************************************************
** Implementation of Variant class
***************************************************************************
*/
Variant::Variant(const Value& value, const InputContext* context)
:
m_value(value),
m_context(context)
{
    if (!context)
        throw RuntimeError("No input context provided for Variant constructor");
}

Variant::Variant(const Value& value)
:
m_value(value),
m_context(InputContext::NoContext())
{}

Variant::Variant(
    const Value& value,
    ValueToObject& mapToObject)
    :
    m_value(),
    m_context(0)
{
    Value::Type vt = value.getType();

    if (vt == Value::MAP)
    {
        MapConstSP m = value.getMap();
        if (m->MapType() == Map::VARIANT)
        {
            // this is a special Map with context & value
            std::string contextName = m->GetValue("context").getString(true);
            Value value = m->GetValue("value");

            m_context = InputContext::Find(contextName);
            m_value.swap(value);

            return;
        }
    }

    // resolves object maps and object references
    // everything else is just passed through
    m_value = mapToObject.ResolveObject(value);
    m_context = InputContext::NoContext();
}

Variant::~Variant()
{}

Variant::Variant(const Variant& variant)
:
m_value(variant.m_value),
m_context(variant.m_context)
{}

Variant& Variant::operator=(const Variant& variant)
{
    m_value = variant.m_value;
    m_context = variant.m_context;
    return *this;
}

MapConstSP Variant::ToMap() const
{
    MapSP m(new Map(Map::VARIANT));

    const char* context = GetContext();
    if (context && *context)
        m->SetValue("context", context);

    const Value value = GetValue();
    if (!value.isUndefined())
        m->SetValue("value", value);

    return m;
}

Variant::Variant(const MapConstSP& m)
:
m_value(),
m_context(0)
{
    SPI_PRE_CONDITION(m);

    Map::Type mapType = m->MapType();
    if (mapType == Map::VARIANT)
    {
        std::string contextName = m->GetValue("context").getString();
        Value value = m->GetValue("value");

        m_context = InputContext::Find(contextName);
        m_value.swap(value);
    }
    else
    {
        SPI_THROW_RUNTIME_ERROR("Variant(const MapConstSP&) should only be used for Maps of type VARIANT");
    }
}

Value::Type Variant::ValueType() const
{
    return m_value.getType();
}

/* data access methods */
bool Variant::IsUndefined() const
{
    return m_value.isUndefined();
}

char Variant::ToChar(bool optional, char defaultValue) const
{
    return m_context->ValueToChar(m_value, optional, defaultValue);
}

std::string Variant::ToString(bool optional, const char* defaultValue) const
{
    return m_context->ValueToString(m_value, optional, defaultValue);
}

int Variant::ToInt(bool optional, int defaultValue) const
{
    return m_context->ValueToInt(m_value, optional, defaultValue);
}

bool Variant::ToBool(bool optional, bool defaultValue) const
{
    return m_context->ValueToBool(m_value, optional, defaultValue);
}

double Variant::ToDouble(bool optional, double defaultValue) const
{
    return m_context->ValueToDouble(m_value, optional, defaultValue);
}

Date Variant::ToDate(bool optional, Date defaultValue) const
{
    return m_context->ValueToDate(m_value, optional, defaultValue);
}

DateTime Variant::ToDateTime(bool optional, DateTime defaultValue) const
{
    return m_context->ValueToDateTime(m_value, optional, defaultValue);
}

ObjectConstSP Variant::ToConstObject(ObjectType* objectType, bool optional) const
{
    return m_context->ValueToObject(m_value, objectType, optional);
}

Variant::operator char() const
{
    return ToChar();
}

Variant::operator std::string() const
{
    return ToString();
}

Variant::operator int() const
{
    return ToInt();
}

Variant::operator bool() const
{
    return ToBool();
}

Variant::operator double() const
{
    return ToDouble();
}

Variant::operator Date() const
{
    return ToDate();
}

Variant::operator ObjectConstSP() const
{
    return ToConstObject();
}

std::vector<std::string> Variant::ToStringVector(bool optional, const char* defaultValue) const
{
    return m_context->ValueToStringVector(m_value, optional, defaultValue);
}

std::vector<double> Variant::ToDoubleVector(bool optional, double defaultValue) const
{
    return m_context->ValueToDoubleVector(m_value, optional, defaultValue);
}

std::vector<int> Variant::ToIntVector(bool optional, int defaultValue) const
{
    return m_context->ValueToIntVector(m_value, optional, defaultValue);
}

std::vector<bool> Variant::ToBoolVector(bool optional, bool defaultValue) const
{
    return m_context->ValueToBoolVector(m_value, optional, defaultValue);
}

std::vector<Date> Variant::ToDateVector(bool optional, Date defaultValue) const
{
    return m_context->ValueToDateVector(m_value, optional, defaultValue);
}

std::vector<DateTime> Variant::ToDateTimeVector(bool optional, DateTime defaultValue) const
{
    return m_context->ValueToDateTimeVector(m_value, optional, defaultValue);
}

std::vector<ObjectConstSP> Variant::ToConstObjectVector(ObjectType* objectType, bool optional) const
{
    return m_context->ValueToObjectVector(m_value, objectType, optional);
}

Variant Variant::ToScalar() const
{
    if (m_value.getType() == Value::ARRAY)
    {
        Value scalar = m_value.convertArrayToScalar();
        return Variant(scalar, m_context);
    }
    return Variant(m_value, m_context);
}

/*
***************************************************************************
** Constructors for Variant
***************************************************************************
*/
Variant::Variant ()
:
m_value(Value()),
m_context(InputContext::NoContext())
{}

Variant::Variant (char value)
:
m_value(value),
m_context(InputContext::NoContext())
{}

Variant::Variant (const char* value)
:
m_value(value),
m_context(InputContext::NoContext())
{}

Variant::Variant (const std::string& value)
:
m_value(value),
m_context(InputContext::NoContext())
{}

Variant::Variant (int value)
:
m_value(value),
m_context(InputContext::NoContext())
{}

Variant::Variant (double value)
:
m_value(value),
m_context(InputContext::NoContext())
{}

Variant::Variant (bool value)
:
m_value(value),
m_context(InputContext::NoContext())
{}

Variant::Variant (const Date& value)
:
m_value(value),
m_context(InputContext::NoContext())
{}

Variant::Variant (const ObjectConstSP& value)
:
m_value(value),
m_context(InputContext::NoContext())
{}

Variant::operator Value() const
{
    if (m_context != InputContext::NoContext())
        throw RuntimeError("Cannot cast to Value for input Variant");

    return m_value;
}

const Value& Variant::GetValue() const
{
    return m_value;
}

const char* Variant::GetContext() const
{
    return m_context->Context();
}

const InputContext* Variant::GetInputContext() const
{
    return m_context;
}

Value Variant::VectorToValue(const std::vector<spi::Variant>& in)
{
    // this only works if all the inputs are scalars
    size_t N = in.size();
    std::vector<Value> values;

    for (size_t i = 0; i < N; ++i)
    {
        const Value& value = in[i].GetValue();
        if (value.getType() == Value::ARRAY)
            SPI_THROW_RUNTIME_ERROR("Cannot convert array elements to scalars");

        values.push_back(value);
    }

    return Value(values);
}

Value Variant::MatrixToValue(const spi::MatrixData<Variant>& in)
{
    return Value(in.ToArray());
}

SPI_END_NAMESPACE

