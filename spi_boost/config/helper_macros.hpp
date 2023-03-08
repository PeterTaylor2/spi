#ifndef SPI_BOOST_CONFIG_HELPER_MACROS_HPP_INCLUDED
#define SPI_BOOST_CONFIG_HELPER_MACROS_HPP_INCLUDED

//  Copyright 2001 John Maddock.
//  Copyright 2017 Peter Dimov.
//
//  Distributed under the Boost Software License, Version 1.0.
//
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//
//  SPI_BOOST_STRINGIZE(X)
//  SPI_BOOST_JOIN(X, Y)
//
//  Note that this header is C compatible.

//
// Helper macro SPI_BOOST_STRINGIZE:
// Converts the parameter X to a string after macro replacement
// on X has been performed.
//
#define SPI_BOOST_STRINGIZE(X) SPI_BOOST_DO_STRINGIZE(X)
#define SPI_BOOST_DO_STRINGIZE(X) #X

//
// Helper macro SPI_BOOST_JOIN:
// The following piece of macro magic joins the two
// arguments together, even when one of the arguments is
// itself a macro (see 16.3.1 in C++ standard).  The key
// is that macro expansion of macro arguments does not
// occur in SPI_BOOST_DO_JOIN2 but does in SPI_BOOST_DO_JOIN.
//
#define SPI_BOOST_JOIN(X, Y) SPI_BOOST_DO_JOIN(X, Y)
#define SPI_BOOST_DO_JOIN(X, Y) SPI_BOOST_DO_JOIN2(X,Y)
#define SPI_BOOST_DO_JOIN2(X, Y) X##Y

#endif // SPI_BOOST_CONFIG_HELPER_MACROS_HPP_INCLUDED
