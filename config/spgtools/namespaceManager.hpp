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
#ifndef SPGTOOLS_NAMESPACE_MANAGER_HPP
#define SPGTOOLS_NAMESPACE_MANAGER_HPP

#include <sstream>
#include <vector>

class GeneratedOutput;

/**
 * Namespace manager class.
 *
 * Manages namespaces in c++ context.
 */
class NamespaceManager
{
public:
    NamespaceManager();

    void startNamespace(GeneratedOutput& ostr,
                        const std::string& ns, size_t indent=4);
    void endAllNamespaces(GeneratedOutput& ostr);
    void indent(GeneratedOutput& ostr);
    size_t indentLevel() const;

private:
    std::vector<std::string> m_namespaces;
    std::vector<size_t> m_indentLevels;

    NamespaceManager(const NamespaceManager&);
    NamespaceManager& operator=(const NamespaceManager&);
};

/**
 * Given a name this function will strip off the parts of the name that are
 * in common with the namespace. Thus if you are already in the namespace you
 * don't need to use quite so much of the name in order to use it correctly.
 */
std::string removeCommonNamespace(
    const std::string& name,
    const std::string& ns);

/**
 * Given a namespace which might be empty, this function either returns an
 * empty string if the namespace is empty, or else it returns the namespace
 * followed by the given separator.
 */
std::string makeNamespaceSep(const std::string& ns, const char* sep);

#endif
