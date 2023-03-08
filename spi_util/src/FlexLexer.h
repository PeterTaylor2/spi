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
// -*-C++-*-
// FlexLexer.h -- define interfaces for lexical analyzer classes generated
// by flex

// Copyright (c) 1993 The Regents of the University of California.
// All rights reserved.
//
// This code is derived from software contributed to Berkeley by
// Kent Williams and Tom Epperly.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:

//  1. Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the distribution.

//  Neither the name of the University nor the names of its contributors
//  may be used to endorse or promote products derived from this software
//  without specific prior written permission.

//  THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE.

// This file defines FlexLexer, an abstract class which specifies the
// external interface provided to flex C++ lexer objects, and yyFlexLexer,
// which defines a particular lexer class.
//
// If you want to create multiple lexer classes, you use the -P flag
// to rename each yyFlexLexer to some other xxFlexLexer.  You then
// include <FlexLexer.h> in your other sources once per lexer class:
//
//	#undef yyFlexLexer
//	#define yyFlexLexer xxFlexLexer
//	#include <FlexLexer.h>
//
//	#undef yyFlexLexer
//	#define yyFlexLexer zzFlexLexer
//	#include <FlexLexer.h>
//	...

/**
 * Note that for use within SPI we will simply put the SPI_NAMESPACE
 * around everything. However this is quite difficult to get to compile,
 * so we will do without namespaces for the moment. Essentially this code
 * does not get exported, so the namespace is not really necessary.
 *
 * So I commented out some more stuff about multiple definitions of
 * yyFlexLexer and changed the header file guard to be SPI specific.
 *
 * I also removed the extern C++ stuff
 *
 * The original FlexLexer.h file was found in /usr/include after I
 * had installed flex on WSL. This was flex version 2.6.4
 */

#ifndef _SPI_UTIL_FLEX_LEXER_H
#define _SPI_UTIL_FLEX_LEXER_H

#include "Namespace.hpp"

#include <iostream>

struct yy_buffer_state;
typedef int yy_state_type;

class FlexLexer
{
public:
    virtual ~FlexLexer() { }

    const char* YYText() const { return yytext; }
    int YYLeng()        const { return yyleng; }

    virtual void
        yy_switch_to_buffer(yy_buffer_state* new_buffer) = 0;
    virtual yy_buffer_state* yy_create_buffer(std::istream* s, int size) = 0;
    virtual yy_buffer_state* yy_create_buffer(std::istream& s, int size) = 0;
    virtual void yy_delete_buffer(yy_buffer_state* b) = 0;
    virtual void yyrestart(std::istream* s) = 0;
    virtual void yyrestart(std::istream& s) = 0;

    virtual int yylex() = 0;

    // Call yylex with new input/output sources.
    int yylex(std::istream& new_in, std::ostream& new_out)
    {
        switch_streams(new_in, new_out);
        return yylex();
    }

    int yylex(std::istream* new_in, std::ostream* new_out = 0)
    {
        switch_streams(new_in, new_out);
        return yylex();
    }

    // Switch to new input/output streams.  A nil stream pointer
    // indicates "keep the current one".
    virtual void switch_streams(std::istream* new_in,
        std::ostream* new_out) = 0;
    virtual void switch_streams(std::istream& new_in,
        std::ostream& new_out) = 0;

    int lineno() const { return yylineno; }

    int debug() const { return yy_flex_debug; }
    void set_debug(int flag) { yy_flex_debug = flag; }

protected:
    char* yytext;
    int yyleng;
    int yylineno;       // only maintained if you use %option yylineno
    int yy_flex_debug;  // only has effect with -d or "%option debug"
};

class yyFlexLexer : public FlexLexer {
public:
    // arg_yyin and arg_yyout default to the cin and cout, but we
    // only make that assignment when initializing in yylex().
    yyFlexLexer(std::istream& arg_yyin, std::ostream& arg_yyout);
    yyFlexLexer(std::istream* arg_yyin = 0, std::ostream* arg_yyout = 0);
private:
    void ctor_common();

public:

    virtual ~yyFlexLexer();

    void yy_switch_to_buffer(yy_buffer_state* new_buffer);
    yy_buffer_state* yy_create_buffer(std::istream* s, int size);
    yy_buffer_state* yy_create_buffer(std::istream& s, int size);
    void yy_delete_buffer(yy_buffer_state* b);
    void yyrestart(std::istream* s);
    void yyrestart(std::istream& s);

    void yypush_buffer_state(yy_buffer_state* new_buffer);
    void yypop_buffer_state();

    virtual int yylex();
    virtual void switch_streams(std::istream& new_in, std::ostream& new_out);
    virtual void switch_streams(std::istream* new_in = 0, std::ostream* new_out = 0);
    virtual int yywrap();

protected:
    virtual int LexerInput(char* buf, int max_size);
    virtual void LexerOutput(const char* buf, int size);
    virtual void LexerError(const char* msg);

    void yyunput(int c, char* buf_ptr);
    int yyinput();

    void yy_load_buffer_state();
    void yy_init_buffer(yy_buffer_state* b, std::istream& s);
    void yy_flush_buffer(yy_buffer_state* b);

    int yy_start_stack_ptr;
    int yy_start_stack_depth;
    int* yy_start_stack;

    void yy_push_state(int new_state);
    void yy_pop_state();
    int yy_top_state();

    yy_state_type yy_get_previous_state();
    yy_state_type yy_try_NUL_trans(yy_state_type current_state);
    int yy_get_next_buffer();

    std::istream yyin;  // input source for default LexerInput
    std::ostream yyout; // output sink for default LexerOutput

    // yy_hold_char holds the character lost when yytext is formed.
    char yy_hold_char;

    // Number of characters read into yy_ch_buf.
    int yy_n_chars;

    // Points to current character in buffer.
    char* yy_c_buf_p;

    int yy_init;                // whether we need to initialize
    int yy_start;               // start state number

    // Flag which is used to allow yywrap()'s to do buffer switches
    // instead of setting up a fresh yyin.  A bit of a hack ...
    int yy_did_buffer_switch_on_eof;


    size_t yy_buffer_stack_top; /**< index of top of stack. */
    size_t yy_buffer_stack_max; /**< capacity of stack. */
    yy_buffer_state** yy_buffer_stack; /**< Stack as an array. */
    void yyensure_buffer_stack(void);

    // The following are not always needed, but may be depending
    // on use of certain flex features (like REJECT or yymore()).

    yy_state_type yy_last_accepting_state;
    char* yy_last_accepting_cpos;

    yy_state_type* yy_state_buf;
    yy_state_type* yy_state_ptr;

    char* yy_full_match;
    int* yy_full_state;
    int yy_full_lp;

    int yy_lp;
    int yy_looking_for_trail_begin;

    int yy_more_flag;
    int yy_more_len;
    int yy_more_offset;
    int yy_prev_more_offset;
};


#endif // _SPI_UTIL_FLEX_LEXER_H

