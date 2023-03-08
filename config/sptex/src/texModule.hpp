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
#ifndef TEX_MODULE_HPP
#define TEX_MODULE_HPP

#include <vector>
#include <string>
#include <set>

#include <spi/spdoc_configTypes.hpp>

class ServiceIndex;

void writeTexModuleConstructs(
    const std::string&           dirname,
    const spdoc::ServiceConstSP& service,
    const spdoc::ModuleConstSP&  module,
    ServiceIndex&                serviceIndex,
    std::vector<std::string>&    simpleTypes,
    std::vector<std::string>&    enums,
    std::vector<std::string>&    classes,
    std::vector<std::string>&    functions,
    std::vector<std::string>&    classMethods,
    std::set<std::string>&       typesUsed,
    std::set<std::string>&       fns);

void addModuleToServiceIndex(
    const spdoc::ModuleConstSP& module,
    ServiceIndex& serviceIndex);

#endif
