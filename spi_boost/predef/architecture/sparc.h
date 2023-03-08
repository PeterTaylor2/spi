/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_ARCHITECTURE_SPARC_H
#define SPI_BOOST_PREDEF_ARCHITECTURE_SPARC_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_ARCH_SPARC`]

[@http://en.wikipedia.org/wiki/SPARC SPARC] architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__sparc__`] [__predef_detection__]]
    [[`__sparc`] [__predef_detection__]]

    [[`__sparcv9`] [9.0.0]]
    [[`__sparcv8`] [8.0.0]]
    ]
 */

#define SPI_BOOST_ARCH_SPARC SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__sparc__) || defined(__sparc)
#   undef SPI_BOOST_ARCH_SPARC
#   if !defined(SPI_BOOST_ARCH_SPARC) && defined(__sparcv9)
#       define SPI_BOOST_ARCH_SPARC SPI_BOOST_VERSION_NUMBER(9,0,0)
#   endif
#   if !defined(SPI_BOOST_ARCH_SPARC) && defined(__sparcv8)
#       define SPI_BOOST_ARCH_SPARC SPI_BOOST_VERSION_NUMBER(8,0,0)
#   endif
#   if !defined(SPI_BOOST_ARCH_SPARC)
#       define SPI_BOOST_ARCH_SPARC SPI_BOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if SPI_BOOST_ARCH_SPARC
#   define SPI_BOOST_ARCH_SPARC_AVAILABLE
#endif

#define SPI_BOOST_ARCH_SPARC_NAME "SPARC"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_ARCH_SPARC,SPI_BOOST_ARCH_SPARC_NAME)
