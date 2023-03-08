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
** attribute.cpp
***************************************************************************
** Implements the Attribute class and related helper functions.
***************************************************************************
*/

#include "attribute.hpp"

#include <spi/RuntimeError.hpp>
#include <spi/Service.hpp>
#include <spi/StringUtil.hpp>
#include <spi/spdoc_configTypes.hpp>

#include "constant.hpp"
#include "converter.hpp"
#include "dataType.hpp"
#include "generatorTools.hpp"
#include "verbatim.hpp"

/*
***************************************************************************
** Implementation of Attribute
***************************************************************************
*/
AttributeConstSP Attribute::Make(
    const std::vector<std::string>& description,
    const DataTypeConstSP& dataType,
    bool innerConst,
    const std::string& name,
    int arrayDim,
    bool isOptional,
    const ConstantConstSP& defaultValue)
{
    return AttributeConstSP
        (new Attribute(
            description, dataType, innerConst, name, arrayDim, isOptional,
            defaultValue));
}

AttributeConstSP Attribute::InstanceType(const DataTypeConstSP& dataType, bool innerConst)
{
    return Attribute::Make(
        std::vector<std::string>(), dataType, innerConst, "self");
}

AttributeConstSP Attribute::ReturnType(
    const std::vector<std::string>& description,
    const DataTypeConstSP& dataType, bool innerConst, int arrayDim)
{
    return Attribute::Make(
        description, dataType, innerConst, std::string(), arrayDim);
}

Attribute::Attribute(
    const std::vector<std::string>& description,
    const DataTypeConstSP& dataType,
    bool innerConst,
    const std::string& name,
    int arrayDim,
    bool isOptional,
    const ConstantConstSP& defaultValue)
    :
    m_description(description),
    m_dataType(dataType),
    m_innerConst(innerConst),
    m_name(name),
    m_arrayDim(arrayDim),
    m_isOptional(isOptional),
    m_defaultValue(defaultValue)
{
    SPI_PRE_CONDITION(arrayDim >= 0 && arrayDim <= 2);
}

const std::vector<std::string>& Attribute::description() const
{
    return m_description;
}

const DataTypeConstSP& Attribute::dataType() const
{
    return m_dataType;
}

bool Attribute::innerConst() const
{
    return m_innerConst;
}

const std::string& Attribute::name() const
{
    return m_name;
}

//bool Attribute::isArray() const
//{
//    return m_arrayDim == 1;
//}

int Attribute::arrayDim() const
{
    return m_arrayDim;
}

bool Attribute::isOptional() const
{
    return m_isOptional;
}

const ConstantConstSP Attribute::defaultValue() const
{
    return m_defaultValue;
}

AttributeConstSP Attribute::rename(const std::string& newName) const
{
    return Attribute::Make(m_description, m_dataType, m_innerConst, newName, m_arrayDim,
        m_isOptional, m_defaultValue);
}

spdoc::AttributeConstSP Attribute::getDoc() const
{
    return spdoc::Attribute::Make(m_name, m_description, m_dataType->getDoc(),
        m_arrayDim, m_isOptional,
        m_isOptional ? m_defaultValue->getDoc() : spdoc::ConstantConstSP());
}

std::string Attribute::innerType() const
{
    if (m_arrayDim)
        return m_dataType->innerArrayType(m_arrayDim);

    return m_dataType->innerValueType();
}

std::string Attribute::outerType() const
{
    if (m_arrayDim)
        return m_dataType->outerArrayType(m_arrayDim);

    return m_dataType->outerValueType();
}

bool Attribute::needsTranslation() const
{
    return m_dataType->needsTranslation();
}


/*
***************************************************************************
** Implementation of FunctionAttribute
***************************************************************************
*/
FunctionAttributeConstSP FunctionAttribute::Make(
    const AttributeConstSP& attribute,
    bool isOutput)
{
    return FunctionAttributeConstSP
        (new FunctionAttribute(attribute, isOutput));
}

FunctionAttribute::FunctionAttribute(
    const AttributeConstSP& attribute,
    bool isOutput)
    :
    m_attribute(attribute),
    m_isOutput(isOutput)
{}

const AttributeConstSP& FunctionAttribute::attribute() const
{
    return m_attribute;
}

bool FunctionAttribute::isOutput() const
{
    return m_isOutput;
}

spdoc::AttributeConstSP FunctionAttribute::getDoc() const
{
    return m_attribute->getDoc();
}

std::vector<AttributeConstSP> FunctionAttribute::getAttributeVector(
    const std::vector<FunctionAttributeConstSP>& functionAttributes)
{
    std::vector<AttributeConstSP> attributes;
    for (size_t i = 0; i < functionAttributes.size(); ++i)
        attributes.push_back(functionAttributes[i]->attribute());
    return attributes;
}

/*
***************************************************************************
** Implementation of ClassAttribute
***************************************************************************
*/
ClassAttributeConstSP ClassAttribute::Make(
    const AttributeConstSP& attribute,
    ClassAttributeAccess accessLevel,
    const VerbatimConstSP& fromInnerCode,
    bool noConvert,
    bool canHide,
    const std::string& hideIf,
    bool noCopy,
    const ConverterConstSP& converter)
{
    return ClassAttributeConstSP
        (new ClassAttribute(attribute, accessLevel, fromInnerCode, noConvert,
                            canHide, hideIf, noCopy, converter));
}

ClassAttribute::ClassAttribute(
    const AttributeConstSP& attribute,
    ClassAttributeAccess accessLevel,
    const VerbatimConstSP& fromInnerCode,
    bool noConvert,
    bool canHide,
    const std::string& hideIf,
    bool noCopy,
    const ConverterConstSP& converter)
    :
    m_attribute(attribute),
    m_accessLevel(accessLevel),
    m_fromInnerCode(fromInnerCode),
    m_noConvert(noConvert),
    m_canHide(canHide),
    m_hideIf(hideIf),
    m_noCopy(noCopy),
    m_converter(converter)
{}

const AttributeConstSP& ClassAttribute::attribute() const
{
    return m_attribute;
}

ClassAttributeAccess ClassAttribute::accessLevel() const
{
    return m_accessLevel;
}

const VerbatimConstSP& ClassAttribute::fromInnerCode() const
{
    return m_fromInnerCode;
}

bool ClassAttribute::noConvert() const
{
    if (!m_fromInnerCode)
        return false;

    return m_noConvert;
}

bool ClassAttribute::canHide() const
{
    return m_canHide;
}

const std::string& ClassAttribute::hideIf() const
{
    return m_hideIf;
}

bool ClassAttribute::noCopy() const
{
    return m_noCopy;
}

const ConverterConstSP& ClassAttribute::converter() const
{
    return m_converter;
}

spdoc::ClassAttributeConstSP ClassAttribute::getDoc(bool isStruct, bool isProperty) const
{
    bool accessible = isAccessible();
    std::string accessor;
    if (accessible)
    {
        if (isStruct)
        {
            if (isProperty)
            {
                accessor = spi::StringFormat("%s()", m_attribute->name().c_str());
            }
            else
            {
                accessor = m_attribute->name();
            }
        }
        else
        {
            accessor = spi::StringFormat(
                "%s()", m_attribute->name().c_str());
        }
    }

    return spdoc::ClassAttribute::Make(
        m_attribute->name(), m_attribute->description(),
        m_attribute->dataType()->getDoc(),
        m_attribute->arrayDim(),
        m_attribute->isOptional(),
        m_attribute->isOptional() ? m_attribute->defaultValue()->getDoc() : spdoc::ConstantConstSP(),
        accessible,
        accessor);
}

bool ClassAttribute::isAccessible() const
{
    switch(m_accessLevel)
    {
    case ClassAttributeAccess::PUBLIC:
        return true;
    case ClassAttributeAccess::PROPERTY:
        return true;
    default:
        return false;
    }
}

std::vector<AttributeConstSP> ClassAttribute::getAttributeVector(
    const std::vector<ClassAttributeConstSP>& classAttributes)
{
    std::vector<AttributeConstSP> attributes;
    for (size_t i = 0; i < classAttributes.size(); ++i)
        attributes.push_back(classAttributes[i]->attribute());
    return attributes;
}
