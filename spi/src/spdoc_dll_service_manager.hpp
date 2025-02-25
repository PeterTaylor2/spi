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

#ifndef _SPDOC_DLL_SERVICE_MANAGER_HPP_
#define _SPDOC_DLL_SERVICE_MANAGER_HPP_

/**
****************************************************************************
* Header file: spdoc_dll_service_manager.hpp
****************************************************************************
*/

#include "spdoc_dll_service.hpp"

SPDOC_BEGIN_NAMESPACE

using spi::SafeCopy;

spi::Service* spdoc_service();
void spdoc_check_permission();
bool spdoc_begin_function(bool noLogging=false);
void spdoc_end_function();
std::runtime_error spdoc_catch_exception(
    bool isLogging,
    const char* name,
    std::exception& e);
std::runtime_error spdoc_catch_all(
    bool isLogging,
    const char* name);

SPDOC_END_NAMESPACE

#endif /* _SPDOC_DLL_SERVICE_MANAGER_HPP_*/

