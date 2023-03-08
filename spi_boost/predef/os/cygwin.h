/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_OS_CYGWIN_H
#define SPI_BOOST_PREDEF_OS_CYGWIN_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_OS_CYGWIN`]

[@http://en.wikipedia.org/wiki/Cygwin Cygwin] evironment.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__CYGWIN__`] [__predef_detection__]]

    [[`CYGWIN_VERSION_API_MAJOR`, `CYGWIN_VERSION_API_MINOR`] [V.R.0]]
    ]
 */

#define SPI_BOOST_OS_CYGWIN SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(SPI_BOOST_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(__CYGWIN__) \
    )
#   include <cygwin/version.h>
#   undef SPI_BOOST_OS_CYGWIN
#   define SPI_BOOST_OS_CYGWIN \
        SPI_BOOST_VERSION_NUMBER(CYGWIN_VERSION_API_MAJOR,\
                             CYGWIN_VERSION_API_MINOR, 0)
#endif

#if SPI_BOOST_OS_CYGWIN
#   define SPI_BOOST_OS_CYGWIN_AVAILABLE
#   include <spi_boost/predef/detail/os_detected.h>
#endif

#define SPI_BOOST_OS_CYGWIN_NAME "Cygwin"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_OS_CYGWIN,SPI_BOOST_OS_CYGWIN_NAME)
