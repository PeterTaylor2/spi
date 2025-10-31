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
#ifndef CPP_TYPE_HPP
#define CPP_TYPE_HPP

/*
***************************************************************************
** cppType.hpp
***************************************************************************
** Defines the CppType class used in the config parser and code generator.
***************************************************************************
*/

#include <string>

std::string CppReferenceType(
    const std::string& cppType,
    const std::string& ns=std::string());

std::string CppValueType(
    const std::string& cppType,
    const std::string& ns=std::string());

std::string CppArrayType(
    const std::string& cppType,
    int arrayDim,
    const std::string& ns=std::string());

bool CppIsReferenceType(
    const std::string& cppType);

#endif
