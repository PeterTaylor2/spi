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
/**
***************************************************************************
** Declare.hpp
***************************************************************************
** Defines shared pointer types for non reference counted classes.
***************************************************************************
*/

#ifndef SPI_DECLARE_HPP
#define SPI_DECLARE_HPP

#include "platform.h"
#include "DeclSpec.h"
#include "Namespace.hpp"

#include <stdexcept>

#include <spi_boost/shared_ptr.hpp>

#undef SPI_DECLARE_CLASS
#define SPI_DECLARE_CLASS(T) \
class T;\
typedef spi_boost::shared_ptr<T>       T##SP;\
typedef spi_boost::shared_ptr<T const> T##ConstSP

#undef SPI_DECLARE_STRUCT
#define SPI_DECLARE_STRUCT(T) \
struct T;\
typedef spi_boost::shared_ptr<T>       T##SP;\
typedef spi_boost::shared_ptr<T const> T##ConstSP

#endif


