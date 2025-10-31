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
***************************************************************************
** templateLexer.hpp
***************************************************************************
** Header file for the LEX-generated configuration file Lexer. This takes a
** file and returns a stream of tokens to be processed by the parser.
***************************************************************************
*/

#ifndef SPGTOOLS_TEMPLATE_LEXER_HPP
#define SPGTOOLS_TEMPLATE_LEXER_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#define yyFlexLexer yyTemplateFlexLexer
#include "FlexLexer.h"

/* Token types.

   Since punctuation just returns the character, we need to choose numbers
   out of the range of the largest character, i.e. at least 256.
 */
#define TEMPLATE_TOKEN_TYPE_CHAR      257
#define TEMPLATE_TOKEN_TYPE_NAME      258

class TemplateLexer : public yyFlexLexer
{
public:
    typedef union TokenValue {
        char   aChar;
        char*  aName;
    } TokenValue;

    class Token
    {
    public:
        int        type;
        // note some members of TokenValue union are heap-allocated
        // hence we need to take care with memory management
        TokenValue value;

        Token (int tokenType, TokenValue tokenValue);
        Token ();

        // the BIG THREE
        ~Token();
        Token (const Token &other);
        Token& operator= (const Token &other);

        std::string toString() const;
    private:
        Token& swap (Token &other);
    };

    TemplateLexer (const std::string &arg_filename,
                   std::istream      *arg_yyin,
                   std::ostream      *arg_yyout = 0);

    ~TemplateLexer();

    // this gets the token - possibly by picking up previous tokens
    // tht have been returned to the token_stack
    Token getToken();
    void returnToken(Token &token);

    /* this method is written for us by flex */
    int getToken(TokenValue *tokenValue);

    std::string filename;
    int num_errors;

    void error_handler(const char *format, ...);
    void dprintf(const char *format, ...);
    int getLineNumber() const;
};

std::ostream& operator << (std::ostream&out, const TemplateLexer::Token& token);

#endif


