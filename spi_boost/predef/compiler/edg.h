/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_EDG_H
#define SPI_BOOST_PREDEF_COMPILER_EDG_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_EDG`]

[@http://en.wikipedia.org/wiki/Edison_Design_Group EDG C++ Frontend] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__EDG__`] [__predef_detection__]]

    [[`__EDG_VERSION__`] [V.R.0]]
    ]
 */

#define SPI_BOOST_COMP_EDG SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__EDG__)
#   define SPI_BOOST_COMP_EDG_DETECTION SPI_BOOST_PREDEF_MAKE_10_VRR(__EDG_VERSION__)
#endif

#ifdef SPI_BOOST_COMP_EDG_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_EDG_EMULATED SPI_BOOST_COMP_EDG_DETECTION
#   else
#       undef SPI_BOOST_COMP_EDG
#       define SPI_BOOST_COMP_EDG SPI_BOOST_COMP_EDG_DETECTION
#   endif
#   define SPI_BOOST_COMP_EDG_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_EDG_NAME "EDG C++ Frontend"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_EDG,SPI_BOOST_COMP_EDG_NAME)

#ifdef SPI_BOOST_COMP_EDG_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_EDG_EMULATED,SPI_BOOST_COMP_EDG_NAME)
#endif
