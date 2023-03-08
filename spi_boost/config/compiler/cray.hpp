//  Copyright 2011 John Maddock
//  Copyright 2013, 2017-2018 Cray, Inc.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

// Cray C++ compiler setup.
//
// There are a few parameters that affect the macros defined in this file:
//
// - What version of CCE (Cray Compiling Environment) are we running? This
//   comes from the '_RELEASE_MAJOR', '_RELEASE_MINOR', and
//   '_RELEASE_PATCHLEVEL' macros.
// - What C++ standards conformance level are we using (e.g. '-h
//   std=c++14')? This comes from the '__cplusplus' macro.
// - Are we using GCC extensions ('-h gnu' or '-h nognu')? If we have '-h
//   gnu' then CCE emulates GCC, and the macros '__GNUC__',
//   '__GNUC_MINOR__', and '__GNUC_PATCHLEVEL__' are defined.
//
// This file is organized as follows:
//
// - Verify that the combination of parameters listed above is supported.
//   If we have an unsupported combination, we abort with '#error'.
// - Establish baseline values for all Boost macros.
// - Apply changes to the baseline macros based on compiler version. These
//   changes are cummulative so each version section only describes the
//   changes since the previous version.
//   - Within each version section, we may also apply changes based on
//     other parameters (i.e. C++ standards conformance level and GCC
//     extensions).
//
// To test changes to this file:
//
// ```
// module load cce/8.6.5 # Pick the version you want to test.
// cd spi_boost/libs/config/test/all
// b2 -j 8 toolset=cray cxxstd=03 cxxstd=11 cxxstd=14 cxxstd-dialect=gnu linkflags=-lrt
// ```
// Note: Using 'cxxstd-dialect=iso' is not supported at this time (the
// tests run, but many tests fail).
//
// Note: 'linkflags=-lrt' is needed in Cray Linux Environment. Otherwise
// you get an 'undefined reference to clock_gettime' error.
//
// Note: If a test '*_fail.cpp' file compiles, but fails to run, then it is
// reported as a defect. However, this is not actually a defect. This is an
// area where the test system is somewhat broken. Tests that are failing
// because of this problem are noted in the comments.
//
// Pay attention to the macro definitions for the macros you wish to
// modify. For example, only macros categorized as compiler macros should
// appear in this file; platform macros should not appear in this file.
// Also, some macros have to be defined to specific values; it is not
// always enough to define or undefine a macro.
//
// Macro definitions are available in the source code at:
//
// `spi_boost/libs/config/doc/html/spi_boost_config/spi_boost_macro_reference.html`
//
// Macro definitions are also available online at:
//
// http://www.boost.org/doc/libs/master/libs/config/doc/html/spi_boost_config/spi_boost_macro_reference.html
//
// Typically, if you enable a feature, and the tests pass, then you have
// nothing to worry about. However, it's sometimes hard to figure out if a
// disabled feature needs to stay disabled. To get a list of disabled
// features, run 'b2' in 'spi_boost/libs/config/checks'. These are the macros
// you should pay attention to (in addition to macros that cause test
// failures).

////
//// Front matter
////

// In a developer build of the Cray compiler (i.e. a compiler built by a
// Cray employee), the release patch level is reported as "x". This gives
// versions that look like e.g. "8.6.x".
//
// To accomplish this, the the Cray compiler preprocessor inserts:
//
// #define _RELEASE_PATCHLEVEL x
//
// If we are using a developer build of the compiler, we want to use the
// configuration macros for the most recent patch level of the release. To
// accomplish this, we'll pretend that _RELEASE_PATCHLEVEL is 99.
//
// However, it's difficult to detect if _RELEASE_PATCHLEVEL is x. We must
// consider that the x will be expanded if x is defined as a macro
// elsewhere. For example, imagine if someone put "-D x=3" on the command
// line, and _RELEASE_PATCHLEVEL is x. Then _RELEASE_PATCHLEVEL would
// expand to 3, and we could not distinguish it from an actual
// _RELEASE_PATCHLEVEL of 3. This problem only affects developer builds; in
// production builds, _RELEASE_PATCHLEVEL is always an integer.
//
// IMPORTANT: In developer builds, if x is defined as a macro, you will get
// an incorrect configuration. The behavior in this case is undefined.
//
// Even if x is not defined, we have to use some trickery to detect if
// _RELEASE_PATCHLEVEL is x. First we define SPI_BOOST_CRAY_x to some arbitrary
// magic value, 9867657. Then we use SPI_BOOST_CRAY_APPEND to append the
// expanded value of _RELEASE_PATCHLEVEL to the string "SPI_BOOST_CRAY_".
//
// - If _RELEASE_PATCHLEVEL is undefined, we get "SPI_BOOST_CRAY_".
// - If _RELEASE_PATCHLEVEL is 5, we get "SPI_BOOST_CRAY_5".
// - If _RELEASE_PATCHLEVEL is x (and x is not defined) we get
//   "SPI_BOOST_CRAY_x":
//
// Then we check if SPI_BOOST_CRAY_x is equal to the output of
// SPI_BOOST_CRAY_APPEND. In other words, the output of SPI_BOOST_CRAY_APPEND is
// treated as a macro name, and expanded again. If we can safely assume
// that SPI_BOOST_CRAY_ is not a macro defined as our magic number, and
// SPI_BOOST_CRAY_5 is not a macro defined as our magic number, then the only
// way the equality test can pass is if _RELEASE_PATCHLEVEL expands to x.
//
// So, that is how we detect if we are using a developer build of the Cray
// compiler.

#define SPI_BOOST_CRAY_x 9867657 // Arbitrary number
#define SPI_BOOST_CRAY_APPEND(MACRO) SPI_BOOST_CRAY_APPEND_INTERNAL(MACRO)
#define SPI_BOOST_CRAY_APPEND_INTERNAL(MACRO) SPI_BOOST_CRAY_##MACRO

#if SPI_BOOST_CRAY_x == SPI_BOOST_CRAY_APPEND(_RELEASE_PATCHLEVEL)

    // This is a developer build.
    //
    // - _RELEASE_PATCHLEVEL is defined as x, and x is not defined as a macro.

    // Pretend _RELEASE_PATCHLEVEL is 99, so we get the configuration for the
    // most recent patch level in this release.

    #define SPI_BOOST_CRAY_VERSION (_RELEASE_MAJOR * 10000 + _RELEASE_MINOR * 100 + 99)

#else

    // This is a production build.
    //
    // _RELEASE_PATCHLEVEL is not defined as x, or x is defined as a macro.

    #define SPI_BOOST_CRAY_VERSION (_RELEASE_MAJOR * 10000 + _RELEASE_MINOR * 100 + _RELEASE_PATCHLEVEL)

#endif // SPI_BOOST_CRAY_x == SPI_BOOST_CRAY_APPEND(_RELEASE_PATCHLEVEL)

#undef SPI_BOOST_CRAY_APPEND_INTERNAL
#undef SPI_BOOST_CRAY_APPEND
#undef SPI_BOOST_CRAY_x


#ifdef __GNUC__
#   define SPI_BOOST_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

#ifndef SPI_BOOST_COMPILER
#   define SPI_BOOST_COMPILER "Cray C++ version " SPI_BOOST_STRINGIZE(_RELEASE_MAJOR) "." SPI_BOOST_STRINGIZE(_RELEASE_MINOR) "." SPI_BOOST_STRINGIZE(_RELEASE_PATCHLEVEL)
#endif

// Since the Cray compiler defines '__GNUC__', we have to emulate some
// additional GCC macros in order to make everything work.
//
// FIXME: Perhaps Cray should fix the compiler to define these additional
// macros for GCC emulation?

#if __cplusplus >= 201103L && defined(__GNUC__) && !defined(__GXX_EXPERIMENTAL_CXX0X__)
#   define __GXX_EXPERIMENTAL_CXX0X__ 1
#endif

////
//// Parameter validation
////

// FIXME: Do we really need to support compilers before 8.5? Do they pass
// the Boost.Config tests?

#if SPI_BOOST_CRAY_VERSION < 80000
#  error "Boost is not configured for Cray compilers prior to version 8, please try the configure script."
#endif

// We only support recent EDG based compilers.

#ifndef __EDG__
#  error "Unsupported Cray compiler, please try running the configure script."
#endif

////
//// Baseline values
////

#include <spi_boost/config/compiler/common_edg.hpp>

#define SPI_BOOST_HAS_NRVO
#define SPI_BOOST_NO_COMPLETE_VALUE_INITIALIZATION
#define SPI_BOOST_NO_CXX11_AUTO_DECLARATIONS
#define SPI_BOOST_NO_CXX11_AUTO_MULTIDECLARATIONS
#define SPI_BOOST_NO_CXX11_CHAR16_T
#define SPI_BOOST_NO_CXX11_CHAR32_T
#define SPI_BOOST_NO_CXX11_CONSTEXPR
#define SPI_BOOST_NO_CXX11_DECLTYPE
#define SPI_BOOST_NO_CXX11_DECLTYPE_N3276
#define SPI_BOOST_NO_CXX11_DEFAULTED_FUNCTIONS
#define SPI_BOOST_NO_CXX11_DELETED_FUNCTIONS
#define SPI_BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS
#define SPI_BOOST_NO_CXX11_FINAL
#define SPI_BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
#define SPI_BOOST_NO_CXX11_LAMBDAS
#define SPI_BOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS
#define SPI_BOOST_NO_CXX11_NOEXCEPT
#define SPI_BOOST_NO_CXX11_NULLPTR
#define SPI_BOOST_NO_CXX11_RANGE_BASED_FOR
#define SPI_BOOST_NO_CXX11_RAW_LITERALS
#define SPI_BOOST_NO_CXX11_REF_QUALIFIERS
#define SPI_BOOST_NO_CXX11_RVALUE_REFERENCES
#define SPI_BOOST_NO_CXX11_SCOPED_ENUMS
#define SPI_BOOST_NO_CXX11_SFINAE_EXPR
#define SPI_BOOST_NO_CXX11_STATIC_ASSERT
#define SPI_BOOST_NO_CXX11_TEMPLATE_ALIASES
#define SPI_BOOST_NO_CXX11_THREAD_LOCAL
#define SPI_BOOST_NO_CXX11_UNICODE_LITERALS
#define SPI_BOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX
#define SPI_BOOST_NO_CXX11_USER_DEFINED_LITERALS
#define SPI_BOOST_NO_CXX11_VARIADIC_MACROS
#define SPI_BOOST_NO_CXX11_VARIADIC_TEMPLATES
#define SPI_BOOST_NO_SFINAE_EXPR
#define SPI_BOOST_NO_TWO_PHASE_NAME_LOOKUP

//#define SPI_BOOST_BCB_PARTIAL_SPECIALIZATION_BUG
#define SPI_BOOST_MATH_DISABLE_STD_FPCLASSIFY
//#define SPI_BOOST_HAS_FPCLASSIFY

#define SPI_BOOST_SP_USE_PTHREADS 
#define SPI_BOOST_AC_USE_PTHREADS 

//
// Everything that follows is working around what are thought to be
// compiler shortcomings. Revist all of these regularly.
//

//#define SPI_BOOST_USE_ENUM_STATIC_ASSERT
//#define SPI_BOOST_BUGGY_INTEGRAL_CONSTANT_EXPRESSIONS //(this may be implied by the previous #define

// These constants should be provided by the compiler.

#ifndef __ATOMIC_RELAXED
#define __ATOMIC_RELAXED 0
#define __ATOMIC_CONSUME 1
#define __ATOMIC_ACQUIRE 2
#define __ATOMIC_RELEASE 3
#define __ATOMIC_ACQ_REL 4
#define __ATOMIC_SEQ_CST 5
#endif

////
//// Version changes
////

//
// 8.5.0
//

#if SPI_BOOST_CRAY_VERSION >= 80500

#if __cplusplus >= 201103L

#undef SPI_BOOST_HAS_NRVO
#undef SPI_BOOST_NO_COMPLETE_VALUE_INITIALIZATION
#undef SPI_BOOST_NO_CXX11_AUTO_DECLARATIONS
#undef SPI_BOOST_NO_CXX11_AUTO_MULTIDECLARATIONS
#undef SPI_BOOST_NO_CXX11_CHAR16_T
#undef SPI_BOOST_NO_CXX11_CHAR32_T
#undef SPI_BOOST_NO_CXX11_CONSTEXPR
#undef SPI_BOOST_NO_CXX11_DECLTYPE
#undef SPI_BOOST_NO_CXX11_DECLTYPE_N3276
#undef SPI_BOOST_NO_CXX11_DEFAULTED_FUNCTIONS
#undef SPI_BOOST_NO_CXX11_DELETED_FUNCTIONS
#undef SPI_BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS
#undef SPI_BOOST_NO_CXX11_FINAL
#undef SPI_BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
#undef SPI_BOOST_NO_CXX11_LAMBDAS
#undef SPI_BOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS
#undef SPI_BOOST_NO_CXX11_NOEXCEPT
#undef SPI_BOOST_NO_CXX11_NULLPTR
#undef SPI_BOOST_NO_CXX11_RANGE_BASED_FOR
#undef SPI_BOOST_NO_CXX11_RAW_LITERALS
#undef SPI_BOOST_NO_CXX11_REF_QUALIFIERS
#undef SPI_BOOST_NO_CXX11_RVALUE_REFERENCES
#undef SPI_BOOST_NO_CXX11_SCOPED_ENUMS
#undef SPI_BOOST_NO_CXX11_SFINAE_EXPR
#undef SPI_BOOST_NO_CXX11_STATIC_ASSERT
#undef SPI_BOOST_NO_CXX11_TEMPLATE_ALIASES
#undef SPI_BOOST_NO_CXX11_THREAD_LOCAL
#undef SPI_BOOST_NO_CXX11_UNICODE_LITERALS
#undef SPI_BOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX
#undef SPI_BOOST_NO_CXX11_USER_DEFINED_LITERALS
#undef SPI_BOOST_NO_CXX11_VARIADIC_MACROS
#undef SPI_BOOST_NO_CXX11_VARIADIC_TEMPLATES
#undef SPI_BOOST_NO_SFINAE_EXPR
#undef SPI_BOOST_NO_TWO_PHASE_NAME_LOOKUP
#undef SPI_BOOST_MATH_DISABLE_STD_FPCLASSIFY
#undef SPI_BOOST_SP_USE_PTHREADS 
#undef SPI_BOOST_AC_USE_PTHREADS 

#define SPI_BOOST_HAS_VARIADIC_TMPL
#define SPI_BOOST_HAS_UNISTD_H
#define SPI_BOOST_HAS_TR1_COMPLEX_INVERSE_TRIG
#define SPI_BOOST_HAS_TR1_COMPLEX_OVERLOADS
#define SPI_BOOST_HAS_STDINT_H
#define SPI_BOOST_HAS_STATIC_ASSERT
#define SPI_BOOST_HAS_SIGACTION
#define SPI_BOOST_HAS_SCHED_YIELD
#define SPI_BOOST_HAS_RVALUE_REFS
#define SPI_BOOST_HAS_PTHREADS
#define SPI_BOOST_HAS_PTHREAD_YIELD
#define SPI_BOOST_HAS_PTHREAD_MUTEXATTR_SETTYPE
#define SPI_BOOST_HAS_PARTIAL_STD_ALLOCATOR
#define SPI_BOOST_HAS_NRVO
#define SPI_BOOST_HAS_NL_TYPES_H
#define SPI_BOOST_HAS_NANOSLEEP
#define SPI_BOOST_NO_CXX11_SMART_PTR
#define SPI_BOOST_NO_CXX11_HDR_FUNCTIONAL
#define SPI_BOOST_NO_CXX14_CONSTEXPR
#define SPI_BOOST_HAS_LONG_LONG
#define SPI_BOOST_HAS_FLOAT128

#if __cplusplus < 201402L
#define SPI_BOOST_NO_CXX11_DECLTYPE_N3276
#endif // __cplusplus < 201402L

#endif // __cplusplus >= 201103L

#endif // SPI_BOOST_CRAY_VERSION >= 80500

//
// 8.6.4
// (versions prior to 8.6.5 do not define _RELEASE_PATCHLEVEL)
//

#if SPI_BOOST_CRAY_VERSION >= 80600

#if __cplusplus >= 199711L
#define SPI_BOOST_HAS_FLOAT128
#define SPI_BOOST_HAS_PTHREAD_YIELD // This is a platform macro, but it improves test results.
#define SPI_BOOST_NO_COMPLETE_VALUE_INITIALIZATION // This is correct. Test compiles, but fails to run.
#undef  SPI_BOOST_NO_CXX11_CHAR16_T
#undef  SPI_BOOST_NO_CXX11_CHAR32_T
#undef  SPI_BOOST_NO_CXX11_INLINE_NAMESPACES
#undef  SPI_BOOST_NO_CXX11_FINAL
#undef  SPI_BOOST_NO_CXX11_FIXED_LENGTH_VARIADIC_TEMPLATE_EXPANSION_PACKS
#undef  SPI_BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
#define SPI_BOOST_NO_CXX11_SFINAE_EXPR // This is correct, even though '*_fail.cpp' test fails.
#undef  SPI_BOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX
#undef  SPI_BOOST_NO_CXX11_VARIADIC_MACROS
#undef  SPI_BOOST_NO_CXX11_VARIADIC_TEMPLATES
// 'SPI_BOOST_NO_DEDUCED_TYPENAME' test is broken. The test files are enabled /
// disabled with an '#ifdef SPI_BOOST_DEDUCED_TYPENAME'. However,
// 'spi_boost/libs/config/include/spi_boost/config/detail/suffix.hpp' ensures that
// 'SPI_BOOST_DEDUCED_TYPENAME' is always defined (the value it is defined as
// depends on 'SPI_BOOST_NO_DEDUCED_TYPENAME'). So, modifying
// 'SPI_BOOST_NO_DEDUCED_TYPENAME' has no effect on which tests are run.
//
// The 'no_ded_typename_pass.cpp' test should always compile and run
// successfully, because 'SPI_BOOST_DEDUCED_TYPENAME' must always have an
// appropriate value (it's not just something that you turn on or off).
// Therefore, if you wish to test changes to 'SPI_BOOST_NO_DEDUCED_TYPENAME',
// you have to modify 'no_ded_typename_pass.cpp' to unconditionally include
// 'spi_boost_no_ded_typename.ipp'.
#undef  SPI_BOOST_NO_DEDUCED_TYPENAME // This is correct. Test is broken.
#undef  SPI_BOOST_NO_SFINAE_EXPR
#undef  SPI_BOOST_NO_TWO_PHASE_NAME_LOOKUP
#endif // __cplusplus >= 199711L

#if __cplusplus >= 201103L
#undef  SPI_BOOST_NO_CXX11_ALIGNAS
#undef  SPI_BOOST_NO_CXX11_DECLTYPE_N3276
#define SPI_BOOST_NO_CXX11_HDR_ATOMIC
#undef  SPI_BOOST_NO_CXX11_HDR_FUNCTIONAL
#define SPI_BOOST_NO_CXX11_HDR_REGEX // This is correct. Test compiles, but fails to run.
#undef  SPI_BOOST_NO_CXX11_SFINAE_EXPR
#undef  SPI_BOOST_NO_CXX11_SMART_PTR
#undef  SPI_BOOST_NO_CXX11_TRAILING_RESULT_TYPES
#endif // __cplusplus >= 201103L

#if __cplusplus >= 201402L
#undef  SPI_BOOST_NO_CXX14_CONSTEXPR
#define SPI_BOOST_NO_CXX14_DIGIT_SEPARATORS
#endif // __cplusplus == 201402L

#endif // SPI_BOOST_CRAY_VERSION >= 80600

//
// 8.6.5
// (no change from 8.6.4)
//

//
// 8.7.0
//

#if SPI_BOOST_CRAY_VERSION >= 80700

#if __cplusplus >= 199711L
#endif // __cplusplus >= 199711L

#if __cplusplus >= 201103L
#undef  SPI_BOOST_NO_CXX11_HDR_ATOMIC
#undef  SPI_BOOST_NO_CXX11_HDR_REGEX
#endif // __cplusplus >= 201103L

#if __cplusplus >= 201402L
#endif // __cplusplus == 201402L

#endif // SPI_BOOST_CRAY_VERSION >= 80700

//
// Next release
//

#if SPI_BOOST_CRAY_VERSION > 80799

#if __cplusplus >= 199711L
#endif // __cplusplus >= 199711L

#if __cplusplus >= 201103L
#endif // __cplusplus >= 201103L

#if __cplusplus >= 201402L
#endif // __cplusplus == 201402L

#endif // SPI_BOOST_CRAY_VERSION > 80799

////
//// Remove temporary macros
////

// I've commented out some '#undef' statements to signify that we purposely
// want to keep certain macros.

//#undef __GXX_EXPERIMENTAL_CXX0X__
//#undef SPI_BOOST_COMPILER
#undef SPI_BOOST_GCC_VERSION
#undef SPI_BOOST_CRAY_VERSION
