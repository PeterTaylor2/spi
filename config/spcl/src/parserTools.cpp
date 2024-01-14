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
#include "parserTools.hpp"

#include <set>
#include <spi/RuntimeError.hpp>
#include <spi/StringUtil.hpp>

#include <string.h>

DescriptionParser::DescriptionParser()
  : description()
{}

void DescriptionParser::clear()
{
    description.clear();
}

std::vector<std::string> DescriptionParser::take()
{
    std::vector<std::string> output;
    output.swap(description);
    return output;
}

// consumes description tokens from the lexer
// returns the first non description token
ConfigLexer::Token DescriptionParser::consume(
    ConfigLexer& lexer,
    bool verbose)
{
    ConfigLexer::Token token = lexer.getToken();

    while (token.type == SPI_CONFIG_TOKEN_TYPE_DESCRIPTION)
    {
        std::vector<std::string> newlines = spi::StringSplit(
            token.value.aDescription, '\n');

        for (size_t i = 0; i < newlines.size(); ++i)
        {
            std::string line = spi::StringStrip(newlines[i]);
            while (spi::StringStartsWith(line, "*"))
                line = line.substr(1);
            line = spi::StringStrip(line);

            if (line.empty())
            {
                if (description.size() > 0 && !description.back().empty())
                    description.push_back(line);
            }
            else
            {
                description.push_back(line);
            }
        }

        if (verbose)
        {
            std::cout << "\nDESCRIPTION:" << std::endl;
            for (size_t i = 0; i < description.size(); ++i)
            {
                std::cout << i << ": [" << description[i] << "]"
                          << std::endl;
            }
            std::cout << std::endl;
        }
        token = lexer.getToken();
    }
    return token;
}

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
    bool verbose)
{
    ParserOptions output(defaultOptions);
    ConfigLexer::Token token = lexer.getToken();

    while (strchr(terminators, token.type) == NULL)
    {
        std::string name;

        if (token.type == SPI_CONFIG_TOKEN_TYPE_NAME)
        {
            name = token.value.aName;
        }
        else if (token.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD &&
                 token.value.aKeyword[0] != '%')
        {
            name = token.value.aKeyword;
        }
        else
        {
            throw spi::RuntimeError("%s: Expecting ';' or name. Actual was (%s)",
                                    __FUNCTION__, token.toString().c_str());
        }

        if (defaultOptions.count(name) == 0)
        {
            std::vector<std::string> allOptions;
            for (ParserOptions::const_iterator iter = defaultOptions.begin();
                iter != defaultOptions.end(); ++iter)
            {
                allOptions.push_back(iter->first);
            }
            std::string allOptionsString = spi_util::StringJoin(", ", allOptions);
            throw spi::RuntimeError("%s: Name %s not expected - available options %s",
                __FUNCTION__, name.c_str(), allOptionsString.c_str());
        }

        token = lexer.getToken();

        if (token.type != '=')
            throw spi::RuntimeError("%s: Expecting '=' after '%s'. Actual was (%s)",
                                    __FUNCTION__, name.c_str(),
                                    token.toString().c_str());

        token = lexer.getToken();

        ConstantConstSP constant;

        switch (token.type)
        {
        case SPI_CONFIG_TOKEN_TYPE_INT:
            constant = IntConstant::Make(token.value.anInt);
            break;
        case SPI_CONFIG_TOKEN_TYPE_DATE:
            constant = DateConstant::Make(spi::Date(token.value.aDate));
            break;
        case SPI_CONFIG_TOKEN_TYPE_DOUBLE:
            constant = DoubleConstant::Make(token.value.aDouble);
            break;
        case SPI_CONFIG_TOKEN_TYPE_CHAR:
            constant = CharConstant::Make(token.value.aChar);
            break;
        case SPI_CONFIG_TOKEN_TYPE_STRING:
            constant = StringConstant::Make(token.value.aString);
            break;
        case SPI_CONFIG_TOKEN_TYPE_BOOL:
            constant = BoolConstant::Make(token.value.aBool);
            break;
        case SPI_CONFIG_TOKEN_TYPE_NAME:
            constant = StringConstant::Make(token.value.aName);
            break;
        default:
            throw spi::RuntimeError("%s: Expecting constant after '%s='. Actual "
                                    "was (%s)", __FUNCTION__, name.c_str(),
                                    token.toString().c_str());
        }

        ConstantConstSP defaultOption = defaultOptions.find(name)->second;
        if (!defaultOption->isUndefined() &&
            strcmp(defaultOption->typeName(), constant->typeName()) != 0)
        {
            throw spi::RuntimeError("%s: Expecting option %s of type %s. Actual "
                                    "value was (%s)", __FUNCTION__,
                                    name.c_str(),
                                    defaultOption->typeName(),
                                    constant->toCode().c_str());
        }
        output[name] = constant;

        if (verbose)
        {
            std::cout << "\nOPTION:" << std::endl;
            std::cout << name << " = " << constant->toCode() << std::endl;
            std::cout << std::endl;
        }
        token = lexer.getToken();
    }
    lexer.returnToken(token); // put back the terminating token discovered
    return output;
}


ConstantConstSP getOption(const ParserOptions& options, const char* name)
{
    ParserOptions::const_iterator iter = options.find(name);
    if (iter == options.end())
        return Constant::UNDEFINED;
    return iter->second;
}

ConstantConstSP parseConstant(ConfigLexer& lexer)
{
    ConfigLexer::Token token = lexer.getToken();

    ConstantConstSP constant;

    switch (token.type)
    {
    case SPI_CONFIG_TOKEN_TYPE_INT:
        return IntConstant::Make(token.value.anInt);
    case SPI_CONFIG_TOKEN_TYPE_DATE:
        return DateConstant::Make(spi::Date(token.value.aDate));
    case SPI_CONFIG_TOKEN_TYPE_DOUBLE:
        return DoubleConstant::Make(token.value.aDouble);
    case SPI_CONFIG_TOKEN_TYPE_CHAR:
        return CharConstant::Make(token.value.aChar);
    case SPI_CONFIG_TOKEN_TYPE_STRING:
        return StringConstant::Make(token.value.aString);
    case SPI_CONFIG_TOKEN_TYPE_BOOL:
        return BoolConstant::Make(token.value.aBool);
    case SPI_CONFIG_TOKEN_TYPE_NAME:
        // potentially if we have a name it could be compound (:: separated)
        // for the moment assume simple
        return StringConstant::Make(token.value.aName);
    default:
        throw spi::RuntimeError("Expecting constant = actual was (%s)",
                                token.toString().c_str());
    }
}

ConfigLexer::Token getTokenOfType(
    ConfigLexer& lexer,
    int expectedTokenType,
    const char* expectedTokenName,
    const char* function)
{
    ConfigLexer::Token token = lexer.getToken();

    if (token.type != expectedTokenType)
    {
        if (!expectedTokenName)
            throw spi::RuntimeError("%s: %c expected. Actual was (%s)",
                               function ? function : __FUNCTION__,
                               expectedTokenType,
                               token.toString().c_str());

        throw spi::RuntimeError("%s: %s expected. Actual was (%s)",
                           function ? function : __FUNCTION__,
                           expectedTokenName,
                           token.toString().c_str());
    }

    return token;
}

static std::string tokenToCppTypeNamePart(ConfigLexer::Token& token)
{
    if (token.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD)
    {
        // some keywords are built-in variables
        std::string keyword (token.value.aKeyword);

        // at present string is a keyword - possibly a mistake
        // we might want to have std::string
        // hence string might be the 2nd part of a c++ typename

        if (keyword == "string")
        {
            return keyword;
        }
    }
    else if (token.type == SPI_CONFIG_TOKEN_TYPE_NAME)
    {
        return std::string(token.value.aName);
    }
    throw spi::RuntimeError("Looking for part of c++ typename - found %s",
        token.toString().c_str());
}


std::string getCppTypeName(
    ConfigLexer& lexer,
    bool noTemplates)
{
    // UGLY

    ConfigLexer::Token token = lexer.getToken();
    std::vector<std::string> parts;

    if (token.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD)
    {
        // some keywords are built-in variables
        std::string keyword (token.value.aKeyword);

        // at present size_t and string are keywords
        // I think that is probably a mistake - consider changing it

        if (keyword == "int" ||
            keyword == "char" ||
            keyword == "long" ||
            keyword == "double" ||
            keyword == "size_t")
        {
            return keyword;
        }

        if (keyword != "::")
            throw spi::RuntimeError("Keyword %s found when looking for c++ typename",
                                    token.value.aKeyword);

        parts.push_back(keyword);
        token = lexer.getToken();
    }

    std::string part = tokenToCppTypeNamePart(token);
    parts.push_back(part);

    // ok - we have started - we have the first part of a name
    // it might be in the global namespace but that is no problem

    // if the next token is '<' or '::' then we have to continue
    // once we get '<' we need to continue until we get a matching '>'
    // but we will use recursion to find the typename inside the <...>

    token = lexer.getToken();

    if (token.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD
        && strcmp(token.value.aKeyword, "::") == 0)
    {
        lexer.returnToken(token);
        std::string subTypeName = getCppTypeName(lexer, noTemplates);
        parts.push_back(subTypeName);
        return spi::StringJoin("", parts);
    }

    if (token.type == '<' && !noTemplates)
    {
        parts.push_back("< ");
        std::string subTypeName = getCppTypeName(lexer, false);
        parts.push_back(subTypeName);
        token = lexer.getToken();
        if (token.type != '>')
            throw spi::RuntimeError("Expecting '>' to match '<'");
        parts.push_back(" >");
        return spi::StringJoin("", parts);
    }

    lexer.returnToken(token);
    return spi::StringJoin("", parts);
}

/*
 * Splits a c++ name into name and namespace
 */
void splitCppTypeName(
    const std::string& fullName,
    std::string& name,
    std::string& ns)
{
    std::vector<std::string> nameParts = spi::StringSplit(fullName, "::");

    SPI_POST_CONDITION(nameParts.size() > 0);

    name = nameParts.back();

    if (nameParts.size() > 1)
    {
        std::ostringstream oss;
        size_t N = nameParts.size() - 1;
        for (size_t i = 0; i < N; ++i)
        {
            oss << spi::StringStrip(nameParts[i]);
            if (i+1 < N)
                oss << "::";
        }
        ns = oss.str();
    }
    else
    {
        ns = "";
    }
}


std::string getConstantValue(
    ConfigLexer& lexer)
{
    ConfigLexer::Token token = lexer.getToken();

    switch(token.type)
    {
    case SPI_CONFIG_TOKEN_TYPE_INT:
    case SPI_CONFIG_TOKEN_TYPE_DATE:
    case SPI_CONFIG_TOKEN_TYPE_DOUBLE:
    case SPI_CONFIG_TOKEN_TYPE_CHAR:
    case SPI_CONFIG_TOKEN_TYPE_STRING:
    case SPI_CONFIG_TOKEN_TYPE_BOOL:
    case SPI_CONFIG_TOKEN_TYPE_EXPRESSION:
        return token.toString();
    }

    // we also accept constants defined as names
    // these can include namespace separators (::) and may start with ::
    std::ostringstream oss;
    if (token.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD
        && strcmp(token.value.aKeyword, "::") == 0)
    {
        oss << "::";
        token = lexer.getToken();
    }
    if (token.type != SPI_CONFIG_TOKEN_TYPE_NAME)
    {
        throw spi::RuntimeError("Looking for constant value. Actual was (%s)",
                                 token.toString().c_str());
    }

    oss << token.value.aName;
    ConfigLexer::Token nextToken = lexer.getToken();
    while (nextToken.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD &&
           strcmp(nextToken.value.aKeyword, "::") == 0)
    {
        oss << "::";
        nextToken = lexer.getToken();
        if (token.type != SPI_CONFIG_TOKEN_TYPE_NAME)
        {
            throw spi::RuntimeError("Looking for NAME for constant value. Actual was (%s)",
                                    nextToken.toString().c_str());
        }
        oss << nextToken.value.aName;
        nextToken = lexer.getToken();
    }
    lexer.returnToken(nextToken);
    return oss.str();
}
