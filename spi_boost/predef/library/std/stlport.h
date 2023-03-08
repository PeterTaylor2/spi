/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_LIBRARY_STD_STLPORT_H
#define SPI_BOOST_PREDEF_LIBRARY_STD_STLPORT_H

#include <spi_boost/predef/library/std/_prefix.h>

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_LIB_STD_STLPORT`]

[@http://sourceforge.net/projects/stlport/ STLport Standard C++] library.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__SGI_STL_PORT`] [__predef_detection__]]
    [[`_STLPORT_VERSION`] [__predef_detection__]]

    [[`_STLPORT_MAJOR`, `_STLPORT_MINOR`, `_STLPORT_PATCHLEVEL`] [V.R.P]]
    [[`_STLPORT_VERSION`] [V.R.P]]
    [[`__SGI_STL_PORT`] [V.R.P]]
    ]
 */

#define SPI_BOOST_LIB_STD_STLPORT SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)
#   undef SPI_BOOST_LIB_STD_STLPORT
#   if !defined(SPI_BOOST_LIB_STD_STLPORT) && defined(_STLPORT_MAJOR)
#       define SPI_BOOST_LIB_STD_STLPORT \
            SPI_BOOST_VERSION_NUMBER(_STLPORT_MAJOR,_STLPORT_MINOR,_STLPORT_PATCHLEVEL)
#   endif
#   if !defined(SPI_BOOST_LIB_STD_STLPORT) && defined(_STLPORT_VERSION)
#       define SPI_BOOST_LIB_STD_STLPORT SPI_BOOST_PREDEF_MAKE_0X_VRP(_STLPORT_VERSION)
#   endif
#   if !defined(SPI_BOOST_LIB_STD_STLPORT)
#       define SPI_BOOST_LIB_STD_STLPORT SPI_BOOST_PREDEF_MAKE_0X_VRP(__SGI_STL_PORT)
#   endif
#endif

#if SPI_BOOST_LIB_STD_STLPORT
#   define SPI_BOOST_LIB_STD_STLPORT_AVAILABLE
#endif

#define SPI_BOOST_LIB_STD_STLPORT_NAME "STLport"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_LIB_STD_STLPORT,SPI_BOOST_LIB_STD_STLPORT_NAME)
