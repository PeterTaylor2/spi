//  (C) Copyright John Maddock 2001. 
//  (C) Copyright Darin Adler 2001. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Metrowerks standard library:

#ifndef __MSL_CPP__
#  include <spi_boost/config/no_tr1/utility.hpp>
#  ifndef __MSL_CPP__
#     error This is not the MSL standard library!
#  endif
#endif

#if __MSL_CPP__ >= 0x6000  // Pro 6
#  define SPI_BOOST_HAS_HASH
#  define SPI_BOOST_STD_EXTENSION_NAMESPACE Metrowerks
#endif
#define SPI_BOOST_HAS_SLIST

#if __MSL_CPP__ < 0x6209
#  define SPI_BOOST_NO_STD_MESSAGES
#endif

// check C lib version for <stdint.h>
#include <cstddef>

#if defined(__MSL__) && (__MSL__ >= 0x5000)
#  define SPI_BOOST_HAS_STDINT_H
#  if !defined(__PALMOS_TRAPS__)
#    define SPI_BOOST_HAS_UNISTD_H
#  endif
   // boilerplate code:
#  include <spi_boost/config/detail/posix_features.hpp>
#endif

#if defined(_MWMT) || _MSL_THREADSAFE
#  define SPI_BOOST_HAS_THREADS
#endif

#ifdef _MSL_NO_EXPLICIT_FUNC_TEMPLATE_ARG
#  define SPI_BOOST_NO_STD_USE_FACET
#  define SPI_BOOST_HAS_TWO_ARG_USE_FACET
#endif

//  C++0x headers not yet implemented
//
#  define SPI_BOOST_NO_CXX11_HDR_ARRAY
#  define SPI_BOOST_NO_CXX11_HDR_CHRONO
#  define SPI_BOOST_NO_CXX11_HDR_CODECVT
#  define SPI_BOOST_NO_CXX11_HDR_CONDITION_VARIABLE
#  define SPI_BOOST_NO_CXX11_HDR_FORWARD_LIST
#  define SPI_BOOST_NO_CXX11_HDR_FUTURE
#  define SPI_BOOST_NO_CXX11_HDR_INITIALIZER_LIST
#  define SPI_BOOST_NO_CXX11_HDR_MUTEX
#  define SPI_BOOST_NO_CXX11_HDR_RANDOM
#  define SPI_BOOST_NO_CXX11_HDR_RATIO
#  define SPI_BOOST_NO_CXX11_HDR_REGEX
#  define SPI_BOOST_NO_CXX11_HDR_SYSTEM_ERROR
#  define SPI_BOOST_NO_CXX11_HDR_THREAD
#  define SPI_BOOST_NO_CXX11_HDR_TUPLE
#  define SPI_BOOST_NO_CXX11_HDR_TYPE_TRAITS
#  define SPI_BOOST_NO_CXX11_HDR_TYPEINDEX
#  define SPI_BOOST_NO_CXX11_HDR_UNORDERED_MAP
#  define SPI_BOOST_NO_CXX11_HDR_UNORDERED_SET
#  define SPI_BOOST_NO_CXX11_NUMERIC_LIMITS
#  define SPI_BOOST_NO_CXX11_ALLOCATOR
#  define SPI_BOOST_NO_CXX11_POINTER_TRAITS
#  define SPI_BOOST_NO_CXX11_ATOMIC_SMART_PTR
#  define SPI_BOOST_NO_CXX11_SMART_PTR
#  define SPI_BOOST_NO_CXX11_HDR_FUNCTIONAL
#  define SPI_BOOST_NO_CXX11_HDR_ATOMIC
#  define SPI_BOOST_NO_CXX11_STD_ALIGN
#  define SPI_BOOST_NO_CXX11_ADDRESSOF

#if defined(__has_include)
#if !__has_include(<shared_mutex>)
#  define SPI_BOOST_NO_CXX14_HDR_SHARED_MUTEX
#elif __cplusplus < 201402
#  define SPI_BOOST_NO_CXX14_HDR_SHARED_MUTEX
#endif
#else
#  define SPI_BOOST_NO_CXX14_HDR_SHARED_MUTEX
#endif

// C++14 features
#  define SPI_BOOST_NO_CXX14_STD_EXCHANGE

// C++17 features
#  define SPI_BOOST_NO_CXX17_STD_APPLY
#  define SPI_BOOST_NO_CXX17_STD_INVOKE
#  define SPI_BOOST_NO_CXX17_ITERATOR_TRAITS

#define SPI_BOOST_STDLIB "Metrowerks Standard Library version " SPI_BOOST_STRINGIZE(__MSL_CPP__)
