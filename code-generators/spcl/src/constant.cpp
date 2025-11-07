/*

    Sartorial Programming Interface (SPI) code generators
    Copyright (C) 2012-2023 Sartorial Programming Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/
/*
***************************************************************************
** constant.cpp
***************************************************************************
** Implements Constant and its sub-classes.
***************************************************************************
*/

#include "constant.hpp"

#include <spi/RuntimeError.hpp>
#include <spi/Service.hpp>
#include <spi/StringUtil.hpp>
#include <spi/spdoc_constant.hpp>

#include "generatorTools.hpp"

/*
***************************************************************************
** Singletons
***************************************************************************
*/

BoolConstantConstSP Constant::TRUE(new BoolConstant(true));
BoolConstantConstSP Constant::FALSE(new BoolConstant(false));
UndefinedConstantConstSP Constant::UNDEFINED(new UndefinedConstant());

/*
***************************************************************************
** Implementation of Constant
***************************************************************************
*/
Constant::Constant()
{}

int Constant::getInt() const
{
    throw spi::RuntimeError("Expecting 'int' actual was '%s'", typeName());
}

spi::Date Constant::getDate() const
{
    throw spi::RuntimeError("Expecting 'Date' actual was '%s'", typeName());
}

spi::DateTime Constant::getDateTime() const
{
    throw spi::RuntimeError("Expecting 'DateTime' actual was '%s'", typeName());
}

double Constant::getDouble() const
{
    throw spi::RuntimeError("Expecting 'double' actual was '%s'", typeName());
}

char Constant::getChar() const
{
    throw spi::RuntimeError("Expecting 'char' actual was '%s'", typeName());
}

std::string Constant::getString() const
{
    throw spi::RuntimeError("Expecting 'string' actual was '%s'", typeName());
}

bool Constant::getBool() const
{
    throw spi::RuntimeError("Expecting 'bool' actual was '%s'", typeName());
}

bool Constant::isUndefined() const
{
    return false;
}

/*
***************************************************************************
** Implementation of IntConstant
***************************************************************************
*/
IntConstantConstSP IntConstant::Make(int value)
{
    return IntConstantConstSP(new IntConstant(value));
}

std::string IntConstant::toCode(spdoc::PublicType publicType) const
{
    return spi::StringFormat("%d", m_value);
}

const char* IntConstant::typeName() const
{
    return "int";
}

IntConstant::IntConstant(int value)
    :
    m_value(value)
{}

int IntConstant::getInt() const
{
    return m_value;
}

spdoc::ConstantConstSP IntConstant::getDoc() const
{
    return spdoc::IntConstant::Make(m_value);
}

/*
***************************************************************************
** Implementation of DateConstant
***************************************************************************
*/
DateConstantConstSP DateConstant::Make(spi::Date value)
{
    return DateConstantConstSP(new DateConstant(value));
}

std::string DateConstant::toCode(spdoc::PublicType publicType) const
{
    if (!m_value)
        return "spi::Date()";

    int year, month, day;
    m_value.YMD(&year, &month, &day);

    return spi::StringFormat("spi::Date(%d,%d,%d)", year, month, day);
}

const char* DateConstant::typeName() const
{
    return "Date";
}

DateConstant::DateConstant(spi::Date value)
    :
    m_value(value)
{}

spi::Date DateConstant::getDate() const
{
    return m_value;
}

spdoc::ConstantConstSP DateConstant::getDoc() const
{
    return spdoc::DateConstant::Make(m_value);
}

/*
***************************************************************************
** Implementation of DateTimeConstant
***************************************************************************
*/
DateTimeConstantConstSP DateTimeConstant::Make(spi::DateTime value)
{
    return DateTimeConstantConstSP(new DateTimeConstant(value));
}

std::string DateTimeConstant::toCode(spdoc::PublicType publicType) const
{
    if (!m_value)
        return "spi::DateTime()";

    spi::Date date = m_value.Date();

    int year, month, day;
    date.YMD(&year, &month, &day);

    int hours, minutes, seconds;
    m_value.HMS(&hours, &minutes, &seconds);

    return spi::StringFormat("spi::DateTime(spi::Date(%d,%d,%d),%d,%d,%d",
        year, month, day, hours, minutes, seconds);
}

const char* DateTimeConstant::typeName() const
{
    return "DateTime";
}

DateTimeConstant::DateTimeConstant(spi::DateTime value)
    :
    m_value(value)
{}

spi::DateTime DateTimeConstant::getDateTime() const
{
    return m_value;
}

spdoc::ConstantConstSP DateTimeConstant::getDoc() const
{
    SPI_NOT_IMPLEMENTED;
    //return spdoc::DateTimeConstant::Make(value);
}

/*
***************************************************************************
** Implementation of DoubleConstant
***************************************************************************
*/
DoubleConstantConstSP DoubleConstant::Make(double value)
{
    return DoubleConstantConstSP(new DoubleConstant(value));
}

std::string DoubleConstant::toCode(spdoc::PublicType publicType) const
{
    // %.17g gives maximum value
    // %.15g is more likely to get rid of nasty trailing zeroes
    // given that this is a number defined in a configuration file
    // 17 digit accuracy is not really required
    std::string code = spi::StringFormat("%.15g", m_value);
    const char* buf = code.c_str();

    // we want to see 1.0 instead of 1 (for example)
    if (strchr(buf, 'e') == NULL && strchr(buf, '.') == NULL)
    {
        code = code + ".0";
    }
    return code;
}

const char* DoubleConstant::typeName() const
{
    return "double";
}

DoubleConstant::DoubleConstant(double value)
    :
    m_value(value)
{}

double DoubleConstant::getDouble() const
{
    return m_value;
}

spdoc::ConstantConstSP DoubleConstant::getDoc() const
{
    return spdoc::DoubleConstant::Make(m_value);
}

/*
***************************************************************************
** Implementation of CharConstant
***************************************************************************
*/
CharConstantConstSP CharConstant::Make(char value)
{
    return CharConstantConstSP(new CharConstant(value));
}

std::string CharConstant::toCode(spdoc::PublicType publicType) const
{
    return spi::StringFormat("'%c'", m_value);
}

const char* CharConstant::typeName() const
{
    return "char";
}

CharConstant::CharConstant(char value)
    :
    m_value(value)
{}

char CharConstant::getChar() const
{
    return m_value;
}

spdoc::ConstantConstSP CharConstant::getDoc() const
{
    return spdoc::CharConstant::Make(m_value);
}

/*
***************************************************************************
** Implementation of StringConstant
***************************************************************************
*/
StringConstantConstSP StringConstant::Make(const std::string& value)
{
    return StringConstantConstSP(new StringConstant(value));
}

std::string StringConstant::toCode(spdoc::PublicType publicType) const
{
    switch (publicType)
    {
    case spdoc::PublicType::DOUBLE:
    {
        std::string ucValue = spi_util::StringUpper(m_value);
        if (ucValue == "NAN")
            return "spi::not_a_number";
        if (ucValue == "INF")
            return "spi::infinity";
        if (ucValue == "-INF")
            return "-spi::infinity";
        SPI_THROW_RUNTIME_ERROR("Cannot use string '" << m_value << "' as default value for a double");
        break;
    }
    default:
        return spi::StringFormat("\"%s\"", m_value.c_str());
    }
}

const char* StringConstant::typeName() const
{
    return "string";
}

StringConstant::StringConstant(const std::string& value)
    :
    m_value(value)
{}

std::string StringConstant::getString() const
{
    return m_value;
}

spdoc::ConstantConstSP StringConstant::getDoc() const
{
    return spdoc::StringConstant::Make(m_value);
}

/*
***************************************************************************
** Implementation of BoolConstant
***************************************************************************
*/
BoolConstantConstSP BoolConstant::Make(bool value)
{
    return value ? Constant::TRUE : Constant::FALSE;
}

std::string BoolConstant::toCode(spdoc::PublicType publicType) const
{
    return m_value ? "true" : "false";
}

const char* BoolConstant::typeName() const
{
    return "bool";
}

BoolConstant::BoolConstant(bool value)
    :
    m_value(value)
{}

bool BoolConstant::getBool() const
{
    return m_value;
}

spdoc::ConstantConstSP BoolConstant::getDoc() const
{
    return spdoc::BoolConstant::Make(m_value);
}

/*
***************************************************************************
** Implementation of UndefinedConstant
***************************************************************************
*/

UndefinedConstantConstSP UndefinedConstant::Make()
{
    return Constant::UNDEFINED;
}

std::string UndefinedConstant::toCode(spdoc::PublicType publicType) const
{
    switch(publicType)
    {
    case spdoc::PublicType::BOOL:
        return "false";
    case spdoc::PublicType::CHAR:
        return "' '";
    case spdoc::PublicType::INT:
        return "0";
    case spdoc::PublicType::DOUBLE:
        return "0.0";
    case spdoc::PublicType::STRING:
        return "std::string()";
    case spdoc::PublicType::DATE:
        return "spi::Date()";
    case spdoc::PublicType::DATETIME:
        return "spi::DateTime()";
    case spdoc::PublicType::ENUM:
    case spdoc::PublicType::ENUM_BITMASK:
    case spdoc::PublicType::CLASS:
    case spdoc::PublicType::OBJECT:
    case spdoc::PublicType::MAP:
    case spdoc::PublicType::VARIANT:
    case spdoc::PublicType::UNINITIALIZED_VALUE:
        return "{}"; // c++11 only
    }
    throw spi::RuntimeError("UndefinedConstant::toCode");
}

const char* UndefinedConstant::typeName() const
{
    return "undefined";
}

int UndefinedConstant::getInt() const
{
    return 0;
}

spi::Date UndefinedConstant::getDate() const
{
    return spi::Date();
}

spi::DateTime UndefinedConstant::getDateTime() const
{
    return spi::DateTime();
}

double UndefinedConstant::getDouble() const
{
    return 0.0;
}

std::string UndefinedConstant::getString() const
{
    return std::string();
}

bool UndefinedConstant::getBool() const
{
    return false;
}

bool UndefinedConstant::isUndefined() const
{
    return true;
}

UndefinedConstant::UndefinedConstant()
{}

spdoc::ConstantConstSP UndefinedConstant::getDoc() const
{
    return spdoc::UndefinedConstant::Make();
}
