/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_ARCHITECTURE_Z_H
#define SPI_BOOST_PREDEF_ARCHITECTURE_Z_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_ARCH_Z`]

[@http://en.wikipedia.org/wiki/Z/Architecture z/Architecture] architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__SYSC_ZARCH__`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_ARCH_Z SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__SYSC_ZARCH__)
#   undef SPI_BOOST_ARCH_Z
#   define SPI_BOOST_ARCH_Z SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif

#if SPI_BOOST_ARCH_Z
#   define SPI_BOOST_ARCH_Z_AVAILABLE
#endif

#define SPI_BOOST_ARCH_Z_NAME "z/Architecture"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_ARCH_Z,SPI_BOOST_ARCH_Z_NAME)
