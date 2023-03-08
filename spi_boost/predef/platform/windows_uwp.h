/*
Copyright James E. King III, 2017
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_PLAT_WINDOWS_UWP_H
#define SPI_BOOST_PREDEF_PLAT_WINDOWS_UWP_H

#include <spi_boost/predef/make.h>
#include <spi_boost/predef/os/windows.h>
#include <spi_boost/predef/version_number.h>

/*`
[heading `SPI_BOOST_PLAT_WINDOWS_UWP`]

[@http://docs.microsoft.com/windows/uwp/ Universal Windows Platform]
is available if the current development environment is capable of targeting 
UWP development.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__MINGW64_VERSION_MAJOR` from `_mingw.h`] [`>= 3`]]
    [[`VER_PRODUCTBUILD` from `ntverp.h`] [`>= 9200`]]
]
*/

#define SPI_BOOST_PLAT_WINDOWS_UWP SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE
#define SPI_BOOST_PLAT_WINDOWS_SDK_VERSION SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if SPI_BOOST_OS_WINDOWS
//  MinGW (32-bit) has no ntverp.h header
#if !defined(__MINGW32__)
#   include <ntverp.h>
#   undef SPI_BOOST_PLAT_WINDOWS_SDK_VERSION
#   define SPI_BOOST_PLAT_WINDOWS_SDK_VERSION SPI_BOOST_VERSION_NUMBER(0, 0, VER_PRODUCTBUILD)
#endif

// 9200 is Windows SDK 8.0 from ntverp.h which introduced family support
#if ((SPI_BOOST_PLAT_WINDOWS_SDK_VERSION >= SPI_BOOST_VERSION_NUMBER(0, 0, 9200)) || \
     (defined(__MINGW64__) && __MINGW64_VERSION_MAJOR >= 3))
#   undef SPI_BOOST_PLAT_WINDOWS_UWP
#   define SPI_BOOST_PLAT_WINDOWS_UWP SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif
#endif

#if SPI_BOOST_PLAT_WINDOWS_UWP
#   define SPI_BOOST_PLAT_WINDOWS_UWP_AVAILABLE
#   include <spi_boost/predef/detail/platform_detected.h>
#   include <winapifamily.h>    // Windows SDK
#endif

#define SPI_BOOST_PLAT_WINDOWS_UWP_NAME "Universal Windows Platform"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_PLAT_WINDOWS_UWP, SPI_BOOST_PLAT_WINDOWS_UWP_NAME)
