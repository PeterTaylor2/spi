/*
Copyright Rene Rivera 2008-2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#include <spi_boost/predef/architecture/x86/32.h>
#include <spi_boost/predef/architecture/x86/64.h>

#ifndef SPI_BOOST_PREDEF_ARCHITECTURE_X86_H
#define SPI_BOOST_PREDEF_ARCHITECTURE_X86_H

/*`
[heading `SPI_BOOST_ARCH_X86`]

[@http://en.wikipedia.org/wiki/X86 Intel x86] architecture. This is
a category to indicate that either `SPI_BOOST_ARCH_X86_32` or
`SPI_BOOST_ARCH_X86_64` is detected.
 */

#define SPI_BOOST_ARCH_X86 SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if SPI_BOOST_ARCH_X86_32 || SPI_BOOST_ARCH_X86_64
#   undef SPI_BOOST_ARCH_X86
#   define SPI_BOOST_ARCH_X86 SPI_BOOST_VERSION_NUMBER_AVAILABLE
#endif

#if SPI_BOOST_ARCH_X86
#   define SPI_BOOST_ARCH_X86_AVAILABLE
#endif

#define SPI_BOOST_ARCH_X86_NAME "Intel x86"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_ARCH_X86,SPI_BOOST_ARCH_X86_NAME)
