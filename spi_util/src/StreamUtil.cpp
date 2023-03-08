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
#include "StreamUtil.hpp"

#include <string.h>
#include <algorithm>

SPI_UTIL_NAMESPACE

std::vector<std::string> StreamReadLines(std::istream& str)
{
    std::vector<std::string> lines;

    while(str)
    {
        std::string line;

        int count;
        int length;
        do
        {
            // this algorithm seems to work for small buffer sizes
            // however 256 seems like a reasonable size given that
            // in general we are going to be reading text files
            char buf[256];
            str.getline(&buf[0], sizeof(buf));

            line += buf;

            count = (int) str.gcount();
            length = (int) strlen(buf);

            // indicates that we found a newline and discarded it
            if (length < count)
                break;

            // indicates we are at the end of the file
            if (count == 0)
                break;

            // indicates that we didn't find a newline and need to keep
            // going - however getline will have set the failbit
            str.clear();
        }
        while (true);

#ifndef _MSC_VER
        line.erase(remove(line.begin(), line.end(), '\r'), line.end());
#endif
        lines.push_back(line);
    }

    return lines;
}

SPI_UTIL_END_NAMESPACE
