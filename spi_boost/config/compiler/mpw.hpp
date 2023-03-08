//  (C) Copyright John Maddock 2001 - 2002.
//  (C) Copyright Aleksey Gurtovoy 2002.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  MPW C++ compilers setup:

#   if    defined(__SC__)
#     define SPI_BOOST_COMPILER "MPW SCpp version " SPI_BOOST_STRINGIZE(__SC__)
#   elif defined(__MRC__)
#     define SPI_BOOST_COMPILER "MPW MrCpp version " SPI_BOOST_STRINGIZE(__MRC__)
#   else
#     error "Using MPW compiler configuration by mistake.  Please update."
#   endif

//
// MPW 8.90:
//
#if (MPW_CPLUS <= 0x890) || !defined(SPI_BOOST_STRICT_CONFIG)
#  define SPI_BOOST_NO_CV_SPECIALIZATIONS
#  define SPI_BOOST_NO_DEPENDENT_NESTED_DERIVATIONS
#  define SPI_BOOST_NO_DEPENDENT_TYPES_IN_TEMPLATE_VALUE_PARAMETERS
#  define SPI_BOOST_NO_INCLASS_MEMBER_INITIALIZATION
#  define SPI_BOOST_NO_INTRINSIC_WCHAR_T
#  define SPI_BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#  define SPI_BOOST_NO_USING_TEMPLATE

#  define SPI_BOOST_NO_CWCHAR
#  define SPI_BOOST_NO_LIMITS_COMPILE_TIME_CONSTANTS

#  define SPI_BOOST_NO_STD_ALLOCATOR /* actually a bug with const reference overloading */

#endif

//
// C++0x features
//
//   See spi_boost\config\suffix.hpp for SPI_BOOST_NO_LONG_LONG
//
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
#define SPI_BOOST_NO_CXX11_RVALUE_REFERENCES
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

//
// versions check:
// we don't support MPW prior to version 8.9:
#if MPW_CPLUS < 0x890
#  error "Compiler not supported or configured - please reconfigure"
#endif
//
// last known and checked version is 0x890:
#if (MPW_CPLUS > 0x890)
#  if defined(SPI_BOOST_ASSERT_CONFIG)
#     error "spi_boost: Unknown compiler version - please run the configure tests and report the results"
#  endif
#endif


