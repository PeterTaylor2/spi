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
#ifndef SERVICEDEFINITION_HPP
#define SERVICEDEFINITION_HPP

/*
***************************************************************************
** serviceDefinition.hpp
***************************************************************************
** Defines the serviceDefinition class.
***************************************************************************
*/

#include <vector>
#include <set>

#include <spi/Date.hpp>
#include <spi/Service.hpp>
#include <spi/ObjectSmartPtr.hpp>
#include <spi_util/FourPartVersion.hpp>
#include <spi/spdoc_namespace.hpp>

SPI_DECLARE_RC_CLASS(Description);
SPI_DECLARE_RC_CLASS(CppType);
SPI_DECLARE_RC_CLASS(DataType);
SPI_DECLARE_RC_CLASS(Class);
SPI_DECLARE_RC_CLASS(InnerClass);
SPI_DECLARE_RC_CLASS(InnerClassTemplate);
SPI_DECLARE_RC_CLASS(Enum);
SPI_DECLARE_RC_CLASS(ModuleDefinition);
SPI_DECLARE_RC_CLASS(ServiceDefinition);
SPI_DECLARE_RC_CLASS(Verbatim);
SPI_DECLARE_RC_CLASS(TypesLibrary);

struct Options;

#include <spgtools/generatedOutput.hpp>

SPDOC_BEGIN_NAMESPACE
SPI_DECLARE_OBJECT_CLASS(Service);
SPDOC_END_NAMESPACE

/**
 * Isolates the part of a service definition that is needed to
 * re-use types and base classes in another service.
 */
class TypesLibrary : public spi::RefCounter
{
public:
    static TypesLibraryConstSP Make(
        const std::string& name,
        const std::string& ns,
        const std::string& version,
        const std::string& lastModuleName,
        const std::vector<DataTypeConstSP>& dataTypes,
        const std::vector<DataTypeConstSP>& publicDataTypes,
        const std::vector<ClassConstSP>& classes,
        const std::vector<EnumConstSP>& enums);

    // removes types which are already defined in one of the existing types libraries
    // note that all the namespaces must be the same
    TypesLibraryConstSP RemoveDuplicates(const std::vector<TypesLibraryConstSP>& tls) const;

    const std::string& name() const;
    const std::string& ns() const;
    const std::string& version() const;
    const std::string& lastModuleName() const;
    const std::vector<DataTypeConstSP>& dataTypes() const;
    const std::vector<DataTypeConstSP>& publicDataTypes() const;
    const std::vector<ClassConstSP>& baseClasses() const;
    std::vector<InnerClassConstSP> baseInnerClasses() const;
    const std::vector<EnumConstSP>& enums() const;

protected:
    TypesLibrary(
        const std::string& name,
        const std::string& ns,
        const std::string& version,
        const std::string& lastModuleName,
        const std::vector<DataTypeConstSP>& dataTypes,
        const std::vector<DataTypeConstSP>& publicDataTypes,
        const std::vector<ClassConstSP>& classes,
        const std::vector<EnumConstSP>& enums);

private:
    std::string m_name;
    std::string m_ns;
    std::string m_version;
    std::string m_lastModuleName;
    std::vector<DataTypeConstSP> m_dataTypes;
    std::vector<DataTypeConstSP> m_publicDataTypes;
    std::vector<ClassConstSP>    m_baseClasses;
    std::vector<EnumConstSP>     m_enums;
};

/**
 * Service definition.
 *
 * Keeps track of modules, types etc.
 */
class ServiceDefinition : public spi::RefCounter
{
public:
    static ServiceDefinitionSP Make(
        const std::string& name,
        const std::string& dllName,
        const std::string& longName,
        const std::string& ns,
        const std::string& version,
        const std::string& declSpec,
        const std::string& sharedPtr,
        const std::string& sharedPtrInclude,
        bool noLog,
        bool useVersionedNamespace,
        const std::vector<std::string>& description,
        const std::string& helpFunc);

    // add methods do not allow duplicates
    // get methods return empty rather than throw exception if missing
    void importTypesLibrary(const TypesLibraryConstSP& tl, bool publicImport);
    void addModule(ModuleDefinitionSP& module);
    void addServiceLevelModule();
    ModuleDefinitionSP getModule(const std::string& name) const;
    const std::vector<ModuleDefinitionSP>& getModules() const;
    // some types (struct or class) can have an incomplete definition defined first
    // in such cases addDataType will return the previous defined DataType
    // although it will validate that the dataType(s) are equivalent
    DataTypeConstSP addDataType(const DataTypeConstSP& dataType,
        bool incompleteType=false);
    // at the end of the config parser we check to see if there are any incomplete types
    // in that case we report this as an error
    std::vector<std::string> incompleteTypes();
    void addPublicDataType(const DataTypeConstSP& dataType);
    DataTypeConstSP getDataType(const std::string& name,
        const std::string& ns=std::string(),
        bool isPublic=false) const;
    bool isIncompleteType(const std::string& name) const;
    void addStandardDataTypes();
    void setBaseService(const ServiceDefinitionConstSP& service);
    void addClass(const ClassConstSP& aClass);
    ClassConstSP getClass(const std::string& name) const;
    void addInnerClass(const InnerClassConstSP& anInnerClass);
    InnerClassConstSP getInnerClass(const std::string& name) const;
    void addInnerClassTemplate(const InnerClassTemplateConstSP& innerClassTemplate);
    InnerClassTemplateConstSP getInnerClassTemplate(const std::string& name) const;
    void addStartup(const VerbatimConstSP& verbatim, const std::string& dirname);
    void addShutdown(const VerbatimConstSP& verbatim);
    void addPermission(const VerbatimConstSP& verbatim);
    void addServiceInit(const VerbatimConstSP& verbatim);
    void addInclude(const std::string& include);

    const std::string& getName() const;
    const std::string& getLongName() const;
    const std::string& getNamespace() const;
    const spi_util::FourPartVersion& getVersion() const;
    std::string getVersionString() const;
    const std::string& getDeclSpec() const;
    const std::string& getDeclSpecHeader() const;
    const std::string& getSharedPtr() const;
    //const std::string& getSpDynamicCast() const;
    const std::string& getSharedPtrInclude() const;
    bool noLog() const;
    bool useVersionedNamespace() const;
    const std::string& getImport() const;
    const std::string& getTypeConvertersHeader() const;

    spdoc::ServiceConstSP getDoc() const;
    TypesLibraryConstSP getTypesLibrary() const;
    bool hasBaseService() const;

    std::string fullNamespace(const std::string& ns, const char* sep = "::") const;

    void writeMakefileProperties(const std::string& fn, const std::string& cwd,
        const std::string& outputDir);
    void writeDeclSpecHeader(
        const std::string& fn, 
        const std::string& cwd,
        const Options& options);
    void writeServiceNamespace(
        const std::string& fn,
        const std::string& cwd,
        const Options& options,
        bool types);
    void writeStartNamespace(GeneratedOutput& ostr);
    void writeEndNamespace(GeneratedOutput& ostr);
    void writeAllHeader(const std::string& fn, const std::string& cwd);
    void writeServiceHeaders(
        const std::string& fn1,
        const std::string& fn2,
        const std::string& cwd,
        const Options& options,
        bool types);
    void writeTimeoutHeader(
        const std::string& fn,
        const std::string& cwd,
        const Options& options);
    void writeServiceSource(
        const std::string& fn,
        const std::string& cwd,
        const Options& options,
        bool types);
    void writeTypeConvertersHeader(
        const std::string& fn,
        const std::string& cwd,
        const Options& options);
    bool writePreviousModuleInclude(
        GeneratedOutput& ostr, const std::string& moduleName,
        bool noHeaderSplit, bool helper);
    void writeUsingImportedTypes(
        GeneratedOutput& ostr);
    void writeUsingImportedHelperTypes(
        GeneratedOutput& ostr);
    void writeDeclareImportedConverters(
        GeneratedOutput& ostr);
    void writeUsingImportedConverters(
        GeneratedOutput& ostr);

protected:
    ServiceDefinition(
        const std::string& name,
        const std::string& dllName,
        const std::string& longName,
        const std::string& ns,
        const std::string& version,
        const std::string& declSpec,
        const std::string& sharedPtr,
        //const std::string& spDynamicCast,
        const std::string& sharedPtrInclude,
        bool noLog,
        bool useVersionedNamespace,
        const std::vector<std::string>& description,
        const std::string& helpFunc);

private:
    std::string m_name;
    std::string m_dllName;
    std::string m_longName;
    std::string m_namespace;
    spi_util::FourPartVersion m_version;
    std::string m_declSpec;
    std::string m_sharedPtr;
    //std::string m_spDynamicCast;
    std::string m_sharedPtrInclude;
    bool m_noLog;
    bool m_useVersionedNamespace;
    std::string m_helpFunc; // added to the first module if defined
    std::vector<std::string>        m_description;
    std::vector<DataTypeConstSP>    m_dataTypes;
    std::vector<DataTypeConstSP>    m_publicDataTypes;
    std::vector<ClassConstSP>       m_classes;
    std::vector<InnerClassConstSP>  m_innerClasses;
    std::vector<InnerClassTemplateConstSP>  m_innerClassTemplates;
    std::vector<ModuleDefinitionSP> m_modules;
    std::string m_startupDirname;
    VerbatimConstSP m_startup;
    VerbatimConstSP m_shutdown;
    VerbatimConstSP m_permission;
    VerbatimConstSP m_serviceInit;
    std::vector<std::string> m_includes;
    std::vector<TypesLibraryConstSP> m_importedTypes;
    std::vector<bool> m_publicImports;
    std::set<std::string> m_incompleteTypes;

    void VerifyAndComplete();
    void addModuleToIndex(const ModuleDefinitionSP& module);
    DataTypeConstSP addDataTypeToIndex(const DataTypeConstSP& dataType, bool incompleteType);
    void addClassToIndex(const ClassConstSP& aClass);
    void addInnerClassToIndex(const InnerClassConstSP& anInnerClass);
    void addInnerClassTemplateToIndex(const InnerClassTemplateConstSP& innerClassTemplate);

    std::map<std::string, ModuleDefinitionSP> m_indexModules;
    std::map<std::string, DataTypeConstSP>    m_indexDataTypes;
    std::map<std::string, DataTypeConstSP>    m_indexPublicDataTypes;
    std::map<std::string, ClassConstSP>       m_indexClasses;
    std::map<std::string, InnerClassConstSP>  m_indexInnerClasses;
    std::map<std::string, InnerClassTemplateConstSP> m_indexInnerClassTemplates;

    std::string                               m_declSpecHeader;
    std::string                               m_import;
    ServiceDefinitionConstSP                  m_baseService;
    std::string                               m_typeConvertersHeader;

    std::set<std::string> m_moduleNamespaces;
    std::string m_lastModuleNamespace;
};

#endif
