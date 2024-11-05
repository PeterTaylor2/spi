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
** innerClass.cpp
***************************************************************************
** Implements the InnerClass class used in the configuration parser.
***************************************************************************
*/

#include "innerClass.hpp"

#include <spi/RuntimeError.hpp>
#include <spi/Service.hpp>
#include <spi/StringUtil.hpp>

#include "constant.hpp"
#include "cppType.hpp"
#include "dataType.hpp"
#include "serviceDefinition.hpp"
#include "generatorTools.hpp"

/*
***************************************************************************
** Implementation of InnerClass
***************************************************************************
*/
InnerClassConstSP InnerClass::Make(
    const std::string& typeName,
    const std::string& ns,
    const std::string& freeFunc,
    const std::string& copyFunc,
    const std::string& preDeclaration,
    const std::string& sharedPtr,
    bool               isShared,
    bool               isConst,
    bool               isOpen,
    bool               isStruct,
    bool               isCached,
    bool               isTemplate,
    bool               byValue,
    const std::string& boolTest,
    bool               allowConst)
{
    return new InnerClass(
        typeName, ns, freeFunc, copyFunc, preDeclaration, sharedPtr,
        isShared, isConst, isOpen, isStruct, isCached, isTemplate, byValue, boolTest, allowConst);
}

InnerClass::InnerClass(
    const std::string& typeName,
    const std::string& ns,
    const std::string& freeFunc,
    const std::string& copyFunc,
    const std::string& preDeclaration,
    const std::string& sharedPtr,
    bool               isShared,
    bool               isConst,
    bool               isOpen,
    bool               isStruct,
    bool               isCached,
    bool               isTemplate,
    bool               byValue,
    const std::string& boolTest,
    bool               allowConst)
    :
    m_typeName(typeName),
    m_ns(ns),
    m_freeFunc(freeFunc),
    m_copyFunc(copyFunc),
    m_preDeclaration(preDeclaration),
    m_sharedPtr(sharedPtr),
    m_isShared(isShared),
    m_isConst(isConst),
    m_isOpen(isOpen),
    m_isStruct(isStruct),
    m_isCached(isCached),
    m_isTemplate(isTemplate),
    m_byValue(byValue),
    m_boolTest(boolTest),
    m_allowConst(allowConst)
{
    VerifyAndComplete();
}

void InnerClass::preDeclare(GeneratedOutput& ostr,
                            NamespaceManager& nsman) const
{
    if (m_isTemplate)
        return;

    nsman.startNamespace(ostr, m_ns, 4);
    nsman.indent(ostr);
    if (!m_preDeclaration.empty())
    {
        ostr << m_preDeclaration << "\n";
    }
    else if (m_isStruct)
    {
        ostr << "struct " << m_typeName << ";\n";
    }
    else
    {
        ostr << "class " << m_typeName << ";\n";
    }
}

const std::string& InnerClass::getCppType() const
{
    return m_cppType;
}

const std::string& InnerClass::getCppRefType() const
{
    return m_cppRefType;
}

std::string InnerClass::ToSharedPointer(const std::string& inner) const
{
    if (m_isShared)
        return inner;

    std::ostringstream oss;

    oss << m_sharedPointer << "(";

    if (m_byValue)
    {
        if (m_copyFunc.empty())
        {
            // we assume that the inner_type has a copy constructor
            oss << "new inner_type(inner)";
        }
        else
        {
            // we assume the copyFunc takes a pointer
            oss << m_copyFunc << "(&" << inner << ")";
        }
    }
    else
    {
        oss << inner;
    }

    if (!m_freeFunc.empty())
        oss << ", " << m_freeFunc;
    oss << ")";

    return oss.str();
}

InnerClassConstSP InnerClass::setSharedPtr(const std::string & sharedPtr) const
{
    return InnerClass::Make(
        this->m_typeName,
        this->m_ns,
        this->m_freeFunc,
        this->m_copyFunc,
        this->m_preDeclaration,
        sharedPtr,
        this->m_isShared,
        this->m_isConst,
        this->m_isOpen,
        this->m_isStruct,
        this->m_isCached,
        this->m_isTemplate,
        this->m_byValue,
        this->m_boolTest,
        this->m_allowConst);
}

void InnerClass::VerifyAndComplete()
{
    std::ostringstream ostr;

    if (m_byValue && m_isShared)
        throw spi::RuntimeError("Inner class cannot be passed by value for shared pointers");

    if (m_allowConst)
    {
        if (m_isConst)
            throw spi::RuntimeError("Inner class is const - allowConst is not required");
        if (m_byValue)
            throw spi::RuntimeError("Inner class cannot be passed by value and allow const");
    }

    ostr << m_ns << "::" << m_typeName;
    if (m_isConst)
        ostr << " const";
    m_fullTypeName = ostr.str();

    if (m_isConst)
    {
        m_fullTypeNameConst = m_fullTypeName;
    }
    else
    {
        ostr.str(""); // clear it out to start again
        ostr << m_ns << "::" << m_typeName << " const";
        m_fullTypeNameConst = ostr.str();
    }

    ostr.str(""); // clear it out to start again
    ostr << m_sharedPtr << "< " << m_fullTypeName << " >";
    m_sharedPointer = ostr.str();

    ostr.str(""); // clear it out to start again
    ostr << m_sharedPtr << "< " << m_fullTypeNameConst << " >";
    m_constSharedPointer = ostr.str();

    ostr.str(""); // clear it out to start again
    ostr << m_fullTypeName << "*";
    m_rawPointer = ostr.str();

    ostr.str(""); // clear it out to start again
    ostr << m_fullTypeNameConst << "*";
    m_constRawPointer = ostr.str();

    ostr.str(""); // clear it out to start again
    if (!m_ns.empty())
        ostr << m_ns << "::";
    ostr << m_typeName;
    m_name = ostr.str();

    if (m_isShared)
    {
        m_cppType = m_sharedPointer;
        m_cppRefType = "const " + m_sharedPointer + "&";
        m_constCppType = m_constSharedPointer;
        m_constCppRefType = "const " + m_constSharedPointer + "&";
    }
    else if (m_byValue)
    {
        m_cppType = m_ns + "::" + m_typeName;
        m_cppRefType = (m_isConst ? "const " : "") + m_cppType + "&";
        m_constCppType = m_cppType; // won't be used
        m_constCppRefType = m_cppRefType; // won't be used
    }
    else
    {
        m_cppType = m_rawPointer;
        m_cppRefType = m_rawPointer;
        m_constCppType = m_constRawPointer;
        m_constCppRefType = m_constRawPointer;
    }

    if (m_isStruct && m_isTemplate)
        throw spi::RuntimeError("Inner class cannot be both struct and template");

    std::vector<std::string> parts = spi_util::StringSplit(m_sharedPtr, "::");
    parts[parts.size() - 1] = "dynamic_pointer_cast";
    m_spDynamicCast = spi_util::StringJoin("::", parts);

}

/*
***************************************************************************
** Implementation of InnerClassTemplate
***************************************************************************
*/
InnerClassTemplateConstSP InnerClassTemplate::Make(
    const std::string& name,
    const std::string& ns,
    const std::string& preDeclaration,
    const std::string& boolTest,
    size_t numArguments)
{
    return new InnerClassTemplate(name, ns, preDeclaration, boolTest, numArguments);
}

InnerClassTemplate::InnerClassTemplate(
    const std::string& name,
    const std::string& ns,
    const std::string& preDeclaration,
    const std::string& boolTest,
    size_t numArguments)
    :
    m_name(name),
    m_ns(ns),
    m_preDeclaration(preDeclaration),
    m_boolTest(boolTest),
    m_numArguments(numArguments)
{
    VerifyAndComplete();
}

void InnerClassTemplate::preDeclare(GeneratedOutput& ostr,
                                    NamespaceManager& nsman) const
{
    if (!m_preDeclaration.empty())
    {
        nsman.startNamespace(ostr, m_ns, 4);
        nsman.indent(ostr);
        ostr << m_preDeclaration << "\n";
    }
}

void InnerClassTemplate::VerifyAndComplete()
{
    SPI_PRE_CONDITION(!m_name.empty());
    SPI_PRE_CONDITION(m_numArguments > 0);

    std::ostringstream ostr;

    ostr << m_ns << "::" << m_name;
    m_fullName = ostr.str();
}

InnerClassConstSP InnerClassTemplate::MakeInnerClass(
    const std::vector<std::string>& templateArgs,
    const std::string& sharedPtr,
    bool               isShared,
    bool               isConst,
    bool               isOpen,
    bool               isStruct,
    bool               isCached,
    bool               byValue,
    bool               allowConst) const
{
    if (templateArgs.size() != m_numArguments)
    {
        throw spi::RuntimeError("Mis-match between template arguments and expected");
    }

    std::string typeName = spi::StringFormat(
        "%s< %s >",
        m_name.c_str(),
        spi_util::StringJoin(", ", templateArgs).c_str());

    return InnerClass::Make(typeName, m_ns, "", "", "", sharedPtr,
        isShared, isConst, isOpen, isStruct, isCached, true, byValue,
        byValue && m_boolTest.empty() ? "true" : m_boolTest, allowConst);
}


