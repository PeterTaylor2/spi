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
 * Lexical analyzer for the SPI configuration file format
 *
 * We need to recognize integers, dates, strings, real numbers, identifiers
 * and various bits of punctuation and keywords from the parser.
 *
 * In addition we need to be able to extract a verbatim block - what this
 * will do is extract from the code until a terminating separator is
 * detected.
 */
%{

#include <spi/platform.h>
#include "configLexer.hpp"
#include <spi/StringUtil.hpp>

#include <stdarg.h> // for variable argument list methods
#include <string.h> // for strdup etc

#ifdef _MSC_VER
/* this disables the warning message about dividing by zero */
#pragma warning( disable : 4723 )
#define YY_NO_UNISTD_H
#define strdup _strdup
#endif

#define YY_DECL int ConfigLexer::getToken(TokenValue *tokenValue)
#define YY_USER_ACTION if (m_inVerbatim) { m_verbatim << yytext; }

using namespace std; // to get cin and cout

// we will have various states in addition to the normal state (INITIAL)
// these are CPP_COMMENT, C_COMMENT, DESCRIPTION, COMMAND

%}

integer  -?[0-9]+
date     [0-9]+\-[0-9]+\-[0-9]+
double   -?((([0-9]+)|([0-9]*\.[0-9]+))([eE][-+]?[0-9]+)?)
string   \"([^\"]|\\\")*[\"]
name     [a-zA-Z_][a-zA-Z0-9_]*

%option yylineno

%x CPP_COMMENT
%x C_COMMENT
%x DESCRIPTION
%x COMMAND
%%

[ \t\r\n] ; /* ignore whitespace */

 /*
 * Integers - positive or negative.
 */
{integer} {
    tokenValue->anInt = atoi(yytext);
    dprintf ("INT: %ld\n", tokenValue->anInt);
    return SPI_CONFIG_TOKEN_TYPE_INT;
}

 /*
 * Dates - YYYY-MM-DD
 */
{date} {
    tokenValue->aDate = stringToDate(yytext);
    dprintf ("DATE: %s\n", yytext);
    return SPI_CONFIG_TOKEN_TYPE_DATE;
}

 /*
 * Doubles - positive or negative, with optional exponent.
 */
{double} {
    char* ep;
    tokenValue->aDouble = strtod(yytext, &ep);
    // we should check that ep points at the end of the yytext
    // it ought to be ok since yytext is of the correct pattern
    dprintf ("DOUBLE: %f\n", tokenValue->aDouble);
    return SPI_CONFIG_TOKEN_TYPE_DOUBLE;
}

 /*
 * Single characters represented in C-style within single quotes.
 *
 * We won't allow these to be escaped - when we have a char we
 * really expect it to be printable.
 */
'.' {
    tokenValue->aChar = yytext[1];
    dprintf ("CHAR: %c\n", tokenValue->aChar);
    return SPI_CONFIG_TOKEN_TYPE_CHAR;
}


 /*
 * Single characters supporting escaping - designed for use solely within
 * verbatim C++ code.
 */
'\\.' {
    tokenValue->aCharEscape = yytext[2];
    dprintf("CHAR_ESCAPE: %c\n", tokenValue->aCharEscape);
    return SPI_CONFIG_TOKEN_TYPE_CHAR_ESCAPE;
}

 /*
  * Expressions are like in a Unix-style shell - enclosed by ``
  */
`.*` {
    yytext[strlen(yytext)-1] = '\0';
    try
    {
        std::string expr = spi::StringCapture(yytext+1);
        tokenValue->anExpression = strdup(expr.c_str());
    }
    catch (std::exception &e)
    {
        error_handler("%s\n", e.what());
    }
    return SPI_CONFIG_TOKEN_TYPE_EXPRESSION;
}

 /*
 * Strings - we do not allow strings to be continued over a line
 */
{string} {
    if (yytext[yyleng-1] != '"')
    {
        error_handler ("Unterminated character string %s", yytext);
    }
    else
    {
        yytext[yyleng-1] = '\0';
        try
        {
            std::string str = spi::StringCapture(yytext+1);
            tokenValue->aString = strdup(str.c_str());
            dprintf ("STRING: %s\n", tokenValue->aString);
        }
        catch (std::exception &e)
        {
            error_handler("%s\n", e.what());
        }
    }
    return SPI_CONFIG_TOKEN_TYPE_STRING;
}

 /*
  * Boolean values can use true, True, TRUE, false, False or FALSE
  */
true|false|True|False|TRUE|FALSE {
    tokenValue->aBool = (yytext[0] == 't' || yytext[0] == 'T');
    dprintf ("BOOL: %d\n", tokenValue->aBool);
    return SPI_CONFIG_TOKEN_TYPE_BOOL;
}

 /*
  * Keywords just return the keyword
  *
  * Special language keywords begin with %
  * Keywords borrowed from c++ are as is
  * Some primitive types which are not primitive c++ types are treated
  * as keywords - e.g. string, Date, size_t
  */
\%include   | /* indicates that we will include text before parsing */
\%module    | /* definition for a module */
\%import    | /* indicates that we are importing definitions from a service */
\%service   | /* definition for a service */
\%export    | /* indicates that we are exporting definitions from a module */
char        | /* type char */
int         | /* type int */
long        | /* type long */
double      | /* type double */
string      | /* type string */
size_t      | /* type size_t */
void        | /* return type */
class       | /* class definition or wrapper */
struct      | /* struct definition or wrapper or part of a typedef */
typedef     | /* type definition */
template    | /* can be part of a typedef or wrapper */
typename    | /* typename - indicates we are wrapping a typedef */
enum        | /* enumerated type */
extern      | /* extern - indicates function or class method */
static      | /* indicates a static member function of a class */
virtual     | /* indicates a virtual method of a class */
delegate    | /* indicates wrapper class can be delegate for inner class */
const       | /* const - can apply to methods or classes */
property    | /* indicates that the field is a property only */
private     | /* indicates a private field */
public      | /* indicates a public field */
operator    | /* used for CoerceTo specification */
private\:   | /* indicates a private (verbatim) section of a class */
protected\: | /* indicates protected (verbatim) section of a class */
public\:    | /* indicates a public (verbatim) section of a class */
\:\:        {
    tokenValue->aKeyword = strdup(yytext);
    return SPI_CONFIG_TOKEN_TYPE_KEYWORD;
}

 /*
  * commands begin with # and go to the end of the line
  */
\# {
    BEGIN(COMMAND);
    clear_buffer();
}

 /*
  * command ends with new line
  */
<COMMAND>\n {
    tokenValue->aCommand = strdup(m_buffer.str().c_str());
    dprintf ("COMMAND: %s\n", tokenValue->aCommand);
    clear_buffer();
    BEGIN(INITIAL);
    return SPI_CONFIG_TOKEN_TYPE_COMMAND;
}

 /*
  * any character within the COMMAND state is appended to the buffer
  */
<COMMAND>. m_buffer << yytext[0];

 /*
  * doxygen style c-comments are treated as description
  * they start with / ** and go to the matching end comment
  */

\/\*\* {
    BEGIN(DESCRIPTION);
    clear_buffer();
}

 /*
  * doxygen style comments end with the end comment
  * we are not so sophisticated that we check whether they are part of a string
  */
<DESCRIPTION>\*\/ {
    tokenValue->aDescription = strdup(m_buffer.str().c_str());
    dprintf ("DESCRIPTION: %s\n", tokenValue->aDescription);
    clear_buffer();
    BEGIN(INITIAL);
    return SPI_CONFIG_TOKEN_TYPE_DESCRIPTION;
}

 /*
  * new lines in the DESCRIPTION state are appended to the buffer
  */
<DESCRIPTION>\n m_buffer << std::endl;

 /*
  * any character in the DESCRIPTION state is appended to the buffer
  */
<DESCRIPTION>. m_buffer << yytext[0];

 /*
  * c++ comments begin with // and go to the end of the line
  */
\/\/ BEGIN(CPP_COMMENT);

 /*
  * the c++ comment ends with new line - the contents are ignored
  */
<CPP_COMMENT>\n BEGIN(INITIAL);

 /*
 * c++ comments are completely ignored
 */
<CPP_COMMENT>.

 /*
 * c-comments are treated the same as c++-comments
 * they are ignored unless they are also doxygen style comments which are
 * processed first in this list (is that relevant or is lex clever enough
 * to find the maximum length text that matches a rule?
 */
\/\* BEGIN(C_COMMENT);

 /*
  * c-comments end with the end of comment sequence
  */
<C_COMMENT>\*\/ BEGIN(INITIAL);

<C_COMMENT>\n

 /*
 * c comments are completely ignored
 */
<C_COMMENT>.

 /*
  * We will support various punctuation marks as follows:
  * !%^&*()-+={[}]:;~#|<,>.?/
  *
  * (typed in from a QWERTY keyboard from top left to bottom right)
  *
  * Some of these are needed for verbatim code which we tokenize when
  * searching for the end of the block.
  */

[\!\%\^\&\*\(\)\-\+\=\{\[\}\]\:\;\~\#\|\<\,\>\.\?\/] {
    dprintf ("PUNCT: %s\n", yytext);
    return yytext[0]; }

<<EOF>> {
    yyterminate();
}

 /* name - valid C/C++ identifier and defined after all the keywords */

{name} {
    tokenValue->aName = strdup(yytext);
    dprintf ("NAME: %s\n", tokenValue->aName);
    return SPI_CONFIG_TOKEN_TYPE_NAME;
}

. {
    error_handler ("Invalid character %c\n", yytext[0]);
    return 0;
}

%%

 /*
 * We should not get here
 */
int yyFlexLexer::yylex()
{
    fprintf (stderr, "%s: should not get here!\n", __FUNCTION__);
    exit (1);
}

/*
** This function handles the condition where we reach the end of a file
** and need to start again.
*/
int yyFlexLexer::yywrap(void)
{
    return 1; /* no more files */
}

void ConfigLexer::error_handler(const char *format, ...)
{
    ++num_errors;
    fprintf (stderr, "%s(%d): error: ", filename.c_str(), lineno());
    va_list ap;
    va_start (ap, format);
    vfprintf (stderr, format, ap);
    va_end (ap);
}

void ConfigLexer::dprintf(const char* format, ...)
{
#if YYDEBUG
    va_list ap;
    va_start (ap, format);
    vprintf (format, ap);
    va_end (ap);
#endif
}

int ConfigLexer::stringToDate (const char* str)
{
    try
    {
        spi::Date date = spi::Date::FromString(str);
        return (int)date;
    } catch (std::exception&) {
        error_handler ("Could not convert %s to date\n", str);
        return -1;
    }
}

ConfigLexer::ConfigLexer (const std::string &arg_filename,
                          std::istream      *arg_yyin,
                          std::ostream      *arg_yyout)
:
yyFlexLexer(arg_yyin, arg_yyout),
    filename(arg_filename),
    num_errors(0),
    token_stack(),
    m_buffer(),
    m_inVerbatim(false),
    m_verbatim()
{}

ConfigLexer::~ConfigLexer() {}

ConfigLexer::Token ConfigLexer::getToken()
{
    if (token_stack.size() > 0)
    {
        Token token = token_stack.back();
        token_stack.pop_back();
        return token;
    }
    int tokenType;
    TokenValue tokenValue;
    tokenType = getToken(&tokenValue);
    Token token(tokenType, tokenValue);

    return token;
}

void ConfigLexer::returnToken(Token &token)
{
    token_stack.push_back(token);
}


namespace {

    ConfigLexer::TokenValue TokenValueCopy(
        int                     tokenType,
        ConfigLexer::TokenValue value)
    {
        ConfigLexer::TokenValue copy;

        switch (tokenType)
        {
        case SPI_CONFIG_TOKEN_TYPE_STRING:
        case SPI_CONFIG_TOKEN_TYPE_NAME:
        case SPI_CONFIG_TOKEN_TYPE_KEYWORD:
        case SPI_CONFIG_TOKEN_TYPE_DESCRIPTION:
        case SPI_CONFIG_TOKEN_TYPE_EXPRESSION:
        case SPI_CONFIG_TOKEN_TYPE_COMMAND:
            /* copying the string is the same as copying any char* member
               of the union! */
            copy.aString = strdup(value.aString);
            break;
        default:
            copy = value;
            break;
        }
        return copy;
    }

    void TokenValueFree(
        int                     tokenType,
        ConfigLexer::TokenValue value)
    {
        switch(tokenType)
        {
        case SPI_CONFIG_TOKEN_TYPE_STRING:
        case SPI_CONFIG_TOKEN_TYPE_NAME:
        case SPI_CONFIG_TOKEN_TYPE_KEYWORD:
        case SPI_CONFIG_TOKEN_TYPE_DESCRIPTION:
        case SPI_CONFIG_TOKEN_TYPE_EXPRESSION:
        case SPI_CONFIG_TOKEN_TYPE_COMMAND:
            /* freeing the string is the same as freeing any char* member
               of the union */
            free(value.aString);
            break;
        default:
            break; // do nothing
        }
        memset(&value, 0, sizeof(ConfigLexer::TokenValue));
    }

} // end of anonymous namespace

ConfigLexer::Token::Token (int tokenType, TokenValue tokenValue)
    :
    type(tokenType),
    value(tokenValue)
{}

ConfigLexer::Token::Token ()
{
    type          = 0;
    value.aDouble = 0.0;
}

ConfigLexer::Token::~Token()
{
    TokenValueFree(type, value);
}

ConfigLexer::Token::Token (const Token &other)
{
    type  = other.type;
    value = TokenValueCopy(other.type, other.value);
}

ConfigLexer::Token& ConfigLexer::Token::swap (ConfigLexer::Token &other)
{
    std::swap(type, other.type);
    std::swap(value, other.value);
    return *this;
}

ConfigLexer::Token& ConfigLexer::Token::operator= (const Token &other)
{
    ConfigLexer::Token tmp(other);
    swap(tmp);
    return *this;
}

std::string ConfigLexer::Token::toString() const
{
    switch (type)
    {
    case SPI_CONFIG_TOKEN_TYPE_INT:
        return spi::StringFormat("%d", value.anInt);
    case SPI_CONFIG_TOKEN_TYPE_DATE:
        return std::string(spi::Date(value.aDate));
    case SPI_CONFIG_TOKEN_TYPE_DOUBLE:
        return spi::StringFormat("%f", value.aDouble);
    case SPI_CONFIG_TOKEN_TYPE_CHAR:
        return spi::StringFormat("'%c'", value.aChar);
    case SPI_CONFIG_TOKEN_TYPE_STRING:
        return spi::StringFormat("\"%s\"", value.aString);
    case SPI_CONFIG_TOKEN_TYPE_BOOL:
        return std::string(value.aBool ? "true" : "false");
    case SPI_CONFIG_TOKEN_TYPE_NAME:
        return std::string(value.aName);
    case SPI_CONFIG_TOKEN_TYPE_KEYWORD:
        return std::string(value.aKeyword);
    case SPI_CONFIG_TOKEN_TYPE_DESCRIPTION:
        return spi::StringFormat("/**%s*/", value.aDescription);
    case SPI_CONFIG_TOKEN_TYPE_EXPRESSION:
        return spi::StringFormat("%s", value.anExpression);
    case SPI_CONFIG_TOKEN_TYPE_COMMAND:
        return spi::StringFormat("#%s", value.aCommand);
    case SPI_CONFIG_TOKEN_TYPE_CHAR_ESCAPE:
        return spi::StringFormat("'\\%c'", value.aCharEscape);
    case 0:
        return std::string("EOF");
    }
    return spi::StringFormat("%c", type);
}

std::ostream& operator << (std::ostream& os, const ConfigLexer::Token& token)
{
    os << token.toString();
    return os;
}

void ConfigLexer::clear_buffer()
{
    m_buffer.clear();
    m_buffer.str(std::string());
}

VerbatimConstSP ConfigLexer::getVerbatim(
    char incrementor,
    char terminator,
    const char* extras)
{
    int lineNumber = getLineNumber();

    // initialise capture of all text before it gets put into a token
    m_inVerbatim = true;
    m_verbatim.str(std::string());

    Token token = getToken();
    int level = 0;

    while (token.type)
    {
        if (token.type == incrementor)
        {
            ++level;
        }
        else if (token.type == terminator)
        {
            if (level == 0)
                break;
            --level;
        }
        token = getToken();
    }

    if (token.type != terminator)
    {
        error_handler("Unterminated code block - EOF before matching %c\n",
            terminator);
        // throw std::runtime_error("Could not get VERBATIM code");
        return VerbatimConstSP();
    }

    m_verbatim << extras;
    m_inVerbatim = false;

    std::vector<std::string> code = spi::StringSplit(m_verbatim.str(), '\n');
    m_verbatim.str(std::string());

    return Verbatim::Make(filename, lineNumber, code);
}

int ConfigLexer::getLineNumber() const
{
    return yylineno;
}

