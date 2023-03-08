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
#ifndef TEX_INDEX_HPP
#define TEX_INDEX_HPP

/**
 * Indexing functions to enable us to make forward references within the
 * generated user guide.
 */

#include <map>
#include <set>
#include <string>

class ServiceIndex
{
public:
    ServiceIndex();

    void AddDerivedClass(const std::string& baseClass, const std::string& derivedClass);
    void AddConstructor(const std::string& cls, const std::string& constructor);
    void IncrementNumClassMethods();

    const std::set<std::string>& DerivedClasses(const std::string& baseClass);
    const std::set<std::string>& Constructors(const std::string& cls);
    size_t NumClassMethods();

private:
    std::map<std::string, std::set<std::string> > m_indexBaseDerived;
    std::map<std::string, std::set<std::string> > m_indexClassConstructor;
    size_t m_numClassMethods;
};

#endif
