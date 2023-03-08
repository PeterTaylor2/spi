/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_OS_UNIX_H
#define SPI_BOOST_PREDEF_OS_UNIX_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_OS_UNIX`]

[@http://en.wikipedia.org/wiki/Unix Unix Environment] operating system.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`unix`] [__predef_detection__]]
    [[`__unix`] [__predef_detection__]]
    [[`_XOPEN_SOURCE`] [__predef_detection__]]
    [[`_POSIX_SOURCE`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_OS_UNIX SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(unix) || defined(__unix) || \
    defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
#   undef SPI_BOOST_OS_UNIX
#   define SPI_BOOST_OS_UNIX SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif

#if SPI_BOOST_OS_UNIX
#   define SPI_BOOST_OS_UNIX_AVAILABLE
#endif

#define SPI_BOOST_OS_UNIX_NAME "Unix Environment"

/*`
[heading `SPI_BOOST_OS_SVR4`]

[@http://en.wikipedia.org/wiki/UNIX_System_V SVR4 Environment] operating system.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__sysv__`] [__predef_detection__]]
    [[`__SVR4`] [__predef_detection__]]
    [[`__svr4__`] [__predef_detection__]]
    [[`_SYSTYPE_SVR4`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_OS_SVR4 SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__sysv__) || defined(__SVR4) || \
    defined(__svr4__) || defined(_SYSTYPE_SVR4)
#   undef SPI_BOOST_OS_SVR4
#   define SPI_BOOST_OS_SVR4 SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif

#if SPI_BOOST_OS_SVR4
#   define SPI_BOOST_OS_SVR4_AVAILABLE
#endif

#define SPI_BOOST_OS_SVR4_NAME "SVR4 Environment"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_OS_UNIX,SPI_BOOST_OS_UNIX_NAME)
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_OS_SVR4,SPI_BOOST_OS_SVR4_NAME)
