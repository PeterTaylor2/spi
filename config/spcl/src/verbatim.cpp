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
** verbatim.cpp
***************************************************************************
** Describes a block of code to be included verbatim in the generated
** code. This will include the location of the code in the file (name
** and line number) plus the actual code itself.
***************************************************************************
*/

#include "verbatim.hpp"

#include <spi/RuntimeError.hpp>
#include <spi/Service.hpp>
#include <spi/StringUtil.hpp>

#include "generatorTools.hpp"



/*
***************************************************************************
** Implementation of Verbatim
***************************************************************************
*/
VerbatimConstSP Verbatim::Make(
    const std::string&              sourceFilename,
    int                             lineNumber,
    const std::vector<std::string>& code)
{
    return new Verbatim(sourceFilename, lineNumber, code);
}

Verbatim::Verbatim(
    const std::string&              sourceFilename,
    int                             lineNumber,
    const std::vector<std::string>& code)
    :
    m_sourceFilename(sourceFilename),
    m_lineNumber(lineNumber),
    m_code(),
    m_includes()
{
    size_t nbLines = code.size();
    bool addToCode = false;
    for (size_t i = 0; i < nbLines; ++i)
    {
        const std::string& line = code[i];
        if (addToCode)
        {
            m_code.push_back(line);
            continue;
        }
        const std::string& stripLine = spi::StringStrip(line);
        if (stripLine.length() == 0)
        {
            ++m_lineNumber; // strip leading blanks
        }
        else if (spi::StringStartsWith(stripLine, "#include "))
        {
            m_includes.push_back(line);
            ++m_lineNumber; // #include at the start goes into m_includes
        }
        else
        {
            addToCode = true;
            m_code.push_back(line);
        }
    }
}

const std::vector<std::string>& Verbatim::getCode() const
{
    return m_code;
}

const std::vector<std::string>& Verbatim::getIncludes() const
{
    return m_includes;
}

const std::string& Verbatim::getSourceFilename() const
{
    return m_sourceFilename;
}

int Verbatim::getLineNumber() const
{
    return m_lineNumber;
}
