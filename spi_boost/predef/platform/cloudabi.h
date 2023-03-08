/*
  Copyright 2017 James E. King, III
  Distributed under the Boost Software License, Version 1.0.
  (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_PLAT_CLOUDABI_H
#define SPI_BOOST_PREDEF_PLAT_CLOUDABI_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_PLAT_CLOUDABI`]

[@https://github.com/NuxiNL/cloudabi CloudABI] platform.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__CloudABI__`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_PLAT_CLOUDABI SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__CloudABI__)
#   undef SPI_BOOST_PLAT_CLOUDABI
#   define SPI_BOOST_PLAT_CLOUDABI SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif

#if SPI_BOOST_PLAT_CLOUDABI
#   define SPI_BOOST_PLAT_CLOUDABI_AVAILABLE
#   include <spi_boost/predef/detail/platform_detected.h>
#endif

#define SPI_BOOST_PLAT_CLOUDABI_NAME "CloudABI"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_PLAT_CLOUDABI,SPI_BOOST_PLAT_CLOUDABI_NAME)
