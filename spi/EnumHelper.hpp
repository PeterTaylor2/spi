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
#ifndef SPI_ENUM_HELPER_HPP
#define SPI_ENUM_HELPER_HPP

#include "Object.hpp"
#include "Namespace.hpp"
#include "DeclSpec.h"
#include "Map.hpp"
#include <spi_util/MatrixData.hpp>

#include <string>
#include <vector>

/*
 * We implement enumerated types according to the following scheme:
 *
 * class X
 * {
 * public:
 *    enum Enum { A, B, C};
 *
 *    X(X::Enum e);
 *    X(const char* s);
 *    operator X::Enum() const;
 *    operator const char*() const;
 *
 * private:
 *    Enum e;
 * };
 *
 * In other words by putting the enum inside the class we establish a
 * namespace for the enumerands - these are now X::A, X::B etc.
 *
 * Also by providing constructor and cast operations we make it possible
 * to use either a string in quotes or X::Enum value as an input wherever
 * we expect X.
 *
 * X is to be passed by value (it has no virtual functions hence its size
 * is the same as the size of the corresponding enum).
 *
 * This file provides the inline implementation of the cast/constructor
 * methods in terms of two static methods to be implemented for each
 * enumerated type.
 */

/**
 * When using SPI_DECLARE_ENUM_TYPE you should do something like this:
 *
 * class T
 * {
 * public:
 *     enum Enum { A, B, C };
 *     SPI_DECLARE_ENUM_STRING_CONV(T);
 *
 *     T(T::Enum value) : value(value) {}
 *     operator T::Enum() const { return value; }
 * private:
 *     T::Enum value;
 * };
 *
 * The last three elements (constructor, cast operator, value) could be
 * contained within a macro, but it is easier to explain the class if you
 * can see these elements directly in the class definition.
 *
 * When implementing it you need to provide the following two functions:
 *
 * T::Enum T::from_string(const char* str);
 * const char* T::to_string(T::Enum value);
 */
#define SPI_DECLARE_ENUM_STRING_CONV(T)    \
    T() : value(UNINITIALIZED_VALUE) {} \
    T(const char* str) { value = T::from_string(str); } \
    T(const std::string& str) { value = T::from_string(str.c_str()); } \
    T(const spi::Value& value);\
    operator const char*() const { return T::to_string(value); }\
    operator std::string() const { return std::string(T::to_string(value)); } \
    operator spi::Value() const { return spi::Value(T::to_string(value)); }\
    std::string to_string() const { return std::string(T::to_string(value));}\
    static T::Enum from_string(const char*);                            \
    static const char* to_string(T::Enum)

SPI_BEGIN_NAMESPACE

class SPI_IMPORT EnumInfo
{
public:
    EnumInfo();
    void Initialise(const std::string& name,
                    const std::vector<std::string>& enumerands);

    bool Initialised() const;
    std::string Name() const;
    std::vector<std::string> Enumerands() const;

private:
    std::string m_name;
    std::vector<std::string> m_enumerands;
};

template<class T>
std::vector<std::string> EnumVectorToStringVector(
    const std::vector<T>& values)
{
    std::vector<std::string> output;
    output.reserve(values.size());
    for (size_t i = 0; i < values.size(); ++i)
        output.push_back(values[i].to_string());
    return output;
}

template<class T>
std::vector<T> EnumVectorFromStringVector(
    const std::vector<std::string>& values)
{
    std::vector<T> output;
    output.reserve(values.size());
    for (size_t i = 0; i < values.size(); ++i)
        output.push_back(T(values[i]));
    return output;
}

template<class T>
std::vector<int> EnumVectorToIntVector(
    const std::vector<T>& values)
{
    std::vector<int> output;
    output.reserve(values.size());
    for (size_t i = 0; i < values.size(); ++i)
        output.push_back(values[i].to_int());
    return output;
}

template<class T>
std::vector<T> EnumVectorFromIntVector(
    const std::vector<std::string>& values)
{
    std::vector<T> output;
    output.reserve(values.size());
    for (size_t i = 0; i < values.size(); ++i)
        output.push_back(T(values[i]));
    return output;
}

template<class T>
std::vector<T> EnumVectorFromValueVector(
    const std::vector<Value>& values,
    bool isOptional = false,
    const Value& defaultValue = Value())
{
    std::vector<T> output;
    size_t N = values.size();
    output.reserve(N);
    for (size_t i = 0; i < N; ++i)
    {
        const Value& value = values[i];
        if (value.isUndefined())
        {
            if (isOptional)
            {
                output.push_back(T(defaultValue));
            }
            else
            {
                SPI_THROW_RUNTIME_ERROR("Undefined value for EnumVector");
            }
        }
        else
        {
            output.push_back(T(value));
        }
    }
    return output;
}

template<class T>
spi_util::MatrixData<T> EnumNatrixFromValueMatrix(
    const spi_util::MatrixData<Value>& values,
    bool isOptional = false,
    const Value& defaultValue = Value())
{
    std::vector<T> v;
    size_t nbRows = values.Rows();
    size_t nbCols = values.Cols();
    size_t size = nbRows * nbCols;
    const Value* p = values.DataPointer();
    v.reserve(size);

    for (size_t i = 0; i < size; ++i)
    {
        const Value& value = p[i];
        if (value.isUndefined())
        {
            if (isOptional)
            {
                v.push_back(T(defaultValue));
            }
            else
            {
                SPI_THROW_RUNTIME_ERROR("Undefined value for EnumMatrix");
            }
        }
        else
        {
            v.push_back(T(value));
        }
    }

    return spi_util::MatrixData<T>(nbRows, nbCols, v);
}

SPI_END_NAMESPACE

#endif
