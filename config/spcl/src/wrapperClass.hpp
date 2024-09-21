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
#ifndef WRAPPERCLASS_HPP
#define WRAPPERCLASS_HPP

/*
***************************************************************************
** wrapperClass.hpp
***************************************************************************
** Defines the WrapperClass classes used in the configuration parser.
***************************************************************************
*/

#include "class.hpp"

#include <vector>

SPI_DECLARE_RC_CLASS(Attribute);
SPI_DECLARE_RC_CLASS(ClassAttribute);
SPI_DECLARE_RC_CLASS(Verbatim);
SPI_DECLARE_RC_CLASS(ClassMethod);
SPI_DECLARE_RC_CLASS(CoerceTo);
SPI_DECLARE_RC_CLASS(InnerClass);
SPI_DECLARE_RC_CLASS(WrapperClass);

SPDOC_BEGIN_NAMESPACE
SPI_DECLARE_OBJECT_CLASS(Class);
SPDOC_END_NAMESPACE

/**
 * Defines a wrapper class.
 *
 * Inheritance not yet supported. When we do implement inheritance,
 * then it is from another wrapper class and the assumption is
 * that the inner classes form the same inheritance tree.
 */
class WrapperClass : public Class
{
public:
    static WrapperClassSP Make(
        const std::vector<std::string>& description,
        const std::string&              name,
        const std::string&              ns,
        const InnerClassConstSP&        innerClass,
        const WrapperClassConstSP&      baseClass,
        bool                            isVirtual,
        bool                            noMake,
        const std::string&              objectName,
        bool                            isDelegate,
        bool                            canPut,
        bool                            noId,
        bool                            asValue,
        bool                            uuid,
        bool incomplete);

    void addClassAttribute(const ClassAttributeConstSP& classAttribute);
    void addMethod(const ClassMethodConstSP& method);
    void addDelegate(
        const std::string& prototype,
        const VerbatimConstSP& implementation);
    void addVerbatim(const std::string& verbatimStart,
                     const VerbatimConstSP& verbatim);
    void addCoerceFrom(const CoerceFromConstSP& coerceFrom);
    void addCoerceTo(const CoerceToConstSP& coerceTo);
    void addValidateInner(const std::string& varName,
        const VerbatimConstSP& verbatim);
    void addDynamicProperties(const VerbatimConstSP& verbatim);
    void addAsValueCode(const VerbatimConstSP& verbatim);

    // re-implementation of Construct
    void declareInner(
        GeneratedOutput& ostr,
        NamespaceManager& nsman,
        const ServiceDefinitionSP& svc) const;

    int preDeclare(GeneratedOutput& ostr,
                   const ServiceDefinitionSP& svc) const;

    void declareClassFunctions(GeneratedOutput& ostr,
        const ServiceDefinitionSP& svc) const;

    // implementation of Construct
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

    // implementation of Class
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
    bool byValue() const;
    std::string ObjectName() const;
    std::vector<ClassAttributeConstSP> getBaseClassProperties() const;
    std::vector<AttributeConstSP> AllAttributes() const;
    std::vector<ClassAttributeConstSP> AllClassAttributes() const;

protected:
    WrapperClass(
        const std::vector<std::string>& description,
        const std::string& name,
        const std::string& ns,
        const InnerClassConstSP& innerClass,
        const WrapperClassConstSP& baseClass,
        bool                            isVirtual,
        bool                            noMake,
        const std::string& objectName,
        bool                            isDelegate,
        bool                            canPut,
        bool                            noId,
        bool                            asValue,
        bool                            uuid,
        bool incomplete);

private:
    std::vector<std::string>           m_description;
    std::string                        m_name;
    std::string                        m_ns;
    InnerClassConstSP                  m_innerClass;
    WrapperClassConstSP                m_baseClass;
    bool                               m_isVirtual;
    bool                               m_noMake;
    std::string                        m_objectName;
    bool                               m_isDelegate;
    bool                               m_canPut;
    bool                               m_noId;
    bool                               m_asValue;
    bool                               m_uuid;
    bool                               m_incomplete;

private:
    std::string                        m_verbatimStart;
    VerbatimConstSP                    m_verbatim;
    VerbatimConstSP                    m_verbatimConstructor;
    std::vector<ClassAttributeConstSP> m_classAttributes;
    std::vector<ClassAttributeConstSP> m_classProperties;
    std::vector<ClassMethodConstSP>    m_methods;
    std::vector<std::string>           m_delegatePrototypes;
    std::vector<VerbatimConstSP>       m_delegateImplementations;
    std::vector<CoerceFromConstSP>     m_coerceFromVector;
    std::vector<CoerceToConstSP>       m_coerceToVector;
    std::string                        m_validateInnerName;
    VerbatimConstSP                    m_validateInnerCode;
    VerbatimConstSP                    m_dynamicPropertiesCode;
    VerbatimConstSP                    m_asValueCode;

    mutable DataTypeConstSP m_dataType;

    void VerifyAndComplete();
    bool m_hasVirtualMethods;
    std::vector<AttributeConstSP> m_attributes;

    mutable spdoc::ClassConstSP m_doc;

public:
    bool hasConstructor() const;
    const std::vector<std::string>& description() const { return m_description; }
    const std::string& name() const { return m_name; }
    const std::string& ns() const { return m_ns; }
    const InnerClassConstSP& innerClass() const { return m_innerClass; }
    const WrapperClassConstSP& baseClass() const { return m_baseClass; }
    bool isVirtual() const { return m_isVirtual; }
    bool noMake() const { return m_noMake; }
    const std::string& objectName() const { return m_objectName; }
    bool canPut() const { return m_canPut; }
    bool noId() const { return m_noId; }
    bool asValue() const { return m_asValue; }
    bool uuid() const { return m_uuid; }
    const DataTypeConstSP& dataType() const { return m_dataType; }
    const std::vector<ClassAttributeConstSP>& classProperties() const { return m_classProperties; }

    void setVerbatimConstructor(const VerbatimConstSP& verbatimConstructor);
    void setDataType(const DataTypeConstSP& dataType);
};

#endif
