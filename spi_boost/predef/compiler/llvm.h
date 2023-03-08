/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_LLVM_H
#define SPI_BOOST_PREDEF_COMPILER_LLVM_H

/* Other compilers that emulate this one need to be detected first. */

#include <spi_boost/predef/compiler/clang.h>

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_LLVM`]

[@http://en.wikipedia.org/wiki/LLVM LLVM] compiler.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__llvm__`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_COMP_LLVM SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__llvm__)
#   define SPI_BOOST_COMP_LLVM_DETECTION SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif

#ifdef SPI_BOOST_COMP_LLVM_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_LLVM_EMULATED SPI_BOOST_COMP_LLVM_DETECTION
#   else
#       undef SPI_BOOST_COMP_LLVM
#       define SPI_BOOST_COMP_LLVM SPI_BOOST_COMP_LLVM_DETECTION
#   endif
#   define SPI_BOOST_COMP_LLVM_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_LLVM_NAME "LLVM"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_LLVM,SPI_BOOST_COMP_LLVM_NAME)

#ifdef SPI_BOOST_COMP_LLVM_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_LLVM_EMULATED,SPI_BOOST_COMP_LLVM_NAME)
#endif
