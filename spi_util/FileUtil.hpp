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
#ifndef SPI_FILE_UTIL_HPP
#define SPI_FILE_UTIL_HPP

/*
***************************************************************************
* FileUtil.hpp
***************************************************************************
* Some file manipulation functions - starting off with file names and
* directory names.
***************************************************************************
*/

#include "DeclSpec.h"
#include "Namespace.hpp"

#include <string>
#include <vector>
#include <set>
#include <deque>

SPI_UTIL_NAMESPACE

namespace path
{
    /// returns the current directory
    SPI_UTIL_IMPORT
    std::string getcwd();

    /// extracts the directory name from the path
    SPI_UTIL_IMPORT
    std::string dirname(const std::string& path);

    /// extracts the base file name from the path
    SPI_UTIL_IMPORT
    std::string basename(const std::string& path);

    /// joins a number of components to create a full file name
    /// you need to use 0 to end the inputs
    SPI_UTIL_IMPORT
    std::string join(const char* path, ...);

    /// returns the valid posix filename (using / as separator)
    SPI_UTIL_IMPORT
    std::string posix(const std::string& path);

    /// returns the vali win32 filename (using \ as separator)
    SPI_UTIL_IMPORT
    std::string win32(const std::string& path);

    /// converts to the current operating system (decision based on compiler)
    SPI_UTIL_IMPORT
    std::string convert(const std::string& path);

    /// returns the directory separator (decision based on compiler)
    SPI_UTIL_IMPORT
    char dirsep();

    /// normalises a path
    SPI_UTIL_IMPORT
    std::string normpath(const std::string& path);

    /**
     * Returns the absolute path of a file. Essentially this gives the
     * same answer as joining the current working directory with the path
     * and using normpath on the result.
     */
    SPI_UTIL_IMPORT
    std::string abspath(const std::string& path);

    /**
     * Assuming we are currently in directory dn, this function returns
     * the relative path to get to the path described by path.
     */
    SPI_UTIL_IMPORT
    std::string relativePath(const std::string& path, const std::string& dn);

    /**
     * Returns true if the path represents a directory
     */
    SPI_UTIL_IMPORT
    bool isdir(const std::string& path);

    /**
     * Returns true if the path represents a file
     */
    SPI_UTIL_IMPORT
    bool isfile(const std::string& path);
} // end of namespace path

/**
 * Returns the last update time for a file.
 *
 * The integer component matches the date serial number used within DateUtil.hpp
 *
 * Returns 0 if the file does not exist.
 */
SPI_UTIL_IMPORT
double FileLastUpdateTime(const std::string& fn);

/**
 * Directory contains the contents of a directory.
 * It will exclude . and .. and separate entries into files and directories.
 */
struct SPI_UTIL_IMPORT Directory
{
    std::string dn;
    std::set<std::string> fns;
    std::set<std::string> dns;

    Directory();
    Directory(const std::string& dn);
};

/**
 * DirectoryWalker is to be used as follows:
 *
 * std::string dn; // initialise to the directory of interest
 * DirectoryWalker walker(dn);
 * Directory* d = walker.Next();
 * while (d)
 * {
 *     // process directory using d->dirName, d->fileNames, d->dirNames
 *     // dirNames is a set and you can remove directories from it in
 *     // order to restrict the further directories from the search
 *
 *     d = walker.Next();
 * }
 *
 * alternatively:
 *
 * for (Directory* d = walker.Next(); d; d = walker.Next())
 * {
 *    // process directory d as above
 * }
 *
 */
class SPI_UTIL_IMPORT DirectoryWalker
{
public:
    // initialises the directory walker
    DirectoryWalker(
        const std::string& dirName,
        const std::set<std::string>& excludeDirNames=std::set<std::string>(),
        bool depthFirst=false);

    /** Get details about the next directory. Returns NULL if there are no
        more directories.

        You can remove items from dirNames and as a result the directory
        walker will not go into those directories.
    */
    Directory* Next();

private:

    // this is the directory returned by Next()
    Directory m_current;

    std::string             m_dirName;
    std::set<std::string>   m_excludeDirNames;
    std::deque<std::string> m_unvisited;
    bool m_depthFirst;
    bool m_started;
};

SPI_UTIL_IMPORT
void MakeDirectory(const std::string& dn);

SPI_UTIL_IMPORT
void MakeDirectories(const std::string& dn);

SPI_UTIL_END_NAMESPACE

#endif
