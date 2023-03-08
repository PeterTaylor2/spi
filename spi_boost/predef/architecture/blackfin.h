/*
Copyright Rene Rivera 2013-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_ARCHITECTURE_BLACKFIN_H
#define SPI_BOOST_PREDEF_ARCHITECTURE_BLACKFIN_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_ARCH_BLACKFIN`]

Blackfin Processors from Analog Devices.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__bfin__`] [__predef_detection__]]
    [[`__BFIN__`] [__predef_detection__]]
    [[`bfin`] [__predef_detection__]]
    [[`BFIN`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_ARCH_BLACKFIN SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__bfin__) || defined(__BFIN__) || \
    defined(bfin) || defined(BFIN)
#   undef SPI_BOOST_ARCH_BLACKFIN
#   define SPI_BOOST_ARCH_BLACKFIN SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif

#if SPI_BOOST_ARCH_BLACKFIN
#   define SPI_BOOST_ARCH_BLACKFIN_AVAILABLE
#endif

#define SPI_BOOST_ARCH_BLACKFIN_NAME "Blackfin"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_ARCH_BLACKFIN,SPI_BOOST_ARCH_BLACKFIN_NAME)
