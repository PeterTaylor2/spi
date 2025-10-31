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
#include "namespaceManager.hpp"

#include "generatedOutput.hpp"

#include <algorithm>
#include <sstream>

#include <spi/StringUtil.hpp>
#include <spi/RuntimeError.hpp>

#include <fstream>
#include <iostream>

NamespaceManager::NamespaceManager()
    :
    m_namespaces(),
    m_indentLevels()
{}

void NamespaceManager::startNamespace(
    GeneratedOutput& ostr,
    const std::string& ns,
    size_t indent)
{
    SPI_PRE_CONDITION(m_namespaces.size() == m_indentLevels.size());

    std::vector<std::string> namespaces;

    if (!ns.empty())
        namespaces = spi::StringSplit(ns, "::");

    size_t nbNew = namespaces.size();
    size_t nbOld = m_namespaces.size();
    size_t nbCommon = 0;

    for (size_t i = 0; i < nbNew && i < nbOld; ++i)
    {
        if (namespaces[i].empty())
            throw spi::RuntimeError("Badly defined namespace %s", ns.c_str());

        if (namespaces[i] == m_namespaces[i])
            ++nbCommon;
        else
            break;
    }

    while (nbOld > nbCommon)
    {
        SPI_POST_CONDITION(nbOld == m_namespaces.size());

        std::string nsEnd = m_namespaces.back();
        m_indentLevels.pop_back();
        std::string nsIndent(indentLevel(), ' ');

        ostr << nsIndent << "} // end of namespace " << nsEnd << "\n";

        --nbOld;
        m_namespaces.pop_back();
    }

    if (nbNew > nbCommon)
    {
        // this is the general rule that when we start some new code we add
        // a newline, and we don't add a newline when we end some code
        //
        // however if we are starting more than one namespace (since it is
        // a compound namespace) we don't want newlines within each block
        ostr << "\n";
        while (nbNew > nbCommon)
        {
            SPI_POST_CONDITION(nbCommon == m_namespaces.size());

            const std::string& nsBegin = namespaces[nbCommon];
            std::string nsIndent(indentLevel(), ' ');

            ostr << nsIndent << "namespace " << nsBegin << " {\n";

            ++nbCommon;
            m_namespaces.push_back(nsBegin);
            m_indentLevels.push_back(indentLevel() + indent);
        }
    }
    SPI_POST_CONDITION(nbCommon == m_namespaces.size());
    SPI_POST_CONDITION(nbCommon == m_indentLevels.size());
}

void NamespaceManager::endAllNamespaces(GeneratedOutput& ostr)
{
    // exactly equivalent to starting an empty namespace
    startNamespace(ostr, "");
}

size_t NamespaceManager::indentLevel() const
{
    if (m_indentLevels.size() == 0)
        return 0;

    return m_indentLevels.back();
}

void NamespaceManager::indent(GeneratedOutput& ostr)
{
    ostr << std::string(indentLevel(), ' ');
}


std::string removeCommonNamespace(
    const std::string& name,
    const std::string& ns)
{
    if (ns.empty())
        return name;

    std::vector<std::string> nameParts = spi::StringSplit(name, "::");
    std::vector<std::string> nsParts   = spi::StringSplit(ns, "::");

    size_t nbCommon;
    size_t nbNameParts = nameParts.size();
    size_t nbNsParts   = nsParts.size();

    nbCommon = 0;
    for (size_t i = 0; i < nbNsParts && i+1 < nbNameParts; ++i)
    {
        if (nameParts[i] != nsParts[i])
            break;
        ++nbCommon;
    }

    if (nbCommon == 0)
        return name;

    SPI_PRE_CONDITION(nbCommon < nbNameParts);

    return spi::StringJoin("::", nameParts, nbCommon);
}


/**
 * Given a namespace which might be empty, this function either returns an
 * empty string if the namespace is empty, or else it returns the namespace
 * followed by the given separator.
 */
std::string makeNamespaceSep(const std::string& ns, const char* sep)
{
    if (ns.empty())
        return std::string();

    return ns + sep;
}

