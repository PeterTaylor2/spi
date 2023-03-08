/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_GREENHILLS_H
#define SPI_BOOST_PREDEF_COMPILER_GREENHILLS_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_GHS`]

[@http://en.wikipedia.org/wiki/Green_Hills_Software Green Hills C/C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__ghs`] [__predef_detection__]]
    [[`__ghs__`] [__predef_detection__]]

    [[`__GHS_VERSION_NUMBER__`] [V.R.P]]
    [[`__ghs`] [V.R.P]]
    ]
 */

#define SPI_BOOST_COMP_GHS SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__ghs) || defined(__ghs__)
#   if !defined(SPI_BOOST_COMP_GHS_DETECTION) && defined(__GHS_VERSION_NUMBER__)
#       define SPI_BOOST_COMP_GHS_DETECTION SPI_BOOST_PREDEF_MAKE_10_VRP(__GHS_VERSION_NUMBER__)
#   endif
#   if !defined(SPI_BOOST_COMP_GHS_DETECTION) && defined(__ghs)
#       define SPI_BOOST_COMP_GHS_DETECTION SPI_BOOST_PREDEF_MAKE_10_VRP(__ghs)
#   endif
#   if !defined(SPI_BOOST_COMP_GHS_DETECTION)
#       define SPI_BOOST_COMP_GHS_DETECTION SPI_BOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef SPI_BOOST_COMP_GHS_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_GHS_EMULATED SPI_BOOST_COMP_GHS_DETECTION
#   else
#       undef SPI_BOOST_COMP_GHS
#       define SPI_BOOST_COMP_GHS SPI_BOOST_COMP_GHS_DETECTION
#   endif
#   define SPI_BOOST_COMP_GHS_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_GHS_NAME "Green Hills C/C++"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_GHS,SPI_BOOST_COMP_GHS_NAME)

#ifdef SPI_BOOST_COMP_GHS_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_GHS_EMULATED,SPI_BOOST_COMP_GHS_NAME)
#endif
