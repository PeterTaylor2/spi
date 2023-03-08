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
** coerceTo.cpp
***************************************************************************
*/

#include "coerceTo.hpp"

#include <spi/RuntimeError.hpp>
#include <spi/Service.hpp>
#include <spi/StringUtil.hpp>
#include <spi/spdoc_configTypes.hpp>

#include "classMethod.hpp"
#include "constant.hpp"
#include "construct.hpp"
#include "innerClass.hpp"
#include "moduleDefinition.hpp"
#include "serviceDefinition.hpp"
#include "verbatim.hpp"
#include "generatorTools.hpp"

CoerceToConstSP CoerceTo::Make(
    const std::vector<std::string>& description,
    const DataTypeConstSP& targetType,
    const VerbatimConstSP& code)
{
    return CoerceToConstSP(new CoerceTo(description, targetType, code));
}

CoerceTo::CoerceTo(
    const std::vector<std::string>& description,
    const DataTypeConstSP& targetType,
    const VerbatimConstSP& code)
    :
    m_description(description),
    m_targetType(targetType),
    m_code(code)
{
    if (targetType->publicType() != spdoc::PublicType::CLASS)
        throw spi::RuntimeError("Can only CoerceTo a class");
}

const DataTypeConstSP& CoerceTo::getTargetType() const
{
    return m_targetType;
}

const VerbatimConstSP& CoerceTo::getCode() const
{
    return m_code;
}

void CoerceTo::declare(
    GeneratedOutput& ostr,
    const std::string& className) const
{
    writeStartCommentBlock(ostr, true, 4);
    ostr << "    * Converts to " << m_targetType->name() << "\n";
    if (m_description.size() > 0)
    {
        ostr << "    *\n";
        writeComments(ostr, m_description, 0, 4);
    }
    writeEndCommentBlock(ostr, 4);

    ostr << "    operator " << m_targetType->outerValueType() << "() const;\n";
}

void CoerceTo::implement(GeneratedOutput& ostr,
    const std::string& className) const
{
    ostr << "\n"
         << className << "::operator " << m_targetType->outerValueType() << "() const\n"
         << "{";
    writeVerbatim(ostr, m_code);
}

void CoerceTo::declare(
    GeneratedOutput& ostr,
    const std::string& className,
    const std::vector<CoerceToConstSP>& coerceTo)
{
    if (coerceTo.size() == 0)
        return;

    ostr << "\n"
         << "    spi::ObjectConstSP " << "coerce_to_object(\n"
         << "        const std::string& className) const;\n";
}

void CoerceTo::implement(
    GeneratedOutput& ostr,
    const std::string& className,
    const std::vector<CoerceToConstSP>& coerceTo)
{
    if (coerceTo.size() == 0)
        return;

    ostr << "\n"
         << "spi::ObjectConstSP " << className << "::coerce_to_object(\n"
         << "    const std::string& className) const\n"
         << "{";

    for (size_t i = 0; i < coerceTo.size(); ++i)
    {
        const CoerceToConstSP& item = coerceTo[i];
        ostr << "\n"
             << "    if (className == \""
             << item->m_targetType->objectNameAlias() << "\")\n"
             << "        return operator "
             << item->m_targetType->outerValueType() << "();\n";
    }

    ostr << "\n"
         << "    return spi::ObjectConstSP();\n"
         << "}\n";
}

spdoc::CoerceToConstSP CoerceTo::getDoc() const
{
    return spdoc::CoerceTo::Make(
        m_description,
        m_targetType->name(),
        m_targetType->getDoc());
}
