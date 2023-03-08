/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_OS_LINUX_H
#define SPI_BOOST_PREDEF_OS_LINUX_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_OS_LINUX`]

[@http://en.wikipedia.org/wiki/Linux Linux] operating system.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`linux`] [__predef_detection__]]
    [[`__linux`] [__predef_detection__]]
    [[`__linux__`] [__predef_detection__]]
    [[`__gnu_linux__`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_OS_LINUX SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(SPI_BOOST_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(linux) || defined(__linux) || \
    defined(__linux__) || defined(__gnu_linux__) \
    )
#   undef SPI_BOOST_OS_LINUX
#   define SPI_BOOST_OS_LINUX SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif

#if SPI_BOOST_OS_LINUX
#   define SPI_BOOST_OS_LINUX_AVAILABLE
#   include <spi_boost/predef/detail/os_detected.h>
#endif

#define SPI_BOOST_OS_LINUX_NAME "Linux"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_OS_LINUX,SPI_BOOST_OS_LINUX_NAME)
