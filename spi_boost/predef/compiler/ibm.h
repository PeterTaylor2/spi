/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_IBM_H
#define SPI_BOOST_PREDEF_COMPILER_IBM_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_IBM`]

[@http://en.wikipedia.org/wiki/VisualAge IBM XL C/C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__IBMCPP__`] [__predef_detection__]]
    [[`__xlC__`] [__predef_detection__]]
    [[`__xlc__`] [__predef_detection__]]

    [[`__COMPILER_VER__`] [V.R.P]]
    [[`__xlC__`] [V.R.P]]
    [[`__xlc__`] [V.R.P]]
    [[`__IBMCPP__`] [V.R.P]]
    ]
 */

#define SPI_BOOST_COMP_IBM SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__IBMCPP__) || defined(__xlC__) || defined(__xlc__)
#   if !defined(SPI_BOOST_COMP_IBM_DETECTION) && defined(__COMPILER_VER__)
#       define SPI_BOOST_COMP_IBM_DETECTION SPI_BOOST_PREDEF_MAKE_0X_VRRPPPP(__COMPILER_VER__)
#   endif
#   if !defined(SPI_BOOST_COMP_IBM_DETECTION) && defined(__xlC__)
#       define SPI_BOOST_COMP_IBM_DETECTION SPI_BOOST_PREDEF_MAKE_0X_VVRR(__xlC__)
#   endif
#   if !defined(SPI_BOOST_COMP_IBM_DETECTION) && defined(__xlc__)
#       define SPI_BOOST_COMP_IBM_DETECTION SPI_BOOST_PREDEF_MAKE_0X_VVRR(__xlc__)
#   endif
#   if !defined(SPI_BOOST_COMP_IBM_DETECTION)
#       define SPI_BOOST_COMP_IBM_DETECTION SPI_BOOST_PREDEF_MAKE_10_VRP(__IBMCPP__)
#   endif
#endif

#ifdef SPI_BOOST_COMP_IBM_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_IBM_EMULATED SPI_BOOST_COMP_IBM_DETECTION
#   else
#       undef SPI_BOOST_COMP_IBM
#       define SPI_BOOST_COMP_IBM SPI_BOOST_COMP_IBM_DETECTION
#   endif
#   define SPI_BOOST_COMP_IBM_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_IBM_NAME "IBM XL C/C++"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_IBM,SPI_BOOST_COMP_IBM_NAME)

#ifdef SPI_BOOST_COMP_IBM_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_IBM_EMULATED,SPI_BOOST_COMP_IBM_NAME)
#endif
