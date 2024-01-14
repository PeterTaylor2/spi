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

const char* copyright = "Copyright (C) 2012-2023 Sartorial Programming Ltd.";

#include <stdarg.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <spi/RuntimeError.hpp>
#include <spi/StringUtil.hpp>
#include <spi_util/FileUtil.hpp>
#include <spi_util/CommandLine.hpp>
#include <spi/Service.hpp>

#include <spi/spdoc_configTypes.hpp>
#include <spi/spdoc_dll_service.hpp>

#include "texService.hpp"
#include "texOptions.hpp"
#include <spgtools/licenseTools.hpp>

static void print_usage(std::ostream& ostr, const std::string& exe, const char* longOptions)
{
    ostr << "USAGE: " << exe << " [-w] [-v] [-i <dnImports>] [-S <summary>] <infile> <outfile> <dirname> [<extras...>]\n\n";
    ostr << "where longOptions can be as follows:\n\t--"
        << spi_util::StringReplace(longOptions, " ", "\n\t--") << std::endl;
}

static int run(
    const std::string& infilename,
    const std::string& outfilename,
    const std::string& dirname,
    const std::vector<std::string>& extras,
    const std::vector<std::string>& dnImports,
    const std::string& summaryfilename,
    const Options& getOptions)
{
    spi::ServiceSP docService = spdoc::spdoc_start_service();
    spdoc::ServiceConstSP serviceDoc = spdoc::Service::from_file(infilename);
    std::vector<spdoc::ServiceConstSP> extraServiceDocs;
    for (size_t i = 0; i < extras.size(); ++i)
        extraServiceDocs.push_back(spdoc::Service::from_file(extras[i]));

    setOptions(getOptions);
    writeTexService(dirname, serviceDoc, dnImports, extraServiceDocs);

    serviceDoc->to_file(outfilename.c_str());

    if (!summaryfilename.empty())
    {
        std::vector<spdoc::ServiceConstSP> services;
        services.push_back(serviceDoc);
        services.insert(services.end(), extraServiceDocs.begin(), extraServiceDocs.end());
        const std::vector<std::string>& summary = spdoc::Service::combineSummaries(services, true);

        std::cout << "writing " << summaryfilename << std::endl;
        std::ofstream sfp(summaryfilename.c_str());
        if (sfp)
        {
            size_t numLines = summary.size();
            for (size_t i = 0; i < numLines; ++i)
            {
                sfp << summary[i] << "\n";
            }
            sfp.close();
        }
        else
        {
            std::cerr << "WARNING: Could not open " << summaryfilename << std::endl;
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
    bool waitAtStart = false;

    std::string infilename;
    std::string outfilename;
    std::string dirname;

    std::string summaryfilename;
    std::vector<std::string> extras;
    std::vector<std::string> dnImports;

    Options getOptions;
    std::string exe("SPTEX");
    const char* longOptions = "license writeIncludes writeBackup";

    try
    {
        spi_util::CommandLine commandLine(argc, argv, "wvi=S=", longOptions);
        exe = spi_util::path::basename(commandLine.exeName);

        std::string opt;
        std::string val;
        while (commandLine.getOption(opt,val))
        {
            if (opt == "-w")
            {
                waitAtStart = true;
            }
            else if (opt == "-v")
            {
                getOptions.verbose = true;
            }
            else if (opt == "-i")
            {
                dnImports.push_back(val);
            }
            else if (opt == "-S")
            {
                summaryfilename = val;
            }
            else if (opt == "--license")
            {
                printBanner(exe, true);
            }
            else if (opt == "--writeIncludes")
            {
                getOptions.writeIncludes = true;
            }
            else if (opt == "--writeBackup")
            {
                getOptions.writeBackup = true;
            }
            else
            {
                std::cerr << "Unrecognised option: " << opt << std::endl;
                print_usage(std::cerr, exe, longOptions);
                return -1;
            }
        }

        if (commandLine.args.size() < 3)
        {
            print_usage(std::cerr, exe, longOptions);
            return -1;
        }

        infilename  = commandLine.args[0];
        outfilename = commandLine.args[1];
        dirname     = commandLine.args[2];
        for (size_t i = 3; i < commandLine.args.size(); ++i)
            extras.push_back(commandLine.args[i]);
    }
    catch (std::exception& e)
    {
        print_usage(std::cerr, exe, longOptions);
        std::cerr << "ERROR: " << e.what() << std::endl;
        return -1;
    }

    printBanner(exe);

    if (waitAtStart)
    {
        char buf[128];
        std::cout << "Enter to continue:";
        std::cin >> buf;
    }

    try
    {
        int status = run(infilename, outfilename, dirname, extras, dnImports, summaryfilename, getOptions);
        return status;
    }
    catch (std::exception &e)
    {
        fprintf(stderr, "%s\n", e.what());
        return -1;
    }
}

