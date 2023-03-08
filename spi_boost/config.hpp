//  Boost config.hpp configuration header file  ------------------------------//

//  (C) Copyright John Maddock 2002.
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/config for most recent version.

//  Boost config.hpp policy and rationale documentation has been moved to
//  http://www.boost.org/libs/config
//
//  CAUTION: This file is intended to be completely stable -
//           DO NOT MODIFY THIS FILE!
//

#ifndef SPI_BOOST_CONFIG_HPP
#define SPI_BOOST_CONFIG_HPP

// if we don't have a user config, then use the default location:
#if !defined(SPI_BOOST_USER_CONFIG) && !defined(SPI_BOOST_NO_USER_CONFIG)
#  define SPI_BOOST_USER_CONFIG <spi_boost/config/user.hpp>
#if 0
// For dependency trackers:
#  include <spi_boost/config/user.hpp>
#endif
#endif
// include it first:
#ifdef SPI_BOOST_USER_CONFIG
#  include SPI_BOOST_USER_CONFIG
#endif

// if we don't have a compiler config set, try and find one:
#if !defined(SPI_BOOST_COMPILER_CONFIG) && !defined(SPI_BOOST_NO_COMPILER_CONFIG) && !defined(SPI_BOOST_NO_CONFIG)
#  include <spi_boost/config/detail/select_compiler_config.hpp>
#endif
// if we have a compiler config, include it now:
#ifdef SPI_BOOST_COMPILER_CONFIG
#  include SPI_BOOST_COMPILER_CONFIG
#endif

// if we don't have a std library config set, try and find one:
#if !defined(SPI_BOOST_STDLIB_CONFIG) && !defined(SPI_BOOST_NO_STDLIB_CONFIG) && !defined(SPI_BOOST_NO_CONFIG) && defined(__cplusplus)
#  include <spi_boost/config/detail/select_stdlib_config.hpp>
#endif
// if we have a std library config, include it now:
#ifdef SPI_BOOST_STDLIB_CONFIG
#  include SPI_BOOST_STDLIB_CONFIG
#endif

// if we don't have a platform config set, try and find one:
#if !defined(SPI_BOOST_PLATFORM_CONFIG) && !defined(SPI_BOOST_NO_PLATFORM_CONFIG) && !defined(SPI_BOOST_NO_CONFIG)
#  include <spi_boost/config/detail/select_platform_config.hpp>
#endif
// if we have a platform config, include it now:
#ifdef SPI_BOOST_PLATFORM_CONFIG
#  include SPI_BOOST_PLATFORM_CONFIG
#endif

// get config suffix code:
#include <spi_boost/config/detail/suffix.hpp>

#ifdef SPI_BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#endif  // SPI_BOOST_CONFIG_HPP
