/*
Copyright Rene Rivera 2015-2019
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_PLAT_ANDROID_H
#define SPI_BOOST_PREDEF_PLAT_ANDROID_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_PLAT_ANDROID`]

[@http://en.wikipedia.org/wiki/Android_%28operating_system%29 Android] platform.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__ANDROID__`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_PLAT_ANDROID SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__ANDROID__)
#   undef SPI_BOOST_PLAT_ANDROID
#   define SPI_BOOST_PLAT_ANDROID SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif

#if SPI_BOOST_PLAT_ANDROID
#   define SPI_BOOST_PLAT_ANDROID_AVAILABLE
#   include <spi_boost/predef/detail/platform_detected.h>
#endif

#define SPI_BOOST_PLAT_ANDROID_NAME "Android"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_PLAT_ANDROID,SPI_BOOST_PLAT_ANDROID_NAME)
