/*

    Sartorial Programming Interface (SPI) runtime libraries

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
#ifndef SPI_UTIL_UDP_UPLOAD_HPP
#define SPI_UTIL_UDL_UPLOAD_HPP

#include "DeclSpec.h"
#include "Namespace.hpp"
#include "RefCounter.hpp"
#include <string>
#include <vector>

SPI_UTIL_NAMESPACE

namespace csv
{
    SPI_UTIL_DECLARE_RC_CLASS(Data);
}

SPI_UTIL_DECLARE_RC_CLASS(JSONValue);

SPI_UTIL_IMPORT
void UDPUploadCSV(
    const std::string& serverName,
    int serverPort,
    const csv::Data* data);

SPI_UTIL_IMPORT
void UDPUploadJSON(
    const std::string& serverName,
    int serverPort,
    const std::vector<JSONValue>& jsonValues);

SPI_UTIL_IMPORT
std::string UDPGetHostByName(const std::string& name);

SPI_UTIL_END_NAMESPACE

#endif

