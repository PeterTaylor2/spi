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
#ifndef TEX_OPTIONS_HPP
#define TEX_OPTIONS_HPP

#include <string>
#include <vector>

struct Options
{
    Options() :
        verbose(false),
        writeIncludes(false)
    {}

    bool verbose;
    bool writeIncludes;
};

void setOptions(const Options& getOptions);
const Options& getOptions();

#endif
