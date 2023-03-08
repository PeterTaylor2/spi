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
** LexerImpl.hpp
***************************************************************************
** Header file for the LEX-generated lexer. This takes a file and
** returns a stream of tokens to be processed by the parser.
***************************************************************************
*/

#ifndef SPI_UTIL_LEXER_IMPL_HPP
#define SPI_UTIL_LEXER_IMPL_HPP

#include "Namespace.hpp"

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>

#include "FlexLexer.h"
#include "Lexer.hpp"

SPI_UTIL_NAMESPACE

class LexerImpl : public yyFlexLexer
{
public:

    LexerImpl (const std::string &arg_filename,
               std::istream      *arg_yyin,
               std::ostream      *arg_yyout = 0);

    ~LexerImpl();

    /* this method is written for us by flex */
    int getToken(Lexer::TokenValue *tokenValue);

    std::string filename;
    int num_errors;

    std::string get_errors();
    void error_handler(const char *format, ...);
    void dprintf(const char *format, ...);
    int stringToDate (const char* str);

private:
    std::ostringstream errors;
};

SPI_UTIL_END_NAMESPACE

#endif


