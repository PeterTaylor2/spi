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
#ifndef SPI_SESSION_HPP
#define SPI_SESSION_HPP

/**
***************************************************************************
** Session.hpp
***************************************************************************
*/

#include "DeclSpec.h"
#include "Namespace.hpp"
#include <string>
#include <ostream>

SPI_BEGIN_NAMESPACE

namespace session
{
    // note that all these functions are designed to be thread safe
    SPI_IMPORT
    void start_session();

    SPI_IMPORT
    void add_function_name(const std::string& funcName);

    SPI_IMPORT
    void add_file_name(const std::string& fileName);

    SPI_IMPORT
    void end_session();

    SPI_IMPORT
    void to_stream(std::ostream& ostr);

    SPI_IMPORT
    bool session_logging();
}

SPI_END_NAMESPACE

#define SPI_SESSION_LOG(fn) spi::session::add_function_name(fn)

#endif
