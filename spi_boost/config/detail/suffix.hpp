//  Boost config.hpp configuration header file  ------------------------------//
//  spi_boostinspect:ndprecated_macros -- tell the inspect tool to ignore this file

//  Copyright (c) 2001-2003 John Maddock
//  Copyright (c) 2001 Darin Adler
//  Copyright (c) 2001 Peter Dimov
//  Copyright (c) 2002 Bill Kempf
//  Copyright (c) 2002 Jens Maurer
//  Copyright (c) 2002-2003 David Abrahams
//  Copyright (c) 2003 Gennaro Prota
//  Copyright (c) 2003 Eric Friedman
//  Copyright (c) 2010 Eric Jourdanneau, Joel Falcou
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/ for most recent version.

//  Boost config.hpp policy and rationale documentation has been moved to
//  http://www.boost.org/libs/config/
//
//  This file is intended to be stable, and relatively unchanging.
//  It should contain boilerplate code only - no compiler specific
//  code unless it is unavoidable - no changes unless unavoidable.

#ifndef SPI_BOOST_CONFIG_SUFFIX_HPP
#define SPI_BOOST_CONFIG_SUFFIX_HPP

#if defined(__GNUC__) && (__GNUC__ >= 4)
//
// Some GCC-4.x versions issue warnings even when __extension__ is used,
// so use this as a workaround:
//
#pragma GCC system_header
#endif

//
// ensure that visibility macros are always defined, thus symplifying use
//
#ifndef SPI_BOOST_SYMBOL_EXPORT
# define SPI_BOOST_SYMBOL_EXPORT
#endif
#ifndef SPI_BOOST_SYMBOL_IMPORT
# define SPI_BOOST_SYMBOL_IMPORT
#endif
#ifndef SPI_BOOST_SYMBOL_VISIBLE
# define SPI_BOOST_SYMBOL_VISIBLE
#endif

//
// look for long long by looking for the appropriate macros in <limits.h>.
// Note that we use limits.h rather than climits for maximal portability,
// remember that since these just declare a bunch of macros, there should be
// no namespace issues from this.
//
#if !defined(SPI_BOOST_HAS_LONG_LONG) && !defined(SPI_BOOST_NO_LONG_LONG)                                              \
   && !defined(SPI_BOOST_MSVC) && !defined(__BORLANDC__)
# include <limits.h>
# if (defined(ULLONG_MAX) || defined(ULONG_LONG_MAX) || defined(ULONGLONG_MAX))
#   define SPI_BOOST_HAS_LONG_LONG
# else
#   define SPI_BOOST_NO_LONG_LONG
# endif
#endif

// GCC 3.x will clean up all of those nasty macro definitions that
// SPI_BOOST_NO_CTYPE_FUNCTIONS is intended to help work around, so undefine
// it under GCC 3.x.
#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(SPI_BOOST_NO_CTYPE_FUNCTIONS)
#  undef SPI_BOOST_NO_CTYPE_FUNCTIONS
#endif

//
// Assume any extensions are in namespace std:: unless stated otherwise:
//
#  ifndef SPI_BOOST_STD_EXTENSION_NAMESPACE
#    define SPI_BOOST_STD_EXTENSION_NAMESPACE std
#  endif

//
// If cv-qualified specializations are not allowed, then neither are cv-void ones:
//
#  if defined(SPI_BOOST_NO_CV_SPECIALIZATIONS) \
      && !defined(SPI_BOOST_NO_CV_VOID_SPECIALIZATIONS)
#     define SPI_BOOST_NO_CV_VOID_SPECIALIZATIONS
#  endif

//
// If there is no numeric_limits template, then it can't have any compile time
// constants either!
//
#  if defined(SPI_BOOST_NO_LIMITS) \
      && !defined(SPI_BOOST_NO_LIMITS_COMPILE_TIME_CONSTANTS)
#     define SPI_BOOST_NO_LIMITS_COMPILE_TIME_CONSTANTS
#     define SPI_BOOST_NO_MS_INT64_NUMERIC_LIMITS
#     define SPI_BOOST_NO_LONG_LONG_NUMERIC_LIMITS
#  endif

//
// if there is no long long then there is no specialisation
// for numeric_limits<long long> either:
//
#if !defined(SPI_BOOST_HAS_LONG_LONG) && !defined(SPI_BOOST_NO_LONG_LONG_NUMERIC_LIMITS)
#  define SPI_BOOST_NO_LONG_LONG_NUMERIC_LIMITS
#endif

//
// if there is no __int64 then there is no specialisation
// for numeric_limits<__int64> either:
//
#if !defined(SPI_BOOST_HAS_MS_INT64) && !defined(SPI_BOOST_NO_MS_INT64_NUMERIC_LIMITS)
#  define SPI_BOOST_NO_MS_INT64_NUMERIC_LIMITS
#endif

//
// if member templates are supported then so is the
// VC6 subset of member templates:
//
#  if !defined(SPI_BOOST_NO_MEMBER_TEMPLATES) \
       && !defined(SPI_BOOST_MSVC6_MEMBER_TEMPLATES)
#     define SPI_BOOST_MSVC6_MEMBER_TEMPLATES
#  endif

//
// Without partial specialization, can't test for partial specialisation bugs:
//
#  if defined(SPI_BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
      && !defined(SPI_BOOST_BCB_PARTIAL_SPECIALIZATION_BUG)
#     define SPI_BOOST_BCB_PARTIAL_SPECIALIZATION_BUG
#  endif

//
// Without partial specialization, we can't have array-type partial specialisations:
//
#  if defined(SPI_BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
      && !defined(SPI_BOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
#     define SPI_BOOST_NO_ARRAY_TYPE_SPECIALIZATIONS
#  endif

//
// Without partial specialization, std::iterator_traits can't work:
//
#  if defined(SPI_BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
      && !defined(SPI_BOOST_NO_STD_ITERATOR_TRAITS)
#     define SPI_BOOST_NO_STD_ITERATOR_TRAITS
#  endif

//
// Without partial specialization, partial
// specialization with default args won't work either:
//
#  if defined(SPI_BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) \
      && !defined(SPI_BOOST_NO_PARTIAL_SPECIALIZATION_IMPLICIT_DEFAULT_ARGS)
#     define SPI_BOOST_NO_PARTIAL_SPECIALIZATION_IMPLICIT_DEFAULT_ARGS
#  endif

//
// Without member template support, we can't have template constructors
// in the standard library either:
//
#  if defined(SPI_BOOST_NO_MEMBER_TEMPLATES) \
      && !defined(SPI_BOOST_MSVC6_MEMBER_TEMPLATES) \
      && !defined(SPI_BOOST_NO_TEMPLATED_ITERATOR_CONSTRUCTORS)
#     define SPI_BOOST_NO_TEMPLATED_ITERATOR_CONSTRUCTORS
#  endif

//
// Without member template support, we can't have a conforming
// std::allocator template either:
//
#  if defined(SPI_BOOST_NO_MEMBER_TEMPLATES) \
      && !defined(SPI_BOOST_MSVC6_MEMBER_TEMPLATES) \
      && !defined(SPI_BOOST_NO_STD_ALLOCATOR)
#     define SPI_BOOST_NO_STD_ALLOCATOR
#  endif

//
// without ADL support then using declarations will break ADL as well:
//
#if defined(SPI_BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP) && !defined(SPI_BOOST_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL)
#  define SPI_BOOST_FUNCTION_SCOPE_USING_DECLARATION_BREAKS_ADL
#endif

//
// Without typeid support we have no dynamic RTTI either:
//
#if defined(SPI_BOOST_NO_TYPEID) && !defined(SPI_BOOST_NO_RTTI)
#  define SPI_BOOST_NO_RTTI
#endif

//
// If we have a standard allocator, then we have a partial one as well:
//
#if !defined(SPI_BOOST_NO_STD_ALLOCATOR)
#  define SPI_BOOST_HAS_PARTIAL_STD_ALLOCATOR
#endif

//
// We can't have a working std::use_facet if there is no std::locale:
//
#  if defined(SPI_BOOST_NO_STD_LOCALE) && !defined(SPI_BOOST_NO_STD_USE_FACET)
#     define SPI_BOOST_NO_STD_USE_FACET
#  endif

//
// We can't have a std::messages facet if there is no std::locale:
//
#  if defined(SPI_BOOST_NO_STD_LOCALE) && !defined(SPI_BOOST_NO_STD_MESSAGES)
#     define SPI_BOOST_NO_STD_MESSAGES
#  endif

//
// We can't have a working std::wstreambuf if there is no std::locale:
//
#  if defined(SPI_BOOST_NO_STD_LOCALE) && !defined(SPI_BOOST_NO_STD_WSTREAMBUF)
#     define SPI_BOOST_NO_STD_WSTREAMBUF
#  endif

//
// We can't have a <cwctype> if there is no <cwchar>:
//
#  if defined(SPI_BOOST_NO_CWCHAR) && !defined(SPI_BOOST_NO_CWCTYPE)
#     define SPI_BOOST_NO_CWCTYPE
#  endif

//
// We can't have a swprintf if there is no <cwchar>:
//
#  if defined(SPI_BOOST_NO_CWCHAR) && !defined(SPI_BOOST_NO_SWPRINTF)
#     define SPI_BOOST_NO_SWPRINTF
#  endif

//
// If Win32 support is turned off, then we must turn off
// threading support also, unless there is some other
// thread API enabled:
//
#if defined(SPI_BOOST_DISABLE_WIN32) && defined(_WIN32) \
   && !defined(SPI_BOOST_DISABLE_THREADS) && !defined(SPI_BOOST_HAS_PTHREADS)
#  define SPI_BOOST_DISABLE_THREADS
#endif

//
// Turn on threading support if the compiler thinks that it's in
// multithreaded mode.  We put this here because there are only a
// limited number of macros that identify this (if there's any missing
// from here then add to the appropriate compiler section):
//
#if (defined(__MT__) || defined(_MT) || defined(_REENTRANT) \
    || defined(_PTHREADS) || defined(__APPLE__) || defined(__DragonFly__)) \
    && !defined(SPI_BOOST_HAS_THREADS)
#  define SPI_BOOST_HAS_THREADS
#endif

//
// Turn threading support off if SPI_BOOST_DISABLE_THREADS is defined:
//
#if defined(SPI_BOOST_DISABLE_THREADS) && defined(SPI_BOOST_HAS_THREADS)
#  undef SPI_BOOST_HAS_THREADS
#endif

//
// Turn threading support off if we don't recognise the threading API:
//
#if defined(SPI_BOOST_HAS_THREADS) && !defined(SPI_BOOST_HAS_PTHREADS)\
      && !defined(SPI_BOOST_HAS_WINTHREADS) && !defined(SPI_BOOST_HAS_BETHREADS)\
      && !defined(SPI_BOOST_HAS_MPTASKS)
#  undef SPI_BOOST_HAS_THREADS
#endif

//
// Turn threading detail macros off if we don't (want to) use threading
//
#ifndef SPI_BOOST_HAS_THREADS
#  undef SPI_BOOST_HAS_PTHREADS
#  undef SPI_BOOST_HAS_PTHREAD_MUTEXATTR_SETTYPE
#  undef SPI_BOOST_HAS_PTHREAD_YIELD
#  undef SPI_BOOST_HAS_PTHREAD_DELAY_NP
#  undef SPI_BOOST_HAS_WINTHREADS
#  undef SPI_BOOST_HAS_BETHREADS
#  undef SPI_BOOST_HAS_MPTASKS
#endif

//
// If the compiler claims to be C99 conformant, then it had better
// have a <stdint.h>:
//
#  if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#     define SPI_BOOST_HAS_STDINT_H
#     ifndef SPI_BOOST_HAS_LOG1P
#        define SPI_BOOST_HAS_LOG1P
#     endif
#     ifndef SPI_BOOST_HAS_EXPM1
#        define SPI_BOOST_HAS_EXPM1
#     endif
#  endif

//
// Define SPI_BOOST_NO_SLIST and SPI_BOOST_NO_HASH if required.
// Note that this is for backwards compatibility only.
//
#  if !defined(SPI_BOOST_HAS_SLIST) && !defined(SPI_BOOST_NO_SLIST)
#     define SPI_BOOST_NO_SLIST
#  endif

#  if !defined(SPI_BOOST_HAS_HASH) && !defined(SPI_BOOST_NO_HASH)
#     define SPI_BOOST_NO_HASH
#  endif

//
// Set SPI_BOOST_SLIST_HEADER if not set already:
//
#if defined(SPI_BOOST_HAS_SLIST) && !defined(SPI_BOOST_SLIST_HEADER)
#  define SPI_BOOST_SLIST_HEADER <slist>
#endif

//
// Set SPI_BOOST_HASH_SET_HEADER if not set already:
//
#if defined(SPI_BOOST_HAS_HASH) && !defined(SPI_BOOST_HASH_SET_HEADER)
#  define SPI_BOOST_HASH_SET_HEADER <hash_set>
#endif

//
// Set SPI_BOOST_HASH_MAP_HEADER if not set already:
//
#if defined(SPI_BOOST_HAS_HASH) && !defined(SPI_BOOST_HASH_MAP_HEADER)
#  define SPI_BOOST_HASH_MAP_HEADER <hash_map>
#endif

//  SPI_BOOST_HAS_ABI_HEADERS
//  This macro gets set if we have headers that fix the ABI,
//  and prevent ODR violations when linking to external libraries:
#if defined(SPI_BOOST_ABI_PREFIX) && defined(SPI_BOOST_ABI_SUFFIX) && !defined(SPI_BOOST_HAS_ABI_HEADERS)
#  define SPI_BOOST_HAS_ABI_HEADERS
#endif

#if defined(SPI_BOOST_HAS_ABI_HEADERS) && defined(SPI_BOOST_DISABLE_ABI_HEADERS)
#  undef SPI_BOOST_HAS_ABI_HEADERS
#endif

//  SPI_BOOST_NO_STDC_NAMESPACE workaround  --------------------------------------//
//  Because std::size_t usage is so common, even in spi_boost headers which do not
//  otherwise use the C library, the <cstddef> workaround is included here so
//  that ugly workaround code need not appear in many other spi_boost headers.
//  NOTE WELL: This is a workaround for non-conforming compilers; <cstddef>
//  must still be #included in the usual places so that <cstddef> inclusion
//  works as expected with standard conforming compilers.  The resulting
//  double inclusion of <cstddef> is harmless.

# if defined(SPI_BOOST_NO_STDC_NAMESPACE) && defined(__cplusplus)
#   include <cstddef>
    namespace std { using ::ptrdiff_t; using ::size_t; }
# endif

//  Workaround for the unfortunate min/max macros defined by some platform headers

#define SPI_BOOST_PREVENT_MACRO_SUBSTITUTION

#ifndef SPI_BOOST_USING_STD_MIN
#  define SPI_BOOST_USING_STD_MIN() using std::min
#endif

#ifndef SPI_BOOST_USING_STD_MAX
#  define SPI_BOOST_USING_STD_MAX() using std::max
#endif

//  SPI_BOOST_NO_STD_MIN_MAX workaround  -----------------------------------------//

#  if defined(SPI_BOOST_NO_STD_MIN_MAX) && defined(__cplusplus)

namespace std {
  template <class _Tp>
  inline const _Tp& min SPI_BOOST_PREVENT_MACRO_SUBSTITUTION (const _Tp& __a, const _Tp& __b) {
    return __b < __a ? __b : __a;
  }
  template <class _Tp>
  inline const _Tp& max SPI_BOOST_PREVENT_MACRO_SUBSTITUTION (const _Tp& __a, const _Tp& __b) {
    return  __a < __b ? __b : __a;
  }
}

#  endif

// SPI_BOOST_STATIC_CONSTANT workaround --------------------------------------- //
// On compilers which don't allow in-class initialization of static integral
// constant members, we must use enums as a workaround if we want the constants
// to be available at compile-time. This macro gives us a convenient way to
// declare such constants.

#  ifdef SPI_BOOST_NO_INCLASS_MEMBER_INITIALIZATION
#       define SPI_BOOST_STATIC_CONSTANT(type, assignment) enum { assignment }
#  else
#     define SPI_BOOST_STATIC_CONSTANT(type, assignment) static const type assignment
#  endif

// SPI_BOOST_USE_FACET / HAS_FACET workaround ----------------------------------//
// When the standard library does not have a conforming std::use_facet there
// are various workarounds available, but they differ from library to library.
// The same problem occurs with has_facet.
// These macros provide a consistent way to access a locale's facets.
// Usage:
//    replace
//       std::use_facet<Type>(loc);
//    with
//       SPI_BOOST_USE_FACET(Type, loc);
//    Note do not add a std:: prefix to the front of SPI_BOOST_USE_FACET!
//  Use for SPI_BOOST_HAS_FACET is analogous.

#if defined(SPI_BOOST_NO_STD_USE_FACET)
#  ifdef SPI_BOOST_HAS_TWO_ARG_USE_FACET
#     define SPI_BOOST_USE_FACET(Type, loc) std::use_facet(loc, static_cast<Type*>(0))
#     define SPI_BOOST_HAS_FACET(Type, loc) std::has_facet(loc, static_cast<Type*>(0))
#  elif defined(SPI_BOOST_HAS_MACRO_USE_FACET)
#     define SPI_BOOST_USE_FACET(Type, loc) std::_USE(loc, Type)
#     define SPI_BOOST_HAS_FACET(Type, loc) std::_HAS(loc, Type)
#  elif defined(SPI_BOOST_HAS_STLP_USE_FACET)
#     define SPI_BOOST_USE_FACET(Type, loc) (*std::_Use_facet<Type >(loc))
#     define SPI_BOOST_HAS_FACET(Type, loc) std::has_facet< Type >(loc)
#  endif
#else
#  define SPI_BOOST_USE_FACET(Type, loc) std::use_facet< Type >(loc)
#  define SPI_BOOST_HAS_FACET(Type, loc) std::has_facet< Type >(loc)
#endif

// SPI_BOOST_NESTED_TEMPLATE workaround ------------------------------------------//
// Member templates are supported by some compilers even though they can't use
// the A::template member<U> syntax, as a workaround replace:
//
// typedef typename A::template rebind<U> binder;
//
// with:
//
// typedef typename A::SPI_BOOST_NESTED_TEMPLATE rebind<U> binder;

#ifndef SPI_BOOST_NO_MEMBER_TEMPLATE_KEYWORD
#  define SPI_BOOST_NESTED_TEMPLATE template
#else
#  define SPI_BOOST_NESTED_TEMPLATE
#endif

// SPI_BOOST_UNREACHABLE_RETURN(x) workaround -------------------------------------//
// Normally evaluates to nothing, unless SPI_BOOST_NO_UNREACHABLE_RETURN_DETECTION
// is defined, in which case it evaluates to return x; Use when you have a return
// statement that can never be reached.

#ifndef SPI_BOOST_UNREACHABLE_RETURN
#  ifdef SPI_BOOST_NO_UNREACHABLE_RETURN_DETECTION
#     define SPI_BOOST_UNREACHABLE_RETURN(x) return x;
#  else
#     define SPI_BOOST_UNREACHABLE_RETURN(x)
#  endif
#endif

// SPI_BOOST_DEDUCED_TYPENAME workaround ------------------------------------------//
//
// Some compilers don't support the use of `typename' for dependent
// types in deduced contexts, e.g.
//
//     template <class T> void f(T, typename T::type);
//                                  ^^^^^^^^
// Replace these declarations with:
//
//     template <class T> void f(T, SPI_BOOST_DEDUCED_TYPENAME T::type);

#ifndef SPI_BOOST_NO_DEDUCED_TYPENAME
#  define SPI_BOOST_DEDUCED_TYPENAME typename
#else
#  define SPI_BOOST_DEDUCED_TYPENAME
#endif

#ifndef SPI_BOOST_NO_TYPENAME_WITH_CTOR
#  define SPI_BOOST_CTOR_TYPENAME typename
#else
#  define SPI_BOOST_CTOR_TYPENAME
#endif

// long long workaround ------------------------------------------//
// On gcc (and maybe other compilers?) long long is alway supported
// but it's use may generate either warnings (with -ansi), or errors
// (with -pedantic -ansi) unless it's use is prefixed by __extension__
//
#if defined(SPI_BOOST_HAS_LONG_LONG) && defined(__cplusplus)
namespace spi_boost{
#  ifdef __GNUC__
   __extension__ typedef long long long_long_type;
   __extension__ typedef unsigned long long ulong_long_type;
#  else
   typedef long long long_long_type;
   typedef unsigned long long ulong_long_type;
#  endif
}
#endif
// same again for __int128:
#if defined(SPI_BOOST_HAS_INT128) && defined(__cplusplus)
namespace spi_boost{
#  ifdef __GNUC__
   __extension__ typedef __int128 int128_type;
   __extension__ typedef unsigned __int128 uint128_type;
#  else
   typedef __int128 int128_type;
   typedef unsigned __int128 uint128_type;
#  endif
}
#endif
// same again for __float128:
#if defined(SPI_BOOST_HAS_FLOAT128) && defined(__cplusplus)
namespace spi_boost {
#  ifdef __GNUC__
   __extension__ typedef __float128 float128_type;
#  else
   typedef __float128 float128_type;
#  endif
}
#endif

// SPI_BOOST_[APPEND_]EXPLICIT_TEMPLATE_[NON_]TYPE macros --------------------------//

// These macros are obsolete. Port away and remove.

#  define SPI_BOOST_EXPLICIT_TEMPLATE_TYPE(t)
#  define SPI_BOOST_EXPLICIT_TEMPLATE_TYPE_SPEC(t)
#  define SPI_BOOST_EXPLICIT_TEMPLATE_NON_TYPE(t, v)
#  define SPI_BOOST_EXPLICIT_TEMPLATE_NON_TYPE_SPEC(t, v)

#  define SPI_BOOST_APPEND_EXPLICIT_TEMPLATE_TYPE(t)
#  define SPI_BOOST_APPEND_EXPLICIT_TEMPLATE_TYPE_SPEC(t)
#  define SPI_BOOST_APPEND_EXPLICIT_TEMPLATE_NON_TYPE(t, v)
#  define SPI_BOOST_APPEND_EXPLICIT_TEMPLATE_NON_TYPE_SPEC(t, v)

// When SPI_BOOST_NO_STD_TYPEINFO is defined, we can just import
// the global definition into std namespace:
#if defined(SPI_BOOST_NO_STD_TYPEINFO) && defined(__cplusplus)
#include <typeinfo>
namespace std{ using ::type_info; }
#endif

// ---------------------------------------------------------------------------//

// Helper macro SPI_BOOST_STRINGIZE:
// Helper macro SPI_BOOST_JOIN:

#include <spi_boost/config/helper_macros.hpp>

//
// Set some default values for compiler/library/platform names.
// These are for debugging config setup only:
//
#  ifndef SPI_BOOST_COMPILER
#     define SPI_BOOST_COMPILER "Unknown ISO C++ Compiler"
#  endif
#  ifndef SPI_BOOST_STDLIB
#     define SPI_BOOST_STDLIB "Unknown ISO standard library"
#  endif
#  ifndef SPI_BOOST_PLATFORM
#     if defined(unix) || defined(__unix) || defined(_XOPEN_SOURCE) \
         || defined(_POSIX_SOURCE)
#        define SPI_BOOST_PLATFORM "Generic Unix"
#     else
#        define SPI_BOOST_PLATFORM "Unknown"
#     endif
#  endif

//
// Set some default values GPU support
//
#  ifndef SPI_BOOST_GPU_ENABLED
#  define SPI_BOOST_GPU_ENABLED
#  endif

// SPI_BOOST_RESTRICT ---------------------------------------------//
// Macro to use in place of 'restrict' keyword variants
#if !defined(SPI_BOOST_RESTRICT)
#  if defined(_MSC_VER)
#    define SPI_BOOST_RESTRICT __restrict
#    if !defined(SPI_BOOST_NO_RESTRICT_REFERENCES) && (_MSC_FULL_VER < 190023026)
#      define SPI_BOOST_NO_RESTRICT_REFERENCES
#    endif
#  elif defined(__GNUC__) && __GNUC__ > 3
     // Clang also defines __GNUC__ (as 4)
#    define SPI_BOOST_RESTRICT __restrict__
#  else
#    define SPI_BOOST_RESTRICT
#    if !defined(SPI_BOOST_NO_RESTRICT_REFERENCES)
#      define SPI_BOOST_NO_RESTRICT_REFERENCES
#    endif
#  endif
#endif

// SPI_BOOST_MAY_ALIAS -----------------------------------------------//
// The macro expands to an attribute to mark a type that is allowed to alias other types.
// The macro is defined in the compiler-specific headers.
#if !defined(SPI_BOOST_MAY_ALIAS)
#  define SPI_BOOST_NO_MAY_ALIAS
#  define SPI_BOOST_MAY_ALIAS
#endif

// SPI_BOOST_FORCEINLINE ---------------------------------------------//
// Macro to use in place of 'inline' to force a function to be inline
#if !defined(SPI_BOOST_FORCEINLINE)
#  if defined(_MSC_VER)
#    define SPI_BOOST_FORCEINLINE __forceinline
#  elif defined(__GNUC__) && __GNUC__ > 3
     // Clang also defines __GNUC__ (as 4)
#    define SPI_BOOST_FORCEINLINE inline __attribute__ ((__always_inline__))
#  else
#    define SPI_BOOST_FORCEINLINE inline
#  endif
#endif

// SPI_BOOST_NOINLINE ---------------------------------------------//
// Macro to use in place of 'inline' to prevent a function to be inlined
#if !defined(SPI_BOOST_NOINLINE)
#  if defined(_MSC_VER)
#    define SPI_BOOST_NOINLINE __declspec(noinline)
#  elif defined(__GNUC__) && __GNUC__ > 3
     // Clang also defines __GNUC__ (as 4)
#    if defined(__CUDACC__)
       // nvcc doesn't always parse __noinline__,
       // see: https://svn.spi_boost.org/trac/spi_boost/ticket/9392
#      define SPI_BOOST_NOINLINE __attribute__ ((noinline))
#    else
#      define SPI_BOOST_NOINLINE __attribute__ ((__noinline__))
#    endif
#  else
#    define SPI_BOOST_NOINLINE
#  endif
#endif

// SPI_BOOST_NORETURN ---------------------------------------------//
// Macro to use before a function declaration/definition to designate
// the function as not returning normally (i.e. with a return statement
// or by leaving the function scope, if the function return type is void).
#if !defined(SPI_BOOST_NORETURN)
#  if defined(_MSC_VER)
#    define SPI_BOOST_NORETURN __declspec(noreturn)
#  elif defined(__GNUC__)
#    define SPI_BOOST_NORETURN __attribute__ ((__noreturn__))
#  elif defined(__has_attribute) && defined(__SUNPRO_CC) && (__SUNPRO_CC > 0x5130)
#    if __has_attribute(noreturn)
#      define SPI_BOOST_NORETURN [[noreturn]]
#    endif
#  elif defined(__has_cpp_attribute) 
#    if __has_cpp_attribute(noreturn)
#      define SPI_BOOST_NORETURN [[noreturn]]
#    endif
#  endif
#endif

#if !defined(SPI_BOOST_NORETURN)
#  define SPI_BOOST_NO_NORETURN
#  define SPI_BOOST_NORETURN
#endif

// Branch prediction hints
// These macros are intended to wrap conditional expressions that yield true or false
//
//  if (SPI_BOOST_LIKELY(var == 10))
//  {
//     // the most probable code here
//  }
//
#if !defined(SPI_BOOST_LIKELY)
#  define SPI_BOOST_LIKELY(x) x
#endif
#if !defined(SPI_BOOST_UNLIKELY)
#  define SPI_BOOST_UNLIKELY(x) x
#endif

// Type and data alignment specification
//
#if !defined(SPI_BOOST_ALIGNMENT)
#  if !defined(SPI_BOOST_NO_CXX11_ALIGNAS)
#    define SPI_BOOST_ALIGNMENT(x) alignas(x)
#  elif defined(_MSC_VER)
#    define SPI_BOOST_ALIGNMENT(x) __declspec(align(x))
#  elif defined(__GNUC__)
#    define SPI_BOOST_ALIGNMENT(x) __attribute__ ((__aligned__(x)))
#  else
#    define SPI_BOOST_NO_ALIGNMENT
#    define SPI_BOOST_ALIGNMENT(x)
#  endif
#endif

// Lack of non-public defaulted functions is implied by the lack of any defaulted functions
#if !defined(SPI_BOOST_NO_CXX11_NON_PUBLIC_DEFAULTED_FUNCTIONS) && defined(SPI_BOOST_NO_CXX11_DEFAULTED_FUNCTIONS)
#  define SPI_BOOST_NO_CXX11_NON_PUBLIC_DEFAULTED_FUNCTIONS
#endif

// Lack of defaulted moves is implied by the lack of either rvalue references or any defaulted functions
#if !defined(SPI_BOOST_NO_CXX11_DEFAULTED_MOVES) && (defined(SPI_BOOST_NO_CXX11_DEFAULTED_FUNCTIONS) || defined(SPI_BOOST_NO_CXX11_RVALUE_REFERENCES))
#  define SPI_BOOST_NO_CXX11_DEFAULTED_MOVES
#endif

// Defaulted and deleted function declaration helpers
// These macros are intended to be inside a class definition.
// SPI_BOOST_DEFAULTED_FUNCTION accepts the function declaration and its
// body, which will be used if the compiler doesn't support defaulted functions.
// SPI_BOOST_DELETED_FUNCTION only accepts the function declaration. It
// will expand to a private function declaration, if the compiler doesn't support
// deleted functions. Because of this it is recommended to use SPI_BOOST_DELETED_FUNCTION
// in the end of the class definition.
//
//  class my_class
//  {
//  public:
//      // Default-constructible
//      SPI_BOOST_DEFAULTED_FUNCTION(my_class(), {})
//      // Copying prohibited
//      SPI_BOOST_DELETED_FUNCTION(my_class(my_class const&))
//      SPI_BOOST_DELETED_FUNCTION(my_class& operator= (my_class const&))
//  };
//
#if !(defined(SPI_BOOST_NO_CXX11_DEFAULTED_FUNCTIONS) || defined(SPI_BOOST_NO_CXX11_NON_PUBLIC_DEFAULTED_FUNCTIONS))
#   define SPI_BOOST_DEFAULTED_FUNCTION(fun, body) fun = default;
#else
#   define SPI_BOOST_DEFAULTED_FUNCTION(fun, body) fun body
#endif

#if !defined(SPI_BOOST_NO_CXX11_DELETED_FUNCTIONS)
#   define SPI_BOOST_DELETED_FUNCTION(fun) fun = delete;
#else
#   define SPI_BOOST_DELETED_FUNCTION(fun) private: fun;
#endif

//
// Set SPI_BOOST_NO_DECLTYPE_N3276 when SPI_BOOST_NO_DECLTYPE is defined
//
#if defined(SPI_BOOST_NO_CXX11_DECLTYPE) && !defined(SPI_BOOST_NO_CXX11_DECLTYPE_N3276)
#define SPI_BOOST_NO_CXX11_DECLTYPE_N3276 SPI_BOOST_NO_CXX11_DECLTYPE
#endif

//  -------------------- Deprecated macros for 1.50 ---------------------------
//  These will go away in a future release

//  Use SPI_BOOST_NO_CXX11_HDR_UNORDERED_SET or SPI_BOOST_NO_CXX11_HDR_UNORDERED_MAP
//           instead of SPI_BOOST_NO_STD_UNORDERED
#if defined(SPI_BOOST_NO_CXX11_HDR_UNORDERED_MAP) || defined (SPI_BOOST_NO_CXX11_HDR_UNORDERED_SET)
# ifndef SPI_BOOST_NO_CXX11_STD_UNORDERED
#  define SPI_BOOST_NO_CXX11_STD_UNORDERED
# endif
#endif

//  Use SPI_BOOST_NO_CXX11_HDR_INITIALIZER_LIST instead of SPI_BOOST_NO_INITIALIZER_LISTS
#if defined(SPI_BOOST_NO_CXX11_HDR_INITIALIZER_LIST) && !defined(SPI_BOOST_NO_INITIALIZER_LISTS)
#  define SPI_BOOST_NO_INITIALIZER_LISTS
#endif

//  Use SPI_BOOST_NO_CXX11_HDR_ARRAY instead of SPI_BOOST_NO_0X_HDR_ARRAY
#if defined(SPI_BOOST_NO_CXX11_HDR_ARRAY) && !defined(SPI_BOOST_NO_0X_HDR_ARRAY)
#  define SPI_BOOST_NO_0X_HDR_ARRAY
#endif
//  Use SPI_BOOST_NO_CXX11_HDR_CHRONO instead of SPI_BOOST_NO_0X_HDR_CHRONO
#if defined(SPI_BOOST_NO_CXX11_HDR_CHRONO) && !defined(SPI_BOOST_NO_0X_HDR_CHRONO)
#  define SPI_BOOST_NO_0X_HDR_CHRONO
#endif
//  Use SPI_BOOST_NO_CXX11_HDR_CODECVT instead of SPI_BOOST_NO_0X_HDR_CODECVT
#if defined(SPI_BOOST_NO_CXX11_HDR_CODECVT) && !defined(SPI_BOOST_NO_0X_HDR_CODECVT)
#  define SPI_BOOST_NO_0X_HDR_CODECVT
#endif
//  Use SPI_BOOST_NO_CXX11_HDR_CONDITION_VARIABLE instead of SPI_BOOST_NO_0X_HDR_CONDITION_VARIABLE
#if defined(SPI_BOOST_NO_CXX11_HDR_CONDITION_VARIABLE) && !defined(SPI_BOOST_NO_0X_HDR_CONDITION_VARIABLE)
#  define SPI_BOOST_NO_0X_HDR_CONDITION_VARIABLE
#endif
//  Use SPI_BOOST_NO_CXX11_HDR_FORWARD_LIST instead of SPI_BOOST_NO_0X_HDR_FORWARD_LIST
#if defined(SPI_BOOST_NO_CXX11_HDR_FORWARD_LIST) && !defined(SPI_BOOST_NO_0X_HDR_FORWARD_LIST)
#  define SPI_BOOST_NO_0X_HDR_FORWARD_LIST
#endif
//  Use SPI_BOOST_NO_CXX11_HDR_FUTURE instead of SPI_BOOST_NO_0X_HDR_FUTURE
#if defined(SPI_BOOST_NO_CXX11_HDR_FUTURE) && !defined(SPI_BOOST_NO_0X_HDR_FUTURE)
#  define SPI_BOOST_NO_0X_HDR_FUTURE
#endif

//  Use SPI_BOOST_NO_CXX11_HDR_INITIALIZER_LIST
//  instead of SPI_BOOST_NO_0X_HDR_INITIALIZER_LIST or SPI_BOOST_NO_INITIALIZER_LISTS
#ifdef SPI_BOOST_NO_CXX11_HDR_INITIALIZER_LIST
# ifndef SPI_BOOST_NO_0X_HDR_INITIALIZER_LIST
#  define SPI_BOOST_NO_0X_HDR_INITIALIZER_LIST
# endif
# ifndef SPI_BOOST_NO_INITIALIZER_LISTS
#  define SPI_BOOST_NO_INITIALIZER_LISTS
# endif
#endif

//  Use SPI_BOOST_NO_CXX11_HDR_MUTEX instead of SPI_BOOST_NO_0X_HDR_MUTEX
#if defined(SPI_BOOST_NO_CXX11_HDR_MUTEX) && !defined(SPI_BOOST_NO_0X_HDR_MUTEX)
#  define SPI_BOOST_NO_0X_HDR_MUTEX
#endif
//  Use SPI_BOOST_NO_CXX11_HDR_RANDOM instead of SPI_BOOST_NO_0X_HDR_RANDOM
#if defined(SPI_BOOST_NO_CXX11_HDR_RANDOM) && !defined(SPI_BOOST_NO_0X_HDR_RANDOM)
#  define SPI_BOOST_NO_0X_HDR_RANDOM
#endif
//  Use SPI_BOOST_NO_CXX11_HDR_RATIO instead of SPI_BOOST_NO_0X_HDR_RATIO
#if defined(SPI_BOOST_NO_CXX11_HDR_RATIO) && !defined(SPI_BOOST_NO_0X_HDR_RATIO)
#  define SPI_BOOST_NO_0X_HDR_RATIO
#endif
//  Use SPI_BOOST_NO_CXX11_HDR_REGEX instead of SPI_BOOST_NO_0X_HDR_REGEX
#if defined(SPI_BOOST_NO_CXX11_HDR_REGEX) && !defined(SPI_BOOST_NO_0X_HDR_REGEX)
#  define SPI_BOOST_NO_0X_HDR_REGEX
#endif
//  Use SPI_BOOST_NO_CXX11_HDR_SYSTEM_ERROR instead of SPI_BOOST_NO_0X_HDR_SYSTEM_ERROR
#if defined(SPI_BOOST_NO_CXX11_HDR_SYSTEM_ERROR) && !defined(SPI_BOOST_NO_0X_HDR_SYSTEM_ERROR)
#  define SPI_BOOST_NO_0X_HDR_SYSTEM_ERROR
#endif
//  Use SPI_BOOST_NO_CXX11_HDR_THREAD instead of SPI_BOOST_NO_0X_HDR_THREAD
#if defined(SPI_BOOST_NO_CXX11_HDR_THREAD) && !defined(SPI_BOOST_NO_0X_HDR_THREAD)
#  define SPI_BOOST_NO_0X_HDR_THREAD
#endif
//  Use SPI_BOOST_NO_CXX11_HDR_TUPLE instead of SPI_BOOST_NO_0X_HDR_TUPLE
#if defined(SPI_BOOST_NO_CXX11_HDR_TUPLE) && !defined(SPI_BOOST_NO_0X_HDR_TUPLE)
#  define SPI_BOOST_NO_0X_HDR_TUPLE
#endif
//  Use SPI_BOOST_NO_CXX11_HDR_TYPE_TRAITS instead of SPI_BOOST_NO_0X_HDR_TYPE_TRAITS
#if defined(SPI_BOOST_NO_CXX11_HDR_TYPE_TRAITS) && !defined(SPI_BOOST_NO_0X_HDR_TYPE_TRAITS)
#  define SPI_BOOST_NO_0X_HDR_TYPE_TRAITS
#endif
//  Use SPI_BOOST_NO_CXX11_HDR_TYPEINDEX instead of SPI_BOOST_NO_0X_HDR_TYPEINDEX
#if defined(SPI_BOOST_NO_CXX11_HDR_TYPEINDEX) && !defined(SPI_BOOST_NO_0X_HDR_TYPEINDEX)
#  define SPI_BOOST_NO_0X_HDR_TYPEINDEX
#endif
//  Use SPI_BOOST_NO_CXX11_HDR_UNORDERED_MAP instead of SPI_BOOST_NO_0X_HDR_UNORDERED_MAP
#if defined(SPI_BOOST_NO_CXX11_HDR_UNORDERED_MAP) && !defined(SPI_BOOST_NO_0X_HDR_UNORDERED_MAP)
#  define SPI_BOOST_NO_0X_HDR_UNORDERED_MAP
#endif
//  Use SPI_BOOST_NO_CXX11_HDR_UNORDERED_SET instead of SPI_BOOST_NO_0X_HDR_UNORDERED_SET
#if defined(SPI_BOOST_NO_CXX11_HDR_UNORDERED_SET) && !defined(SPI_BOOST_NO_0X_HDR_UNORDERED_SET)
#  define SPI_BOOST_NO_0X_HDR_UNORDERED_SET
#endif

//  ------------------ End of deprecated macros for 1.50 ---------------------------

//  -------------------- Deprecated macros for 1.51 ---------------------------
//  These will go away in a future release

//  Use     SPI_BOOST_NO_CXX11_AUTO_DECLARATIONS instead of   SPI_BOOST_NO_AUTO_DECLARATIONS
#if defined(SPI_BOOST_NO_CXX11_AUTO_DECLARATIONS) && !defined(SPI_BOOST_NO_AUTO_DECLARATIONS)
#  define SPI_BOOST_NO_AUTO_DECLARATIONS
#endif
//  Use     SPI_BOOST_NO_CXX11_AUTO_MULTIDECLARATIONS instead of   SPI_BOOST_NO_AUTO_MULTIDECLARATIONS
#if defined(SPI_BOOST_NO_CXX11_AUTO_MULTIDECLARATIONS) && !defined(SPI_BOOST_NO_AUTO_MULTIDECLARATIONS)
#  define SPI_BOOST_NO_AUTO_MULTIDECLARATIONS
#endif
//  Use     SPI_BOOST_NO_CXX11_CHAR16_T instead of   SPI_BOOST_NO_CHAR16_T
#if defined(SPI_BOOST_NO_CXX11_CHAR16_T) && !defined(SPI_BOOST_NO_CHAR16_T)
#  define SPI_BOOST_NO_CHAR16_T
#endif
//  Use     SPI_BOOST_NO_CXX11_CHAR32_T instead of   SPI_BOOST_NO_CHAR32_T
#if defined(SPI_BOOST_NO_CXX11_CHAR32_T) && !defined(SPI_BOOST_NO_CHAR32_T)
#  define SPI_BOOST_NO_CHAR32_T
#endif
//  Use     SPI_BOOST_NO_CXX11_TEMPLATE_ALIASES instead of   SPI_BOOST_NO_TEMPLATE_ALIASES
#if defined(SPI_BOOST_NO_CXX11_TEMPLATE_ALIASES) && !defined(SPI_BOOST_NO_TEMPLATE_ALIASES)
#  define SPI_BOOST_NO_TEMPLATE_ALIASES
#endif
//  Use     SPI_BOOST_NO_CXX11_CONSTEXPR instead of   SPI_BOOST_NO_CONSTEXPR
#if defined(SPI_BOOST_NO_CXX11_CONSTEXPR) && !defined(SPI_BOOST_NO_CONSTEXPR)
#  define SPI_BOOST_NO_CONSTEXPR
#endif
//  Use     SPI_BOOST_NO_CXX11_DECLTYPE_N3276 instead of   SPI_BOOST_NO_DECLTYPE_N3276
#if defined(SPI_BOOST_NO_CXX11_DECLTYPE_N3276) && !defined(SPI_BOOST_NO_DECLTYPE_N3276)
#  define SPI_BOOST_NO_DECLTYPE_N3276
#endif
//  Use     SPI_BOOST_NO_CXX11_DECLTYPE instead of   SPI_BOOST_NO_DECLTYPE
#if defined(SPI_BOOST_NO_CXX11_DECLTYPE) && !defined(SPI_BOOST_NO_DECLTYPE)
#  define SPI_BOOST_NO_DECLTYPE
#endif
//  Use     SPI_BOOST_NO_CXX11_DEFAULTED_FUNCTIONS instead of   SPI_BOOST_NO_DEFAULTED_FUNCTIONS
#if defined(SPI_BOOST_NO_CXX11_DEFAULTED_FUNCTIONS) && !defined(SPI_BOOST_NO_DEFAULTED_FUNCTIONS)
#  define SPI_BOOST_NO_DEFAULTED_FUNCTIONS
#endif
//  Use     SPI_BOOST_NO_CXX11_DELETED_FUNCTIONS instead of   SPI_BOOST_NO_DELETED_FUNCTIONS
#if defined(SPI_BOOST_NO_CXX11_DELETED_FUNCTIONS) && !defined(SPI_BOOST_NO_DELETED_FUNCTIONS)
#  define SPI_BOOST_NO_DELETED_FUNCTIONS
#endif
//  Use     SPI_BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS instead of   SPI_BOOST_NO_EXPLICIT_CONVERSION_OPERATORS
#if defined(SPI_BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS) && !defined(SPI_BOOST_NO_EXPLICIT_CONVERSION_OPERATORS)
#  define SPI_BOOST_NO_EXPLICIT_CONVERSION_OPERATORS
#endif
//  Use     SPI_BOOST_NO_CXX11_EXTERN_TEMPLATE instead of   SPI_BOOST_NO_EXTERN_TEMPLATE
#if defined(SPI_BOOST_NO_CXX11_EXTERN_TEMPLATE) && !defined(SPI_BOOST_NO_EXTERN_TEMPLATE)
#  define SPI_BOOST_NO_EXTERN_TEMPLATE
#endif
//  Use     SPI_BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS instead of   SPI_BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
#if defined(SPI_BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS) && !defined(SPI_BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS)
#  define SPI_BOOST_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
#endif
//  Use     SPI_BOOST_NO_CXX11_LAMBDAS instead of   SPI_BOOST_NO_LAMBDAS
#if defined(SPI_BOOST_NO_CXX11_LAMBDAS) && !defined(SPI_BOOST_NO_LAMBDAS)
#  define SPI_BOOST_NO_LAMBDAS
#endif
//  Use     SPI_BOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS instead of   SPI_BOOST_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS
#if defined(SPI_BOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS) && !defined(SPI_BOOST_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS)
#  define SPI_BOOST_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS
#endif
//  Use     SPI_BOOST_NO_CXX11_NOEXCEPT instead of   SPI_BOOST_NO_NOEXCEPT
#if defined(SPI_BOOST_NO_CXX11_NOEXCEPT) && !defined(SPI_BOOST_NO_NOEXCEPT)
#  define SPI_BOOST_NO_NOEXCEPT
#endif
//  Use     SPI_BOOST_NO_CXX11_NULLPTR instead of   SPI_BOOST_NO_NULLPTR
#if defined(SPI_BOOST_NO_CXX11_NULLPTR) && !defined(SPI_BOOST_NO_NULLPTR)
#  define SPI_BOOST_NO_NULLPTR
#endif
//  Use     SPI_BOOST_NO_CXX11_RAW_LITERALS instead of   SPI_BOOST_NO_RAW_LITERALS
#if defined(SPI_BOOST_NO_CXX11_RAW_LITERALS) && !defined(SPI_BOOST_NO_RAW_LITERALS)
#  define SPI_BOOST_NO_RAW_LITERALS
#endif
//  Use     SPI_BOOST_NO_CXX11_RVALUE_REFERENCES instead of   SPI_BOOST_NO_RVALUE_REFERENCES
#if defined(SPI_BOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(SPI_BOOST_NO_RVALUE_REFERENCES)
#  define SPI_BOOST_NO_RVALUE_REFERENCES
#endif
//  Use     SPI_BOOST_NO_CXX11_SCOPED_ENUMS instead of   SPI_BOOST_NO_SCOPED_ENUMS
#if defined(SPI_BOOST_NO_CXX11_SCOPED_ENUMS) && !defined(SPI_BOOST_NO_SCOPED_ENUMS)
#  define SPI_BOOST_NO_SCOPED_ENUMS
#endif
//  Use     SPI_BOOST_NO_CXX11_STATIC_ASSERT instead of   SPI_BOOST_NO_STATIC_ASSERT
#if defined(SPI_BOOST_NO_CXX11_STATIC_ASSERT) && !defined(SPI_BOOST_NO_STATIC_ASSERT)
#  define SPI_BOOST_NO_STATIC_ASSERT
#endif
//  Use     SPI_BOOST_NO_CXX11_STD_UNORDERED instead of   SPI_BOOST_NO_STD_UNORDERED
#if defined(SPI_BOOST_NO_CXX11_STD_UNORDERED) && !defined(SPI_BOOST_NO_STD_UNORDERED)
#  define SPI_BOOST_NO_STD_UNORDERED
#endif
//  Use     SPI_BOOST_NO_CXX11_UNICODE_LITERALS instead of   SPI_BOOST_NO_UNICODE_LITERALS
#if defined(SPI_BOOST_NO_CXX11_UNICODE_LITERALS) && !defined(SPI_BOOST_NO_UNICODE_LITERALS)
#  define SPI_BOOST_NO_UNICODE_LITERALS
#endif
//  Use     SPI_BOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX instead of   SPI_BOOST_NO_UNIFIED_INITIALIZATION_SYNTAX
#if defined(SPI_BOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX) && !defined(SPI_BOOST_NO_UNIFIED_INITIALIZATION_SYNTAX)
#  define SPI_BOOST_NO_UNIFIED_INITIALIZATION_SYNTAX
#endif
//  Use     SPI_BOOST_NO_CXX11_VARIADIC_TEMPLATES instead of   SPI_BOOST_NO_VARIADIC_TEMPLATES
#if defined(SPI_BOOST_NO_CXX11_VARIADIC_TEMPLATES) && !defined(SPI_BOOST_NO_VARIADIC_TEMPLATES)
#  define SPI_BOOST_NO_VARIADIC_TEMPLATES
#endif
//  Use     SPI_BOOST_NO_CXX11_VARIADIC_MACROS instead of   SPI_BOOST_NO_VARIADIC_MACROS
#if defined(SPI_BOOST_NO_CXX11_VARIADIC_MACROS) && !defined(SPI_BOOST_NO_VARIADIC_MACROS)
#  define SPI_BOOST_NO_VARIADIC_MACROS
#endif
//  Use     SPI_BOOST_NO_CXX11_NUMERIC_LIMITS instead of   SPI_BOOST_NO_NUMERIC_LIMITS_LOWEST
#if defined(SPI_BOOST_NO_CXX11_NUMERIC_LIMITS) && !defined(SPI_BOOST_NO_NUMERIC_LIMITS_LOWEST)
#  define SPI_BOOST_NO_NUMERIC_LIMITS_LOWEST
#endif
//  ------------------ End of deprecated macros for 1.51 ---------------------------



//
// Helper macros SPI_BOOST_NOEXCEPT, SPI_BOOST_NOEXCEPT_IF, SPI_BOOST_NOEXCEPT_EXPR
// These aid the transition to C++11 while still supporting C++03 compilers
//
#ifdef SPI_BOOST_NO_CXX11_NOEXCEPT
#  define SPI_BOOST_NOEXCEPT
#  define SPI_BOOST_NOEXCEPT_OR_NOTHROW throw()
#  define SPI_BOOST_NOEXCEPT_IF(Predicate)
#  define SPI_BOOST_NOEXCEPT_EXPR(Expression) false
#else
#  define SPI_BOOST_NOEXCEPT noexcept
#  define SPI_BOOST_NOEXCEPT_OR_NOTHROW noexcept
#  define SPI_BOOST_NOEXCEPT_IF(Predicate) noexcept((Predicate))
#  define SPI_BOOST_NOEXCEPT_EXPR(Expression) noexcept((Expression))
#endif
//
// Helper macro SPI_BOOST_FALLTHROUGH
// Fallback definition of SPI_BOOST_FALLTHROUGH macro used to mark intended
// fall-through between case labels in a switch statement. We use a definition
// that requires a semicolon after it to avoid at least one type of misuse even
// on unsupported compilers.
//
#ifndef SPI_BOOST_FALLTHROUGH
#  define SPI_BOOST_FALLTHROUGH ((void)0)
#endif

//
// constexpr workarounds
//
#if defined(SPI_BOOST_NO_CXX11_CONSTEXPR)
#define SPI_BOOST_CONSTEXPR
#define SPI_BOOST_CONSTEXPR_OR_CONST const
#else
#define SPI_BOOST_CONSTEXPR constexpr
#define SPI_BOOST_CONSTEXPR_OR_CONST constexpr
#endif
#if defined(SPI_BOOST_NO_CXX14_CONSTEXPR)
#define SPI_BOOST_CXX14_CONSTEXPR
#else
#define SPI_BOOST_CXX14_CONSTEXPR constexpr
#endif

//
// Unused variable/typedef workarounds:
//
#ifndef SPI_BOOST_ATTRIBUTE_UNUSED
#  define SPI_BOOST_ATTRIBUTE_UNUSED
#endif

#define SPI_BOOST_STATIC_CONSTEXPR  static SPI_BOOST_CONSTEXPR_OR_CONST

//
// Set SPI_BOOST_HAS_STATIC_ASSERT when SPI_BOOST_NO_CXX11_STATIC_ASSERT is not defined
//
#if !defined(SPI_BOOST_NO_CXX11_STATIC_ASSERT) && !defined(SPI_BOOST_HAS_STATIC_ASSERT)
#  define SPI_BOOST_HAS_STATIC_ASSERT
#endif

//
// Set SPI_BOOST_HAS_RVALUE_REFS when SPI_BOOST_NO_CXX11_RVALUE_REFERENCES is not defined
//
#if !defined(SPI_BOOST_NO_CXX11_RVALUE_REFERENCES) && !defined(SPI_BOOST_HAS_RVALUE_REFS)
#define SPI_BOOST_HAS_RVALUE_REFS
#endif

//
// Set SPI_BOOST_HAS_VARIADIC_TMPL when SPI_BOOST_NO_CXX11_VARIADIC_TEMPLATES is not defined
//
#if !defined(SPI_BOOST_NO_CXX11_VARIADIC_TEMPLATES) && !defined(SPI_BOOST_HAS_VARIADIC_TMPL)
#define SPI_BOOST_HAS_VARIADIC_TMPL
#endif
//
// Set SPI_BOOST_NO_CXX11_FIXED_LENGTH_VARIADIC_TEMPLATE_EXPANSION_PACKS when
// SPI_BOOST_NO_CXX11_VARIADIC_TEMPLATES is set:
//
#if defined(SPI_BOOST_NO_CXX11_VARIADIC_TEMPLATES) && !defined(SPI_BOOST_NO_CXX11_FIXED_LENGTH_VARIADIC_TEMPLATE_EXPANSION_PACKS)
#  define SPI_BOOST_NO_CXX11_FIXED_LENGTH_VARIADIC_TEMPLATE_EXPANSION_PACKS
#endif

// This is a catch all case for obsolete compilers / std libs:
#if !defined(__has_include)
#  define SPI_BOOST_NO_CXX17_HDR_OPTIONAL
#  define SPI_BOOST_NO_CXX17_HDR_STRING_VIEW
#else
#if !__has_include(<optional>)
#  define SPI_BOOST_NO_CXX17_HDR_OPTIONAL
#endif
#if !__has_include(<string_view>)
#  define SPI_BOOST_NO_CXX17_HDR_STRING_VIEW
#endif
#endif

//
// Finish off with checks for macros that are depricated / no longer supported,
// if any of these are set then it's very likely that much of Boost will no
// longer work.  So stop with a #error for now, but give the user a chance
// to continue at their own risk if they really want to:
//
#if defined(SPI_BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && !defined(SPI_BOOST_CONFIG_ALLOW_DEPRECATED)
#  error "You are using a compiler which lacks features which are now a minimum requirement in order to use Boost, define SPI_BOOST_CONFIG_ALLOW_DEPRECATED if you want to continue at your own risk!!!"
#endif

#endif
