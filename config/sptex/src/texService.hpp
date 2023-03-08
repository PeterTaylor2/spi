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
#ifndef TEX_SERVICE_HPP
#define TEX_SERVICE_HPP

#include <vector>
#include <string>

class GeneratedOutput;

#include <spi/spdoc_configTypes.hpp>

void writeTexService(
    const std::string&              dirname,
    const spdoc::ServiceConstSP&    service,
    const std::vector<std::string>& dnImports,
    const std::vector<spdoc::ServiceConstSP>& extraServices);

#endif
