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
** FileUtil.hpp
***************************************************************************
** Some file name manipulation functions.
***************************************************************************
*/

#include "FileUtil.hpp"
#include "StringUtil.hpp"

#include <stdarg.h>

#include <algorithm>
#include <sstream>
#include <vector>

namespace path
{
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

        char* sep = "";
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
}

