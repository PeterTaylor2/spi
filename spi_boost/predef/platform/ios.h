/*
Copyright Ruslan Baratov 2017
Copyright Rene Rivera 2017
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_PLAT_IOS_H
#define SPI_BOOST_PREDEF_PLAT_IOS_H

#include <spi_boost/predef/os/ios.h> // SPI_BOOST_OS_IOS
#include <spi_boost/predef/version_number.h> // SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

/*`
[heading `SPI_BOOST_PLAT_IOS_DEVICE`]
[heading `SPI_BOOST_PLAT_IOS_SIMULATOR`]

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`TARGET_IPHONE_SIMULATOR`] [__predef_detection__]]
    [[`TARGET_OS_SIMULATOR`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_PLAT_IOS_DEVICE SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE
#define SPI_BOOST_PLAT_IOS_SIMULATOR SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

// https://opensource.apple.com/source/CarbonHeaders/CarbonHeaders-18.1/TargetConditionals.h
#if SPI_BOOST_OS_IOS
#    include <TargetConditionals.h>
#    if defined(TARGET_OS_SIMULATOR) && (TARGET_OS_SIMULATOR == 1)
#        undef SPI_BOOST_PLAT_IOS_SIMULATOR
#        define SPI_BOOST_PLAT_IOS_SIMULATOR SPI_BOOST_VERSION_NUMBER_AVAILABLE
#    elif defined(TARGET_IPHONE_SIMULATOR) && (TARGET_IPHONE_SIMULATOR == 1)
#        undef SPI_BOOST_PLAT_IOS_SIMULATOR
#        define SPI_BOOST_PLAT_IOS_SIMULATOR SPI_BOOST_VERSION_NUMBER_AVAILABLE
#    else
#        undef SPI_BOOST_PLAT_IOS_DEVICE
#        define SPI_BOOST_PLAT_IOS_DEVICE SPI_BOOST_VERSION_NUMBER_AVAILABLE
#    endif
#endif

#if SPI_BOOST_PLAT_IOS_SIMULATOR
#    define SPI_BOOST_PLAT_IOS_SIMULATOR_AVAILABLE
#    include <spi_boost/predef/detail/platform_detected.h>
#endif

#if SPI_BOOST_PLAT_IOS_DEVICE
#    define SPI_BOOST_PLAT_IOS_DEVICE_AVAILABLE
#    include <spi_boost/predef/detail/platform_detected.h>
#endif

#define SPI_BOOST_PLAT_IOS_SIMULATOR_NAME "iOS Simulator"
#define SPI_BOOST_PLAT_IOS_DEVICE_NAME "iOS Device"

#endif // SPI_BOOST_PREDEF_PLAT_IOS_H

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_PLAT_IOS_SIMULATOR,SPI_BOOST_PLAT_IOS_SIMULATOR_NAME)
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_PLAT_IOS_DEVICE,SPI_BOOST_PLAT_IOS_DEVICE_NAME)
