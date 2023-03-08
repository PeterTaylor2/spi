//  (C) Copyright John Maddock 2001 - 2003. 
//  (C) Copyright Darin Adler 2001. 
//  (C) Copyright Jens Maurer 2001 - 2003. 
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  generic SGI STL:

#if !defined(__STL_CONFIG_H)
#  include <spi_boost/config/no_tr1/utility.hpp>
#  if !defined(__STL_CONFIG_H)
#      error "This is not the SGI STL!"
#  endif
#endif

//
// No std::iterator traits without partial specialisation:
//
#if !defined(__STL_CLASS_PARTIAL_SPECIALIZATION)
#  define SPI_BOOST_NO_STD_ITERATOR_TRAITS
#endif

//
// No std::stringstream with gcc < 3
//
#if defined(__GNUC__) && (__GNUC__ < 3) && \
     ((__GNUC_MINOR__ < 95) || (__GNUC_MINOR__ == 96)) && \
     !defined(__STL_USE_NEW_IOSTREAMS) || \
   defined(__APPLE_CC__)
   // Note that we only set this for GNU C++ prior to 2.95 since the
   // latest patches for that release do contain a minimal <sstream>
   // If you are running a 2.95 release prior to 2.95.3 then this will need
   // setting, but there is no way to detect that automatically (other
   // than by running the configure script).
   // Also, the unofficial GNU C++ 2.96 included in RedHat 7.1 doesn't
   // have <sstream>.
#  define SPI_BOOST_NO_STRINGSTREAM
#endif

// Apple doesn't seem to reliably defined a *unix* macro
#if !defined(CYGWIN) && (  defined(__unix__)  \
                        || defined(__unix)    \
                        || defined(unix)      \
                        || defined(__APPLE__) \
                        || defined(__APPLE)   \
                        || defined(APPLE))
#  include <unistd.h>
#endif


//
// Assume no std::locale without own iostreams (this may be an
// incorrect assumption in some cases):
//
#if !defined(__SGI_STL_OWN_IOSTREAMS) && !defined(__STL_USE_NEW_IOSTREAMS)
#  define SPI_BOOST_NO_STD_LOCALE
#endif

//
// Original native SGI streams have non-standard std::messages facet:
//
#if defined(__sgi) && (_COMPILER_VERSION <= 650) && !defined(__SGI_STL_OWN_IOSTREAMS)
#  define SPI_BOOST_NO_STD_LOCALE
#endif

//
// SGI's new iostreams have missing "const" in messages<>::open
//
#if defined(__sgi) && (_COMPILER_VERSION <= 740) && defined(__STL_USE_NEW_IOSTREAMS)
#  define SPI_BOOST_NO_STD_MESSAGES
#endif

//
// No template iterator constructors, or std::allocator
// without member templates:
//
#if !defined(__STL_MEMBER_TEMPLATES)
#  define SPI_BOOST_NO_TEMPLATED_ITERATOR_CONSTRUCTORS
#  define SPI_BOOST_NO_STD_ALLOCATOR
#endif

//
// We always have SGI style hash_set, hash_map, and slist:
//
#define SPI_BOOST_HAS_HASH
#define SPI_BOOST_HAS_SLIST

//
// If this is GNU libstdc++2, then no <limits> and no std::wstring:
//
#if (defined(__GNUC__) && (__GNUC__ < 3))
#  include <string>
#  if defined(__BASTRING__)
#     define SPI_BOOST_NO_LIMITS
// Note: <spi_boost/limits.hpp> will provide compile-time constants
#     undef SPI_BOOST_NO_LIMITS_COMPILE_TIME_CONSTANTS
#     define SPI_BOOST_NO_STD_WSTRING
#  endif
#endif

//
// There is no standard iterator unless we have namespace support:
//
#if !defined(__STL_USE_NAMESPACES)
#  define SPI_BOOST_NO_STD_ITERATOR
#endif

//
// Intrinsic type_traits support.
// The SGI STL has it's own __type_traits class, which
// has intrinsic compiler support with SGI's compilers.
// Whatever map SGI style type traits to spi_boost equivalents:
//
#define SPI_BOOST_HAS_SGI_TYPE_TRAITS

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

#define SPI_BOOST_STDLIB "SGI standard library"
