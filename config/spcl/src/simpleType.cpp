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
** simpleType.cpp
***************************************************************************
** Implements the SimpleType class.
***************************************************************************
*/

#include "simpleType.hpp"

#include <spi/RuntimeError.hpp>
#include <spi/Service.hpp>
#include <spi/StringUtil.hpp>
#include <spi/spdoc_configTypes.hpp>

#include "classMethod.hpp"
#include "constant.hpp"
#include "cppType.hpp"
#include "innerClass.hpp"
#include "moduleDefinition.hpp"
#include "serviceDefinition.hpp"
#include "verbatim.hpp"
#include "generatorTools.hpp"

/*
***************************************************************************
** Implementation of SimpleType
***************************************************************************
*/
SimpleTypeConstSP SimpleType::Make(
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
    const VerbatimConstSP&          convertOut)
{
    return new SimpleType(description, name, ns, outerDataType,
                          innerTypeName, innerHeader, innerTypedef, noDoc,
                          convertInVarName, convertIn,
                          convertOutVarName, convertOut);
}

SimpleType::SimpleType(
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
    const VerbatimConstSP&          convertOut)
    :
    m_description(description),
    m_name(name),
    m_ns(ns),
    m_outerDataType(outerDataType),
    m_innerTypeName(innerTypeName),
    m_innerHeader(innerHeader),
    m_innerTypedef(innerTypedef),
    m_noDoc(noDoc),
    m_convertInVarName(convertInVarName),
    m_convertIn(convertIn),
    m_convertOutVarName(convertOutVarName),
    m_convertOut(convertOut)
{
    VerifyAndComplete();
}

void SimpleType::declare(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    // the name of the type is going to be typedef'd to the outerTypeName
    // this will put it inside the namespace
    //
    // we also add the description

    // writeStartCommentBlock(ostr, true);
    // if (description.size() == 0)
    //     ostr << "* No description.\n";
    // else
    //     writeComments(ostr, description);
    // writeEndCommentBlock(ostr);
    // ostr << "typedef " << outerTypeName << " " << name << ";\n";
}

bool SimpleType::declareInClasses() const
{
    return false; // no declarations either way...
}

void SimpleType::declareHelper(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    //ostr << "\n";
    //ostr << svc->getImport() << "\n";
    //ostr << innerTypeName << " " << name << "_convert_in("
    //     << outerDataType->outerReferenceType() << " "
    //     << convertInVarName << ");\n";

    //if (convertOut)
    //{
    //    ostr << svc->getImport() << "\n";
    //    ostr << outerDataType->outerValueType() << " " << name << "_convert_out(const "
    //         << innerTypeName << "& " << convertOutVarName << ");\n";
    //}
}

void SimpleType::implement(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    ostr << "\n";
    ostr << m_innerTypeName << " " << m_name << "_convert_in("
         << m_outerDataType->outerReferenceType() << " " << m_convertInVarName << ")\n";
    ostr << "{";
    writeVerbatim(ostr, m_convertIn);

    if (m_convertOut)
    {
        bool innerIsPointer = spi::StringEndsWith(m_innerTypeName, "*");
        ostr << "\n";
        ostr << m_outerDataType->outerValueType() << " " << m_name
             << "_convert_out(";
        if (!innerIsPointer)
            ostr << "const ";
        ostr << m_innerTypeName;
        if (!innerIsPointer)
            ostr << "&";
        ostr << " " << m_convertOutVarName << ")\n";
        ostr << "{";
        writeVerbatim(ostr, m_convertOut);
    }
}

void SimpleType::implementHelper(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
}

void SimpleType::implementRegistration(
    GeneratedOutput& ostr,
    const char* serviceName,
    bool types) const
{
}

const char* SimpleType::type() const
{
    return "SIMPLE_TYPE";
}

spdoc::ConstructConstSP SimpleType::getDoc() const
{
    if (!m_doc)
    {
        m_doc = spdoc::SimpleType::Make(
            m_name, m_description, m_outerDataType->name(), m_noDoc);
    }
    return m_doc;
}


const DataTypeConstSP& SimpleType::getDataType(
    const ServiceDefinitionSP& svc, bool ignored) const
{
    if (!m_dataType)
    {
        if (svc->getDataType(m_name))
            throw spi::RuntimeError("%s is already defined", m_name.c_str());

        const std::string& outerType = m_outerDataType->outerType();
        const std::string& innerType = m_innerTypeName;

        std::string convertInFormat;
        std::string convertOutFormat;
        DataTypeConstSP primitiveType;

        convertInFormat = m_name + "_convert_in(%s)";
        if (!m_ns.empty())
        {
            convertInFormat = m_ns + "::" + convertInFormat;
        }
        if (m_convertOut)
        {
            convertOutFormat = m_name + "_convert_out(%s)";
            if (!m_ns.empty())
                convertOutFormat = m_ns + "::" + convertOutFormat;
        }
        else
        {
            convertOutFormat = "%s";
        }
        if (m_noDoc)
        {
            if (!(m_outerDataType->isPrimitive()))
            {
                throw spi::RuntimeError("Outer type %s for simple type %s is "
                    "not a primitive type",
                    m_outerDataType->name().c_str(), m_name.c_str());
            }
            primitiveType = m_outerDataType;
        }

        m_dataType = DataType::Make(
            m_name, m_ns, "", outerType, outerType, innerType, innerType,
            m_publicType, "", false, false, m_noDoc,
            convertInFormat, convertOutFormat, "", primitiveType, false, false, ignored);
        svc->addDataType(m_dataType);

        DataTypeConstSP publicDataType = DataType::Make(
            m_name, m_ns, "", outerType, outerType,
            "", "", m_publicType, "", false, false, m_noDoc);

        if (!m_noDoc)
            publicDataType->setDoc(m_dataType->getDoc());

        svc->addPublicDataType(publicDataType);
    }
    return m_dataType;
}

void SimpleType::VerifyAndComplete()
{
    // should check that convertIn and convertOut have precisely
    // one copy of the string %s

    SPI_PRE_CONDITION(!m_name.empty());
    SPI_PRE_CONDITION(m_outerDataType);
    SPI_PRE_CONDITION(m_outerDataType->isPrimitive());
    SPI_PRE_CONDITION(!m_innerTypeName.empty());
    SPI_PRE_CONDITION(!m_convertInVarName.empty());
    SPI_PRE_CONDITION(m_convertIn);
    if (m_innerTypeName != m_outerDataType->outerValueType())
        SPI_PRE_CONDITION(!m_convertOutVarName.empty());
    if (!m_convertOutVarName.empty())
        SPI_PRE_CONDITION(m_convertOut);

    m_publicType = m_outerDataType->publicType();
}

void SimpleType::declareTypeConversions(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc) const
{
    ostr << "\n";
    ostr << svc->getImport() << "\n";
    ostr << m_innerTypeName << " " << m_name << "_convert_in("
         << m_outerDataType->outerReferenceType() << " "
         << m_convertInVarName << ");\n";

    if (m_convertOut)
    {
        bool innerIsPointer = spi::StringEndsWith(m_innerTypeName, "*");
        ostr << "\n";
        ostr << svc->getImport() << "\n";
        ostr << m_outerDataType->outerValueType() << " " << m_name
             << "_convert_out(";
        if (!innerIsPointer)
            ostr << "const ";
        ostr << m_innerTypeName;
        if (!innerIsPointer)
            ostr << "&";
        ostr << " " << m_convertOutVarName << ");\n";
    }
}

void SimpleType::writeInnerHeader(
    GeneratedOutput& ostr) const
{
    writeInclude(ostr, m_innerHeader);
}

void SimpleType::writeInnerPreDeclaration(
    GeneratedOutput& ostr,
    NamespaceManager& nsm) const
{
    // namespace is that of the inner type
    if (!m_innerTypedef.empty())
    {
        std::string innerTypedef1;
        std::string innerTypedef2;
        SplitTypedef(m_innerTypedef, innerTypedef1, innerTypedef2);

        size_t pos = m_innerTypeName.find_last_of("::");
        if (pos != std::string::npos)
        {
            SPI_POST_CONDITION(pos > 0); // because we searched for 2-character string
            nsm.startNamespace(ostr, m_innerTypeName.substr(0, pos - 1));
            nsm.indent(ostr);
            ostr << innerTypedef1 << " " << m_innerTypeName.substr(pos + 1) << innerTypedef2 << ";\n";
        }
        else
        {
            nsm.endAllNamespaces(ostr);
            nsm.indent(ostr);
            ostr << innerTypedef1 << " " << m_innerTypeName << innerTypedef2 << ";\n";
        }
    }
}
