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
#include "OptionsParser.hpp"
#include "RuntimeError.hpp"
#include "Value.hpp"

#include <sstream>
#include <spi_util/Lexer.hpp>

SPI_BEGIN_NAMESPACE

using spi_util::Lexer;

namespace
{
    template<typename T>
    T GetOptionValue(const std::string& name,
                     const std::map<std::string,Value>& options)
    {
        std::map<std::string,Value>::const_iterator iter = options.find(name);
        if (iter == options.end())
        {
            throw RuntimeError("%s: %s not defined in options",
                               __FUNCTION__, name.c_str());
        }
	T out;
	iter->second.Cast(out);
	return out;
    }

    template<typename T>
    void SetOptionValue(const std::string& name, T value,
                        std::map<std::string,Value>& options)
    {
        std::map<std::string,Value>::iterator iter = options.find(name);
        if (iter == options.end())
        {
            options[name] = Value(value);
        }
        else
        {
            Value v(value);
            if (v.getType() != iter->second.getType())
            {
                throw RuntimeError("%s: Type mismatch when setting option %s. "
                                   "Previous type: %s. New type: %s",
                                   __FUNCTION__, name.c_str(),
                                   Value::TypeToString(v.getType()),
                                   Value::TypeToString(iter->second.getType()));
            }
            iter->second = v;
        }
    }

}

OptionsParser::OptionsParser(const std::string& name)
    :
    m_name(name),
    m_options(),
    m_defaults()
{}

namespace
{
    std::runtime_error TypeMismatch(
        const std::string& optionName,
        Value::Type expected,
        const Lexer::Token& value)
    {
        return RuntimeError("Type mismatch for option %s - expected %s, "
                            "provided value %s",
                            optionName.c_str(),
                            Value::TypeToString(expected),
                            value.toString().c_str());
    }
}

void OptionsParser::Parse(const char* str)
{
    // create input string stream
    // create Lexer
    // parse the input string stream looking for name=value pairs
    // put the value in options

    m_options.clear();
    m_options = m_defaults;

    std::istringstream iss(str);

    Lexer lexer(m_name, &iss);

    Lexer::Token token = lexer.getToken();
    while (token.type)
    {
        // expect name=value
        if (token.type != SPI_TOKEN_TYPE_NAME)
            throw RuntimeError("%s is not a name", token.toString().c_str());

        std::string optionName = token.value.aName;

        if (m_options.count(optionName) == 0)
        {
            throw RuntimeError("%s is not an allowed option name",
                               optionName.c_str());
        }

        Value::Type expectedType = m_options[optionName].getType();

        token = lexer.getToken();
        if (token.type != '=')
            throw RuntimeError("Expecting = after name %s", optionName.c_str());

        token = lexer.getToken();

        switch(token.type)
        {
        case SPI_TOKEN_TYPE_INT:
            if (expectedType != Value::INT)
                throw TypeMismatch(optionName, expectedType, token);
            SetOptionValue(optionName, token.value.anInt, m_options);
            break;
        case SPI_TOKEN_TYPE_DATE:
            if (expectedType != Value::DATE)
                throw TypeMismatch(optionName, expectedType, token);
            SetOptionValue(optionName, Date(token.value.aDate), m_options);
            break;
        case SPI_TOKEN_TYPE_DOUBLE:
            if (expectedType != Value::DOUBLE)
                throw TypeMismatch(optionName, expectedType, token);
            SetOptionValue(optionName, token.value.aDouble, m_options);
            break;
        case SPI_TOKEN_TYPE_CHAR:
            if (expectedType != Value::CHAR)
                throw TypeMismatch(optionName, expectedType, token);
            SetOptionValue(optionName, token.value.aChar, m_options);
            break;
        case SPI_TOKEN_TYPE_STRING:
            if (expectedType != Value::STRING)
                throw TypeMismatch(optionName, expectedType, token);
            SetOptionValue(optionName, std::string(token.value.aString),
                           m_options);
            break;
        case SPI_TOKEN_TYPE_BOOL:
            if (expectedType != Value::BOOL)
                throw TypeMismatch(optionName, expectedType, token);
            SetOptionValue(optionName, token.value.aBool, m_options);
            break;
        case SPI_TOKEN_TYPE_NAME:
            if (expectedType != Value::STRING)
                throw TypeMismatch(optionName, expectedType, token);
            SetOptionValue(optionName, std::string(token.value.aName),
                           m_options);
            break;
        default:
            throw TypeMismatch(optionName, expectedType, token);
        }

        token = lexer.getToken();
    }
}

void OptionsParser::SetChar(const std::string& name, char value)
{
    SetOptionValue(name, value, m_defaults);
}

void OptionsParser::SetInt(const std::string& name, int value)
{
    SetOptionValue(name, value, m_defaults);
}

void OptionsParser::SetDouble(const std::string& name, double value)
{
    SetOptionValue(name, value, m_defaults);
}

void OptionsParser::SetDate(const std::string& name, Date value)
{
    SetOptionValue(name, value, m_defaults);
}

void OptionsParser::SetString(const std::string& name, const std::string& value)
{
    SetOptionValue(name, value, m_defaults);
}

void OptionsParser::SetBool(const std::string& name, bool value)
{
    SetOptionValue(name, value, m_defaults);
}

char OptionsParser::GetChar(const std::string& name) const
{
    return GetOptionValue<char>(name, m_options);
}

int OptionsParser::GetInt(const std::string& name) const
{
    return GetOptionValue<int>(name, m_options);
}

double OptionsParser::GetDouble(const std::string& name) const
{
    return GetOptionValue<double>(name, m_options);
}

Date OptionsParser::GetDate(const std::string& name) const
{
    return GetOptionValue<Date>(name, m_options);
}

std::string OptionsParser::GetString(const std::string& name) const
{
    return GetOptionValue<std::string>(name, m_options);
}

bool OptionsParser::GetBool(const std::string& name) const
{
    return GetOptionValue<bool>(name, m_options);
}


SPI_END_NAMESPACE

