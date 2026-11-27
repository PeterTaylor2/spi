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
#define SPI_UTIL_UDP_UPLOAD_HPP

#include "DeclSpec.h"
#include "Namespace.hpp"
#include "RefCounter.hpp"
#include <memory>
#include <string>
#include <vector>

SPI_UTIL_NAMESPACE

namespace csv
{
    SPI_UTIL_DECLARE_RC_CLASS(Data);
}

SPI_UTIL_DECLARE_RC_CLASS(JSONValue);

/// Sends datagrams to one IPv4 address and port.
/// The socket (and Winsock on Windows) is opened once by the constructor
/// and released by the destructor, so one sender can send many datagrams.
/// The constructor throws if the address is not a dotted IPv4 address
/// (a hostname is never silently turned into a broadcast address).
class SPI_UTIL_IMPORT UDPSender
{
public:
    UDPSender(const std::string& ipAddress, int port);
    ~UDPSender();

    UDPSender(const UDPSender&) = delete;
    UDPSender& operator=(const UDPSender&) = delete;

    /// sends one datagram containing the line - returns false if the send failed
    /// (UDP gives no delivery guarantee, so true only means the OS accepted it)
    bool SendLine(const std::string& line);

    /// serialises one JSON value (as in UDPUploadJSON) and sends it as one datagram
    bool SendJSON(const JSONValue& value);

    /// writes one row in CSV format (as csv::WriteLine does) and sends it as one datagram
    bool SendCSVRow(const std::vector<std::string>& row);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

SPI_UTIL_IMPORT
bool IsIPv4Address(const std::string& s);

SPI_UTIL_IMPORT
void UDPUploadCSV(
    const std::string& ipAddress,
    int serverPort,
    const csv::Data* data);

SPI_UTIL_IMPORT
void UDPUploadJSON(
    const std::string& ipAddress,
    int serverPort,
    const std::vector<JSONValue>& jsonValues);

SPI_UTIL_IMPORT
std::string UDPGetHostByName(const std::string& name);

SPI_UTIL_END_NAMESPACE

#endif

