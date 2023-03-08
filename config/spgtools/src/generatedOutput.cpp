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
#include "generatedOutput.hpp"

#include <algorithm>
#include <sstream>

#include <spi_util/FileUtil.hpp>
#include <spi/StringUtil.hpp>
#include <spi/RuntimeError.hpp>

#include <fstream>
#include <iostream>

#include <string.h>

namespace {
    
void CheckDirectory(const std::string& cwd)
{
    if (!spi_util::path::isdir(cwd))
    {
        throw spi::RuntimeError("Directory '%s' does not exist",
            cwd.c_str());
    }
}

} // end of anonymous namespace

GeneratedOutput::GeneratedOutput(const std::string& filename,
                                 const std::string& cwd)
    :
    m_filename(filename),
    m_cwd(cwd),
    m_oss(),
    m_lines(0),
    m_closed(false)
{
    CheckDirectory(m_cwd);
}

GeneratedOutput::GeneratedOutput(const std::string& filename)
    :
    m_filename(filename),
    m_cwd(spi_util::path::dirname(filename)),
    m_oss(),
    m_lines(0),
    m_closed(false)
{
    CheckDirectory(m_cwd);
}

GeneratedOutput::~GeneratedOutput()
{
    close();
}

GeneratedOutput& GeneratedOutput::operator << (char c)
{
    testClosure();

    if (c == '\n')
        ++m_lines;

    m_oss << c;

    return *this;
}

GeneratedOutput& GeneratedOutput::operator << (const char* str)
{
    testClosure();

    const char* pos = strchr(str, '\n');
    while (pos != NULL)
    {
        ++m_lines;
        pos = strchr(pos+1, '\n');
    }

    m_oss << str;

    return *this;
}

GeneratedOutput& GeneratedOutput::operator << (const std::string& str)
{
    return operator << (str.c_str());
}

size_t GeneratedOutput::lines() const
{
    return m_lines;
}

const std::string& GeneratedOutput::filename() const
{
    return m_filename;
}

bool GeneratedOutput::close()
{
    if (m_closed)
        return false;

    std::string newContents = m_oss.str();

    bool writeMe = writeFileIfChanged(m_filename.c_str(), newContents);

    return writeMe;
}

void GeneratedOutput::testClosure() const
{
    if (m_closed)
    {
        throw spi::RuntimeError("OutputStream for %s has been closed",
                                m_filename.c_str());
    }
}

std::string GeneratedOutput::relativePath(const std::string& filename)
{
    return spi_util::path::relativePath(filename, m_cwd);
}


std::string headerGuardMacroName(const std::string& fn)
{
    std::string bnu = spi::StringUpper(spi_util::path::basename(fn));
    std::replace(bnu.begin(),bnu.end(), '.', '_');
    std::ostringstream ostr;

    ostr << "_" << bnu << "_";

    return ostr.str();
}

void writeStartCommentBlock(
    GeneratedOutput& ostr,
    bool doxygenate,
    size_t indent,
    size_t vIndent)
{
    if (vIndent)
        ostr << std::string(vIndent, '\n');
    if (indent > 40)
        indent = 40;
    std::string indentString(indent, ' ');
    ostr << indentString << "/*";
    if (doxygenate)
        ostr << "*";
    ostr << "\n" << indentString << std::string(76-indent, '*') << '\n';
}

void writeEndCommentBlock(GeneratedOutput& ostr, size_t indent)
{
    if (indent > 40)
        indent = 40;
    std::string indentString(indent, ' ');
    ostr << indentString << std::string(76-indent, '*') << "\n"
         << indentString << "*/"
         << '\n';
}

void writeComments(
    GeneratedOutput& ostr,
    const std::vector<std::string>& text,
    size_t indent,
    size_t hIndent)
{
    std::string indentString(indent, ' ');
    std::string hIndentString(hIndent, ' ');
    bool blankLine = false;
    for (size_t i = 0; i < text.size(); ++i)
    {
        const std::string line = spi::StringStrip(text[i], false, true);
        if (line.empty())
        {
            blankLine = true;
        }
        else if (line[0] == '!')
        {
            // lines beginning with '!' are for tex documents only
        }
        else
        {
            if (blankLine)
            {
                ostr << hIndentString << "*\n";
                blankLine = false;
            }
            ostr << hIndentString << "* " << indentString << text[i] << "\n";
        }
    }
}

void writeGeneratedCodeNotice(GeneratedOutput& ostr, const std::string& ofn)
{
    writeStartCommentBlock(ostr, false);
    ostr << "* Generated code - do not edit\n";
    writeEndCommentBlock(ostr);
}

void startHeaderFile(
    GeneratedOutput& ostr,
    const std::string& fn)
{
    const std::string& headerGuard = headerGuardMacroName(fn);
    ostr << "#ifndef " << headerGuard << "\n"
         << "#define " << headerGuard << "\n";

    writeStartCommentBlock(ostr, true);
    ostr << "* Header file: " << spi_util::path::basename(fn) << "\n";
    writeEndCommentBlock(ostr);
}

void endHeaderFile(
    GeneratedOutput& ostr,
    const std::string& fn)
{
    const std::string& headerGuard = headerGuardMacroName(fn);
    ostr << "\n#endif /* " << headerGuard << "*/\n" << '\n';
}

void startSourceFile(
    GeneratedOutput& ostr,
    const std::string& ofn,
    bool doxygenate)
{
    writeStartCommentBlock(ostr, doxygenate, 0, 0);
    ostr << "* Source file: " << spi_util::path::basename(ofn) << "\n";
    writeEndCommentBlock(ostr);
}

void endSourceFile(
    GeneratedOutput& ostr,
    const std::string& ofn)
{
    ostr << "\n";
}


bool writeFileIfChanged(
    const char*        filename,
    const std::string& newContents)
{
    bool writeFile = false;

    std::ifstream original(filename);
    if (original.good())
    {
        std::stringstream buffer;
        buffer << original.rdbuf();
        original.close();
        std::string oldContents = buffer.str();

        const char* oldc = oldContents.c_str();
        const char* newc = newContents.c_str();

        // loop through but skip '\r'
        bool diff = false;
        do
        {
            while (*oldc == '\r')
                ++oldc;

            while (*newc == '\r')
                ++newc;

            if (*oldc != *newc)
            {
                diff = true;
                break;
            }

            ++oldc;
            ++newc;
        } while (*oldc && *newc);

        if (*oldc || *newc)
            diff = true;

        writeFile = diff;
    }
    else
    {
        writeFile = true;
    }

    if (writeFile)
    {
        std::cout << filename << std::endl;
        std::ofstream ostr(filename);
        if (!ostr.good())
        {
            throw spi::RuntimeError("Could not open %s for writing",
                                    filename);
        }
        ostr.write(newContents.c_str(), newContents.length());
        ostr.close();
    }

    return writeFile;
}
