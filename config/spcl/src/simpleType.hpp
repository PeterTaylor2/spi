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
#ifndef SIMPLE_TYPE_HPP
#define SIMPLE_TYPE_HPP

/*
***************************************************************************
** simpleType.hpp
***************************************************************************
** Defines the SimpleType classes used in the configuration parser.
***************************************************************************
*/

#include "construct.hpp"
#include <spi/spdoc_namespace.hpp>

#include <vector>

SPI_DECLARE_RC_CLASS(SimpleType);
SPI_DECLARE_RC_CLASS(Verbatim);
SPI_DECLARE_RC_CLASS(DataType);

SPDOC_BEGIN_NAMESPACE
SPI_DECLARE_OBJECT_CLASS(SimpleType);
SPDOC_END_NAMESPACE

/**
 * Simple type.
 *
 * Defines a simple type which on the outside is a primitive type,
 * and on the inside is a user defined type.
 *
 * You need to be able to provide a one line converter to convert
 * instances of the inner type to the outer type and vice versa.
 *
 * The inner type is always going to be passed by value rather than
 * by reference.
 *
 * The end user of the generated library will only see the outer type.
 */
class SimpleType : public Construct
{
public:
    static SimpleTypeConstSP Make(
        const std::vector<std::string>& description,
        const std::string&              name,
        const std::string&              ns,
        const DataTypeConstSP&          outerDataType,
        const std::string&              innerTypeName,
        const std::string&              innerHeader,
        const std::string&              innerTypedef,
        bool                            noDoc,
        const std::string&              convertInVarName,
        const VerbatimConstSP&          convertIn,
        const std::string&              convertOutVarName,
        const VerbatimConstSP&          convertOut);

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

    const DataTypeConstSP& getDataType(const ServiceDefinitionSP& svc, bool ignore) const;

protected:
    SimpleType(
        const std::vector<std::string>& description,
        const std::string&              name,
        const std::string&              ns,
        const DataTypeConstSP&          outerDataType,
        const std::string&              innerTypeName,
        const std::string&              innerHeader,
        const std::string&              innerTypedef,
        bool                            noDoc,
        const std::string&              convertInVarName,
        const VerbatimConstSP&          convertIn,
        const std::string&              convertOutVarName,
        const VerbatimConstSP&          convertOut);

private:
    std::vector<std::string> m_description;
    std::string              m_name;
    std::string              m_ns;
    DataTypeConstSP          m_outerDataType;
    std::string              m_innerTypeName;
    std::string              m_innerHeader;
    std::string              m_innerTypedef;
    bool                     m_noDoc;
    std::string              m_convertInVarName;
    VerbatimConstSP          m_convertIn;
    std::string              m_convertOutVarName;
    VerbatimConstSP          m_convertOut;

    void VerifyAndComplete();

    spdoc::PublicType::Enum m_publicType;

    mutable DataTypeConstSP m_dataType;
    mutable spdoc::SimpleTypeConstSP m_doc;
};

#endif
