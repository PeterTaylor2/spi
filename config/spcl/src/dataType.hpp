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
#ifndef DATA_TYPE_HPP
#define DATA_TYPE_HPP

/*
***************************************************************************
** dataType.hpp
***************************************************************************
** Defines the data type classes used in the configuration parser.
***************************************************************************
*/

#include <vector>

#include <spi/Date.hpp>
#include <spi/Service.hpp>
#include <spi/spdoc_publicType.hpp>

#include "constant.hpp"

SPI_DECLARE_RC_CLASS(CppType);
SPI_DECLARE_RC_CLASS(DataType);
SPI_DECLARE_RC_CLASS(InputConverter);

SPDOC_BEGIN_NAMESPACE
SPI_DECLARE_OBJECT_CLASS(DataType);
SPDOC_END_NAMESPACE

/**
 * Data type.
 *
 * Atttributes parsed from the configuration file must have a DataType.
 * The DataType consists of an outer type and an inner type. These might
 * be the same (in which case the inner type is missing).
 *
 * If they are different then a conversion rule is implemented. We need
 * in that case to declare the type of the data type, since the types of
 * data type support are limited.
 *
 * 1. Built-in type (bool, char, int, double, string, Date)
 * 2. Enumerated types (inner type is an enum or integer type)
 * 3. Enumerated classes (inner type is a singleton class with methods)
 * 4. Simple type (outer type is a built-in type, inner type is a type
 *    with value semantics - potentially we could do Dates like this)
 * 5. Unwrapped class.
 * 6. Wrapper class.
 *
 * If there is an inner type and an outer type, then there will be a
 * standard pair of conversion routines going from type to type.
 *
 * Note that the name contains any namespaces using '.' as a separator.
 */
class DataType : public spi::RefCounter
{
public:
    static DataTypeConstSP Make(
        const std::string& name,
        const std::string& nsModule,
        const std::string& nsService,
        const std::string& cppName,
        const std::string& outerType,
        const std::string& innerType,
        const std::string& innerRefType,
        spdoc::PublicType publicType,
        const std::string& objectName,
        bool isClosed, // only for CLASS
        bool innerByValue, // only for CLASS
        bool noDoc,
        const InputConverterConstSP& convertIn,
        const std::string& convertOut = std::string(),
        const std::string& copyInner = std::string(),
        const DataTypeConstSP& primitiveType = DataTypeConstSP(),
        bool isPrimitive = false,
        bool objectAsValue = false,
        bool ignored = false);

    static DataTypeConstSP Make(
        const std::string& name,
        const std::string& nsModule,
        const std::string& nsService,
        const std::string& cppName,
        const std::string& outerType,
        const std::string& innerType,
        const std::string& innerRefType,
        spdoc::PublicType publicType,
        const std::string& objectName,
        bool isClosed, // only for CLASS
        bool innerByValue, // only for CLASS
        bool noDoc,
        const std::string& convertIn = std::string(),
        const std::string& convertOut = std::string(),
        const std::string& copyInner = std::string(),
        const DataTypeConstSP& primitiveType = DataTypeConstSP(),
        bool isPrimitive = false,
        bool objectAsValue = false,
        bool ignored = false);

    static DataTypeConstSP MakePrimitive(
        const std::string& name,
        const std::string& cppType);

    const std::string& name() const;
    const std::string& nsService() const;
    bool needsTranslation() const;
    bool isClosed() const;
    bool innerByValue() const;
    bool isPrimitive() const;
    bool noDoc() const;
    bool objectAsValue() const;
    bool ignored() const;
    bool isEqual(const DataTypeConstSP& other) const;
    std::string cppName() const;
    std::string outerValueType() const;
    std::string outerReferenceType() const;
    std::string outerArrayType(int arrayDim) const;
    std::string innerValueType() const;
    std::string innerReferenceType() const;
    std::string innerArrayType(int arrayDim) const;
    std::string validateOuter(const std::string& value, bool optional) const;
    std::string translateOuter(const std::string& value) const;
    std::string translateInner(const std::string& value,
        bool copy=false) const;

    const std::string& outerType() const;
    const std::string& innerType() const;
    const std::string& innerRefType() const;
    const InputConverterConstSP& convertIn() const;
    const std::string& convertOut() const;
    const std::string& copyInner() const;

    // these four methods might be better implemented inside Attribute
    // Attribute knows the name and whether the field is an array or not
    std::string toMapCode(
        const std::string& objMap,
        const std::string& name,
        const std::string& data,
        int arrayDim,
        bool canBeHidden=false,
        const ConstantConstSP& valueToHide=ConstantConstSP(),
        const std::string& hideIf=std::string()) const;

    std::string fromMapCode(
        const std::string& objMap,
        const std::string& valueToObject,
        const std::string& name,
        int arrayDim,
        bool isOptional,
        const ConstantConstSP& defaultValue) const;

    std::string toValueCode(
        const std::string& data,
        int arrayDim) const;

    std::string fromValueCode(
        const std::string& data,
        int arrayDim) const;

    std::string castOuterTypeToValue(
        const std::string& data,
        int arrayDim) const;

    const spdoc::DataTypeConstSP& getDoc() const;
    void setDoc(const spdoc::DataTypeConstSP& doc) const;
    spdoc::PublicType publicType() const;
    std::string objectName() const;
    std::string objectNameAlias() const;
    std::string argType() const;

protected:
    DataType(
        const std::string& name,
        const std::string& nsService,
        const std::string& cppName,
        const std::string& outerType,
        const std::string& innerType,
        const std::string& innerRefType,
        spdoc::PublicType publicType,
        const std::string& objectName,
        bool isClosed, // only for CLASS
        bool innerByValue, // only for CLASS
        bool isPrimitive,
        bool noDoc,
        const InputConverterConstSP& convertIn,
        const std::string& convertOut=std::string(),
        const std::string& copyInner=std::string(),
        const DataTypeConstSP& primitiveType=DataTypeConstSP(),
        bool objectAsValue = false,
        bool ignored = false);

private:
    std::string m_name;
    std::string m_nsService;
    std::string m_cppName;
    std::string m_outerType;
    std::string m_innerType;
    std::string m_innerRefType;
    spdoc::PublicType m_publicType;
    std::string m_objectName;
    bool m_isClosed;
    bool m_innerByValue;
    bool m_isPrimitive;
    bool m_noDoc;
    InputConverterConstSP m_convertIn;
    std::string m_convertOut;
    std::string m_copyInner;
    std::string m_fullName;
    DataTypeConstSP m_primitiveType;
    bool m_objectAsValue;
    bool m_ignored;

    mutable spdoc::DataTypeConstSP m_doc;
};

#endif
