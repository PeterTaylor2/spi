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
** moduleDefinition.cpp
***************************************************************************
** Implements the ModuleDefinition class.
***************************************************************************
*/

#include "moduleDefinition.hpp"

#include <spi/RuntimeError.hpp>
#include <spi/Service.hpp>
#include <spi/StringUtil.hpp>

#include <spi/spdoc_configTypes.hpp>

#include "construct.hpp"
#include "enum.hpp"
#include "serviceDefinition.hpp"
#include "innerClass.hpp"
#include "generatorTools.hpp"
#include "verbatim.hpp"
#include "verbatimConstruct.hpp"
#include "options.hpp"
#include "function.hpp"

#include <spgtools/licenseTools.hpp>

#include <iostream>

/*
***************************************************************************
** Implementation of ModuleDefinition
***************************************************************************
*/
ModuleDefinitionSP ModuleDefinition::Make(
    const std::string& name,
    const std::vector<std::string>& description,
    const std::string& moduleNamespace)
{
    return ModuleDefinitionSP(
        new ModuleDefinition(name, description, moduleNamespace));
}

ModuleDefinition::ModuleDefinition(
    const std::string& name,
    const std::vector<std::string>& description,
    const std::string& moduleNamespace)
    :
    m_name(name),
    m_description(description),
    m_moduleNamespace(moduleNamespace),
    m_constructs(),
    m_includes(),
    m_headerVerbatim()
{}

const std::string& ModuleDefinition::name() const
{
    return m_name;
}

const std::string& ModuleDefinition::moduleNamespace() const
{
    return m_moduleNamespace;
}

//std::string ModuleDefinition::getFullNamespace(const ServiceDefinitionSP& svc,
//                                               bool types) const
//{
//    std::string svcNamespace = svc->getNamespaceVersion();
//
//    if (types)
//        svcNamespace += "_types";
//
//    if (moduleNamespace.empty())
//        return svcNamespace;
//
//    return spi::StringFormat("%s::%s", svcNamespace.c_str(), moduleNamespace.c_str());
//}

void ModuleDefinition::addConstruct(const ConstructConstSP& construct)
{
    m_constructs.push_back(construct);
}

void ModuleDefinition::addEnum(const EnumConstSP& enumType)
{
    m_constructs.push_back(enumType);
    m_enums.push_back(enumType);
}

const std::vector<ConstructConstSP>& ModuleDefinition::constructs() const
{
    return m_constructs;
}

const std::vector<EnumConstSP>& ModuleDefinition::enums() const
{
    return m_enums;
}

void ModuleDefinition::addInclude(const std::string& include)
{
    m_includes.push_back(include);
}


const std::vector<std::string>& ModuleDefinition::includes() const
{
    return m_includes;
}

void ModuleDefinition::addVerbatim(const VerbatimConstSP& verbatim)
{
    if (m_constructs.size() == 0)
    {
        m_headerVerbatim.push_back(verbatim);
    }
    else
    {
        VerbatimConstructSP construct = VerbatimConstruct::Make(verbatim);
        m_constructs.push_back(construct);
    }
}

void ModuleDefinition::addInnerClass(const InnerClassConstSP& innerClass)
{
    m_innerClasses.push_back(innerClass);
}

void ModuleDefinition::addInnerClassTemplate(const InnerClassTemplateConstSP& innerClassTemplate)
{
    m_innerClassTemplates.push_back(innerClassTemplate);
}

spdoc::ModuleConstSP ModuleDefinition::getDoc() const
{
    std::vector<spdoc::ConstructConstSP> docs;
    for (size_t i = 0; i < m_constructs.size(); ++i)
    {
        spdoc::ConstructConstSP doc = m_constructs[i]->getDoc();
        if (doc)
            docs.push_back(doc);
    }

    return spdoc::Module::Make(m_name, m_description, m_moduleNamespace, docs);
}

void ModuleDefinition::writeHeader(
    const std::string& fn,
    const std::string& cwd,
    const ServiceDefinitionSP& svc,
    const Options& options,
    bool types)
{
    GeneratedOutput ostr(fn, cwd, options.writeBackup);
    writeLicense(ostr, options.license);
    startHeaderFile(ostr, fn);

    NamespaceManager nsman;

    bool isFirst = false;
    if (options.noHeaderSplit)
    {
        const std::string& sharedPtrInclude = svc->getSharedPtrInclude();

        ostr << "\n";
        ostr << "#include <spi/spi.hpp>\n";

        if (!sharedPtrInclude.empty())
        {
            ostr << "#include " << sharedPtrInclude << "\n";
        }

        // we include the previous module's header file - thus via a daisy-chain
        // we include all previous modules
        bool isFirst = svc->writePreviousModuleInclude(ostr, m_name, options.noHeaderSplit, false);

        for (size_t i = 0; i < m_innerClasses.size(); ++i)
            m_innerClasses[i]->preDeclare(ostr, nsman);

        for (size_t i = 0; i < m_innerClassTemplates.size(); ++i)
            m_innerClassTemplates[i]->preDeclare(ostr, nsman);

    }
    else
    {
        ostr << "\n"
             << "#include \"" << svc->getNamespace() << "_" << m_name << "_classes.hpp\"\n";
    }


    // nsman.startNamespace(ostr, getFullNamespace(svc, types), 0);
    svc->writeStartNamespace(ostr);
    nsman.startNamespace(ostr, m_moduleNamespace, 0);

    if (options.noHeaderSplit)
    {
        if (isFirst)
            svc->writeUsingImportedTypes(ostr);

        int count = 0;
        for (size_t i = 0; i < m_constructs.size(); ++i)
            count += m_constructs[i]->preDeclare(ostr, svc);
        if (count > 0)
            ostr << "\n";
        for (size_t i = 0; i < m_constructs.size(); ++i)
        {
            m_constructs[i]->declare(ostr, svc, types);
            m_constructs[i]->declareClassFunctions(ostr, svc);
        }
    }
    else
    {
        for (size_t i = 0; i < m_constructs.size(); ++i)
        {
            if (!m_constructs[i]->declareInClasses())
            {
                m_constructs[i]->declare(ostr, svc, types);
            }
            m_constructs[i]->declareClassFunctions(ostr, svc);
        }
    }

    ostr << "\n";
    nsman.endAllNamespaces(ostr);
    svc->writeEndNamespace(ostr);
    endHeaderFile(ostr, fn);
}

void ModuleDefinition::writeClassesHeader(
    const std::string& fn,
    const std::string& cwd,
    const ServiceDefinitionSP& svc,
    const Options& options,
    bool types)
{
    GeneratedOutput ostr(fn, cwd, options.writeBackup);
    writeLicense(ostr, options.license);
    startHeaderFile(ostr, fn);

    const std::string& sharedPtrInclude = svc->getSharedPtrInclude();

    ostr << "\n";
    ostr << "#include <spi/spi.hpp>\n";

    if (!sharedPtrInclude.empty())
    {
        ostr << "#include " << sharedPtrInclude << "\n";
    }

    // we include the previous module's header file - thus via a daisy-chain
    // we include all previous modules
    bool isFirst = svc->writePreviousModuleInclude(ostr, m_name, false, false);

    NamespaceManager nsman;

    for (size_t i = 0; i < m_innerClasses.size(); ++i)
        m_innerClasses[i]->preDeclare(ostr, nsman);

    for (size_t i = 0; i < m_innerClassTemplates.size(); ++i)
        m_innerClassTemplates[i]->preDeclare(ostr, nsman);

    // these appear in two places
    writeInnerPreDeclarations(ostr, nsman);

    nsman.endAllNamespaces(ostr);

    //nsman.startNamespace(ostr, getFullNamespace(svc, types), 0);
    svc->writeStartNamespace(ostr);
    nsman.startNamespace(ostr, m_moduleNamespace, 0);

    if (isFirst)
        svc->writeUsingImportedTypes(ostr);

    int count = 0;
    for (size_t i = 0; i < m_constructs.size(); ++i)
        count += m_constructs[i]->preDeclare(ostr, svc);
    if (count > 0)
        ostr << "\n";

    for (size_t i = 0; i < m_constructs.size(); ++i)
    {
        if (m_constructs[i]->declareInClasses())
        {
            m_constructs[i]->declare(ostr, svc, types);
        }
    }

    ostr << "\n";
    nsman.endAllNamespaces(ostr);
    svc->writeEndNamespace(ostr);
    endHeaderFile(ostr, fn);
}

void ModuleDefinition::writeHelperHeader(
    const std::string& fn,
    const std::string& cwd,
    const ServiceDefinitionSP& svc,
    const Options& options,
    bool types)
{
    GeneratedOutput ostr(fn, cwd, options.writeBackup);
    writeLicense(ostr, options.license);
    startHeaderFile(ostr, fn);

    if (options.noHeaderSplit)
    {
        ostr << "\n"
            << "#include \"" << svc->getNamespace() << "_" << m_name << ".hpp\"\n";
    }
    else
    {
        ostr << "\n"
            << "#include \"" << svc->getNamespace() << "_" << m_name << "_classes.hpp\"\n";
    }
    ostr << "#include \"" << svc->getTypeConvertersHeader() << "\"\n";

    NamespaceManager nsman;
    svc->writeStartNamespace(ostr);
    nsman.startNamespace(ostr, m_moduleNamespace, 0);

    for (size_t i = 0; i < m_constructs.size(); ++i)
        m_constructs[i]->declareHelper(ostr, svc, types);

    ostr << "\n"
         << "void " << m_name << "_register_object_types"
         << "(const spi::ServiceSP& svc);\n";

    ostr << "\n";
    nsman.endAllNamespaces(ostr);
    svc->writeEndNamespace(ostr);
    endHeaderFile(ostr, fn);
}

void ModuleDefinition::writeSource(
    const std::string& fn,
    const std::string& cwd,
    const ServiceDefinitionSP& svc,
    const Options& options,
    bool types)
{
    GeneratedOutput ostr(fn, cwd, options.writeBackup);
    writeLicense(ostr, options.license);
    startSourceFile(ostr, fn);
    if (!noGeneratedCodeNotice())
        writeGeneratedCodeNotice(ostr, fn);

    ostr << "\n"
         << "#include \"" << svc->getNamespace() << "_" << m_name << ".hpp\"\n"
         << "#include \"" << svc->getNamespace() << "_" << m_name << "_helper.hpp\"\n"
         << "#include \"" << svc->getName() << "_dll_service_manager.hpp\"\n"
         << "\n";

    for (size_t i = 0; i < m_includes.size(); ++i)
        ostr << "#include " << m_includes[i] << "\n";

    NamespaceManager nsman;
    //nsman.startNamespace(ostr, getFullNamespace(svc, types), 0);
    svc->writeStartNamespace(ostr);
    nsman.startNamespace(ostr, m_moduleNamespace, 0);

    for (size_t i = 0; i < m_constructs.size(); ++i)
    {
        m_constructs[i]->implement(ostr, svc, types);
    }

    ostr << "\n";

    nsman.endAllNamespaces(ostr);
    svc->writeEndNamespace(ostr);
    endSourceFile(ostr, fn);
}

void ModuleDefinition::writeHelperSource(
    const std::string& fn,
    const std::string& cwd,
    const ServiceDefinitionSP& svc,
    const Options& options,
    bool types)
{
    GeneratedOutput ostr(fn, cwd, options.writeBackup);
    writeLicense(ostr, options.license);
    startSourceFile(ostr, fn);
    if (!noGeneratedCodeNotice())
        writeGeneratedCodeNotice(ostr, fn);

    ostr << "\n"
         << "#include \"" << svc->getNamespace() << "_" << m_name << ".hpp\"\n"
         << "#include \"" << svc->getNamespace() << "_" << m_name << "_helper.hpp\"\n"
         << "#include \"" << svc->getName() << "_dll_service_manager.hpp\"\n";

    NamespaceManager nsman;
    //nsman.startNamespace(ostr, getFullNamespace(svc, types), 0);
    svc->writeStartNamespace(ostr);
    nsman.startNamespace(ostr, m_moduleNamespace, 0);

    for (size_t i = 0; i < m_constructs.size(); ++i)
        m_constructs[i]->implementHelper(ostr, svc, types);

    ostr << "\n"
         << "void " << m_name << "_register_object_types"
         << "(const spi::ServiceSP& svc)\n"
         << "{\n";

    for (size_t i = 0; i < m_constructs.size(); ++i)
        m_constructs[i]->implementRegistration(ostr, "svc", types);

    ostr << "}\n";

    ostr << "\n";
    nsman.endAllNamespaces(ostr);
    svc->writeEndNamespace(ostr);
    endSourceFile(ostr, fn);
}

void ModuleDefinition::declareTypeConversions(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc)
{
    for (size_t i = 0; i < m_headerVerbatim.size(); ++i)
        writeVerbatim(ostr, m_headerVerbatim[i], 0, true);
    for (size_t i = 0; i < m_constructs.size(); ++i)
        m_constructs[i]->declareTypeConversions(ostr, svc);
}

void ModuleDefinition::writeInnerHeaders(
    GeneratedOutput& ostr)
{
    for (size_t i = 0; i < m_headerVerbatim.size(); ++i)
    {
        const std::vector<std::string>& includes = m_headerVerbatim[i]->getIncludes();
        for (size_t j = 0; j < includes.size(); ++j)
            ostr << includes[j] << "\n";
    }

    for (size_t i = 0; i < m_constructs.size(); ++i)
        m_constructs[i]->writeInnerHeader(ostr);
}

void ModuleDefinition::writeInnerPreDeclarations(
    GeneratedOutput& ostr,
    NamespaceManager& nsm)
{
    for (size_t i = 0; i < m_constructs.size(); ++i)
        m_constructs[i]->writeInnerPreDeclaration(ostr, nsm);
}
