/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_GCC_XML_H
#define SPI_BOOST_PREDEF_COMPILER_GCC_XML_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_GCCXML`]

[@http://www.gccxml.org/ GCC XML] compiler.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__GCCXML__`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_COMP_GCCXML SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__GCCXML__)
#   define SPI_BOOST_COMP_GCCXML_DETECTION SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif

#ifdef SPI_BOOST_COMP_GCCXML_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_GCCXML_EMULATED SPI_BOOST_COMP_GCCXML_DETECTION
#   else
#       undef SPI_BOOST_COMP_GCCXML
#       define SPI_BOOST_COMP_GCCXML SPI_BOOST_COMP_GCCXML_DETECTION
#   endif
#   define SPI_BOOST_COMP_GCCXML_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_GCCXML_NAME "GCC XML"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_GCCXML,SPI_BOOST_COMP_GCCXML_NAME)

#ifdef SPI_BOOST_COMP_GCCXML_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_GCCXML_EMULATED,SPI_BOOST_COMP_GCCXML_NAME)
#endif
