/*
Copyright Rene Rivera 2011-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_LANGUAGE_OBJC_H
#define SPI_BOOST_PREDEF_LANGUAGE_OBJC_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_LANG_OBJC`]

[@http://en.wikipedia.org/wiki/Objective-C Objective-C] language.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__OBJC__`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_LANG_OBJC SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__OBJC__)
#   undef SPI_BOOST_LANG_OBJC
#   define SPI_BOOST_LANG_OBJC SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif

#if SPI_BOOST_LANG_OBJC
#   define SPI_BOOST_LANG_OBJC_AVAILABLE
#endif

#define SPI_BOOST_LANG_OBJC_NAME "Objective-C"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_LANG_OBJC,SPI_BOOST_LANG_OBJC_NAME)
