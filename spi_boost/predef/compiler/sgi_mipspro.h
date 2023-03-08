/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_SGI_MIPSPRO_H
#define SPI_BOOST_PREDEF_COMPILER_SGI_MIPSPRO_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_SGI`]

[@http://en.wikipedia.org/wiki/MIPSpro SGI MIPSpro] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__sgi`] [__predef_detection__]]
    [[`sgi`] [__predef_detection__]]

    [[`_SGI_COMPILER_VERSION`] [V.R.P]]
    [[`_COMPILER_VERSION`] [V.R.P]]
    ]
 */

#define SPI_BOOST_COMP_SGI SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__sgi) || defined(sgi)
#   if !defined(SPI_BOOST_COMP_SGI_DETECTION) && defined(_SGI_COMPILER_VERSION)
#       define SPI_BOOST_COMP_SGI_DETECTION SPI_BOOST_PREDEF_MAKE_10_VRP(_SGI_COMPILER_VERSION)
#   endif
#   if !defined(SPI_BOOST_COMP_SGI_DETECTION) && defined(_COMPILER_VERSION)
#       define SPI_BOOST_COMP_SGI_DETECTION SPI_BOOST_PREDEF_MAKE_10_VRP(_COMPILER_VERSION)
#   endif
#   if !defined(SPI_BOOST_COMP_SGI_DETECTION)
#       define SPI_BOOST_COMP_SGI_DETECTION SPI_BOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef SPI_BOOST_COMP_SGI_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_SGI_EMULATED SPI_BOOST_COMP_SGI_DETECTION
#   else
#       undef SPI_BOOST_COMP_SGI
#       define SPI_BOOST_COMP_SGI SPI_BOOST_COMP_SGI_DETECTION
#   endif
#   define SPI_BOOST_COMP_SGI_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_SGI_NAME "SGI MIPSpro"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_SGI,SPI_BOOST_COMP_SGI_NAME)

#ifdef SPI_BOOST_COMP_SGI_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_SGI_EMULATED,SPI_BOOST_COMP_SGI_NAME)
#endif
