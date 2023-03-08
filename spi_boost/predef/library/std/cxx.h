/*
Copyright Rene Rivera 2011-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_LIBRARY_STD_CXX_H
#define SPI_BOOST_PREDEF_LIBRARY_STD_CXX_H

#include <spi_boost/predef/library/std/_prefix.h>

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_LIB_STD_CXX`]

[@http://libcxx.llvm.org/ libc++] C++ Standard Library.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`_LIBCPP_VERSION`] [__predef_detection__]]

    [[`_LIBCPP_VERSION`] [V.0.P]]
    ]
 */

#define SPI_BOOST_LIB_STD_CXX SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(_LIBCPP_VERSION)
#   undef SPI_BOOST_LIB_STD_CXX
#   define SPI_BOOST_LIB_STD_CXX SPI_BOOST_PREDEF_MAKE_10_VPPP(_LIBCPP_VERSION)
#endif

#if SPI_BOOST_LIB_STD_CXX
#   define SPI_BOOST_LIB_STD_CXX_AVAILABLE
#endif

#define SPI_BOOST_LIB_STD_CXX_NAME "libc++"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_LIB_STD_CXX,SPI_BOOST_LIB_STD_CXX_NAME)
