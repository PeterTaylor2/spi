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

#include "ObjectURL.hpp"
#include "Service.hpp"
#include "InputContext.hpp"
#include "RuntimeError.hpp"

#include <spi_util/UrlRead.hpp>
#include <time.h>

SPI_BEGIN_NAMESPACE

namespace {

SPI_DECLARE_RC_CLASS(UrlCache);

class UrlCache : public RefCounter
{
public:
    UrlCache() : m_cache()
    {}

    void from_file(const char* filename)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void to_file(const char* filename) const
    {
        SPI_NOT_IMPLEMENTED;
    }

    size_t size() const
    {
        return m_cache.size();
    }

    bool find(const std::string& url, time_t& timeEntry, std::string& dataEntry) const
    {
        std::map<std::string, std::pair<time_t,std::string> >::const_iterator
            iter = m_cache.find(url);

        if (iter == m_cache.end())
            return false;

        timeEntry = iter->second.first;
        dataEntry = iter->second.second;
        return true;
    }

    void add(const std::string& url, const std::string& data)
    {
        m_cache[url] = std::pair<time_t,std::string>(time(NULL), data);
    }

    void clear_entry(const std::string& url)
    {
        if (m_cache.count(url) != 0)
            m_cache.erase(url);
    }

private:

    std::map<std::string, std::pair<time_t, std::string> > m_cache;

};

UrlCacheSP theCache;

} // end of anonymous namespace

std::string read_url(const std::string& url,
    int timeout,
    int cacheAge)
{
    if (theCache && cacheAge >= 0)
    {
        time_t timeEntry;
        std::string dataEntry;

        bool inCache = theCache->find(url, timeEntry, dataEntry);
        if (inCache)
        {
            if (cacheAge == 0)
                return dataEntry;

            time_t timeNow = time(NULL);
            if (timeNow <= timeEntry + cacheAge)
                return dataEntry;
        }
    }

    std::string post;
    std::vector<std::string> headers;
    bool authenticate = false;
    bool compress = true;

    std::string contents = spi_util::URLReadContents(url, true, timeout,
        post, headers, authenticate, compress);

    if (contents.length() > 0 && theCache)
    {
        theCache->add(url, contents);
    }

    return contents;
}

void url_cache_clear_entry(const std::string& url)
{
    if (theCache)
        theCache->clear_entry(url);
}

void url_cache_clear()
{
    theCache.reset();
}

void url_cache_init()
{
    if (!theCache)
    {
        theCache.reset(new UrlCache());
    }
}

void url_cache_load(const char* filename)
{
    url_cache_init();
    theCache->from_file(filename);
}

void url_cache_save(const char* filename)
{
    if (theCache)
        theCache->to_file(filename);
}

size_t url_cache_size()
{
    if (theCache)
        return theCache->size();

    return 0;
}

Value ObjectFromURL(
    const ServiceSP& service,
    const Value& in_url,
    const Value& in_timeout,
    const Value& in_names,
    const Value& v1,
    const Value& v2,
    const Value& v3,
    const Value& v4,
    const Value& v5,
    const Value& v6,
    const Value& v7,
    const Value& v8,
    const Value& v9,
    const Value& v10,
    const Value& v11,
    const Value& v12,
    const Value& v13,
    const Value& v14,
    const Value& v15,
    const Value& v16,
    const Value& v17,
    const Value& v18,
    const Value& v19,
    const Value& v20,
    const InputContext* context)
{
    Value values[20] = {v1, v2, v3, v4, v5, v6, v7, v8, v9, v10,
        v11, v12, v13, v14, v15, v16, v17, v18, v19, v20};

    std::string url = context->ValueToString(in_url);

    try
    {
        std::vector<int> timeoutVector = context->ValueToIntVector(
            in_timeout, true);
        std::vector<std::string> names = context->ValueToStringVector(
            in_names, true);

        int timeout;
        int cacheAge;

        switch(timeoutVector.size())
        {
        case 0:
            timeout = 0;
            cacheAge = 0;
        case 1:
            timeout = timeoutVector[0];
            cacheAge = 0;
            break;
        case 2:
            timeout = timeoutVector[0];
            cacheAge = timeoutVector[1];
            break;
        default:
            throw RuntimeError("%s: Too many inputs for timeout (%d) - "
                "maximum is 2", __FUNCTION__, (int)timeoutVector.size());
        }

        const char* sep = "?";
        std::ostringstream oss;
        oss << url;

        size_t numNames = names.size();
        if (numNames > 20)
            throw RuntimeError("%s: Too many names (%d) - maximum is 20",
                __FUNCTION__, (int)numNames);

        for (size_t i = 0; i < numNames; ++i)
        {
            if (names[i].length() == 0)
                continue;

            std::string value = context->ValueToString(values[i], true);
            oss << sep << names[i] << "=" << value;
            sep = "&";
        }

        url = oss.str();
        ObjectConstSP obj = service->object_from_url(url, timeout, cacheAge);

        return Value(obj);
    }
    catch (std::exception& e)
    {
        throw RuntimeError("%s: Failed to read from URL:%s\n\t:\n%s",
            __FUNCTION__, url.c_str(), e.what());
    }
}

Value UrlCacheSize(
    const ServiceSP& service)
{
    return Value((int)url_cache_size());
}

Value UrlCacheSave(
    const ServiceSP& service,
    const Value& filename,
    const InputContext* context)
{
    url_cache_save(
        context->ValueToString(filename).c_str());

    return Value(true);
}

Value UrlCacheLoad(
    const ServiceSP& service,
    const Value& filename,
    const InputContext* context)
{
    url_cache_load(
        context->ValueToString(filename).c_str());

    return Value(true);
}

Value UrlCacheInit(const ServiceSP& service)
{
    url_cache_init();
    return Value(true);
}

Value UrlCacheClear(const ServiceSP& service)
{
    url_cache_clear();
    return Value(true);
}

SPI_END_NAMESPACE
