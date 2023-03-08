/*
Copyright Rene Rivera 2011-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_ARCHITECTURE_CONVEX_H
#define SPI_BOOST_PREDEF_ARCHITECTURE_CONVEX_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_ARCH_CONVEX`]

[@http://en.wikipedia.org/wiki/Convex_Computer Convex Computer] architecture.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__convex__`] [__predef_detection__]]

    [[`__convex_c1__`] [1.0.0]]
    [[`__convex_c2__`] [2.0.0]]
    [[`__convex_c32__`] [3.2.0]]
    [[`__convex_c34__`] [3.4.0]]
    [[`__convex_c38__`] [3.8.0]]
    ]
 */

#define SPI_BOOST_ARCH_CONVEX SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__convex__)
#   undef SPI_BOOST_ARCH_CONVEX
#   if !defined(SPI_BOOST_ARCH_CONVEX) && defined(__convex_c1__)
#       define SPI_BOOST_ARCH_CONVEX SPI_BOOST_VERSION_NUMBER(1,0,0)
#   endif
#   if !defined(SPI_BOOST_ARCH_CONVEX) && defined(__convex_c2__)
#       define SPI_BOOST_ARCH_CONVEX SPI_BOOST_VERSION_NUMBER(2,0,0)
#   endif
#   if !defined(SPI_BOOST_ARCH_CONVEX) && defined(__convex_c32__)
#       define SPI_BOOST_ARCH_CONVEX SPI_BOOST_VERSION_NUMBER(3,2,0)
#   endif
#   if !defined(SPI_BOOST_ARCH_CONVEX) && defined(__convex_c34__)
#       define SPI_BOOST_ARCH_CONVEX SPI_BOOST_VERSION_NUMBER(3,4,0)
#   endif
#   if !defined(SPI_BOOST_ARCH_CONVEX) && defined(__convex_c38__)
#       define SPI_BOOST_ARCH_CONVEX SPI_BOOST_VERSION_NUMBER(3,8,0)
#   endif
#   if !defined(SPI_BOOST_ARCH_CONVEX)
#       define SPI_BOOST_ARCH_CONVEX SPI_BOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if SPI_BOOST_ARCH_CONVEX
#   define SPI_BOOST_ARCH_CONVEX_AVAILABLE
#endif

#define SPI_BOOST_ARCH_CONVEX_NAME "Convex Computer"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_ARCH_CONVEX,SPI_BOOST_ARCH_CONVEX_NAME)
