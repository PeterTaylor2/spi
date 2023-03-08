/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_GCC_H
#define SPI_BOOST_PREDEF_COMPILER_GCC_H

/* Other compilers that emulate this one need to be detected first. */

#include <spi_boost/predef/compiler/clang.h>

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_GNUC`]

[@http://en.wikipedia.org/wiki/GNU_Compiler_Collection Gnu GCC C/C++] compiler.
Version number available as major, minor, and patch (if available).

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__GNUC__`] [__predef_detection__]]

    [[`__GNUC__`, `__GNUC_MINOR__`, `__GNUC_PATCHLEVEL__`] [V.R.P]]
    [[`__GNUC__`, `__GNUC_MINOR__`] [V.R.0]]
    ]
 */

#define SPI_BOOST_COMP_GNUC SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__GNUC__)
#   if !defined(SPI_BOOST_COMP_GNUC_DETECTION) && defined(__GNUC_PATCHLEVEL__)
#       define SPI_BOOST_COMP_GNUC_DETECTION \
            SPI_BOOST_VERSION_NUMBER(__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__)
#   endif
#   if !defined(SPI_BOOST_COMP_GNUC_DETECTION)
#       define SPI_BOOST_COMP_GNUC_DETECTION \
            SPI_BOOST_VERSION_NUMBER(__GNUC__,__GNUC_MINOR__,0)
#   endif
#endif

#ifdef SPI_BOOST_COMP_GNUC_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_GNUC_EMULATED SPI_BOOST_COMP_GNUC_DETECTION
#   else
#       undef SPI_BOOST_COMP_GNUC
#       define SPI_BOOST_COMP_GNUC SPI_BOOST_COMP_GNUC_DETECTION
#   endif
#   define SPI_BOOST_COMP_GNUC_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_GNUC_NAME "Gnu GCC C/C++"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_GNUC,SPI_BOOST_COMP_GNUC_NAME)

#ifdef SPI_BOOST_COMP_GNUC_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_GNUC_EMULATED,SPI_BOOST_COMP_GNUC_NAME)
#endif
