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
/**
 * Indexing functions to enable us to make forward references within the
 * generated user guide.
 */

#include "texIndex.hpp"

#include <spgtools/generatedOutput.hpp>
#include <spgtools/commonTools.hpp>

#include <spi_util/FileUtil.hpp>
#include <spi/RuntimeError.hpp>
#include <spi/StringUtil.hpp>

#include <algorithm>

typedef std::vector<std::string, std::set<std::string> > Index;

ServiceIndex::ServiceIndex()
:
m_indexBaseDerived(),
m_indexClassConstructor(),
m_numClassMethods(0)
{}

void ServiceIndex::AddDerivedClass(
    const std::string& baseClass,
    const std::string& derivedClass)
{
    m_indexBaseDerived[baseClass].insert(derivedClass);
}

void ServiceIndex::AddConstructor(
    const std::string& cls,
    const std::string& constructor)
{
    m_indexClassConstructor[cls].insert(constructor);
}

void ServiceIndex::IncrementNumClassMethods()
{
    ++m_numClassMethods;
}

const std::set<std::string>& ServiceIndex::DerivedClasses(
    const std::string& baseClass)
{
    return m_indexBaseDerived[baseClass];
}

const std::set<std::string>& ServiceIndex::Constructors(
    const std::string& cls)
{
    return m_indexClassConstructor[cls];
}

size_t ServiceIndex::NumClassMethods()
{
    return m_numClassMethods;
}

