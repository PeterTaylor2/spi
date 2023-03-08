/*
Copyright Rene Rivera 2008-2017
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_COMPILER_INTEL_H
#define SPI_BOOST_PREDEF_COMPILER_INTEL_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/make.h>

/*`
[heading `SPI_BOOST_COMP_INTEL`]

[@http://en.wikipedia.org/wiki/Intel_C%2B%2B Intel C/C++] compiler.
Version number available as major, minor, and patch.

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`__INTEL_COMPILER`] [__predef_detection__]]
    [[`__ICL`] [__predef_detection__]]
    [[`__ICC`] [__predef_detection__]]
    [[`__ECC`] [__predef_detection__]]

    [[`__INTEL_COMPILER`] [V.R]]
    [[`__INTEL_COMPILER` and `__INTEL_COMPILER_UPDATE`] [V.R.P]]
    ]
 */

#define SPI_BOOST_COMP_INTEL SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#if defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || \
    defined(__ECC)
/*`
[note Because of an Intel mistake in the release version numbering when
`__INTEL_COMPILER` is `9999` it is detected as version 12.1.0.]
 */
#   if !defined(SPI_BOOST_COMP_INTEL_DETECTION) && defined(__INTEL_COMPILER) && (__INTEL_COMPILER == 9999)
#       define SPI_BOOST_COMP_INTEL_DETECTION SPI_BOOST_VERSION_NUMBER(12,1,0)
#   endif
#   if !defined(SPI_BOOST_COMP_INTEL_DETECTION) && defined(__INTEL_COMPILER) && defined(__INTEL_COMPILER_UPDATE)
#       define SPI_BOOST_COMP_INTEL_DETECTION SPI_BOOST_VERSION_NUMBER( \
            SPI_BOOST_VERSION_NUMBER_MAJOR(SPI_BOOST_PREDEF_MAKE_10_VVRR(__INTEL_COMPILER)), \
            SPI_BOOST_VERSION_NUMBER_MINOR(SPI_BOOST_PREDEF_MAKE_10_VVRR(__INTEL_COMPILER)), \
            __INTEL_COMPILER_UPDATE)
#   endif
#   if !defined(SPI_BOOST_COMP_INTEL_DETECTION) && defined(__INTEL_COMPILER)
#       define SPI_BOOST_COMP_INTEL_DETECTION SPI_BOOST_PREDEF_MAKE_10_VVRR(__INTEL_COMPILER)
#   endif
#   if !defined(SPI_BOOST_COMP_INTEL_DETECTION)
#       define SPI_BOOST_COMP_INTEL_DETECTION SPI_BOOST_VERSION_NUMBER_AVAILABLE
#   endif
#endif

#ifdef SPI_BOOST_COMP_INTEL_DETECTION
#   if defined(SPI_BOOST_PREDEF_DETAIL_COMP_DETECTED)
#       define SPI_BOOST_COMP_INTEL_EMULATED SPI_BOOST_COMP_INTEL_DETECTION
#   else
#       undef SPI_BOOST_COMP_INTEL
#       define SPI_BOOST_COMP_INTEL SPI_BOOST_COMP_INTEL_DETECTION
#   endif
#   define SPI_BOOST_COMP_INTEL_AVAILABLE
#   include <spi_boost/predef/detail/comp_detected.h>
#endif

#define SPI_BOOST_COMP_INTEL_NAME "Intel C/C++"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_INTEL,SPI_BOOST_COMP_INTEL_NAME)

#ifdef SPI_BOOST_COMP_INTEL_EMULATED
#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_COMP_INTEL_EMULATED,SPI_BOOST_COMP_INTEL_NAME)
#endif
