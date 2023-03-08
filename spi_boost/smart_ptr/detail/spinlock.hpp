#ifndef SPI_BOOST_SMART_PTR_DETAIL_SPINLOCK_HPP_INCLUDED
#define SPI_BOOST_SMART_PTR_DETAIL_SPINLOCK_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  spi_boost/detail/spinlock.hpp
//
//  Copyright (c) 2008 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  struct spinlock
//  {
//      void lock();
//      bool try_lock();
//      void unlock();
//
//      class scoped_lock;
//  };
//
//  #define SPI_BOOST_DETAIL_SPINLOCK_INIT <unspecified>
//

#include <spi_boost/config.hpp>
#include <spi_boost/smart_ptr/detail/sp_has_sync.hpp>

#if defined( SPI_BOOST_SP_USE_STD_ATOMIC )
# if !defined( __clang__ )
#   include <spi_boost/smart_ptr/detail/spinlock_std_atomic.hpp>
# else
//  Clang (at least up to 3.4) can't compile spinlock_pool when
//  using std::atomic, so substitute the __sync implementation instead.
#   include <spi_boost/smart_ptr/detail/spinlock_sync.hpp>
# endif

#elif defined( SPI_BOOST_SP_USE_PTHREADS )
#  include <spi_boost/smart_ptr/detail/spinlock_pt.hpp>

#elif !defined( SPI_BOOST_NO_CXX11_HDR_ATOMIC )
#  include <spi_boost/smart_ptr/detail/spinlock_std_atomic.hpp>

#elif defined(__GNUC__) && defined( __arm__ ) && !defined( __thumb__ )
#  include <spi_boost/smart_ptr/detail/spinlock_gcc_arm.hpp>

#elif defined( SPI_BOOST_SP_HAS_SYNC )
#  include <spi_boost/smart_ptr/detail/spinlock_sync.hpp>

#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#  include <spi_boost/smart_ptr/detail/spinlock_w32.hpp>

#elif defined(SPI_BOOST_HAS_PTHREADS)
#  include <spi_boost/smart_ptr/detail/spinlock_pt.hpp>

#elif !defined(SPI_BOOST_HAS_THREADS)
#  include <spi_boost/smart_ptr/detail/spinlock_nt.hpp>

#else
#  error Unrecognized threading platform
#endif

#endif // #ifndef SPI_BOOST_SMART_PTR_DETAIL_SPINLOCK_HPP_INCLUDED
