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
#include "FileUtil.hpp"
#include "StringUtil.hpp"
#include "RuntimeError.hpp"
#include "Utils.hpp"

#include <spi_boost/shared_ptr.hpp>

#include <stdarg.h>

#include <algorithm>
#include <sstream>
#include <vector>
#include <sys/stat.h>

#ifdef _MSC_VER
#include <direct.h>
#define stat _stat

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

#else
#include <dirent.h>
#include <unistd.h>
#include "DateUtil.hpp"
#endif

#include <iostream>
#include <fstream>


#define BEGIN_ANONYMOUS_NAMESPACE namespace {
#define END_ANONYMOUS_NAMESPACE }

SPI_UTIL_NAMESPACE

namespace path
{
    BEGIN_ANONYMOUS_NAMESPACE
    bool isAbsolutePath(const std::string& path)
    {
#ifdef _MSC_VER
        if (path.length() >= 2)
            return path[1] == ':';
        return false;
#else
        if (path.length() >= 1)
            return path[0] == '/';
        return false;
#endif
    }
    END_ANONYMOUS_NAMESPACE

    std::string getcwd()
    {
#ifdef _MSC_VER
        char* cwd = _getcwd(NULL, 0);
#else
        char* cwd = ::getcwd(NULL, 0);
#endif
        if (!cwd)
        {
            throw RuntimeError("getcwd failed");
        }
        std::string out(cwd);
        free(cwd);
        return out;
    }

    std::string dirname(const std::string& path)
    {
        std::string work = posix(path);
        size_t pos = work.find_last_of('/');
        if (pos == std::string::npos)
        {
            return "";
        }
        if (pos == 0)
        {
            // there is a difference between /fn and fn
            // the first case fn is in the top level directory
            // the second case fn is in the current directory
            return "/";
        }
        return work.substr(0,pos);
    }

    std::string basename(const std::string& path)
    {
        std::string work = posix(path);
        size_t pos = work.find_last_of('/');
        if (pos == std::string::npos)
            return work;
        return work.substr(pos+1);
    }

    std::string join(const char* path, ...)
    {
        va_list args;
        std::ostringstream ostr;

        const char* sep = "";
        if (*path)
        {
            ostr << path;
            sep = "/";
        }
        va_start (args, path);
        while (true)
        {
            const char* name = va_arg(args, const char*);
            if (!name)
                break;

            if (*name)
            {
                ostr << sep << name;
                sep = "/";
            }
        }
        va_end(args);

        return ostr.str();
    }

    std::string posix(const std::string& path)
    {
        std::string result(path);
        replace(result.begin(), result.end(), '\\', '/');
        return result;
    }

    std::string win32(const std::string & path)
    {
        std::string result(path);
        replace(result.begin(), result.end(), '/', '\\');
        return result;
    }

    std::string convert(const std::string & path)
    {
#ifdef _MSC_VER
        return win32(path);
#else
        return posix(path);
#endif
    }

    char dirsep()
    {
#ifdef _MSC_VER
        return '\\';
#else
        return '/';
#endif
    }

    std::string normpath(const std::string& path)
    {
        std::vector<std::string> oldParts = StringSplit(posix(path), '/');
        std::vector<std::string> newParts;

        for (size_t i = 0; i < oldParts.size(); ++i)
        {
            const std::string& part = oldParts[i];
            if (part == ".")
                continue;
            if (part == ".." && newParts.size() > 0 && newParts.back() != "..")
                newParts.pop_back();
            else
                newParts.push_back(part);
        }
        return StringJoin("/", newParts);
    }


    /**
     * Returns the absolute path of a file. Essentially this gives the
     * same answer as joining the current working directory with the path
     * and using normpath on the result.
     */
    std::string abspath(const std::string& path)
    {
        if (isAbsolutePath(path))
            return normpath(path);

        std::string cwd = getcwd();
        std::string ffn = join(cwd.c_str(), path.c_str(), 0);
        return normpath(ffn);
    }

    /**
     * Assuming we are currently in directory dn, this function returns
     * the relative path to get to the path described by path.
     */
    std::string relativePath(
        const std::string& in_path,
        const std::string& in_dnFrom)
    {
        // first we must make the inputs into absolute paths
        std::string path   = abspath(in_path);
        std::string dnFrom = abspath(in_dnFrom);

        std::string dnTo = spi_util::path::dirname(path);

        std::vector<std::string> fromParts = StringSplit(dnFrom, '/');
        std::vector<std::string> toParts   = StringSplit(dnTo, '/');

        size_t common = 0;
        while (common < fromParts.size() && common < toParts.size())
        {
            if (fromParts[common] == toParts[common])
                ++common;
            else
                break;
        }

        std::vector<std::string> relativePath;

        for (size_t i = common; i < fromParts.size(); ++i)
            relativePath.push_back("..");

        for (size_t i = common; i < toParts.size(); ++i)
        {
            if (toParts[i].length() > 0)
                relativePath.push_back(toParts[i]);
        }

        relativePath.push_back(spi_util::path::basename(path));

        return StringJoin("/", relativePath);
    }

    bool isdir(const std::string& path)
    {
        struct stat statResult;
        if (stat(path.c_str(), &statResult) == 0)
        {
            if (statResult.st_mode & S_IFDIR)
                return true;
        }
        return false;
    }

    bool isfile(const std::string& path)
    {
        struct stat statResult;
        if (stat(path.c_str(), &statResult) == 0)
        {
            if (statResult.st_mode & S_IFREG)
                return true;
        }
        return false;
    }

} // end of namespace path

/**
 * Returns the last update time for a file.
 *
 * The integer component matches the date serial number used within DateUtil.hpp
 *
 * Returns 0 if the file does not exist.
 */
#ifdef _MSC_VER

static double DaysFromFileTime(const FILETIME* ft)
{
    // FILETIME is a struct consisting of a pair of 32-bit integers
    // we can cast to 64-bit integer and do some maths upon it
    __int64 value = *((const __int64*)ft);

    // value is number of 100 nano-second intervals since 1.1.1601
    // hence we don't need to convert the date component (it has the same base)
    double seconds = (double)value / 1e7;
    double days    = seconds / (60 * 60 * 24);

    return days;
}

double FileLastUpdateTime(const std::string& fn)
{
    HANDLE h;
    WIN32_FIND_DATA data;

    h = FindFirstFile(fn.c_str(), &data);

    if (h == INVALID_HANDLE_VALUE)
        return 0.0; // as promised

    FindClose(h);

    return DaysFromFileTime(&data.ftLastWriteTime);
}

#else

double FileLastUpdateTime(const std::string& fn)
{
    struct stat buf;
    if (stat(fn.c_str(), &buf) != 0)
        return 0.0;

    struct tm tm;
    gmtime_r(&buf.st_mtime, &tm);

    int date = YMDToDate(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    double seconds = (tm.tm_hour * 60 + tm.tm_min) * 60 + tm.tm_sec;

    double time = date + seconds / (60 * 60 * 24);

    return time;
}

#endif

BEGIN_ANONYMOUS_NAMESPACE

std::string FileReadContentsC(const char* filename)
{
    FILE* fp = fopen(filename, "rb");
    if (!fp)
    {
        SPI_UTIL_THROW_RUNTIME_ERROR("Could not open filename " << filename);
    }
    std::string contents;
    fseek(fp, 0, SEEK_END);
    contents.resize(ftell(fp));
    rewind(fp);
    fread(&contents[0], 1, contents.size(), fp);
    fclose(fp);
    return contents;
}

std::string FileReadContentsCPP(const char* filename)
{
    std::ifstream fp(filename, std::ios::in | std::ios::binary);
    if (!fp)
    {
        SPI_UTIL_THROW_RUNTIME_ERROR("Could not open filename " << filename);
    }
    std::string contents;
    fp.seekg(0, std::ios::end);
    contents.resize(IntegerCast<size_t>(fp.tellg()));
    fp.seekg(0, std::ios::beg);
    fp.read(&contents[0], contents.size());
    fp.close();
    return contents;
}

std::string FileReadContentsIterator(const char* filename)
{
    std::ifstream fp(filename, std::ios::in | std::ios::binary);
    if (!fp)
    {
        SPI_UTIL_THROW_RUNTIME_ERROR("Could not open filename " << filename);
    }
    return std::string(std::istreambuf_iterator<char>(fp), std::istreambuf_iterator<char>());
}

std::string FileReadContentsRdbuf(const char* filename)
{
    std::ifstream fp(filename, std::ios::in | std::ios::binary);
    if (!fp)
    {
        SPI_UTIL_THROW_RUNTIME_ERROR("Could not open filename " << filename);
    }
    std::ostringstream contents;
    contents << fp.rdbuf();
    fp.close();
    return contents.str();
}

END_ANONYMOUS_NAMESPACE

std::string FileReadContents(const char* filename, FileReadMethod method)
{
    SPI_UTIL_PRE_CONDITION(filename);

    switch(method)
    {
    case FILE_READ_METHOD_DEFAULT:
// various methods were tested in testReadFile.cpp
// it appears that C is better for Windows and CPP is better for Linux
#ifdef _MSC_VER
        return FileReadContentsC(filename);
#else
        return FileReadContentsCPP(filename);
#endif
    case FILE_READ_METHOD_C:
        return FileReadContentsC(filename);
    case FILE_READ_METHOD_CPP:
        return FileReadContentsCPP(filename);
    case FILE_READ_METHOD_ITERATOR:
        return FileReadContentsIterator(filename);
    case FILE_READ_METHOD_RDBUF:
        return FileReadContentsRdbuf(filename);
    default:
        SPI_UTIL_THROW_RUNTIME_ERROR("Unknown method " << (int)method);
    }
}

void MakeDirectory(const std::string & dirName)
{
    std::string dn = path::convert(dirName);
    if (path::isdir(dn))
        return;

    if (path::isfile(dn))
        SPI_UTIL_THROW_RUNTIME_ERROR("'" << dn << "' is a file");

#ifdef _MSC_VER
    _mkdir(dn.c_str());
#else
    mkdir(dn.c_str(), 0770);
#endif

    if (!path::isdir(dn))
        SPI_UTIL_THROW_RUNTIME_ERROR("Cannot create directory '" << dn << "'");
}

void MakeDirectories(const std::string & dn)
{
    std::string dirName = dn;
    std::vector<std::string> makeLater;
    while (!path::isdir(dirName))
    {
        makeLater.push_back(dirName);
        dirName = path::dirname(dirName);
    }
    size_t N = makeLater.size();
    for (size_t i = N; i > 0; --i)
    {
        std::string dirName = makeLater[i - 1];
        MakeDirectory(dirName);
    }
}

DirectoryWalker::DirectoryWalker(
    const std::string& dirName,
    const std::set<std::string>& excludeDirNames,
    bool depthFirst)
    :
    m_current(),
    m_dirName(dirName),
    m_excludeDirNames(excludeDirNames),
    m_unvisited(),
    m_depthFirst(depthFirst),
    m_started(false)
{}

Directory* DirectoryWalker::Next()
{
    if (!m_started)
    {
        m_current = Directory(m_dirName);
        m_started = true;
    }
    else
    {
        if (m_depthFirst)
        {
            // we want to search in alphabetical order but depth first
            // hence we need to push the directories from m_current
            // in reverse order to the front of the unvisited deque
            std::set<std::string>::const_reverse_iterator iter;
            for (iter = m_current.dns.rbegin();
                 iter != m_current.dns.rend();
                 ++iter)
            {
                std::string fdn = path::join(
                    m_current.dn.c_str(), iter->c_str(), 0);
                m_unvisited.push_front(fdn);
            }
        }
        else
        {
            // we are pushing to the back of the unvisited deque
            std::set<std::string>::const_iterator iter;
            for (iter = m_current.dns.begin();
                 iter != m_current.dns.end();
                 ++iter)
            {
                std::string fdn = path::join(
                    m_current.dn.c_str(), iter->c_str(), 0);
                m_unvisited.push_back(fdn);
            }
        }

        if (m_unvisited.size() == 0)
            return 0;

        std::string dn = m_unvisited.front();
        m_unvisited.pop_front();
        m_current = Directory(dn);
    }
    return &m_current;
}

Directory::Directory()
    :
    dn(),
    fns(),
    dns()
{}

Directory::Directory(const std::string& dn)
    :
    dn(dn),
    fns(),
    dns()
{
#ifdef _MSC_VER

    // this code was taken from MSDN help and modified slightly
    // for this context
    WIN32_FIND_DATA ffd;
    TCHAR szDir[MAX_PATH];
    size_t length_of_arg;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError=0;

    // Check that the input path plus 3 is not longer than MAX_PATH.
    // Three characters are for the "\*" plus NULL appended below.

    StringCchLength(dn.c_str(), MAX_PATH, &length_of_arg);

    if (length_of_arg > (MAX_PATH - 3))
        throw RuntimeError("Directory path is too long.");

    // Prepare string for use with FindFile functions.  First, copy the
    // string to a buffer, then append '\*' to the directory name.

    StringCchCopy(szDir, MAX_PATH, dn.c_str());
    StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

    // Find the first file in the directory.

    hFind = FindFirstFile(szDir, &ffd);
    if (hFind == INVALID_HANDLE_VALUE)
        throw RuntimeError("%s is not a directory", dn.c_str());

    do
    {
        std::string fn(ffd.cFileName);
        if (fn == "." || fn == "..")
            continue;

        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            dns.insert(fn);
        }
        else
        {
            fns.insert(fn);
        }
    }
    while (FindNextFile(hFind, &ffd) != 0);

    dwError = GetLastError();
    FindClose(hFind);
    if (dwError != ERROR_NO_MORE_FILES)
        throw RuntimeError("FindFirstFile failed for %s", dn.c_str());

#else
    DIR* dir = opendir(dn.c_str());
    if (!dir)
        throw RuntimeError("%s is not a directory", dn.c_str());

    spi_boost::shared_ptr<DIR> dir_sp(dir, closedir);

    struct dirent* entry = readdir(dir);
    while (entry)
    {
        std::string fn = &entry->d_name[0];

        if (fn == "." || fn == "..")
        {
            entry = readdir(dir);
            continue;
        }

        std::string ffn = path::join(dn.c_str(), fn.c_str(), 0);
        if (path::isdir(ffn))
        {
            dns.insert(fn);
        }
        else
        {
            fns.insert(fn);
        }
        entry = readdir(dir);
    }
#endif
}

SPI_UTIL_END_NAMESPACE
