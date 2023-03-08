/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_KAI_H
#define SPI_BOOST_PREDEF_COMPILER_KAI_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_KCC`]

Kai C++ compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__KCC`] [__predef_detection__]]

    [[`__KCC_VERSION`] [V.R.P]]
    ]
 */

#define SPI_BOOST_COMP_KCC SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__KCC)
#   define SPI_BOOST_COMP_KCC_DETECTION SPI_BOOST_PREDEF_MAKE_0X_VRPP(__KCC_VERSION)
#endif

#ifdef SPI_BOOST_COMP_KCC_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_KCC_EMULATED SPI_BOOST_COMP_KCC_DETECTION
#   else
#       undef SPI_BOOST_COMP_KCC
#       define SPI_BOOST_COMP_KCC SPI_BOOST_COMP_KCC_DETECTION
#   endif
#   define SPI_BOOST_COMP_KCC_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_KCC_NAME "Kai C++"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_KCC,SPI_BOOST_COMP_KCC_NAME)

#ifdef SPI_BOOST_COMP_KCC_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_KCC_EMULATED,SPI_BOOST_COMP_KCC_NAME)
#endif
