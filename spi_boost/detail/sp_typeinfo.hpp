#ifndef SPI_BOOST_DETAIL_SP_TYPEINFO_HPP_INCLUDED
#define SPI_BOOST_DETAIL_SP_TYPEINFO_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  detail/sp_typeinfo.hpp
//
//  Deprecated, please use spi_boost/core/typeinfo.hpp
//
//  Copyright 2007 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <spi_boost/core/typeinfo.hpp>

namespace spi_boost
{

namespace detail
{

typedef spi_boost::core::typeinfo sp_typeinfo;

} // namespace detail

} // namespace spi_boost

#define SPI_BOOST_SP_TYPEID(T) SPI_BOOST_CORE_TYPEID(T)

#endif  // #ifndef SPI_BOOST_DETAIL_SP_TYPEINFO_HPP_INCLUDED
