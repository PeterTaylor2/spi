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

/*
***************************************************************************
** UrlRead.hpp
***************************************************************************
** Functions for reading the entire contents for a URL.
**
** Wrapper to curl.lib (or libcurl.a)
**
** This module will be the only module which uses curl.lib so if you don't
** use anything in this module then you won't need to link with curl.lib.
***************************************************************************
*/

#include "UrlRead.hpp"

#include "StringUtil.hpp"
#include "RuntimeError.hpp"
#include "JSONValue.hpp"
#include "JSONParser.hpp"

#undef SPI_UTIL_CLOCK_EVENTS
#include "ClockUtil.hpp"

#include <sstream>

#ifdef _MSC_VER
#define CURL_STATICLIB
#include <spi_curl/curl.h>
#else
#include <curl/curl.h>
#endif

#include <spi_boost/shared_ptr.hpp>

#define BEGIN_ANONYMOUS_NAMESPACE namespace {
#define END_ANONYMOUS_NAMESPACE }

SPI_UTIL_NAMESPACE

BEGIN_ANONYMOUS_NAMESPACE

// smart container of curl_slist*
// deletes itself on going out of scope
class LinkedList
{
public:
    LinkedList() : m_slist(NULL)
    {}

    ~LinkedList()
    {
        if (m_slist)
        {
            curl_slist_free_all(m_slist);
            m_slist = NULL;
        }
    }

    void Append(const std::string& str)
    {
        curl_slist* tmp = curl_slist_append(m_slist, str.c_str());
        if (!tmp)
            SPI_UTIL_THROW_RUNTIME_ERROR("Could not append '" << str << "' to curl_slist");
        m_slist = tmp;
    }

    curl_slist* slist() { return m_slist; }

private:

    curl_slist* m_slist;
};

struct Data
{
    Data() : oss()
    {}

    std::stringstream oss;

    void add_data(char* ptr, size_t size)
    {
        oss.write(ptr, size);
    }

    std::string str()
    {
        // we might need to put a null terminator on the end of the stream
        // however current observation is that this is unnecessary

        return oss.str();
    }
};

static size_t write_callback(char* ptr, size_t size, size_t n, void* user)
{
    size_t consume = size*n;
    if (consume > 0)
    {
        try
        {
            ((Data*)user)->add_data(ptr, consume);
        }
        catch (...)
        {
            return 0;
        }
    }
    return consume;
}

static void errorHandler(CURLcode status)
{
    if (status != CURLE_OK)
    {
        throw RuntimeError("CURL error: %d: %s",
            (int)status,
            curl_easy_strerror(status));
    }
}

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

void URLReadContentsData(
    Data& data,
    const std::string& url,
    bool noProxy,
    int timeout,
    const std::string& post,
    const std::vector<std::string>& vHeaders)
{
    SPI_UTIL_PRE_CONDITION(timeout != 0);

    LinkedList headers;

    if (!GlobalInitialisation)
        GlobalInitialisation.reset(new GlobalInit());

    CURL* handle = curl_easy_init();
    spi_boost::shared_ptr<CURL> spHandle(handle, curl_easy_cleanup);

    errorHandler(curl_easy_setopt(handle, CURLOPT_URL, url.c_str()));

    if (noProxy)
    {
        std::string noProxyAddress;
        if (StringStartsWith(url, "http://"))
        {
            noProxyAddress = StringSplit(url.substr(7), '/')[0];
        }
        else
        {
            noProxyAddress = StringSplit(url, '/')[0];
        }

        errorHandler(curl_easy_setopt(handle, CURLOPT_NOPROXY, noProxyAddress.c_str()));
    }

    if (vHeaders.size() > 0)
    {
        size_t N = vHeaders.size();
        for (size_t i = 0; i < N; ++i)
        {
            headers.Append(vHeaders[i]);
        }
        errorHandler(curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers.slist()));
    }

    if (!post.empty())
    {
        errorHandler(curl_easy_setopt(handle, CURLOPT_POST, 1L));
        errorHandler(curl_easy_setopt(handle, CURLOPT_POSTFIELDS, post.c_str()));
        errorHandler(curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, (long)post.length()));
    }

    errorHandler(curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback));
    errorHandler(curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data));
    if (timeout > 0)
        errorHandler(curl_easy_setopt(handle, CURLOPT_TIMEOUT, timeout));
    errorHandler(curl_easy_perform(handle));
}

END_ANONYMOUS_NAMESPACE

std::string URLReadContents(
    const std::string& url,
    bool noProxy,
    int timeout,
    const std::string& post,
    const std::vector<std::string>& headers)
{
    if (timeout == 0)
        return std::string();

    Data data;

    URLReadContentsData(data, url, noProxy, timeout, post, headers);

    return data.str();
}

JSONMapConstSP URLReadContentsJSON(
    const std::string& url,
    bool noProxy,
    int timeout,
    const JSONMapConstSP& jsonPost,
    const std::vector<std::string>& headers)
{
    if (timeout == 0)
        return JSONMapConstSP();

    Data data;

    std::string post;
    if (jsonPost)
    {
        post = JSONValueToString(JSONValue(jsonPost));
    }

    URLReadContentsData(data, url, noProxy, timeout, post, headers);

    JSONValue jv = JSONParseValue(data.oss);
    return jv.GetMap();
}


SPI_UTIL_END_NAMESPACE
