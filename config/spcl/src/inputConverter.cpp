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
** inputConverter.cpp
***************************************************************************
** Defines classes which can handle input conversion and validation.
***************************************************************************
*/

#include "inputConverter.hpp"

#include <spi/StringUtil.hpp>

#include <sstream>

InputConverterConstSP InputConverter::StringFormat(const std::string & format)
{
    return InputConverterStringFormat::Make(format);
}

InputConverterConstSP InputConverter::Class(const std::string& format)
{
    return InputConverterClass::Make(format);
}

InputConverterStringFormatConstSP InputConverterStringFormat::Make(const std::string & format)
{
    return InputConverterStringFormatConstSP(new InputConverterStringFormat(format));
}

std::string InputConverterStringFormat::Validation(const std::string & name, bool optional) const
{
    // we don't understand optionality for the regular case, e.g. int, double etc
    return std::string();
}

std::string InputConverterStringFormat::Conversion(const std::string & name) const
{
    return spi::StringFormat(m_format.c_str(), name.c_str());
}

InputConverterStringFormat::InputConverterStringFormat(const std::string & format)
    :
    m_format(format)
{
}

InputConverterClassConstSP InputConverterClass::Make(const std::string & format)
{
    return InputConverterClassConstSP(new InputConverterClass(format));
}

std::string InputConverterClass::Validation(const std::string & name, bool optional) const
{
    std::ostringstream code;

    if (!optional)
        code << "SPI_PRE_CONDITION(" << name << ")";

    return code.str();
}

std::string InputConverterClass::Conversion(const std::string & name) const
{
    if (m_format.empty())
        return name;

    return spi::StringFormat(m_format.c_str(), name.c_str());
}

InputConverterClass::InputConverterClass(const std::string & format)
    :
    m_format(format)
{
}
