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
#ifndef MODULE_DEFINITION_HPP
#define MODULE_DEFINITION_HPP

/*
***************************************************************************
** moduleDefinition.hpp
***************************************************************************
** Defines the ModuleDefinition class.
***************************************************************************
*/

#include <vector>

#include <spi/Date.hpp>
#include <spi/Service.hpp>
#include <spi/ObjectSmartPtr.hpp>
#include <spi/spdoc_namespace.hpp>

SPI_DECLARE_RC_CLASS(Construct);
SPI_DECLARE_RC_CLASS(Enum);
SPI_DECLARE_RC_CLASS(ModuleDefinition);
SPI_DECLARE_RC_CLASS(ServiceDefinition);
SPI_DECLARE_RC_CLASS(Verbatim);
SPI_DECLARE_RC_CLASS(InnerClass);
SPI_DECLARE_RC_CLASS(InnerClassTemplate);
class GeneratedOutput;
class NamespaceManager;
struct Options;

SPDOC_BEGIN_NAMESPACE
SPI_DECLARE_OBJECT_CLASS(Module);
SPDOC_END_NAMESPACE

/**
 * Module definition.
 *
 * Consists of other components - classes / functions / typedefs etc.
 */
class ModuleDefinition : public spi::RefCounter
{
public:
    static ModuleDefinitionSP Make(
        const std::string& name,
        const std::vector<std::string>& description,
        const std::string& moduleNamespace);

    void addConstruct(const ConstructConstSP& construct);
    void addEnum(const EnumConstSP& enumType);
    const std::vector<ConstructConstSP>& constructs() const;
    const std::vector<EnumConstSP>& enums() const;
    void addInclude(const std::string& include);
    void addVerbatim(const VerbatimConstSP& verbatim);
    void addInnerClass(const InnerClassConstSP& innerClass);
    void addInnerClassTemplate(const InnerClassTemplateConstSP& innerClassTemplate);
    const std::vector<std::string>& includes() const;
    const std::string& name() const;
    const std::string& moduleNamespace() const;
    //std::string getFullNamespace(const ServiceDefinitionSP& svc,
    //    bool types) const;

    void writeHeader(
        const std::string& fn,
        const std::string& cwd,
        const ServiceDefinitionSP& svc,
        const Options& options,
        bool types);
    void writeClassesHeader(
        const std::string& fn,
        const std::string& cwd,
        const ServiceDefinitionSP& svc,
        const Options& options,
        bool types);
    void writeHelperHeader(
        const std::string& fn,
        const std::string& cwd,
        const ServiceDefinitionSP& svc,
        const Options& options,
        bool types);
    void writeSource(
        const std::string& fn,
        const std::string& cwd,
        const ServiceDefinitionSP& svc,
        const Options& options,
        bool types);
    void writeHelperSource(
        const std::string& fn,
        const std::string& cwd,
        const ServiceDefinitionSP& svc,
        const Options& options,
        bool types);
    void declareTypeConversions(
        GeneratedOutput& ostr,
        const ServiceDefinitionSP& svc);
    void writeInnerHeaders(
        GeneratedOutput& ostr);
    void writeInnerPreDeclarations(
        GeneratedOutput& ostr,
        NamespaceManager& nsm);

    spdoc::ModuleConstSP getDoc() const;

protected:
    ModuleDefinition(
        const std::string& name,
        const std::vector<std::string>& description,
        const std::string& moduleNamespace);

private:
    std::string m_name;
    std::vector<std::string> m_description;
    std::string m_moduleNamespace;
    std::vector<ConstructConstSP> m_constructs;
    std::vector<EnumConstSP> m_enums;
    std::vector<std::string> m_includes;
    std::vector<VerbatimConstSP> m_headerVerbatim;
    std::vector<InnerClassConstSP> m_innerClasses;
    std::vector<InnerClassTemplateConstSP> m_innerClassTemplates;
};

#endif
