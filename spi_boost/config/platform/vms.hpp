//  (C) Copyright Artyom Beilis 2010.  
//  Use, modification and distribution are subject to the  
//  Boost Software License, Version 1.0. (See accompanying file  
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 

#ifndef SPI_BOOST_CONFIG_PLATFORM_VMS_HPP 
#define SPI_BOOST_CONFIG_PLATFORM_VMS_HPP 

#define SPI_BOOST_PLATFORM "OpenVMS" 

#undef  SPI_BOOST_HAS_STDINT_H 
#define SPI_BOOST_HAS_UNISTD_H 
#define SPI_BOOST_HAS_NL_TYPES_H 
#define SPI_BOOST_HAS_GETTIMEOFDAY 
#define SPI_BOOST_HAS_DIRENT_H 
#define SPI_BOOST_HAS_PTHREADS 
#define SPI_BOOST_HAS_NANOSLEEP 
#define SPI_BOOST_HAS_CLOCK_GETTIME 
#define SPI_BOOST_HAS_PTHREAD_MUTEXATTR_SETTYPE 
#define SPI_BOOST_HAS_LOG1P 
#define SPI_BOOST_HAS_EXPM1 
#define SPI_BOOST_HAS_THREADS 
#undef  SPI_BOOST_HAS_SCHED_YIELD 

#endif 
