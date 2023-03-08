/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_LIBRARY_STD_STDCPP3_H
#define SPI_BOOST_PREDEF_LIBRARY_STD_STDCPP3_H

#include <spi_boost/predef/library/std/_prefix.h>

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_LIB_STD_GNU`]

[@http://gcc.gnu.org/libstdc++/ GNU libstdc++] Standard C++ library.
Version number available as year (from 1970), month, and day.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__GLIBCXX__`] [__predef_detection__]]
    [[`__GLIBCPP__`] [__predef_detection__]]

    [[`__GLIBCXX__`] [V.R.P]]
    [[`__GLIBCPP__`] [V.R.P]]
    ]
 */

#define SPI_BOOST_LIB_STD_GNU SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__GLIBCPP__) || defined(__GLIBCXX__)
#   undef SPI_BOOST_LIB_STD_GNU
#   if defined(__GLIBCXX__)
#       define SPI_BOOST_LIB_STD_GNU SPI_BOOST_PREDEF_MAKE_YYYYMMDD(__GLIBCXX__)
#   else
#       define SPI_BOOST_LIB_STD_GNU SPI_BOOST_PREDEF_MAKE_YYYYMMDD(__GLIBCPP__)
#   endif
#endif

#if SPI_BOOST_LIB_STD_GNU
#   define SPI_BOOST_LIB_STD_GNU_AVAILABLE
#endif

#define SPI_BOOST_LIB_STD_GNU_NAME "GNU"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_LIB_STD_GNU,SPI_BOOST_LIB_STD_GNU_NAME)
