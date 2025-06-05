/*

    Sartorial Programming Interface (SPI) runtime libraries
    Copyright (C) 2012-2023 Sartorial Programming Ltd.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA

*/

/*
 * Lexical analyzer for the JSON text format
 *
 * From json.org:
 *
 * [begin quote]
 *
 * JSON is built on two structures:
 *
 * A collection of name/value pairs. In various languages, this is realized 
 * as an object, record, struct, dictionary, hash table, keyed list, or 
 * associative array.
 *
 * An ordered list of values. In most languages, this is realized as an array,
 * vector, list, or sequence.
 *
 * These are universal data structures. Virtually all modern programming 
 * languages support them in one form or another. It makes sense that a data 
 * format that is interchangeable with programming languages also be based 
 * on these structures.
 *
 * In JSON, they take on these forms:
 *
 * An object is an unordered set of name/value pairs. An object begins with
 * { (left brace) and ends with } (right brace). Each name is followed 
 * by : (colon) and the name/value pairs are separated by , (comma).
 *
 * An array is an ordered collection of values. An array begins
 * with [ (left bracket) and ends with ] (right bracket). 
 * Values are separated by , (comma).
 *
 * A value can be a string in double quotes, or a number, or true or false 
 * or null, or an object or an array. These structures can be nested.
 *
 * [end quote]
 * 
 * The lexical analyzer described by this file is simply going to parse
 * out the individual elements.
 */

%{

#include "platform.h"
#include "JSONLexerImpl.hpp"
#include "StringUtil.hpp"
#include <stdarg.h> // for variable argument list methods
#include <limits> // for std::numeric_limits

#ifdef _MSC_VER
/* this disables the warning message about dividing by zero */
#pragma warning( disable : 4723 )
#define YY_NO_UNISTD_H
#define strdup _strdup
#endif

#define YY_DECL int spi_util::JSONLexerImpl::getToken(JSONLexer::TokenValue *tokenValue)

using namespace std; // to get cin and cout

%}

integer     -?[0-9]+
double      -?((([0-9]+)|([0-9]*\.[0-9]+))([eE][-+]?[0-9]+)?)
string      \"([^\"]|\\\")*[\"]

%option yylineno

%%

[ \t\r\n] ; /* ignore whitespace */

 /*
 * Doubles - positive or negative, with optional exponent.
 */
{double} {
    char* ep;
    tokenValue->aDouble = strtod(yytext, &ep);
    // we should check that ep points at the end of the yytext
    // it ought to be ok since yytext is of the correct pattern
    dprintf ("DOUBLE: %f\n", tokenValue->aDouble);
    return SPI_JSON_TOKEN_TYPE_DOUBLE;
}

null {
    dprintf ("NULL\n");
    return SPI_JSON_TOKEN_TYPE_NULL;
}

 /*
 * Strings - we do not allow strings to be continued over a line.
 * However we do need to deal with escape sequences within the
 * string.
 */
{string} {
    tokenValue->aString = strdup(yytext+1); /* skip open quote */
    if (tokenValue->aString[yyleng-2] == '"')
    {
        /* skip close quote */
        tokenValue->aString[yyleng-2] = '\0';
        try
        {
            std::string str = spi_util::StringCapture(tokenValue->aString);
            strcpy(tokenValue->aString, str.c_str());
        }
        catch (std::exception &e)
        {
            error_handler("%s", e.what());
        }
    }
    else
    {
        error_handler ("Unterminated character string %s", yytext);
    }
    dprintf ("STRING: %s\n", tokenValue->aString);
    return SPI_JSON_TOKEN_TYPE_STRING;
}

 /*
 * Boolean values can use true or false
 */
true|false {
    tokenValue->aBool = (yytext[0] == 't');
    dprintf ("BOOL: %d\n", tokenValue->aBool);
    return SPI_JSON_TOKEN_TYPE_BOOL;
}

 /*
 * these values are not in the official JSON syntax but are supported
 * by the Python JSON parser. Hence we will output these values.
 */
NaN {
    tokenValue->aDouble = std::numeric_limits<double>::quiet_NaN();
    dprintf ("DOUBLE: %f\n", tokenValue->aDouble);
    return SPI_JSON_TOKEN_TYPE_DOUBLE;
}

Infinity {
    tokenValue->aDouble = std::numeric_limits<double>::infinity();
    dprintf ("DOUBLE: %f\n", tokenValue->aDouble);
    return SPI_JSON_TOKEN_TYPE_DOUBLE;
}

-Infinity {
    tokenValue->aDouble = -std::numeric_limits<double>::infinity();
    dprintf ("DOUBLE: %f\n", tokenValue->aDouble);
    return SPI_JSON_TOKEN_TYPE_DOUBLE;
}

 /*
 * We will support various punctuation marks as follows: ,{}:[]
 */

[\,\{\}\:\[\]] {
    dprintf ("PUNCT: %s\n", yytext);
    return yytext[0]; }

<<EOF>> {
    yyterminate();
}

. {
    error_handler ("Invalid character %c", yytext[0]);
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

SPI_UTIL_NAMESPACE

void JSONLexerImpl::error_handler(const char *format, ...)
{
    if (num_errors > 0)
        errors << "\n";

    errors << filename << ":" << lineno() << ": error: ";

    va_list ap;
    va_start (ap, format);
    errors << StringFormatV(format, ap);
    va_end (ap);

    ++num_errors;
}

std::string JSONLexerImpl::get_errors()
{
    return errors.str();
}

void JSONLexerImpl::dprintf(const char* format, ...)
{
#if YYDEBUG
    va_list ap;
    va_start (ap, format);
    vprintf (format, ap);
    va_end (ap);
#endif
}

JSONLexerImpl::JSONLexerImpl (const std::string &arg_filename,
                      std::istream      *arg_yyin,
                      std::ostream      *arg_yyout)
    :
    yyFlexLexer(arg_yyin, arg_yyout),
    filename(arg_filename),
    num_errors(0),
    errors()
{}

JSONLexerImpl::~JSONLexerImpl() {}

JSONLexer::JSONLexer(const std::string &arg_filename,
             std::istream      *arg_yyin,
             std::ostream      *arg_yyout)
    :
    impl(new JSONLexerImpl(arg_filename, arg_yyin, arg_yyout)),
    token_stack()
{}

JSONLexer::~JSONLexer()
{
    delete impl;
}

JSONLexer::Token JSONLexer::getToken()
{
    if (token_stack.size() > 0)
    {
        Token token = token_stack.back();
        token_stack.pop_back();
        return token;
    }
    int tokenType;
    TokenValue tokenValue;
    tokenType = impl->getToken(&tokenValue);
    if (impl->num_errors > 0)
        throw std::runtime_error(impl->get_errors());
    Token token(tokenType, tokenValue);

    return token;
}

void JSONLexer::returnToken(Token &token)
{
    token_stack.push_back(token);
}

int JSONLexer::lineno()
{
    return impl->lineno();
}

namespace {

    JSONLexer::TokenValue TokenValueCopy(
        int                   tokenType,
        JSONLexer::TokenValue value)
    {
        JSONLexer::TokenValue copy;

        switch (tokenType)
        {
        case SPI_JSON_TOKEN_TYPE_STRING:
            copy.aString = strdup(value.aString);
            break;
        default:
            copy = value;
            break;
        }
        return copy;
    }

    void TokenValueFree(
        int                   tokenType,
        JSONLexer::TokenValue value)
    {
        switch(tokenType)
        {
        case SPI_JSON_TOKEN_TYPE_STRING:
            free(value.aString);
            break;
        default:
            break; // do nothing
        }
        memset(&value, 0, sizeof(JSONLexer::TokenValue));
    }

} // end of anonymous namespace

JSONLexer::Token::Token (int tokenType, TokenValue tokenValue)
    :
    type(tokenType),
    value(tokenValue)
{}

JSONLexer::Token::~Token()
{
    TokenValueFree(type, value);
}

JSONLexer::Token::Token (const Token &other)
{
    type  = other.type;
    value = TokenValueCopy(other.type, other.value);
}

JSONLexer::Token& JSONLexer::Token::swap (JSONLexer::Token &other)
{
    std::swap(type, other.type);
    std::swap(value, other.value);
    return *this;
}

JSONLexer::Token& JSONLexer::Token::operator= (const JSONLexer::Token &other)
{
    JSONLexer::Token tmp(other);
    swap(tmp);
    return *this;
}

std::string JSONLexer::Token::toString() const
{
    switch (type)
    {
    case SPI_JSON_TOKEN_TYPE_DOUBLE:
        return StringFormat("%f", value.aDouble);
    case SPI_JSON_TOKEN_TYPE_STRING:
        return StringFormat("\"%s\"", value.aString);
    case SPI_JSON_TOKEN_TYPE_BOOL:
        return std::string(value.aBool ? "true" : "false");
    case SPI_JSON_TOKEN_TYPE_NULL:
        return "null";
    case 0:
        return std::string("EOF");
    }
    return StringFormat("%c", type);
}

SPI_UTIL_END_NAMESPACE
