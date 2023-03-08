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
#ifndef SPI_OPTIONS_PARSER_HPP
#define SPI_OPTIONS_PARSER_HPP

#include "Value.hpp"
#include "Map.hpp"

SPI_BEGIN_NAMESPACE

class SPI_IMPORT OptionsParser
{
public:
    OptionsParser(const std::string& name);

    void Parse(const char* str);

    // call these functions before calling Parse to set the defaults
    void SetChar(const std::string& name, char value);
    void SetInt(const std::string& name, int value);
    void SetDouble(const std::string& name, double value);
    void SetDate(const std::string& name, Date value);
    void SetString(const std::string& name, const std::string& value);
    void SetBool(const std::string& name, bool value);

    // call these functions after calling Parse to extract the values to use
    char GetChar(const std::string& name) const;
    int GetInt(const std::string& name) const;
    double GetDouble(const std::string& name) const;
    Date GetDate(const std::string& name) const;
    std::string GetString(const std::string& name) const;
    bool GetBool(const std::string& name) const;

private:
    std::string                  m_name;
    std::map<std::string, Value> m_options;
    std::map<std::string, Value> m_defaults;

    OptionsParser(const OptionsParser&);
    OptionsParser& operator=(const OptionsParser&);
};

SPI_END_NAMESPACE

#endif
