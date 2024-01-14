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

#include "serviceParser.hpp"
#include "options.hpp"
#include "cppGenerator.hpp"
#include "generatorTools.hpp"
#include <spgtools/commonTools.hpp>
#include "types_types.hpp"
#include "types_dll_service.hpp"
#include "serviceDefinition.hpp"
#include <spgtools/licenseTools.hpp>

#include <spi/RuntimeError.hpp>
#include <spi/StringUtil.hpp>
#include <spi_util/FileUtil.hpp>
#include <spi_util/CommandLine.hpp>

#define SPI_UTIL_CLOCK_EVENTS
#include <spi_util/ClockUtil.hpp>

#include <spi/spdoc_configTypes.hpp>
#include <spi/spdoc_dll_service.hpp>

static void print_standard_usage(FILE* fp, const std::string& exe, const char* longOptions)
{
    fprintf(fp, "\nUSAGE: %s [-w] [-c cdirname] [-t tdirname] [longOptions] (infile outfile dirname)\n\n", exe.c_str());
    std::vector<std::string> vLongOptions = spi_util::StringSplit(longOptions, " ");
    size_t N = vLongOptions.size();
    fprintf(fp, "where longOptions can be as follows:\n");
    for (size_t i = 0; i < N; ++i)
    {
        fprintf(fp, "\t--%s\n", vLongOptions[i].c_str());
    }
}

static void print_doc_usage(FILE* fp, const std::string& exe)
{
    fprintf(fp, "Usage: %s [-d] (texoutfile)\n", exe.c_str());
}

static void generate(
    const ServiceDefinitionSP& service,
    const std::string& outfilename,
    const std::string& outfilename2,
    const std::string& dirname,
    const Options& options,
    bool verbose)
{
    spdoc::ServiceConstSP serviceDoc = service->getDoc();
    serviceDoc->to_file(outfilename.c_str());

    types::TypesLibraryConstSP typesLibrary = types::TypesLibrary::Wrap(
        service->getTypesLibrary());
    typesLibrary->to_file(outfilename2.c_str());

    generateCpp(service, dirname, options);
}

static void writeParserDoc(
    const std::string& outfilename,
    bool verbose)
{
    GeneratedOutput ostr(outfilename, spi_util::path::dirname(outfilename), false);
    writeServiceParserDoc(ostr, verbose);
    ostr << "\n";
    ostr.close();
}

int main(int argc, char* argv[])
{
    bool waitAtStart = false;
    bool types = false;
    bool verbose = false;
    bool doc = false;
    bool cpp = false;
    bool clockEvents = false;

    Options options;

    std::string infilename;
    std::string outfilename;
    std::string outfilename2;
    std::string sfilename;
    bool sortSummary = false;
    std::string dirname;
    std::string tdirname;
    std::string cdirname;
    std::string version;

    std::string exe("SPCL");
    const char* longOptions = "allHeader noVerbatimLine noHeaderSplit noTidyup"
        " noGeneratedCodeNotice sortSummary version= optionalArgs license outputDir="
        " sessionLogging licenseFile= backup";

    try
    {
        spi_util::CommandLine commandLine(argc, argv, "wt=vdc=CS=", longOptions);
        exe = spi_util::path::basename(commandLine.exeName);

        std::string opt;
        std::string val;
        while (commandLine.getOption(opt,val))
        {
            if (opt == "-w")
            {
                waitAtStart = true;
            }
            else if (opt == "-t")
            {
                types = true;
                tdirname = val;
            }
            else if (opt == "-v")
            {
                verbose = true;
            }
            else if (opt == "-d")
            {
                doc = true;
            }
            else if (opt == "-c")
            {
                cpp = true;
                cdirname = val;
            }
            else if (opt == "--noVerbatimLine")
            {
                options.noVerbatimLine = true;
            }
            else if (opt == "--noGeneratedCodeNotice")
            {
                options.noVerbatimLine = true;
                options.noGeneratedCodeNotice = true;
            }
            else if (opt == "--allHeader")
            {
                options.allHeader = true;
            }
            else if (opt == "--sessionLogging")
            {
                options.sessionLogging = true;
            }
            else if (opt == "--noHeaderSplit")
            {
                options.noHeaderSplit = true;
            }
            else if (opt == "--noTidyup")
            {
                options.noTidyup = true;
            }
            else if (opt == "--version")
            {
                version = val;
            }
            else if (opt == "-C")
            {
                clockEvents = true;
            }
            else if (opt == "-S")
            {
                sfilename = val;
            }
            else if (opt == "--sortSummary")
            {
                sortSummary = true;
            }
            else if (opt == "--optionalArgs")
            {
                options.optionalArgs = true;
            }
            else if (opt == "--license")
            {
                printBanner(exe, true);
            }
            else if (opt == "--licenseFile")
            {
                std::string fn = val;
                options.license = readLicenseFile(val);
            }
            else if (opt == "--outputDir")
            {
                options.outputDir = val;
            }
            else if (opt == "--backup")
            {
                options.writeBackup = true;
            }
            else
            {
                fprintf(stderr, "Unsupported option: %s\n", opt.c_str());
                print_standard_usage(stderr, exe, longOptions);
                return -1;
            }
        }

        if (doc)
        {
            if (commandLine.args.size() != 1)
            {
                print_doc_usage(stderr, exe);
                return -1;
            }
            outfilename = commandLine.args[0];
        }
        else
        {
            printBanner(exe);
            if (commandLine.args.size() != 4)
            {
                print_standard_usage(stderr, exe, longOptions);
                return -1;
            }
            infilename   = commandLine.args[0];
            outfilename  = commandLine.args[1];
            outfilename2 = commandLine.args[2];
            dirname      = commandLine.args[3];
        }
    }
    catch (std::exception& e)
    {
        print_standard_usage(stderr, exe, longOptions);
        fprintf(stderr, "ERROR: %s\n", e.what());
        return -1;
    }

    if (waitAtStart)
    {
        char buf[128];
        std::cout << "Enter to continue:";
        std::cin >> buf;
    }

    if (clockEvents)
    {
        SPI_UTIL_CLOCK_EVENTS_START();
    }
    try
    {
        if (doc)
        {
            writeParserDoc(outfilename, verbose);
        }
        else
        {
            spdoc::spdoc_start_service();
            types::types_start_service();
            setGeneratorToolsOptions(options);
            ServiceDefinitionSP service = serviceParser(infilename, version, verbose);
            generate(service, outfilename, outfilename2, dirname, options, verbose);
            if (types)
                generateTypes(service, tdirname, options);
            if (cpp)
                generateConfig(service, cdirname);
            if (!sfilename.empty())
            {
                std::vector<std::string> summary = service->getDoc()->Summary(sortSummary);
                std::cout << "writing " << sfilename << std::endl;
                std::ofstream ofp(sfilename.c_str());
                size_t N = summary.size();
                for (size_t i = 0; i < N; ++i)
                    ofp << summary[i] << std::endl;
                ofp.close();
            }

            types::types_stop_service();
            spdoc::spdoc_stop_service();
        }
    }
    catch (std::exception &e)
    {
        fprintf(stderr, "%s\n", e.what());
        return -1;
    }
    if (clockEvents)
    {
        SPI_UTIL_CLOCK_EVENTS_WRITE("spcl-clock.txt");
    }
    return 0;
}

