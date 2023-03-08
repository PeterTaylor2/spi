/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_LIBRARY_STD_MODENA_H
#define SPI_BOOST_PREDEF_LIBRARY_STD_MODENA_H

#include <spi_boost/predef/library/std/_prefix.h>

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_LIB_STD_MSIPL`]

[@http://modena.us/ Modena Software Lib++] Standard C++ Library.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`MSIPL_COMPILE_H`] [__predef_detection__]]
    [[`__MSIPL_COMPILE_H`] [__predef_detection__]]
    ]
 */

#define SPI_BOOST_LIB_STD_MSIPL SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(MSIPL_COMPILE_H) || defined(__MSIPL_COMPILE_H)
#   undef SPI_BOOST_LIB_STD_MSIPL
#   define SPI_BOOST_LIB_STD_MSIPL SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif

#if SPI_BOOST_LIB_STD_MSIPL
#   define SPI_BOOST_LIB_STD_MSIPL_AVAILABLE
#endif

#define SPI_BOOST_LIB_STD_MSIPL_NAME "Modena Software Lib++"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_LIB_STD_MSIPL,SPI_BOOST_LIB_STD_MSIPL_NAME)
