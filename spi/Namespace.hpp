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
** Namespace.hpp
***************************************************************************
** Defines the namespace macros. This will give us flexibility to change
** the namespace for different releases in the future.
**
** See also spi/makefiles/version.mk
**
** The version numbers defined in this file must match those defined in
** version.mk
***************************************************************************
*/

#ifndef SPI_NAMESPACE_HPP
#define SPI_NAMESPACE_HPP

#undef SPI_BEGIN_NAMESPACE
#undef SPI_END_NAMESPACE

#undef SPI_VERSION
#define SPI_VERSION 1.0.0.0

#if defined SPI_STATIC || !defined SPI_VERSION

// for the static library build we don't bother with the versioned namespace
// and alias - the static library is not part of the general release

#define SPI_NAMESPACE spi
#define SPI_BEGIN_NAMESPACE namespace SPI_NAMESPACE {
#define SPI_END_NAMESPACE }

#else

#define SPI_NAMESPACE spi_100
#define SPI_BEGIN_NAMESPACE namespace SPI_NAMESPACE {
#define SPI_END_NAMESPACE }

SPI_BEGIN_NAMESPACE
SPI_END_NAMESPACE

// defines an alias so that we can use spi:: in calling code
// note that we have to use the namespace before we can define an alias to it
namespace spi = SPI_NAMESPACE;

#endif /* SPI_STATIC */

#endif /* SPI_NAMESPACE_HPP */

