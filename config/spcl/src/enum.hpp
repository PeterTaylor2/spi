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
#ifndef ENUM_HPP
#define ENUM_HPP

/*
***************************************************************************
** enum.hpp
***************************************************************************
** Defines the Enum classes used in the configuration parser.
***************************************************************************
*/

#include "construct.hpp"

#include "dataType.hpp"

#include <vector>
#include <string>

class NamespaceManager;
class GeneratedOutput;

SPI_DECLARE_RC_CLASS(Enumerand);
SPI_DECLARE_RC_CLASS(Enum);
SPI_DECLARE_RC_CLASS(Attribute);
SPI_DECLARE_RC_CLASS(Verbatim);
SPI_DECLARE_RC_CLASS(EnumConstructor);
SPI_DECLARE_RC_CLASS(ServiceDefinition);

SPDOC_BEGIN_NAMESPACE
SPI_DECLARE_OBJECT_CLASS(Enum);
SPI_DECLARE_OBJECT_CLASS(EnumConstructor);
SPDOC_END_NAMESPACE

/**
 * Defines an enum constructor.
 *
 * This consists of the following:
 *    attribute
 *    code
 */
class EnumConstructor : public spi::RefCounter
{
public:
    static EnumConstructorConstSP Make(
        const std::vector<std::string>& description,
        const AttributeConstSP& coerceFrom,
        const VerbatimConstSP& code);

    const spdoc::PublicType publicType() const;
    const VerbatimConstSP& code() const;

    void declare(GeneratedOutput& ostr,
        const std::string& enumName,
        const ServiceDefinitionConstSP& svc) const;

    void declareCoerce(GeneratedOutput& ostr,
        const std::string& enumName,
        const ServiceDefinitionConstSP& svc) const;

    void implement(GeneratedOutput& ostr,
        const std::string& enumName,
        const ServiceDefinitionConstSP& svc) const;

    spdoc::EnumConstructorConstSP doc() const;

private:
    EnumConstructor(const std::vector<std::string>& description,
        const AttributeConstSP& coerceFrom,
        const VerbatimConstSP& code);

    std::vector<std::string> m_description;
    AttributeConstSP m_coerceFrom;
    VerbatimConstSP  m_code;

    spdoc::EnumConstructorConstSP m_doc;
};

/**
 * Enumerand - a single entry in an enumerated type.
 *
 * An enumerand has a name and an optional value. We don't really care
 * what the value is at this point, so we store it as a string.
 *
 * It also has a description that might appear in the user guide.
 */
class Enumerand : public spi::RefCounter
{
public:
    //friend class Enum;

    static EnumerandConstSP Make(
        const std::vector<std::string>& description,
        const std::string&              name,
        const std::string&              value,
        const std::vector<std::string>& alternates);

    const std::string& outputString() const;

protected:
    Enumerand(
        const std::vector<std::string>& description,
        const std::string&              name,
        const std::string&              value,
        const std::vector<std::string>& alternates);

private:
    std::vector<std::string> m_description;
    std::string              m_name;
    std::string              m_value;
    std::vector<std::string> m_alternates;

public:
    const std::vector<std::string>& description() const { return m_description; }
    const std::string& name() const { return m_name; }
    const std::string& value() const { return m_value; }
    const std::vector<std::string>& alternates() const { return m_alternates; }
};


/**
 * Enumerated type.
 *
 * Defines an enumerated type. The outer type is defined as a class with
 * an enumerated type inside the class, and a single value equal to the
 * enumerated type. Hence the size of the outer type is the same as the
 * size of the enumerated type since there are no virtual functions
 * defined.
 *
 * For the moment the inner type will be the enumerated type defined
 * within the class (just to set an example of conversion functions).
 *
 * The string values will match the enumerands.
 */
class Enum : public Construct
{
public:
    static EnumConstSP Make(
        const std::vector<std::string>& description,
        const std::string& name,
        const std::string& ns,
        const std::string& innerName,
        const std::string& innerHeader,
        const std::string& enumTypedef,
        const std::vector<EnumerandConstSP>& enumerands,
        const std::vector<EnumConstructorConstSP>& constructors);

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

    void declareTypeConversions(
        GeneratedOutput& ostr,
        const ServiceDefinitionSP& svc) const;

    void writeInnerHeader(
        GeneratedOutput& ostr) const;

    void writeInnerPreDeclaration(
        GeneratedOutput& ostr,
        NamespaceManager& nsm) const;

    const char* type() const;
    spdoc::ConstructConstSP getDoc() const;

    const DataTypeConstSP& dataType(const ServiceDefinitionSP& svc, bool ignored) const;

protected:
    Enum(
        const std::vector<std::string>& description,
        const std::string& name,
        const std::string& ns,
        const std::string& innerName,
        const std::string& innerHeader,
        const std::string& enumTypedef,
        const std::vector<EnumerandConstSP>& enumerands,
        const std::vector<EnumConstructorConstSP>& constructors);

private:
    std::vector<std::string>      m_description;
    std::string                   m_name;
    std::string                   m_ns;
    std::string                   m_innerName;
    std::string                   m_innerHeader;
    std::string                   m_enumTypedef;
    std::vector<EnumerandConstSP> m_enumerands;
    std::vector<EnumConstructorConstSP> m_constructors;

    void VerifyAndComplete();

    mutable DataTypeConstSP m_dataType;
    std::map<std::string, std::string> m_indexEnumerands;
    std::vector<std::string> m_possibleValues;
    std::vector<spdoc::PublicType> m_constructorTypes;

    mutable spdoc::EnumConstSP m_doc;

public:
    const std::string& name() const { return m_name; }
    const std::vector<EnumerandConstSP>& enumerands() const { return m_enumerands; }
    const std::vector<spdoc::PublicType>& constructorTypes() const
    { return m_constructorTypes; }

};

#endif
