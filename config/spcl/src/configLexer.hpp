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
** configLexer.hpp
***************************************************************************
** Header file for the LEX-generated configuration file Lexer. This takes a
** file and returns a stream of tokens to be processed by the parser.
***************************************************************************
*/

#ifndef SPI_CONFIG_LEXER_HPP
#define SPI_CONFIG_LEXER_HPP

#include <spi/Date.hpp>
#include <spi/Map.hpp>
#include "verbatim.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#define yyFlexLexer yyConfigFlexLexer
#include <FlexLexer.h>

/* Token types.

   Since punctuation just returns the character, we need to choose numbers
   out of the range of the largest character, i.e. at least 256.
 */
#define SPI_CONFIG_TOKEN_TYPE_INT         257
#define SPI_CONFIG_TOKEN_TYPE_DATE        258
#define SPI_CONFIG_TOKEN_TYPE_DOUBLE      259
#define SPI_CONFIG_TOKEN_TYPE_CHAR        260
#define SPI_CONFIG_TOKEN_TYPE_STRING      261
#define SPI_CONFIG_TOKEN_TYPE_BOOL        262
#define SPI_CONFIG_TOKEN_TYPE_NAME        263
#define SPI_CONFIG_TOKEN_TYPE_KEYWORD     264
#define SPI_CONFIG_TOKEN_TYPE_DESCRIPTION 265
#define SPI_CONFIG_TOKEN_TYPE_EXPRESSION  266
#define SPI_CONFIG_TOKEN_TYPE_COMMAND     267
#define SPI_CONFIG_TOKEN_TYPE_CHAR_ESCAPE 268

class ConfigLexer : public yyFlexLexer
{
public:
    typedef union TokenValue {
        int    anInt;
        int    aDate;
        double aDouble;
        char   aChar;
        char*  aString;
        bool   aBool;
        char*  aName;
        char*  aKeyword;
        char*  aDescription;
        char*  anExpression;
        char*  aCommand;
        char   aCharEscape;
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

    ConfigLexer (const std::string &arg_filename,
                 std::istream      *arg_yyin,
                 std::ostream      *arg_yyout = 0);

    ~ConfigLexer();

    // this gets the token - possibly by picking up previous tokens
    // tht have been returned to the token_stack
    Token getToken();
    void returnToken(Token &token);

    /* this method is written for us by flex */
    int getToken(TokenValue *tokenValue);

    std::string filename;
    int num_errors;
    std::vector<Token> token_stack;

    void error_handler(const char *format, ...);
    void dprintf(const char *format, ...);
    int stringToDate (const char* str);
    int getLineNumber() const;

    /**
     * gets a block of code terminated with an unmatched }
     * we assume that the initial { has already been read from file
     * however any extra { within the code will increment the count
     * note that comments and strings are not parsed correctly at
     * present which could be a bit of a problem
     *
     * probably we need to parse using the parser, detect the point
     * within the stream where the terminating token '}' appears
     * and then get that part of the file - could be a bit tricky
     * to do correctly
     */
    VerbatimConstSP getVerbatim(
        char incrementor = '{',
        char terminator = '}',
        const char* extras="");

private:
    void clear_buffer();
    std::ostringstream m_buffer;
    bool m_inVerbatim;
    std::ostringstream m_verbatim;
};

std::ostream& operator << (std::ostream&out,
                           const ConfigLexer::Token& token);

#endif


