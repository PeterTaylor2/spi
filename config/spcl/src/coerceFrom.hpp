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
#ifndef COERCE_FROM_HPP
#define COERCE_FROM_HPP

/*
***************************************************************************
** coerceFrom.hpp
***************************************************************************
** Defines the CoerceFrom class which describes how we can coerce an object
** from some other type
***************************************************************************
*/

#include <vector>

#include "dataType.hpp"

class GeneratedOutput;

SPI_DECLARE_RC_CLASS(Attribute);
SPI_DECLARE_RC_CLASS(Verbatim);
SPI_DECLARE_RC_CLASS(CoerceFrom);
SPI_DECLARE_RC_CLASS(ServiceDefinition);

SPDOC_BEGIN_NAMESPACE
SPI_DECLARE_OBJECT_CLASS(CoerceFrom);
SPDOC_END_NAMESPACE

/**
 * Defines an object coerce from object.
 *
 * This consists of the following:
 *    attribute
 *    code
 */
class CoerceFrom : public spi::RefCounter
{
public:
    static CoerceFromConstSP Make(
        const std::vector<std::string>& description,
        const AttributeConstSP& coerceFrom,
        const VerbatimConstSP&  code,
        bool convert);

    const DataTypeConstSP& getDataType() const;
    const std::string& getName() const;
    int arrayDim() const;
    const VerbatimConstSP& getCode() const;

    void declare(GeneratedOutput& ostr,
        const std::string& className,
        const ServiceDefinitionConstSP& svc) const;

    void implement(GeneratedOutput& ostr,
        const std::string& className,
        const ServiceDefinitionConstSP& svc) const;

    static bool implement(
        GeneratedOutput& ostr,
        const std::string& className,
        const ServiceDefinitionConstSP& svc,
        const std::vector<CoerceFromConstSP>& coerceFrom);

    spdoc::CoerceFromConstSP getDoc() const;

private:
    CoerceFrom(const std::vector<std::string>& description,
               const AttributeConstSP& coerceFrom,
               const VerbatimConstSP& code,
               bool convert);

    std::vector<std::string> m_description;
    AttributeConstSP m_coerceFrom;
    VerbatimConstSP  m_code;
    bool m_convert;
};

#endif
