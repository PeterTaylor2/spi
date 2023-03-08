//
//  spi_boost/assert.hpp - SPI_BOOST_ASSERT(expr)
//                     SPI_BOOST_ASSERT_MSG(expr, msg)
//                     SPI_BOOST_VERIFY(expr)
//                     SPI_BOOST_VERIFY_MSG(expr, msg)
//                     SPI_BOOST_ASSERT_IS_VOID
//
//  Copyright (c) 2001, 2002 Peter Dimov and Multi Media Ltd.
//  Copyright (c) 2007, 2014 Peter Dimov
//  Copyright (c) Beman Dawes 2011
//  Copyright (c) 2015 Ion Gaztanaga
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//
//  Note: There are no include guards. This is intentional.
//
//  See http://www.boost.org/libs/assert/assert.html for documentation.
//

//
// Stop inspect complaining about use of 'assert':
//
// spi_boostinspect:naassert_macro
//

//
// SPI_BOOST_ASSERT, SPI_BOOST_ASSERT_MSG, SPI_BOOST_ASSERT_IS_VOID
//

#undef SPI_BOOST_ASSERT
#undef SPI_BOOST_ASSERT_MSG
#undef SPI_BOOST_ASSERT_IS_VOID

#ifndef SPI_BOOST_ENABLE_ASSERT_HANDLER
#define SPI_BOOST_ENABLE_ASSERT_HANDLER
#endif

#if defined(SPI_BOOST_DISABLE_ASSERTS) || ( defined(SPI_BOOST_ENABLE_ASSERT_DEBUG_HANDLER) && defined(NDEBUG) )

# define SPI_BOOST_ASSERT(expr) ((void)0)
# define SPI_BOOST_ASSERT_MSG(expr, msg) ((void)0)
# define SPI_BOOST_ASSERT_IS_VOID

#elif defined(SPI_BOOST_ENABLE_ASSERT_HANDLER) || ( defined(SPI_BOOST_ENABLE_ASSERT_DEBUG_HANDLER) && !defined(NDEBUG) )

#include <spi_boost/config.hpp> // for SPI_BOOST_LIKELY
#include <spi_boost/current_function.hpp>

namespace spi_boost
{
    void assertion_failed(char const * expr, char const * function, char const * file, long line); // user defined
    void assertion_failed_msg(char const * expr, char const * msg, char const * function, char const * file, long line); // user defined
} // namespace spi_boost

#define SPI_BOOST_ASSERT(expr) (SPI_BOOST_LIKELY(!!(expr))? ((void)0): ::spi_boost::assertion_failed(#expr, SPI_BOOST_CURRENT_FUNCTION, __FILE__, __LINE__))
#define SPI_BOOST_ASSERT_MSG(expr, msg) (SPI_BOOST_LIKELY(!!(expr))? ((void)0): ::spi_boost::assertion_failed_msg(#expr, msg, SPI_BOOST_CURRENT_FUNCTION, __FILE__, __LINE__))

#else

# include <assert.h> // .h to support old libraries w/o <cassert> - effect is the same

# define SPI_BOOST_ASSERT(expr) assert(expr)
# define SPI_BOOST_ASSERT_MSG(expr, msg) assert((expr)&&(msg))
#if defined(NDEBUG)
# define SPI_BOOST_ASSERT_IS_VOID
#endif

#endif

//
// SPI_BOOST_VERIFY, SPI_BOOST_VERIFY_MSG
//

#undef SPI_BOOST_VERIFY
#undef SPI_BOOST_VERIFY_MSG

#if defined(SPI_BOOST_DISABLE_ASSERTS) || ( !defined(SPI_BOOST_ENABLE_ASSERT_HANDLER) && defined(NDEBUG) )

# define SPI_BOOST_VERIFY(expr) ((void)(expr))
# define SPI_BOOST_VERIFY_MSG(expr, msg) ((void)(expr))

#else

# define SPI_BOOST_VERIFY(expr) SPI_BOOST_ASSERT(expr)
# define SPI_BOOST_VERIFY_MSG(expr, msg) SPI_BOOST_ASSERT_MSG(expr,msg)

#endif
