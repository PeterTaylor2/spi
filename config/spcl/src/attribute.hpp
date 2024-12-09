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
#ifndef ATTRIBUTE_HPP
#define ATTRIBUTE_HPP

/*
***************************************************************************
** attribute.hpp
***************************************************************************
** Defines the Attribute class and related helper functions.
***************************************************************************
*/

#include <vector>

#include <spi/Date.hpp>
#include <spi/Service.hpp>

#include "constant.hpp"

SPI_DECLARE_RC_CLASS(DataType);
SPI_DECLARE_RC_CLASS(Attribute);
SPI_DECLARE_RC_CLASS(ClassAttribute);
SPI_DECLARE_RC_CLASS(FunctionAttribute);
SPI_DECLARE_RC_CLASS(Verbatim);
SPI_DECLARE_RC_CLASS(Converter);

SPDOC_BEGIN_NAMESPACE
SPI_DECLARE_OBJECT_CLASS(Attribute);
SPI_DECLARE_OBJECT_CLASS(ClassAttribute);
SPDOC_END_NAMESPACE

/**
 * Defines a single attribute.
 *
 * This has data type and name. It also has description to be used as needed.
 */
class Attribute : public spi::RefCounter
{
public:
    //friend class ClassAttribute;

    static AttributeConstSP Make(
        const std::vector<std::string>& description,
        const DataTypeConstSP& dataType,
        const std::string& name,
        int arrayDim=0,
        bool isOptional=false,
        const ConstantConstSP& defaultValue=Constant::UNDEFINED);

    static AttributeConstSP InstanceType(
        const DataTypeConstSP& dataType);

    static AttributeConstSP ReturnType(
        const std::vector<std::string>& description,
        const DataTypeConstSP& dataType,
        int arrayDim,
        bool isOptional);

    const std::vector<std::string>& description() const;
    const DataTypeConstSP& dataType() const;
    const std::string& name() const;
    int arrayDim() const;
    //bool isArray() const;
    bool isOptional() const;
    const ConstantConstSP defaultValue() const;

    AttributeConstSP rename(const std::string& newName) const;
    spdoc::AttributeConstSP getDoc() const;

    std::string innerType() const;
    std::string outerType() const;
    bool needsTranslation() const;

    bool checkNonNull() const;

protected:
    Attribute(
        const std::vector<std::string>& description,
        const DataTypeConstSP& dataType,
        const std::string& name,
        int arrayDim,
        bool isOptional,
        const ConstantConstSP& defaultValue);

private:
    std::vector<std::string> m_description;
    DataTypeConstSP          m_dataType;
    std::string              m_name;
    int                      m_arrayDim;
    bool                     m_isOptional;
    const ConstantConstSP    m_defaultValue;
};

/**
 * Defines a single attribute of a function call.
 *
 * This is an attribute plus a flag to denote input or output.
 */
class FunctionAttribute : public spi::RefCounter
{
public:
    static FunctionAttributeConstSP Make(
        const AttributeConstSP& attribute,
        bool isOutput);

    const AttributeConstSP& attribute() const;
    bool isOutput() const;
    spdoc::AttributeConstSP getDoc() const;

    static std::vector<AttributeConstSP> getAttributeVector(
        const std::vector<FunctionAttributeConstSP>& functionAttributes);

protected:
    FunctionAttribute(
        const AttributeConstSP& attribute,
        bool isOutput);

private:
    AttributeConstSP m_attribute;
    bool m_isOutput;
};

class ClassAttributeAccess
{
public:
    enum Enum
    {
        PRIVATE,
        PUBLIC,
        PROPERTY
    };

    ClassAttributeAccess(ClassAttributeAccess::Enum value) : value(value) {};
    operator ClassAttributeAccess::Enum() const { return value; }
private:
    ClassAttributeAccess::Enum value;

};

/**
 * Defines a single attribute of a class.
 *
 * This is an attribute, plus access level, plus (optional) code used
 * to fetch the attribute from an inner class.
 */
class ClassAttribute : public spi::RefCounter
{
public:
    static ClassAttributeConstSP Make(
        const AttributeConstSP& attribute,
        ClassAttributeAccess accessLevel,
        const VerbatimConstSP& fromInnerCode,
        bool noConvert,
        bool canHide,
        const std::string& hideIf,
        bool noCopy,
        const ConverterConstSP& converter);

    const AttributeConstSP& attribute() const;
    ClassAttributeAccess accessLevel() const;
    const VerbatimConstSP& fromInnerCode() const;
    spdoc::ClassAttributeConstSP getDoc(bool isStruct, bool isProperty) const;
    const ConverterConstSP& converter() const;
    bool isAccessible() const;
    bool noConvert() const;
    bool canHide() const;
    const std::string& hideIf() const;
    bool noCopy() const;

    static std::vector<AttributeConstSP> getAttributeVector(
        const std::vector<ClassAttributeConstSP>& classAttributes);

protected:
    ClassAttribute(
        const AttributeConstSP& attribute,
        ClassAttributeAccess accessLevel,
        const VerbatimConstSP& fromInnerCode,
        bool noConvert,
        bool canHide,
        const std::string& hideIf,
        bool noCopy,
        const ConverterConstSP& converter);

private:
    AttributeConstSP m_attribute;
    ClassAttributeAccess m_accessLevel;
    VerbatimConstSP m_fromInnerCode;
    bool m_noConvert;
    bool m_canHide;
    std::string m_hideIf;
    bool m_noCopy;
    ConverterConstSP m_converter;
};


#endif
