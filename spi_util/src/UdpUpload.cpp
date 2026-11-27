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

#include "../UdpUpload.hpp"
#include "../CSV.hpp"
#include "../RuntimeError.hpp"
#include "../JSON.hpp"

#undef SPI_UTIL_CLOCK_EVENTS
#include "ClockUtil.hpp"

#undef DEBUG_LOGGING
#ifdef DEBUG_LOGGING
#include <iostream>
#endif

#include <sstream>

#ifdef _MSC_VER

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>

#else

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>

#endif

#include <string.h>

#define BEGIN_ANONYMOUS_NAMESPACE namespace {
#define END_ANONYMOUS_NAMESPACE }

SPI_UTIL_NAMESPACE

// true if s is exactly n1.n2.n3.n4, each n being 0..255 with no leading zeros
bool IsIPv4Address(const std::string& s)
{
    int parts = 0;
    size_t i = 0;
    while (i < s.size())
    {
        const char first = s[i];
        size_t digits = 0;
        int value = 0;
        while (i < s.size() && s[i] >= '0' && s[i] <= '9')
        {
            value = value * 10 + (s[i] - '0');
            if (++digits > 3 || value > 255)
                return false;
            ++i;
        }
        if (digits == 0 || (digits > 1 && first == '0'))
            return false;
        ++parts;

        if (i == s.size())
            break;
        if (s[i] != '.')
            return false;
        if (++i == s.size())
            return false;               // trailing dot
    }
    return parts == 4;
}

/*
*****************************************************************************
* Implementation of UDPSender and its implementation class UDPSender::Impl
*****************************************************************************
*/
struct UDPSender::Impl
{
#ifdef _MSC_VER
    bool wsaStarted;
    SOCKET sock;
#else
    int sock;
#endif
    sockaddr_in addr;
    std::ostringstream os;      // reused for every JSON value

    Impl()
        :
#ifdef _MSC_VER
        wsaStarted(false),
        sock(INVALID_SOCKET),
#else
        sock(-1),
#endif
        addr(),
        os()
    {
        memset(&addr, 0, sizeof(addr));
    }

    // undoes whatever the constructor managed to do, exactly once, on every exit path
    // (destructors must not throw, so return codes are ignored)
    ~Impl()
    {
#ifdef _MSC_VER
        if (sock != INVALID_SOCKET)
            closesocket(sock);
        if (wsaStarted)
            WSACleanup();
#else
        if (sock >= 0)
            close(sock);
#endif
    }
};

UDPSender::UDPSender(const std::string& ipAddress, int port)
    :
    m_impl(new Impl())
{
    if (!IsIPv4Address(ipAddress))
        SPI_UTIL_THROW_RUNTIME_ERROR("UDPSender: '" << ipAddress << "' is not a dotted IPv4 address");
    if (port <= 0 || port > 65535)
        SPI_UTIL_THROW_RUNTIME_ERROR("UDPSender: invalid port " << port);

    Impl& d = *m_impl;

    d.addr.sin_family = AF_INET;
    d.addr.sin_port = htons((unsigned short)port);
    d.addr.sin_addr.s_addr = inet_addr(ipAddress.c_str());

#ifdef _MSC_VER
    // Initialize Winsock version 2.2
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        SPI_UTIL_THROW_RUNTIME_ERROR("WSAStartup failed");
    d.wsaStarted = true;

    d.sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (d.sock == INVALID_SOCKET)
        SPI_UTIL_THROW_RUNTIME_ERROR("Could not create socket with error " << WSAGetLastError());
#else
    d.sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (d.sock < 0)
        SPI_UTIL_THROW_RUNTIME_ERROR("socket creation failed");
#endif
}

UDPSender::~UDPSender()
{
    // Impl's destructor does the work
}

bool UDPSender::SendLine(const std::string& line)
{
    Impl& d = *m_impl;

#ifdef _MSC_VER
    int sent = sendto(d.sock, line.c_str(), (int)line.size(), 0,
        (const SOCKADDR*)&d.addr, sizeof(d.addr));
    return sent == (int)line.size();
#else
    ssize_t sent = sendto(d.sock, line.c_str(), line.size(), 0,
        (const struct sockaddr*)&d.addr, sizeof(d.addr));
    return sent == (ssize_t)line.size();
#endif
}

bool UDPSender::SendJSON(const JSONValue& value)
{
    Impl& d = *m_impl;

    d.os.str(std::string());
    d.os.clear();
    JSONValueToStream(d.os, value, true, 0);

    return SendLine(d.os.str());
}

bool UDPSender::SendCSVRow(const std::vector<std::string>& row)
{
    Impl& d = *m_impl;

    d.os.str(std::string());
    d.os.clear();
    csv::WriteLine(d.os, row);

    return SendLine(d.os.str());
}

void UDPUploadCSV(
    const std::string& ipAddress,
    int serverPort,
    const csv::Data* data)
{
    if (ipAddress.empty() || serverPort == 0 || !data || data->numRows() == 0)
        return;

    UDPSender sender(ipAddress, serverPort);

    // send one line at a time
    // rows() returns a reference, whereas row(i) would copy every row
    const std::vector<std::vector<std::string>>& rows = data->rows();

    for (size_t i = 0; i < rows.size(); ++i)
        sender.SendCSVRow(rows[i]);
}

void UDPUploadJSON(
    const std::string& ipAddress,
    int serverPort,
    const std::vector<JSONValue>& jsonValues)
{
    if (ipAddress.empty() || serverPort == 0 || jsonValues.size() == 0)
        return;

    UDPSender sender(ipAddress, serverPort);

    // send one line at a time - assume that each line is fairly short as it is a single JSON value
    for (size_t i = 0; i < jsonValues.size(); ++i)
        sender.SendJSON(jsonValues[i]);
}

std::string UDPGetHostByName(const std::string& name)
{
    if (name.empty())
        return std::string();

    if (IsIPv4Address(name))
        return name;

#ifdef _MSC_VER
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return std::string();

    struct hostent* host = gethostbyname(name.c_str());
    std::string result;
    if (host && host->h_addr_list && host->h_addr_list[0])
    {
        in_addr addr;
        memcpy(&addr, host->h_addr_list[0], sizeof(in_addr));
        // inet_ntoa returns a pointer to a statically allocated buffer; copy it.
        result = inet_ntoa(addr);
    }

    WSACleanup();
    return result;
#else
    struct addrinfo hints;
    struct addrinfo* res = nullptr;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // prefer IPv4 for UDP uploads in this codebase

    if (getaddrinfo(name.c_str(), nullptr, &hints, &res) != 0)
        return std::string();

    std::string result;
    if (res && res->ai_addr)
    {
        char ipstr[INET_ADDRSTRLEN];
        struct sockaddr_in* sa = (struct sockaddr_in*)res->ai_addr;
        if (inet_ntop(AF_INET, &sa->sin_addr, ipstr, sizeof(ipstr)))
            result = ipstr;
    }

    freeaddrinfo(res);
    return result;
#endif
}



SPI_UTIL_END_NAMESPACE


