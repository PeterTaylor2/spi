/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_ARCHITECTURE_ALPHA_H
#define SPI_BOOST_PREDEF_ARCHITECTURE_ALPHA_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_ARCH_ALPHA`]

[@http://en.wikipedia.org/wiki/DEC_Alpha DEC Alpha] architecture.

[table
    [[__predef_symbol__] [__predef_version__]]
    [[`__alpha__`] [__predef_detection__]]
    [[`__alpha`] [__predef_detection__]]
    [[`_M_ALPHA`] [__predef_detection__]]

    [[`__alpha_ev4__`] [4.0.0]]
    [[`__alpha_ev5__`] [5.0.0]]
    [[`__alpha_ev6__`] [6.0.0]]
    ]
 */

#define SPI_BOOST_ARCH_ALPHA SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__alpha__) || defined(__alpha) || \
    defined(_M_ALPHA)
#   undef SPI_BOOST_ARCH_ALPHA
#   if !defined(SPI_BOOST_ARCH_ALPHA) && defined(__alpha_ev4__)
#       define SPI_BOOST_ARCH_ALPHA SPI_BOOST_VERSION_NUMBER(4,0,0)
#   endif
#   if !defined(SPI_BOOST_ARCH_ALPHA) && defined(__alpha_ev5__)
#       define SPI_BOOST_ARCH_ALPHA SPI_BOOST_VERSION_NUMBER(5,0,0)
#   endif
#   if !defined(SPI_BOOST_ARCH_ALPHA) && defined(__alpha_ev6__)
#       define SPI_BOOST_ARCH_ALPHA SPI_BOOST_VERSION_NUMBER(6,0,0)
#   endif
#   if !defined(SPI_BOOST_ARCH_ALPHA)
#       define SPI_BOOST_ARCH_ALPHA SPI_BOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if SPI_BOOST_ARCH_ALPHA
#   define SPI_BOOST_ARCH_ALPHA_AVAILABLE
#endif

#define SPI_BOOST_ARCH_ALPHA_NAME "DEC Alpha"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_ARCH_ALPHA,SPI_BOOST_ARCH_ALPHA_NAME)
