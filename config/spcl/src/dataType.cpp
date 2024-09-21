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
** dataType.cpp
***************************************************************************
** Implements the data type classes.
***************************************************************************
*/

#include "dataType.hpp"

#include <spi/RuntimeError.hpp>
#include <spi/Service.hpp>
#include <spi/StringUtil.hpp>
#include <spi/spdoc_configTypes.hpp>

#include "constant.hpp"
#include "cppType.hpp"
#include "generatorTools.hpp"
#include <spgtools/commonTools.hpp>
#include "inputConverter.hpp"

/*
***************************************************************************
** Implementation of DataType
***************************************************************************
*/

DataTypeConstSP DataType::Make(
    const std::string&     name,
    const std::string&     nsModule,
    const std::string&     nsService,
    const std::string&     cppName,
    const std::string&     outerType,
    const std::string&     innerType,
    const std::string&     innerRefType,
    spdoc::PublicType      publicType,
    const std::string&     objectName,
    bool                   isClosed,
    bool                   noDoc,
    const InputConverterConstSP& convertIn,
    const std::string&     convertOut,
    const std::string&     copyInner,
    const DataTypeConstSP& primitiveType,
    bool                   isPrimitive,
    bool                   objectAsValue,
    bool                   ignored)
{
    const std::string& fullName = nsModule.empty() ? name : nsModule + "." + name;

    return DataTypeConstSP(new DataType(
        fullName, nsService, cppName, outerType, innerType, innerRefType,
        publicType, objectName, isClosed, isPrimitive, noDoc,
        convertIn, convertOut, copyInner, primitiveType, objectAsValue, ignored));
}

DataTypeConstSP DataType::Make(
    const std::string&     name,
    const std::string&     nsModule,
    const std::string&     nsService,
    const std::string&     cppName,
    const std::string&     outerType,
    const std::string&     innerType,
    const std::string&     innerRefType,
    spdoc::PublicType      publicType,
    const std::string&     objectName,
    bool                   isClosed,
    bool                   noDoc,
    const std::string&     convertIn,
    const std::string&     convertOut,
    const std::string&     copyInner,
    const DataTypeConstSP& primitiveType,
    bool                   isPrimitive,
    bool                   objectAsValue,
    bool                   ignored)
{
    const std::string& fullName = nsModule.empty() ? name : nsModule + "." + name;

    return DataTypeConstSP(new DataType(
        fullName, nsService, cppName, outerType, innerType, innerRefType,
        publicType, objectName, isClosed, isPrimitive, noDoc,
        convertIn.empty() ? InputConverterConstSP() : InputConverter::StringFormat(convertIn),
        convertOut, copyInner, primitiveType, objectAsValue, ignored));
}

DataTypeConstSP DataType::MakePrimitive(
    const std::string& name,
    const std::string& cppType)
{
    spdoc::PublicType::Enum publicType =
        spdoc::verifyPrimitiveTypeName(cppType);

    std::string nsService;
    std::string innerType;
    std::string innerRefType;
    std::string objectName;
    bool isClosed = false;
    bool isPrimitive = true;
    bool noDoc = true;
    InputConverterConstSP convertIn;
    std::string convertOut;

    if (name == "Map")
    {
        innerType = "spi::MapConstSP";
        innerRefType = "const spi::MapConstSP&";
        objectName = "Map";
        convertIn = InputConverter::StringFormat("spi::MapObject::get_inner(%s)");
        convertOut = "spi::MapObject::Make(%s)";
    }

    return DataTypeConstSP(
        new DataType(name, nsService, cppType, cppType, innerType, innerRefType, publicType,
                     objectName, isClosed, isPrimitive, noDoc,
                     convertIn, convertOut));
}

DataType::DataType(
    const std::string& name,
    const std::string& nsService,
    const std::string& cppName,
    const std::string& outerType,
    const std::string& innerType,
    const std::string& innerRefType,
    spdoc::PublicType publicType,
    const std::string& objectName,
    bool isClosed,
    bool isPrimitive,
    bool noDoc,
    const InputConverterConstSP& convertIn,
    const std::string& convertOut,
    const std::string& copyInner,
    const DataTypeConstSP& primitiveType,
    bool objectAsValue,
    bool ignored)
    :
    m_name(name),
    m_nsService(nsService),
    m_cppName(cppName),
    m_outerType(outerType),
    m_innerType(innerType),
    m_innerRefType(innerRefType),
    m_publicType(publicType),
    m_objectName(objectName),
    m_isClosed(isClosed),
    m_isPrimitive(isPrimitive),
    m_noDoc(noDoc),
    m_convertIn(convertIn),
    m_convertOut(convertOut),
    m_copyInner(copyInner),
    m_primitiveType(primitiveType),
    m_objectAsValue(objectAsValue),
    m_ignored(ignored)
{}

const std::string& DataType::name() const
{
    return m_name;
}

const std::string& DataType::nsService() const
{
    return m_nsService;
}

bool DataType::needsTranslation() const
{
    return m_innerType.length() > 0;
}

bool DataType::isClosed() const
{
    return m_isClosed;
}

bool DataType::isPrimitive() const
{
    return m_isPrimitive;
}

bool DataType::noDoc() const
{
    return m_noDoc;
}

bool DataType::objectAsValue() const
{
    return m_objectAsValue;
}

bool DataType::isEqual(const DataTypeConstSP& other) const
{
    if (!other) return false;

    const DataType* p = other.get();

    if (p == this) return true;

    if (m_name != p->m_name) return false;
    if (m_nsService != p->m_nsService) return false;
    if (m_cppName != p->m_cppName) return false;
    if (m_outerType != p->m_outerType) return false;
    if (m_innerType != p->m_innerType) return false;
    if (m_innerRefType != p->m_innerRefType) return false;
    if (m_publicType != p->m_publicType) return false;
    if (m_objectName != p->m_objectName) return false;
    if (m_isClosed != p->m_isClosed) return false;
    if (m_isPrimitive != p->m_isPrimitive) return false;
    if (m_noDoc != p->m_noDoc) return false;
    if (m_convertIn)
    {
        if (!(m_convertIn->isEqual(p->m_convertIn))) return false;
    }
    else if (p->m_convertIn)
    {
        return false;
    }
    if (m_convertOut != p->m_convertOut) return false;
    if (m_copyInner != p->m_copyInner) return false;
    if (m_fullName != p->m_fullName) return false;
    if (m_primitiveType)
    {
        if (!(m_primitiveType->isEqual(p->m_primitiveType))) return false;
    }
    else if (p->m_primitiveType)
    {
        return false;
    }
    if (m_objectAsValue != p->m_objectAsValue) return false;
    if (m_ignored != p->m_ignored) return false;

    return true;
}

bool DataType::ignored() const
{
    return m_ignored;
}

const std::string& DataType::outerType() const
{
    return m_outerType;
}

const std::string& DataType::innerType() const
{
    return m_innerType;
}

const std::string& DataType::innerRefType() const
{
    return m_innerRefType;
}

const InputConverterConstSP& DataType::convertIn() const
{
    return m_convertIn;
}

const std::string& DataType::convertOut() const
{
    return m_convertOut;
}

const std::string& DataType::copyInner() const
{
    return m_copyInner;
}

std::string DataType::cppName() const
{
    return m_cppName;
    //return spi::StringJoin("::", spi::StringSplit(m_name, '.'));
}

std::string DataType::outerValueType() const
{
    return CppValueType(m_outerType);
}

std::string DataType::outerReferenceType() const
{
    return CppReferenceType(m_outerType);
}

std::string DataType::outerArrayType(int arrayDim) const
{
    return CppArrayType(m_outerType, arrayDim);
}

std::string DataType::innerValueType() const
{
    if (m_innerType.empty())
        return outerValueType();

    return CppValueType(m_innerType);
}

std::string DataType::innerReferenceType() const
{
    if (m_innerRefType.empty())
        return outerReferenceType();

    return m_innerRefType;
}

std::string DataType::innerArrayType(int arrayDim) const
{
    if (m_innerType.empty())
        return outerArrayType(arrayDim);

    return CppArrayType(m_innerType, arrayDim);
}


std::string DataType::validateOuter(const std::string& data, bool optional) const
{
    if (!m_convertIn)
        return std::string();

    return m_convertIn->Validation(data, optional);
}

std::string DataType::translateOuter(const std::string& data) const
{
    if (!m_convertIn)
        return data;

    return m_convertIn->Conversion(data);
}

std::string DataType::translateInner(const std::string& data, bool copy) const
{
    if (m_convertOut.empty())
        return data;

    if (copy && !m_copyInner.empty())
    {
        std::string dataCopy = spi::StringFormat(
            m_copyInner.c_str(), data.c_str());

        return spi::StringFormat(m_convertOut.c_str(), dataCopy.c_str());
    }

    return spi::StringFormat(m_convertOut.c_str(), data.c_str());
}

std::string DataType::objectNameAlias() const
{
    if (m_publicType != spdoc::PublicType::CLASS)
        throw spi::RuntimeError("objectName only defined for classes");
    if (m_objectName.length() > 0)
        return m_objectName;
    return m_name;
}

std::string DataType::objectName() const
{
    return m_objectName;
}

std::string DataType::toMapCode(
    const std::string& objMap,
    const std::string& fname,
    const std::string& data,
    int arrayDim,
    bool canBeHidden,
    const ConstantConstSP& valueToHide,
    const std::string& hideIf) const
{
    std::ostringstream oss;
    std::ostringstream hideCondition;

    if (canBeHidden)
    {
        switch(arrayDim)
        {
        case 1: // vector
            hideCondition << data << ".size() == 0";
            break;
        case 2: // matrix
            hideCondition << "!" << data;
            break;
        default:
            // set for each scalar type
            break;
        }
    }

    switch(m_publicType)
    {
    case spdoc::PublicType::BOOL:
        oss << objMap << "->" << "SetBool";
        if (arrayDim == 1)
            oss << "Vector";
        else if (arrayDim == 2)
            oss << "Matrix";
        else if (canBeHidden)
        {
            if (valueToHide)
                hideCondition << data << " == " << valueToHide->toCode(m_publicType);
            else
                hideCondition << "!" << data;
        }
        break;
    case spdoc::PublicType::CHAR:
        oss << objMap << "->" << "SetChar";
        if (arrayDim > 0)
            throw spi::RuntimeError("Array of char is not supported");
        if (canBeHidden)
        {
            hideCondition << data << " == ";
            if (valueToHide)
                hideCondition << valueToHide->toCode(m_publicType);
            else
                hideCondition << "'\0'";
        }
        break;
    case spdoc::PublicType::INT:
        oss << objMap << "->" << "SetInt";
        if (arrayDim == 1)
            oss << "Vector";
        else if (arrayDim == 2)
            oss << "Matrix";
        else if (canBeHidden)
        {
            hideCondition << data << " == ";
            if (valueToHide)
                hideCondition << valueToHide->toCode(m_publicType);
            else
                hideCondition << "0";
        }
        break;
    case spdoc::PublicType::DOUBLE:
        oss << objMap << "->" << "SetDouble";
        if (arrayDim == 1)
            oss << "Vector";
        else if (arrayDim == 2)
            oss << "Matrix";
        else if (canBeHidden)
        {
            hideCondition << data << " == ";
            if (valueToHide)
                hideCondition << valueToHide->toCode(m_publicType);
            else
                hideCondition << "0.0";
        }
        break;
    case spdoc::PublicType::STRING:
        oss << objMap << "->" << "SetString";
        if (arrayDim == 1)
            oss << "Vector";
        else if (arrayDim == 2)
            oss << "Matrix";
        else if (canBeHidden)
        {
            if (valueToHide)
            {
                std::string code = spi::StringStrip(valueToHide->toCode(m_publicType));
                if (code == "std::string()" || code == "\"\"")
                {
                    hideCondition << data << ".empty()";
                }
                else if (spi::StringStartsWith(code, "\""))
                {
                    hideCondition << data << " == std::string(" << code << ")";
                }
                else if (spi::StringStartsWith(code, "std::string"))
                {
                    hideCondition << data << " == " << code;
                }
                else
                {
                    SPI_THROW_RUNTIME_ERROR("Unexpected valueToHide '" << code << "'");
                }
            }
            else
            {
                SPI_THROW_RUNTIME_ERROR("No valueToHide provided")
            }
        }
        break;
    case spdoc::PublicType::DATE:
        oss << objMap << "->" << "SetDate";
        if (arrayDim == 1)
            oss << "Vector";
        else if (arrayDim == 2)
            oss << "Matrix";
        else if (canBeHidden)
            hideCondition << "!" << data;
        break;
    case spdoc::PublicType::DATETIME:
        oss << objMap << "->" << "SetDateTime";
        if (arrayDim == 1)
            oss << "Vector";
        else if (arrayDim == 2)
            oss << "Matrix";
        else if (canBeHidden)
            hideCondition << "!" << data;
        break;
    case spdoc::PublicType::ENUM:
        oss << objMap;
        if (arrayDim == 1)
        {
            oss << "->SetEnumVector<" << m_name << ">";
        }
        else if (arrayDim == 2)
        {
            oss << "->SetEnumMatrix<" << m_name << ">";
        }
        else
        {
            // automatic type coercion will get the string
            oss << "->SetString";
            // TBA: could we do better knowing we have an enumerated type?
            if (canBeHidden)
            {
                if (valueToHide)
                {
                    std::string code = spi::StringStrip(valueToHide->toCode(m_publicType));
                    if (spi::StringStartsWith(code, "\""))
                    {
                        // we need to apply the cast-operator on the enumerated type class
                        // applying on the valueToHide doesn't work
                        hideCondition << "(std::string)" << data << " == " << code;
                    }
                    else
                    {
                        hideCondition << data << " == " << code;
                    }
                }
            }
        }
        break;
    case spdoc::PublicType::CLASS:
        oss << objMap;
        if (arrayDim == 1)
        {
            oss << "->SetInstanceVector<" << cppName() << " const>";
        }
        else if (arrayDim == 2)
        {
            oss << "->SetInstanceMatrix<" << cppName() << " const>";
        }
        else
        {
            oss << "->SetObject";
            if (canBeHidden)
                hideCondition << "!" << data;
        }
        break;
    case spdoc::PublicType::MAP:
        oss << objMap;
        if (arrayDim == 1)
        {
            oss << "->SetInstanceVector<spi::MapObject>";
        }
        else if (arrayDim == 2)
        {
            oss << "->SetInstanceMatrix<spi::MapObject>";
        }
        else
        {
            oss << "->SetObject";
            if (canBeHidden)
                hideCondition << "!" << data;
        }
        break;
    case spdoc::PublicType::OBJECT:
        oss << objMap;
        if (arrayDim == 1)
        {
            oss << "->SetObjectVector";
        }
        else if (arrayDim == 2)
        {
            oss << "->SetObjectMatrix";
        }
        else
        {
            oss << "->SetObject";
            if (canBeHidden)
                hideCondition << "!" << data;
        }
        break;
    case spdoc::PublicType::VARIANT:
        oss << objMap << "->" << "SetVariant";
        if (arrayDim == 1)
        {
            oss << "Vector";
        }
        else if (arrayDim == 2)
        {
            oss << "Matrix";
        }
        else if (canBeHidden)
        {
            hideCondition << data << ".IsUndefined()";
        }
        break;

    default:
        throw spi::RuntimeError("%s: unknown enum value %d", __FUNCTION__, (int)m_publicType);
    }
    oss << "(\"" << fname << "\", " << data;

    if (canBeHidden || hideIf.length() > 0)
    {
        // assumes that we still want "OBJECT_GET" to work on hidden values
        //
        // public_only => using "OBJECT_GET"
        // !public_only => serializing
        oss << ", !public_only && (";
        if (hideIf.length() > 0)
            // user-defined hideIf trumps any hideCondition based on default
            oss << hideIf;
        else
            oss << hideCondition.str();
        oss << ")";
        //if (useValueToHide && valueToHide)
        //{
        //    oss << ", " << valueToHide->toCode(publicType);
        //}
    }

    oss << ")";

    return oss.str();
}

std::string DataType::fromMapCode(
    const std::string& objMap,
    const std::string& valueToObject,
    const std::string& fname,
    int arrayDim,
    bool isOptional,
    const ConstantConstSP& defaultValue) const
{
    std::ostringstream oss;

    switch(m_publicType)
    {
    case spdoc::PublicType::BOOL:
        oss << objMap << "->" << "GetBool";
        if (arrayDim == 1)
            oss << "Vector";
        else if (arrayDim == 2)
            oss << "Matrix";
        break;
    case spdoc::PublicType::CHAR:
        oss << objMap << "->" << "GetChar";
        if (arrayDim > 0)
            throw spi::RuntimeError("Array of char is not supported");
        break;
    case spdoc::PublicType::INT:
        oss << objMap << "->" << "GetInt";
        if (arrayDim == 1)
            oss << "Vector";
        else if (arrayDim == 2)
            oss << "Matrix";
        break;
    case spdoc::PublicType::DOUBLE:
        oss << objMap << "->" << "GetDouble";
        if (arrayDim == 1)
            oss << "Vector";
        else if (arrayDim == 2)
            oss << "Matrix";
        break;
    case spdoc::PublicType::STRING:
        oss << objMap << "->" << "GetString";
        if (arrayDim == 1)
            oss << "Vector";
        else if (arrayDim == 2)
            oss << "Matrix";
        break;
    case spdoc::PublicType::DATE:
        oss << objMap << "->" << "GetDate";
        if (arrayDim == 1)
            oss << "Vector";
        else if (arrayDim == 2)
            oss << "Matrix";
        break;
    case spdoc::PublicType::DATETIME:
        oss << objMap << "->" << "GetDateTime";
        if (arrayDim == 1)
            oss << "Vector";
        else if (arrayDim == 2)
            oss << "Matrix";
        break;
    case spdoc::PublicType::ENUM:
        oss << objMap;
        if (arrayDim == 1)
        {
            oss << "->GetEnumVector<" << m_name << ">";
        }
        else if (arrayDim == 2)
        {
            oss << "->GetEnumMatrix<" << m_name << ">";
        }
        else
        {
            // automatic type coercion will convert from string
            oss << "->GetString";
        }
        break;
    case spdoc::PublicType::CLASS:
        oss << objMap;
        if (arrayDim == 1)
        {
            oss << "->GetInstanceVector<" << cppName() << " const>";
        }
        else if (arrayDim == 2)
        {
            oss << "->GetInstanceMatrix<" << cppName() << " const>";
        }
        else
        {
            oss << "->GetInstance<" << cppName() << " const>";
        }
        break;
    case spdoc::PublicType::MAP:
        oss << objMap;
        if (arrayDim == 1)
        {
            oss << "->GetInstanceVector<spi::MapObject>";
        }
        else if (arrayDim == 2)
        {
            oss << "->GetInstanceMatrix<spi::MapObject>";
        }
        else
        {
            oss << "->GetInstance<spi::MapObject>";
        }
        break;
    case spdoc::PublicType::OBJECT:
        oss << objMap;
        if (arrayDim == 1)
        {
            oss << "->GetObjectVector";
        }
        else if (arrayDim == 2)
        {
            oss << "->GetObjectMatrix";
        }
        else
        {
            oss << "->GetObject";
        }
        break;

    case spdoc::PublicType::VARIANT:
        oss << objMap << "->" << "GetVariant";
        if (arrayDim == 1)
            oss << "Vector";
        else if (arrayDim == 2)
            oss << "Matrix";
        break;

    default:
        throw spi::RuntimeError("%s: unknown enum value %d", __FUNCTION__, (int)m_publicType);
    }

    oss << "(\"" << fname << "\"";

    switch(m_publicType)
    {
    case spdoc::PublicType::CLASS:
    case spdoc::PublicType::MAP:
    case spdoc::PublicType::VARIANT:
        oss << ", " << valueToObject;
        break;
    case spdoc::PublicType::OBJECT:
        oss << ", NULL, " << valueToObject;
        break;
    default:
        break;
    }

    if (isOptional)
    {
        if (arrayDim == 0)
        {
            oss << ", true";
            if (defaultValue)
            {
                switch(m_publicType)
                {
                case spdoc::PublicType::BOOL:
                case spdoc::PublicType::CHAR:
                case spdoc::PublicType::INT:
                case spdoc::PublicType::DOUBLE:
                case spdoc::PublicType::ENUM:
                    oss << ", " << defaultValue->toCode(m_publicType);
                    break;
                case spdoc::PublicType::STRING:
                {
                    std::string str = spi_util::StringStrip(defaultValue->toCode(m_publicType));
                    if (str != "std::string()" && str != "\"\"")
                        oss << ", " << str;
                    break;
                }
                case spdoc::PublicType::DATE: // FIXME - is this right?
                case spdoc::PublicType::DATETIME:
                case spdoc::PublicType::CLASS:
                case spdoc::PublicType::MAP:
                case spdoc::PublicType::OBJECT:
                case spdoc::PublicType::VARIANT:
                    // defaultValue not used
                    break;
                default:
                    throw spi::RuntimeError("%s: unknown enum value %d", __FUNCTION__, (int)m_publicType);
                }
            }
        }
        else
        {
            // only objects can have isOptional for arrays
            //
            // we can have optional elements of arrays for input for simple
            // types but these are resolved before we ever get to writing
            // a map
            switch(m_publicType)
            {
            case spdoc::PublicType::BOOL:
            case spdoc::PublicType::CHAR:
            case spdoc::PublicType::INT:
            case spdoc::PublicType::DOUBLE:
            case spdoc::PublicType::STRING:
            case spdoc::PublicType::ENUM:
            case spdoc::PublicType::DATE:
            case spdoc::PublicType::DATETIME:
                break;
            case spdoc::PublicType::CLASS:
            case spdoc::PublicType::MAP:
            case spdoc::PublicType::OBJECT:
            case spdoc::PublicType::VARIANT:
                oss << ", true";
                break;
            default:
                throw spi::RuntimeError("%s: unknown enum value %d", __FUNCTION__, (int)m_publicType);
            }
        }
    }

    oss << ")";
    return oss.str();
}

std::string DataType::toValueCode(
    const std::string& data,
    int arrayDim) const
{
    std::ostringstream oss;

    switch(m_publicType)
    {
    case spdoc::PublicType::BOOL:
        return data;
    case spdoc::PublicType::CHAR:
        if (arrayDim > 0)
            throw spi::RuntimeError("Array of char is not supported");
        return data;
    case spdoc::PublicType::INT:
        return data;
    case spdoc::PublicType::DOUBLE:
        return data;
    case spdoc::PublicType::STRING:
        return data;
    case spdoc::PublicType::DATE:
        return data;
    case spdoc::PublicType::DATETIME:
        return data;
    case spdoc::PublicType::ENUM:
        switch (arrayDim)
        {
        case 1:
            oss << "spi::EnumVectorToStringVector(" << data << ")";
            return oss.str();
        case 0:
            return data;
        default:
            SPI_THROW_RUNTIME_ERROR("Array of enum (dim=" << arrayDim << ") is not supported");
        }
        break;
    case spdoc::PublicType::CLASS:
    case spdoc::PublicType::MAP:
        if (m_objectAsValue)
        {
            oss << "spi::Object::to_value(";
        }
        if (arrayDim == 1)
        {
            oss << "spi::MakeObjectVector<" << cppName() << ">("
                << data << ")";
        }
        else if (arrayDim == 2)
        {
            throw spi::RuntimeError("Matrix of Object is not supported");
            //oss << "spi::MakeObjectMatrix(" << data << ")";
        }
        else
        {
            oss << "spi::ObjectConstSP(" << data << ")";
        }
        if (m_objectAsValue)
        {
            oss << ")";
        }
        return oss.str();
    case spdoc::PublicType::VARIANT:
        if (arrayDim == 0)
            oss << data << ".GetValue()";
        else if (arrayDim == 1)
            oss << "spi::Variant::VectorToValue(" << data << ")";
        else
            oss << "spi::Variant::MatrixToValue(" << data << ")";
        return oss.str();
    case spdoc::PublicType::OBJECT:
        return data;
    default:
        throw spi::RuntimeError("%s: unknown enum value %d", __FUNCTION__, (int)m_publicType);
    }
    throw spi::RuntimeError("PROGRAM_BUG");
}

std::string DataType::fromValueCode(
    const std::string& data,
    int arrayDim) const
{
    return TranslateFromValue(m_publicType, m_name, data, arrayDim);
}

std::string DataType::castOuterTypeToValue(
    const std::string& data,
    int arrayDim) const
{
    std::ostringstream oss;

    switch(m_publicType)
    {
    case spdoc::PublicType::BOOL:
    case spdoc::PublicType::INT:
    case spdoc::PublicType::DOUBLE:
    case spdoc::PublicType::STRING:
    case spdoc::PublicType::DATE:
    case spdoc::PublicType::DATETIME:
    case spdoc::PublicType::OBJECT:
        if (arrayDim == 2)
            oss << "spi::Value(" << data << ".ToArray())";
        else
            oss << "spi::Value(" << data << ")";
        break;
    case spdoc::PublicType::ENUM:
        if (arrayDim > 1)
            throw spi::RuntimeError("Matrix of enum is not supported");
        if (arrayDim == 1)
            oss << "spi::Value(spi::EnumVectorToStringVector(" << data << "))";
        else
            oss << "spi::Value(" << data << ".to_string())";
        break;
    case spdoc::PublicType::CHAR:
        if (arrayDim > 0)
            throw spi::RuntimeError("Array of char is not supported");
        oss << "(spi::Value)" << data;
        break;
    case spdoc::PublicType::CLASS:
    case spdoc::PublicType::MAP:
        if (arrayDim == 1)
        {
            oss << "spi::Value(spi::MakeObjectVector<" << cppName()
                << ">(" << data << "))";
        }
        else if (arrayDim == 2)
        {
            throw spi::RuntimeError("Matrix of Object is not supported");
            //oss << "spi::MakeObjectMatrix(" << data << ")";
        }
        else
        {
            oss << "spi::Object::to_value(" << data << ")";
        }
        break;
    case spdoc::PublicType::VARIANT:
        if (arrayDim == 0)
            oss << data << ".GetValue()";
        else if (arrayDim == 1)
            oss << "spi::Variant::VectorToValue(" << data << ")";
        else
            oss << "spi::Variant::MatrixToValue(" << data << ")";
        break;
    default:
        throw spi::RuntimeError("%s: unknown enum value %d", __FUNCTION__, (int)m_publicType);
    }

    return oss.str();
}

const spdoc::DataTypeConstSP& DataType::getDoc() const
{
    if (!m_doc)
    {
        if (m_primitiveType)
        {
            m_doc = m_primitiveType->getDoc();
        }
        else
        {
            std::string docName;
            if (m_isPrimitive && m_name == "date")
                docName = "Date";
            else if (m_isPrimitive && m_name == "datetime")
                docName = "DateTime";
            else
                docName = m_name;

            m_doc = spdoc::DataType::Make(
                docName,
                CppIsReferenceType(m_outerType),
                outerReferenceType(),
                outerValueType(),
                m_publicType,
                m_noDoc,
                m_nsService, // FIXME
                m_objectAsValue);
        }
    }
    return m_doc;
}

void DataType::setDoc(const spdoc::DataTypeConstSP& doc) const
{
    if (this->m_doc)
        SPI_THROW_RUNTIME_ERROR("doc is already defined");

    if (!doc)
        SPI_THROW_RUNTIME_ERROR("doc is not defined");

    this->m_doc = doc;
}

spdoc::PublicType DataType::publicType() const
{
    return m_publicType;
}

std::string DataType::argType() const
{
    switch (m_publicType)
    {
    case spdoc::PublicType::BOOL:
        return "BOOL";
    case spdoc::PublicType::CHAR:
        return "CHAR";
    case spdoc::PublicType::INT:
        return "INT";
    case spdoc::PublicType::DOUBLE:
        return "DOUBLE";
    case spdoc::PublicType::STRING:
        return "STRING";
    case spdoc::PublicType::DATE:
        return "DATE";
    case spdoc::PublicType::DATETIME:
        return "DATETIME";
    case spdoc::PublicType::ENUM:
        return "ENUM";
    case spdoc::PublicType::CLASS:
    case spdoc::PublicType::OBJECT:
    case spdoc::PublicType::MAP:
        return "OBJECT";
    case spdoc::PublicType::VARIANT:
        return "VARIANT";
    default:
        throw spi::RuntimeError("%s: unknown enum value %d", __FUNCTION__, (int)m_publicType);
    }
}
