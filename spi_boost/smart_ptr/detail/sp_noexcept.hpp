#ifndef SPI_BOOST_SMART_PTR_DETAIL_SP_NOEXCEPT_HPP_INCLUDED
#define SPI_BOOST_SMART_PTR_DETAIL_SP_NOEXCEPT_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  detail/sp_noexcept.hpp
//
//  Copyright 2016, 2017 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

#include <spi_boost/config.hpp>

// SPI_BOOST_SP_NOEXCEPT

#if defined( SPI_BOOST_MSVC ) && SPI_BOOST_MSVC >= 1700 && SPI_BOOST_MSVC < 1900

#  define SPI_BOOST_SP_NOEXCEPT SPI_BOOST_NOEXCEPT_OR_NOTHROW

#else

#  define SPI_BOOST_SP_NOEXCEPT SPI_BOOST_NOEXCEPT

#endif

// SPI_BOOST_SP_NOEXCEPT_WITH_ASSERT

#if defined(SPI_BOOST_DISABLE_ASSERTS) || ( defined(SPI_BOOST_ENABLE_ASSERT_DEBUG_HANDLER) && defined(NDEBUG) )

#  define SPI_BOOST_SP_NOEXCEPT_WITH_ASSERT SPI_BOOST_SP_NOEXCEPT

#elif defined(SPI_BOOST_ENABLE_ASSERT_HANDLER) || ( defined(SPI_BOOST_ENABLE_ASSERT_DEBUG_HANDLER) && !defined(NDEBUG) )

#  define SPI_BOOST_SP_NOEXCEPT_WITH_ASSERT

#else

#  define SPI_BOOST_SP_NOEXCEPT_WITH_ASSERT SPI_BOOST_SP_NOEXCEPT

#endif

#endif  // #ifndef SPI_BOOST_SMART_PTR_DETAIL_SP_NOEXCEPT_HPP_INCLUDED
