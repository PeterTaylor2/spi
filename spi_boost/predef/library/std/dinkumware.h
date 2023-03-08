/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_LIBRARY_STD_DINKUMWARE_H
#define SPI_BOOST_PREDEF_LIBRARY_STD_DINKUMWARE_H

#include <spi_boost/predef/library/std/_prefix.h>

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_LIB_STD_DINKUMWARE`]

[@http://en.wikipedia.org/wiki/Dinkumware Dinkumware] Standard C++ Library.
If available version number as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`_YVALS`, `__IBMCPP__`] [__predef_detection__]]
    [[`_CPPLIB_VER`] [__predef_detection__]]

    [[`_CPPLIB_VER`] [V.R.0]]
    ]
 */

#define SPI_BOOST_LIB_STD_DINKUMWARE SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if (defined(_YVALS) && !defined(__IBMCPP__)) || defined(_CPPLIB_VER)
#   undef SPI_BOOST_LIB_STD_DINKUMWARE
#   if defined(_CPPLIB_VER)
#       define SPI_BOOST_LIB_STD_DINKUMWARE SPI_BOOST_PREDEF_MAKE_10_VVRR(_CPPLIB_VER)
#   else
#       define SPI_BOOST_LIB_STD_DINKUMWARE SPI_BOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if SPI_BOOST_LIB_STD_DINKUMWARE
#   define SPI_BOOST_LIB_STD_DINKUMWARE_AVAILABLE
#endif

#define SPI_BOOST_LIB_STD_DINKUMWARE_NAME "Dinkumware"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_LIB_STD_DINKUMWARE,SPI_BOOST_LIB_STD_DINKUMWARE_NAME)
