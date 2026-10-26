#
# information about CURL source code for windows builds
# since libcurl is not provided by default on windows we create our own library
# and use spi-curl to distinguish it from other instances of libcurl
#

G_WIN32_HAS_CURL=1
G_WIN32_CURL_VERSION=8.21.0
G_WIN32_CURL_HOME=$(U_SPI_HOME)/3rdParty/curl-$(G_WIN32_CURL_VERSION)
G_WIN32_CURL_DLL=spi-curl-$(G_WIN32_CURL_VERSION)
SPI_CURL_DLL=$(G_WIN32_CURL_DLL)

