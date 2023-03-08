/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_EKOPATH_H
#define SPI_BOOST_PREDEF_COMPILER_EKOPATH_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_PATH`]

[@http://en.wikipedia.org/wiki/PathScale EKOpath] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__PATHCC__`] [__predef_detection__]]

    [[`__PATHCC__`, `__PATHCC_MINOR__`, `__PATHCC_PATCHLEVEL__`] [V.R.P]]
    ]
 */

#define SPI_BOOST_COMP_PATH SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__PATHCC__)
#   define SPI_BOOST_COMP_PATH_DETECTION \
        SPI_BOOST_VERSION_NUMBER(__PATHCC__,__PATHCC_MINOR__,__PATHCC_PATCHLEVEL__)
#endif

#ifdef SPI_BOOST_COMP_PATH_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_PATH_EMULATED SPI_BOOST_COMP_PATH_DETECTION
#   else
#       undef SPI_BOOST_COMP_PATH
#       define SPI_BOOST_COMP_PATH SPI_BOOST_COMP_PATH_DETECTION
#   endif
#   define SPI_BOOST_COMP_PATH_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_PATH_NAME "EKOpath"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_PATH,SPI_BOOST_COMP_PATH_NAME)

#ifdef SPI_BOOST_COMP_PATH_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_PATH_EMULATED,SPI_BOOST_COMP_PATH_NAME)
#endif
