/*

    Sartorial Programming Interface (SPI) runtime libraries

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA

*/

#ifndef SPI_UTIL_MUTEX_LOCK_HPP
#define SPI_UTIL_MUTEX_LOCK_HPP

#include "DeclSpec.h"
#include "Namespace.hpp"

#ifdef _MSC_VER

// for the reason for requiring _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR see
// https://stackoverflow.com/questions/78598141/first-stdmutexlock-crashes-in-application-built-with-latest-visual-studio
//
// since VS2022 17.10 (_MSC_VER 1940) std::mutex has a constexpr constructor
// which crashes on the first lock if the process has already loaded an older
// mscvp140.dll (e.g. one bundled with another python package) - so we simply
// disable it
//
// the macro must be defined on the command line since from VS18 it needs to be
// defined before xthreads.h is include, and xthreads.h is included by many
// standard headers (e.g. <sstream>, <atomic>)

#ifndef _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR
#error "_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR must be defined on the command line"
#endif

#endif

#include <mutex>

#define SPI_UTIL_GLOBAL_LOCK std::lock_guard<std::recursive_mutex> _lock_guard(spi_util::g_lock_guard)
#define SPI_UTIL_LOCAL_LOCK static std::mutex _lock; std::lock_guard<std::mutex> _lock_guard(_lock)

SPI_UTIL_NAMESPACE

SPI_UTIL_IMPORT
extern std::recursive_mutex g_lock_guard;

SPI_UTIL_END_NAMESPACE

#endif


