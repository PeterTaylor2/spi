/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_DIGNUS_H
#define SPI_BOOST_PREDEF_COMPILER_DIGNUS_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_SYSC`]

[@http://www.dignus.com/dcxx/ Dignus Systems/C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__SYSC__`] [__predef_detection__]]

    [[`__SYSC_VER__`] [V.R.P]]
    ]
 */

#define SPI_BOOST_COMP_SYSC SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__SYSC__)
#   define SPI_BOOST_COMP_SYSC_DETECTION SPI_BOOST_PREDEF_MAKE_10_VRRPP(__SYSC_VER__)
#endif

#ifdef SPI_BOOST_COMP_SYSC_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_SYSC_EMULATED SPI_BOOST_COMP_SYSC_DETECTION
#   else
#       undef SPI_BOOST_COMP_SYSC
#       define SPI_BOOST_COMP_SYSC SPI_BOOST_COMP_SYSC_DETECTION
#   endif
#   define SPI_BOOST_COMP_SYSC_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_SYSC_NAME "Dignus Systems/C++"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_SYSC,SPI_BOOST_COMP_SYSC_NAME)

#ifdef SPI_BOOST_COMP_SYSC_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_SYSC_EMULATED,SPI_BOOST_COMP_SYSC_NAME)
#endif
