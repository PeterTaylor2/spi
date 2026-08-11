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

extern void ShutdownCURL();

BEGIN_ANONYMOUS_NAMESPACE

#ifdef _MSC_VER

void errorHandler(const char* errmsg)
{
    int errorCode = WSAGetLastError();
    WSACleanup();
    SPI_UTIL_THROW_RUNTIME_ERROR(errmsg << " with error " << errorCode);
}

#endif

END_ANONYMOUS_NAMESPACE

void UDPUploadCSV(
    const std::string& serverName,
    int serverPort,
    const csv::Data* data)
{
    if (serverName.empty() || serverPort == 0 || !data || data->numRows() == 0)
        return;

    ShutdownCURL(); // we cannot use LIBCURL for UDP but CURL may block using the socket libraries

#ifdef _MSC_VER
    {
        WSADATA wsaData;
        SOCKET sendingSocket;
        SOCKADDR_IN recvAddress;

        // Initialize Winsock version 2.2

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            errorHandler("WSAStartup failed");
        }

        // Create a new socket to receive datagrams on.

        sendingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        if (sendingSocket == INVALID_SOCKET)
        {
            errorHandler("Could not create socket");
        }

        recvAddress.sin_family = AF_INET;
        recvAddress.sin_port = htons(serverPort);
        recvAddress.sin_addr.s_addr = inet_addr(serverName.c_str());

        // send one line at a time
        size_t NR = data->numRows();

        for (size_t i = 0; i < NR; ++i)
        {
            std::ostringstream os;
            csv::WriteLine(os, data->row(i));
            std::string line = os.str();

            int sent = sendto(sendingSocket, line.c_str(), (int)line.size(), 0, (SOCKADDR*)&recvAddress, sizeof(recvAddress));
        }

        if (closesocket(sendingSocket) != 0)
        {
            errorHandler("closesocket failed");
        }

        if (WSACleanup() != 0)
        {
            errorHandler("WSACleanup failed");
        }
    }
#else
    {
        struct sockaddr_in recvAddress;

        // Create UDP socket
        int sendingSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (sendingSocket < 0)
        {
            SPI_UTIL_THROW_RUNTIME_ERROR("socket creation failed");
        }

        memset(&recvAddress, 0, sizeof(recvAddress));

        // Fill server address info
        recvAddress.sin_family = AF_INET;
        recvAddress.sin_port = htons(serverPort);
        recvAddress.sin_addr.s_addr = inet_addr(serverName.c_str());

        // send one line at a time
        size_t NR = data->numRows();

        for (size_t i = 0; i < NR; ++i)
        {
            std::ostringstream os;
            csv::WriteLine(os, data->row(i));
            std::string line = os.str();

            int sent = sendto(sendingSocket, line.c_str(), line.size(), 0,
                (const struct sockaddr*)&recvAddress, sizeof(recvAddress));
        }

        // Close socket
        close(sendingSocket);
    }

#endif

}

void UDPUploadJSON(
    const std::string& serverName,
    int serverPort,
    const std::vector<JSONValue>& jsonValues)
{
    if (serverName.empty() || serverPort == 0 || jsonValues.size() == 0)
        return;

    ShutdownCURL(); // we cannot use LIBCURL for UDP but CURL may block using the socket libraries

#ifdef _MSC_VER
    {
        WSADATA wsaData;
        SOCKET sendingSocket;
        SOCKADDR_IN recvAddress;

        // Initialize Winsock version 2.2

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            errorHandler("WSAStartup failed");
        }

        // Create a new socket to receive datagrams on.

        sendingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        if (sendingSocket == INVALID_SOCKET)
        {
            errorHandler("Could not create socket");
        }

        recvAddress.sin_family = AF_INET;
        recvAddress.sin_port = htons(serverPort);
        recvAddress.sin_addr.s_addr = inet_addr(serverName.c_str());

        // send one line at a time - assume that each line is fairly short as it is a single JSON value
        size_t NR = jsonValues.size();

        for (size_t i = 0; i < NR; ++i)
        {
            std::ostringstream os;
            JSONValueToStream(os, jsonValues[i], true, 0);
            std::string line = os.str();

#ifdef DEBUG_LOGGING
            std::cout << line << std::endl;
#endif

            int sent = sendto(sendingSocket, line.c_str(), (int)line.size(), 0, (SOCKADDR*)&recvAddress, sizeof(recvAddress));
        }

        if (closesocket(sendingSocket) != 0)
        {
            errorHandler("closesocket failed");
        }

        if (WSACleanup() != 0)
        {
            errorHandler("WSACleanup failed");
        }
    }
#else
    {
        struct sockaddr_in recvAddress;

        // Create UDP socket
        int sendingSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (sendingSocket < 0)
        {
            SPI_UTIL_THROW_RUNTIME_ERROR("socket creation failed");
        }

        memset(&recvAddress, 0, sizeof(recvAddress));

        // Fill server address info
        recvAddress.sin_family = AF_INET;
        recvAddress.sin_port = htons(serverPort);
        recvAddress.sin_addr.s_addr = inet_addr(serverName.c_str());

        // send one line at a time - assume that each line is fairly short as it is a single JSON value
        size_t NR = jsonValues.size();

        for (size_t i = 0; i < NR; ++i)
        {
            std::ostringstream os;
            JSONValueToStream(os, jsonValues[i], true, 0);
            std::string line = os.str();

#ifdef DEBUG_LOGGING
            std::cout << "Sending " << line << std::endl;
#endif

            int sent = sendto(sendingSocket, line.c_str(), line.size(), 0,
                (const struct sockaddr*)&recvAddress, sizeof(recvAddress));
        }

        // Close socket
        close(sendingSocket);
    }

#endif

}

std::string UDPGetHostByName(const std::string& name)
{
    if (name.empty())
        return std::string();

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


