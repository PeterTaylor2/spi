/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_WATCOM_H
#define SPI_BOOST_PREDEF_COMPILER_WATCOM_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_WATCOM`]

[@http://en.wikipedia.org/wiki/Watcom Watcom C++] compiler.
Version number available as major, and minor.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__WATCOMC__`] [__predef_detection__]]

    [[`__WATCOMC__`] [V.R.P]]
    ]
 */

#define SPI_BOOST_COMP_WATCOM SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__WATCOMC__)
#   define SPI_BOOST_COMP_WATCOM_DETECTION SPI_BOOST_PREDEF_MAKE_10_VVRR(__WATCOMC__)
#endif

#ifdef SPI_BOOST_COMP_WATCOM_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_WATCOM_EMULATED SPI_BOOST_COMP_WATCOM_DETECTION
#   else
#       undef SPI_BOOST_COMP_WATCOM
#       define SPI_BOOST_COMP_WATCOM SPI_BOOST_COMP_WATCOM_DETECTION
#   endif
#   define SPI_BOOST_COMP_WATCOM_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_WATCOM_NAME "Watcom C++"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_WATCOM,SPI_BOOST_COMP_WATCOM_NAME)

#ifdef SPI_BOOST_COMP_WATCOM_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_WATCOM_EMULATED,SPI_BOOST_COMP_WATCOM_NAME)
#endif
