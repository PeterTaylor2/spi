#include <spi/EnumHelper.hpp>
#include <spi/RuntimeError.hpp>
#include <spi/Value.hpp>

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include <spi_boost/shared_ptr.hpp>

#ifndef _MSC_VER
#include <curl/curl.h>
#else
#include <spi_curl/curl.h>
#endif

#include <spi_util/UrlRead.hpp>

#include <spi_util/TestRunner.hpp>

static void info(const char* name, const char* value)
{
    std::cout << name << ": " << (!value ? "NULL" : value) << std::endl;
}

static void info(const char* name, int value)
{
    std::cout << name << ": " << value << std::endl;
}

static void testFetch(void)
{
    CURLcode status = curl_global_init(CURL_GLOBAL_ALL);
    if (status != CURLE_OK)
    {
        fprintf(stderr, "Curl initialisation failed: %s\n",
            curl_easy_strerror(status));
        return;
    }

    SPI_POST_CONDITION(status == CURLE_OK);

    curl_version_info_data* versionInfo = curl_version_info(CURLVERSION_NOW);
    info("curl version", versionInfo->version);
    info("curl host   ", versionInfo->host);
    info("ssl version ", versionInfo->ssl_version);
    info("zlib version", versionInfo->libz_version);
    info("features    ", versionInfo->features);

    std::cout << "\n";
    const char* env_http_proxy = getenv("http_proxy");
    const char* env_no_proxy = getenv("no_proxy");

    info("env(http_proxy)", env_http_proxy);
    info("env(no_proxy)  ", env_no_proxy);

    bool noProxy = false;
    int timeout = 10;
    std::string url("http://www.example.com");
    std::cout << "\n" << "Reading: " << url << " with noProxy=" << (noProxy ? "true" : "false")
        << " and timeout=" << timeout << std::endl;
    std::string contents = spi_util::URLReadContents(url, noProxy, timeout);
    std::cout << "length(" << contents.length() << ")\n" << std::endl;

    if (!noProxy && env_http_proxy && contents.length() < 50)
    {
        std::cout << "contents=" << contents << std::endl;
    }
}

int main(int argc, char* argv[])
{
    return spi_util::TestRunner(argc, argv,
        "Fetch", testFetch,
        0);
}
