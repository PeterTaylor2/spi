/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_CLANG_H
#define SPI_BOOST_PREDEF_COMPILER_CLANG_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_CLANG`]

[@http://en.wikipedia.org/wiki/Clang Clang] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__clang__`] [__predef_detection__]]

    [[`__clang_major__`, `__clang_minor__`, `__clang_patchlevel__`] [V.R.P]]
    ]
 */

#define SPI_BOOST_COMP_CLANG SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__clang__)
#   define SPI_BOOST_COMP_CLANG_DETECTION SPI_BOOST_VERSION_NUMBER(__clang_major__,__clang_minor__,__clang_patchlevel__)
#endif

#ifdef SPI_BOOST_COMP_CLANG_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_CLANG_EMULATED SPI_BOOST_COMP_CLANG_DETECTION
#   else
#       undef SPI_BOOST_COMP_CLANG
#       define SPI_BOOST_COMP_CLANG SPI_BOOST_COMP_CLANG_DETECTION
#   endif
#   define SPI_BOOST_COMP_CLANG_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_CLANG_NAME "Clang"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_CLANG,SPI_BOOST_COMP_CLANG_NAME)

#ifdef SPI_BOOST_COMP_CLANG_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_CLANG_EMULATED,SPI_BOOST_COMP_CLANG_NAME)
#endif
