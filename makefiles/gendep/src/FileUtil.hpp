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

#ifndef SPI_FILE_UTIL_HPP
#define SPI_FILE_UTIL_HPP

/*
***************************************************************************
** FileUtil.hpp
***************************************************************************
** Some file name manipulation functions.
***************************************************************************
*/

#include <string>

namespace path
{
    std::string dirname(const std::string& path);
    std::string basename(const std::string& path);
    std::string join(const char* path, ...);
    std::string posix(const std::string& path);
    std::string normpath(const std::string& path);
} // end of namespace path

#endif
