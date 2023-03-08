#ifndef SPI_BOOST_CONFIG_HEADER_DEPRECATED_HPP_INCLUDED
#define SPI_BOOST_CONFIG_HEADER_DEPRECATED_HPP_INCLUDED

//  Copyright 2017 Peter Dimov.
//
//  Distributed under the Boost Software License, Version 1.0.
//
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//
//  SPI_BOOST_HEADER_DEPRECATED("<alternative>")
//
//  Expands to the equivalent of
//    SPI_BOOST_PRAGMA_MESSAGE("This header is deprecated. Use <alternative> instead.")
//
//  Note that this header is C compatible.

#include <spi_boost/config/pragma_message.hpp>

#if defined(SPI_BOOST_ALLOW_DEPRECATED_HEADERS)
# define SPI_BOOST_HEADER_DEPRECATED(a)
#else
# define SPI_BOOST_HEADER_DEPRECATED(a) SPI_BOOST_PRAGMA_MESSAGE("This header is deprecated. Use " a " instead.")
#endif

#endif // SPI_BOOST_CONFIG_HEADER_DEPRECATED_HPP_INCLUDED
