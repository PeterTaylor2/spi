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
#include "UserInfo.hpp"
#include "RuntimeError.hpp"
#include "Utils.hpp"

#include <spi_boost/shared_ptr.hpp>

#ifdef _MSC_VER

#include <windows.h>
#include <tchar.h>
#include <lm.h>

#else

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>

#endif

SPI_UTIL_NAMESPACE

#define BEGIN_ANONYMOUS_NAMESPACE namespace {
#define END_ANONYMOUS_NAMESPACE }

BEGIN_ANONYMOUS_NAMESPACE

std::string DefaultUserName(const char* userName = NULL)
{
    if (!userName || !*userName)
        return UserName();

    return std::string(userName);
}

END_ANONYMOUS_NAMESPACE

#ifdef _MSC_VER

BEGIN_ANONYMOUS_NAMESPACE

// this function assumes that we can simply throw away the extra stuff
std::string wcharCstr2String(const wchar_t* in)
{
    size_t inLen = wcslen(in);
    std::ostringstream oss;
    for (size_t i = 0; i < inLen; ++i)
        oss << (char)in[i];
    return oss.str();
}

END_ANONYMOUS_NAMESPACE

// returns the name of the currently logged on user
std::string UserName()
{
    TCHAR userName[256];
    DWORD size = 256;
    if (!GetUserName(userName, &size))
        SPI_UTIL_THROW_RUNTIME_ERROR("GetUserName failed");

    return std::string(userName);
}

// returns the name of the current computer
std::string ComputerName(bool fullyQualified)
{
    TCHAR computerName[256];
    DWORD size = 256;
    if (fullyQualified)
    {
        BOOL rc = GetComputerNameExA(ComputerNameDnsFullyQualified, computerName, &size);
        if (!rc)
        {
            throw RuntimeError("GetComputerNameExA failed: %d", GetLastError());
        }
    }
    else
    {
        BOOL rc = GetComputerNameA(computerName, &size);
        if (!rc)
        {
            throw RuntimeError("GetComputerNameA failed: %ld", GetLastError());
        }
    }

    return std::string(computerName);
}

// returns the names of the groups to which the a given user belongs
// by default the current user is used if no userName provided
std::vector<std::string> UserGroups(
    const char* in_serverName,
    const char* in_userName)
{
    static std::vector<std::string> lastResult;
    static std::string lastServerName;
    static std::string lastUserName;

    std::string serverName(in_serverName ? in_serverName : "");
    std::string userName = DefaultUserName(in_userName);

    // TBD: thread safety for the cache of last result
    if (serverName == lastServerName && userName == lastUserName)
        return lastResult;

    std::wstring wServerName(serverName.begin(), serverName.end());
    std::wstring wUserName(userName.begin(), userName.end());

    LPGROUP_USERS_INFO_0 buf = NULL;
    DWORD entriesRead = 0;
    DWORD totalEntries = 0;

    if (NetUserGetGroups(
        wServerName.empty() ? NULL : wServerName.c_str(),
        wUserName.c_str(),
        0, // level
        (LPBYTE*)&buf,
        MAX_PREFERRED_LENGTH, // preferred max length
        &entriesRead,
        &totalEntries) != NERR_Success)
    {
        SPI_UTIL_THROW_RUNTIME_ERROR("NetUserGetGroups failed");
    }

    std::vector<std::string> result;
    const char* error = NULL;
    if (buf)
    {
        LPGROUP_USERS_INFO_0 tmp = buf;
        for (DWORD i = 0; i < entriesRead; ++i)
        {
            if (!tmp)
            {
                error = "Access violation";
                break;
            }

            std::string groupName = wcharCstr2String(tmp->grui0_name);
            result.push_back(groupName);
            tmp++;
        }

        NetApiBufferFree(buf);
    }

    if (error)
        SPI_UTIL_THROW_RUNTIME_ERROR(error);

    // TBD: thread safety for the cache of last result
    // save results to prevent repeated NET access for the same user name
    lastServerName = serverName;
    lastUserName = userName;
    lastResult = result;

    return result;
}

unsigned int ProcessID()
{
    DWORD pid = GetCurrentProcessId();
    return IntegerCast<unsigned int>(pid);
}

#else

// returns the name of the currently logged on user
std::string UserName()
{
    char* username;

    username = getlogin();

    if (!username)
    {
        username = getenv("LOGNAME");
    }

    if (username)
        return std::string(username);

    SPI_UTIL_THROW_RUNTIME_ERROR("Unable to get user name via getlogin or LOGNAME via getenv");
}

// returns the name of the current computer
std::string ComputerName(bool fullyQualified)
{
    char hostName[256];
    int rc = gethostname(&hostName[0], 256);
    if (rc != 0)
    {
        throw RuntimeError("gethostname failed with code %d", rc);
    }

    if (fullyQualified)
    {
        struct hostent h2;
        struct hostent* h;
        char w[1024]; // work buffer
        int err;

        rc = gethostbyname2_r(hostName, AF_INET, &h2, w, sizeof(w), &h, &err);

        if (rc != 0)
        {
            rc = gethostbyname2_r(hostName, AF_INET6, &h2, w, sizeof(w), &h, &err);
        }

        if (rc != 0)
        {
            throw RuntimeError("gethostbyname2_r failed with code %d", rc);
        }

        SPI_UTIL_POST_CONDITION(h == &h2);
        return std::string(h2.h_name);
    }
    else
    {
        return std::string(&hostName[0]);
    }
}

// returns the names of the groups to which the current user belongs
// it is possible that serverName is only a concept for windows
std::vector<std::string> UserGroups(
    const char* in_serverName,
    const char* in_userName)
{
    std::string serverName(in_serverName ? in_serverName : "");
    std::string userName = DefaultUserName(in_userName);

    SPI_UTIL_NOT_IMPLEMENTED;
}

unsigned int ProcessID()
{
    pid_t pid = getpid();
    return IntegerCast<unsigned int>(pid);
}

#endif

SPI_UTIL_END_NAMESPACE
