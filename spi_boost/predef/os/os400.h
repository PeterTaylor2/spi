/*
Copyright Rene Rivera 2011-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_OS_OS400_H
#define SPI_BOOST_PREDEF_OS_OS400_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_OS_OS400`]

[@http://en.wikipedia.org/wiki/IBM_i IBM OS/400] operating system.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__OS400__`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_OS_OS400 SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(SPI_BOOST_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(__OS400__) \
    )
#   undef SPI_BOOST_OS_OS400
#   define SPI_BOOST_OS_OS400 SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif

#if SPI_BOOST_OS_OS400
#   define SPI_BOOST_OS_OS400_AVAILABLE
#   include <spi_boost/predef/detail/os_detected.h>
#endif

#define SPI_BOOST_OS_OS400_NAME "IBM OS/400"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_OS_OS400,SPI_BOOST_OS_OS400_NAME)
