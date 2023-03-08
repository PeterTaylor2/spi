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

#include "pyWriter.hpp"
#include <spgtools/licenseTools.hpp>

static void print_usage(std::ostream& ostr, const std::string& exe, const char* longOptions)
{
    ostr << "USAGE: " << exe << " [-w] [-v] [longOptions] <infile> <outfile> <dirname>\n\n";
    ostr << "where longOptions can be as follows:\n\t--" 
        << spi_util::StringReplace(longOptions, " ", "\n\t--") << std::endl;
}

static void tidyup(
    const spdoc::ServiceConstSP& svc,
    const std::string& dn,
    const std::set<std::string>& fns)
{
    std::set<std::string>::const_iterator iter;

    spi_util::Directory d(dn);
    for (iter = d.fns.begin(); iter != d.fns.end(); ++iter)
    {
        if (spi::StringEndsWith(*iter, ".h") ||
            spi::StringEndsWith(*iter, ".hpp") ||
            spi::StringEndsWith(*iter, ".cpp"))
        {
            std::string ffn = spi_util::path::join(
                dn.c_str(), iter->c_str(), NULL);
            if (!fns.count(ffn))
            {
                std::cout << "Removing " << ffn << std::endl;
                remove(ffn.c_str());
            }
        }
    }
}

static int run(
    const std::string& infilename,
    const std::string& outfilename,
    const std::string& dirname,
    const Options& options)
{
    spi::ServiceSP docService = spdoc::spdoc_start_service();
    spdoc::ServiceConstSP serviceDoc = spdoc::Service::from_file(infilename.c_str());

    PythonServiceConstSP service = PythonService::Make(serviceDoc, options);

    size_t nbModules = serviceDoc->modules.size();
    std::vector<PythonModuleConstSP> modules;
    for (size_t i = 0; i < nbModules; ++i)
    {
        spdoc::ModuleConstSP moduleDoc = serviceDoc->modules[i];
        modules.push_back(PythonModule::Make(service, moduleDoc));
    }

    std::set<std::string> fns;
    for (size_t i = 0; i < nbModules; ++i)
    {
        const PythonModuleConstSP& module = modules[i];

        fns.insert(module->writeHeaderFile(dirname));
        fns.insert(module->writeSourceFile(dirname));
    }

    fns.insert(service->writeDeclSpecHeaderFile(dirname));
    fns.insert(service->writePydHeaderFile(dirname));
    fns.insert(service->writePydSourceFile(dirname));
    if (!options.noImporter)
        fns.insert(service->writePyImporter(dirname));

    tidyup(serviceDoc, dirname, fns);

    serviceDoc->to_file(outfilename.c_str());

    return 0;
}

int main(int argc, char* argv[])
{
    bool waitAtStart = false;

    std::string infilename;
    std::string outfilename;
    std::string dirname;

    std::string exe("SPPY");
    const char* longOptions = "noImporter noGeneratedCodeNotice lowerCase lowerCaseMethod"
        " objectCoerce helpFuncList license licenseFile=";

    Options options;

    try
    {
        spi_util::CommandLine commandLine(argc, argv, "wv", longOptions);
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
                options.verbose = true;
            }
            else if (opt == "--noImporter")
            {
                options.noImporter = true;
            }
            else if (opt == "--noGeneratedCodeNotice")
            {
                options.noGeneratedCodeNotice = true;
            }
            else if (opt == "--lowerCase")
            {
                options.lowerCase = true;
            }
            else if (opt == "--lowerCaseMethod")
            {
                options.lowerCaseMethod = true;
            }
            else if (opt == "--objectCoerce")
            {
                options.objectCoerce = true;
            }
            else if (opt == "--helpFuncList")
            {
                options.helpFuncList = true;
            }
            else if (opt == "--license")
            {
                printBanner(exe.c_str(), true);
            }
            else if (opt == "--licenseFile")
            {
                options.license = readLicenseFile(val);
            }
            else
            {
                std::cerr << "Unrecognised option: " << opt << std::endl;
                print_usage(std::cerr, exe, longOptions);
                return -1;
            }
        }


        if (commandLine.args.size() != 3)
        {
            print_usage(std::cerr, exe, longOptions);
            return -1;
        }

        infilename  = commandLine.args[0];
        outfilename = commandLine.args[1];
        dirname     = commandLine.args[2];
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

    // timings not done

    try
    {
        int status = run(infilename, outfilename, dirname, options);
        return status;
    }
    catch (std::exception &e)
    {
        fprintf(stderr, "%s\n", e.what());
        return -1;
    }
}

