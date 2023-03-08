#ifndef SPI_BOOST_SMART_PTR_DETAIL_LOCAL_COUNTED_BASE_HPP_INCLUDED
#define SPI_BOOST_SMART_PTR_DETAIL_LOCAL_COUNTED_BASE_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  detail/local_counted_base.hpp
//
//  Copyright 2017 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/smart_ptr/ for documentation.

#include <spi_boost/smart_ptr/detail/shared_count.hpp>
#include <spi_boost/config.hpp>
#include <utility>

namespace spi_boost
{

namespace detail
{

class SPI_BOOST_SYMBOL_VISIBLE local_counted_base
{
private:

    local_counted_base & operator= ( local_counted_base const & );

private:

    // not 'int' or 'unsigned' to avoid aliasing and enable optimizations
    enum count_type { min_ = 0, initial_ = 1, max_ = 2147483647 };

    count_type local_use_count_;

public:

    SPI_BOOST_CONSTEXPR local_counted_base() SPI_BOOST_SP_NOEXCEPT: local_use_count_( initial_ )
    {
    }

    SPI_BOOST_CONSTEXPR local_counted_base( local_counted_base const & ) SPI_BOOST_SP_NOEXCEPT: local_use_count_( initial_ )
    {
    }

    virtual ~local_counted_base() /*SPI_BOOST_SP_NOEXCEPT*/
    {
    }

    virtual void local_cb_destroy() SPI_BOOST_SP_NOEXCEPT = 0;

    virtual spi_boost::detail::shared_count local_cb_get_shared_count() const SPI_BOOST_SP_NOEXCEPT = 0;

    void add_ref() SPI_BOOST_SP_NOEXCEPT
    {
#if !defined(__NVCC__)
#if defined( __has_builtin )
# if __has_builtin( __builtin_assume )

        __builtin_assume( local_use_count_ >= 1 );

# endif
#endif
#endif

        local_use_count_ = static_cast<count_type>( local_use_count_ + 1 );
    }

    void release() SPI_BOOST_SP_NOEXCEPT
    {
        local_use_count_ = static_cast<count_type>( local_use_count_ - 1 );

        if( local_use_count_ == 0 )
        {
            local_cb_destroy();
        }
    }

    long local_use_count() const SPI_BOOST_SP_NOEXCEPT
    {
        return local_use_count_;
    }
};

class SPI_BOOST_SYMBOL_VISIBLE local_counted_impl: public local_counted_base
{
private:

    local_counted_impl( local_counted_impl const & );

private:

    shared_count pn_;

public:

    explicit local_counted_impl( shared_count const& pn ) SPI_BOOST_SP_NOEXCEPT: pn_( pn )
    {
    }

#if !defined( SPI_BOOST_NO_CXX11_RVALUE_REFERENCES )

    explicit local_counted_impl( shared_count && pn ) SPI_BOOST_SP_NOEXCEPT: pn_( std::move(pn) )
    {
    }

#endif

    virtual void local_cb_destroy() SPI_BOOST_SP_NOEXCEPT
    {
        delete this;
    }

    virtual spi_boost::detail::shared_count local_cb_get_shared_count() const SPI_BOOST_SP_NOEXCEPT
    {
        return pn_;
    }
};

class SPI_BOOST_SYMBOL_VISIBLE local_counted_impl_em: public local_counted_base
{
public:

    shared_count pn_;

    virtual void local_cb_destroy() SPI_BOOST_SP_NOEXCEPT
    {
        shared_count().swap( pn_ );
    }

    virtual spi_boost::detail::shared_count local_cb_get_shared_count() const SPI_BOOST_SP_NOEXCEPT
    {
        return pn_;
    }
};

} // namespace detail

} // namespace spi_boost

#endif  // #ifndef SPI_BOOST_SMART_PTR_DETAIL_LOCAL_COUNTED_BASE_HPP_INCLUDED
