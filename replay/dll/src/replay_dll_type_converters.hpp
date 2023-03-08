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

#ifndef _REPLAY_DLL_TYPE_CONVERTERS_HPP_
#define _REPLAY_DLL_TYPE_CONVERTERS_HPP_

/**
****************************************************************************
* Header file: replay_dll_type_converters.hpp
****************************************************************************
*/

#include <spi/spi.hpp>
#include "replay_namespace.hpp"
#include "replay_dll_decl_spec.h"

SPI_REPLAY_BEGIN_NAMESPACE

class ValueType;

REPLAY_IMPORT
SPI_NAMESPACE::Value::Type ValueType_convert_in(const ValueType&);

REPLAY_IMPORT
ValueType ValueType_convert_out(SPI_NAMESPACE::Value::Type);

SPI_REPLAY_END_NAMESPACE

#endif /* _REPLAY_DLL_TYPE_CONVERTERS_HPP_*/

