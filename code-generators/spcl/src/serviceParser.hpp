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
#ifndef SPI_CONFIG_SERVICE_PARSER_HPP
#define SPI_CONFIG_SERVICE_PARSER_HPP

#include <string>
#include "serviceDefinition.hpp"

ServiceDefinitionSP serviceParser(
    const std::string& fn,
    const std::string& svoFileName,
    const std::string& commandLineVersion,
    bool verbose);

void writeServiceParserDoc(
    GeneratedOutput& ostr,
    bool verbose);

#endif
