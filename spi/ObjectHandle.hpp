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
#ifndef SPI_OBJECT_HANDLE_HPP
#define SPI_OBJECT_HANDLE_HPP

/*
***************************************************************************
** ObjectHandle.hpp
**
** Defines functions for mapping Objects to string and back again.
**
** Although this is only going to be used by Excel, we keep the code at
** a higher level so that other platforms can read/write object handles
** if they are interacting with Excel at some other level.
***************************************************************************
*/

#include "DeclSpec.h"
#include "Namespace.hpp"
#include "Object.hpp"
#include "Map.hpp"

SPI_BEGIN_NAMESPACE

/**
 * Stores an object and returns the corresponding handle to get it back again.
 */
SPI_IMPORT
std::string ObjectHandleSave(
    const std::string&   baseName,
    const ObjectConstSP& object,
    bool                 noCount=false);

/**
 * Finds an object given its handle.
 */
SPI_IMPORT
ObjectConstSP ObjectHandleFind(
    const std::string& handle);

SPI_IMPORT
int ObjectHandleFreeAll();

SPI_IMPORT
int ObjectHandleCount(const std::string& className);

SPI_IMPORT
void ObjectHandleFree(const std::string& handle);

SPI_IMPORT
std::vector<std::string> ObjectHandleList(
    const std::string& baseName,
    const std::string& className);

SPI_IMPORT
std::string ObjectHandleClassName(
    const std::string& handle);

bool IsNullHandle(const std::string& handle);

SPI_END_NAMESPACE

#endif
