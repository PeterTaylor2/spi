#ifndef SPI_BOOST_SMART_PTR_DETAIL_SP_COUNTED_BASE_PT_HPP_INCLUDED
#define SPI_BOOST_SMART_PTR_DETAIL_SP_COUNTED_BASE_PT_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  detail/sp_counted_base_pt.hpp
//
//  Copyright (c) 2001, 2002, 2003 Peter Dimov and Multi Media Ltd.
//  Copyright 2004-2005 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <spi_boost/detail/sp_typeinfo.hpp>
#include <spi_boost/assert.hpp>
#include <spi_boost/config.hpp>
#include <spi_boost/cstdint.hpp>
#include <pthread.h>

namespace spi_boost
{

namespace detail
{

class SPI_BOOST_SYMBOL_VISIBLE sp_counted_base
{
private:

    sp_counted_base( sp_counted_base const & );
    sp_counted_base & operator= ( sp_counted_base const & );

    spi_boost::int_least32_t use_count_;        // #shared
    spi_boost::int_least32_t weak_count_;       // #weak + (#shared != 0)

    mutable pthread_mutex_t m_;

public:

    sp_counted_base(): use_count_( 1 ), weak_count_( 1 )
    {
// HPUX 10.20 / DCE has a nonstandard pthread_mutex_init

#if defined(__hpux) && defined(_DECTHREADS_)
        SPI_BOOST_VERIFY( pthread_mutex_init( &m_, pthread_mutexattr_default ) == 0 );
#else
        SPI_BOOST_VERIFY( pthread_mutex_init( &m_, 0 ) == 0 );
#endif
    }

    virtual ~sp_counted_base() // nothrow
    {
        SPI_BOOST_VERIFY( pthread_mutex_destroy( &m_ ) == 0 );
    }

    // dispose() is called when use_count_ drops to zero, to release
    // the resources managed by *this.

    virtual void dispose() = 0; // nothrow

    // destroy() is called when weak_count_ drops to zero.

    virtual void destroy() // nothrow
    {
        delete this;
    }

    virtual void * get_deleter( sp_typeinfo const & ti ) = 0;
    virtual void * get_local_deleter( sp_typeinfo const & ti ) = 0;
    virtual void * get_untyped_deleter() = 0;

    void add_ref_copy()
    {
        SPI_BOOST_VERIFY( pthread_mutex_lock( &m_ ) == 0 );
        ++use_count_;
        SPI_BOOST_VERIFY( pthread_mutex_unlock( &m_ ) == 0 );
    }

    bool add_ref_lock() // true on success
    {
        SPI_BOOST_VERIFY( pthread_mutex_lock( &m_ ) == 0 );
        bool r = use_count_ == 0? false: ( ++use_count_, true );
        SPI_BOOST_VERIFY( pthread_mutex_unlock( &m_ ) == 0 );
        return r;
    }

    void release() // nothrow
    {
        SPI_BOOST_VERIFY( pthread_mutex_lock( &m_ ) == 0 );
        spi_boost::int_least32_t new_use_count = --use_count_;
        SPI_BOOST_VERIFY( pthread_mutex_unlock( &m_ ) == 0 );

        if( new_use_count == 0 )
        {
            dispose();
            weak_release();
        }
    }

    void weak_add_ref() // nothrow
    {
        SPI_BOOST_VERIFY( pthread_mutex_lock( &m_ ) == 0 );
        ++weak_count_;
        SPI_BOOST_VERIFY( pthread_mutex_unlock( &m_ ) == 0 );
    }

    void weak_release() // nothrow
    {
        SPI_BOOST_VERIFY( pthread_mutex_lock( &m_ ) == 0 );
        spi_boost::int_least32_t new_weak_count = --weak_count_;
        SPI_BOOST_VERIFY( pthread_mutex_unlock( &m_ ) == 0 );

        if( new_weak_count == 0 )
        {
            destroy();
        }
    }

    long use_count() const // nothrow
    {
        SPI_BOOST_VERIFY( pthread_mutex_lock( &m_ ) == 0 );
        spi_boost::int_least32_t r = use_count_;
        SPI_BOOST_VERIFY( pthread_mutex_unlock( &m_ ) == 0 );

        return r;
    }
};

} // namespace detail

} // namespace spi_boost

#endif  // #ifndef SPI_BOOST_SMART_PTR_DETAIL_SP_COUNTED_BASE_PT_HPP_INCLUDED
