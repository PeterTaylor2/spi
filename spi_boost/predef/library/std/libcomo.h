/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_LIBRARY_STD_LIBCOMO_H
#define SPI_BOOST_PREDEF_LIBRARY_STD_LIBCOMO_H

#include <spi_boost/predef/library/std/_prefix.h>

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_LIB_STD_COMO`]

[@http://www.comeaucomputing.com/libcomo/ Comeau Computing] Standard C++ Library.
Version number available as major.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__LIBCOMO__`] [__predef_detection__]]

    [[`__LIBCOMO_VERSION__`] [V.0.0]]
    ]
 */

#define SPI_BOOST_LIB_STD_COMO SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__LIBCOMO__)
#   undef SPI_BOOST_LIB_STD_COMO
#   define SPI_BOOST_LIB_STD_COMO SPI_BOOST_VERSION_NUMBER(__LIBCOMO_VERSION__,0,0)
#endif

#if SPI_BOOST_LIB_STD_COMO
#   define SPI_BOOST_LIB_STD_COMO_AVAILABLE
#endif

#define SPI_BOOST_LIB_STD_COMO_NAME "Comeau Computing"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_LIB_STD_COMO,SPI_BOOST_LIB_STD_COMO_NAME)
