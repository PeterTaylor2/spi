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
#ifndef VERBATIM_HPP
#define VERBATIM_HPP

/*
***************************************************************************
** verbatim.hpp
***************************************************************************
** Describes a block of code to be included verbatim in the generated
** code. This will include the location of the code in the file (name
** and line number) plus the actual code itself.
***************************************************************************
*/

#include <vector>

#include <spi/Date.hpp>
#include <spi/Service.hpp>

SPI_DECLARE_RC_CLASS(Verbatim);

#include <spgtools/generatedOutput.hpp>

/**
 * Defines a verbatim block of code.
 */
class Verbatim : public spi::RefCounter
{
public:
    static VerbatimConstSP Make(
        const std::string&              sourceFilename,
        int                             lineNumber,
        const std::vector<std::string>& code);

    const std::vector<std::string>& getCode() const;
    const std::vector<std::string>& getIncludes() const;
    const std::string& getSourceFilename() const;
    int getLineNumber() const;

protected:
    Verbatim(
        const std::string&              sourceFilename,
        int                             lineNumber,
        const std::vector<std::string>& code);

public:
    std::string              m_sourceFilename;
    int                      m_lineNumber;
    std::vector<std::string> m_code;
    std::vector<std::string> m_includes;
};

#endif
