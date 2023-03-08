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
#include "CommandLine.hpp"
#include "RuntimeError.hpp"
#include "StringUtil.hpp"

#include <string.h>

#define BEGIN_ANONYMOUS_NAMESPACE namespace {
#define END_ANONYMOUS_NAMESPACE }

SPI_UTIL_NAMESPACE

BEGIN_ANONYMOUS_NAMESPACE

/**
 * Tries to match a possible optionCode.
 * If this fails, then an exception is thrown.
 * If it succeeds, then returns true if a parameter is expected and false
 * otherwise.
 */
bool matchLongOption(const std::string& optionCode,
                     const std::vector<std::string>& longOptions)
{
    std::string optionCodeEquals = optionCode + "=";

    for (size_t i = 0; i < longOptions.size(); ++i)
    {
        if (longOptions[i] == optionCode)
            return false;

        if (longOptions[i] == optionCodeEquals)
            return true;
    }

    throw RuntimeError("--%s not supported", optionCode.c_str());
}

bool matchShortOption(char option, const char* shortOptions)
{
    size_t n = strlen(shortOptions);

    for (size_t i = 0; i < n; ++i)
    {
        if (option == shortOptions[i])
        {
            if (shortOptions[i+1] == '=')
                return true;
            return false;
        }
        else if (shortOptions[i+1] == '=')
        {
            ++i;
        }
    }
    throw RuntimeError("-%c not supported", option);
}

int processOption(CommandLine& commandLine,
                  const std::string& arg,
                  int n,
                  int argc,
                  char* argv[],
                  const char* shortOptions,
                  const std::vector<std::string>& longOptions)
{
    SPI_UTIL_PRE_CONDITION(arg[0] == '-');

    bool isLongOption = arg.length() > 1 && arg[1] == '-';

    if (isLongOption)
    {
        std::vector<std::string> longOption = StringSplit(arg.substr(2), '=');
        SPI_UTIL_POST_CONDITION(longOption.size() >= 1);

        const std::string& optionCode = longOption[0];
        bool expectsValue = matchLongOption(optionCode, longOptions);

        std::string optionValue;
        if (expectsValue)
        {
            if (longOption.size() == 1)
            {
                // go to the next argument to get the parameter
                ++n;
                if (n >= argc)
                {
                    throw RuntimeError("No value for --%s",
                                       optionCode.c_str());
                }
                optionValue = argv[n];
            }
            else
            {
                std::ostringstream oss;
                oss << longOption[1];
                for (size_t i = 2; i < longOption.size(); ++i)
                    oss << '=' << longOption[i];
                optionValue = oss.str();
            }
        }
        else if (longOption.size() > 1)
        {
            throw RuntimeError("Unnecessary parameters: %s", arg.c_str());
        }
        commandLine.optionCodes.push_back("--" + optionCode);
        commandLine.optionValues.push_back(optionValue);
    }
    else
    {
        std::string shortOption = arg.substr(1);

        for (size_t i = 0; i < shortOption.length(); ++i)
        {
            bool expectsValue = matchShortOption(shortOption[i], shortOptions);

            std::string optionValue;
            if (expectsValue)
            {
                if (i+1 < shortOption.length())
                {
                    throw RuntimeError("Expects value for option code %c",
                                       shortOption[i]);
                }
                else
                {
                    ++n;
                    if (n >= argc)
                    {
                        throw RuntimeError("No value for -%c",
                                           shortOption[i]);
                    }
                    optionValue = argv[n];
                }
            }

            commandLine.optionCodes.push_back("-" + shortOption.substr(i,1));
            commandLine.optionValues.push_back(optionValue);
        }
    }

    ++n;

    return n;
}

END_ANONYMOUS_NAMESPACE

CommandLine::CommandLine()
    :
    exeName(),
    optionCodes(),
    optionValues(),
    args(),
    iter(0)
{}

CommandLine::CommandLine(
    int argc,
    char* argv[],
    const char* shortOptions,
    const char* longOptions)
    :
    exeName(),
    optionCodes(),
    optionValues(),
    args(),
    iter(0)
{
    SPI_UTIL_PRE_CONDITION(argc >= 1);

    exeName = argv[0];
    int n = 1;

    std::string optSep = "-";

    std::vector<std::string> longOptionVector = StringSplit(longOptions, ' ');

    while (n < argc)
    {
        std::string arg = argv[n];
        if (arg[0] != '-')
            break;

        n = processOption(*this, arg, n, argc, argv, shortOptions,
                          longOptionVector);
    }

    // remaining parameters returned as args
    while (n < argc)
    {
        args.push_back(argv[n]);
        ++n;
    }
}

bool CommandLine::getOption(std::string& optionCode, std::string& optionValue)
{
    SPI_UTIL_PRE_CONDITION(optionCodes.size() == optionValues.size());

    if (iter < optionCodes.size())
    {
        optionCode = optionCodes[iter];
        optionValue = optionValues[iter];
        ++iter;
        return true;
    }

    iter = 0;
    return false;
}

std::string CommandLine::toString()
{
    SPI_UTIL_PRE_CONDITION(optionCodes.size() == optionValues.size());

    std::ostringstream oss;

    oss << exeName;
    for (size_t i = 0; i < optionCodes.size(); ++i)
    {
        const std::string& optionCode = optionCodes[i];
        const std::string& optionValue = optionValues[i];
        if (StringStartsWith(optionCode, "--"))
        {
            oss << " " << optionCode;
            if (optionValue.length() > 0)
                oss << "=" << optionValue;
        }
        else
        {
            oss << " " << optionCode;
            if (optionValue.length() > 0)
                oss << " " << optionValue;
        }
    }

    for (size_t i = 0; i < args.size(); ++i)
        oss << " " << args[i];

    return oss.str();
}


//CommandLine ParseCommandLine(
//    int argc,
//    char* argv[],
//    const char* shortOptions,
//    const char* longOptions)
//{
//    SPI_UTIL_PRE_CONDITION(argc >= 1);
//
//    CommandLine out;
//
//    out.exeName = argv[0];
//    int n = 1;
//
//    std::string optSep = "-";
//
//    std::vector<std::string> longOptionVector = StringSplit(longOptions, ' ');
//
//    while (n < argc)
//    {
//        std::string arg = argv[n];
//        if (arg[0] != '-')
//            break;
//
//        n = processOption(out, arg, n, argc, argv, shortOptions,
//                          longOptionVector);
//    }
//
//    // remaining parameters returned as args
//    while (n < argc)
//    {
//        out.args.push_back(argv[n]);
//        ++n;
//    }
//
//    return out;
//}

SPI_UTIL_END_NAMESPACE
