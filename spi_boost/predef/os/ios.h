/*
Copyright Franz Detro 2014
Copyright Rene Rivera 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_OS_IOS_H
#define SPI_BOOST_PREDEF_OS_IOS_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_OS_IOS`]

[@http://en.wikipedia.org/wiki/iOS iOS] operating system.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__APPLE__`] [__predef_detection__]]
    [[`__MACH__`] [__predef_detection__]]
    [[`__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__`] [__predef_detection__]]

    [[`__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__`] [__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__*1000]]
    ]
 */

#define SPI_BOOST_OS_IOS SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(SPI_BOOST_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(__APPLE__) && defined(__MACH__) && \
    defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) \
    )
#   undef SPI_BOOST_OS_IOS
#   define SPI_BOOST_OS_IOS (__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__*1000)
#endif

#if SPI_BOOST_OS_IOS
#   define SPI_BOOST_OS_IOS_AVAILABLE
#   include <spi_boost/predef/detail/os_detected.h>
#endif

#define SPI_BOOST_OS_IOS_NAME "iOS"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_OS_IOS,SPI_BOOST_OS_IOS_NAME)
