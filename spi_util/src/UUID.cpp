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
#include "UUID.hpp"

#include "RuntimeError.hpp"

#ifdef NO_UUID

#include "uuid4.h"

SPI_UTIL_NAMESPACE

std::string uuid_string()
{
    UUID4_STATE_T state;
    UUID4_T uuid;

    if (uuid4_seed(&state) != 0)
        return "UUID-not-available";

    uuid4_gen(&state, &uuid);

    char buffer[UUID4_STR_BUFFER_SIZE];
    if (!uuid4_to_s(uuid, buffer, sizeof(buffer)))
        return "UUID-not-available";

    return std::string(buffer);
}

SPI_UTIL_END_NAMESPACE

#else

#ifdef _MSC_VER
#include <windows.h>
#else
#include <uuid/uuid.h>
#endif

SPI_UTIL_NAMESPACE

std::string uuid_string()
{
    std::string res;
#ifdef _MSC_VER
    UUID uuid;
    if (UuidCreate(&uuid) != RPC_S_OK)
        return "UUID-not-available";
    // RPC_CSTR is simply unsigned char*
    RPC_CSTR uuid_cstr = NULL;
    if (UuidToStringA(&uuid, &uuid_cstr) != RPC_S_OK)
        return "UUID-not-available";
    res = std::string((char*)uuid_cstr);
    RpcStringFreeA(&uuid_cstr);
#else
    uuid_t uuid;
    char uuid_cstr[37]; // 36 byte uuid plus null.
    uuid_generate(uuid);
    uuid_unparse(uuid, uuid_cstr);
    res = std::string(uuid_cstr);
#endif

    return res;

}

SPI_UTIL_END_NAMESPACE

#endif
