/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_ARCHITECTURE_SYS390_H
#define SPI_BOOST_PREDEF_ARCHITECTURE_SYS390_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_ARCH_SYS390`]

[@http://en.wikipedia.org/wiki/System/390 System/390] architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__s390__`] [__predef_detection__]]
    [[`__s390x__`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_ARCH_SYS390 SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__s390__) || defined(__s390x__)
#   undef SPI_BOOST_ARCH_SYS390
#   define SPI_BOOST_ARCH_SYS390 SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif

#if SPI_BOOST_ARCH_SYS390
#   define SPI_BOOST_ARCH_SYS390_AVAILABLE
#endif

#define SPI_BOOST_ARCH_SYS390_NAME "System/390"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_ARCH_SYS390,SPI_BOOST_ARCH_SYS390_NAME)
