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
#include <cctype>
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
  *
  * Changes: '^' no longer uses \verb, since \verb is not allowed inside the
  * argument of a macro such as \optrow; '~' and '|' are now escaped as well
  * (an unescaped ~ is a non-breaking space in tex and | does not print
  * properly in the default font encoding).
  */
std::string texEscape(const std::string& plainText, bool keepQuotes)
{
    if (!plainText.empty() && plainText[0] == '!')
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
            oss << "\\textasciicircum{}";
            break;
        case '~':
            oss << "\\textasciitilde{}";
            break;
        case '|':
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

std::string texName(const std::string& name, bool keepQuotes)
{
    std::ostringstream oss;

    char prev = '\0';
    for (size_t i = 0; i < name.length(); ++i)
    {
        const char c = name[i];
        if (((prev == '.' || prev == '_') && isalnum((unsigned char)c))
            || ((islower((unsigned char)prev) || isdigit((unsigned char)prev)) && isupper((unsigned char)c)))
        {
            oss << "\\allowbreak{}";
        }
        // texEscape takes a leading '!' to mean text already in tex
        if (c == '!')
            oss << c;
        else
            oss << texEscape(std::string(1, c), keepQuotes);
        prev = c;
    }

    return oss.str();
}

std::string texHeading(const std::string& name)
{
    return "[" + texEscape(name) + "]{" + texName(name) + "}";
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
 *
 * Arguments after nbColumns are pairs of (int width in points,
 * const char* heading), one pair per column.
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
    for (int i = 0; i < nbColumns; ++i)
    {
        int width = va_arg(args, int);
        const char* heading = va_arg(args, const char*);

        widths.push_back(width);
        headings.push_back(std::string(heading));
    }
    va_end(args);

    ostr << "\n\n\\begin{spitable}{" << nbColumns << "}{";
    for (int i = 0; i < nbColumns; ++i)
        ostr << "|p{" << widths[i] << "pt}";
    ostr << "|}{";
    for (int i = 0; i < nbColumns; ++i)
    {
        if (i > 0)
            ostr << " & ";
        ostr << "\\textbf{" << headings[i] << "}";
    }
    ostr << "}\n";
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
    for (int i = 0; i < nbColumns; ++i)
    {
        const char* columnEntry = va_arg(args, const char*);
        columnEntries.push_back(std::string(columnEntry));
    }
    va_end(args);

    ostr << "\\spirow{";
    for (int i = 0; i < nbColumns; ++i)
    {
        if (i > 0)
            ostr << " & ";
        ostr << texEscape(columnEntries[i]);
    }
    ostr << "}\n";
}


/**
 * Writes the end of a table in Tex format.
 * (nbColumns is no longer needed but is kept so callers do not change.)
 */
void writeTexEndTable(GeneratedOutput& ostr, int /*nbColumns*/)
{
    ostr << "\\end{spitable}\n\n";
}

 /**
  * Writes the beginning of a specific options table in Tex format.
  */
void writeTexBeginOptionsTable(
    GeneratedOutput& ostr)
{
    ostr << "\n\n\\begin{optionstable}\n";
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
    ostr << "\\optrow{" << texEscape(name) << "}{"
        << texEscape(defaultValue) << "}{"
        << texEscape(range) << "}\n";
}

/**
 * Writes the end of an options table in Tex format.
 */
void writeTexEndOptionsTable(GeneratedOutput& ostr)
{
    ostr << "\\end{optionstable}\n\n";
}

/**
 * Writes the beginning of a block of syntax description.
 *
 * Syntax is pretty much shown verbatim but with control sequences supported.
 * The syntax environment (see spidoc.tex) uses the fancyvrb package and sets
 * the command characters, so no escaping of \ { } is needed here.
 */
void writeTexBeginSyntax(GeneratedOutput& ostr, bool usesDescription)
{
    ostr << "\n\\syntaxhead\n"
        << "\\begin{syntax}\n";

    if (usesDescription)
        ostr << "    \\syntaxdesc\n";
}

/**
 * Writes the end of a block of syntax description.
 */
void writeTexEndSyntax(GeneratedOutput& ostr)
{
    ostr << "\\end{syntax}\n";
}
