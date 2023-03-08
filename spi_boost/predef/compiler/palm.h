/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_PALM_H
#define SPI_BOOST_PREDEF_COMPILER_PALM_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_PALM`]

Palm C/C++ compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`_PACC_VER`] [__predef_detection__]]

    [[`_PACC_VER`] [V.R.P]]
    ]
 */

#define SPI_BOOST_COMP_PALM SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(_PACC_VER)
#   define SPI_BOOST_COMP_PALM_DETECTION SPI_BOOST_PREDEF_MAKE_0X_VRRPP000(_PACC_VER)
#endif

#ifdef SPI_BOOST_COMP_PALM_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_PALM_EMULATED SPI_BOOST_COMP_PALM_DETECTION
#   else
#       undef SPI_BOOST_COMP_PALM
#       define SPI_BOOST_COMP_PALM SPI_BOOST_COMP_PALM_DETECTION
#   endif
#   define SPI_BOOST_COMP_PALM_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_PALM_NAME "Palm C/C++"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_PALM,SPI_BOOST_COMP_PALM_NAME)

#ifdef SPI_BOOST_COMP_PALM_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_PALM_EMULATED,SPI_BOOST_COMP_PALM_NAME)
#endif
