
//  (C) Copyright John maddock 1999. 
//  (C) David Abrahams 2002.  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// use this header as a workaround for missing <limits>

//  See http://www.boost.org/libs/compatibility/index.html for documentation.

#ifndef SPI_BOOST_LIMITS
#define SPI_BOOST_LIMITS

#include <spi_boost/config.hpp>

#ifdef SPI_BOOST_NO_LIMITS
#  error "There is no std::numeric_limits suppport available."
#else
# include <limits>
#endif

#if (defined(SPI_BOOST_HAS_LONG_LONG) && defined(SPI_BOOST_NO_LONG_LONG_NUMERIC_LIMITS)) \
      || (defined(SPI_BOOST_HAS_MS_INT64) && defined(SPI_BOOST_NO_MS_INT64_NUMERIC_LIMITS))
// Add missing specializations for numeric_limits:
#ifdef SPI_BOOST_HAS_MS_INT64
#  define SPI_BOOST_LLT __int64
#  define SPI_BOOST_ULLT unsigned __int64
#else
#  define SPI_BOOST_LLT  ::spi_boost::long_long_type
#  define SPI_BOOST_ULLT  ::spi_boost::ulong_long_type
#endif

#include <climits>  // for CHAR_BIT

namespace std
{
  template<>
  class numeric_limits<SPI_BOOST_LLT> 
  {
   public:

      SPI_BOOST_STATIC_CONSTANT(bool, is_specialized = true);
#ifdef SPI_BOOST_HAS_MS_INT64
      static SPI_BOOST_LLT min SPI_BOOST_PREVENT_MACRO_SUBSTITUTION (){ return 0x8000000000000000i64; }
      static SPI_BOOST_LLT max SPI_BOOST_PREVENT_MACRO_SUBSTITUTION (){ return 0x7FFFFFFFFFFFFFFFi64; }
#elif defined(LLONG_MAX)
      static SPI_BOOST_LLT min SPI_BOOST_PREVENT_MACRO_SUBSTITUTION (){ return LLONG_MIN; }
      static SPI_BOOST_LLT max SPI_BOOST_PREVENT_MACRO_SUBSTITUTION (){ return LLONG_MAX; }
#elif defined(LONGLONG_MAX)
      static SPI_BOOST_LLT min SPI_BOOST_PREVENT_MACRO_SUBSTITUTION (){ return LONGLONG_MIN; }
      static SPI_BOOST_LLT max SPI_BOOST_PREVENT_MACRO_SUBSTITUTION (){ return LONGLONG_MAX; }
#else
      static SPI_BOOST_LLT min SPI_BOOST_PREVENT_MACRO_SUBSTITUTION (){ return 1LL << (sizeof(SPI_BOOST_LLT) * CHAR_BIT - 1); }
      static SPI_BOOST_LLT max SPI_BOOST_PREVENT_MACRO_SUBSTITUTION (){ return ~(min)(); }
#endif
      SPI_BOOST_STATIC_CONSTANT(int, digits = sizeof(SPI_BOOST_LLT) * CHAR_BIT -1);
      SPI_BOOST_STATIC_CONSTANT(int, digits10 = (CHAR_BIT * sizeof (SPI_BOOST_LLT) - 1) * 301L / 1000);
      SPI_BOOST_STATIC_CONSTANT(bool, is_signed = true);
      SPI_BOOST_STATIC_CONSTANT(bool, is_integer = true);
      SPI_BOOST_STATIC_CONSTANT(bool, is_exact = true);
      SPI_BOOST_STATIC_CONSTANT(int, radix = 2);
      static SPI_BOOST_LLT epsilon() throw() { return 0; };
      static SPI_BOOST_LLT round_error() throw() { return 0; };

      SPI_BOOST_STATIC_CONSTANT(int, min_exponent = 0);
      SPI_BOOST_STATIC_CONSTANT(int, min_exponent10 = 0);
      SPI_BOOST_STATIC_CONSTANT(int, max_exponent = 0);
      SPI_BOOST_STATIC_CONSTANT(int, max_exponent10 = 0);

      SPI_BOOST_STATIC_CONSTANT(bool, has_infinity = false);
      SPI_BOOST_STATIC_CONSTANT(bool, has_quiet_NaN = false);
      SPI_BOOST_STATIC_CONSTANT(bool, has_signaling_NaN = false);
      SPI_BOOST_STATIC_CONSTANT(bool, has_denorm = false);
      SPI_BOOST_STATIC_CONSTANT(bool, has_denorm_loss = false);
      static SPI_BOOST_LLT infinity() throw() { return 0; };
      static SPI_BOOST_LLT quiet_NaN() throw() { return 0; };
      static SPI_BOOST_LLT signaling_NaN() throw() { return 0; };
      static SPI_BOOST_LLT denorm_min() throw() { return 0; };

      SPI_BOOST_STATIC_CONSTANT(bool, is_iec559 = false);
      SPI_BOOST_STATIC_CONSTANT(bool, is_bounded = true);
      SPI_BOOST_STATIC_CONSTANT(bool, is_modulo = true);

      SPI_BOOST_STATIC_CONSTANT(bool, traps = false);
      SPI_BOOST_STATIC_CONSTANT(bool, tinyness_before = false);
      SPI_BOOST_STATIC_CONSTANT(float_round_style, round_style = round_toward_zero);
      
  };

  template<>
  class numeric_limits<SPI_BOOST_ULLT> 
  {
   public:

      SPI_BOOST_STATIC_CONSTANT(bool, is_specialized = true);
#ifdef SPI_BOOST_HAS_MS_INT64
      static SPI_BOOST_ULLT min SPI_BOOST_PREVENT_MACRO_SUBSTITUTION (){ return 0ui64; }
      static SPI_BOOST_ULLT max SPI_BOOST_PREVENT_MACRO_SUBSTITUTION (){ return 0xFFFFFFFFFFFFFFFFui64; }
#elif defined(ULLONG_MAX) && defined(ULLONG_MIN)
      static SPI_BOOST_ULLT min SPI_BOOST_PREVENT_MACRO_SUBSTITUTION (){ return ULLONG_MIN; }
      static SPI_BOOST_ULLT max SPI_BOOST_PREVENT_MACRO_SUBSTITUTION (){ return ULLONG_MAX; }
#elif defined(ULONGLONG_MAX) && defined(ULONGLONG_MIN)
      static SPI_BOOST_ULLT min SPI_BOOST_PREVENT_MACRO_SUBSTITUTION (){ return ULONGLONG_MIN; }
      static SPI_BOOST_ULLT max SPI_BOOST_PREVENT_MACRO_SUBSTITUTION (){ return ULONGLONG_MAX; }
#else
      static SPI_BOOST_ULLT min SPI_BOOST_PREVENT_MACRO_SUBSTITUTION (){ return 0uLL; }
      static SPI_BOOST_ULLT max SPI_BOOST_PREVENT_MACRO_SUBSTITUTION (){ return ~0uLL; }
#endif
      SPI_BOOST_STATIC_CONSTANT(int, digits = sizeof(SPI_BOOST_LLT) * CHAR_BIT);
      SPI_BOOST_STATIC_CONSTANT(int, digits10 = (CHAR_BIT * sizeof (SPI_BOOST_LLT)) * 301L / 1000);
      SPI_BOOST_STATIC_CONSTANT(bool, is_signed = false);
      SPI_BOOST_STATIC_CONSTANT(bool, is_integer = true);
      SPI_BOOST_STATIC_CONSTANT(bool, is_exact = true);
      SPI_BOOST_STATIC_CONSTANT(int, radix = 2);
      static SPI_BOOST_ULLT epsilon() throw() { return 0; };
      static SPI_BOOST_ULLT round_error() throw() { return 0; };

      SPI_BOOST_STATIC_CONSTANT(int, min_exponent = 0);
      SPI_BOOST_STATIC_CONSTANT(int, min_exponent10 = 0);
      SPI_BOOST_STATIC_CONSTANT(int, max_exponent = 0);
      SPI_BOOST_STATIC_CONSTANT(int, max_exponent10 = 0);

      SPI_BOOST_STATIC_CONSTANT(bool, has_infinity = false);
      SPI_BOOST_STATIC_CONSTANT(bool, has_quiet_NaN = false);
      SPI_BOOST_STATIC_CONSTANT(bool, has_signaling_NaN = false);
      SPI_BOOST_STATIC_CONSTANT(bool, has_denorm = false);
      SPI_BOOST_STATIC_CONSTANT(bool, has_denorm_loss = false);
      static SPI_BOOST_ULLT infinity() throw() { return 0; };
      static SPI_BOOST_ULLT quiet_NaN() throw() { return 0; };
      static SPI_BOOST_ULLT signaling_NaN() throw() { return 0; };
      static SPI_BOOST_ULLT denorm_min() throw() { return 0; };

      SPI_BOOST_STATIC_CONSTANT(bool, is_iec559 = false);
      SPI_BOOST_STATIC_CONSTANT(bool, is_bounded = true);
      SPI_BOOST_STATIC_CONSTANT(bool, is_modulo = true);

      SPI_BOOST_STATIC_CONSTANT(bool, traps = false);
      SPI_BOOST_STATIC_CONSTANT(bool, tinyness_before = false);
      SPI_BOOST_STATIC_CONSTANT(float_round_style, round_style = round_toward_zero);
      
  };
}
#endif 

#endif

