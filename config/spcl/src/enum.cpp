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
** enum.cpp
***************************************************************************
** Implements the Enum and Enumerand classes.
***************************************************************************
*/

#include "enum.hpp"

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
#include <spgtools/commonTools.hpp>

#include <iostream>

using spi_util::StringSplit;
using spi_util::StringStrip;
using spi_util::StringStartsWith;
using spi_util::StringEndsWith;

/*
***************************************************************************
** Implementation of Enumerand
***************************************************************************
*/
EnumerandConstSP Enumerand::Make(
    const std::vector<std::string>& description,
    const std::string&              name,
    const std::string&              value,
    const std::vector<std::string>& alternates)
{
    return new Enumerand(description, name, value, alternates);
}

Enumerand::Enumerand(
    const std::vector<std::string>& description,
    const std::string&              name,
    const std::string&              value,
    const std::vector<std::string>& alternates)
    :
    m_description(description),
    m_name(name),
    m_value(value),
    m_alternates(alternates)
{}

const std::string& Enumerand::outputString() const
{
    if (m_alternates.size() > 0)
        return m_alternates[0];
    return m_name;
}

/*
***************************************************************************
** Implementation of Enum
***************************************************************************
*/
EnumConstSP Enum::Make(
    const std::vector<std::string>&      description,
    const std::string&                   name,
    const std::string&                   ns,
    const std::string&                   innerName,
    const std::string&                   innerHeader,
    const std::string&                   enumTypedef,
    const std::vector<EnumerandConstSP>& enumerands)
{
    return new Enum(description, name, ns, innerName, innerHeader, enumTypedef, enumerands);
}

bool Enum::declareInClasses() const
{
    return true;
}

void Enum::declare(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    size_t nbEnumerands = m_enumerands.size();

    writeStartCommentBlock(ostr, true);

    char* enumNameSep;

    enumNameSep = "::";
    ostr << "* Class " << m_name << " containing enumerated type "
         << m_name << "::Enum.\n"
         << "* Whenever " << m_name << " is expected you can use "
         << m_name << "::Enum,\n"
         << "* and vice versa, because automatic type conversion is provided by\n"
         << "* the constructor and cast operator.\n";

    if (m_description.size() != 0)
    {
        ostr << "*\n";
        writeComments(ostr, m_description);
    }

    if (nbEnumerands > 0)
    {
        ostr << "*\n";
        for (size_t i = 0; i < nbEnumerands; ++i)
        {
            const EnumerandConstSP& enumerand = m_enumerands[i];
            ostr << "* " << m_name << enumNameSep << enumerand->name() << "\n";
            if (enumerand->description().size() > 0)
                writeComments(ostr, enumerand->description(), 4, 0);
        }
    }

    writeEndCommentBlock(ostr);

    ostr << "class " << svc->getImport() << " " << m_name << "\n"
         << "{\n"
         << "public:\n"
         << "    enum Enum\n"
         << "    {\n";

    for (size_t i = 0; i < nbEnumerands; ++i)
        ostr << "        " << m_enumerands[i]->name() << ",\n";

    ostr << "        UNINITIALIZED_VALUE\n"
        << "    };\n"
        << "\n"
        << "    static spi::EnumInfo* get_enum_info();\n"
        << "\n"
        << "    " << m_name << "() : value(UNINITIALIZED_VALUE) {}\n"
        << "    " << m_name << "(" << m_name << "::Enum value) : value(value) {}\n"
        << "    " << m_name << "(const char* str) : value(" << m_name << "::from_string(str)) {}\n"
        << "    " << m_name << "(const std::string & str) : value("<< m_name << "::from_string(str.c_str())) {}\n"
        << "    " << m_name << "(const spi::Value & value);\n"
        << "\n"
        << "    operator " << m_name << "::Enum() const { return value; }\n"
        << "    operator std::string() const { return std::string(" << m_name << "::to_string(value)); }\n"
        << "    operator spi::Value() const { return spi::Value(" << m_name << "::to_string(value)); }\n"
        << "    std::string to_string() const { return std::string("<< m_name << "::to_string(value)); }\n"
        << "    spi::Value to_value() const { return spi::Value(" << m_name << "::to_string(value)); }\n"
        << "\n"
        << "    static " << m_name << "::Enum from_string(const char*);\n"
        << "    static const char* to_string(" << m_name << "::Enum);\n"
        // FIXME: validate that operator Enum() has a good value!
         << "\n"
         << "private:\n"
         << "    " << m_name << "::Enum value;\n"
         << "};\n";
}

void Enum::declareHelper(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    //if (!innerName.empty())
    //{
    //    ostr << "\n"
    //         << svc->getImport() << "\n"
    //         << innerName << " " << name << "_convert_in(const " << name << "&);\n";
    //    ostr << "\n"
    //         << svc->getImport() << "\n"
    //         << name << " " << name << "_convert_out(" << innerName << ");\n";
    //}
}

void Enum::implement(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    ostr << "\n"
        << m_name << "::" << m_name << "(const spi::Value &v)\n"
        << "{\n"
        << "    switch(v.getType())\n"
        << "    {\n"
        << "    case spi::Value::INT:\n"
        << "        value = (Enum)v.getInt();\n"
        << "        break;\n"
        << "    case spi::Value::SHORT_STRING:\n"
        << "    case spi::Value::STRING:\n"
        << "        value = from_string(v.getString().c_str());\n"
        << "        break;\n"
        << "    default:\n"
        << "        SPI_THROW_RUNTIME_ERROR(\"Bad value type: \" << spi::Value::TypeToString(v.getType()));\n"
        << "    }\n"
        << "}\n";

    if (!m_innerName.empty())
    {
        ostr << "\n"
             << m_innerName << " " << m_name << "_convert_in(const " << m_name << "& v_)\n"
             << "{\n"
             << "    switch((" << m_name << "::Enum)v_)\n"
             << "    {\n";

        for (size_t i = 0; i < m_enumerands.size(); ++i)
        {
            ostr << "    case " << m_name << "::" << m_enumerands[i]->name() << ":\n"
                 << "        return " << m_enumerands[i]->value() << ";\n";
        }
        ostr << "    case " << m_name << "::UNINITIALIZED_VALUE:\n"
             << "        throw std::runtime_error(\"Uninitialized value for "
             << m_name << "\");\n"
             << "    }\n"
             << "    throw spi::RuntimeError(\"Bad enumerated value\");\n"
             << "}\n";

        ostr << "\n"
             << m_name << " " << m_name << "_convert_out(" << m_innerName << " v_)\n"
             << "{\n";

        // implement as sequence of if statements rather than
        // switch since the innerType might not be numeric
        // but could be an enumerated class (or even a string)
        for (size_t i = 0; i < m_enumerands.size(); ++i)
        {
            ostr << "    if (v_ == " << m_enumerands[i]->value() << ")\n"
                 << "        return " << m_name << "::" << m_enumerands[i]->name()
                 << ";\n";
        }
        ostr << "    throw spi::RuntimeError(\"Bad enumerated value\");\n"
             << "}\n";
    }
}

void Enum::implementHelper(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    ostr << "\n";
    ostr << "spi::EnumInfo* " << m_name << "::get_enum_info()\n"
         << "{\n"
         << "    static spi::EnumInfo the_info;\n"
         << "    if (!the_info.Initialised())\n"
         << "    {\n"
         << "        std::vector<std::string> enumerands;\n";

    for (size_t i = 0; i < m_enumerands.size(); ++i)
    {
        ostr << "        enumerands.push_back(\""
             << m_enumerands[i]->outputString() << "\");\n";
    }

    ostr << "        the_info.Initialise(\"" << m_name << "\", enumerands);\n"
         << "    }\n"
         << "    return &the_info;\n"
         << "}\n";

    std::map<std::string,std::string>::const_iterator iter =
         m_indexEnumerands.begin();

    ostr << "\n"
         << m_name << "::Enum " << m_name << "::from_string(const char* str)\n"
         << "{\n"
         << "    std::string uc_ = spi::StringUpper(str);\n";

    if (iter->first == "")
    {
        ostr << "    if (uc_ == \"\")\n"
             << "        return " << m_name << "::" << iter->second << ";\n";
        ++iter;
    }

    ostr << "    switch(uc_[0])\n"
         << "    {\n";

    char prevFirstChar = 0;
    for (; iter != m_indexEnumerands.end(); ++iter)
    {
        char firstChar = iter->first[0];
        if (firstChar != prevFirstChar)
        {
            if (prevFirstChar)
            {
                ostr << "        break;\n";
            }
            ostr << "    case '" << firstChar << "':\n";
            prevFirstChar = firstChar;
        }
        ostr << "        if (uc_ == \"" << iter->first << "\")\n"
             << "            return " << m_name << "::" << iter->second << ";\n";
    }

    ostr << "        break;\n"
         << "    }\n"
         << "    throw spi::RuntimeError(\"Cannot convert '%s' to "
         << m_name << ". Possible values:\\n\"\n"
         << "       \"'" << spi::StringJoin("', '", m_possibleValues) << "'\",\n"
         << "        str);\n"
         << "}\n";


    ostr << "\n"
         << "const char* " << m_name << "::to_string(" << m_name << "::Enum v_)\n"
         << "{\n"
         << "    switch(v_)\n"
         << "    {\n";

    for (size_t i = 0; i < m_enumerands.size(); ++i)
    {
        ostr << "    case " << m_name << "::" << m_enumerands[i]->name() << ":\n"
             << "        return \""
             << m_enumerands[i]->outputString()
             << "\";\n";
    }

    ostr << "    case " << m_name << "::UNINITIALIZED_VALUE:\n"
         << "        throw std::runtime_error(\"Uninitialized value for "
         << m_name << "\");\n"
         << "    }\n"
         << "    throw std::runtime_error(\"Bad enumerated value\");\n"
         << "}\n";

}

void Enum::implementRegistration(
    GeneratedOutput& ostr,
    const char* serviceName,
    bool types) const
{
    ostr << "    " << serviceName << "->add_enum_info(" << m_name
         << "::get_enum_info());\n";
}

const char* Enum::type() const
{
    return "ENUM";
}

spdoc::ConstructConstSP Enum::getDoc() const
{
    if (!m_doc)
    {
        std::vector<spdoc::EnumerandConstSP> enumerandDocs;
        for (size_t i = 0; i < m_enumerands.size(); ++i)
        {
            std::string code = m_enumerands[i]->name();
            std::vector<std::string> strings = m_enumerands[i]->alternates();
            if (strings.size() == 0)
                strings.push_back(m_enumerands[i]->name());

            enumerandDocs.push_back(spdoc::Enumerand::Make(
                code, strings, m_enumerands[i]->description()));
        }
        m_doc = spdoc::Enum::Make(m_name, m_description, enumerandDocs);
    }
    return m_doc;
}

Enum::Enum(
    const std::vector<std::string>&      description,
    const std::string&                   name,
    const std::string&                   ns,
    const std::string&                   innerName,
    const std::string&                   innerHeader,
    const std::string&                   enumTypedef,
    const std::vector<EnumerandConstSP>& enumerands)
    :
    m_description(description),
    m_name(name),
    m_ns(ns),
    m_innerName(innerName),
    m_innerHeader(innerHeader),
    m_enumTypedef(enumTypedef),
    m_enumerands(enumerands)
{
    VerifyAndComplete();
}

const DataTypeConstSP& Enum::dataType(const ServiceDefinitionSP& svc, bool ignored) const
{
    if (!m_dataType)
    {
        // outerType is called name
        // innerType is called name::Enum if there is no innerName
        // other innerName - which can be defined before
        //
        // in addition name needs to be not previously defined as DataType

        std::string dataTypeName = m_ns.empty() ? m_name : m_ns + "." + m_name;
        std::string outerTypeName = m_ns.empty() ? m_name : m_ns + "::" + m_name;
        if (svc->getDataType(dataTypeName))
        {
            throw spi::RuntimeError("DataType %s is already defined",
                                    m_name.c_str());
        }
        //if (svc->getCppType(outerTypeName))
        //{
        //    throw spi::RuntimeError("CppType %s is already defined",
        //                            outerTypeName.c_str());
        //}

        const std::string& outerType = outerTypeName; // CppType::Make(outerTypeName, false);
        std::string innerType;
        //bool newInnerType = false;
        std::string convertIn;
        std::string convertOut;

        //if (innerName.empty())
        //{
        //    std::string innerTypeName = outerTypeName + "::Enum";
        //    //if (svc->getCppType(innerTypeName))
        //    //{
        //    //    throw spi::RuntimeError("CppType %s is already defined",
        //    //                            innerTypeName.c_str());
        //    //}
        //    innerType = innerTypeName; // CppType::Make(innerTypeName, false);
        //    //newInnerType = true;
        //    // automatic type conversions work - no need for convertIn/Out
        //}
        if (!m_innerName.empty())
        {
            innerType = m_innerName; // svc->getCppType(innerName);
            //if (!innerType)
            //{
            //    innerType = CppType::Make(innerName, false);
            //    newInnerType = true;
            //}
            convertIn = spi::StringFormat("%s_convert_in(%%s)", m_name.c_str());
            convertOut = spi::StringFormat("%s_convert_out(%%s)", m_name.c_str());
            if (!m_ns.empty())
            {
                convertIn = m_ns + "::" + convertIn;
                convertOut = m_ns + "::" + convertOut;
            }
        }

        //svc->addCppType(outerType);
        //if (newInnerType)
        //    svc->addCppType(innerType);

        m_dataType = DataType::Make(m_name, m_ns, svc->getNamespace(),
            outerType, outerType,
            innerType, innerType, spdoc::PublicType::ENUM, "", false,
            false, convertIn, convertOut,
            std::string(), DataTypeConstSP(), false, false, ignored);

        svc->addDataType(m_dataType);

        if (!m_innerName.empty())
        {
            DataTypeConstSP publicDataType = DataType::Make(
                m_name, m_ns, svc->getNamespace(), outerType, outerType, "", "",
                spdoc::PublicType::ENUM, "", false, false);

            publicDataType->setDoc(m_dataType->getDoc());

            svc->addPublicDataType(publicDataType);
        }
    }
    return m_dataType;
}

void Enum::VerifyAndComplete()
{
    bool isEnumClass = StringStartsWith(m_enumTypedef, "enum ") &&
        StringEndsWith(m_enumTypedef, " class");
    std::string innerPrefix;

    if (isEnumClass)
    {
        innerPrefix = m_innerName + "::";
    }
    else
    {
        size_t pos = m_innerName.rfind("::");
        if (pos != std::string::npos)
        {
            innerPrefix = m_innerName.substr(0, pos+2);
        }
        else
        {
            innerPrefix = "::";
        }
    }

    m_indexEnumerands.clear();
    m_possibleValues.clear();
    for (size_t i = 0; i < m_enumerands.size(); ++i)
    {
        EnumerandConstSP enumerand = m_enumerands[i];
        if (!enumerand)
            throw spi::RuntimeError("%s: NULL enumerand", __FUNCTION__);

        if (enumerand->value().empty())
        {
            std::string value = innerPrefix + enumerand->name();
            enumerand = Enumerand::Make(enumerand->description(),
                enumerand->name(), value, enumerand->alternates());
            m_enumerands[i] = enumerand;
        }

        if (m_enumerands[i]->alternates().size() > 0)
        {
            // when we have alternates then the name of the enumerand
            // is not used in string comparisons
            for (size_t j = 0; j < m_enumerands[i]->alternates().size(); ++j)
            {
                const std::string& alternate = m_enumerands[i]->alternates()[j];
                const std::string& uc = spi::StringUpper(alternate);
                if (m_indexEnumerands.count(uc) > 0)
                    throw spi::RuntimeError("%s: %s is not a unique enumerand",
                                            __FUNCTION__, uc.c_str());
                m_indexEnumerands[uc] = m_enumerands[i]->name();
                m_possibleValues.push_back(alternate);
            }
        }
        else
        {
            const std::string& uc = spi::StringUpper(m_enumerands[i]->name());
            if (m_indexEnumerands.count(uc) > 0)
                throw spi::RuntimeError("%s: %s is not a unique enumerand",
                                        __FUNCTION__, uc.c_str());
            m_indexEnumerands[uc] = m_enumerands[i]->name();
            m_possibleValues.push_back(m_enumerands[i]->name());
        }
    }
}

void Enum::declareTypeConversions(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc) const
{
    if (!m_innerName.empty())
    {
        ostr << "\n"
             << "class " << m_name << ";\n";
        ostr << "\n"
             << svc->getImport() << "\n"
             << m_innerName << " " << m_name << "_convert_in(const " << m_name << "&);\n";
        ostr << "\n"
             << svc->getImport() << "\n"
             << m_name << " " << m_name << "_convert_out(" << m_innerName << ");\n";
    }
}

void Enum::writeInnerHeader(
    GeneratedOutput & ostr) const
{
    writeInclude(ostr, m_innerHeader);
}

void Enum::writeInnerPreDeclaration(
    GeneratedOutput& ostr,
    NamespaceManager& nsm) const
{
    // namespace is that of the inner type
    if (m_innerHeader.empty() && !m_enumTypedef.empty() && m_enumTypedef != "enum")
    {
        std::string enumTypedef1;
        std::string enumTypedef2;
        SplitTypedef(m_enumTypedef, enumTypedef1, enumTypedef2);

        size_t pos = m_innerName.find_last_of("::");
        if (pos != std::string::npos)
        {
            SPI_POST_CONDITION(pos > 0); // because we searched for 2-character string
            nsm.startNamespace(ostr, m_innerName.substr(0, pos-1));
            nsm.indent(ostr);
            ostr << enumTypedef1 << " " << m_innerName.substr(pos + 1) << enumTypedef2 << ";\n";
        }
        else
        {
            nsm.endAllNamespaces(ostr);
            nsm.indent(ostr);
            ostr << enumTypedef1 << " " << m_innerName << enumTypedef2 << ";\n";
        }
    }
}
