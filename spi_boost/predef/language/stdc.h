/*
Copyright Rene Rivera 2011-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_LANGUAGE_STDC_H
#define SPI_BOOST_PREDEF_LANGUAGE_STDC_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_LANG_STDC`]

[@http://en.wikipedia.org/wiki/C_(programming_language) Standard C] language.
If available, the year of the standard is detected as YYYY.MM.1 from the Epoc date.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__STDC__`] [__predef_detection__]]

    [[`__STDC_VERSION__`] [V.R.P]]
    ]
 */

#define SPI_BOOST_LANG_STDC SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__STDC__)
#   undef SPI_BOOST_LANG_STDC
#   if defined(__STDC_VERSION__)
#       if (__STDC_VERSION__ > 100)
#           define SPI_BOOST_LANG_STDC SPI_BOOST_PREDEF_MAKE_YYYYMM(__STDC_VERSION__)
#       else
#           define SPI_BOOST_LANG_STDC SPI_BOOST_VERSION_NUMBER_AVAILABLE
#       endif
#   else
#       define SPI_BOOST_LANG_STDC SPI_BOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#if SPI_BOOST_LANG_STDC
#   define SPI_BOOST_LANG_STDC_AVAILABLE
#endif

#define SPI_BOOST_LANG_STDC_NAME "Standard C"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_LANG_STDC,SPI_BOOST_LANG_STDC_NAME)
