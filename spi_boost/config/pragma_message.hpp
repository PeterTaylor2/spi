#ifndef SPI_BOOST_CONFIG_PRAGMA_MESSAGE_HPP_INCLUDED
#define SPI_BOOST_CONFIG_PRAGMA_MESSAGE_HPP_INCLUDED

//  Copyright 2017 Peter Dimov.
//
//  Distributed under the Boost Software License, Version 1.0.
//
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//
//  SPI_BOOST_PRAGMA_MESSAGE("message")
//
//  Expands to the equivalent of #pragma message("message")
//
//  Note that this header is C compatible.

#include <spi_boost/config/helper_macros.hpp>

#if defined(SPI_BOOST_DISABLE_PRAGMA_MESSAGE)
# define SPI_BOOST_PRAGMA_MESSAGE(x)
#elif defined(__INTEL_COMPILER)
# define SPI_BOOST_PRAGMA_MESSAGE(x) __pragma(message(__FILE__ "(" SPI_BOOST_STRINGIZE(__LINE__) "): note: " x))
#elif defined(__GNUC__)
# define SPI_BOOST_PRAGMA_MESSAGE(x) _Pragma(SPI_BOOST_STRINGIZE(message(x)))
#elif defined(_MSC_VER)
# define SPI_BOOST_PRAGMA_MESSAGE(x) __pragma(message(__FILE__ "(" SPI_BOOST_STRINGIZE(__LINE__) "): note: " x))
#else
# define SPI_BOOST_PRAGMA_MESSAGE(x)
#endif

#endif // SPI_BOOST_CONFIG_PRAGMA_MESSAGE_HPP_INCLUDED
