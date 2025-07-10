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
#include "texUtils.hpp"

#include "generatedOutput.hpp"
#include "commonTools.hpp"

#include <spi_util/FileUtil.hpp>
#include <spi/RuntimeError.hpp>
#include <spi/StringUtil.hpp>

#include <algorithm>
#include <cstdarg>

std::string texFileName(
    const std::string& dirname,
    const char* filename,
    bool forImport)
{
    const char* tex = forImport ? "tex_imports" : "tex";
    return spi_util::path::join(dirname.c_str(), tex, filename, 0);
}

/**
 * Writes a list of \input files to the output directory.
 * Returns true if the file was updated.
 */
void writeTexInputList(
    GeneratedOutput& ostr,
    const std::vector<std::string>& inputs,
    bool sortInputs,
    bool addNewPage)
{
    std::vector<std::string> sortedInputs;
    if (sortInputs)
    {
        sortedInputs = inputs;
        std::sort(sortedInputs.begin(), sortedInputs.end());
    }
    const std::vector<std::string>& inputsUsed =
        sortInputs ? sortedInputs : inputs;

    ostr << "\n";
    if (inputsUsed.size() == 0)
    {
        ostr << "None\n";
    }
    else
    {
        for (size_t i = 0; i < inputsUsed.size(); ++i)
        {
            if (addNewPage)
                ostr << "\\newpage\n";
            ostr << "\\input{tex/" << inputsUsed[i] << "}\n";
        }
    }
}

/**
 * Adds escapes to plain text to enable it to be shown in tex.
 *
 * Any genuine escapes would end up being doubled up - so only use this
 * for plain text without prior escapes.
 *
 * Sometimes the latter problem is difficult to avoid - in that case put
 * '!' at the beginning of the text and this function strips of the '!'
 * and assumes that the rest of the text is in acceptable format already.
 */
std::string texEscape(const std::string& plainText, bool keepQuotes)
{
    if (plainText[0] == '!')
        return plainText.substr(1);

    std::ostringstream oss;

    bool inQuotes = false;

    const char* str = plainText.c_str();
    size_t len = plainText.length();
    for (size_t i = 0; i < len; ++i)
    {
        char c = str[i];
        switch (c)
        {
        case '\\':
            oss << "$\\backslash$";
            break;
        case '{':
        case '}':
        case '$':
        case '%':
        case '&':
        case '_':
        case '#':
            oss << "\\" << c;
            break;
        case '^':
            oss << "\\verb|" << c << "|";
            break;
        case '>':
        case '<':
            oss << '$' << c << '$';
            break;
        case '"':
            // note we only process double quotes since a single quote could
            // also be an apostrophe and we can't really be expected to
            // distinguish between them
            if (keepQuotes)
            {
                oss << '"';
            }
            else if (inQuotes)
            {
                oss << "''";
                inQuotes = false;
            }
            else
            {
                oss << "``";
                inQuotes = true;
            }
            break;
        default:
            oss << c;
            break;
        }
    }

    return oss.str();
}

std::string texValue(const std::string& varName)
{
    return spi::StringFormat("!\\texttt{\\$%s}", texEscape(varName).c_str());
}

/**
 * Write a block of description in Tex format to file.
 *
 * If the first line of the description starts with '!' then it is assumed
 * that the description has already been prepared in Tex format.
 *
 * Otherwise the text is considered as plain text and all tex characters
 * will be escaped out before printing.
 */
void writeTexDescription (
    GeneratedOutput& ostr,
    const std::vector<std::string>& description,
    bool firstParagraphOnly)
{
    if (description.size() == 0)
        return;

    if (firstParagraphOnly)
    {
        const std::vector< std::vector<std::string> > blocks =
            SplitDescription(description);
        SPI_POST_CONDITION(blocks.size() > 0);
        writeTexDescription(ostr, blocks[0]);
    }
    else
    {
        for (size_t i = 0; i < description.size(); ++i)
        {
            if (spi::StringStartsWith(description[i], "!"))
            {
                ostr << spi::StringStrip(description[i].substr(1), false) << "\n";
            }
            else
            {
                ostr << texEscape(description[i]) << "\n";
            }
        }
    }
}

/**
 * Write the beginning of a general table in Tex format.
 */
void writeTexBeginTable(
    GeneratedOutput& ostr,
    int nbColumns,
    ...)
{
    std::vector<int> widths;
    std::vector<std::string> headings;

    if (nbColumns <= 0)
        throw spi::RuntimeError("%s: No columns defined", __FUNCTION__);

    va_list args;

    va_start(args, nbColumns);

    try
    {
        for (int i = 0; i < nbColumns; ++i)
        {
            int width = va_arg(args, int);
            const char* heading = va_arg(args, const char*);

            widths.push_back(width);
            headings.push_back(std::string(heading));
        }
    }
    catch (...)
    {
        va_end(args);
        throw;
    }
    va_end(args);

    ostr << "\n\n"
         << "\\small\n"
         << "\n"
         << "\\begin{longtable}{";

    for (int i = 0; i < nbColumns; ++i)
    {
        ostr << "|p{" << widths[i] << "pt}";
    }

    ostr << "|l}\n"
         << "\n"
         << "\\cline{1-" << nbColumns << "} ";

    for (int i = 0; i < nbColumns; ++i)
    {
        ostr << "\\textbf{" << headings[i] << "} & ";
    }

    ostr << "\n"
         << "\\endfirsthead\n"
         << "\n"
         << "\\cline{1-" << nbColumns << "} ";

    for (int i = 0; i < nbColumns; ++i)
    {
        ostr << "\\textbf{" << headings[i] << "} & ";
    }

    ostr << "\\\\\n"
         << "\\cline{1-" << nbColumns << "} "
         << "\\endhead\n"
         << "\n"
         << "\\cline{1-" << nbColumns << "} \\multicolumn{" << nbColumns
         << "}{r}{{Continued on next page}}\n"
         << "\\endfoot\n"
         << "\n"
         << "\\cline{1-" << nbColumns << "}\n"
         << "\\endlastfoot\n";
}


void writeTexTableRow(
    GeneratedOutput& ostr,
    int nbColumns,
    ...)
{
    std::vector<std::string> columnEntries;

    if (nbColumns <= 0)
        throw spi::RuntimeError("%s: No columns defined", __FUNCTION__);

    va_list args;

    va_start(args, nbColumns);

    try
    {
        for (int i = 0; i < nbColumns; ++i)
        {
            const char* columnEntry = va_arg(args, const char*);
            columnEntries.push_back(std::string(columnEntry));
        }
    }
    catch (...)
    {
        va_end(args);
        throw;
    }
    va_end(args);

    ostr << "\n"
         << "\\cline{1-" << nbColumns << "} ";

    for (int i = 0; i < nbColumns; ++i)
    {
        if (i > 0)
            ostr << " & ";

        ostr << texEscape(columnEntries[i]);
    }

    ostr << " \\\\ \n";
}

/**
 * Writes the end of a table in Tex format.
 */
void writeTexEndTable(GeneratedOutput& ostr, int nbColumns)
{
    ostr << "\\cline{1-" << nbColumns << "}\n\n"
         << "\\end{longtable}\n"
         << "\\normalsize\n";
}

/**
 * Writes the beginning of a specific options table in Tex format.
 */
void writeTexBeginOptionsTable(
    GeneratedOutput& ostr)
{
    ostr << "\n\n"
         << "\\small\n"
         << "\n"
         << "\\begin{longtable}{|p{80pt}|p{150pt}|p{140pt}|l}\n"
         << "\n"
         << "\\cline{1-3} \\textbf{Name} & \\textbf{Default} & \\textbf{Range} & \n"
         << "\\endfirsthead\n"
         << "\n"
         << "\\cline{1-3} \\textbf{Name} & \\textbf{Default} & \\textbf{Range} & \n"
         << "\\endhead\n"
         << "\n"
         << "\\cline{1-3} \\multicolumn{3}{r}{{Continued on next page}}\n"
         << "\\endfoot\n"
         << "\n"
         << "\\cline{1-3}\n"
         << "\\endlastfoot\n";
}

/**
 * Write one row of an options table.
 */
void writeTexOptionsTableRow(
    GeneratedOutput& ostr,
    const std::string& name,
    const std::string& defaultValue,
    const std::string& range)
{
    ostr << "\n"
         << "\\cline{1-3} \\texttt{" << texEscape(name) << "} & "
         << texEscape(defaultValue) << " & "
         << texEscape(range) << " \\\\ \n";
}

/**
 * Writes the end of a table in Tex format.
 */
void writeTexEndOptionsTable(GeneratedOutput& ostr)
{
    ostr << "\\cline{1-3}\n\n"
         << "\\end{longtable}\n"
         << "\\normalsize\n";
}

/**
 * Writes the beginning of a block of syntax description.
 *
 * Syntax is pretty much shown verbatim but with control sequences supported.
 * It uses the fancyvrb package.
 */
void writeTexBeginSyntax(GeneratedOutput& ostr, bool usesDescription)
{
    ostr << "\n"
         << "\\textbf{SYNTAX:}\n"
         << "\\nopagebreak\n"
         << "\\begin{Verbatim}[commandchars=\\\\\\{\\}]\n";

    // the choice of command chars means that \{} are treated as command
    // characters within the Verbatim block - there is double escaping
    // which is where there are so many \'s

    if (usesDescription)
    {
        ostr << "    \\emph{/** description */}\n";
    }
}

/**
 * Writes the end of a block of syntax description.
 */
void writeTexEndSyntax(GeneratedOutput& ostr)
{
    ostr << "\\end{Verbatim}\n";
}
