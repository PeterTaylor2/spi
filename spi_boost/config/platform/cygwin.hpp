//  (C) Copyright John Maddock 2001 - 2003. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  cygwin specific config options:

#define SPI_BOOST_PLATFORM "Cygwin"
#define SPI_BOOST_HAS_DIRENT_H
#define SPI_BOOST_HAS_LOG1P
#define SPI_BOOST_HAS_EXPM1

//
// Threading API:
// See if we have POSIX threads, if we do use them, otherwise
// revert to native Win threads.
#define SPI_BOOST_HAS_UNISTD_H
#include <unistd.h>
#if defined(_POSIX_THREADS) && (_POSIX_THREADS+0 >= 0) && !defined(SPI_BOOST_HAS_WINTHREADS)
#  define SPI_BOOST_HAS_PTHREADS
#  define SPI_BOOST_HAS_SCHED_YIELD
#  define SPI_BOOST_HAS_GETTIMEOFDAY
#  define SPI_BOOST_HAS_PTHREAD_MUTEXATTR_SETTYPE
//#  define SPI_BOOST_HAS_SIGACTION
#else
#  if !defined(SPI_BOOST_HAS_WINTHREADS)
#     define SPI_BOOST_HAS_WINTHREADS
#  endif
#  define SPI_BOOST_HAS_FTIME
#endif

//
// find out if we have a stdint.h, there should be a better way to do this:
//
#include <sys/types.h>
#ifdef _STDINT_H
#define SPI_BOOST_HAS_STDINT_H
#endif
#if __GNUC__ > 5 && !defined(SPI_BOOST_HAS_STDINT_H)
#   define SPI_BOOST_HAS_STDINT_H
#endif

#include <cygwin/version.h>
#if (CYGWIN_VERSION_API_MAJOR == 0 && CYGWIN_VERSION_API_MINOR < 231)
/// Cygwin has no fenv.h
#define SPI_BOOST_NO_FENV_H
#endif

// Cygwin has it's own <pthread.h> which breaks <shared_mutex> unless the correct compiler flags are used:
#ifndef SPI_BOOST_NO_CXX14_HDR_SHARED_MUTEX
#include <pthread.h>
#if !(__XSI_VISIBLE >= 500 || __POSIX_VISIBLE >= 200112)
#  define SPI_BOOST_NO_CXX14_HDR_SHARED_MUTEX
#endif
#endif

// boilerplate code:
#include <spi_boost/config/detail/posix_features.hpp>

//
// Cygwin lies about XSI conformance, there is no nl_types.h:
//
#ifdef SPI_BOOST_HAS_NL_TYPES_H
#  undef SPI_BOOST_HAS_NL_TYPES_H
#endif




