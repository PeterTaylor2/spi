/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_MICROTEC_H
#define SPI_BOOST_PREDEF_COMPILER_MICROTEC_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_MRI`]

[@http://www.mentor.com/microtec/ Microtec C/C++] compiler.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`_MRI`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_COMP_MRI SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(_MRI)
#   define SPI_BOOST_COMP_MRI_DETECTION SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif

#ifdef SPI_BOOST_COMP_MRI_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_MRI_EMULATED SPI_BOOST_COMP_MRI_DETECTION
#   else
#       undef SPI_BOOST_COMP_MRI
#       define SPI_BOOST_COMP_MRI SPI_BOOST_COMP_MRI_DETECTION
#   endif
#   define SPI_BOOST_COMP_MRI_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_MRI_NAME "Microtec C/C++"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_MRI,SPI_BOOST_COMP_MRI_NAME)

#ifdef SPI_BOOST_COMP_MRI_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_MRI_EMULATED,SPI_BOOST_COMP_MRI_NAME)
#endif
