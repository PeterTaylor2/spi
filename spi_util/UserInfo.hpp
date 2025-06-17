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
#ifndef SPI_USER_INFO_HPP
#define SPI_USER_INFO_HPP

/*
***************************************************************************
* UserInfo.hpp
***************************************************************************
* Information about the currently logged on user.
***************************************************************************
*/

#include "DeclSpec.h"
#include "Namespace.hpp"

#include <string>
#include <vector>

SPI_UTIL_NAMESPACE

// returns the name of the currently logged on user
SPI_UTIL_IMPORT
std::string UserName();

// returns the name of the current computer
SPI_UTIL_IMPORT
std::string ComputerName(bool fullyQualified = false);

// returns the names of the groups to which the a given user belongs
// by default the current user is used if no userName provided
SPI_UTIL_IMPORT
std::vector<std::string> UserGroups
(const char* serverName=NULL,
 const char* userName=NULL);

SPI_UTIL_IMPORT
unsigned int ProcessID();

SPI_UTIL_END_NAMESPACE

#endif
