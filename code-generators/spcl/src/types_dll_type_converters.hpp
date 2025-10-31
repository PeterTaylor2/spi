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

#ifndef _TYPES_DLL_TYPE_CONVERTERS_HPP_
#define _TYPES_DLL_TYPE_CONVERTERS_HPP_

/**
****************************************************************************
* Header file: types_dll_type_converters.hpp
****************************************************************************
*/

#include <spi/spi.hpp>
#include "types_namespace.hpp"
#include "types_dll_decl_spec.h"
#include <spi/spdoc_publicType.hpp>

TYPES_BEGIN_NAMESPACE

class PublicType;

SPI_IMPORT
spdoc::PublicType::Enum PublicType_convert_in(const PublicType&);

SPI_IMPORT
PublicType PublicType_convert_out(spdoc::PublicType::Enum);

TYPES_END_NAMESPACE

#endif /* _TYPES_DLL_TYPE_CONVERTERS_HPP_*/

