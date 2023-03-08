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
#include "TestRunner.hpp"
#include "CommandLine.hpp"
#include "StringUtil.hpp"
#include "ClockUtil.hpp"

#include <stdarg.h>
#include <stdio.h>

#include <iostream>
#include <map>
#include <vector>
#include <string>

SPI_UTIL_NAMESPACE

/**
 * Tries to call a test routine.
 *
 * Captures exceptions and prints to stderr, and returns an error count
 * of 1 or 0.
 */
static int tryTestRoutine(const char* name, TestRoutine* tester, int repeats)
{
    try
    {
        Clock clock;
        clock.Start();
        tester();
        while (repeats > 1)
        {
            --repeats;
            tester();
        }
        double totalTime = clock.Time();
        fprintf(stdout, "%s: Test succeeded (time:%f)\n", name, totalTime);
        return 0;
    }
    catch (std::exception &e)
    {
        fprintf(stderr, "%s: Test failed\n\n", name);
        fprintf(stderr, "%s\n", e.what());
        return 1;
    }
}

static void print_usage(char* progname)
{
    fprintf(stderr, "Usage: %s [-w] [-t] [-rN] (testnames...)\n", progname);
}

int TestRunner(int argc, char* argv[], ...)
{
    std::vector<std::string> testNames;
    std::map<std::string, TestRoutine*> testRoutines;

    va_list args;
    va_start (args, argv);
    while(true)
    {
        char* name = va_arg(args, char*);
        if (!name)
            break;

        std::string sname(name);
        TestRoutine* tester = va_arg(args, TestRoutine*);

        testNames.push_back(sname);
        testRoutines[sname] = tester;
    }
    va_end(args);

    bool waitAtStart = false;
    bool timings = false;
    int  repeats = 1;

    try
    {
        CommandLine commandLine(argc, argv, "wr=t");

        std::string optionCode;
        std::string optionValue;

        while (commandLine.getOption(optionCode, optionValue))
        {
            if (optionCode == "-w")
                waitAtStart = true;
            else if (optionCode == "-t")
                timings = true;
            else if (optionCode == "-r")
                repeats = StringToInt(optionValue);
            else
            {
                print_usage(argv[0]);
                return -1;
            }
        }

        if (commandLine.args.size() > 0)
        {
            testNames.clear();
            testNames = commandLine.args;
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return -1;
    }

    if (waitAtStart)
    {
        char buf[128];
        std::cout << "Enter to continue:";
        std::cin >> buf;
    }

    // timings not done

    int numErrors = 0;
    for (size_t i = 0; i < testNames.size(); ++i)
    {
        std::string name = testNames[i];
        std::map<std::string, TestRoutine*>::const_iterator iter;

        iter = testRoutines.find(name);
        if (iter == testRoutines.end())
        {
            fprintf(stderr, "Could not find test routine for %s\n",
                    name.c_str());
            return -1;
        }

        numErrors += tryTestRoutine(name.c_str(), iter->second, repeats);
    }

    return numErrors;
}

SPI_UTIL_END_NAMESPACE

