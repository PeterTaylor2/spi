#ifndef SPI_BOOST_SMART_PTR_DETAIL_SP_INTERLOCKED_HPP_INCLUDED
#define SPI_BOOST_SMART_PTR_DETAIL_SP_INTERLOCKED_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  spi_boost/detail/sp_interlocked.hpp
//
//  Copyright 2005, 2014 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//

#include <spi_boost/config.hpp>

// SPI_BOOST_SP_HAS_INTRIN_H

// VC9 has intrin.h, but it collides with <utility>
#if defined( SPI_BOOST_MSVC ) && SPI_BOOST_MSVC >= 1600

# define SPI_BOOST_SP_HAS_INTRIN_H

// Unlike __MINGW64__, __MINGW64_VERSION_MAJOR is defined by MinGW-w64 for both 32 and 64-bit targets.
#elif defined( __MINGW64_VERSION_MAJOR )

// MinGW-w64 provides intrin.h for both 32 and 64-bit targets.
# define SPI_BOOST_SP_HAS_INTRIN_H

#elif defined( __LP64__ )

// We have to use intrin.h on Cygwin 64
# define SPI_BOOST_SP_HAS_INTRIN_H

// Intel C++ on Windows on VC10+ stdlib
#elif defined( SPI_BOOST_INTEL_WIN ) && defined( _CPPLIB_VER ) && _CPPLIB_VER >= 520

# define SPI_BOOST_SP_HAS_INTRIN_H

// clang-cl on Windows on VC10+ stdlib
#elif defined( __clang__ ) && defined( _MSC_VER ) && defined( _CPPLIB_VER ) && _CPPLIB_VER >= 520

# define SPI_BOOST_SP_HAS_INTRIN_H

#endif

#if defined( SPI_BOOST_USE_WINDOWS_H )

# include <windows.h>

# define SPI_BOOST_SP_INTERLOCKED_INCREMENT InterlockedIncrement
# define SPI_BOOST_SP_INTERLOCKED_DECREMENT InterlockedDecrement
# define SPI_BOOST_SP_INTERLOCKED_COMPARE_EXCHANGE InterlockedCompareExchange
# define SPI_BOOST_SP_INTERLOCKED_EXCHANGE InterlockedExchange
# define SPI_BOOST_SP_INTERLOCKED_EXCHANGE_ADD InterlockedExchangeAdd

#elif defined( SPI_BOOST_USE_INTRIN_H ) || defined( SPI_BOOST_SP_HAS_INTRIN_H )

#include <intrin.h>

# define SPI_BOOST_SP_INTERLOCKED_INCREMENT _InterlockedIncrement
# define SPI_BOOST_SP_INTERLOCKED_DECREMENT _InterlockedDecrement
# define SPI_BOOST_SP_INTERLOCKED_COMPARE_EXCHANGE _InterlockedCompareExchange
# define SPI_BOOST_SP_INTERLOCKED_EXCHANGE _InterlockedExchange
# define SPI_BOOST_SP_INTERLOCKED_EXCHANGE_ADD _InterlockedExchangeAdd

#elif defined( _WIN32_WCE )

#if _WIN32_WCE >= 0x600

extern "C" long __cdecl _InterlockedIncrement( long volatile * );
extern "C" long __cdecl _InterlockedDecrement( long volatile * );
extern "C" long __cdecl _InterlockedCompareExchange( long volatile *, long, long );
extern "C" long __cdecl _InterlockedExchange( long volatile *, long );
extern "C" long __cdecl _InterlockedExchangeAdd( long volatile *, long );

# define SPI_BOOST_SP_INTERLOCKED_INCREMENT _InterlockedIncrement
# define SPI_BOOST_SP_INTERLOCKED_DECREMENT _InterlockedDecrement
# define SPI_BOOST_SP_INTERLOCKED_COMPARE_EXCHANGE _InterlockedCompareExchange
# define SPI_BOOST_SP_INTERLOCKED_EXCHANGE _InterlockedExchange
# define SPI_BOOST_SP_INTERLOCKED_EXCHANGE_ADD _InterlockedExchangeAdd

#else

// under Windows CE we still have old-style Interlocked* functions

extern "C" long __cdecl InterlockedIncrement( long* );
extern "C" long __cdecl InterlockedDecrement( long* );
extern "C" long __cdecl InterlockedCompareExchange( long*, long, long );
extern "C" long __cdecl InterlockedExchange( long*, long );
extern "C" long __cdecl InterlockedExchangeAdd( long*, long );

# define SPI_BOOST_SP_INTERLOCKED_INCREMENT InterlockedIncrement
# define SPI_BOOST_SP_INTERLOCKED_DECREMENT InterlockedDecrement
# define SPI_BOOST_SP_INTERLOCKED_COMPARE_EXCHANGE InterlockedCompareExchange
# define SPI_BOOST_SP_INTERLOCKED_EXCHANGE InterlockedExchange
# define SPI_BOOST_SP_INTERLOCKED_EXCHANGE_ADD InterlockedExchangeAdd

#endif

#elif defined( SPI_BOOST_MSVC ) || defined( SPI_BOOST_INTEL_WIN )

#if defined( __CLRCALL_PURE_OR_CDECL )

extern "C" long __CLRCALL_PURE_OR_CDECL _InterlockedIncrement( long volatile * );
extern "C" long __CLRCALL_PURE_OR_CDECL _InterlockedDecrement( long volatile * );
extern "C" long __CLRCALL_PURE_OR_CDECL _InterlockedCompareExchange( long volatile *, long, long );
extern "C" long __CLRCALL_PURE_OR_CDECL _InterlockedExchange( long volatile *, long );
extern "C" long __CLRCALL_PURE_OR_CDECL _InterlockedExchangeAdd( long volatile *, long );

#else

extern "C" long __cdecl _InterlockedIncrement( long volatile * );
extern "C" long __cdecl _InterlockedDecrement( long volatile * );
extern "C" long __cdecl _InterlockedCompareExchange( long volatile *, long, long );
extern "C" long __cdecl _InterlockedExchange( long volatile *, long );
extern "C" long __cdecl _InterlockedExchangeAdd( long volatile *, long );

# if defined( SPI_BOOST_MSVC ) && SPI_BOOST_MSVC == 1310
//From MSDN, Visual Studio .NET 2003 spedific: To declare one of the interlocked functions
//for use as an intrinsic, the function must be declared with the leading underscore and
//the new function must appear in a #pragma intrinsic statement.
#  pragma intrinsic( _InterlockedIncrement )
#  pragma intrinsic( _InterlockedDecrement )
#  pragma intrinsic( _InterlockedCompareExchange )
#  pragma intrinsic( _InterlockedExchange )
#  pragma intrinsic( _InterlockedExchangeAdd )
# endif

#endif

# define SPI_BOOST_SP_INTERLOCKED_INCREMENT _InterlockedIncrement
# define SPI_BOOST_SP_INTERLOCKED_DECREMENT _InterlockedDecrement
# define SPI_BOOST_SP_INTERLOCKED_COMPARE_EXCHANGE _InterlockedCompareExchange
# define SPI_BOOST_SP_INTERLOCKED_EXCHANGE _InterlockedExchange
# define SPI_BOOST_SP_INTERLOCKED_EXCHANGE_ADD _InterlockedExchangeAdd

#elif defined( WIN32 ) || defined( _WIN32 ) || defined( __WIN32__ ) || defined( __CYGWIN__ )

namespace spi_boost
{

namespace detail
{

extern "C" __declspec(dllimport) long __stdcall InterlockedIncrement( long volatile * );
extern "C" __declspec(dllimport) long __stdcall InterlockedDecrement( long volatile * );
extern "C" __declspec(dllimport) long __stdcall InterlockedCompareExchange( long volatile *, long, long );
extern "C" __declspec(dllimport) long __stdcall InterlockedExchange( long volatile *, long );
extern "C" __declspec(dllimport) long __stdcall InterlockedExchangeAdd( long volatile *, long );

} // namespace detail

} // namespace spi_boost

# define SPI_BOOST_SP_INTERLOCKED_INCREMENT ::spi_boost::detail::InterlockedIncrement
# define SPI_BOOST_SP_INTERLOCKED_DECREMENT ::spi_boost::detail::InterlockedDecrement
# define SPI_BOOST_SP_INTERLOCKED_COMPARE_EXCHANGE ::spi_boost::detail::InterlockedCompareExchange
# define SPI_BOOST_SP_INTERLOCKED_EXCHANGE ::spi_boost::detail::InterlockedExchange
# define SPI_BOOST_SP_INTERLOCKED_EXCHANGE_ADD ::spi_boost::detail::InterlockedExchangeAdd

#else

# error "Interlocked intrinsics not available"

#endif

#endif // #ifndef SPI_BOOST_SMART_PTR_DETAIL_SP_INTERLOCKED_HPP_INCLUDED
