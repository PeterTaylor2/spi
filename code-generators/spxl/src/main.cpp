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

#include "xlWriter.hpp"
#include <spgtools/licenseTools.hpp>

static void print_usage(std::ostream& ostr, const std::string& exe, const char* longOptions)
{
    ostr << "USAGE: " << exe << " [-w] [-v] [longOptions] <infile> <outfile> <dirname> <indirvba>\n\n";
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
            spi::StringEndsWith(*iter, ".cpp") ||
            spi::StringEndsWith(*iter, ".bas") ||
            spi::StringEndsWith(*iter, ".frm") ||
            spi::StringEndsWith(*iter, ".frx"))
        {
            std::string ffn = spi_util::path::join(dn.c_str(), iter->c_str(), 0);
            if (!fns.count(ffn))
            {
                std::cout << "Removing " << ffn << std::endl;
                remove(ffn.c_str());
            }
        }
    }

    // repeat for parent directory since at one point we also wrote header files there
    std::string dnParent = spi_util::path::dirname(dn);
    spi_util::Directory dParent(dnParent);
    for (iter = dParent.fns.begin(); iter != dParent.fns.end(); ++iter)
    {
        if (spi::StringEndsWith(*iter, ".h") ||
            spi::StringEndsWith(*iter, ".hpp"))
        {
            std::string ffn = spi_util::path::join(dnParent.c_str(), iter->c_str(), 0);
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
    const std::string& outdir,
    const std::string& indirvba,
    const Options& options,
    bool verbose)
{
    spdoc::spdoc_start_service();
    spdoc::ServiceConstSP serviceDoc = spdoc::Service::from_file(infilename.c_str());

    ExcelServiceConstSP service = ExcelService::Make(serviceDoc, options);

    size_t nbModules = serviceDoc->modules.size();
    std::vector<ExcelModuleConstSP> modules;
    for (size_t i = 0; i < nbModules; ++i)
    {
        spdoc::ModuleConstSP moduleDoc = serviceDoc->modules[i];
        modules.push_back(ExcelModule::Make(service, moduleDoc));
    }

    std::set<std::string> fns;
    for (size_t i = 0; i < nbModules; ++i)
    {
        const ExcelModuleConstSP& module = modules[i];

        fns.insert(module->writeHeaderFile(outdir));
        fns.insert(module->writeSourceFile(outdir));
    }

    fns.insert(service->writeDeclSpecHeaderFile(outdir));
    fns.insert(service->writeXllHeaderFile(outdir));
    fns.insert(service->writeXllSourceFile(outdir));
    fns.insert(service->writeVbaFile(outdir));

    std::vector<std::string> vbaFns = service->translateVbaFiles(
        outdir, indirvba);

    for (size_t i = 0; i < vbaFns.size(); ++i)
        fns.insert(vbaFns[i]);

    tidyup(serviceDoc, outdir, fns);

    serviceDoc->to_file(outfilename.c_str());

    return 0;
}

int main(int argc, char* argv[])
{
    bool waitAtStart = false;
    bool verbose = false;

    bool nameAtEnd = false;

    std::string infilename;
    std::string outfilename;
    std::string outdir;
    std::string indirvba;

    std::string exe("SPXL");

    Options options;

    const char* longOptions = "nameAtEnd noGeneratedCodeNotice upperCase funcNameSep= noObjectFuncs parent="
        " optionsFile= license licenseFile= errIsNA backup xlTargetVersion= nsUpperCase";

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
                verbose = true;
            }
            else if (opt == "--parent")
            {
                std::cerr << "ignoring deprecated parameter --parent" << std::endl;
            }
            else if (opt == "--nameAtEnd")
            {
                options.nameAtEnd = true;
            }
            else if (opt == "--noGeneratedCodeNotice")
            {
                options.noGeneratedCodeNotice = true;
            }
            else if (opt == "--upperCase")
            {
                options.upperCase = true;
            }
            else if (opt == "--nsUpperCase")
            {
                options.nsUpperCase = true;
            }
            else if (opt == "--funcNameSep")
            {
                options.funcNameSep = val;
            }
            else if (opt == "--errIsNA")
            {
                options.errIsNA = true;
            }
            else if (opt == "--optionsFile")
            {
                options.update(val);
            }
            else if (opt == "--license")
            {
                printBanner(exe, true);
            }
            else if (opt == "--licenseFile")
            {
                options.license = readLicenseFile(val);
            }
            else if (opt == "--noObjectFuncs")
            {
                options.noObjectFuncs = true;
            }
            else if (opt == "--backup")
            {
                options.writeBackup = true;
            }
            else if (opt == "--xlTargetVersion")
            {
                options.xlTargetVersion = spi_util::StringToInt(val);
            }
            else
            {
                std::cerr << "Unrecognised option: " << opt << std::endl;
                print_usage(std::cerr, exe, longOptions);
                return -1;
            }
        }

        if (commandLine.args.size() != 4)
        {
            print_usage(std::cerr, exe, longOptions);
            return -1;
        }

        infilename  = commandLine.args[0];
        outfilename = commandLine.args[1];
        outdir      = commandLine.args[2];
        indirvba    = commandLine.args[3];
    }
    catch (std::exception& e)
    {
        print_usage(std::cerr, "SPXL", longOptions);
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
        int status = run(infilename, outfilename, outdir, indirvba, options, verbose);
        return status;
    }
    catch (std::exception &e)
    {
        fprintf(stderr, "%s\n", e.what());
        return -1;
    }
}

