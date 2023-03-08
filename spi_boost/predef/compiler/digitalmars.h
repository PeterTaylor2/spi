/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_DIGITALMARS_H
#define SPI_BOOST_PREDEF_COMPILER_DIGITALMARS_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_DMC`]

[@http://en.wikipedia.org/wiki/Digital_Mars Digital Mars] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__DMC__`] [__predef_detection__]]

    [[`__DMC__`] [V.R.P]]
    ]
 */

#define SPI_BOOST_COMP_DMC SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__DMC__)
#   define SPI_BOOST_COMP_DMC_DETECTION SPI_BOOST_PREDEF_MAKE_0X_VRP(__DMC__)
#endif

#ifdef SPI_BOOST_COMP_DMC_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_DMC_EMULATED SPI_BOOST_COMP_DMC_DETECTION
#   else
#       undef SPI_BOOST_COMP_DMC
#       define SPI_BOOST_COMP_DMC SPI_BOOST_COMP_DMC_DETECTION
#   endif
#   define SPI_BOOST_COMP_DMC_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_DMC_NAME "Digital Mars"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_DMC,SPI_BOOST_COMP_DMC_NAME)

#ifdef SPI_BOOST_COMP_DMC_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_DMC_EMULATED,SPI_BOOST_COMP_DMC_NAME)
#endif
