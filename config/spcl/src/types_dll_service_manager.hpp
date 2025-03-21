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

#ifndef _TYPES_DLL_SERVICE_MANAGER_HPP_
#define _TYPES_DLL_SERVICE_MANAGER_HPP_

/**
****************************************************************************
* Header file: types_dll_service_manager.hpp
****************************************************************************
*/

/*
****************************************************************************
* Generated code - do not edit
****************************************************************************
*/

#include "types_dll_service.hpp"

TYPES_BEGIN_NAMESPACE

using spi::SafeCopy;

spi::Service* types_service();
void types_check_permission();
bool types_begin_function(bool noLogging=false);
void types_end_function();
std::runtime_error types_catch_exception(
    bool isLogging,
    const char* name,
    std::exception& e);
std::runtime_error types_catch_all(
    bool isLogging,
    const char* name);

TYPES_END_NAMESPACE

#endif /* _TYPES_DLL_SERVICE_MANAGER_HPP_*/

