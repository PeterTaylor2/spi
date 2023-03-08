/*
Copyright Charly Chevalier 2015
Copyright Joel Falcou 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_HARDWARE_SIMD_PPC_H
#define SPI_BOOST_PREDEF_HARDWARE_SIMD_PPC_H

#include <spi_boost/predef/version_number.h>
#include <spi_boost/predef/hardware/simd/ppc/versions.h>

/*`
 [heading `SPI_BOOST_HW_SIMD_PPC`]

 The SIMD extension for PowerPC (*if detected*).
 Version number depends on the most recent detected extension.

 [table
     [[__predef_symbol__] [__predef_version__]]

     [[`__VECTOR4DOUBLE__`] [__predef_detection__]]

     [[`__ALTIVEC__`] [__predef_detection__]]
     [[`__VEC__`] [__predef_detection__]]

     [[`__VSX__`] [__predef_detection__]]
     ]

 [table
     [[__predef_symbol__] [__predef_version__]]

     [[`__VECTOR4DOUBLE__`] [SPI_BOOST_HW_SIMD_PPC_QPX_VERSION]]

     [[`__ALTIVEC__`] [SPI_BOOST_HW_SIMD_PPC_VMX_VERSION]]
     [[`__VEC__`] [SPI_BOOST_HW_SIMD_PPC_VMX_VERSION]]

     [[`__VSX__`] [SPI_BOOST_HW_SIMD_PPC_VSX_VERSION]]
     ]

 */

#define SPI_BOOST_HW_SIMD_PPC SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE

#undef SPI_BOOST_HW_SIMD_PPC
#if !defined(SPI_BOOST_HW_SIMD_PPC) && defined(__VECTOR4DOUBLE__)
#   define SPI_BOOST_HW_SIMD_PPC SPI_BOOST_HW_SIMD_PPC_QPX_VERSION
#endif
#if !defined(SPI_BOOST_HW_SIMD_PPC) && defined(__VSX__)
#   define SPI_BOOST_HW_SIMD_PPC SPI_BOOST_HW_SIMD_PPC_VSX_VERSION
#endif
#if !defined(SPI_BOOST_HW_SIMD_PPC) && (defined(__ALTIVEC__) || defined(__VEC__))
#   define SPI_BOOST_HW_SIMD_PPC SPI_BOOST_HW_SIMD_PPC_VMX_VERSION
#endif

#if !defined(SPI_BOOST_HW_SIMD_PPC)
#   define SPI_BOOST_HW_SIMD_PPC SPI_BOOST_VERSION_NUMBER_NOT_AVAILABLE
#else
#   define SPI_BOOST_HW_SIMD_PPC_AVAILABLE
#endif

#define SPI_BOOST_HW_SIMD_PPC_NAME "PPC SIMD"

#endif

#include <spi_boost/predef/detail/test.h>
SPI_BOOST_PREDEF_DECLARE_TEST(SPI_BOOST_HW_SIMD_PPC, SPI_BOOST_HW_SIMD_PPC_NAME)
