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
#include "clibParser.hpp"

#include <set>

#include "configLexer.hpp"
#include "parserTools.hpp"
#include "construct.hpp"
#include "moduleDefinition.hpp"
#include "attribute.hpp"
#include "innerClass.hpp"
#include "classMethod.hpp"

#include <spi/Service.hpp>
#include <spi/RuntimeError.hpp>
#include <spi/StringUtil.hpp>

#include <string.h>

namespace {

void parseFunction(
    const std::vector<std::string>& tokens,
    const std::map<std::string, int>& defines,
    ServiceDefinitionSP& service,
    bool verbose)
{}

void mainLoop(std::istream& istr,
              ServiceDefinitionSP& service,
              bool verbose)
{
    std::string word;
    const int BEGIN = 0;
    const int STANDARD_FUNCTIONS = 1;
    const int FUNCTIONS = 2;
    // const int END = 99;

    std::string libPrefix;
    std::string lastStandardFunction;
    std::vector<std::string> classNames;

    std::map<std::string, int> defines;

    int state = BEGIN;
    while (istr)
    {
        // this operation delimits on white space
        istr >> word;

        if (word == "typedef")
        {
            istr >> word;
            if (word == "struct")
            {
                std::string structName;
                std::string className;
                istr >> structName;
                istr >> word;
                if (spi::StringEndsWith(word, ";"))
                    className = word.substr(0, word.length()-1);
                else
                    className = word;
                if (structName.substr(1) == className &&
                    spi::StringStartsWith(className, "CLASS_"))
                {
                    className = className.substr(6);
                    std::cout << "CLASS: " << className << std::endl;
                    classNames.push_back(className);
                }
            }
        }

        if (word == "int" && state == BEGIN)
        {
            // first function is the INITIALIZE function
            // this will tell us the libPrefix
            istr >> word;
            if (word == "WINAPI")
            {
                istr >> word;
            }
            libPrefix = spi::StringSplit(word, '_')[0];
            std::cout << "PREFIX: " << libPrefix << std::endl;
            lastStandardFunction = libPrefix + "_FREE";
            state = STANDARD_FUNCTIONS;
        }

        if (state == STANDARD_FUNCTIONS && word == lastStandardFunction)
        {
            state = FUNCTIONS;
        }


        if (word == "#define" && state == FUNCTIONS)
        {
            std::string definition;
            std::string value;

            istr >> definition;
            istr >> value;

            std::cout << "#define " << definition << " " << value << std::endl;

            defines[definition] = spi::StringToInt(value);
        }

        if (state == FUNCTIONS && word == "int")
        {
            // need to parse a function
            // at present we will get the function name and parse to the ;
            std::string functionName;

            // tokens will exclude "int WINAPI" and the final ";"
            // they will include all brackets and comments
            std::vector<std::string> tokens;
            istr >> word;
            if (word != "WINAPI")
                throw spi::RuntimeError("Expected WINAPI after int");
            istr >> functionName;
            tokens.push_back(functionName);
            std::cout << "FUNCTION: " << functionName << std::endl;
            istr >> word;
            while (istr && word != ");")
            {
                if (word == "GTO_CONST")
                    tokens.push_back("const");
                else
                    tokens.push_back(word);
                istr >> word;
            }
            tokens.push_back(")");
            parseFunction(tokens, defines, service, verbose);
        }
    }
}

} // end of anonymous namespace



void clibParser(const std::string& fn,
                ServiceDefinitionSP& service,
                bool verbose)
{
    std::ifstream istr(fn.c_str());
    if (!istr)
        throw spi::RuntimeError("Could not open %s", fn.c_str());

    try
    {
        mainLoop(istr, service, verbose);
    }
    catch (std::exception &e)
    {
        istr.sync();
        long pos = (long)istr.tellg();
        int lineNumber = 1;
        if (pos > 0)
        {
            istr.seekg(0, std::ios::beg);
            char* buffer = new char[pos+1];
            istr.read(buffer, pos);
            buffer[pos] = '\0';
            while (pos > 0)
            {
                if (buffer[pos] == '\n')
                    ++lineNumber;
                --pos;
            }
            if (buffer[0] == '\n')
                ++lineNumber;
            delete[] buffer;
        }
        throw spi::RuntimeError("%s:%d: %s", fn.c_str(), lineNumber, e.what());
    }
}
