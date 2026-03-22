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

#include "CurlUtil.hpp"

#include "MutexLock.hpp"
#include "RuntimeError.hpp"

#undef SPI_UTIL_CLOCK_EVENTS
#include "ClockUtil.hpp"

#include <sstream>

#ifdef _MSC_VER
#include <spi_curl/curl.h>
#else
#include <curl/curl.h>
#endif

#include <spi_boost/shared_ptr.hpp>

#define BEGIN_ANONYMOUS_NAMESPACE namespace {
#define END_ANONYMOUS_NAMESPACE }

SPI_UTIL_NAMESPACE

BEGIN_ANONYMOUS_NAMESPACE

// designed to manage global initialisation and tidy-up of the CURL library
struct GlobalInit
{
    GlobalInit()
    {
        errorHandler(curl_global_init(CURL_GLOBAL_ALL));
    }

    ~GlobalInit()
    {
        curl_global_cleanup();
    }

private:
    GlobalInit(const GlobalInit&);
    GlobalInit& operator=(const GlobalInit&);
};

spi_boost::shared_ptr<GlobalInit> GlobalInitialisation;

END_ANONYMOUS_NAMESPACE

void InitializeCURL()
{
    SPI_UTIL_LOCAL_LOCK;

    if (!GlobalInitialisation)
        GlobalInitialisation.reset(new GlobalInit());
}

void errorHandler(CURLcode status)
{
    if (status != CURLE_OK)
    {
        throw RuntimeError("CURL error: %d: %s",
            (int)status,
            curl_easy_strerror(status));
    }
}

SPI_UTIL_END_NAMESPACE

