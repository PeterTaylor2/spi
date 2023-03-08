/*

    Sartorial Programming Interface (SPI) code generators
    Copyright (C) 2012-2023 Sartorial Programming Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/
/*
***************************************************************************
** cppType.cpp
***************************************************************************
** Implements the CppType class.
***************************************************************************
*/

#include "cppType.hpp"

#include <spi/StringUtil.hpp>
#include <spi/RuntimeError.hpp>
#include <spgtools/namespaceManager.hpp>

std::string CppReferenceType(
    const std::string& cppType,
    const std::string& ns)
{
    std::string valueType = CppValueType(cppType, ns);

    if (CppIsReferenceType(cppType))
        return spi::StringFormat("const %s&", valueType.c_str());

    return valueType;
}

std::string CppValueType(
    const std::string& cppType,
    const std::string& ns)
{
    std::string valueType;

    if (CppIsReferenceType(cppType))
    {
        valueType = cppType.substr(0, cppType.length()-1);
    }
    else
    {
        valueType = cppType;
    }
    return removeCommonNamespace(valueType, ns);
}

std::string CppArrayType(
    const std::string& cppType,
    int arrayDim,
    const std::string& ns)
{
    // FIXME: for 2+ dimension array we need to know whether we have
    // ragged arrays - for the moment we assume fixed
    std::string valueType = CppValueType(cppType, ns);

    const char* templateClass;
    switch(arrayDim)
    {
    case 1:
        templateClass = "std::vector";
        break;
    case 2:
        templateClass = "spi::MatrixData";
        break;
    default:
        throw spi::RuntimeError("Array dimension must be 1 or 2");
    }

    if (spi::StringEndsWith(valueType, ">"))
        return spi::StringFormat("%s< %s >",
            templateClass, valueType.c_str());
    return spi::StringFormat("%s<%s>",
        templateClass, valueType.c_str());
}

bool CppIsReferenceType(
    const std::string& cppType)
{
    return spi::StringEndsWith(cppType, "&");
}
