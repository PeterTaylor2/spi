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
#ifndef INPUT_CONVERTER_HPP
#define INPUT_CONVERTER_CPP

/*
***************************************************************************
** inputConverter.hpp
***************************************************************************
** Defines classes which can handle input conversion and validation.
***************************************************************************
*/

#include <spi/RefCounter.hpp>

SPI_DECLARE_RC_CLASS(InputConverter);
SPI_DECLARE_RC_CLASS(InputConverterStringFormat);
SPI_DECLARE_RC_CLASS(InputConverterClass);

/**
 * Input converter interface class.
 */
class InputConverter : public spi::RefCounter
{
public:
    virtual ~InputConverter()
    {}

    virtual std::string Validation(const std::string& name, bool optional) const = 0;
    virtual std::string Conversion(const std::string& name) const = 0;
    virtual bool isEqual(const InputConverterConstSP& other) const = 0;
    static InputConverterConstSP StringFormat(const std::string& format);
    static InputConverterConstSP Class(const std::string& format);
};

class InputConverterStringFormat : public InputConverter
{
public:
    static InputConverterStringFormatConstSP Make(const std::string& format);
    std::string Validation(const std::string& name, bool optional) const;
    std::string Conversion(const std::string& name) const;
    bool isEqual(const InputConverterConstSP& other) const;

private:
    InputConverterStringFormat(const std::string& format);

    std::string m_format;

public:
    const std::string format() const { return m_format; }
};

class InputConverterClass : public InputConverter
{
public:
    static InputConverterClassConstSP Make(
        const std::string& format);

    std::string Validation(const std::string& name, bool optional) const;
    std::string Conversion(const std::string& name) const;
    bool isEqual(const InputConverterConstSP& other) const;

private:
    InputConverterClass(const std::string& format);

    std::string m_format;

public:
    const std::string format() const { return m_format; }
};

#endif
