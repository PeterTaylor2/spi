/*
Copyright Charly Chevalier 2015
Copyright Joel Falcou 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef SPI_BOOST_PREDEF_HARDWARE_SIMD_ARM_VERSIONS_H
#define SPI_BOOST_PREDEF_HARDWARE_SIMD_ARM_VERSIONS_H

#include <spi_boost/predef/version_number.h>

/*`
 Those defines represent ARM SIMD extensions versions.

 [note You *MUST* compare them with the predef `SPI_BOOST_HW_SIMD_ARM`.]
 */

// ---------------------------------

/*`
 [heading `SPI_BOOST_HW_SIMD_ARM_NEON_VERSION`]

 The [@https://en.wikipedia.org/wiki/ARM_architecture#Advanced_SIMD_.28NEON.29 NEON]
 ARM extension version number.

 Version number is: *1.0.0*.
 */
#define SPI_BOOST_HW_SIMD_ARM_NEON_VERSION SPI_BOOST_VERSION_NUMBER(1, 0, 0)

#endif
