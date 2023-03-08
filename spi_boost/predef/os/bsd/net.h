/*
Copyright Rene Rivera 2012-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_OS_BSD_NET_H
#define SPI_BOOST_PREDEF_OS_BSD_NET_H

#include <spi_boost/predef/os/bsd.h>

/*`
[heading `SPI_BOOST_OS_BSD_NET`]

[@http://en.wikipedia.org/wiki/Netbsd NetBSD] operating system.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__NETBSD__`] [__predef_detection__]]
    [[`__NetBSD__`] [__predef_detection__]]

    [[`__NETBSD_version`] [V.R.P]]
    [[`NetBSD0_8`] [0.8.0]]
    [[`NetBSD0_9`] [0.9.0]]
    [[`NetBSD1_0`] [1.0.0]]
    [[`__NetBSD_Version`] [V.R.P]]
    ]
 */

#define SPI_BOOST_OS_BSD_NET SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if !defined(SPI_BOOST_PREDEF_DETAIL_OS_DETECTED) && ( \
    defined(__NETBSD__) || defined(__NetBSD__) \
    )
#   ifndef SPI_BOOST_OS_BSD_AVAILABLE
#       define SPI_BOOST_OS_BSD SPI_BOOST_VERSION_NUMBER_AVAILABLE
#       define SPI_BOOST_OS_BSD_AVAILABLE
#   endif
#   undef SPI_BOOST_OS_BSD_NET
#   if defined(__NETBSD__)
#       if defined(__NETBSD_version)
#           if __NETBSD_version < 500000
#               define SPI_BOOST_OS_BSD_NET \
                    SPI_BOOST_PREDEF_MAKE_10_VRP000(__NETBSD_version)
#           else
#               define SPI_BOOST_OS_BSD_NET \
                    SPI_BOOST_PREDEF_MAKE_10_VRR000(__NETBSD_version)
#           endif
#       else
#           define SPI_BOOST_OS_BSD_NET SPI_BOOST_VERSION_NUMBER_AVAILABLE
#       endif
#   elif defined(__NetBSD__)
#       if !defined(SPI_BOOST_OS_BSD_NET) && defined(NetBSD0_8)
#           define SPI_BOOST_OS_BSD_NET SPI_BOOST_VERSION_NUMBER(0,8,0)
#       endif
#       if !defined(SPI_BOOST_OS_BSD_NET) && defined(NetBSD0_9)
#           define SPI_BOOST_OS_BSD_NET SPI_BOOST_VERSION_NUMBER(0,9,0)
#       endif
#       if !defined(SPI_BOOST_OS_BSD_NET) && defined(NetBSD1_0)
#           define SPI_BOOST_OS_BSD_NET SPI_BOOST_VERSION_NUMBER(1,0,0)
#       endif
#       if !defined(SPI_BOOST_OS_BSD_NET) && defined(__NetBSD_Version)
#           define SPI_BOOST_OS_BSD_NET \
                SPI_BOOST_PREDEF_MAKE_10_VVRR00PP00(__NetBSD_Version)
#       endif
#       if !defined(SPI_BOOST_OS_BSD_NET)
#           define SPI_BOOST_OS_BSD_NET SPI_BOOST_VERSION_NUMBER_AVAILABLE
#       endif
#   endif
#endif

#if SPI_BOOST_OS_BSD_NET
#   define SPI_BOOST_OS_BSD_NET_AVAILABLE
#   include <spi_boost/predef/detail/os_detected.h>
#endif

#define SPI_BOOST_OS_BSD_NET_NAME "NetBSD"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_OS_BSD_NET,SPI_BOOST_OS_BSD_NET_NAME)
