/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_PLAT_MINGW64_H
#define SPI_BOOST_PREDEF_PLAT_MINGW64_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_PLAT_MINGW64`]

[@https://mingw-w64.org/ MinGW-w64] platform.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__MINGW64__`] [__predef_detection__]]

    [[`__MINGW64_VERSION_MAJOR`, `__MINGW64_VERSION_MINOR`] [V.R.0]]
    ]
 */

#define SPI_BOOST_PLAT_MINGW64 SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__MINGW64__)
#   include <_mingw.h>
#   if !defined(SPI_BOOST_PLAT_MINGW64_DETECTION) && (defined(__MINGW64_VERSION_MAJOR) && defined(__MINGW64_VERSION_MINOR))
#       define SPI_BOOST_PLAT_MINGW64_DETECTION \
            SPI_BOOST_VERSION_NUMBER(__MINGW64_VERSION_MAJOR,__MINGW64_VERSION_MINOR,0)
#   endif
#   if !defined(SPI_BOOST_PLAT_MINGW64_DETECTION)
#       define SPI_BOOST_PLAT_MINGW64_DETECTION SPI_BOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef SPI_BOOST_PLAT_MINGW64_DETECTION
#   define SPI_BOOST_PLAT_MINGW64_AVAILABLE
#   if defined(SPI_BOOST_PREDEF_DETAIL_PLAT_DETECTED)
#       define SPI_BOOST_PLAT_MINGW64_EMULATED SPI_BOOST_PLAT_MINGW64_DETECTION
#   else
#       undef SPI_BOOST_PLAT_MINGW64
#       define SPI_BOOST_PLAT_MINGW64 SPI_BOOST_PLAT_MINGW64_DETECTION
#   endif
#   include <spi_boost/predef/detail/platform_detected.h>
#endif

#define SPI_BOOST_PLAT_MINGW64_NAME "MinGW-w64"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_PLAT_MINGW64,SPI_BOOST_PLAT_MINGW64_NAME)

#ifdef SPI_BOOST_PLAT_MINGW64_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_PLAT_MINGW64_EMULATED,SPI_BOOST_PLAT_MINGW64_NAME)
#endif
