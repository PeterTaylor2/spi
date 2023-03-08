//  This header intentionally has no include guards.
//
//  Copyright (c) 2001-2009, 2012 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

#if !defined( SPI_BOOST_NO_CXX11_EXPLICIT_CONVERSION_OPERATORS ) && !defined( SPI_BOOST_NO_CXX11_NULLPTR )

    explicit operator bool () const SPI_BOOST_SP_NOEXCEPT
    {
        return px != 0;
    }

#else

    typedef element_type * this_type::*unspecified_bool_type;

    operator unspecified_bool_type() const SPI_BOOST_SP_NOEXCEPT
    {
        return px == 0? 0: &this_type::px;
    }

#endif

    // operator! is redundant, but some compilers need it
    bool operator! () const SPI_BOOST_SP_NOEXCEPT
    {
        return px == 0;
    }
