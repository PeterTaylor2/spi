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
#ifndef SPI_OBJECT_URL_HPP
#define SPI_OBJECT_URL_HPP

/*
 * Isolates all URL based functions to improve static linking.
 */

#include "Object.hpp"

#include <fstream>
#include <vector>
#include <list>

SPI_BEGIN_NAMESPACE

SPI_DECLARE_RC_CLASS(Service);
class InputContext;

std::string read_url(const std::string& url,
    int timeout,
    int cacheAge);

void url_cache_clear_entry(const std::string& url);
void url_cache_clear();
void url_cache_init();
void url_cache_load(const char* filename);
void url_cache_save(const char* filename);
size_t url_cache_size();

ObjectConstSP object_from_url(
    const ServiceConstSP& service,
    const std::string& url,
    int timeout = 0,
    int cacheAge = 0);

SPI_END_NAMESPACE

#endif
