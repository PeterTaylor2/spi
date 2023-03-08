/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_LIBRARY_STD_VACPP_H
#define SPI_BOOST_PREDEF_LIBRARY_STD_VACPP_H

#include <spi_boost/predef/library/std/_prefix.h>

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_LIB_STD_IBM`]

[@http://www.ibm.com/software/awdtools/xlcpp/ IBM VACPP Standard C++] library.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__IBMCPP__`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_LIB_STD_IBM SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__IBMCPP__)
#   undef SPI_BOOST_LIB_STD_IBM
#   define SPI_BOOST_LIB_STD_IBM SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif

#if SPI_BOOST_LIB_STD_IBM
#   define SPI_BOOST_LIB_STD_IBM_AVAILABLE
#endif

#define SPI_BOOST_LIB_STD_IBM_NAME "IBM VACPP"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_LIB_STD_IBM,SPI_BOOST_LIB_STD_IBM_NAME)
