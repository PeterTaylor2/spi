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
#ifndef STRUCT_HPP
#define STRUCT_HPP

/*
***************************************************************************
** struct.hpp
***************************************************************************
** Defines the Struct classes used in the configuration parser.
***************************************************************************
*/

#include "class.hpp"

#include <vector>
#include <string>

SPI_DECLARE_RC_CLASS(Attribute);
SPI_DECLARE_RC_CLASS(ClassAttribute);
SPI_DECLARE_RC_CLASS(Verbatim);
SPI_DECLARE_RC_CLASS(ClassMethod);
SPI_DECLARE_RC_CLASS(CoerceFrom);
SPI_DECLARE_RC_CLASS(CoerceTo);
SPI_DECLARE_RC_CLASS(Struct);
SPI_DECLARE_RC_CLASS(Converter);

#include "attribute.hpp"

SPDOC_BEGIN_NAMESPACE
SPI_DECLARE_OBJECT_CLASS(Class);
SPDOC_END_NAMESPACE

/**
 * Defines a struct.
 *
 * We distinguish rather arbitrarily between struct and class.
 *
 * Structs are simplified classes. All attributes are public, and there
 * is no wrapping.
 */
class Struct : public Class
{
public:
    static StructSP Make(
        const std::vector<std::string>& description,
        const std::string&              name,
        const std::string&              ns,
        const ClassConstSP&             baseClass,
        bool                            noMake,
        const std::string&              objectName,
        bool                            canPut,
        bool                            noId,
        bool                            isVirtual,
        bool                            asValue,
        bool                            uuid,
        bool                            byValue,
        bool                            useAccessors,
        bool                            incomplete,
        const std::string&              constructor);

    void addClassAttribute(const ClassAttributeConstSP& attribute);
    void addMethod(const ClassMethodConstSP& method);
    void addVerbatim(const std::string& verbatimStart,
                     const VerbatimConstSP& verbatim);
    void addValidation(const VerbatimConstSP& verbatim);
    void addCoerceFrom(const CoerceFromConstSP& coerceFrom);
    void addCoerceTo(const CoerceToConstSP& coerceTo);
    void addDynamicProperties(const VerbatimConstSP& verbatim);
    void addAsValueCode(const VerbatimConstSP& verbatim);

    // re-implementation of Construct
    int preDeclare(GeneratedOutput& ostr,
                   const ServiceDefinitionSP& svc) const;

    void declareClassFunctions(GeneratedOutput& ostr,
        const ServiceDefinitionSP& svc) const;

    // implementation of construct
    void declare(GeneratedOutput& ostr,
                 const ServiceDefinitionSP& svc,
                 bool types) const;

    bool declareInClasses() const;

    void declareHelper(GeneratedOutput& ostr,
                       const ServiceDefinitionSP& svc,
                       bool types) const;

    void implement(GeneratedOutput& ostr,
                   const ServiceDefinitionSP& svc,
                   bool types) const;

    void implementHelper(GeneratedOutput& ostr,
                         const ServiceDefinitionSP& svc,
                         bool types) const;

    void implementRegistration(
        GeneratedOutput& ostr,
        const char* serviceName,
        bool types) const;

    const char* type() const;
    spdoc::ConstructConstSP getDoc() const;

    // re-implementation of Class
    std::string getName(bool includeNamespace, const char* sep) const;
    std::string getObjectName() const;
    bool isAbstract() const;
    bool isWrapperClass() const;
    bool isDelegate() const;
    bool isVirtualMethod(const std::string& methodName) const;
    bool hasNonConstMethods() const;
    const DataTypeConstSP& getDataType(const ServiceDefinitionSP& svc, bool ignored) const;
    ClassConstSP getBaseClass() const;
    std::vector<CoerceFromConstSP> getCoerceFrom() const;
    std::vector<AttributeConstSP> AllAttributes() const;

protected:
    Struct(
        const std::vector<std::string>& description,
        const std::string&              name,
        const std::string&              ns,
        const ClassConstSP&             baseClass,
        bool                            noMake,
        const std::string&              objectName,
        bool                            canPut,
        bool                            noId,
        bool                            isVirtual,
        bool                            asValue,
        bool                            uuid,
        bool                            byValue,
        bool useAccessors,
        bool incomplete,
        const std::string& constructor);

private:
    std::vector<std::string>        m_description;
    std::string                     m_name;
    std::string                     m_ns;
    ClassConstSP                    m_baseClass;
    bool                            m_noMake;
    std::string                     m_objectName;
    bool                            m_canPut;
    bool                            m_noId;
    bool                            m_isVirtual;
    bool                            m_asValue;
    bool                            m_uuid;
    bool                            m_byValue;
    bool                            m_useAccessors;
    bool                            m_incomplete;
    std::string                     m_constructor;

    std::vector<AttributeConstSP>   m_attributes;
    std::vector<ClassMethodConstSP> m_methods;
    std::string                     m_verbatimStart;
    VerbatimConstSP                 m_verbatim;
    VerbatimConstSP                 m_validation;
    std::vector<CoerceFromConstSP>  m_coerceFromVector;
    std::vector<CoerceToConstSP>    m_coerceToVector;
    VerbatimConstSP                 m_dynamicPropertiesCode;
    VerbatimConstSP                 m_asValueCode;

    mutable DataTypeConstSP m_dataType;

    bool m_hasVirtualMethods;
    bool m_needsHelperClass;
    std::vector<ClassAttributeConstSP> m_classAttributes;
    std::vector<ClassAttributeConstSP> m_classProperties;

    mutable spdoc::ClassConstSP m_doc;

    void VerifyAndComplete();
    bool methodNeedsHelper(const ClassMethodConstSP& method) const;
    void addAttribute(const AttributeConstSP& attribute,
        bool canHide, const std::string& hideIf,
        ClassAttributeAccess accessLevel,
        const VerbatimConstSP& propertyCode,
        const ConverterConstSP& converter);

public:
    const std::vector<std::string>& description() const { return m_description; }
    const std::string& name() const { return m_name; }
    const std::string& ns() const { return m_ns; }
    const ClassConstSP& baseClass() const { return m_baseClass; }
    bool noMake() const { return m_noMake; }
    const std::string& objectName() const { return m_objectName; }
    bool canPut() const { return m_canPut; }
    bool noId() const { return m_noId; }
    bool isVirtual() const { return m_isVirtual; }
    bool asValue() const { return m_asValue; }
    bool uuid() const { return m_uuid; }
    bool byValue() const { return m_byValue; }
    bool useAccessors() const { return m_useAccessors; }
    bool incomplete() const { return m_incomplete; }
};

#endif
