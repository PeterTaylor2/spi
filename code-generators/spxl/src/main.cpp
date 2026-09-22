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

    const char* longOptions = "help nameAtEnd noGeneratedCodeNotice upperCase funcNameSep= noObjectFuncs noPrefixObjectFuncs parent= "
        " optionsFile= license licenseFile= errIsNA backup xlTargetVersion= nsUpperCase";

    try
    {
        std::vector<spi_util::CommandLineOption> clOptions = {
            { "help", "h", "Print this help message and exit"},
            { "optionsFile", "", "Defines an options file written in JSON which defines various options.\n"
                "If you want to change the names of the functions that are written on your behalf for manipulating\n"
                "objects in a generic manner, then the optionsFile approach is the one that you should be using.\n"
                ":\n"
                "These options are available with the optionsFile: funcNameSep, noGeneratedCodeNotice, nameAtEnd,\n"
                "upperCase, nsUpperCase, noObjectFuncs, errIsNA, xlVersion.\n"
                "These options can also be specified independently on the command line.\n"
                ":\n"
                "These options can only be defined via the optionsFile - they define the names for particular functions\n"
                "that are generated for all services:\n"
                "helpFunc, hepFuncList, helpEnum, objectCoerce, startLogging, stopLogging, startTiming, stopTiming,\n"
                "clearTimings, getTimings, setErrorPopups, objectToString, objectFromString, objectGet, objectPut,\n"
                "objectToFile, objectFromFile, objectCount, objectFree, objectFreeAll, objectList, objectSHA\n",
                true},
            { "nameAtEnd", "", "Functions which return objects in the Excel API are returned as strings to the\n"
                "spreadsheet. You need to provide a name which will form part of this string - the so-called\n"
                "baseName. By default, the name is required as the first parameter, but it is actually more\n"
                "natural to have the baseName as the final parameter. Initially (i.e. when the SPI project was started)\n"
                "defining the name at the end had the problem that if you added extra (hopefully optional) parameters\n"
                "over time, then existing Excel spreadsheets would now be providing the base name of the object in the\n"
                "position where we expected the new function input. Essentially this would break the Excel spreadsheet.\n"
                "However we solved this problem - the key is that if the name is defined at the end, then we assume\n"
                "that the last parameter entered is the baseName, and that any missing parameters are assumed to take\n"
                "their default value. Hence we recommend turning on the nameAtEnd feature."},
            { "noObjectFuncs", "", "If defined, then we do not define any of the object functions, e.g. objectToString\n"
                "etc. See the optionsFile parameter for a list of the possible object functions.\n"
                "You might set this flag if another of the Excel add-ins contains all the object functions.\n"},
            { "noPrefixObjectFuncs", "", "If defined, then we do not use the namespace prefix for the object functions.\n"},
            { "parent", "", "This option is deprecated and ignored", true },
            { "errIsNA", "", "Use this option to set error results as #N/A as opposed to the default which is #NUM!\n" },
            { "xlTargetVersion", "", "Defines which version of Excel we are targetting. By default 12.\n"
                "Other possible values are 4 and 15, but 4 is now really old-fashioned and somewhat restrictive.\n", true},
            { "nsUpperCase", "", "The namespace component of the name is converted to upper case."},
            { "upperCase", "", "Convert all function names entirely to upper case."},
            { "funcNameSep", "", "Defines the separator that is used join together parts of the function name.\n"
                "The default value is '.' - one which we have seen quite often used is '_'", true},
            { "backup", "", "Creates a backup file (original name with .bak appended) whenever a file is changed." },
            { "noGeneratedCodeNotice", "", "Do not print the generated code notice at the top of each file in the generated code\n"
                "In addition we also trigger noVerbatimLine option"},
            { "license", "", "Prints the license for SPCS."},
            { "licenseFile", "", "This is the name of a file which contains the license for your code."},
            { "", "w", "Waits at the start - the purpose is to allow a debugger to be attached"},
            { "", "v", "Verbose - this appears to have no effect"},
        };

        spi_util::CommandLine commandLine(argc, argv, "wv", longOptions);
        exe = spi_util::path::basename(commandLine.exeName);

        std::string opt;
        std::string val;
        while (commandLine.getOption(opt,val))
        {
            if (opt == "-h" || opt == "--help")
            {
                spi_util::CommandLineOption::PrintHelp(
                    stdout,
                    exe.c_str(),
                    "infile outfile dirname indirvba",
                    clOptions);
                return 0;
            }
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
            else if (opt == "--noPrefixObjectFuncs")
            {
                options.noPrefixObjectFuncs = true;
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

