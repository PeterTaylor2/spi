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
#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <string>

struct Options
{
    Options();

    bool noGeneratedCodeNotice;
    bool noVerbatimLine;
    bool optionalArgs;
    bool noHeaderSplit;
    bool allHeader;
    bool noTidyup;
    bool sessionLogging;
    std::string license;
    std::string outputDir;
    bool writeBackup;
    bool checkNonNull;
};

#endif
