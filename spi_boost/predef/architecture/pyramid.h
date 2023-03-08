/*
Copyright Rene Rivera 2011-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_ARCHITECTURE_PYRAMID_H
#define SPI_BOOST_PREDEF_ARCHITECTURE_PYRAMID_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_ARCH_PYRAMID`]

Pyramid 9810 architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`pyr`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_ARCH_PYRAMID SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(pyr)
#   undef SPI_BOOST_ARCH_PYRAMID
#   define SPI_BOOST_ARCH_PYRAMID SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif

#if SPI_BOOST_ARCH_PYRAMID
#   define SPI_BOOST_ARCH_PYRAMID_AVAILABLE
#endif

#define SPI_BOOST_ARCH_PYRAMID_NAME "Pyramid 9810"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_ARCH_PYRAMID,SPI_BOOST_ARCH_PYRAMID_NAME)
