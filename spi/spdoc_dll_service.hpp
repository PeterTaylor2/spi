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

#ifndef _SPDOC_DLL_SERVICE_HPP_
#define _SPDOC_DLL_SERVICE_HPP_

/**
****************************************************************************
* Header file: spdoc_dll_service.hpp
****************************************************************************
*/

#include <spi/spi.hpp>
#include "spdoc_dll_decl_spec.h"
#include "spdoc_namespace.hpp"

SPDOC_BEGIN_NAMESPACE

SPI_IMPORT
spi::ServiceSP spdoc_start_service();

SPI_IMPORT
void spdoc_stop_service();

SPI_IMPORT
void spdoc_start_logging(const char* filename, const char* options="");

SPI_IMPORT
void spdoc_stop_logging();

SPI_IMPORT
spi::ServiceSP spdoc_exported_service();

SPI_IMPORT
spi::Date spdoc_get_time_out();

SPI_IMPORT
const char* spdoc_version();

SPDOC_END_NAMESPACE

#endif /* _SPDOC_DLL_SERVICE_HPP_*/

