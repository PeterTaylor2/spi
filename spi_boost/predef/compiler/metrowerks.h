/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_METROWERKS_H
#define SPI_BOOST_PREDEF_COMPILER_METROWERKS_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_MWERKS`]

[@http://en.wikipedia.org/wiki/CodeWarrior Metrowerks CodeWarrior] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__MWERKS__`] [__predef_detection__]]
    [[`__CWCC__`] [__predef_detection__]]

    [[`__CWCC__`] [V.R.P]]
    [[`__MWERKS__`] [V.R.P >= 4.2.0]]
    [[`__MWERKS__`] [9.R.0]]
    [[`__MWERKS__`] [8.R.0]]
    ]
 */

#define SPI_BOOST_COMP_MWERKS SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__MWERKS__) || defined(__CWCC__)
#   if !defined(SPI_BOOST_COMP_MWERKS_DETECTION) && defined(__CWCC__)
#       define SPI_BOOST_COMP_MWERKS_DETECTION SPI_BOOST_PREDEF_MAKE_0X_VRPP(__CWCC__)
#   endif
#   if !defined(SPI_BOOST_COMP_MWERKS_DETECTION) && (__MWERKS__ >= 0x4200)
#       define SPI_BOOST_COMP_MWERKS_DETECTION SPI_BOOST_PREDEF_MAKE_0X_VRPP(__MWERKS__)
#   endif
#   if !defined(SPI_BOOST_COMP_MWERKS_DETECTION) && (__MWERKS__ >= 0x3204) // note the "skip": 04->9.3
#       define SPI_BOOST_COMP_MWERKS_DETECTION SPI_BOOST_VERSION_NUMBER(9,(__MWERKS__)%100-1,0)
#   endif
#   if !defined(SPI_BOOST_COMP_MWERKS_DETECTION) && (__MWERKS__ >= 0x3200)
#       define SPI_BOOST_COMP_MWERKS_DETECTION SPI_BOOST_VERSION_NUMBER(9,(__MWERKS__)%100,0)
#   endif
#   if !defined(SPI_BOOST_COMP_MWERKS_DETECTION) && (__MWERKS__ >= 0x3000)
#       define SPI_BOOST_COMP_MWERKS_DETECTION SPI_BOOST_VERSION_NUMBER(8,(__MWERKS__)%100,0)
#   endif
#   if !defined(SPI_BOOST_COMP_MWERKS_DETECTION)
#       define SPI_BOOST_COMP_MWERKS_DETECTION SPI_BOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef SPI_BOOST_COMP_MWERKS_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_MWERKS_EMULATED SPI_BOOST_COMP_MWERKS_DETECTION
#   else
#       undef SPI_BOOST_COMP_MWERKS
#       define SPI_BOOST_COMP_MWERKS SPI_BOOST_COMP_MWERKS_DETECTION
#   endif
#   define SPI_BOOST_COMP_MWERKS_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_MWERKS_NAME "Metrowerks CodeWarrior"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_MWERKS,SPI_BOOST_COMP_MWERKS_NAME)

#ifdef SPI_BOOST_COMP_MWERKS_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_MWERKS_EMULATED,SPI_BOOST_COMP_MWERKS_NAME)
#endif
