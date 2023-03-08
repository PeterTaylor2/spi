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

#ifndef _REPLAY_DLL_TIME_OUT_HPP_
#define _REPLAY_DLL_TIME_OUT_HPP_

/**
****************************************************************************
* Header file: replay_dll_time_out.hpp
****************************************************************************
*/

#include <spi/spi.hpp>
#include "replay_dll_decl_spec.h"
#include "replay_namespace.hpp"

SPI_REPLAY_BEGIN_NAMESPACE

void replay_set_time_out(spi::Date timeout, const char* msg = NULL);
void replay_check_time_out();

SPI_REPLAY_END_NAMESPACE

#endif /* _REPLAY_DLL_TIME_OUT_HPP_*/

