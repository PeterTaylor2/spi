/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_BORLAND_H
#define SPI_BOOST_PREDEF_COMPILER_BORLAND_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_BORLAND`]

[@http://en.wikipedia.org/wiki/C_plus_plus_builder Borland C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__BORLANDC__`] [__predef_detection__]]
    [[`__CODEGEARC__`] [__predef_detection__]]

    [[`__BORLANDC__`] [V.R.P]]
    [[`__CODEGEARC__`] [V.R.P]]
    ]
 */

#define SPI_BOOST_COMP_BORLAND SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__BORLANDC__) || defined(__CODEGEARC__)
#   if !defined(SPI_BOOST_COMP_BORLAND_DETECTION) && (defined(__CODEGEARC__))
#       define SPI_BOOST_COMP_BORLAND_DETECTION SPI_BOOST_PREDEF_MAKE_0X_VVRP(__CODEGEARC__)
#   endif
#   if !defined(SPI_BOOST_COMP_BORLAND_DETECTION)
#       define SPI_BOOST_COMP_BORLAND_DETECTION SPI_BOOST_PREDEF_MAKE_0X_VVRP(__BORLANDC__)
#   endif
#endif

#ifdef SPI_BOOST_COMP_BORLAND_DETECTION
#   define SPI_BOOST_COMP_BORLAND_AVAILABLE
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_BORLAND_EMULATED SPI_BOOST_COMP_BORLAND_DETECTION
#   else
#       undef SPI_BOOST_COMP_BORLAND
#       define SPI_BOOST_COMP_BORLAND SPI_BOOST_COMP_BORLAND_DETECTION
#   endif
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_BORLAND_NAME "Borland C++"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_BORLAND,SPI_BOOST_COMP_BORLAND_NAME)

#ifdef SPI_BOOST_COMP_BORLAND_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_BORLAND_EMULATED,SPI_BOOST_COMP_BORLAND_NAME)
#endif
