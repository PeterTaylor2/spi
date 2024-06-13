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
#ifndef SPCS_WRITER_HPP
#define SPCS_WRITER_HPP

#include <spi/RefCounter.hpp>
#include <spi/ObjectHelper.hpp>
#include <spi/spdoc_namespace.hpp>
#include <spi/spdoc_publicType.hpp>

SPI_DECLARE_RC_CLASS(CService);
SPI_DECLARE_RC_CLASS(CModule);

class GeneratedOutput;

SPDOC_BEGIN_NAMESPACE
    SPI_DECLARE_OBJECT_CLASS(Service);
    SPI_DECLARE_OBJECT_CLASS(Module);
    SPI_DECLARE_OBJECT_CLASS(Function);
    SPI_DECLARE_OBJECT_CLASS(Class);
    SPI_DECLARE_OBJECT_CLASS(Enum);
    SPI_DECLARE_OBJECT_CLASS(DataType);
    SPI_DECLARE_OBJECT_CLASS(ClassMethod);
    SPI_DECLARE_OBJECT_CLASS(Attribute);
    SPI_DECLARE_OBJECT_CLASS(ClassAttribute);
SPDOC_END_NAMESPACE

struct Options
{
    Options()
        :
        noGeneratedCodeNotice(false),
        license(),
        writeBackup()
    {}

    bool noGeneratedCodeNotice;
    std::string license;
    bool writeBackup;
};

struct Usage
{
    Usage();

    bool hasVector;
    bool hasMatrix;
};

/**
***************************************************************************
** Defines the CService class.
**
** Contains an instance of spdoc::Service and provides writer methods for
** the C extension build.
***************************************************************************
*/
class CService : public spi::RefCounter
{
public:
    static CServiceConstSP Make(
        const spdoc::ServiceConstSP& service,
        const std::string& nsGlobal,
        const std::string& dllName,
        const std::string& companyName,
        const std::vector<std::string>& exclusions,
        const Options& options);

    std::string writeServiceFile(const std::string& dirname) const;
    std::string writeAssemblyInfo(const std::string& dirname) const;
    std::string writeEnumExtensionsFile(const std::string& dirname) const;

    const std::string& name() const;
    const std::string& ns() const;
    const std::string& nsGlobal() const;
    const std::string& import() const;
    const std::string& csDllImport() const;
    const std::string& spiImport() const;
    const spdoc::ServiceConstSP& service() const;

    bool noGeneratedCodeNotice() const;
    const std::string& license() const;
    bool writeBackup() const;

    std::string rename(const std::string& name) const;

protected:
    CService(
        const spdoc::ServiceConstSP& service,
        const std::string& nsGlobal,
        const std::string& dllName,
        const std::string& companyName,
        const std::vector<std::string>& exclusions,
        const Options& options);

private:

    spdoc::ServiceConstSP m_service;
    std::string m_nsGlobal;
    std::string m_dllName;
    std::string m_companyName;
    std::string m_import;
    std::string m_csDllImport;
    std::string m_spiImport;

    std::set<std::string> m_exclusions;

    Options m_options;
};

/**
***************************************************************************
** Defines the CModule class.
**
** Contains an instance of spdoc::Module and provides writer methods for
** the C add-in build.
***************************************************************************
*/
class CModule : public spi::RefCounter
{
public:
    static CModuleConstSP Make(
        const CServiceConstSP&  service,
        const spdoc::ModuleConstSP& module);

    std::string writeModuleFile(const std::string& dirname) const;
    static void updateInitClasses(GeneratedOutput& ostr, const spdoc::ModuleConstSP& module);
    static bool updateEnumExtensions(
        GeneratedOutput& ostr,
        const spdoc::ServiceConstSP& svc,
        const spdoc::ModuleConstSP& module);

    friend class CService;

protected:
    CModule(
        const CServiceConstSP&  service,
        const spdoc::ModuleConstSP& module);

private:

    void implementFunction(GeneratedOutput& ostr,
        const spdoc::Function* func) const;

    void implementClass(GeneratedOutput& ostr,
        const spdoc::Class* cls) const;

    void implementProperty(
        GeneratedOutput& ostr,
        const spdoc::ClassAttributeConstSP& attr,
        const std::string& cname,
        bool canPut,
        bool isOverride) const;

    void implementClassMethod(GeneratedOutput& ostr,
        const spdoc::Class* cls,
        const spdoc::ClassMethod* method,
        const std::string& cFuncName) const;

    void declarePlatformInvokeArgs(GeneratedOutput& ostr,
        const spdoc::Function* func,
        bool includeSelf) const;

    void implementEnum(GeneratedOutput& ostr,
        const spdoc::Enum* enumType) const;

    void implementFunctionSummary(
        GeneratedOutput& ostr,
        const spdoc::Function* func,
        size_t indent) const;

    void implementFunctionBegin(
        GeneratedOutput& ostr,
        const spdoc::Function* func,
        size_t indent) const;

    void implementFunctionEnd(
        GeneratedOutput& ostr,
        const spdoc::Function* func,
        size_t indent,
        const std::string& outputClassName) const;

    std::string defineValueTupleOutput(
        GeneratedOutput& ostr,
        const spdoc::Function* func) const;

    void implementPlatformInvoke(
        GeneratedOutput& ostr,
        const spdoc::Function* func,
        size_t indent,
        const char* sep) const;

    CServiceConstSP service;
    spdoc::ModuleConstSP module;
};

class CDataType
{
public:
    CDataType(const spdoc::DataTypeConstSP& dataType, const CServiceConstSP& service);

    // name used in the C-interface
    std::string cName() const;

    // type used within the body of a C# function preparatory to use via P/INVOKE
    std::string csiType(int arrayDim) const;

    // type used in the C# interface
    std::string csType(int arrayDim) const;

    // converts from either C# type from the interface or internal type to c-type
    std::string cs_to_c(int arrayDim, const std::string& name) const;

    std::string c_to_csi(int arrayDim, const std::string& name) const;
    std::string csi_to_cs(int arrayDim, const std::string& name) const;

private:

    spdoc::DataTypeConstSP dataType;
    spdoc::PublicType::Enum publicType;
    CServiceConstSP service;
};

#endif
