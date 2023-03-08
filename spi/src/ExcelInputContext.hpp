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
#ifndef SPI_EXCEL_INPUT_CONTEXT_HPP
#define SPI_EXCEL_INPUT_CONTEXT_HPP

/**
***************************************************************************
** Defines ExcelInputContext class.
**
** This is a private header file and ExcelInputContext is returned as
** a singleton via static method InputContext::ExcelContext defined
** in InputContext.hpp
***************************************************************************
*/

#include "InputContext.hpp"

SPI_BEGIN_NAMESPACE

class ExcelInputContext : public InputContext
{
public:
    static int DATE_OFFSET;
    static bool allOptional;

    const char* Context() const;
    bool ValueToBool(
        const Value& value,
        bool isOptional,
        bool defaultValue) const;
    char ValueToChar(
        const Value& value,
        bool isOptional,
        char defaultValue) const;
    int ValueToInt(const Value& value,
        bool isOptional,
        int defaultValue) const;
    double ValueToDouble(const Value& value,
        bool isOptional,
        double defaultValue) const;
    std::string ValueToString(
        const Value& value,
        bool isOptional,
        const char* defaultValue) const;
    Date ValueToDate(
        const Value& value,
        bool isOptional,
        Date defaultValue) const;
    DateTime ValueToDateTime(
        const Value& value,
        bool optional,
        DateTime defaultValue) const;
    ObjectConstSP ValueToObject(
        const Value& value,
        ObjectType* objectType,
        bool optional) const;
    //spi::MatrixData<double> ValueToDoubleMatrix(
    //    const Value& value,
    //    bool optional) const;
	bool AcceptScalarForArray() const;
    bool StripArray() const;
};

SPI_END_NAMESPACE

#endif // SPI_XL_VALUE_HPP

