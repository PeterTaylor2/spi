/*
Copyright Rene Rivera 2011-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_OS_VMS_H
#define SPI_BOOST_PREDEF_OS_VMS_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_OS_VMS`]

[@http://en.wikipedia.org/wiki/Vms VMS] operating system.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`VMS`] [__predef_detection__]]
    [[`__VMS`] [__predef_detection__]]

    [[`__VMS_VER`] [V.R.P]]
    ]
 */

#define SPI_BOOST_OS_VMS SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(SPI_BOOST_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(VMS) || defined(__VMS) \
    )
#   undef SPI_BOOST_OS_VMS
#   if defined(__VMS_VER)
#       define SPI_BOOST_OS_VMS SPI_BOOST_PREDEF_MAKE_10_VVRR00PP00(__VMS_VER)
#   else
#       define SPI_BOOST_OS_VMS SPI_BOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if SPI_BOOST_OS_VMS
#   define SPI_BOOST_OS_VMS_AVAILABLE
#   include <spi_boost/predef/detail/os_detected.h>
#endif

#define SPI_BOOST_OS_VMS_NAME "VMS"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_OS_VMS,SPI_BOOST_OS_VMS_NAME)
