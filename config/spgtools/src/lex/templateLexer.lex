/*

Copyright (C) 2015 Sartorial Programming Ltd.

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

/*
 * Lexical analyzer for template files used in code generation.
 *
 * A template file is standard code with the addition of a specific
 * pattern for inserting specific text into the file.
 *
 * The pattern we will use is $(name)
 */
%{

#include <spi/platform.h>
#include "templateLexer.hpp"
#include <spi/StringUtil.hpp>

#include <stdarg.h> // for variable argument list methods
#include <string.h> // for strdup etc

#ifdef _MSC_VER
/* this disables the warning message about dividing by zero */
#pragma warning( disable : 4723 )
#define YY_NO_UNISTD_H
#define strdup _strdup
#endif

#define YY_DECL int TemplateLexer::getToken(TokenValue *tokenValue)

using namespace std; // to get cin and cout

%}

name     \$\([a-zA-Z_][a-zA-Z0-9_]*\)
dollar2  \$\$

/*%option yylineno*/

%%

[ \t\r\n] {
    tokenValue->aChar = yytext[0];
    return TEMPLATE_TOKEN_TYPE_CHAR; // preserve whitespace 
}

 /*
 * Strings - we do not allow strings to be continued over a line
 */
{name} {
    if (yytext[yyleng-1] != ')')
    {
        error_handler ("Unterminated name definition %s", yytext);
    }
    else
    {
        yytext[yyleng-1] = '\0';
        try
        {
            /* skip the first two characters $( */
            tokenValue->aName = strdup(yytext+2);
            dprintf ("NAME: %s\n", tokenValue->aName);
        }
        catch (std::exception &e)
        {
            error_handler("%s\n", e.what());
        }
    }
    return TEMPLATE_TOKEN_TYPE_NAME;
}

{dollar2} {
    tokenValue->aChar = '$';
    return TEMPLATE_TOKEN_TYPE_CHAR;
}

<<EOF>> {
    yyterminate();
}

. {
    tokenValue->aChar = yytext[0];
    return TEMPLATE_TOKEN_TYPE_CHAR;
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

void TemplateLexer::error_handler(const char *format, ...)
{
    ++num_errors;
    fprintf (stderr, "%s(%d): error: ", filename.c_str(), lineno());
    va_list ap;
    va_start (ap, format);
    vfprintf (stderr, format, ap);
    va_end (ap);
}

void TemplateLexer::dprintf(const char* format, ...)
{
#if YYDEBUG
    va_list ap;
    va_start (ap, format);
    vprintf (format, ap);
    va_end (ap);
#endif
}

TemplateLexer::TemplateLexer (const std::string &arg_filename,
                              std::istream      *arg_yyin,
                              std::ostream      *arg_yyout)
:
yyFlexLexer(arg_yyin, arg_yyout),
    filename(arg_filename),
    num_errors(0)
{}

TemplateLexer::~TemplateLexer() {}

TemplateLexer::Token TemplateLexer::getToken()
{
    int tokenType;
    TokenValue tokenValue;
    tokenType = getToken(&tokenValue);
    Token token(tokenType, tokenValue);

    return token;
}

namespace {

    TemplateLexer::TokenValue TokenValueCopy(
        int tokenType,
        TemplateLexer::TokenValue value)
    {
        TemplateLexer::TokenValue copy;

        switch (tokenType)
        {
        case TEMPLATE_TOKEN_TYPE_NAME:
            copy.aName = strdup(value.aName);
            break;
        default:
            copy = value;
            break;
        }
        return copy;
    }

    void TokenValueFree(
        int tokenType,
        TemplateLexer::TokenValue value)
    {
        switch(tokenType)
        {
        case TEMPLATE_TOKEN_TYPE_NAME:
            free(value.aName);
            break;
        default:
            break; // do nothing
        }
        memset(&value, 0, sizeof(TemplateLexer::TokenValue));
    }

} // end of anonymous namespace

TemplateLexer::Token::Token (int tokenType, TokenValue tokenValue)
    :
    type(tokenType),
    value(tokenValue)
{}

TemplateLexer::Token::Token ()
{
    type        = 0;
    value.aName = NULL;
}

TemplateLexer::Token::~Token()
{
    TokenValueFree(type, value);
}

TemplateLexer::Token::Token (const Token &other)
{
    type  = other.type;
    value = TokenValueCopy(other.type, other.value);
}

TemplateLexer::Token& TemplateLexer::Token::swap (TemplateLexer::Token &other)
{
    std::swap(type, other.type);
    std::swap(value, other.value);
    return *this;
}

TemplateLexer::Token& TemplateLexer::Token::operator= (const Token &other)
{
    TemplateLexer::Token tmp(other);
    swap(tmp);
    return *this;
}

std::string TemplateLexer::Token::toString() const
{
    switch (type)
    {
    case TEMPLATE_TOKEN_TYPE_CHAR:
        if (value.aChar == '$')
            return "$$";
        return std::string(1, value.aChar);
    case TEMPLATE_TOKEN_TYPE_NAME:
        return spi::StringFormat("<<<$(%s)>>>", value.aName);
    case 0:
        return std::string("EOF");
    }
    return std::string(); // should not get here
}

std::ostream& operator << (std::ostream& os, const TemplateLexer::Token& token)
{
    os << token.toString();
    return os;
}

int TemplateLexer::getLineNumber() const
{
    return yylineno;
}

