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
** converter.cpp
***************************************************************************
** Implements the Converter class used in the configuration parser.
***************************************************************************
*/

#include "converter.hpp"

#include "attribute.hpp"
#include "verbatim.hpp"

/*
***************************************************************************
** Implementation of Converter
***************************************************************************
*/
ConverterConstSP Converter::Make(
    const std::vector<AttributeConstSP>& attributes,
    const VerbatimConstSP& code)
{
    return new Converter(attributes, code);
}

Converter::Converter(
    const std::vector<AttributeConstSP>& attributes,
    const VerbatimConstSP& code)
    :
    m_attributes(attributes),
    m_code(code)
{
}

const std::vector<AttributeConstSP>& Converter::attributes() const
{
    return m_attributes;
}

const VerbatimConstSP Converter::code() const
{
    return m_code;
}
