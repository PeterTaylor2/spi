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
#ifndef PARSER_TOOLS_HPP
#define PARSER_TOOLS_HPP

#include "constant.hpp"
#include "configLexer.hpp"

typedef std::map<std::string, ConstantConstSP> ParserOptions;

class DescriptionParser
{
public:
    DescriptionParser();

    // clears the description
    void clear();

    // takes (and clears) the description
    std::vector<std::string> take();

    // consumes description tokens from the lexer and returns the first
    // non-description token
    ConfigLexer::Token consume(ConfigLexer& lexer, bool verbose=false);

private:
    std::vector<std::string> description;
};

/*
 * Parses a sequence of name=value returning a map of constants.
 * Parsing ends when the terminating token is encountered.
 * For example, we might parse constants until we encounter ;
 *
 * If we reach the end of the file then an exception is thrown.
 * If the data cannot be parsed then an exception is thrown.
 *
 * On success, we push back the final token since we allow termination
 * via multiple single character tokens.
 */
ParserOptions parseOptions(
    ConfigLexer& lexer,
    const char* terminators,
    const ParserOptions& defaultOptions,
    bool verbose);

ConstantConstSP getOption(const ParserOptions& options, const char* name);

ConstantConstSP parseConstant(ConfigLexer& lexer);


/*
 * Get token of particular type, throwing an exception for other types.
 */
ConfigLexer::Token getTokenOfType(
    ConfigLexer& lexer,
    int expectedTokenType,
    const char* expectedTokenName=0,
    const char* function=0);

/*
 * Gets a c++ typename. Returns any tokens arising from lookahead operations.
 *
 * This will handle namespaces and templates. For example
 * boost::shared_ptr< ns::myType >
 */
std::string getCppTypeName(
    ConfigLexer& lexer,
    bool noTemplates=false);

/*
 * Splits a c++ name into name and namespace
 */
void splitCppTypeName(
    const std::string& fullName,
    std::string& name,
    std::string& ns);

/*
 * Gets a constant value and returns as a string.
 *
 * We cannot be sure that a token of type NAME is a constant, but we will
 * assume that it is.
 *
 * We can also accept ints, doubles, strings etc.
 *
 * We actually return something that could be replicated verbatim in the
 * generated code - it may or may not compile - that will be out of our
 * hands at this point.
 */
std::string getConstantValue(
    ConfigLexer& lexer);

#endif
