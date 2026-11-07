/*

    Sartorial Programming Interface (SPI) runtime libraries
    Copyright (C) 2012-2023 Sartorial Programming Ltd.

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

#include <spi_boost/assert.hpp>

#include "../RuntimeError.hpp"
#include "../FileUtil.hpp"

namespace spi_boost
{
#if defined(__GNUC__) || defined(__clang__)
__attribute__((visibility("default")))
#endif
void assertion_failed(
    char const* expr,
    char const* function,
    char const* file,
    long line)
{
#ifdef _MSC_VER
#pragma comment(linker, "/EXPORT:" __FUNCDNAME__)
#endif
    std::string baseName = spi_util::path::basename(file);
    std::string dirName = spi_util::path::basename(spi_util::path::dirname(file));
    throw spi_util::RuntimeError("Expression '%s' failed at '%s/%s:%ld'",
        expr, dirName.c_str(), baseName.c_str(), line);
}
}

