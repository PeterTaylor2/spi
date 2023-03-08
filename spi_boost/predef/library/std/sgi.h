/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_LIBRARY_STD_SGI_H
#define SPI_BOOST_PREDEF_LIBRARY_STD_SGI_H

#include <spi_boost/predef/library/std/_prefix.h>

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_LIB_STD_SGI`]

[@http://www.sgi.com/tech/stl/ SGI] Standard C++ library.
If available version number as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__STL_CONFIG_H`] [__predef_detection__]]

    [[`__SGI_STL`] [V.R.P]]
    ]
 */

#define SPI_BOOST_LIB_STD_SGI SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__STL_CONFIG_H)
#   undef SPI_BOOST_LIB_STD_SGI
#   if defined(__SGI_STL)
#       define SPI_BOOST_LIB_STD_SGI SPI_BOOST_PREDEF_MAKE_0X_VRP(__SGI_STL)
#   else
#       define SPI_BOOST_LIB_STD_SGI SPI_BOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if SPI_BOOST_LIB_STD_SGI
#   define SPI_BOOST_LIB_STD_SGI_AVAILABLE
#endif

#define SPI_BOOST_LIB_STD_SGI_NAME "SGI"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_LIB_STD_SGI,SPI_BOOST_LIB_STD_SGI_NAME)
