#ifndef SPI_BOOST_SMART_PTR_DETAIL_SP_COUNTED_BASE_HPP_INCLUDED
#define SPI_BOOST_SMART_PTR_DETAIL_SP_COUNTED_BASE_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  detail/sp_counted_base.hpp
//
//  Copyright 2005-2013 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <spi_boost/config.hpp>
#include <spi_boost/smart_ptr/detail/sp_has_sync.hpp>

#if !defined( __c2__ ) && defined( __clang__ ) && defined( __has_extension )
# if __has_extension( __c_atomic__ )
#   define SPI_BOOST_SP_HAS_CLANG_C11_ATOMICS
# endif
#endif

#if defined( SPI_BOOST_SP_DISABLE_THREADS )
# include <spi_boost/smart_ptr/detail/sp_counted_base_nt.hpp>

#elif defined( SPI_BOOST_SP_USE_STD_ATOMIC )
# include <spi_boost/smart_ptr/detail/sp_counted_base_std_atomic.hpp>

#elif defined( SPI_BOOST_SP_USE_SPINLOCK )
# include <spi_boost/smart_ptr/detail/sp_counted_base_spin.hpp>

#elif defined( SPI_BOOST_SP_USE_PTHREADS )
# include <spi_boost/smart_ptr/detail/sp_counted_base_pt.hpp>

#elif defined( SPI_BOOST_DISABLE_THREADS ) && !defined( SPI_BOOST_SP_ENABLE_THREADS ) && !defined( SPI_BOOST_DISABLE_WIN32 )
# include <spi_boost/smart_ptr/detail/sp_counted_base_nt.hpp>

#elif defined( SPI_BOOST_SP_HAS_CLANG_C11_ATOMICS )
# include <spi_boost/smart_ptr/detail/sp_counted_base_clang.hpp>

#elif !defined( SPI_BOOST_NO_CXX11_HDR_ATOMIC )
# include <spi_boost/smart_ptr/detail/sp_counted_base_std_atomic.hpp>

#elif defined( __SNC__ )
# include <spi_boost/smart_ptr/detail/sp_counted_base_snc_ps3.hpp>

#elif defined( __GNUC__ ) && ( defined( __i386__ ) || defined( __x86_64__ ) ) && !defined(__PATHSCALE__)
# include <spi_boost/smart_ptr/detail/sp_counted_base_gcc_x86.hpp>

#elif defined(__HP_aCC) && defined(__ia64)
# include <spi_boost/smart_ptr/detail/sp_counted_base_acc_ia64.hpp>

#elif defined( __GNUC__ ) && defined( __ia64__ ) && !defined( __INTEL_COMPILER ) && !defined(__PATHSCALE__)
# include <spi_boost/smart_ptr/detail/sp_counted_base_gcc_ia64.hpp>

#elif defined( __IBMCPP__ ) && defined( __powerpc )
# include <spi_boost/smart_ptr/detail/sp_counted_base_vacpp_ppc.hpp>

#elif defined( __MWERKS__ ) && defined( __POWERPC__ )
# include <spi_boost/smart_ptr/detail/sp_counted_base_cw_ppc.hpp>

#elif defined( __GNUC__ ) && ( defined( __powerpc__ ) || defined( __ppc__ ) || defined( __ppc ) ) && !defined(__PATHSCALE__) && !defined( _AIX )
# include <spi_boost/smart_ptr/detail/sp_counted_base_gcc_ppc.hpp>

#elif defined( __GNUC__ ) && ( defined( __mips__ ) || defined( _mips ) ) && !defined(__PATHSCALE__) && !defined( __mips16 )
# include <spi_boost/smart_ptr/detail/sp_counted_base_gcc_mips.hpp>

#elif defined( SPI_BOOST_SP_HAS_SYNC )
# include <spi_boost/smart_ptr/detail/sp_counted_base_sync.hpp>

#elif defined(__GNUC__) && ( defined( __sparcv9 ) || ( defined( __sparcv8 ) && ( __GNUC__ * 100 + __GNUC_MINOR__ >= 402 ) ) )
# include <spi_boost/smart_ptr/detail/sp_counted_base_gcc_sparc.hpp>

#elif defined( WIN32 ) || defined( _WIN32 ) || defined( __WIN32__ ) || defined(__CYGWIN__)
# include <spi_boost/smart_ptr/detail/sp_counted_base_w32.hpp>

#elif defined( _AIX )
# include <spi_boost/smart_ptr/detail/sp_counted_base_aix.hpp>

#elif !defined( SPI_BOOST_HAS_THREADS )
# include <spi_boost/smart_ptr/detail/sp_counted_base_nt.hpp>

#else
# include <spi_boost/smart_ptr/detail/sp_counted_base_spin.hpp>

#endif

#undef SPI_BOOST_SP_HAS_CLANG_C11_ATOMICS

#endif  // #ifndef SPI_BOOST_SMART_PTR_DETAIL_SP_COUNTED_BASE_HPP_INCLUDED
