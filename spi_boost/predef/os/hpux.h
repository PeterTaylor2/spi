/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_OS_HPUX_H
#define SPI_BOOST_PREDEF_OS_HPUX_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_OS_HPUX`]

[@http://en.wikipedia.org/wiki/HP-UX HP-UX] operating system.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`hpux`] [__predef_detection__]]
    [[`_hpux`] [__predef_detection__]]
    [[`__hpux`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_OS_HPUX SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(SPI_BOOST_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(hpux) || defined(_hpux) || defined(__hpux) \
    )
#   undef SPI_BOOST_OS_HPUX
#   define SPI_BOOST_OS_HPUX SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif

#if SPI_BOOST_OS_HPUX
#   define SPI_BOOST_OS_HPUX_AVAILABLE
#   include <spi_boost/predef/detail/os_detected.h>
#endif

#define SPI_BOOST_OS_HPUX_NAME "HP-UX"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_OS_HPUX,SPI_BOOST_OS_HPUX_NAME)
