#include <spi/EnumHelper.hpp>
#include <spi/RuntimeError.hpp>
#include <spi/Value.hpp>

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <stdio.h>

#include <spi_boost/shared_ptr.hpp>

#ifndef _MSC_VER
#include <curl/curl.h>
#else
#include <spi_curl/curl.h>
#endif

#include <spi_util/TestRunner.hpp>

static void errorHandler(CURLcode status)
{
  // if (status != CURLE_OK)
  //   {
  //     fprintf(stderr, "ERROR: %d\n", (int)status);
  //     fprintf(stderr, "ERROR: %s\n", curl_easy_strerror(status));
  //   }

  printf("status:%d\n", (int)status);
  SPI_POST_CONDITION(status == CURLE_OK);

}

static std::string testFetchOne(CURL* handle, const char* url)
{
  CURLcode status = curl_easy_setopt(handle, CURLOPT_URL, url);
  errorHandler(status);

  status = curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION);
  errorHandler(status);

  status = curl_easy_perform(handle);
  errorHandler(status);


    return std::string();
    // potentially we could compare the results of the HTTP command
    // with the result that we have on file which was obtained using Python
}

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
  
  CURL* handle = curl_easy_init();
  spi_boost::shared_ptr<CURL> spHandle(handle, curl_easy_cleanup);

    testFetchOne(handle,
       "http://www.bridgebase.com/tools/vugraph_linfetch.php?id=40000");

    curl_global_cleanup();
}

int main(int argc, char* argv[])
{
    return spi_util::TestRunner(argc, argv,
				"Fetch", testFetch,
				0);
}
