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
#ifndef CPP_GENERATOR_HPP
#define CPP_GENERATOR_HPP

#include "serviceDefinition.hpp"
#include <string>
#include "options.hpp"

void generateCpp(const ServiceDefinitionSP& svc, const std::string& dn, const Options& options);
void generateTypes(const ServiceDefinitionSP& svc, const std::string& dn, const Options& options);
void generateConfig(const ServiceDefinitionSP& svc, const std::string& dn);

#endif

