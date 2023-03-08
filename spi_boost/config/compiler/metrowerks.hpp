//  (C) Copyright John Maddock 2001.
//  (C) Copyright Darin Adler 2001.
//  (C) Copyright Peter Dimov 2001.
//  (C) Copyright David Abrahams 2001 - 2002.
//  (C) Copyright Beman Dawes 2001 - 2003.
//  (C) Copyright Stefan Slapeta 2004.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  Metrowerks C++ compiler setup:

// locale support is disabled when linking with the dynamic runtime
#   ifdef _MSL_NO_LOCALE
#     define SPI_BOOST_NO_STD_LOCALE
#   endif

#   if __MWERKS__ <= 0x2301  // 5.3
#     define SPI_BOOST_NO_FUNCTION_TEMPLATE_ORDERING
#     define SPI_BOOST_NO_POINTER_TO_MEMBER_CONST
#     define SPI_BOOST_NO_DEPENDENT_TYPES_IN_TEMPLATE_VALUE_PARAMETERS
#     define SPI_BOOST_NO_MEMBER_TEMPLATE_KEYWORD
#   endif

#   if __MWERKS__ <= 0x2401  // 6.2
//#     define SPI_BOOST_NO_FUNCTION_TEMPLATE_ORDERING
#   endif

#   if(__MWERKS__ <= 0x2407)  // 7.x
#     define SPI_BOOST_NO_MEMBER_FUNCTION_SPECIALIZATIONS
#     define SPI_BOOST_NO_UNREACHABLE_RETURN_DETECTION
#   endif

#   if(__MWERKS__ <= 0x3003)  // 8.x
#     define SPI_BOOST_NO_SFINAE
#    endif

// the "|| !defined(SPI_BOOST_STRICT_CONFIG)" part should apply to the last
// tested version *only*:
#   if(__MWERKS__ <= 0x3207) || !defined(SPI_BOOST_STRICT_CONFIG) // 9.6
#     define SPI_BOOST_NO_MEMBER_TEMPLATE_FRIENDS
#     define SPI_BOOST_NO_IS_ABSTRACT
#    endif

#if !__option(wchar_type)
#   define SPI_BOOST_NO_INTRINSIC_WCHAR_T
#endif

#if !__option(exceptions) && !defined(SPI_BOOST_NO_EXCEPTIONS)
#   define SPI_BOOST_NO_EXCEPTIONS
#endif

#if (__INTEL__ && _WIN32) || (__POWERPC__ && macintosh)
#   if __MWERKS__ == 0x3000
#     define SPI_BOOST_COMPILER_VERSION 8.0
#   elif __MWERKS__ == 0x3001
#     define SPI_BOOST_COMPILER_VERSION 8.1
#   elif __MWERKS__ == 0x3002
#     define SPI_BOOST_COMPILER_VERSION 8.2
#   elif __MWERKS__ == 0x3003
#     define SPI_BOOST_COMPILER_VERSION 8.3
#   elif __MWERKS__ == 0x3200
#     define SPI_BOOST_COMPILER_VERSION 9.0
#   elif __MWERKS__ == 0x3201
#     define SPI_BOOST_COMPILER_VERSION 9.1
#   elif __MWERKS__ == 0x3202
#     define SPI_BOOST_COMPILER_VERSION 9.2
#   elif __MWERKS__ == 0x3204
#     define SPI_BOOST_COMPILER_VERSION 9.3
#   elif __MWERKS__ == 0x3205
#     define SPI_BOOST_COMPILER_VERSION 9.4
#   elif __MWERKS__ == 0x3206
#     define SPI_BOOST_COMPILER_VERSION 9.5
#   elif __MWERKS__ == 0x3207
#     define SPI_BOOST_COMPILER_VERSION 9.6
#   else
#     define SPI_BOOST_COMPILER_VERSION __MWERKS__
#   endif
#else
#  define SPI_BOOST_COMPILER_VERSION __MWERKS__
#endif

//
// C++0x features
//
//   See spi_boost\config\suffix.hpp for SPI_BOOST_NO_LONG_LONG
//
#if __MWERKS__ > 0x3206 && __option(rvalue_refs)
#  define SPI_BOOST_HAS_RVALUE_REFS
#else
#  define SPI_BOOST_NO_CXX11_RVALUE_REFERENCES
#endif
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
#define SPI_BOOST_NO_CXX11_EXTERN_TEMPLATE
#define SPI_BOOST_NO_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGS
#define SPI_BOOST_NO_CXX11_HDR_INITIALIZER_LIST
#define SPI_BOOST_NO_CXX11_LAMBDAS
#define SPI_BOOST_NO_CXX11_LOCAL_CLASS_TEMPLATE_PARAMETERS
#define SPI_BOOST_NO_CXX11_NOEXCEPT
#define SPI_BOOST_NO_CXX11_NULLPTR
#define SPI_BOOST_NO_CXX11_RANGE_BASED_FOR
#define SPI_BOOST_NO_CXX11_RAW_LITERALS
#define SPI_BOOST_NO_CXX11_SCOPED_ENUMS
#define SPI_BOOST_NO_SFINAE_EXPR
#define SPI_BOOST_NO_CXX11_SFINAE_EXPR
#define SPI_BOOST_NO_CXX11_STATIC_ASSERT
#define SPI_BOOST_NO_CXX11_TEMPLATE_ALIASES
#define SPI_BOOST_NO_CXX11_UNICODE_LITERALS
#define SPI_BOOST_NO_CXX11_VARIADIC_TEMPLATES
#define SPI_BOOST_NO_CXX11_VARIADIC_MACROS
#define SPI_BOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX
#define SPI_BOOST_NO_CXX11_USER_DEFINED_LITERALS
#define SPI_BOOST_NO_CXX11_ALIGNAS
#define SPI_BOOST_NO_CXX11_TRAILING_RESULT_TYPES
#define SPI_BOOST_NO_CXX11_INLINE_NAMESPACES
#define SPI_BOOST_NO_CXX11_REF_QUALIFIERS
#define SPI_BOOST_NO_CXX11_FINAL
#define SPI_BOOST_NO_CXX11_THREAD_LOCAL

// C++ 14:
#if !defined(__cpp_aggregate_nsdmi) || (__cpp_aggregate_nsdmi < 201304)
#  define SPI_BOOST_NO_CXX14_AGGREGATE_NSDMI
#endif
#if !defined(__cpp_binary_literals) || (__cpp_binary_literals < 201304)
#  define SPI_BOOST_NO_CXX14_BINARY_LITERALS
#endif
#if !defined(__cpp_constexpr) || (__cpp_constexpr < 201304)
#  define SPI_BOOST_NO_CXX14_CONSTEXPR
#endif
#if !defined(__cpp_decltype_auto) || (__cpp_decltype_auto < 201304)
#  define SPI_BOOST_NO_CXX14_DECLTYPE_AUTO
#endif
#if (__cplusplus < 201304) // There's no SD6 check for this....
#  define SPI_BOOST_NO_CXX14_DIGIT_SEPARATORS
#endif
#if !defined(__cpp_generic_lambdas) || (__cpp_generic_lambdas < 201304)
#  define SPI_BOOST_NO_CXX14_GENERIC_LAMBDAS
#endif
#if !defined(__cpp_init_captures) || (__cpp_init_captures < 201304)
#  define SPI_BOOST_NO_CXX14_INITIALIZED_LAMBDA_CAPTURES
#endif
#if !defined(__cpp_return_type_deduction) || (__cpp_return_type_deduction < 201304)
#  define SPI_BOOST_NO_CXX14_RETURN_TYPE_DEDUCTION
#endif
#if !defined(__cpp_variable_templates) || (__cpp_variable_templates < 201304)
#  define SPI_BOOST_NO_CXX14_VARIABLE_TEMPLATES
#endif

// C++17
#if !defined(__cpp_structured_bindings) || (__cpp_structured_bindings < 201606)
#  define SPI_BOOST_NO_CXX17_STRUCTURED_BINDINGS
#endif
#if !defined(__cpp_inline_variables) || (__cpp_inline_variables < 201606)
#  define SPI_BOOST_NO_CXX17_INLINE_VARIABLES
#endif
#if !defined(__cpp_fold_expressions) || (__cpp_fold_expressions < 201603)
#  define SPI_BOOST_NO_CXX17_FOLD_EXPRESSIONS
#endif
#if !defined(__cpp_if_constexpr) || (__cpp_if_constexpr < 201606)
#  define SPI_BOOST_NO_CXX17_IF_CONSTEXPR
#endif

#define SPI_BOOST_COMPILER "Metrowerks CodeWarrior C++ version " SPI_BOOST_STRINGIZE(SPI_BOOST_COMPILER_VERSION)

//
// versions check:
// we don't support Metrowerks prior to version 5.3:
#if __MWERKS__ < 0x2301
#  error "Compiler not supported or configured - please reconfigure"
#endif
//
// last known and checked version:
#if (__MWERKS__ > 0x3205)
#  if defined(SPI_BOOST_ASSERT_CONFIG)
#     error "spi_boost: Unknown compiler version - please run the configure tests and report the results"
#  endif
#endif







