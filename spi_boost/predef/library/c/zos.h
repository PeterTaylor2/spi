/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_LIBRARY_C_ZOS_H
#define SPI_BOOST_PREDEF_LIBRARY_C_ZOS_H

#include <spi_boost/predef/library/c/_prefix.h>

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_LIB_C_ZOS`]

z/OS libc Standard C library.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__LIBREL__`] [__predef_detection__]]

    [[`__LIBREL__`] [V.R.P]]
    [[`__TARGET_LIB__`] [V.R.P]]
    ]
 */

#define SPI_BOOST_LIB_C_ZOS SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__LIBREL__)
#   undef SPI_BOOST_LIB_C_ZOS
#   if !defined(SPI_BOOST_LIB_C_ZOS) && defined(__LIBREL__)
#       define SPI_BOOST_LIB_C_ZOS SPI_BOOST_PREDEF_MAKE_0X_VRRPPPP(__LIBREL__)
#   endif
#   if !defined(SPI_BOOST_LIB_C_ZOS) && defined(__TARGET_LIB__)
#       define SPI_BOOST_LIB_C_ZOS SPI_BOOST_PREDEF_MAKE_0X_VRRPPPP(__TARGET_LIB__)
#   endif
#   if !defined(SPI_BOOST_LIB_C_ZOS)
#       define SPI_BOOST_LIB_C_ZOS SPI_BOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if SPI_BOOST_LIB_C_ZOS
#   define SPI_BOOST_LIB_C_ZOS_AVAILABLE
#endif

#define SPI_BOOST_LIB_C_ZOS_NAME "z/OS"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_LIB_C_ZOS,SPI_BOOST_LIB_C_ZOS_NAME)
