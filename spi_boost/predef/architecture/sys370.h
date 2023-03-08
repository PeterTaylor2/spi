/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_ARCHITECTURE_SYS370_H
#define SPI_BOOST_PREDEF_ARCHITECTURE_SYS370_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_ARCH_SYS370`]

[@http://en.wikipedia.org/wiki/System/370 System/370] architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__370__`] [__predef_detection__]]
    [[`__THW_370__`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_ARCH_SYS370 SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__370__) || defined(__THW_370__)
#   undef SPI_BOOST_ARCH_SYS370
#   define SPI_BOOST_ARCH_SYS370 SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif

#if SPI_BOOST_ARCH_SYS370
#   define SPI_BOOST_ARCH_SYS370_AVAILABLE
#endif

#define SPI_BOOST_ARCH_SYS370_NAME "System/370"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_ARCH_SYS370,SPI_BOOST_ARCH_SYS370_NAME)
