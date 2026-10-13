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
/*
***************************************************************************
** ObjectMake.hpp
***************************************************************************
** Defines functionality for ObjectMake.
***************************************************************************
*/

#ifndef SPI_OBJECT_MAKE_HPP
#define SPI_OBJECT_MAKE_HPP

#include "Object.hpp"
#include "ObjectHelper.hpp"
#include "Value.hpp"

#include <string>
#include <vector>

SPI_BEGIN_NAMESPACE

SPI_IMPORT
ObjectConstSP ObjectMake(
    const std::string& className,
    const std::vector<std::string>& names,
    const std::vector<Value>& values,
    const InputContext* context = NULL);

SPI_END_NAMESPACE

#endif
