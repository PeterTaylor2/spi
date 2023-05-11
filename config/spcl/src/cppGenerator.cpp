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
#include "cppGenerator.hpp"

#include "moduleDefinition.hpp"
#include "generatorTools.hpp"

#include <spi_util/FileUtil.hpp>
#include <spi/StringUtil.hpp>

#include <iostream>

static void generateService(
    const ServiceDefinitionSP& svc,
    const std::string& dn,
    std::set<std::string>& fns,
    const Options& options,
    bool types=false);

static void generateModule(
    const ServiceDefinitionSP& svc,
    const ModuleDefinitionSP& module,
    const std::string& dn,
    std::set<std::string>& fns,
    bool noHeaderSplit,
    const std::string& license,
    bool types=false);

static void generateModuleConfig(
    const ServiceDefinitionSP& svc,
    const ModuleDefinitionSP& module,
    const std::string& dn,
    std::set<std::string>& fns);

static void tidyup(
    const ServiceDefinitionSP& svc,
    const std::string& dn,
    const std::set<std::string>& fns);

static void tidyupConfig(
    const ServiceDefinitionSP& svc,
    const std::string& dn,
    const std::set<std::string>& fns);

void generateCpp(
    const ServiceDefinitionSP& svc,
    const std::string& dn,
    const Options& options)
{
    std::set<std::string> fns;
    generateService(svc, dn, fns, options);
    if (options.allHeader)
    {
        std::string fn = spi_util::path::posix(spi::StringFormat("%s/%s_dll_all.hpp",
                dn.c_str(), svc->getName().c_str()));
        svc->writeAllHeader(fn, dn);
        fns.insert(fn);
    }
    const std::vector<ModuleDefinitionSP>& modules = svc->getModules();
    for (size_t i = 0; i < modules.size(); ++i)
        generateModule(svc, modules[i], dn, fns, options.noHeaderSplit, options.license);
    if (!options.noTidyup)
        tidyup(svc, dn, fns);
}

void generateTypes(const ServiceDefinitionSP& svc, const std::string& dn, const Options& options)
{
    std::set<std::string> fns;
    generateService(svc, dn, fns, options, true);
    const std::vector<ModuleDefinitionSP>& modules = svc->getModules();
    for (size_t i = 0; i < modules.size(); ++i)
        generateModule(svc, modules[i], dn, fns, true, options.license, true);
    if (!options.noTidyup)
        tidyup(svc, dn, fns);
}

void generateConfig(const ServiceDefinitionSP& svc, const std::string& dn)
{
    // we only generate the config files
    // thus leaving the service file to decide how much to wrap and the
    // overall namespace
    std::set<std::string> fns;
    //generateService(svc, dn, fns, true);
    const std::vector<ModuleDefinitionSP>& modules = svc->getModules();
    for (size_t i = 0; i < modules.size(); ++i)
        generateModuleConfig(svc, modules[i], dn, fns);
    tidyupConfig(svc, dn, fns);
}

static void tidyup(
    const ServiceDefinitionSP& svc,
    const std::string& dn,
    const std::set<std::string>& fns)
{
    std::set<std::string>::const_iterator iter;

    spi_util::Directory d(dn);
    for (iter = d.fns.begin(); iter != d.fns.end(); ++iter)
    {
        if ((spi::StringEndsWith(*iter, ".h") ||
             spi::StringEndsWith(*iter, ".hpp")) &&
             iter->find('_') != std::string::npos)
        {
            std::string ffn = spi_util::path::join(
                dn.c_str(), iter->c_str(), NULL);
            if (!fns.count(ffn))
            {
                std::cout << "Removing " << ffn << std::endl;
                remove(ffn.c_str());
            }
        }
    }

    spi_util::Directory d2(dn + "/src");
    for (iter = d2.fns.begin(); iter != d2.fns.end(); ++iter)
    {
        if ((spi::StringEndsWith(*iter, ".cpp") ||
             spi::StringEndsWith(*iter, ".hpp")) &&
             iter->find('_') != std::string::npos)
        {
            std::string ffn = spi_util::path::join(
                dn.c_str(), "src", iter->c_str(), NULL);
            if (!fns.count(ffn))
            {
                std::cout << "Removing " << ffn << std::endl;
                remove(ffn.c_str());
            }
        }
    }

}

static void tidyupConfig(
    const ServiceDefinitionSP& svc,
    const std::string& dn,
    const std::set<std::string>& fns)
{
    std::set<std::string>::const_iterator iter;

    spi_util::Directory d(dn);
    std::string prefix = svc->getNamespace() + "_";
    for (iter = d.fns.begin(); iter != d.fns.end(); ++iter)
    {
        if (spi::StringEndsWith(*iter, ".cfg"))
        {
            std::string ffn = spi_util::path::join(
                dn.c_str(), iter->c_str(), NULL);
            if (!fns.count(ffn))
            {
                std::cout << "Removing " << ffn << std::endl;
                remove(ffn.c_str());
            }
        }
    }
}


void generateService(
    const ServiceDefinitionSP& svc,
    const std::string& dn,
    std::set<std::string>& fns,
    const Options& options,
    bool types)
{
    const std::string& name = svc->getName();
    const std::string& ns   = svc->getNamespace();

    /*
    **********************************************************************
    * makefile properties file - this is an attempt to avoid lots of
    * repetitive definitions in the actual makefiles used - in particular
    * means that we can change the service names more readily
    **********************************************************************
    */
    std::string fn = spi_util::path::posix(
        spi::StringFormat("%s/properties.mk", dn.c_str()));
    svc->writeMakefileProperties(fn, dn, options.outputDir);
    fns.insert(fn);

    // remove the ns_version.mk file - the problem with having the namespace
    // in the name is that the file becomes redundant if we change the
    // namespace
    fn = spi_util::path::posix(
        spi::StringFormat("%s/%s_version.mk", dn.c_str(), ns.c_str()));
    if (spi_util::path::isfile(fn))
    {
        std::cout << "Removing " << fn << std::endl;
        remove(fn.c_str());
    }

    /*
    **********************************************************************
    * declspec (possibly misnamed)
    **********************************************************************
    */
    fn = spi_util::path::posix(
        spi::StringFormat("%s/%s_dll_decl_spec.h", dn.c_str(), name.c_str()));
    svc->writeDeclSpecHeader(fn, dn, options.license);
    fns.insert(fn);

    /*
    **********************************************************************
    * service namespace
    **********************************************************************
    */
    fn = spi_util::path::posix(
        spi::StringFormat("%s/%s_namespace.hpp", dn.c_str(), name.c_str()));
    svc->writeServiceNamespace(fn, dn, options.license, types);
    fns.insert(fn);

    /*
    ******************************************************************
    * service headers
    ******************************************************************
    */
    std::string fn1 = spi_util::path::posix(
        spi::StringFormat("%s/%s_dll_service.hpp", dn.c_str(), name.c_str()));
    std::string fn2 = spi_util::path::posix(
        spi::StringFormat("%s/src/%s_dll_service_manager.hpp", dn.c_str(), name.c_str()));
    svc->writeServiceHeaders(fn1, fn2, dn, options.license, types);
    fns.insert(fn1);
    fns.insert(fn2);

    /*
    ******************************************************************
    * time-out header
    ******************************************************************
    */
    fn = spi_util::path::posix(
        spi::StringFormat("%s/src/%s_dll_time_out.hpp", dn.c_str(), name.c_str()));
    svc->writeTimeoutHeader(fn, dn, options.license);
    fns.insert(fn);

    /*
    ******************************************************************
    * service source
    ******************************************************************
    */
    fn = spi_util::path::posix(
        spi::StringFormat("%s/src/%s_dll_service_manager.cpp", dn.c_str(), name.c_str()));
    svc->writeServiceSource(fn, dn, options.license, types);
    fns.insert(fn);

    /*
    ******************************************************************
    * inner type converters header
    ******************************************************************
    */
    fn = spi_util::path::posix(
        spi::StringFormat("%s/src/%s_dll_type_converters.hpp", dn.c_str(), name.c_str()));
    svc->writeTypeConvertersHeader(fn, dn, options.license);
    fns.insert(fn);
}


void generateModule(
    const ServiceDefinitionSP& svc,
    const ModuleDefinitionSP& module,
    const std::string& dn,
    std::set<std::string>& fns,
    bool noHeaderSplit,
    const std::string& license,
    bool types)
{
    const char* name  = module->name().c_str();
    // const char* serviceName = svc->getName().c_str();
    const char* ns   = svc->getNamespace().c_str();

    std::string hfn = spi_util::path::posix(
        spi::StringFormat("%s/%s_%s.hpp", dn.c_str(), ns, name));
    std::string hcfn = spi_util::path::posix(
        spi::StringFormat("%s/%s_%s_classes.hpp", dn.c_str(), ns, name));
    std::string hhfn = spi_util::path::posix(
        spi::StringFormat("%s/src/%s_%s_helper.hpp", dn.c_str(), ns, name));
    std::string hhcfn = spi_util::path::posix(
        spi::StringFormat("%s/src/%s_%s_helper_classes.hpp", dn.c_str(), ns, name));
    std::string ofn = spi_util::path::posix(
        spi::StringFormat("%s/src/%s_%s.cpp", dn.c_str(), ns, name));
    std::string ohfn = spi_util::path::posix(
        spi::StringFormat("%s/src/%s_%s_helper.cpp", dn.c_str(), ns, name));

    // const std::vector<ConstructConstSP>& constructs = module->getConstructs();
    // size_t nbConstructs = constructs.size();

    /*
    **********************************************************************
    * main header
    **********************************************************************
    */
    module->writeHeader(hfn, dn, svc, noHeaderSplit, license, types);
    fns.insert(hfn);

    /*
    **********************************************************************
    * forward header
    **********************************************************************
    */
    if (!noHeaderSplit)
    {
        module->writeClassesHeader(hcfn, dn, svc, license, types);
        fns.insert(hcfn);
    }

    /*
    **********************************************************************
    * helper header
    **********************************************************************
    */
    module->writeHelperHeader(hhfn, dn, svc, noHeaderSplit, license, types);
    fns.insert(hhfn);

    /*
    **********************************************************************
    * main implementation
    **********************************************************************
    */
    module->writeSource(ofn, dn, svc, license, types);
    fns.insert(ofn);

    /*
    **********************************************************************
    * helper implementation
    **********************************************************************
    */
    module->writeHelperSource(ohfn, dn, svc, license, types);
    fns.insert(ohfn);
}

static void generateModuleConfig(
    const ServiceDefinitionSP& svc,
    const ModuleDefinitionSP& module,
    const std::string& dn,
    std::set<std::string>& fns)
{

}

