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
#ifndef SPC_CWRITER_HPP
#define SPC_CWRITER_HPP

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
        imports(),
        license(),
        writeBackup()
    {}

    bool noGeneratedCodeNotice;
    std::vector<std::string> imports;
    std::string license;
    bool writeBackup;
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
        const Options& options);

    std::string writeDeclSpecHeaderFile(const std::string& dirname) const;
    std::string writePublicHeaderFile(const std::string& dirname,
        const std::vector<CModuleConstSP>& modules) const;
    std::string writePrivateHeaderFile(const std::string& dirname,
        const std::vector<CModuleConstSP>& modules) const;
    std::string writeSourceFile(const std::string& dirname) const;
 
    const std::string& name() const;
    const std::string& ns() const;
    const std::string& import() const;
    bool sharedService() const;
    const spdoc::ServiceConstSP& service() const;
    bool noGeneratedCodeNotice() const;
    const std::string& license() const;
    bool writeBackup() const;

protected:
    CService(
        const spdoc::ServiceConstSP& service,
        const Options& options);

private:

    spdoc::ServiceConstSP m_service;
    std::string m_import;
    Options m_options;

    void updateUsage(const spdoc::DataTypeConstSP& dataType, int arrayDim);
    void updateUsage(const spdoc::FunctionConstSP& function);
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

    void updatePublicHeaderFile(GeneratedOutput& ostr) const;
    void updatePrivateHeaderFile(GeneratedOutput& ostr) const;
    std::string writeSourceFile(const std::string& dirname) const;

    friend class CService;

protected:
    CModule(
        const CServiceConstSP&  service,
        const spdoc::ModuleConstSP& module);

private:

    void declareFunction(
        GeneratedOutput& ostr,
        const spdoc::Function* func,
        bool implementation = false) const;

    void implementFunction(GeneratedOutput& ostr,
                           const spdoc::Function* func) const;
 
    void declareClassConstructor(
        GeneratedOutput& ostr,
        const spdoc::Class* cls,
        bool implementation = false) const;

    void declareClass(GeneratedOutput& ostr,
                      const spdoc::Class* cls) const;
    void declarePrivateClass(GeneratedOutput& ostr,
                             const spdoc::Class* cls) const;
    void implementClass(GeneratedOutput& ostr,
                        const spdoc::Class* cls) const;

    void declareClassMethod(GeneratedOutput& ostr,
        const spdoc::Class* cls,
        const spdoc::ClassMethod* method,
        bool implementation = false) const;

    void implementClassMethod(GeneratedOutput& ostr,
        const spdoc::Class* cls,
        const spdoc::ClassMethod* method) const;

    void declareEnum(GeneratedOutput& ostr,
        const spdoc::Enum* enumType) const;
    void implementEnum(GeneratedOutput& ostr,
        const spdoc::Enum* enumType) const;

    CServiceConstSP service;
    spdoc::ModuleConstSP module;

    // adds inputs from function to the given arguments
    void functionDeclareArgs(
        std::vector<std::string>& args,
        const spdoc::Function* func) const;
};

/**
 * Given a return type + array dimensions this function returns the corresponding
 * attribute - the name will be "_out"
 */
spdoc::AttributeConstSP ReturnTypeAttribute(
    const spdoc::DataTypeConstSP& dataType, int arrayDim);

/**
 * Given an input attribute this function will return the argument in the context of
 * a C-function which can be P/INVOKE'd from C#.
 *
 * In the future this might be multiple arguments (thus we may be able to represent
 * an array as a size and a pointer), but currently it is a single argument.
 */
std::string CFunctionArg(const spdoc::AttributeConstSP& attr, bool output = false);
std::string CFunctionArg(const spdoc::ClassAttributeConstSP& attr, bool output = false);

class CDataType
{
public:
    CDataType(const spdoc::DataTypeConstSP& dataType);

    // type used in the C-interface
    std::string cType(int arrayDim = 0) const;

    // name used in the C++-interface
    std::string cppName() const;

    // type used in the C++ code
    std::string cppType(int arrayDim) const;

    std::string c_to_cpp(int arrayDim, const std::string& name) const;
    std::string cpp_to_c(int arrayDim, const std::string& name) const;
    std::string c_to_value(int arrayDim, const std::string& name) const;

private:

    spdoc::DataTypeConstSP dataType;
    spdoc::PublicType::Enum publicType;
};

#endif
