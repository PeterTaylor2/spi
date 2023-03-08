/*
Copyright Rene Rivera 2012-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_OS_BSD_FREE_H
#define SPI_BOOST_PREDEF_OS_BSD_FREE_H

#include <spi_boost/predef/os/bsd.h>

/*`
[heading `SPI_BOOST_OS_BSD_FREE`]

[@http://en.wikipedia.org/wiki/Freebsd FreeBSD] operating system.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__FreeBSD__`] [__predef_detection__]]

    [[`__FreeBSD_version`] [V.R.P]]
    ]
 */

#define SPI_BOOST_OS_BSD_FREE SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(SPI_BOOST_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(__FreeBSD__) \
    )
#   ifndef SPI_BOOST_OS_BSD_AVAILABLE
#       define SPI_BOOST_OS_BSD SPI_BOOST_VERSION_NUMBER_AVAILABLE
#       define SPI_BOOST_OS_BSD_AVAILABLE
#   endif
#   undef SPI_BOOST_OS_BSD_FREE
#   include <sys/param.h>
#   if defined(__FreeBSD_version)
#       if __FreeBSD_version == 491000
#           define SPI_BOOST_OS_BSD_FREE \
                SPI_BOOST_VERSION_NUMBER(4, 10, 0)
#       elif __FreeBSD_version == 492000
#           define SPI_BOOST_OS_BSD_FREE \
                SPI_BOOST_VERSION_NUMBER(4, 11, 0)
#       elif __FreeBSD_version < 500000
#           define SPI_BOOST_OS_BSD_FREE \
                SPI_BOOST_PREDEF_MAKE_10_VRPPPP(__FreeBSD_version)
#       else
#           define SPI_BOOST_OS_BSD_FREE \
                SPI_BOOST_PREDEF_MAKE_10_VVRRPPP(__FreeBSD_version)
#       endif
#   else
#       define SPI_BOOST_OS_BSD_FREE SPI_BOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if SPI_BOOST_OS_BSD_FREE
#   define SPI_BOOST_OS_BSD_FREE_AVAILABLE
#   include <spi_boost/predef/detail/os_detected.h>
#endif

#define SPI_BOOST_OS_BSD_FREE_NAME "Free BSD"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_OS_BSD_FREE,SPI_BOOST_OS_BSD_FREE_NAME)
