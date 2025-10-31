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
#ifndef SPGTOOLS_TEX_UTILS_HPP
#define SPGTOOLS_TEX_UTILS_HPP

#include <vector>
#include <string>

#include "generatedOutput.hpp"

std::string texFileName(
    const std::string& dirname,
    const char* filename,
    bool forImport=false);

/**
 * Writes a list of \input files to the output directory.
 * Returns true if the file was updated.
 */
void writeTexInputList(
    GeneratedOutput& ostr,
    const std::vector<std::string>& inputs,
    bool sortInputs,
    bool addNewPage=false);

/**
 * Adds escapes to plain text to enable it to be shown in tex.
 * Any genuine escapes would end up being doubled up - so only use this
 * for plain text without prior escapes.
 *
 * Sometimes the latter problem is difficult to avoid - in that case put
 * '!' at the beginning of the text and this function strips of the '!'
 * and assumes that the rest of the text is in acceptable format already.
 */
std::string texEscape(const std::string& plainText, bool keepQuotes=false);

/**
 * Returns the value of a variable according to the typographic conventions.
 */
std::string texValue(const std::string& varName);

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
    bool firstParagraphOnly = false);

/**
 * Writes the beginning of a table in Tex format.
 *
 * Variable entries are pairs of width,heading terminated by 0 for width.
 */
void writeTexBeginTable(
    GeneratedOutput& ostr,
    int nbColumns,
    ...);

/**
 * Write one row of a table.
 *
 * Variable inputs are column entries as c-string terminated by 0.
 */
void writeTexTableRow(
    GeneratedOutput& ostr,
    int nbColumns,
    ...);

/**
 * Writes the end of a table in Tex format.
 */
void writeTexEndTable(GeneratedOutput& ostr, int nbColumns);

/**
 * Writes the beginning of a table in Tex format.
 */
void writeTexBeginOptionsTable(
    GeneratedOutput& ostr);

/**
 * Write one row of an options table.
 */
void writeTexOptionsTableRow(
    GeneratedOutput& ostr,
    const std::string& name,
    const std::string& defaultValue,
    const std::string& range);

/**
 * Writes the end of a table in Tex format.
 */
void writeTexEndOptionsTable(GeneratedOutput& ostr);

/**
 * Writes the beginning of a block of syntax description.
 *
 * Syntax is pretty much shown verbatim but with control sequences supported.
 * It uses the fancyvrb package.
 */
void writeTexBeginSyntax(GeneratedOutput& ostr, bool usesDescription);

/**
 * Writes the end of a block of syntax description.
 */
void writeTexEndSyntax(GeneratedOutput& ostr);

#endif
