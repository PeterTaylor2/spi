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

#ifndef _REPLAY_DLL_SERVICE_HPP_
#define _REPLAY_DLL_SERVICE_HPP_

/**
****************************************************************************
* Header file: replay_dll_service.hpp
****************************************************************************
*/

#include <spi/spi.hpp>
#include <spi/spdoc_configTypes.hpp>
#include "replay_dll_decl_spec.h"
#include "replay_namespace.hpp"

SPI_REPLAY_BEGIN_NAMESPACE

REPLAY_IMPORT
spi::ServiceSP replay_start_service();

REPLAY_IMPORT
void replay_stop_service();

REPLAY_IMPORT
void replay_start_logging(const char* filename, const char* options="");

REPLAY_IMPORT
void replay_stop_logging();

REPLAY_IMPORT
spi::ServiceSP replay_exported_service();

REPLAY_IMPORT
spi::Date replay_get_time_out();

REPLAY_IMPORT
const char* replay_version();

REPLAY_IMPORT
const char* replay_startup_directory();

REPLAY_IMPORT
spdoc::ServiceConstSP replay_service_doc();

SPI_REPLAY_END_NAMESPACE

#endif /* _REPLAY_DLL_SERVICE_HPP_*/

