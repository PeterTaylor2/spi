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

#ifndef _SPDOC_CONSTANT_HELPER_HPP_
#define _SPDOC_CONSTANT_HELPER_HPP_

/**
****************************************************************************
* Header file: spdoc_constant_helper.hpp
****************************************************************************
*/

#include "spdoc_constant.hpp"
#include "spdoc_dll_type_converters.hpp"

SPDOC_BEGIN_NAMESPACE

class Constant_Helper
{
public:

    virtual ~Constant_Helper() {}
    static std::string typeName(
        const ConstantConstSP& self);
    static std::string docString(
        const ConstantConstSP& self);
    static int getInt(
        const ConstantConstSP& self);
    static spi::Date getDate(
        const ConstantConstSP& self);
    static spi::DateTime getDateTime(
        const ConstantConstSP& self);
    static double getDouble(
        const ConstantConstSP& self);
    static char getChar(
        const ConstantConstSP& self);
    static std::string getString(
        const ConstantConstSP& self);
    static bool getBool(
        const ConstantConstSP& self);
};

class IntConstant_Helper
{
public:
    static std::string docString(
        const IntConstantConstSP& self);
    static std::string typeName(
        const IntConstantConstSP& self);
    static int getInt(
        const IntConstantConstSP& self);
    static double getDouble(
        const IntConstantConstSP& self);
};

class DateConstant_Helper
{
public:
    static std::string docString(
        const DateConstantConstSP& self);
    static std::string typeName(
        const DateConstantConstSP& self);
    static spi::Date getDate(
        const DateConstantConstSP& self);
};

class DateTimeConstant_Helper
{
public:
    static std::string docString(
        const DateTimeConstantConstSP& self);
    static std::string typeName(
        const DateTimeConstantConstSP& self);
    static spi::DateTime getDateTime(
        const DateTimeConstantConstSP& self);
};

class DoubleConstant_Helper
{
public:
    static std::string docString(
        const DoubleConstantConstSP& self);
    static std::string typeName(
        const DoubleConstantConstSP& self);
    static double getDouble(
        const DoubleConstantConstSP& self);
};

class CharConstant_Helper
{
public:
    static std::string docString(
        const CharConstantConstSP& self);
    static std::string typeName(
        const CharConstantConstSP& self);
    static char getChar(
        const CharConstantConstSP& self);
};

class StringConstant_Helper
{
public:
    static std::string docString(
        const StringConstantConstSP& self);
    static std::string typeName(
        const StringConstantConstSP& self);
    static std::string getString(
        const StringConstantConstSP& self);
};

class BoolConstant_Helper
{
public:
    static std::string docString(
        const BoolConstantConstSP& self);
    static std::string typeName(
        const BoolConstantConstSP& self);
    static bool getBool(
        const BoolConstantConstSP& self);
};

class UndefinedConstant_Helper
{
public:
    static std::string typeName(
        const UndefinedConstantConstSP& self);
    static std::string docString(
        const UndefinedConstantConstSP& self);
    static int getInt(
        const UndefinedConstantConstSP& self);
    static spi::Date getDate(
        const UndefinedConstantConstSP& self);
    static spi::DateTime getDateTime(
        const UndefinedConstantConstSP& self);
    static double getDouble(
        const UndefinedConstantConstSP& self);
    static char getChar(
        const UndefinedConstantConstSP& self);
    static std::string getString(
        const UndefinedConstantConstSP& self);
    static bool getBool(
        const UndefinedConstantConstSP& self);
};

void constant_register_object_types(const spi::ServiceSP& svc);

SPDOC_END_NAMESPACE

#endif /* _SPDOC_CONSTANT_HELPER_HPP_*/

