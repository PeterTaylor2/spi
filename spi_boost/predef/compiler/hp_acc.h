/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_HP_ACC_H
#define SPI_BOOST_PREDEF_COMPILER_HP_ACC_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_HPACC`]

HP aC++ compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__HP_aCC`] [__predef_detection__]]

    [[`__HP_aCC`] [V.R.P]]
    ]
 */

#define SPI_BOOST_COMP_HPACC SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__HP_aCC)
#   if !defined(SPI_BOOST_COMP_HPACC_DETECTION) && (__HP_aCC > 1)
#       define SPI_BOOST_COMP_HPACC_DETECTION SPI_BOOST_PREDEF_MAKE_10_VVRRPP(__HP_aCC)
#   endif
#   if !defined(SPI_BOOST_COMP_HPACC_DETECTION)
#       define SPI_BOOST_COMP_HPACC_DETECTION SPI_BOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef SPI_BOOST_COMP_HPACC_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_HPACC_EMULATED SPI_BOOST_COMP_HPACC_DETECTION
#   else
#       undef SPI_BOOST_COMP_HPACC
#       define SPI_BOOST_COMP_HPACC SPI_BOOST_COMP_HPACC_DETECTION
#   endif
#   define SPI_BOOST_COMP_HPACC_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_HPACC_NAME "HP aC++"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_HPACC,SPI_BOOST_COMP_HPACC_NAME)

#ifdef SPI_BOOST_COMP_HPACC_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_HPACC_EMULATED,SPI_BOOST_COMP_HPACC_NAME)
#endif
