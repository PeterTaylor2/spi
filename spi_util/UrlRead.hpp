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
#ifndef SPI_UTIL_URL_READ_HPP
#define SPI_UTIL_URL_READ_HPP

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

#include "DeclSpec.h"
#include "Namespace.hpp"
#include "RefCounter.hpp"
#include "JSONValue.hpp"
#include <string>
#include <vector>
#include <map>

SPI_UTIL_NAMESPACE

SPI_UTIL_DECLARE_RC_CLASS(URLInfo);

class SPI_UTIL_IMPORT URLInfo : public RefCounter
{
public:

    URLInfo(
        long responseCode,
        const std::string& contents = std::string(),
        const std::map<std::string, std::string>& responseHeaders = std::map<std::string,std::string>());

private:
    long m_responseCode;
    std::string m_contents;
    std::map<std::string, std::string> m_responseHeaders;

public:
    long responseCode() const { return m_responseCode; }
    const std::string& contents() const { return m_contents; }
    const std::map<std::string, std::string>& responseHeaders() const { return m_responseHeaders; }

    // converts the responseCode to a responseMessage
    // you should really only use this if the response failed in order to form an exception
    std::string responseMessage() const;

    bool failed() const;
};

/**
 * Reads the entire contents for the given URL.
 *
 * No caching is performed by this function.
 *
 * @param url
 * @param noProxy
 *    If this is defined then we will not attempt to use the proxy server
 *    for reading the URL.
 * @param timeout
 *    If this is positive then it is a timeout in seconds.
 *    If this is zero then the function always fails by returning an empty string.
 *    If this is negative then there is no timeout.
 */
SPI_UTIL_IMPORT
std::string URLReadContents(
    const std::string& url,
    bool noProxy,
    int timeout,
    const std::string& post = std::string(),
    const std::vector<std::string>& headers = std::vector<std::string>(),
    bool authenticate = false);

/**
 * Reads the entire contents for the given URL.
 * Including reposonse code and response headers
 *
 * No caching is performed by this function.
 *
 * @param url
 * @param noProxy
 *    If this is defined then we will not attempt to use the proxy server
 *    for reading the URL.
 * @param timeout
 *    If this is positive then it is a timeout in seconds.
 *    If this is zero then the function always fails by returning an empty string.
 *    If this is negative then there is no timeout.
 */
SPI_UTIL_IMPORT
URLInfoConstSP URLReadInfo(
    const std::string& url,
    bool noProxy,
    int timeout,
    const std::string& post = std::string(),
    const std::vector<std::string>& headers = std::vector<std::string>(),
    bool authenticate = false);

/**
 * Reads the entire contents for the given URL.
 *
 * No caching is performed by this function.
 *
 * @param url
 * @param noProxy
 *    If this is defined then we will not attempt to use the proxy server
 *    for reading the URL.
 * @param timeout
 *    If this is positive then it is a timeout in seconds.
 *    If this is zero then the function always fails by returning an empty string.
 *    If this is negative then there is no timeout.
 */
SPI_UTIL_IMPORT
JSONMapConstSP URLReadContentsJSON(
    const std::string& url,
    bool noProxy,
    int timeout,
    const JSONMapConstSP& post = JSONMapConstSP(),
    const std::vector<std::string>& headers = std::vector<std::string>(),
    bool authenticate = false);

SPI_UTIL_END_NAMESPACE

#endif

