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
#ifndef SPI_SPI_HPP
#define SPI_SPI_HPP

/*
 * Groups together all the essential header files used by SPI generated
 * code. Suitable for use as a pre-compiled header.
 */

#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <tuple>

#include <stdarg.h>

#include <spi_boost/shared_ptr.hpp>

#include "platform.h"
#include "DeclSpec.h"
#include "Namespace.hpp"
#include "Date.hpp"
#include "DateTime.hpp"
#include "RefCounter.hpp"
#include "RuntimeError.hpp"
#include "Object.hpp"
#include "ObjectSmartPtr.hpp"
#include "IObjectMap.hpp"
#include "ObjectRef.hpp"
#include "Value.hpp"
#include "Variant.hpp"
#include "Function.hpp"
#include "CommonRuntime.hpp"
#include "Service.hpp"
#include "ObjectHelper.hpp"
#include "EnumHelper.hpp"
#include "StringUtil.hpp"
#include "InputValues.hpp"
#include "MatrixData.hpp"
#include "MapObject.hpp"
#include "SHA.hpp"
#include "Profile.hpp"
#include <spi_util/Utils.hpp>
#include <spi_util/LRUCache.hpp>

#endif
