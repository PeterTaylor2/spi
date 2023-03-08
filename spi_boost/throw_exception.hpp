#ifndef SPI_BOOST_UUID_AA15E74A856F11E08B8D93F24824019B
#define SPI_BOOST_UUID_AA15E74A856F11E08B8D93F24824019B

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  spi_boost/throw_exception.hpp
//
//  Copyright (c) 2002 Peter Dimov and Multi Media Ltd.
//  Copyright (c) 2008-2009 Emil Dotchevski and Reverge Studios, Inc.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  http://www.boost.org/libs/utility/throw_exception.html
//

#include <spi_boost/config.hpp>
#include <spi_boost/detail/workaround.hpp>
#include <exception>

#if !defined( SPI_BOOST_EXCEPTION_DISABLE )
# include <spi_boost/exception/exception.hpp>
#if !defined(SPI_BOOST_THROW_EXCEPTION_CURRENT_FUNCTION)
# include <spi_boost/current_function.hpp>
# define SPI_BOOST_THROW_EXCEPTION_CURRENT_FUNCTION SPI_BOOST_CURRENT_FUNCTION
#endif
# define SPI_BOOST_THROW_EXCEPTION(x) ::spi_boost::exception_detail::throw_exception_(x,SPI_BOOST_THROW_EXCEPTION_CURRENT_FUNCTION,__FILE__,__LINE__)
#else
# define SPI_BOOST_THROW_EXCEPTION(x) ::spi_boost::throw_exception(x)
#endif

#if defined(__GNUC__) && (__GNUC__*100+__GNUC_MINOR__>301) && !defined(SPI_BOOST_EXCEPTION_ENABLE_WARNINGS)
#pragma GCC system_header
#endif
#if defined(_MSC_VER) && !defined(SPI_BOOST_EXCEPTION_ENABLE_WARNINGS)
#pragma warning(push,1)
#endif

namespace spi_boost
{
#ifdef SPI_BOOST_NO_EXCEPTIONS

void throw_exception( std::exception const & e ); // user defined

#else

inline void throw_exception_assert_compatibility( std::exception const & ) { }

template<class E> SPI_BOOST_NORETURN inline void throw_exception( E const & e )
{
    //All spi_boost exceptions are required to derive from std::exception,
    //to ensure compatibility with SPI_BOOST_NO_EXCEPTIONS.
    throw_exception_assert_compatibility(e);

#ifndef SPI_BOOST_EXCEPTION_DISABLE
    throw exception_detail::enable_both( e );
#else
    throw e;
#endif
}

#endif

#if !defined( SPI_BOOST_EXCEPTION_DISABLE )
    namespace
    exception_detail
    {
        template <class E>
        SPI_BOOST_NORETURN
        void
        throw_exception_( E const & x, char const * current_function, char const * file, int line )
        {
            spi_boost::throw_exception(
                set_info(
                    set_info(
                        set_info(
                            enable_error_info(x),
                            throw_function(current_function)),
                        throw_file(file)),
                    throw_line(line)));
        }
    }
#endif
} // namespace spi_boost

#if defined(_MSC_VER) && !defined(SPI_BOOST_EXCEPTION_ENABLE_WARNINGS)
#pragma warning(pop)
#endif
#endif
