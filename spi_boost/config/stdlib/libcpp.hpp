//  (C) Copyright Christopher Jefferson 2011.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  config for libc++
//  Might need more in here later.

#if !defined(_LIBCPP_VERSION)
#  include <ciso646>
#  if !defined(_LIBCPP_VERSION)
#      error "This is not libc++!"
#  endif
#endif

#define SPI_BOOST_STDLIB "libc++ version " SPI_BOOST_STRINGIZE(_LIBCPP_VERSION)

#define SPI_BOOST_HAS_THREADS

#ifdef _LIBCPP_HAS_NO_VARIADICS
#    define SPI_BOOST_NO_CXX11_HDR_TUPLE
#endif

// SPI_BOOST_NO_CXX11_ALLOCATOR should imply no support for the C++11
// allocator model. The C++11 allocator model requires a conforming
// std::allocator_traits which is only possible with C++11 template
// aliases since members rebind_alloc and rebind_traits require it.
#if defined(_LIBCPP_HAS_NO_TEMPLATE_ALIASES)
#    define SPI_BOOST_NO_CXX11_ALLOCATOR
#    define SPI_BOOST_NO_CXX11_POINTER_TRAITS
#endif

#if __cplusplus < 201103
//
// These two appear to be somewhat useable in C++03 mode, there may be others...
//
//#  define SPI_BOOST_NO_CXX11_HDR_ARRAY
//#  define SPI_BOOST_NO_CXX11_HDR_FORWARD_LIST

#  define SPI_BOOST_NO_CXX11_HDR_CODECVT
#  define SPI_BOOST_NO_CXX11_HDR_CONDITION_VARIABLE
#  define SPI_BOOST_NO_CXX11_HDR_INITIALIZER_LIST
#  define SPI_BOOST_NO_CXX11_HDR_MUTEX
#  define SPI_BOOST_NO_CXX11_HDR_RANDOM
#  define SPI_BOOST_NO_CXX11_HDR_RATIO
#  define SPI_BOOST_NO_CXX11_HDR_REGEX
#  define SPI_BOOST_NO_CXX11_HDR_SYSTEM_ERROR
#  define SPI_BOOST_NO_CXX11_HDR_THREAD
#  define SPI_BOOST_NO_CXX11_HDR_TUPLE
#  define SPI_BOOST_NO_CXX11_HDR_TYPEINDEX
#  define SPI_BOOST_NO_CXX11_HDR_UNORDERED_MAP
#  define SPI_BOOST_NO_CXX11_HDR_UNORDERED_SET
#  define SPI_BOOST_NO_CXX11_NUMERIC_LIMITS
#  define SPI_BOOST_NO_CXX11_ALLOCATOR
#  define SPI_BOOST_NO_CXX11_POINTER_TRAITS
#  define SPI_BOOST_NO_CXX11_SMART_PTR
#  define SPI_BOOST_NO_CXX11_HDR_FUNCTIONAL
#  define SPI_BOOST_NO_CXX11_STD_ALIGN
#  define SPI_BOOST_NO_CXX11_ADDRESSOF
#  define SPI_BOOST_NO_CXX11_HDR_ATOMIC
#  define SPI_BOOST_NO_CXX11_ATOMIC_SMART_PTR
#  define SPI_BOOST_NO_CXX11_HDR_CHRONO
#  define SPI_BOOST_NO_CXX11_HDR_TYPE_TRAITS
#  define SPI_BOOST_NO_CXX11_HDR_FUTURE
#elif _LIBCPP_VERSION < 3700
//
// These appear to be unusable/incomplete so far:
//
#  define SPI_BOOST_NO_CXX11_HDR_ATOMIC
#  define SPI_BOOST_NO_CXX11_ATOMIC_SMART_PTR
#  define SPI_BOOST_NO_CXX11_HDR_CHRONO
#  define SPI_BOOST_NO_CXX11_HDR_TYPE_TRAITS
#  define SPI_BOOST_NO_CXX11_HDR_FUTURE
#endif


#if _LIBCPP_VERSION < 3700
// libc++ uses a non-standard messages_base
#define SPI_BOOST_NO_STD_MESSAGES
#endif

// C++14 features
#if (_LIBCPP_VERSION < 3700) || (__cplusplus <= 201402L)
#  define SPI_BOOST_NO_CXX14_STD_EXCHANGE
#endif

// C++17 features
#if (_LIBCPP_VERSION < 4000) || (__cplusplus <= 201402L)
#  define SPI_BOOST_NO_CXX17_STD_APPLY
#  define SPI_BOOST_NO_CXX17_HDR_OPTIONAL
#  define SPI_BOOST_NO_CXX17_HDR_STRING_VIEW
#endif
#if (_LIBCPP_VERSION > 4000) && (__cplusplus > 201402L) && !defined(_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR)
#  define SPI_BOOST_NO_AUTO_PTR
#endif
#if (_LIBCPP_VERSION > 4000) && (__cplusplus > 201402L) && !defined(_LIBCPP_ENABLE_CXX17_REMOVED_RANDOM_SHUFFLE)
#  define SPI_BOOST_NO_CXX98_RANDOM_SHUFFLE
#endif
#if (_LIBCPP_VERSION > 4000) && (__cplusplus > 201402L) && !defined(_LIBCPP_ENABLE_CXX17_REMOVED_BINDERS)
#  define SPI_BOOST_NO_CXX98_BINDERS
#endif

#define SPI_BOOST_NO_CXX17_ITERATOR_TRAITS
#define SPI_BOOST_NO_CXX17_STD_INVOKE      // Invoke support is incomplete (no invoke_result)

#if (_LIBCPP_VERSION <= 1101) && !defined(SPI_BOOST_NO_CXX11_THREAD_LOCAL)
// This is a bit of a sledgehammer, because really it's just libc++abi that has no
// support for thread_local, leading to linker errors such as
// "undefined reference to `__cxa_thread_atexit'".  It is fixed in the
// most recent releases of libc++abi though...
#  define SPI_BOOST_NO_CXX11_THREAD_LOCAL
#endif

#if defined(__linux__) && (_LIBCPP_VERSION < 6000) && !defined(SPI_BOOST_NO_CXX11_THREAD_LOCAL)
// After libc++-dev is installed on Trusty, clang++-libc++ almost works,
// except uses of `thread_local` fail with undefined reference to
// `__cxa_thread_atexit`.
//
// clang's libc++abi provides an implementation by deferring to the glibc
// implementation, which may or may not be available (it is not on Trusty).
// clang 4's libc++abi will provide an implementation if one is not in glibc
// though, so thread local support should work with clang 4 and above as long
// as libc++abi is linked in.
#  define SPI_BOOST_NO_CXX11_THREAD_LOCAL
#endif

#if defined(__has_include)
#if !__has_include(<shared_mutex>)
#  define SPI_BOOST_NO_CXX14_HDR_SHARED_MUTEX
#elif __cplusplus <= 201103
#  define SPI_BOOST_NO_CXX14_HDR_SHARED_MUTEX
#endif
#elif __cplusplus < 201402
#  define SPI_BOOST_NO_CXX14_HDR_SHARED_MUTEX
#endif

#if !defined(SPI_BOOST_NO_CXX14_HDR_SHARED_MUTEX) && (_LIBCPP_VERSION < 5000)
#  define SPI_BOOST_NO_CXX14_HDR_SHARED_MUTEX
#endif

//  --- end ---
