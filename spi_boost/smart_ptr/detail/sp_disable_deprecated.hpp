#ifndef SPI_BOOST_SMART_PTR_DETAIL_SP_DISABLE_DEPRECATED_HPP_INCLUDED
#define SPI_BOOST_SMART_PTR_DETAIL_SP_DISABLE_DEPRECATED_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  spi_boost/smart_ptr/detail/sp_disable_deprecated.hpp
//
//  Copyright 2015 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#include <spi_boost/config.hpp>

#if defined( __GNUC__ ) && ( defined( __GXX_EXPERIMENTAL_CXX0X__ ) || ( __cplusplus >= 201103L ) )

# if defined( SPI_BOOST_GCC )

#  if SPI_BOOST_GCC >= 40600
#   define SPI_BOOST_SP_DISABLE_DEPRECATED
#  endif

# elif defined( __clang__ ) && defined( __has_warning )

#  if __has_warning( "-Wdeprecated-declarations" )
#   define SPI_BOOST_SP_DISABLE_DEPRECATED
#  endif

# endif

#endif

#endif // #ifndef SPI_BOOST_SMART_PTR_DETAIL_SP_DISABLE_DEPRECATED_HPP_INCLUDED
