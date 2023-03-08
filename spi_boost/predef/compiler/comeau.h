/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_COMEAU_H
#define SPI_BOOST_PREDEF_COMPILER_COMEAU_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

#define SPI_BOOST_COMP_COMO SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

/*`
[heading `SPI_BOOST_COMP_COMO`]

[@http://en.wikipedia.org/wiki/Comeau_C/C%2B%2B Comeau C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__COMO__`] [__predef_detection__]]

    [[`__COMO_VERSION__`] [V.R.P]]
    ]
 */

#if defined(__COMO__)
#   if !defined(SPI_BOOST_COMP_COMO_DETECTION) && defined(__COMO_VERSION__)
#       define SPI_BOOST_COMP_COMO_DETECTION SPI_BOOST_PREDEF_MAKE_0X_VRP(__COMO_VERSION__)
#   endif
#   if !defined(SPI_BOOST_COMP_COMO_DETECTION)
#       define SPI_BOOST_COMP_COMO_DETECTION SPI_BOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef SPI_BOOST_COMP_COMO_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_COMO_EMULATED SPI_BOOST_COMP_COMO_DETECTION
#   else
#       undef SPI_BOOST_COMP_COMO
#       define SPI_BOOST_COMP_COMO SPI_BOOST_COMP_COMO_DETECTION
#   endif
#   define SPI_BOOST_COMP_COMO_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_COMO_NAME "Comeau C++"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_COMO,SPI_BOOST_COMP_COMO_NAME)

#ifdef SPI_BOOST_COMP_COMO_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_COMO_EMULATED,SPI_BOOST_COMP_COMO_NAME)
#endif
