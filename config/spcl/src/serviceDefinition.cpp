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
** serviceDefinition.cpp
***************************************************************************
** Implements the serviceDefinition class.
***************************************************************************
*/

#include "serviceDefinition.hpp"

#include <spi/RuntimeError.hpp>
#include <spi/Service.hpp>
#include <spi/StringUtil.hpp>
#include <spi_util/FileUtil.hpp>

#include <spi/spdoc_configTypes.hpp>

#include "construct.hpp"
#include "constant.hpp"
#include "cppType.hpp"
#include "dataType.hpp"
#include "enum.hpp"
#include "innerClass.hpp"
#include "moduleDefinition.hpp"
#include "generatorTools.hpp"
#include "verbatim.hpp"
#include "wrapperClass.hpp"

#include <spgtools/licenseTools.hpp>

#include <iostream>

using spi_util::StringJoin;

/*
***************************************************************************
** Implementation of ServiceDefinition
***************************************************************************
*/
TypesLibraryConstSP TypesLibrary::Make(
    const std::string& name,
    const std::string& ns,
    const std::string& version,
    const std::string& lastModuleName,
    const std::vector<DataTypeConstSP>& dataTypes,
    const std::vector<DataTypeConstSP>& publicDataTypes,
    const std::vector<ClassConstSP>& classes,
    const std::vector<EnumConstSP>& enums)
{
    std::vector<DataTypeConstSP> definedDataTypes;
    std::vector<ClassConstSP> baseClasses;

    for (size_t i = 0; i < dataTypes.size(); ++i)
    {
        if (dataTypes[i]->isPrimitive())
            continue;

        definedDataTypes.push_back(dataTypes[i]);
    }

    for (size_t i = 0; i < classes.size(); ++i)
    {
        if (classes[i]->isAbstract())
            baseClasses.push_back(classes[i]);
    }

    return TypesLibraryConstSP(
        new TypesLibrary(name, ns, version, lastModuleName, definedDataTypes, publicDataTypes, baseClasses, enums));
}

TypesLibraryConstSP TypesLibrary::RemoveDuplicates(const std::vector<TypesLibraryConstSP>& tls) const
{
    std::set<std::string> dataTypeNames;
    std::set<std::string> publicDataTypeNames;
    std::set<std::string> baseClassNames;
    std::set<std::string> enumNames;

    for (size_t j = 0; j < tls.size(); ++j)
    {
        const TypesLibraryConstSP& tl = tls[j];
        if (tl->m_ns != m_ns)
            SPI_THROW_RUNTIME_ERROR("Namespaces for all types library must be the same");

        size_t N = tl->m_dataTypes.size();
        for (size_t i = 0; i < N; ++i)
        {
            dataTypeNames.insert(tl->m_dataTypes[i]->name());
        }

        N = tl->m_publicDataTypes.size();
        for (size_t i = 0; i < N; ++i)
        {
            publicDataTypeNames.insert(tl->m_publicDataTypes[i]->name());
        }

        N = tl->m_baseClasses.size();
        for (size_t i = 0; i < N; ++i)
        {
            baseClassNames.insert(tl->m_baseClasses[i]->getName(true, "."));
        }

        N = tl->m_enums.size();
        for (size_t i = 0; i < N; ++i)
        {
            enumNames.insert(tl->m_enums[i]->name());
        }
    }

    std::vector<DataTypeConstSP> dataTypes;
    std::vector<DataTypeConstSP> publicDataTypes;
    std::vector<ClassConstSP> baseClasses;
    std::vector<EnumConstSP> enums;

    size_t N = m_dataTypes.size();
    for (size_t i = 0; i < N; ++i)
    {
        if (dataTypeNames.count(m_dataTypes[i]->name()) == 0)
        {
            dataTypes.push_back(m_dataTypes[i]);
        }
    }

    N = m_publicDataTypes.size();
    for (size_t i = 0; i < N; ++i)
    {
        if (publicDataTypeNames.count(m_publicDataTypes[i]->name()) == 0)
        {
            publicDataTypes.push_back(m_publicDataTypes[i]);
        }
    }

    N = m_baseClasses.size();
    for (size_t i = 0; i < N; ++i)
    {
        if (baseClassNames.count(m_baseClasses[i]->getName(true, ".")) == 0)
        {
            baseClasses.push_back(m_baseClasses[i]);
        }
    }

    N = m_enums.size();
    for (size_t i = 0; i < N; ++i)
    {
        if (enumNames.count(m_enums[i]->name()) == 0)
        {
            enums.push_back(m_enums[i]);
        }
    }

    return TypesLibraryConstSP(new TypesLibrary(
        m_name, m_ns, m_version, m_lastModuleName,
        dataTypes, publicDataTypes, baseClasses, enums));
}

TypesLibrary::TypesLibrary(
    const std::string& name,
    const std::string& ns,
    const std::string& version,
    const std::string& lastModuleName,
    const std::vector<DataTypeConstSP>& dataTypes,
    const std::vector<DataTypeConstSP>& publicDataTypes,
    const std::vector<ClassConstSP>& baseClasses,
    const std::vector<EnumConstSP>& enums)
    :
    m_name(name),
    m_ns(ns),
    m_version(version),
    m_lastModuleName(lastModuleName),
    m_dataTypes(dataTypes),
    m_publicDataTypes(publicDataTypes),
    m_baseClasses(baseClasses),
    m_enums(enums)
{}

const std::string& TypesLibrary::name() const
{
    return m_name;
}

const std::string& TypesLibrary::ns() const
{
    return m_ns;
}

const std::string& TypesLibrary::version() const
{
    return m_version;
}

const std::string& TypesLibrary::lastModuleName() const
{
    return m_lastModuleName;
}

const std::vector<DataTypeConstSP>& TypesLibrary::dataTypes() const
{
    return m_dataTypes;
}

const std::vector<DataTypeConstSP>& TypesLibrary::publicDataTypes() const
{
    return m_publicDataTypes;
}

const std::vector<ClassConstSP>& TypesLibrary::baseClasses() const
{
    return m_baseClasses;
}

std::vector<InnerClassConstSP> TypesLibrary::baseInnerClasses() const
{
    size_t numBaseClasses = m_baseClasses.size();
    std::vector<InnerClassConstSP> innerClasses;
    for (size_t i = 0; i < numBaseClasses; ++i)
    {
        const ClassConstSP& baseClass = m_baseClasses[i];
        if (!baseClass->isWrapperClass())
            continue;

        const WrapperClass* wrapperClass = dynamic_cast<const WrapperClass*>(
            baseClass.get());

        if (!wrapperClass)
            throw spi::RuntimeError("%s says it is a wrapper class but it isn't",
                baseClass->getName(true, ".").c_str());

        innerClasses.push_back(wrapperClass->innerClass());
    }
    return innerClasses;
}

const std::vector<EnumConstSP>& TypesLibrary::enums() const
{
    return m_enums;
}

/*
***************************************************************************
** Implementation of ServiceDefinition
***************************************************************************
*/
ServiceDefinitionSP ServiceDefinition::Make(
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
    const std::string& helpFunc)
{
    return ServiceDefinitionSP(
        new ServiceDefinition(name, dllName, longName, ns, version, declSpec,
                              sharedPtr, /*spDynamicCast,*/ sharedPtrInclude,
                              noLog, useVersionedNamespace, description, helpFunc));
}

ServiceDefinition::ServiceDefinition(
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
    const std::string& helpFunc)
    :
    m_name(name),
    m_dllName(dllName),
    m_longName(longName),
    m_namespace(ns),
    m_version(version),
    m_declSpec(declSpec),
    m_sharedPtr(sharedPtr),
    //m_spDynamicCast(spDynamicCast),
    m_sharedPtrInclude(sharedPtrInclude),
    m_noLog(noLog),
    m_useVersionedNamespace(useVersionedNamespace),
    m_description(description),
    m_helpFunc(helpFunc),
    m_dataTypes(),
    m_publicDataTypes(),
    m_classes(),
    m_innerClasses(),
    m_innerClassTemplates(),
    m_modules(),
    m_startupDirname(),
    m_startup(),
    m_shutdown(),
    m_permission(),
    m_serviceInit(),
    m_includes(),
    m_importedTypes(),
    m_publicImports()
{
    VerifyAndComplete();
}

TypesLibraryConstSP ServiceDefinition::getTypesLibrary() const
{
    std::string lastModuleName;
    if (m_modules.size() > 0)
    {
        const ModuleDefinitionConstSP& module = m_modules.back();
        lastModuleName = spi::StringFormat("%s_%s",
            m_namespace.c_str(), module->name().c_str());
    }

    std::vector<EnumConstSP> enums;

    for (size_t i = 0; i < m_importedTypes.size(); ++i)
    {
        const std::vector<EnumConstSP>& importedEnums = m_importedTypes[i]->enums();
        enums.insert(enums.end(), importedEnums.begin(), importedEnums.end());
    }

    for (size_t i = 0; i < m_modules.size(); ++i)
    {
        const std::vector<EnumConstSP>& moduleEnums = m_modules[i]->enums();
        enums.insert(enums.end(), moduleEnums.begin(), moduleEnums.end());
    }

    return TypesLibrary::Make(
        m_name,
        m_namespace,
        m_version.versionString(),
        lastModuleName,
        m_dataTypes,
        m_publicDataTypes,
        m_classes,
        enums);
}

void ServiceDefinition::importTypesLibrary(const TypesLibraryConstSP& tl_in, bool publicImport)
{
    if (m_modules.size() > 0)
        throw spi::RuntimeError("Must import types library before defining "
            "any modules");

    TypesLibraryConstSP tl;
    if (m_importedTypes.size() > 0)
    {
        tl = tl_in->RemoveDuplicates(m_importedTypes);
    }
    else
    {
        tl = tl_in;
    }

    m_importedTypes.push_back(tl);
    m_publicImports.push_back(publicImport);

    const std::vector<DataTypeConstSP>& publicDataTypes = tl->publicDataTypes();

    // note that not all baseClasses are wrapper classes
    // so we need to collect the inner classes in a separate list

    if (!publicImport)
    {
        const std::vector<DataTypeConstSP>& dataTypes = tl->dataTypes();
        const std::vector<ClassConstSP>&    baseClasses = tl->baseClasses();
        const std::vector<InnerClassConstSP>& baseInnerClasses = tl->baseInnerClasses();

        for (size_t i = 0; i < baseClasses.size(); ++i)
        {
            addClass(baseClasses[i]);
        }

        for (size_t i = 0; i < baseInnerClasses.size(); ++i)
        {
            addInnerClass(baseInnerClasses[i]);
        }

        for (size_t i = 0; i < dataTypes.size(); ++i)
        {
            addDataType(dataTypes[i]);
        }
    }
    else
    {
        for (size_t i = 0; i < publicDataTypes.size(); ++i)
        {
            addDataType(publicDataTypes[i]);
        }
    }

    for (size_t i = 0; i < publicDataTypes.size(); ++i)
    {
        addPublicDataType(publicDataTypes[i]);
    }

}

void ServiceDefinition::addModule(ModuleDefinitionSP& module)
{
    const std::string& moduleNamespace = module->moduleNamespace();
    if (moduleNamespace != m_lastModuleNamespace)
    {
        if (!m_lastModuleNamespace.empty())
        {
            m_moduleNamespaces.insert(m_lastModuleNamespace);
        }
        m_lastModuleNamespace = moduleNamespace;
        if (!moduleNamespace.empty())
        {
            if (m_moduleNamespaces.count(moduleNamespace) > 0)
            {
                throw spi::RuntimeError("Duplicate module namespace '%s' not in sequence",
                    moduleNamespace.c_str());
            }
        }
    }
    addModuleToIndex(module);
    m_modules.push_back(module); // only after we tested for duplicates
}

void ServiceDefinition::addServiceLevelModule()
{
    std::vector<ConstructConstSP> constructs;

    if (!m_helpFunc.empty())
    {
        std::vector<std::string> description = {
            "Provides help for a given component or lists all components" };
        std::vector<std::string> returnTypeDescription = {
            "If name is undefined, then we return the list of all components.",
            "",
            "Otherwise we describe the component for the given name.",
            "Comments will start with #",
            "",
            "In order to get the information the file " + m_name + ".svo needs to be found in the startup directory" };

        const DataTypeConstSP stringType = getDataType("string");
        std::vector<std::string> code = {
            "    static spdoc::ServiceConstSP doc;"
            "    if (!doc)"
            "        doc = " + m_name + "_service_doc();"
            "",
            "    if (name.empty())"
            "        return doc->getConstructs();"
            "    return doc->getConstruct(name)->Summary(true);"
            "}"
        };

        FunctionConstSP func = Function::Make(
            description,
            returnTypeDescription,
            stringType,
            1,
            m_helpFunc,
            "",
            { FunctionAttribute::Make(
                Attribute::Make({}, stringType, "name", 0, true, StringConstant::Make("")),
                false) },
            Verbatim::Make("", 0, code),
            true, // noLog
            true, // noConvert
            {}, // excel options
            0, // cache size
            false); // optional return type

        constructs.push_back(func);
    }

    if (constructs.size() > 0)
    {
        ModuleDefinitionSP module = ModuleDefinition::Make("service_level", {}, {});
        for (const auto& construct : constructs)
            module->addConstruct(construct);
        addModule(module);
    }
}

ModuleDefinitionSP ServiceDefinition::getModule(const std::string& name) const
{
    std::map<std::string,ModuleDefinitionSP>::const_iterator iter =
        m_indexModules.find(name);

    if (iter == m_indexModules.end())
        return ModuleDefinitionSP();

    return iter->second;
}


const std::vector<ModuleDefinitionSP>& ServiceDefinition::getModules() const
{
    return m_modules;
}

DataTypeConstSP ServiceDefinition::addDataType(
    const DataTypeConstSP& dataType,
    bool incompleteType)
{
    DataTypeConstSP oldType = addDataTypeToIndex(dataType, incompleteType);
    if (oldType)
    {
        if (!incompleteType)
            m_dataTypes.push_back(oldType);
        return oldType;
    }

    if (!incompleteType)
        m_dataTypes.push_back(dataType);

    return dataType;
}

std::vector<std::string> ServiceDefinition::incompleteTypes()
{
    return std::vector<std::string>(m_incompleteTypes.begin(), m_incompleteTypes.end());
}

void ServiceDefinition::addPublicDataType(const DataTypeConstSP& dataType)
{
    const std::string& dataTypeName = dataType->name();
    if (m_indexPublicDataTypes.count(dataTypeName))
        SPI_THROW_RUNTIME_ERROR("Public DataType name " << dataTypeName << " already defined");
    m_indexPublicDataTypes[dataTypeName] = dataType;
    m_publicDataTypes.push_back(dataType);
}

DataTypeConstSP ServiceDefinition::getDataType(
    const std::string& name,
    const std::string& ns,
    bool isPublic) const
{
    std::map<std::string,DataTypeConstSP>::const_iterator iter;

    const std::map<std::string, DataTypeConstSP>& indexDataTypes = isPublic ?
        m_indexPublicDataTypes :
        m_indexDataTypes;

    if (!ns.empty())
    {
        // if we are looking for something when we are in the given namespace
        // then ns.name will be the type required (if it exists)
        //
        // this does mean that with the namespace we can define a new type
        // equal to a type defined in the global namespace, but we cannot
        // then use the type defined in the global namespace
        const std::string& fullName = ns + "." + name;
        iter = indexDataTypes.find(fullName);
        if (iter != indexDataTypes.end())
            return iter->second;
    }

    iter = indexDataTypes.find(name);

    if (iter == indexDataTypes.end())
        return DataTypeConstSP();

    return iter->second;
}

bool ServiceDefinition::isIncompleteType(const std::string& name) const
{
    if (m_incompleteTypes.count(name) > 0)
        return true;
    
    return false;
}

void ServiceDefinition::addClass(const ClassConstSP& aClass)
{
    addClassToIndex(aClass);
    m_classes.push_back(aClass); // only after we tested for duplicates
}

ClassConstSP ServiceDefinition::getClass(
    const std::string& name) const
{
    std::map<std::string,ClassConstSP>::const_iterator iter =
        m_indexClasses.find(name);

    if (iter == m_indexClasses.end())
        return ClassConstSP();

    return iter->second;
}

void ServiceDefinition::addInnerClass(const InnerClassConstSP& anInnerClass)
{
    addInnerClassToIndex(anInnerClass);
    m_innerClasses.push_back(anInnerClass); // only after we tested for duplicates
}

InnerClassConstSP ServiceDefinition::getInnerClass(
    const std::string& name) const
{
    std::map<std::string,InnerClassConstSP>::const_iterator iter =
        m_indexInnerClasses.find(name);

    if (iter == m_indexInnerClasses.end())
        return InnerClassConstSP();

    return iter->second;
}

void ServiceDefinition::addInnerClassTemplate(const InnerClassTemplateConstSP& innerClassTemplate)
{
    addInnerClassTemplateToIndex(innerClassTemplate);
    m_innerClassTemplates.push_back(innerClassTemplate); // only after we tested for duplicates
}

InnerClassTemplateConstSP ServiceDefinition::getInnerClassTemplate(
    const std::string& name) const
{
    std::map<std::string,InnerClassTemplateConstSP>::const_iterator iter =
        m_indexInnerClassTemplates.find(name);

    if (iter == m_indexInnerClassTemplates.end())
        return InnerClassTemplateConstSP();

    return iter->second;
}

void ServiceDefinition::addInclude(const std::string& include)
{
    m_includes.push_back(include);
}

void ServiceDefinition::addStartup(
    const VerbatimConstSP& verbatim,
    const std::string& dirname)
{
    if (!verbatim)
        return;

    if (m_startup)
        throw spi::RuntimeError("startup code already defined");

    m_startup = verbatim;
    m_startupDirname = dirname;
}

void ServiceDefinition::addShutdown(
    const VerbatimConstSP& verbatim)
{
    if (!verbatim)
        return;

    if (m_shutdown)
        throw spi::RuntimeError("shutdown code already defined");

    m_shutdown = verbatim;
}

void ServiceDefinition::addPermission(
    const VerbatimConstSP& verbatim)
{
    if (!verbatim)
        return;

    if (m_permission)
        throw spi::RuntimeError("permission code already defined");

    m_permission = verbatim;
}

void ServiceDefinition::addServiceInit(const VerbatimConstSP& verbatim)
{
    if (!verbatim)
        return;

    if (m_serviceInit)
        throw spi::RuntimeError("service initialisation code already defined");

    m_serviceInit = verbatim;

}

void ServiceDefinition::VerifyAndComplete()
{
    // FIXME: when do we call this function with all these values defined?

    m_indexModules.clear();
    for (size_t i = 0; i < m_modules.size(); ++i)
    {
        addModuleToIndex(m_modules[i]);
    }
    m_indexDataTypes.clear();
    for (size_t i = 0; i < m_dataTypes.size(); ++i)
    {
        addDataTypeToIndex(m_dataTypes[i], false);
    }
    for (size_t i = 0; i < m_classes.size(); ++i)
    {
        addClassToIndex(m_classes[i]);
    }
    for (size_t i = 0; i < m_innerClasses.size(); ++i)
    {
        addInnerClassToIndex(m_innerClasses[i]);
    }
    for (size_t i = 0; i < m_innerClassTemplates.size(); ++i)
    {
        addInnerClassTemplateToIndex(m_innerClassTemplates[i]);
    }
    m_declSpecHeader = spi::StringFormat("%s_dll_decl_spec.h", m_name.c_str());
    m_import = spi::StringFormat("%s_IMPORT", m_declSpec.c_str());
    m_typeConvertersHeader = spi::StringFormat("%s_dll_type_converters.hpp", m_name.c_str());
}

void ServiceDefinition::addModuleToIndex(const ModuleDefinitionSP& module)
{
    const std::string& moduleName = module->name();
    if (getModule(moduleName))
        throw spi::RuntimeError("Module name %s already defined",
                                moduleName.c_str());
    m_indexModules[moduleName] = module;
}

DataTypeConstSP ServiceDefinition::addDataTypeToIndex(
    const DataTypeConstSP& dataType,
    bool incompleteType)
{
    const std::string& dataTypeName = dataType->name();
    const DataTypeConstSP& other = getDataType(dataTypeName);

    if (other)
    {
        bool wasIncompleteType = m_incompleteTypes.count(dataTypeName) > 0;
        if (!wasIncompleteType)
        {
            SPI_THROW_RUNTIME_ERROR("DataType name " << dataTypeName
                << " already defined (and complete)");
        }

        bool equivalentType = other->isEqual(dataType);

        if(!equivalentType)
        {
            SPI_THROW_RUNTIME_ERROR("DataType name " << dataTypeName
                << " was defined as incomplete but is not the same");
        }

        SPI_POST_CONDITION(wasIncompleteType && equivalentType);

        // this is allowed

        // we are also allowed to pre-declare the type more than once
        // as long as we did it the same each time

        if (!incompleteType)
        {
            // we are fully defining the type at this point so remove from
            // the set of incomplete types
            m_incompleteTypes.erase(dataTypeName);
        }
        return other;
    }

    // not found previously
    m_indexDataTypes[dataTypeName] = dataType;
    if (incompleteType)
    {
        m_incompleteTypes.insert(dataTypeName);
    }
    return DataTypeConstSP(); // since we return the previously defined type
}

void ServiceDefinition::addClassToIndex(const ClassConstSP& aClass)
{
    const std::string& className = aClass->getName(true, ".");
    if (getClass(className))
        throw spi::RuntimeError("Class name %s already defined",
                                className.c_str());
    m_indexClasses[className] = aClass;
}

void ServiceDefinition::addInnerClassToIndex(
    const InnerClassConstSP& anInnerClass)
{
    const InnerClassConstSP& innerClass = getInnerClass(anInnerClass->name());
    // allowing innerClass to exist and have the same underlying pointer
    // allows us to import a type library which has more than one base
    // class wrapping the same inner type - since the inner type in that
    // case is the same pointer (due to the use of the ObjectWrapperCache
    // and MapRefCache at critical junctures)
    if (innerClass && innerClass.get() != anInnerClass.get())
    {
        throw spi::RuntimeError("Inner class name %s already defined",
                                anInnerClass->name().c_str());
    }
    if (!innerClass)
        m_indexInnerClasses[anInnerClass->name()] = anInnerClass;
}

void ServiceDefinition::addInnerClassTemplateToIndex(
    const InnerClassTemplateConstSP& innerClassTemplate)
{
    const InnerClassTemplateConstSP& previous = getInnerClassTemplate(innerClassTemplate->fullName());
    if (previous)
    {
        throw spi::RuntimeError("Inner class template '%s' already defined",
                                previous->fullName().c_str());
    }
    m_indexInnerClassTemplates[innerClassTemplate->fullName()] = innerClassTemplate;
}


void ServiceDefinition::setBaseService(const ServiceDefinitionConstSP& svc)
{
    // we need to call this function before we start adding our own modules
    // we can use any data type in the service
    // we add the classes so that we can sub-class any class
    //SPI_PRE_CONDITION(m_cppTypes.size() == 0);
    SPI_PRE_CONDITION(m_dataTypes.size() == 0);
    SPI_PRE_CONDITION(m_classes.size() == 0);
    SPI_PRE_CONDITION(m_modules.size() == 0);
    SPI_PRE_CONDITION(svc->m_modules.size() > 0);
    SPI_PRE_CONDITION(svc->m_namespace == m_namespace);
    SPI_PRE_CONDITION(m_importedTypes.size() == 0);

    m_dataTypes           = svc->m_dataTypes;
    m_publicDataTypes     = svc->m_publicDataTypes;
    m_classes             = svc->m_classes;
    m_indexDataTypes      = svc->m_indexDataTypes;
    m_indexPublicDataTypes = svc->m_indexPublicDataTypes;
    m_indexClasses        = svc->m_indexClasses;
    m_moduleNamespaces    = svc->m_moduleNamespaces;
    m_lastModuleNamespace = svc->m_lastModuleNamespace;
    m_importedTypes       = svc->m_importedTypes;
    m_baseService         = svc;
}

void ServiceDefinition::addStandardDataTypes()
{
    addDataType(DataType::MakePrimitive("int", "int"));
    addDataType(DataType::MakePrimitive("bool", "bool"));
    addDataType(DataType::MakePrimitive("double", "double"));
    addDataType(DataType::MakePrimitive("char", "char"));
    addDataType(DataType::MakePrimitive("string", "std::string&"));
    addDataType(DataType::MakePrimitive("date", "spi::Date"));
    addDataType(DataType::MakePrimitive("datetime", "spi::DateTime"));
    addDataType(DataType::MakePrimitive("Object", "spi::ObjectConstSP&"));
    addDataType(DataType::MakePrimitive("Variant", "spi::Variant&"));
    addDataType(DataType::MakePrimitive("Map", "spi::MapObjectSP&"));

    addDataType(DataType::Make(
        "long",
        "",
        "",
        "int",
        "int",
        "long",
        "long",
        spdoc::PublicType::INT,
        "",
        false,
        true,
        "%s",
        "spi_util::IntegerCast<int,long>(%s)",
        "",
        getDataType("int"),
        true));

    addDataType(DataType::Make(
        "size_t",
        "",
        "",
        "int",
        "int",
        "size_t",
        "size_t",
        spdoc::PublicType::INT,
        "",
        false,
        true,
        "spi_util::IntegerCast<size_t,int>(%s)",
        "spi_util::IntegerCast<int,size_t>(%s)",
        "",
        getDataType("int"),
        true));
}

const std::string& ServiceDefinition::getName() const
{
    return m_name;
}

const std::string& ServiceDefinition::getLongName() const
{
    return m_longName;
}

const std::string& ServiceDefinition::getNamespace() const
{
    return m_namespace;
}

const spi_util::FourPartVersion& ServiceDefinition::getVersion() const
{
    return m_version;
}

std::string ServiceDefinition::getVersionString() const
{
    return m_version.versionString();
}

const std::string& ServiceDefinition::getSharedPtr() const
{
    return m_sharedPtr;
}

//const std::string& ServiceDefinition::getSpDynamicCast() const
//{
//    return m_spDynamicCast;
//}


const std::string& ServiceDefinition::getSharedPtrInclude() const
{
    return m_sharedPtrInclude;
}

const std::string& ServiceDefinition::getDeclSpec() const
{
    return m_declSpec;
}

const std::string& ServiceDefinition::getDeclSpecHeader() const
{
    return m_declSpecHeader;
}

const std::string& ServiceDefinition::getImport() const
{
    return m_import;
}

const std::string& ServiceDefinition::getTypeConvertersHeader() const
{
    return m_typeConvertersHeader;
}

bool ServiceDefinition::noLog() const
{
    return m_noLog;
}

bool ServiceDefinition::useVersionedNamespace() const
{
    return m_useVersionedNamespace;
}

bool ServiceDefinition::hasBaseService() const
{
    if (!m_baseService)
        return false;

    return true;
}

std::string ServiceDefinition::fullNamespace(const std::string& ns, const char* sep) const
{
    std::vector<std::string> nss(1, m_namespace);
    if (!ns.empty())
        nss.push_back(ns);

    return StringJoin(sep, nss);
}

spdoc::ServiceConstSP ServiceDefinition::getDoc() const
{
    std::vector<spdoc::ModuleConstSP> moduleDocs;

    if (m_baseService)
        moduleDocs = m_baseService->getDoc()->modules;

    for (size_t i = 0; i < m_modules.size(); ++i)
        moduleDocs.push_back(m_modules[i]->getDoc());

    std::vector<spdoc::ClassConstSP> importedBaseClasses;
    std::vector<spdoc::EnumConstSP> importedEnums;

    for (size_t j = 0; j < m_importedTypes.size(); ++j)
    {
        if (!m_publicImports[j])
        {
            const std::vector<ClassConstSP>& v = m_importedTypes[j]->baseClasses();
            size_t n = v.size();
            for (size_t i = 0; i < n; ++i)
            {
                importedBaseClasses.push_back(
                    spdoc::Class::Coerce(v[i]->getDoc()));
            }
        }

        std::vector<EnumConstSP> enums = m_importedTypes[j]->enums();
        size_t n = enums.size();
        for (size_t i = 0; i < n; ++i)
            importedEnums.push_back(enums[i]->getDoc());
    }

    // note that all imported types have the same namespace
    // this is validated by TypesLibrary::RemoveDuplicates function

    bool sharedService = m_importedTypes.size() > 0 && (m_importedTypes[0]->ns() == m_namespace);

    return spdoc::Service::Make(m_name, m_description, m_longName,
        m_namespace, m_declSpec, m_version.versionString(), moduleDocs,
        importedBaseClasses, importedEnums, sharedService);
}

void ServiceDefinition::writeMakefileProperties(
    const std::string& fn,
    const std::string& cwd,
    const std::string& outputDir)
{
    GeneratedOutput ostr(fn, cwd, false);

    ostr << "U_SERVICE?=" << m_name << "\n"
         << "U_SERVICE_DLL?=" << m_dllName << "\n"
         << "U_SERVICE_DEBUG?=" << spi::StringUpper(m_namespace) << "_DEBUG\n"
         << "U_SERVICE_NAMESPACE=" << m_namespace << "\n"
         << "U_DECLSPEC=" << m_declSpec << "\n";

    if (!outputDir.empty())
    {
        ostr << "U_OUTPUT_DIR=" << outputDir << "\n";
    }

}

void ServiceDefinition::writeDeclSpecHeader(
    const std::string& fn,
    const std::string& cwd,
    const Options& options)
{
    GeneratedOutput ostr(fn, cwd, options.writeBackup);
    writeLicense(ostr, options.license);

    std::string guardMacro = headerGuardMacroName(fn);
    ostr << "#ifndef " << guardMacro << "\n"
         << "#define " << guardMacro << "\n"
         << "\n";

    writeStartCommentBlock(ostr, false);
    ostr << "** " << spi_util::path::basename(fn) << "\n"
         << "**\n"
         << "** Defines import/export declaration specifications.\n"
         << "**\n";
    writeEndCommentBlock(ostr);
    if (!noGeneratedCodeNotice())
        writeGeneratedCodeNotice(ostr, fn);

    ostr << "\n"
         << "#ifdef " << m_declSpec << "_STATIC\n"
         << "#define " << m_declSpec << "_IMPORT\n"
         << "#else\n";

    ostr << "\n"
         << "#include <spi_util/ImportExport.h>\n"
         << "\n"
         << "#ifdef " << m_declSpec << "_EXPORT\n"
         << "#define " << m_declSpec << "_IMPORT SPI_UTIL_DECLSPEC_EXPORT\n"
         << "#else\n"
         << "#define " << m_declSpec << "_IMPORT SPI_UTIL_DECLSPEC_IMPORT\n"
         << "#endif\n";

    ostr << "\n"
         << "#endif\n";

    ostr << "\n"
         << "#endif /* " << guardMacro << " */\n"
         << "\n";
}

void ServiceDefinition::writeServiceNamespace(
    const std::string& fn,
    const std::string& cwd,
    const Options& options,
    bool types)
{
    GeneratedOutput ostr(fn, cwd, options.writeBackup);
    writeLicense(ostr, options.license);
    startHeaderFile(ostr, fn);
    if (!noGeneratedCodeNotice())
        writeGeneratedCodeNotice(ostr, fn);

    //ostr << "\n"
    //     << "#define " << spi::StringUpper(m_namespace)
    //     << "_VERSION \"" << m_version.versionString() << "\"\n";

    if (m_useVersionedNamespace)
    {
        std::string namespaceVersion = spi::StringFormat(
            "%s_%x", m_namespace.c_str(), m_version.version());

        ostr << "\n"
             << "namespace " << namespaceVersion;

        if (types)
            ostr << "_types";

        ostr << " {}\n"
             << "\n"
             << "namespace " << m_namespace << " = " << namespaceVersion;

        if (types)
            ostr << "_types";

        ostr << ";\n"
             << "\n"
             << "#define " << spi::StringUpper(m_namespace)
             << "_BEGIN_NAMESPACE namespace "
             << namespaceVersion;

        if (types)
            ostr << "_types";
    }
    else
    {
        ostr << "\n"
             << "#define " << spi::StringUpper(m_namespace)
             << "_BEGIN_NAMESPACE namespace "
             << m_namespace;

        if (types)
            ostr << "_types";
    }

    ostr << " {\n"
         << "#define " << spi::StringUpper(m_namespace) << "_END_NAMESPACE }\n";

    endHeaderFile(ostr, fn);
}

void ServiceDefinition::writeStartNamespace(GeneratedOutput& ostr)
{
    ostr << "\n"
         << spi::StringUpper(m_namespace) << "_BEGIN_NAMESPACE\n";
}

void ServiceDefinition::writeEndNamespace(GeneratedOutput& ostr)
{
    ostr << spi::StringUpper(m_namespace) << "_END_NAMESPACE\n";
}

void ServiceDefinition::writeAllHeader(const std::string& fn, const std::string& cwd)
{
    GeneratedOutput ostr(fn, cwd, false);
    startHeaderFile(ostr, fn);

    ostr << "\n"
        << "#include <spi/spi.hpp>\n"
        << "#include \"" << m_name << "_dll_service.hpp\"\n\n";

    size_t numModules = m_modules.size();
    for (size_t i = 0; i < numModules; ++i)
    {
        ostr << "#include \"" << m_namespace << "_" << m_modules[i]->name() << ".hpp\"\n";
    }

    endHeaderFile(ostr, fn);
}

void ServiceDefinition::writeServiceHeaders(
    const std::string& fn1,
    const std::string& fn2,
    const std::string& cwd,
    const Options& options,
    bool types)
{
    // fn1 is in the public directory and contains all the exported functions
    // fn2 is in the source directory and contains all the internal functions
    GeneratedOutput ostr(fn1, cwd, options.writeBackup);
    writeLicense(ostr, options.license);
    startHeaderFile(ostr, fn1);
    if (!noGeneratedCodeNotice())
        writeGeneratedCodeNotice(ostr, fn1);

    ostr << "\n"
         << "#include <spi/spi.hpp>\n"
         << "#include <spi/spdoc_configTypes.hpp>\n"
         << "#include \"" << m_declSpecHeader << "\"\n"
         << "#include \"" << m_name << "_namespace.hpp\"\n";

    writeStartNamespace(ostr);

    //if (types)
    //    ostr << "_types";

    //ostr << " {\n";
    ostr << "\n"
        << m_import << "\n"
        << "spi::ServiceSP " << m_name << "_start_service();\n"
        << "\n"
        << m_import << "\n"
        << "void " << m_name << "_stop_service();\n"
        << "\n"
        << m_import << "\n"
        << "void " << m_name << "_start_logging(const char* filename, const char* options=\"\");\n"
        << "\n"
        << m_import << "\n"
        << "void " << m_name << "_stop_logging();\n"
        << "\n"
        << m_import << "\n"
        << "spi::ServiceSP " << m_name << "_exported_service();\n"
        << "\n"
        << m_import << "\n"
        << "spi::Date " << m_name << "_get_time_out();\n"
        << "\n"
        << m_import << "\n"
        << "const char* " << m_name << "_version();\n"
        << "\n"
        << m_import << "\n"
        << "const char* " << m_name << "_startup_directory();\n"
        << "\n";

    if (!m_baseService)
    {
        ostr << m_import << "\n"
            << "spdoc::ServiceConstSP " << m_name << "_service_doc();\n";
    }

    ostr << "\n";
    writeEndNamespace(ostr);

    //if (types)
    //    ostr << "_types";
    //ostr << "\n";

    endHeaderFile(ostr, fn1);

    GeneratedOutput ostr2(fn2, cwd, options.writeBackup);
    writeLicense(ostr2, options.license);
    startHeaderFile(ostr2, fn2);
    if (!noGeneratedCodeNotice())
        writeGeneratedCodeNotice(ostr2, fn2);

    ostr2 << "\n"
         << "#include \"" << spi_util::path::basename(fn1) << "\"\n";
    writeStartNamespace(ostr2);

    //if (types)
    //    ostr2 << "_types";

    //ostr2 << " {\n";
    ostr2 << "\n"
        << "using spi::SafeCopy;\n"
        << "\n"
        << "spi::Service* " << m_name << "_service();\n"
        << "void " << m_name << "_check_permission();\n"
        << "bool " << m_name << "_begin_function(bool noLogging=false);\n"
        << "void " << m_name << "_end_function();\n"
        << "std::runtime_error " << m_name << "_catch_exception(\n"
        << "    bool isLogging,\n"
        << "    const char* name,\n"
        << "    std::exception& e);\n"
        << "std::runtime_error " << m_name << "_catch_all(\n"
        << "    bool isLogging,\n"
        << "    const char* name);\n";

    if (m_startup)
    {
        ostr2 << "void " << m_name << "_startup(const std::string& "
            << m_startupDirname << ");\n";
    }

    if (m_shutdown)
    {
        ostr2 << "void " << m_name << "_shutdown();\n";
    }

    ostr2 << "\n";
    writeEndNamespace(ostr2);

    //if (types)
    //    ostr2 << "_types";
    //ostr2 << "\n";

    endHeaderFile(ostr2, fn2);
}

void ServiceDefinition::writeTimeoutHeader(
    const std::string& fn,
    const std::string& cwd,
    const Options& options)
{
    GeneratedOutput ostr(fn, cwd, options.writeBackup);
    writeLicense(ostr, options.license);
    startHeaderFile(ostr, fn);
    if (!noGeneratedCodeNotice())
        writeGeneratedCodeNotice(ostr, fn);

    ostr << "\n"
        << "#include <spi/spi.hpp>\n"
        << "#include \"" << m_declSpecHeader << "\"\n"
        << "#include \"" << m_name << "_namespace.hpp\"\n";
    writeStartNamespace(ostr);

    ostr << "\n"
         << "void " << m_name << "_set_time_out(spi::Date timeout, const char* msg = NULL);\n"
         << "void " << m_name << "_check_time_out();\n"
         << "\n";

    writeEndNamespace(ostr);

    endHeaderFile(ostr, fn);
}

void ServiceDefinition::writeServiceSource(
    const std::string& fn,
    const std::string& cwd,
    const Options& options,
    bool types)
{
    GeneratedOutput ostr(fn, cwd, options.writeBackup);
    writeLicense(ostr, options.license);
    startSourceFile(ostr, fn);
    if (!noGeneratedCodeNotice())
        writeGeneratedCodeNotice(ostr, fn);

    ostr << "\n"
         << "#include \"" << m_name << "_dll_service_manager.hpp\"\n"
         << "#include \"" << m_name << "_dll_time_out.hpp\"\n";

    if (m_baseService)
    {
        ostr << "#include \"" << m_baseService->m_name << "_dll_service_manager.hpp\"\n";
    }

    for (size_t j = 0; j < m_importedTypes.size(); ++j)
    {
        ostr << "#include \"" << m_importedTypes[j]->name() << "_dll_service.hpp\"\n";
    }

    ostr << "#include <spi/spdoc_dll_service.hpp>\n"
        << "#include <spi_util/FileUtil.hpp>\n";

    if (m_shutdown)
    {
        ostr << "#include <stdlib.h>\n";
    }

    ostr << "\n";

    for (size_t i = 0; i < m_modules.size(); ++i)
    {
        const ModuleDefinitionSP& module = m_modules[i];
        ostr << "#include \"" << m_namespace << "_"
             << module->name() << "_helper.hpp\"\n";
    }

    // some explanation here
    //
    // services tend to work better when you do an explicit start and stop
    // operations
    //
    // what I definitely think is required is an explicit shut-down operation
    // corresponding to start-up operations - this enables us to explicitly
    // delete singletons in the correct sequence
    //
    // just relying on the service going out of scope is not likely to be
    // reliable
    //
    // from within the library, each logging operation needs to be able
    // to get the service instance - we can trust ourselves not to delete
    // the object, hence get_service (not exported) returns a pointer
    // instead of a shared pointer
    //
    // MakeService (also not exported) actually does the work of creating
    // the service - calls to this function are controlled by start_service
    // and get_service functions

    if (m_includes.size() > 0)
    {
        ostr << "\n";
        for (size_t i = 0; i < m_includes.size(); ++i)
            ostr << "#include " << m_includes[i] << "\n";
    }

    NamespaceManager nsman;
    //if (types)
    //    nsman.startNamespace(ostr, m_namespaceVersion + "_types", 0);
    //else
    //    nsman.startNamespace(ostr, m_namespaceVersion, 0);
    writeStartNamespace(ostr);

    ostr << "\n"
         << "static char g_startup_directory[2048] = \"\";\n"
         << "static spi::ServiceSP g_service;\n"
         << "static const bool* g_is_logging = NULL;\n"
         << "static spi::Date g_time_out;\n"
         << "static bool g_timed_out = false;\n"
         << "static std::string g_time_out_error;\n";

    if (m_startup)
    {
        ostr << "\n"
             << "void " << m_name
             << "_startup(const std::string& " << m_startupDirname << ")\n"
             << "{";
        writeVerbatim(ostr, m_startup);
    }

    if (m_shutdown)
    {
        ostr << "\n"
             << "void " << m_name << "_shutdown()\n"
             << "{";
        writeVerbatim(ostr, m_shutdown);
    }

    // the MakeService function should be called once from <name>_init
    // it registers all the classes with the service to support deserialization
    ostr << "\n"
        << "static spi::ServiceSP MakeService()\n"
        << "{\n";

    if (m_baseService)
    {
        ostr << "    spi::ServiceSP baseSvc = " << m_baseService->m_name << "_exported_service();\n"
             << "    spi::ServiceSP svc = spi::Service::Make(\"" << m_name << "\", baseSvc);\n";
    }
    else if (m_importedTypes.size() > 0 && m_importedTypes[0]->ns() == m_namespace)
    {
        // note that all imported types have the same namespace
        // the service is defined for the namespace
        // hence each imported service defines the same spi::Service object
        // calling each of the exported_service functions ensures all types get registered
        ostr << "    spi::ServiceSP svc = " << m_importedTypes[0]->name() << "_exported_service();\n";
        for (size_t j = 1; j < m_importedTypes.size(); ++j)
            ostr << "    svc = " << m_importedTypes[j]->name() << "_exported_service();\n";
    }
    else
    {
        ostr << "    spi::ServiceSP svc = spi::Service::Make(\"" << m_name
            << "\", \"" << m_namespace << "\", \"" << m_version.versionString() << "\");\n";
    }
    ostr << "    g_is_logging = svc->is_logging_flag();\n";

    if (m_serviceInit)
    {
        writeVerbatim(ostr, m_serviceInit, 4, true);
    }

    ostr << "\n";
    for (size_t i = 0; i < m_modules.size(); ++i)
    {
        const ModuleDefinitionSP& module = m_modules[i];
        const std::string& moduleNamespace = module->moduleNamespace();
        ostr << "    ";
        if (!moduleNamespace.empty())
        {
            ostr << moduleNamespace << "::";
        }
        ostr << module->name() << "_register_object_types(svc);\n";
    }

    ostr << "\n"
        << "    return svc;\n"
        << "}\n";

    // the <name>_init function can be safely called from the public API
    // it will create the Service if not already created and also handle
    // Session initialisation, startup and shutdown
    ostr << "\n"
        << "static void " << m_name << "_init()\n"
        << "{\n"
        << "    if (!g_service)\n"
        << "    {\n";

    if (sessionLogging())
    {
        ostr << "        spi::session::start_session();\n" // the session must be started early
            << "\n";
    }

    ostr << "        g_service = MakeService();\n";

    // the service must exist before we call the startup routine
    if (m_startup)
    {
        ostr << "        " << m_name << "_startup(&g_startup_directory[0]);\n";
    }

    // not clear that atexit is the best way to handle the shutdown
    if (m_shutdown)
    {
        ostr << "\n"
             << "        atexit(" << m_name << "_shutdown);\n";
    }

    ostr << "    }\n"
         << "}\n";

    ostr << "\n"
         << "spi::ServiceSP " << m_name << "_start_service()\n"
         << "{\n"
         << "    " << m_name << "_init();\n"
         << "    g_service->start_up();\n"
         << "    return g_service;\n"
         << "}\n"
         << "\n"
         << "void " << m_name << "_stop_service()\n"
         << "{\n"
         << "    if (g_service)\n"
         << "        g_service->shut_down();\n"
         << "}\n"
         << "\n"
         << "spi::ServiceSP " << m_name << "_exported_service()\n"
         << "{\n"
         << "    " << m_name << "_init();\n"
         << "    return g_service;\n"
         << "}\n"
         << "\n"
         << "spi::Service* " << m_name << "_service()\n"
         << "{\n"
         << "    " << m_name << "_init();\n"
         << "    return g_service.get();\n"
         << "}\n"
         << "\n"
         << "const char* " << m_name << "_version()\n"
         << "{\n"
         << "    return \"" << m_version.versionString() << "\";\n"
         << "}\n"
         << "\n"
         << "bool " << m_name << "_begin_function(bool noLogging)\n"
         << "{\n"
         << "    " << m_name << "_check_permission();\n"
         << "    bool isLogging(false);\n"
         << "    if (g_is_logging && *g_is_logging)\n"
         << "    {\n"
         << "        int log_level = spi::IncrementLogLevel();\n"
         << "        isLogging = !noLogging && log_level == 0;\n"
         << "    }\n"
         << "    return isLogging;\n"
         << "}\n"
         << "\n"
         << "void " << m_name << "_end_function()\n"
         << "{\n"
         << "    if (g_is_logging && *g_is_logging)\n"
         << "        spi::DecrementLogLevel();\n"
         << "}\n"
         << "\n"
         << "\n"
         << "std::runtime_error " << m_name << "_catch_exception(bool isLogging, const char* name, std::exception& e)\n"
         << "{\n"
         << "    if (g_is_logging && *g_is_logging)\n"
         << "        spi::DecrementLogLevel();\n"
         << "    if (isLogging)\n"
         << "        g_service->log_error(e);\n"
         << "    return spi::RuntimeError(e, name);\n"
         << "}\n"
         << "\n"
         << "std::runtime_error " << m_name << "_catch_all(bool isLogging, const char* name)\n"
         << "{\n"
         << "    if (g_is_logging && *g_is_logging)\n"
         << "        spi::DecrementLogLevel();\n"
         << "    std::runtime_error e(\"Unknown exception\");\n"
         << "    if (isLogging)\n"
         << "        g_service->log_error(e);\n"
         << "    return spi::RuntimeError(e, name);\n"
         << "}\n"
         << "\n";

    ostr << "void " << m_name << "_start_logging(const char* filename, const char* options)\n"
        << "{\n"
        << "    g_service->start_logging(filename, options);\n"
        << "}\n"
        << "\n"
        << "void " << m_name << "_stop_logging()\n"
        << "{\n"
        << "    g_service->stop_logging();\n"
        << "}\n"
        << "\n";

    ostr << "void " << m_name << "_set_time_out(spi::Date timeout, const char* msg)\n"
         << "{\n"
         << "    g_time_out = timeout;\n"
         << "    g_timed_out = timeout && (spi::Date::Today() > timeout);\n"
         << "    if (g_timed_out)\n"
         << "    {\n"
         << "        std::ostringstream oss;\n"
         << "        oss << \"" << m_name << " timed out after \" << g_time_out.ToString();\n"
         << "        if (msg)\n"
         << "            oss << \"\\n\" << msg;\n"
         << "        g_time_out_error = oss.str();\n"
         << "    }\n"
         << "}\n"
         << "\n"
         << "spi::Date " << m_name << "_get_time_out()\n"
         << "{\n"
         << "    return g_time_out;\n"
         << "}\n"
         << "\n"
         << "void " << m_name << "_check_time_out()\n"
         << "{\n"
         << "    if (g_timed_out)\n"
         << "    {\n"
         << "        throw std::runtime_error(g_time_out_error);\n"
         << "    }\n"
         << "}\n"
         << "\n";

    ostr << "void " << m_name << "_check_permission()\n"
        << "{\n"
        << "    static bool g_has_permission = false;\n"
        << "    if (g_has_permission)\n"
        << "        return; // permission granted not revoked in current session\n"
        << "    " << m_name << "_check_time_out();\n";

    if (m_permission)
    {
        ostr << "    {";
        writeVerbatim(ostr, m_permission, 4);
    }

    ostr << "    g_has_permission = true;\n"
         << "}\n"
         << "\n";

    ostr << "const char* " << m_name << "_startup_directory()\n"
        << "{\n"
        << "    return &g_startup_directory[0];\n"
        << "}\n"
        << "\n";

    if (!m_baseService)
    {
        ostr << "spdoc::ServiceConstSP " << m_name << "_service_doc()\n"
            << "{\n"
            << "    spdoc::spdoc_start_service();\n"
            << "    std::string fn = spi_util::path::join(&g_startup_directory[0],\n"
            << "        \"" << m_name << ".svo\", 0);\n"
            << "    auto service_doc = spdoc::Service::from_file(fn);\n"
            << "    const std::vector<std::string>& satellites = " << m_name << "_service()->satellites();\n"
            << "    if (satellites.size() > 0)\n"
            << "    {\n"
            << "        std::vector<spdoc::ServiceConstSP> shared_services;\n"
            << "        for (const auto& satellite : satellites)\n"
            << "        {\n"
            << "            fn = spi_util::path::join(&g_startup_directory[0],\n"
            << "                (satellite + \".svo\").c_str(), 0);\n"
            << "            shared_services.push_back(spdoc::Service::from_file(fn));\n"
            << "        }\n"
            << "        service_doc = service_doc->CombineSharedServices(shared_services);\n"
            << "    }\n"
            << "    return service_doc;\n"
            << "}\n"
            << "\n";
    }

    nsman.endAllNamespaces(ostr);
    writeEndNamespace(ostr);

    // we also need a similar block of code for Linux
    ostr << "\n"
         << "#ifdef " << m_declSpec << "_EXPORT\n"
         << "#include <spi_util/FileUtil.hpp>\n"
         << "#ifdef _MSC_VER\n"
         << "#include <windows.h>\n"
         << "\n"
         << "extern \"C\"\n"
         << "BOOL WINAPI DllMain(\n"
         << "    HINSTANCE hDLL,\n"
         << "    DWORD callReason,\n"
         << "    LPVOID reserved)\n"
         << "{\n"
         << "    switch(callReason)\n"
         << "    {\n"
         << "    case DLL_PROCESS_ATTACH:\n"
         << "        try\n"
         << "        {\n";

    // we always want to find the startup directory
    // however we shouldn't risk too much activity at this point
    ostr << "            char buf[2048];\n"
         << "            DWORD size = 2048;\n"
         << "            if (GetModuleFileName(hDLL, buf, size) == size)\n"
         << "                throw std::runtime_error(\"Buffer overflow\");\n"
         << "            std::string dirname = spi_util::path::dirname(buf);\n"
         << "            strcpy(&" << m_namespace << "::g_startup_directory[0], dirname.c_str());\n"
         << "        }\n"
         << "        catch (...)\n"
         << "        {\n"
         << "            return FALSE;\n"
         << "        }\n"
         << "        return TRUE;\n"
         << "    case DLL_PROCESS_DETACH:\n"
         << "        try\n"
         << "        {\n"
         << "            " << m_namespace << "::" << m_name << "_stop_service();\n";
    ostr << "        }\n"
         << "        catch (...)\n"
         << "        {\n"
         << "            return FALSE;\n"
         << "        }\n"
         << "        return TRUE;\n"
         << "    }\n"
         << "    return TRUE;\n"
         << "}\n"
         << "\n"
         << "#endif\n";

    ostr << "\n"
         << "#ifdef __GNUC__\n"
         << "\n";

    // we always want to set g_startup_directory even if we don't have a startup routine
    ostr << "#ifndef _GNU_SOURCE\n"
        << "#define _GNU_SOURCE\n"
        << "#endif\n"
        << "\n"
        << "#ifdef __CYGWIN__\n"
        << "#undef __GNU_VISIBLE\n"
        << "#define __GNU_VISIBLE 1\n"
        << "#endif\n"
        << "\n"
        << "#include <dlfcn.h>\n"
        << "\n"
        << "extern \"C\"\n"
        << "void __attribute__((constructor))\n"
        << m_namespace << "_" << m_name << "_on_attach(void)\n"
        << "{\n"
        << "    try\n"
        << "    {\n"
        << "        Dl_info dl_info;\n"
        << "        if (dladdr((const void*)" << m_namespace << "_" << m_name << "_on_attach, &dl_info) != 0)\n"
        << "        {\n"
        << "            std::string dirname = spi_util::path::abspath(spi_util::path::dirname(dl_info.dli_fname));\n"
        << "            strcpy(&" << m_namespace << "::g_startup_directory[0], dirname.c_str());\n"
        << "        }\n"
        << "    }\n"
        << "    catch (std::exception& e)\n"
        << "    {\n"
        << "        fprintf(stderr, \"Error on attaching shared library: %s\\n\", e.what());\n"
        << "    }\n"
        << "}\n"
        << "\n";

    ostr << "extern \"C\"\n"
         << "void __attribute__((destructor))\n"
         << m_namespace << "_" << m_name << "_on_detach(void)\n"
         << "{\n"
         << "    try\n"
         << "    {\n"
         << "        " << m_namespace << "::" << m_name << "_stop_service();\n";

    ostr << "    }\n"
         << "    catch (std::exception& e)\n"
         << "    {\n"
         << "        fprintf(stderr, \"Error on detaching shared library: %s\\n\", e.what());\n"
         << "    }\n"
         << "}\n"
         << "\n"
         << "#endif\n";

    ostr << "\n"
         << "#endif /* " << m_declSpec << "_EXPORT */\n"
         << "\n";


    endSourceFile(ostr, fn);
}

void ServiceDefinition::writeTypeConvertersHeader(
    const std::string& fn,
    const std::string& cwd,
    const Options& options)
{
    GeneratedOutput ostr(fn, cwd, options.writeBackup);
    writeLicense(ostr, options.license);
    startHeaderFile(ostr, fn);

    ostr << "\n"
         << "#include <spi/spi.hpp>\n"
         << "#include \"" << m_name << "_namespace.hpp\"\n"
         << "#include \"" << m_name << "_dll_decl_spec.h\"\n";

    if (m_baseService)
    {
        ostr << "#include \"" << m_baseService->getTypeConvertersHeader() << "\"\n";
    }

    for (size_t i = 0; i < m_modules.size(); ++i)
    {
        const ModuleDefinitionSP& module = m_modules[i];
        module->writeInnerHeaders(ostr);
    }

    // note the innerPreDeclarations now appear in two places
    NamespaceManager nsm;
    for (size_t i = 0; i < m_modules.size(); ++i)
    {
        const ModuleDefinitionSP& module = m_modules[i];
        module->writeInnerPreDeclarations(ostr, nsm);
    }
    nsm.endAllNamespaces(ostr);

    writeDeclareImportedConverters(ostr);

    ostr << "\n"
         << spi::StringUpper(m_namespace) << "_BEGIN_NAMESPACE\n";

    writeUsingImportedConverters(ostr);

    NamespaceManager nsman;

    ServiceDefinitionSP svc = share_this(this);
    for (size_t i = 0; i < m_modules.size(); ++i)
    {
        const ModuleDefinitionSP& module = m_modules[i];
        nsman.startNamespace(ostr, module->moduleNamespace(), 0);
        module->declareTypeConversions(ostr, svc);
    }

    nsman.endAllNamespaces(ostr);
    ostr << "\n"
         << spi::StringUpper(m_namespace) << "_END_NAMESPACE\n";

    endHeaderFile(ostr, fn);
}

void ServiceDefinition::writeUsingImportedTypes(
    GeneratedOutput& ostr)
{
    for (size_t j = 0; j < m_importedTypes.size(); ++j)
    {
        const std::vector<DataTypeConstSP>& dataTypes = m_publicImports[j] ?
            m_importedTypes[j]->publicDataTypes() :
            m_importedTypes[j]->dataTypes();
        const std::vector<ClassConstSP>     baseClasses = m_importedTypes[j]->baseClasses();
        const std::string& importedNamespace = m_importedTypes[j]->ns();

        if (!m_publicImports[j])
        {
            if (baseClasses.size() > 0)
                ostr << "\n";
            for (size_t i = 0; i < baseClasses.size(); ++i)
            {
                // FIXME: what if the class has a namespace?
                ostr << "using " << importedNamespace << "::"
                    << baseClasses[i]->getName(true, "::") << ";\n";
            }
        }

        if (dataTypes.size() > 0)
            ostr << "\n";
        for (size_t i = 0; i < dataTypes.size(); ++i)
        {
            const DataTypeConstSP& dataType = dataTypes[i];

            if (dataType->ignored())
                continue;

            //const std::string& outerValueType = dataType->outerValueType();
            spdoc::PublicType publicType = dataType->publicType();

            // we only need to declare enums and classes
            // anything else is a simple type where the outer type is primitive
            switch (spdoc::PublicType::Enum(publicType))
            {
            case spdoc::PublicType::ENUM_AS_STRING:
            case spdoc::PublicType::ENUM_AS_INT:
                ostr << "using " << importedNamespace << "::"
                    << dataType->outerValueType() << ";\n";
                break;
            case spdoc::PublicType::CLASS:
                ostr << "using " << importedNamespace << "::"
                    << dataType->outerValueType() << ";\n";
                ostr << "using " << importedNamespace << "::"
                    << dataType->cppName() << ";\n";
                break;
            default:
                continue;
            }
        }
    }

}

void ServiceDefinition::writeUsingImportedHelperTypes(
    GeneratedOutput& ostr)
{
    for (size_t j = 0; j < m_importedTypes.size(); ++j)
    {
        if (m_publicImports[j])
            continue;

        const std::vector<ClassConstSP> baseClasses = m_importedTypes[j]->baseClasses();
        const std::string& importedNamespace = m_importedTypes[j]->ns();

        if (baseClasses.size() > 0)
            ostr << "\n";
        for (size_t i = 0; i < baseClasses.size(); ++i)
        {
            // FIXME: what if the class has a namespace?
            ostr << "using " << importedNamespace << "::"
                << baseClasses[i]->getName(true, "::") << "_Helper;\n";
        }
    }

}

void ServiceDefinition::writeUsingImportedConverters(
    GeneratedOutput& ostr)
{
    for (size_t j = 0; j < m_importedTypes.size(); ++j)
    {
        if (m_publicImports[j])
            continue;

        const std::vector<DataTypeConstSP>& dataTypes = m_importedTypes[j]->dataTypes();
        const std::string& importedNamespace = m_importedTypes[j]->ns();

        if (dataTypes.size() > 0)
            ostr << "\n";
        for (size_t i = 0; i < dataTypes.size(); ++i)
        {
            const DataTypeConstSP& dataType = dataTypes[i];
            spdoc::PublicType publicType = dataType->publicType();

            // we don't need converters for classes (they are within the class)
            switch (spdoc::PublicType::Enum(publicType))
            {
            case spdoc::PublicType::CLASS:
                continue;
            default:
                break;
            }
            if (dataType->needsTranslation())
            {
                ostr << "using " << importedNamespace << "::" << dataType->name()
                    << "_convert_in;\n";
                ostr << "using " << importedNamespace << "::" << dataType->name()
                    << "_convert_out;\n";
            }
        }
    }
}

bool ServiceDefinition::writePreviousModuleInclude(
    GeneratedOutput& ostr, const std::string& moduleName, bool noHeaderSplit, bool helper)
{
    std::string previousModule;
    for (size_t i = 0; i < m_modules.size(); ++i)
    {
        const ModuleDefinitionSP& module = m_modules[i];
        if (module->name() == moduleName)
            break;
        previousModule = module->name();
    }

    if (previousModule.empty())
    {
        if (!helper)
        {
            ostr << "#include \"" << m_name << "_dll_decl_spec.h\"\n";
        }
        else
        {
            // this is needed to provide type definitions for otherwise missing inner types
            ostr << "#include \"" << getTypeConvertersHeader() << "\"\n";
        }

        if (m_baseService)
        {
            ostr << "#include \"" << m_namespace << "_"
                 << m_baseService->m_modules.back()->name();
            if (helper)
                ostr << "_helper";
            if (!noHeaderSplit)
                ostr << "_classes";
            ostr << ".hpp\"\n";
        }
        else if (!helper)
        {
            ostr << "#include \"" << m_name << "_namespace.hpp\"\n";
        }

        for (size_t j = 0; j < m_importedTypes.size(); ++j)
        {
            const std::string lastModuleName = m_importedTypes[j]->lastModuleName();
            if (!(m_publicImports[j] && helper))
            {
                ostr << "#include \"" << lastModuleName;
                if (helper)
                    ostr << "_helper";
                if (!noHeaderSplit)
                    ostr << "_classes";
                ostr << ".hpp\"\n";
            }
        }
    }
    else
    {
        ostr << "#include \"" << m_namespace << "_" << previousModule;
        if (helper)
            ostr << "_helper";
        if (!noHeaderSplit)
            ostr << "_classes";
        ostr << ".hpp\"\n";
    }

    return previousModule.empty();
}

void ServiceDefinition::writeDeclareImportedConverters(
    GeneratedOutput& ostr)
{
    for (size_t j = 0; j < m_importedTypes.size(); ++j)
    {
        if (m_publicImports[j])
            continue;

        // note that this only works if we have access to the dll/src directory
        // for the library from which we are importing the types
        ostr << "\n"
            << "#include \"src/" << m_importedTypes[j]->name() << "_dll_type_converters.hpp\"\n";
    }

}
