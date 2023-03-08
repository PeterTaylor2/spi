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
 * Lexical analyzer for the SPI text format
 *
 * We need to recognize integers, dates, strings, real numbers, identifiers
 * and various bits of punctuation.
 *
 * The punctuation we have to recognise is as follows:
 * {} - opening and closing a logical object
 * *  - reference to a previous object
 * =  - name = value is how we represent name values
 * [] - opening and closing a list
 * :  - after ERROR indicates an error string
 */
%{

#include "platform.h"
#include "LexerImpl.hpp"
#include "StringUtil.hpp"
#include "DateUtil.hpp"
#include <math.h>   // for sqrt - creates nan via sqrt(-1.0)
#include <stdarg.h> // for variable argument list methods
#include <limits>

#ifdef _MSC_VER
#define YY_NO_UNISTD_H
#define strdup _strdup
#endif

#define YY_DECL int spi_util::LexerImpl::getToken(Lexer::TokenValue *tokenValue)

using namespace std; // to get cin and cout

%}

integer     -?[0-9]+
date        [0-9]+\-[0-9]+\-[0-9]+
double      -?((([0-9]+)|([0-9]*\.[0-9]+))([eE][-+]?[0-9]+)?)
string      \"([^\"]|\\\")*[\"]
name          [a-zA-Z_][a-zA-Z0-9_\.]*

%option yylineno

%x COMMENT

%%

[ \t\r\n] ; /* ignore whitespace */

 /*
 * Integers - positive or negative.
 */
{integer} {
    tokenValue->anInt = atoi(yytext);
    dprintf ("INT: %ld\n", tokenValue->anInt);
    return SPI_TOKEN_TYPE_INT;
}

 /*
 * Dates - YYYY-MM-DD
 */
{date} {
    tokenValue->aDate = stringToDate(yytext);
    dprintf ("DATE: %s\n", yytext);
    return SPI_TOKEN_TYPE_DATE;
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
    return SPI_TOKEN_TYPE_DOUBLE;
}

NAN {
    tokenValue->aDouble = std::numeric_limits<double>::quiet_NaN();
    dprintf ("DOUBLE: %f\n", tokenValue->aDouble);
    return SPI_TOKEN_TYPE_DOUBLE;
}

INF {
    tokenValue->aDouble = std::numeric_limits<double>::infinity();
    dprintf ("DOUBLE: %f\n", tokenValue->aDouble);
    return SPI_TOKEN_TYPE_DOUBLE;
}

-INF {
    tokenValue->aDouble = -std::numeric_limits<double>::infinity();
    dprintf ("DOUBLE: %f\n", tokenValue->aDouble);
    return SPI_TOKEN_TYPE_DOUBLE;
}

NULL {
    dprintf ("NULL\n");
    return SPI_TOKEN_TYPE_NULL;
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
    return SPI_TOKEN_TYPE_CHAR;
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
    return SPI_TOKEN_TYPE_STRING;
}

 /*
 * Boolean values can use true or false
 */
true|false {
    tokenValue->aBool = (yytext[0] == 't');
    dprintf ("BOOL: %d\n", tokenValue->aBool);
    return SPI_TOKEN_TYPE_BOOL;
}


 /*
 * We will support various punctuation marks as follows: *,{}:[]=
 */

[\*\,\{\}\:\[\]\=] {
    dprintf ("PUNCT: %s\n", yytext);
    return yytext[0]; }

<<EOF>> {
    yyterminate();
}

 /* field name - essentially must be a C/C++ identifier */

{name} {
    tokenValue->aName = strdup(yytext);
    dprintf ("NAME: %s\n", tokenValue->aName);
    return SPI_TOKEN_TYPE_NAME;
}

 /*
  * comments begin with # and go to the end of the line
  */
\# BEGIN(COMMENT);

 /*
  * the comment ends with new line - the contents are ignored
  */
<COMMENT>\n BEGIN(INITIAL);

 /*
 * comments are completely ignored
 */
<COMMENT>.

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

void LexerImpl::error_handler(const char *format, ...)
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

std::string LexerImpl::get_errors()
{
    return errors.str();
}

void LexerImpl::dprintf(const char* format, ...)
{
#if YYDEBUG
    va_list ap;
    va_start (ap, format);
    vprintf (format, ap);
    va_end (ap);
#endif
}

int LexerImpl::stringToDate (const char* str)
{
    try
    {
        return StringToDate(str);
    } catch (std::exception&) {
        error_handler ("Could not convert %s to date", str);
        return -1;
    }
}

LexerImpl::LexerImpl (const std::string &arg_filename,
                      std::istream      *arg_yyin,
                      std::ostream      *arg_yyout)
    :
    yyFlexLexer(arg_yyin, arg_yyout),
    filename(arg_filename),
    num_errors(0),
    errors()
{}

LexerImpl::~LexerImpl() {}

Lexer::Lexer(const std::string &arg_filename,
             std::istream      *arg_yyin,
             std::ostream      *arg_yyout)
    :
    impl(new LexerImpl(arg_filename, arg_yyin, arg_yyout)),
    token_stack()
{}

Lexer::~Lexer()
{
    delete impl;
}

Lexer::Token Lexer::getToken()
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

void Lexer::returnToken(Token &token)
{
    token_stack.push_back(token);
}

int Lexer::lineno()
{
    return impl->lineno();
}

namespace {

    Lexer::TokenValue TokenValueCopy(
        int               tokenType,
        Lexer::TokenValue value)
    {
        Lexer::TokenValue copy;

        switch (tokenType)
        {
        case SPI_TOKEN_TYPE_STRING:
            copy.aString = strdup(value.aString);
            break;
        case SPI_TOKEN_TYPE_NAME:
            copy.aName = strdup(value.aName);
            break;
        default:
            copy = value;
            break;
        }
        return copy;
    }

    void TokenValueFree(
        int               tokenType,
        Lexer::TokenValue value)
    {
        switch(tokenType)
        {
        case SPI_TOKEN_TYPE_STRING:
            free(value.aString);
            break;
        case SPI_TOKEN_TYPE_NAME:
            free(value.aName);
            break;
        default:
            break; // do nothing
        }
        memset(&value, 0, sizeof(Lexer::TokenValue));
    }

} // end of anonymous namespace

Lexer::Token::Token (int tokenType, TokenValue tokenValue)
    :
    type(tokenType),
    value(tokenValue)
{}

Lexer::Token::~Token()
{
    TokenValueFree(type, value);
}

Lexer::Token::Token (const Token &other)
{
    type  = other.type;
    value = TokenValueCopy(other.type, other.value);
}

Lexer::Token& Lexer::Token::swap (Lexer::Token &other)
{
    std::swap(type, other.type);
    std::swap(value, other.value);
    return *this;
}

Lexer::Token& Lexer::Token::operator= (const Lexer::Token &other)
{
    Lexer::Token tmp(other);
    swap(tmp);
    return *this;
}

std::string Lexer::Token::toString() const
{
    switch (type)
    {
    case SPI_TOKEN_TYPE_INT:
        return StringFormat("%d", value.anInt);
    case SPI_TOKEN_TYPE_DATE:
        return DateToString(value.aDate);
    case SPI_TOKEN_TYPE_DOUBLE:
        return StringFormat("%f", value.aDouble);
    case SPI_TOKEN_TYPE_CHAR:
        return StringFormat("'%c'", value.aChar);
    case SPI_TOKEN_TYPE_STRING:
        return StringFormat("\"%s\"", value.aString);
    case SPI_TOKEN_TYPE_BOOL:
        return std::string(value.aBool ? "true" : "false");
    case SPI_TOKEN_TYPE_NAME:
        return std::string(value.aName);
    case 0:
        return std::string("EOF");
    }
    return StringFormat("%c", type);
}

SPI_UTIL_END_NAMESPACE
