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
#ifndef SPGTOOLS_COMMON_TOOLS_HPP
#define SPGTOOLS_COMMON_TOOLS_HPP

#include <string>
#include <vector>
#include <spi/spdoc_publicType.hpp>

class GeneratedOutput;

std::string TranslateFromValue(
    spdoc::PublicType  publicType,
    const std::string& dataTypeName,
    const std::string& value,
    int arrayDim);

std::string TranslateToValue(
    spdoc::PublicType  publicType,
    const std::string& dataTypeName,
    const std::string& value,
    int arrayDim);

std::string GetFirstParagraph(const std::vector<std::string>& description);

std::string StringReplace(
    const std::string& in,
    const char* oldString,
    const char* newString);

/**
 * Splits a description into blocks where a blank line indicates a new
 * new paragraph.
 *
 * In addition each line of the description will be stripped of leading and
 * trailing white space.
 *
 * You will always get at least one block returned even if there are no
 * contents in the description.
 */
std::vector< std::vector<std::string> > SplitDescription(
    const std::vector<std::string>& description);


/**
 * When writing the parameters for a function call, using this function will
 * try to fit as much into one line as possible for each line of the function
 * call.
 *
 * Will open and close the brackets.
 * Can be used for most target languages.
 */
void writeArgsCall(
    GeneratedOutput& ostr,
    const std::vector<std::string>& args,
    size_t startPos,
    size_t indent,
    size_t maxLine=78);

void generateFromTemplate(
    std::istream& istr,
    const std::string& ifn,
    const std::map<std::string, std::string>& values,
    const std::string& ofn,
    const std::string& cwd,
    bool writeBackup);

// copies a file if it is newer than the old file
// or if the old file does not exist
//
// returns true if the file was copied, false otherwise
bool copyNewFile(
    const std::string& ifn,
    const std::string& ofn);

#endif
