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
#ifndef CONVERTER_HPP
#define CONVERTER_HPP

/*
***************************************************************************
** converter.hpp
***************************************************************************
** Describes a block of code which can be used to convert a previous
** attribute (in a serialized object) to a new attribute
**
** This enables older objects to be read by new code
**
** The old attribute may be redefined as a property but that is a decision
** for the library developer
**
** We need to parse something like this:
** convert(DataType_old old_name) { ... }
**
** The generated code in the from_map function will look like this:
**
** DataType_new new_name;
** if (!obj_map->Exists(new_name))
** {
**     DataType_old old_name = obj_map->Get(old_name);
**     converter code; // defines new_name
** }
** else
** {
**     new_name = obj_map->Get(new_name);
** }
***************************************************************************
*/

#include <vector>

#include <spi/Date.hpp>
#include <spi/Service.hpp>

SPI_DECLARE_RC_CLASS(Attribute);
SPI_DECLARE_RC_CLASS(Converter);
SPI_DECLARE_RC_CLASS(Verbatim);

#include <spgtools/generatedOutput.hpp>

/**
 * Defines an attribute converter.
 */
class Converter : public spi::RefCounter
{
public:
    static ConverterConstSP Make(
        const std::vector<AttributeConstSP>& attributes,
        const VerbatimConstSP& code);

    const std::vector<AttributeConstSP>& attributes() const;
    const VerbatimConstSP code() const;

protected:
    Converter(
        const std::vector<AttributeConstSP>& attribute,
        const VerbatimConstSP& converterCode);

public:
    std::vector<AttributeConstSP> m_attributes;
    VerbatimConstSP m_code;
};

#endif
