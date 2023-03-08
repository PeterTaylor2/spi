#ifndef SPI_BOOST_SMART_PTR_BAD_WEAK_PTR_HPP_INCLUDED
#define SPI_BOOST_SMART_PTR_BAD_WEAK_PTR_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  spi_boost/smart_ptr/bad_weak_ptr.hpp
//
//  Copyright (c) 2001, 2002, 2003 Peter Dimov and Multi Media Ltd.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <spi_boost/config.hpp>
#include <exception>

#ifdef __BORLANDC__
# pragma warn -8026     // Functions with excep. spec. are not expanded inline
#endif

namespace spi_boost
{

// The standard library that comes with Borland C++ 5.5.1, 5.6.4
// defines std::exception and its members as having C calling
// convention (-pc). When the definition of bad_weak_ptr
// is compiled with -ps, the compiler issues an error.
// Hence, the temporary #pragma option -pc below.

#if defined(__BORLANDC__) && __BORLANDC__ <= 0x564
# pragma option push -pc
#endif

#if defined(SPI_BOOST_CLANG)
// Intel C++ on Mac defines __clang__ but doesn't support the pragma
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wweak-vtables"
#endif

class bad_weak_ptr: public std::exception
{
public:

    virtual char const * what() const SPI_BOOST_NOEXCEPT_OR_NOTHROW
    {
        return "tr1::bad_weak_ptr";
    }
};

#if defined(SPI_BOOST_CLANG)
# pragma clang diagnostic pop
#endif

#if defined(__BORLANDC__) && __BORLANDC__ <= 0x564
# pragma option pop
#endif

} // namespace spi_boost

#ifdef __BORLANDC__
# pragma warn .8026     // Functions with excep. spec. are not expanded inline
#endif

#endif  // #ifndef SPI_BOOST_SMART_PTR_BAD_WEAK_PTR_HPP_INCLUDED
