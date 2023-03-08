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

#ifndef _TYPES_DLL_TIME_OUT_HPP_
#define _TYPES_DLL_TIME_OUT_HPP_

/**
****************************************************************************
* Header file: types_dll_time_out.hpp
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

void types_set_time_out(spi::Date timeout, const char* msg = NULL);
void types_check_time_out();

TYPES_END_NAMESPACE

#endif /* _TYPES_DLL_TIME_OUT_HPP_*/

