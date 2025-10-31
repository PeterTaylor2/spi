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
** verbatimConstruct.cpp
***************************************************************************
** Implements the VerbatimConstruct class.
***************************************************************************
*/

#include "verbatimConstruct.hpp"
#include "function.hpp"

#include <spi/RuntimeError.hpp>
#include <spi/Service.hpp>
#include <spi/StringUtil.hpp>
#include <spi/spdoc_configTypes.hpp>

#include "classMethod.hpp"
#include "constant.hpp"
#include "cppType.hpp"
#include "innerClass.hpp"
#include "moduleDefinition.hpp"
#include "serviceDefinition.hpp"
#include "verbatim.hpp"
#include "generatorTools.hpp"

/*
***************************************************************************
** Implementation of VerbatimConstruct
***************************************************************************
*/
VerbatimConstructSP VerbatimConstruct::Make(
    const VerbatimConstSP& verbatim)
{
    return new VerbatimConstruct(verbatim);
}

VerbatimConstruct::VerbatimConstruct(
    const VerbatimConstSP& verbatim)
    :
    m_verbatim(verbatim)
{
}

void VerbatimConstruct::declare(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
}

bool VerbatimConstruct::declareInClasses() const
{
    return false; // no declarations either way...
}

void VerbatimConstruct::declareHelper(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
}

void VerbatimConstruct::implement(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    writeVerbatim(ostr, m_verbatim, 0, true);
}

void VerbatimConstruct::implementHelper(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
}

void VerbatimConstruct::implementRegistration(
    GeneratedOutput& ostr,
    const char* serviceName,
    bool types) const
{
}

spdoc::ConstructConstSP VerbatimConstruct::getDoc() const
{
    return spdoc::ConstructConstSP();
}

