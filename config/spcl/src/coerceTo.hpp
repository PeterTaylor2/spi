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
#ifndef COERCE_TO_HPP
#define COERCE_TO_HPP

/*
***************************************************************************
** coerceTo.hpp
***************************************************************************
** Defines the CoerceTo class which describes how we can coerce an object
** To some other type
***************************************************************************
*/

#include <vector>

#include "dataType.hpp"

class GeneratedOutput;

SPI_DECLARE_RC_CLASS(Class);
SPI_DECLARE_RC_CLASS(Verbatim);
SPI_DECLARE_RC_CLASS(CoerceTo);
SPI_DECLARE_RC_CLASS(ServiceDefinition);

SPDOC_BEGIN_NAMESPACE
SPI_DECLARE_OBJECT_CLASS(CoerceTo);
SPDOC_END_NAMESPACE

/**
 * Defines an object coerce to object.
 *
 * This consists of the following:
 *    class
 *    code
 */
class CoerceTo : public spi::RefCounter
{
public:
    static CoerceToConstSP Make(
        const std::vector<std::string>& description,
        const DataTypeConstSP& targetType,
        const VerbatimConstSP& code);

    const DataTypeConstSP& getTargetType() const;
    const VerbatimConstSP& getCode() const;

    void declare(GeneratedOutput& ostr,
        const std::string& className) const;

    void implement(GeneratedOutput& ostr,
        const std::string& className) const;

    static void declare(GeneratedOutput& ostr,
        const std::string& className,
        const std::vector<CoerceToConstSP>& coerceTo);

    static void implement(
        GeneratedOutput& ostr,
        const std::string& className,
        const std::vector<CoerceToConstSP>& coerceTo);

    spdoc::CoerceToConstSP getDoc() const;

private:
    CoerceTo(const std::vector<std::string>& description,
             const DataTypeConstSP& targetType,
             const VerbatimConstSP& code);

    std::vector<std::string> m_description;
    DataTypeConstSP  m_targetType;
    VerbatimConstSP  m_code;
};

#endif
