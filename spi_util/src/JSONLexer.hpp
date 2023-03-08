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
***************************************************************************
** JSONLexer.hpp
***************************************************************************
** Header file for the LEX-generated JSON lexer. This takes a file and
** returns a stream of tokens to be processed by the parser.
***************************************************************************
*/

#ifndef SPI_UTIL_JSON_LEXER_HPP
#define SPI_UTIL_JSON_LEXER_HPP

#include "Namespace.hpp"

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

/* Token types.

   Since punctuation just returns the character, we need to choose numbers
   out of the range of the largest character, i.e. at least 256.
 */
#define SPI_JSON_TOKEN_TYPE_DOUBLE 256
#define SPI_JSON_TOKEN_TYPE_STRING 257
#define SPI_JSON_TOKEN_TYPE_BOOL   258
#define SPI_JSON_TOKEN_TYPE_NULL   259

SPI_UTIL_NAMESPACE

class JSONLexerImpl;

class JSONLexer
{
public:
    typedef union TokenValue {
        double aDouble;
        char*  aString;
        bool   aBool;
    } TokenValue;

    class Token
    {
    public:
        int        type;
        // note some members of TokenValue union are heap-allocated
        // hence we need to take care with memory management
        TokenValue value;

        Token (int tokenType, TokenValue tokenValue);

        // the BIG THREE
        ~Token();
        Token (const Token &other);
        Token& operator= (const Token &other);

        std::string toString() const;
    private:
        Token& swap (Token &other);
    };

    JSONLexer (const std::string &arg_filename,
               std::istream      *arg_yyin,
               std::ostream      *arg_yyout = 0);

    ~JSONLexer();

    // this gets the token - possibly by picking up previous tokens
    // tht have been returned to the token_stack
    Token getToken();
    void returnToken(Token &token);

    int lineno();

private:

    JSONLexerImpl* impl;
    std::vector<Token> token_stack;

    JSONLexer(const JSONLexer&);
    JSONLexer& operator=(const JSONLexer&);
};

SPI_UTIL_END_NAMESPACE

#endif


