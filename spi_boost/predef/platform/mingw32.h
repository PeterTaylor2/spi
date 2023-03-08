/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_PLAT_MINGW32_H
#define SPI_BOOST_PREDEF_PLAT_MINGW32_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_PLAT_MINGW32`]

[@http://www.mingw.org/ MinGW] platform.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__MINGW32__`] [__predef_detection__]]

    [[`__MINGW32_VERSION_MAJOR`, `__MINGW32_VERSION_MINOR`] [V.R.0]]
    ]
 */

#define SPI_BOOST_PLAT_MINGW32 SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__MINGW32__)
#   include <_mingw.h>
#   if !defined(SPI_BOOST_PLAT_MINGW32_DETECTION) && (defined(__MINGW32_VERSION_MAJOR) && defined(__MINGW32_VERSION_MINOR))
#       define SPI_BOOST_PLAT_MINGW32_DETECTION \
            SPI_BOOST_VERSION_NUMBER(__MINGW32_VERSION_MAJOR,__MINGW32_VERSION_MINOR,0)
#   endif
#   if !defined(SPI_BOOST_PLAT_MINGW32_DETECTION)
#       define SPI_BOOST_PLAT_MINGW32_DETECTION SPI_BOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef SPI_BOOST_PLAT_MINGW32_DETECTION
#   define SPI_BOOST_PLAT_MINGW32_AVAILABLE
#   if defined(SPI_BOOST_PREDEF_DETAIL_PLAT_DETECTED)
#       define SPI_BOOST_PLAT_MINGW32_EMULATED SPI_BOOST_PLAT_MINGW32_DETECTION
#   else
#       undef SPI_BOOST_PLAT_MINGW32
#       define SPI_BOOST_PLAT_MINGW32 SPI_BOOST_PLAT_MINGW32_DETECTION
#   endif
#   include <spi_boost/predef/detail/platform_detected.h>
#endif

#define SPI_BOOST_PLAT_MINGW32_NAME "MinGW"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_PLAT_MINGW32,SPI_BOOST_PLAT_MINGW32_NAME)

#ifdef SPI_BOOST_PLAT_MINGW32_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_PLAT_MINGW32_EMULATED,SPI_BOOST_PLAT_MINGW32_NAME)
#endif
