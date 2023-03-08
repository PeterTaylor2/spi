/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_COMPAQ_H
#define SPI_BOOST_PREDEF_COMPILER_COMPAQ_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_DEC`]

[@http://www.openvms.compaq.com/openvms/brochures/deccplus/ Compaq C/C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__DECCXX`] [__predef_detection__]]
    [[`__DECC`] [__predef_detection__]]

    [[`__DECCXX_VER`] [V.R.P]]
    [[`__DECC_VER`] [V.R.P]]
    ]
 */

#define SPI_BOOST_COMP_DEC SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__DECC) || defined(__DECCXX)
#   if !defined(SPI_BOOST_COMP_DEC_DETECTION) && defined(__DECCXX_VER)
#       define SPI_BOOST_COMP_DEC_DETECTION SPI_BOOST_PREDEF_MAKE_10_VVRR0PP00(__DECCXX_VER)
#   endif
#   if !defined(SPI_BOOST_COMP_DEC_DETECTION) && defined(__DECC_VER)
#       define SPI_BOOST_COMP_DEC_DETECTION SPI_BOOST_PREDEF_MAKE_10_VVRR0PP00(__DECC_VER)
#   endif
#   if !defined(SPI_BOOST_COMP_DEC_DETECTION)
#       define SPI_BOOST_COMP_DEC_DETECTION SPI_BOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef SPI_BOOST_COMP_DEC_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_DEC_EMULATED SPI_BOOST_COMP_DEC_DETECTION
#   else
#       undef SPI_BOOST_COMP_DEC
#       define SPI_BOOST_COMP_DEC SPI_BOOST_COMP_DEC_DETECTION
#   endif
#   define SPI_BOOST_COMP_DEC_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_DEC_NAME "Compaq C/C++"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_DEC,SPI_BOOST_COMP_DEC_NAME)

#ifdef SPI_BOOST_COMP_DEC_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_DEC_EMULATED,SPI_BOOST_COMP_DEC_NAME)
#endif
