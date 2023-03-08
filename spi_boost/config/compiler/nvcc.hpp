//  (C) Copyright Eric Jourdanneau, Joel Falcou 2010
//  Use, modification and distribution are subject to the 
//  Boost Software License, Version 1.0. (See accompanying file 
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for most recent version.

//  NVIDIA CUDA C++ compiler setup

#ifndef SPI_BOOST_COMPILER
#  define SPI_BOOST_COMPILER "NVIDIA CUDA C++ Compiler"
#endif

#if defined(__CUDACC_VER_MAJOR__) && defined(__CUDACC_VER_MINOR__) && defined(__CUDACC_VER_BUILD__)
#  define SPI_BOOST_CUDA_VERSION (__CUDACC_VER_MAJOR__ * 1000000 + __CUDACC_VER_MINOR__ * 10000 + __CUDACC_VER_BUILD__)
#else
// We don't really know what the CUDA version is, but it's definitely before 7.5:
#  define SPI_BOOST_CUDA_VERSION 7000000
#endif

// NVIDIA Specific support
// SPI_BOOST_GPU_ENABLED : Flag a function or a method as being enabled on the host and device
#define SPI_BOOST_GPU_ENABLED __host__ __device__

// A bug in version 7.0 of CUDA prevents use of variadic templates in some occasions
// https://svn.spi_boost.org/trac/spi_boost/ticket/11897
// This is fixed in 7.5. As the following version macro was introduced in 7.5 an existance
// check is enough to detect versions < 7.5
#if SPI_BOOST_CUDA_VERSION < 7050000
#   define SPI_BOOST_NO_CXX11_VARIADIC_TEMPLATES
#endif
// The same bug is back again in 8.0:
#if (SPI_BOOST_CUDA_VERSION > 8000000) && (SPI_BOOST_CUDA_VERSION < 8010000)
#   define SPI_BOOST_NO_CXX11_VARIADIC_TEMPLATES
#endif
// CUDA (8.0) has no constexpr support in msvc mode:
#if defined(_MSC_VER) && (SPI_BOOST_CUDA_VERSION < 9000000)
#  define SPI_BOOST_NO_CXX11_CONSTEXPR
#endif

#ifdef __CUDACC__
//
// When compiing .cu files, there's a bunch of stuff that doesn't work with msvc:
//
#if defined(_MSC_VER)
#  define SPI_BOOST_NO_CXX14_DIGIT_SEPARATORS
#  define SPI_BOOST_NO_CXX11_UNICODE_LITERALS
#endif
//
// And this one effects the NVCC front end,
// See https://svn.spi_boost.org/trac/spi_boost/ticket/13049
//
#if (SPI_BOOST_CUDA_VERSION >= 8000000) && (SPI_BOOST_CUDA_VERSION < 8010000)
#  define SPI_BOOST_NO_CXX11_NOEXCEPT
#endif

#endif

