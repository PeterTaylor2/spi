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
** construct.cpp
***************************************************************************
** Implements the Construct base class
***************************************************************************
*/

#include "construct.hpp"

/*
***************************************************************************
** Implementation of Construct
***************************************************************************
*/
Construct::Construct()
{}

void Construct::declareInner(GeneratedOutput& ostr,
                             NamespaceManager& nsman,
                             const ServiceDefinitionSP& svc) const
{}

int Construct::preDeclare(GeneratedOutput& ostr,
                           const ServiceDefinitionSP& svc) const
{
    return 0;
}

void Construct::declareClassFunctions(GeneratedOutput& ostr, const ServiceDefinitionSP& svc) const
{
    // null operator except for classes
}

void Construct::declareTypeConversions(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc) const
{}

void Construct::writeInnerHeader(
    GeneratedOutput& ostr) const
{}

void Construct::writeInnerPreDeclaration(
    GeneratedOutput& ostr,
    NamespaceManager& nsm) const
{}

void Construct::SplitTypedef(const std::string& td,
    std::string& td1,
    std::string& td2)
{
    size_t pos = td.find_first_of(':');
    if (pos != std::string::npos)
    {
        td1 = spi::StringStrip(td.substr(0, pos));
        td2 = std::string(" ") + td.substr(pos); // includes :
    }
    else
    {
        td1 = td;
        td2 = "";
    }
}


