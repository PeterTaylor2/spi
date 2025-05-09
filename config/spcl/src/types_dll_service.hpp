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

#ifndef _TYPES_DLL_SERVICE_HPP_
#define _TYPES_DLL_SERVICE_HPP_

/**
****************************************************************************
* Header file: types_dll_service.hpp
****************************************************************************
*/

/*
****************************************************************************
* Generated code - do not edit
****************************************************************************
*/

#include <spi/spi.hpp>
#include "types_dll_decl_spec.h"
#include "types_namespace.hpp"

TYPES_BEGIN_NAMESPACE

SPI_IMPORT
spi::ServiceSP types_start_service();

SPI_IMPORT
void types_stop_service();

SPI_IMPORT
void types_start_logging(const char* filename, const char* options="");

SPI_IMPORT
void types_stop_logging();

SPI_IMPORT
spi::ServiceSP types_exported_service();

SPI_IMPORT
spi::Date types_get_time_out();

SPI_IMPORT
const char* types_version();

SPI_IMPORT
const char* types_startup_directory();

TYPES_END_NAMESPACE

#endif /* _TYPES_DLL_SERVICE_HPP_*/

