/*

    GENDEP.EXE - Generates header file dependencies for C++
    Copyright (C) 2013 Sartorial Programming Ltd.

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

/*
***************************************************************************
* Generates header file dependencies for C++
*
* Given an include path, then the syntax is as follows:
*
* gendep.exe [-d outDir] [-s depSuffix] [-o objSuffix] [-I inc] [-f filter]
*    srcFile(s)
*
* Without the -d outDir it will generate the file in the current directory
* using the basename of the srcFile.
*
* Without the -s the dependency suffix is assumed to be d.
*
* Without the -o the object suffix is assumed to be obj on windows and o on
* linux (but no reason to run this on linux since we can just do gcc -MM -MP
* for the same effect).
*
* Any header files which are not found in the given include path will be
* ignored under the assumption that they are system includes and do not
* change from build to build.
*
* The -f option allows you to filter out directories containing header
* files which never change. If a file is found and it starts with the
* filter string then it is not written to file.
***************************************************************************
*/

#include <map>
#include <string>
#include <set>
#include <vector>
#include <fstream>
#include <iostream>

#include <time.h>

#include "getopt.h"
#include "RuntimeError.hpp"
#include "FileUtil.hpp"
#include "StringUtil.hpp"

static const char* argValue(char* argv[]);

static std::string baseName(
    const std::string& fn);

static std::string findInclude(
    const std::string& inc,
    const std::vector<std::string>& includePath,
    const std::string& cwd);

static std::set<std::string> findIncludes(
    const std::string& fn,
    const std::vector<std::string>& includePath,
    bool verbose);

static bool filterOut(
    const std::string& fn,
    const std::vector<std::string>& filters);

static void gendep(
    const std::string& fn,
    const std::vector<std::string>& includePath,
    const std::vector<std::string>& filters,
    const char* outDir,
    const char* depSuffix,
    const char* objSuffix,
    bool verbose,
    std::map<std::string, std::set< std::string> >& cache);

static void mainLoop(
    const std::vector<std::string>& fileNames,
    const std::vector<std::string>& includePath,
    const std::vector<std::string>& filters,
    const char* outDir,
    const char* depSuffix,
    const char* objSuffix,
    bool verbose);

int main(int argc, char* argv[])
{
    try
    {
        int opt;
        int numArgs;

        const char* outDir    = ".";
        const char* depSuffix = "d";
        const char* objSuffix = "obj";
        bool verbose = false;

        std::vector<std::string> includePath;
        std::vector<std::string> fileNames;
        std::vector<std::string> filters;

        while ((opt = getopt(argc, argv, "o:I:vs:d:f:")) != EOF)
        {
            switch(opt)
            {
            case 'd':
                outDir = argValue(argv);
                break;
            case 'I':
                includePath.push_back(std::string(argValue(argv)));
                break;
            case 'f':
                filters.push_back(std::string(argValue(argv)));
                break;
            case 'v':
                verbose = true;
                break;
            case 'o':
                objSuffix = argValue(argv);
                break;
            case 's':
                depSuffix = argValue(argv);
                break;
            }
        }

        numArgs = argc - optind;

        if (numArgs == 0)
            throw RuntimeError("No filenames provided in command line");

        while (optind < argc)
        {
            fileNames.push_back(std::string(argv[optind]));
            ++optind;
        }

        mainLoop(fileNames, includePath, filters, outDir, depSuffix,
                 objSuffix, verbose);
    }
    catch (std::exception& e)
    {
        fprintf(stderr, "%s\n", e.what());
        return -1;
    }
    return 0;
}

static const char* argValue(char* argv[])
{
    char* value = argv[optind-1];
    if (*value == '-')
        return value+2;
    return value;
}

/*
 * Strips of the final extension of the filename - i.e. the part of after the
 * final '.' in the name.
 */
static std::string baseName(
    const std::string& fn)
{
    std::string bn = path::basename(fn);
    std::vector<std::string> bnParts = StringSplit(bn, '.');
    if (bnParts.size() > 1)
        bnParts.pop_back();

    return StringJoin(".", bnParts);
}

/*
  Parses the remainder of a line that began with #include and returns
  the corresponding include file name by checking the includePath.

  Note that we do not change directory throughout this process. The
  original includePath is specified relative to the current working
  directory. However something in "..." within another header file will
  first check relative to the directory of the header file.
*/
static std::string findInclude(
    const std::string&              include,
    const std::vector<std::string>& includePath,
    const std::string&              cwd)
{
    bool searchCwd = false;
    std::string inc = StringStrip(include);
    std::string filename;
    if (StringStartsWith(inc, "<"))
    {
        std::vector<std::string> parts = StringSplit(inc.substr(1), '>');
        filename = parts[0];
    }
    else if (StringStartsWith(inc, "\""))
    {
        std::vector<std::string> parts = StringSplit(inc.substr(1), '"');
        filename = parts[0];
        searchCwd = true;
    }
    else
    {
        return std::string();
    }

    std::vector<std::string> searchPath;
    if (searchCwd)
        searchPath.push_back(cwd);

    searchPath.insert(searchPath.end(), includePath.begin(), includePath.end());

    for (size_t i = 0; i < searchPath.size(); ++i)
    {
        std::string ffn = path::normpath(
            path::join(searchPath[i].c_str(), filename.c_str(), 0));

        // TBA: find whether a file exists other than trying to open it
        // opening a file is potentially quite slow - some sort of stat
        // and checking the stat results would be quicker
        std::ifstream fp(ffn.c_str());
        if (fp)
            return ffn;
    }
    return std::string();
}


/*
 * Finds all the includes within an include file and returns as a set.
 */
static std::set<std::string> findIncludes(
    const std::string& fn,
    const std::vector<std::string>& includePath,
    bool verbose)
{
    std::set<std::string> includes;
    std::vector<std::string> lines;

    if (verbose)
        std::cout << "find includes for " << fn << std::endl;

    try
    {
        std::ifstream fp(fn.c_str());
        if (!fp)
        {
            throw RuntimeError(
                "Could not open %s for reading", fn.c_str());
        }
        while (fp)
        {
            std::string line;
            std::getline(fp, line);
            lines.push_back(line);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return includes;
    }

    for (size_t i = 0; i < lines.size(); ++i)
    {
        std::string line = StringStrip(lines[i]);
        if (StringStartsWith(line, "#include "))
        {
            std::string ifn = findInclude(line.substr(9), includePath,
                                          path::dirname(fn));
            if (!ifn.empty())
                includes.insert(ifn);
        }
    }

    if (verbose)
    {
        for (std::set<std::string>::const_iterator iter = includes.begin();
             iter != includes.end();
             ++iter)
        {
            std::cout << "... " << *iter << std::endl;
        }
    }

    return includes;
}

static bool filterOut(
    const std::string& fn,
    const std::vector<std::string>& filters)
{
    for (size_t i = 0; i < filters.size(); ++i)
    {
        if (StringStartsWith(fn, filters[i]))
            return true;
    }
    return false;
}

static void gendep(
    const std::string& fn,
    const std::vector<std::string>& includePath,
    const std::vector<std::string>& filters,
    const char* outDir,
    const char* depSuffix,
    const char* objSuffix,
    bool verbose,
    std::map<std::string, std::set< std::string> >& cache)
{
    /* find all the includes for a given file */
    /* we use caching to avoid searching the same file more than once */

    std::set<std::string> allIncludes;
    std::set<std::string> todoList = findIncludes(fn, includePath, verbose);

    while (todoList.size() > 0)
    {
        std::set<std::string>::iterator last = todoList.begin();
        std::string item = *last;
        todoList.erase(last);

        if (allIncludes.count(item) > 0)
            continue;

        if (filterOut(item, filters))
            continue;

        allIncludes.insert(item);

        if (cache.count(item) > 0)
        {
            const std::set<std::string>& cacheIncludes = cache[item];
            todoList.insert(cacheIncludes.begin(), cacheIncludes.end());
        }
        else
        {
            const std::set<std::string>& includes = findIncludes(
                item, includePath, verbose);
            cache[item] = includes;
            todoList.insert(includes.begin(), includes.end());
        }
    }

    std::string bfn = baseName(fn);
    std::string dfn = bfn + "." + depSuffix;
    std::string ofn = bfn + "." + objSuffix;

    dfn = path::join(outDir, dfn.c_str(), 0);
    ofn = path::join(outDir, ofn.c_str(), 0);

// makefiles use linux style file names
// hence we should change all file names to be posix-compliant when
// writing the file

    std::vector<std::string> posixIncludes;

    for (std::set<std::string>::const_iterator iter = allIncludes.begin();
         iter != allIncludes.end();
         ++iter)
    {
        posixIncludes.push_back(path::posix(*iter));
    }

    time_t rawtime;
    time (&rawtime);

    if (verbose)
        std::cout << dfn << std::endl;
    std::ofstream fp(dfn.c_str());

    if (!fp)
        throw RuntimeError("Could not open %s for writing", dfn.c_str());

    fp << "# header file dependency rules for " << path::basename(fn) << "\n"
       << "\n"
       << path::posix(ofn) << ": " << path::posix(fn);

    for (size_t i = 0; i < posixIncludes.size(); ++i)
    {
        fp << " \\\n " << posixIncludes[i];
    }
    fp << "\n\n";

    for (size_t i = 0; i < posixIncludes.size(); ++i)
    {
        fp << posixIncludes[i] << ":\n" << std::endl;
    }
}

static void mainLoop(
    const std::vector<std::string>& fileNames,
    const std::vector<std::string>& includePath,
    const std::vector<std::string>& filters,
    const char* outDir,
    const char* depSuffix,
    const char* objSuffix,
    bool verbose)
{
    std::map<std::string, std::set< std::string > > cache;
    for (size_t i = 0; i < fileNames.size(); ++i)
    {
        gendep(fileNames[i], includePath, filters, outDir, depSuffix,
               objSuffix, verbose, cache);
    }
}

