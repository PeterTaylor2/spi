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

#ifndef _SPDOC_PUBLICTYPE_HELPER_HPP_
#define _SPDOC_PUBLICTYPE_HELPER_HPP_

/**
****************************************************************************
* Header file: spdoc_publicType_helper.hpp
****************************************************************************
*/

#include "spdoc_publicType.hpp"
#include "spdoc_dll_type_converters.hpp"

SPDOC_BEGIN_NAMESPACE

PublicType verifyPrimitiveTypeName_Helper(
    const std::string& typeName);

void publicType_register_object_types(const spi::ServiceSP& svc);

SPDOC_END_NAMESPACE

#endif /* _SPDOC_PUBLICTYPE_HELPER_HPP_*/

