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
#ifndef SPI_PROFILE_HPP
#define SPI_PROFILE_HPP

/**
***************************************************************************
** Profile.hpp
***************************************************************************
*/

#include "DeclSpec.h"
#include "Namespace.hpp"
#include "Session.hpp"

#include <spi_util/ClockUtil.hpp>

SPI_BEGIN_NAMESPACE

typedef spi_util::Profile Profile;

SPI_IMPORT
void StartProfiler();

SPI_IMPORT
void StopProfiler();

SPI_IMPORT
void GetProfile(Profile& profile);

SPI_IMPORT
spi_util::ClockEvents* Profiler();

SPI_END_NAMESPACE

#define SPI_PROFILE(fn) spi_util::ClockFunction clock_function_(fn, spi::Profiler())

#endif
