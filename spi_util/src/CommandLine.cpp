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
#include <sstream>

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

/***************************************************************************
 * Implementation of CommandLineOption - probably a better API than just
 * CommandLine but we want to re-use the existing code rather than re-write
 * and accidentally break it.
 ***************************************************************************/
CommandLineOption::CommandLineOption(
    const char* longName,
    const char* shortName,
    const char* help,
    bool hasValue,
    const char* valueName)
    :
    m_longName(longName),
    m_shortName(shortName),
    m_help(help),
    m_hasValue(hasValue),
    m_valueName()
{
    if (m_longName.empty())
    {
        m_hasLong = false;
    }
    else
    {
        m_hasLong = true;
        m_longName = StringStrip(m_longName);

        if (m_longName.length() == 0)
        {
            m_hasLong = false;
        }
        else
        {
            if (m_longName.find(' ') != std::string::npos)
            {
                SPI_UTIL_THROW_RUNTIME_ERROR("Long option name '" << m_longName
                    << "' cannot contain spaces");
            }
        }
    }

    if (m_shortName.empty())
    {
        m_hasShort = false;
    }
    else
    {
        if (m_shortName.length() != 1)
            SPI_UTIL_THROW_RUNTIME_ERROR("Short option name '" << m_shortName
                << "' should have length 1");

        m_hasShort = true;
    }

    if (m_hasValue)
    {
        if (!valueName)
        {
            m_valueName = m_longName;
        }
        else
        {
            m_valueName = valueName;
        }
    }
}

CommandLine CommandLineOption::FromVector(
    int argc,
    char* argv[],
    const std::vector<CommandLineOption>& options)
{
    std::ostringstream shortOptions;
    std::ostringstream longOptions;

    bool hasLongOptions = false;
    for (auto opt = options.begin(); opt != options.end(); ++opt)
    {
        if (opt->m_hasLong)
        {
            if (!hasLongOptions)
            {
                longOptions << " ";
                hasLongOptions = true;
            }
            longOptions << "--" << opt->m_longName;
            if (opt->m_hasValue)
                longOptions << "=";
        }
        else if (opt->m_hasShort)
        {
            shortOptions << opt->m_shortName;
            if (opt->m_hasValue)
                shortOptions << "=";
        }
    }

    std::string sOptions = shortOptions.str();
    std::string lOptions = longOptions.str();

    return CommandLine(argc, argv, sOptions.c_str(), lOptions.c_str());
}

void CommandLineOption::PrintHelp(
    FILE* fp,
    const char* exeName,
    const char* args,
    const std::vector<CommandLineOption>& options)
{
    PrintUsage(fp, exeName, args, options);

    fprintf(fp, "Help:\n");

    for (auto opt = options.begin(); opt != options.end(); ++opt)
    {
        if (!opt->m_hasLong && !opt->m_hasShort)
            continue;

        fprintf(fp, "\t");
        if (opt->m_hasShort)
        {
            fprintf(fp, "-%c", opt->m_shortName[0]);
            if (opt->m_hasValue)
            {
                fprintf(fp, " <%s>", opt->m_valueName.c_str());
            }
        }
        if (opt->m_hasLong)
        {
            if (opt->m_hasShort)
                fprintf(fp, " or ");

            fprintf(fp, "--%s", opt->m_longName.c_str());
            if (opt->m_hasValue)
            {
                fprintf(fp, " = <%s>", opt->m_valueName.c_str());
            }
        }
        fprintf(fp, "\n");

        std::vector<std::string> helpLines = StringSplit(opt->m_help, "\n");
        for (auto iter = helpLines.begin(); iter != helpLines.end(); ++iter)
        {
            if (iter->empty())
                continue;
            if (*iter == ":")
            {
                fprintf(fp, "\n");
            }
            else
            {
                fprintf(fp, "\t\t%s\n", iter->c_str());
            }

        }
    }
}

void CommandLineOption::PrintUsage(
    FILE* fp,
    const char* exeName,
    const char* args,
    const std::vector<CommandLineOption>& options)
{
    fprintf(fp, "\nUSAGE: %s", exeName);

    bool hasLongOptions = false;
    for (auto opt = options.begin(); opt != options.end(); ++opt)
    {
        if (opt->m_hasLong)
        {
            hasLongOptions = true;
        }
        else if (opt->m_hasValue)
        {
            fprintf(fp, " [-%c %s]", opt->m_shortName[0], opt->m_valueName.c_str());
        }
        else
        {
            fprintf(fp, " [-%c]", opt->m_shortName[0]);
        }
    }
    if (hasLongOptions)
    {
        fprintf(fp, " [longOptions]");
    }
    fprintf(fp, " %s\n", args);

    if (hasLongOptions)
    {
        fprintf(fp, "where longOptions can be as follows:\n");
        for (auto opt = options.begin(); opt != options.end(); ++opt)
        {
            if (!opt->m_hasLong)
                continue;

            fprintf(fp, "\t--%s", opt->m_longName.c_str());
            if (opt->m_hasValue)
            {
                fprintf(fp, "=<%s>", opt->m_valueName.c_str());
            }
            fprintf(fp, "\n");
        }
    }
    fprintf(fp, "\n");
    fflush(fp);
}

SPI_UTIL_END_NAMESPACE

