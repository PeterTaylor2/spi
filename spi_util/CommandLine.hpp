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
#ifndef SPI_COMMAND_LINE_HPP
#define SPI_COMMAND_LINE_HPP

/**
***************************************************************************
** CommandLine.hpp
***************************************************************************
** Parses the command line, returning options and parameters.
**
** The user specifies short options via a string of option letters, and
** long options via a string of strings separated by spaces.
**
** Short options can be included within the same parameter as long as the
** short options do not require parameters.
**
** If an option requires a parameter then it should be followed by '='
** in the string of options (short or long).
**
** As soon as a parameter is not an option (preceeded by - or --) then
** we revert to arguments.
**
** Options parameters can either be separated from the option indicator
** by use of '=' or a space.
***************************************************************************
*/

#include "DeclSpec.h"
#include "Namespace.hpp"

#include <string>
#include <vector>

SPI_UTIL_NAMESPACE

struct SPI_UTIL_IMPORT CommandLine
{
    std::string exeName;
    std::vector<std::string> optionCodes;
    std::vector<std::string> optionValues;
    std::vector<std::string> args;

    std::string toString();

    bool getOption(std::string& optionCode, std::string& optionValue);

    /**
     * Parses the command line into options and arguments.
     *
     * @param argc
     *   Number of arguments - standard for main functions for C/C++
     * @param argv
     *   Actual arguments - note that argv[0] is the name of the executable
     * @param shortOptions
     *   Short options which are defined using a single minus sign on the
     *   command line. Specified as a single letter, followed by an optional
     *   equal sign (=) which indicates that the option takes a value which is
     *   provided as the next argument on the command line.
     * @param longOptions
     *   Long options which are defined using two minus signs (--) on the
     *   command line. Specified as a space separated string, with an optional
     *   equal sign (=) which indicates that the options takes a value which
     *   needs to have an equal sign (=) on the command line to specify the
     *   value.
     */
    CommandLine(int argc, char* argv[], const char* shortOptions,
        const char* longOptions="");

private:
    CommandLine();
    size_t iter;
};

//CommandLine ParseCommandLine(
//    int argc,
//    char* argv[],
//    const char* shortOptions,
//    const char* longOptions="");

SPI_UTIL_END_NAMESPACE

#endif
