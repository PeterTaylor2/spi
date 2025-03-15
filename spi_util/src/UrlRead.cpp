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
#include "Utils.hpp"

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
    Data() : responseCode(-1), oss(), headers()
    {}

    long responseCode;
    std::stringstream oss;
    std::map<std::string, std::string> headers;

    void add_data(char* ptr, size_t size)
    {
        oss.write(ptr, size);
    }

    void add_header_data(char* ptr, size_t size)
    {
        std::stringstream ss;
        ss.write(ptr, size);
        std::string s = ss.str();
        std::size_t pos = s.find(':');
        if (pos != std::string::npos)
        {
            std::string name = StringStrip(s.substr(0, pos));
            std::string value = StringStrip(s.substr(pos + 1, s.size() - pos));
            headers[StringLower(name)] = value;
        }
    }

    std::string contents()
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

static size_t header_callback(char* ptr, size_t size, size_t n, void* user)
{
    size_t consume = size * n;
    if (consume > 0)
    {
        try
        {
            ((Data*)user)->add_header_data(ptr, consume);
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
    const std::vector<std::string>& vHeaders,
    bool authenticate)
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

    if (authenticate)
    {
        // Enable HTTP Negotiate (SPNEGO) authentication
        errorHandler(curl_easy_setopt(handle, CURLOPT_HTTPAUTH, CURLAUTH_NEGOTIATE));
        // cURL 7 needs an empty username and password for Negotiate authentication to work
        errorHandler(curl_easy_setopt(handle, CURLOPT_USERNAME, ""));
        errorHandler(curl_easy_setopt(handle, CURLOPT_PASSWORD, ""));
    }


    errorHandler(curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback));
    errorHandler(curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data));

    errorHandler(curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, header_callback));
    errorHandler(curl_easy_setopt(handle, CURLOPT_HEADERDATA, &data));

    if (timeout > 0)
        errorHandler(curl_easy_setopt(handle, CURLOPT_TIMEOUT, timeout));
    errorHandler(curl_easy_perform(handle));

    errorHandler(curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &data.responseCode));
}

END_ANONYMOUS_NAMESPACE

std::string URLReadContents(
    const std::string& url,
    bool noProxy,
    int timeout,
    const std::string& post,
    const std::vector<std::string>& headers,
    bool authenticate)
{
    if (timeout == 0)
        return std::string();

    Data data;

    URLReadContentsData(data, url, noProxy, timeout, post, headers, authenticate);

    return data.contents();
}

URLInfoConstSP URLReadInfo(
    const std::string& url,
    bool noProxy,
    int timeout,
    const std::string& post,
    const std::vector<std::string>& headers,
    bool authenticate)
{
    if (timeout == 0)
    {
        // 408 Request Timeout
        return URLInfoConstSP(new URLInfo(408));
    }

    Data data;

    URLReadContentsData(data, url, noProxy, timeout, post, headers, authenticate);

    return URLInfoConstSP(new URLInfo(data.responseCode, data.contents(), data.headers));
}

JSONMapConstSP URLReadContentsJSON(
    const std::string& url,
    bool noProxy,
    int timeout,
    const JSONMapConstSP& jsonPost,
    const std::vector<std::string>& headers,
    bool authenticate)
{
    if (timeout == 0)
        return JSONMapConstSP();

    Data data;

    std::string post;
    if (jsonPost)
    {
        post = JSONValueToString(JSONValue(jsonPost));
    }

    URLReadContentsData(data, url, noProxy, timeout, post, headers, authenticate); 

    JSONValue jv = JSONParseValue(data.oss);
    return jv.GetMap();
}

std::string URLEscape(const std::string& url)
{
    if (!GlobalInitialisation)
        GlobalInitialisation.reset(new GlobalInit());

    CURL* handle = curl_easy_init();

    char* csUrlEscape = curl_easy_escape(handle, url.c_str(), IntegerCast<int>(url.length()));

    if (csUrlEscape)
    {
        std::string urlEscape(csUrlEscape);
        free(csUrlEscape);
        return urlEscape;
    }

    return url;
}

URLInfo::URLInfo(
    long responseCode,
    const std::string& contents,
    const std::map<std::string, std::string>& responseHeaders)
    :
    m_responseCode(responseCode),
    m_contents(contents),
    m_responseHeaders(responseHeaders)
{
}

std::string URLInfo::responseMessage() const
{
    switch (m_responseCode)
    {
    // start with the most common response codes first
    case 200: return "200 OK";
    case 201: return "201 Created";
    case 400: return "400 Bad Request";
    case 401: return "401 Unauthorized";
    case 404: return "404 Not Found";
    case 100: return "100 Continue";
    case 101: return "101 Switching Protocols";
    case 102: return "102 Processing";
    case 103: return "103 Early Hints";
    case 202: return "202 Accepted";
    case 203: return "203 Non-Authorative Information";
    case 204: return "204 No Content";
    case 205: return "205 Reset Content";
    case 206: return "206 Partial Content";
    case 207: return "207 Multi-Status";
    case 208: return "208 Already Reported";
    case 226: return "226 IM Used";
    case 300: return "300 Multiple Choices";
    case 301: return "301 Moved Permanently";
    case 302: return "302 Found";
    case 303: return "303 See Other";
    case 304: return "304 Not Modified";
    case 307: return "307 Temporary Redirect";
    case 308: return "308 Permanent Redirect";
    case 402: return "402 Payment Required";
    case 403: return "403 Forbidden";
    case 405: return "405 Method Not Allowed";
    case 406: return "406 Not Acceptable";
    case 407: return "407 Proxy Authentication Required";
    case 408: return "408 Request Timeout";
    case 409: return "409 Conflict";
    case 410: return "410 Gone";
    case 411: return "411 Length Required";
    case 412: return "412 Precondition Failed";
    case 413: return "413 Payload Too Large";
    case 414: return "414 URI Too Long";
    case 415: return "415 Unsupported Media Type";
    case 416: return "416 Range Not Satisfiable";
    case 417: return "417 Expectation Failed";
    case 418: return "418 I'm a teapot";
    case 421: return "421 Misdirected Request";
    case 422: return "422 Unprocessable Content";
    case 423: return "423 Locked";
    case 424: return "424 Failed Dependency";
    case 425: return "425 Too Early Experimental";
    case 426: return "426 Upgrade Required";
    case 428: return "428 Precondition Required";
    case 429: return "429 Too Many Requests";
    case 431: return "431 Request Header Fields Too Large";
    case 451: return "451 Unavailable For Legal Reasons";
    case 500: return "500 Internal Server Error";
    case 501: return "501 Not Implemented";
    case 502: return "502 Bad Gateway";
    case 503: return "503 Service Unavailable";
    case 504: return "504 Gateway Timeout";
    case 505: return "505 HTTP Version Not Supported";
    case 506: return "506 Variant Also Negotiates";
    case 507: return "507 Insufficient Storage";
    case 508: return "508 Loop Detected";
    case 510: return "510 Not Extended";
    case 511: return "511 Network Authentication Required";
    default: break;
    }

    return StringFormat("%d Unknown (or deprecated) response", m_responseCode);
}

bool URLInfo::failed() const
{
    return m_responseCode < 200 || m_responseCode >= 400;
}


SPI_UTIL_END_NAMESPACE

