/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_IAR_H
#define SPI_BOOST_PREDEF_COMPILER_IAR_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_IAR`]

IAR C/C++ compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__IAR_SYSTEMS_ICC__`] [__predef_detection__]]

    [[`__VER__`] [V.R.P]]
    ]
 */

#define SPI_BOOST_COMP_IAR SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__IAR_SYSTEMS_ICC__)
#   define SPI_BOOST_COMP_IAR_DETECTION SPI_BOOST_PREDEF_MAKE_10_VVRR(__VER__)
#endif

#ifdef SPI_BOOST_COMP_IAR_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_IAR_EMULATED SPI_BOOST_COMP_IAR_DETECTION
#   else
#       undef SPI_BOOST_COMP_IAR
#       define SPI_BOOST_COMP_IAR SPI_BOOST_COMP_IAR_DETECTION
#   endif
#   define SPI_BOOST_COMP_IAR_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_IAR_NAME "IAR C/C++"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_IAR,SPI_BOOST_COMP_IAR_NAME)

#ifdef SPI_BOOST_COMP_IAR_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_IAR_EMULATED,SPI_BOOST_COMP_IAR_NAME)
#endif
