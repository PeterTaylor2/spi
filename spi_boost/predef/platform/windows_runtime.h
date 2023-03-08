/*
Copyright (c) Microsoft Corporation 2014
Copyright Rene Rivera 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_PLAT_WINDOWS_RUNTIME_H
#define SPI_BOOST_PREDEF_PLAT_WINDOWS_RUNTIME_H

#include <spi_boost/predef/make.h>
#include <spi_boost/predef/os/windows.h>
#include <spi_boost/predef/platform/windows_phone.h>
#include <spi_boost/predef/platform/windows_store.h>
#include <spi_boost/predef/version_number.h>

/*`
[heading `SPI_BOOST_PLAT_WINDOWS_RUNTIME`]

Deprecated.

[@https://docs.microsoft.com/en-us/windows/uwp/get-started/universal-application-platform-guide UWP]
for Windows Phone or Store development.  This does not align to the existing development model for
UWP and is deprecated.  Use one of the other `SPI_BOOST_PLAT_WINDOWS_*`definitions instead.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`SPI_BOOST_PLAT_WINDOWS_PHONE`] [__predef_detection__]]
    [[`SPI_BOOST_PLAT_WINDOWS_STORE`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_PLAT_WINDOWS_RUNTIME SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if SPI_BOOST_OS_WINDOWS && \
    (SPI_BOOST_PLAT_WINDOWS_STORE || SPI_BOOST_PLAT_WINDOWS_PHONE)
#   undef SPI_BOOST_PLAT_WINDOWS_RUNTIME
#   define SPI_BOOST_PLAT_WINDOWS_RUNTIME SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif
 
#if SPI_BOOST_PLAT_WINDOWS_RUNTIME
#   define SPI_BOOST_PLAT_WINDOWS_RUNTIME_AVAILABLE
#   include <spi_boost/predef/detail/platform_detected.h>
#endif

#define SPI_BOOST_PLAT_WINDOWS_RUNTIME_NAME "Windows Runtime"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_PLAT_WINDOWS_RUNTIME,SPI_BOOST_PLAT_WINDOWS_RUNTIME_NAME)
