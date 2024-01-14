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
#ifndef SPPY_WRITER_HPP
#define SPPY_WRITER_HPP

#include <spi/RefCounter.hpp>
#include <spi/ObjectHelper.hpp>
#include <spi/spdoc_namespace.hpp>

SPI_DECLARE_RC_CLASS(PythonService);
SPI_DECLARE_RC_CLASS(PythonModule);

class GeneratedOutput;

SPDOC_BEGIN_NAMESPACE
    SPI_DECLARE_OBJECT_CLASS(Service);
    SPI_DECLARE_OBJECT_CLASS(Module);
    SPI_DECLARE_OBJECT_CLASS(Function);
    SPI_DECLARE_OBJECT_CLASS(Class);
SPDOC_END_NAMESPACE

struct Options
{
    Options() :
        noGeneratedCodeNotice(false),
        noImporter(false),
        verbose(false),
        lowerCase(false),
        lowerCaseMethod(false),
        objectCoerce(false),
        helpFuncList(false),
        keywords(false),
        fastCall(false),
        license(),
        writeBackup(false)
    {}

    bool noGeneratedCodeNotice;
    bool noImporter;
    bool verbose;
    bool lowerCase;
    bool lowerCaseMethod;
    bool objectCoerce;
    bool helpFuncList;
    bool keywords;
    bool fastCall;
    std::string license;
    bool writeBackup;
};

/**
***************************************************************************
** Defines the PythonService class.
**
** Contains an instance of spdoc::Service and provides writer methods for
** the Python extension build.
***************************************************************************
*/
class PythonService : public spi::RefCounter
{
public:
    static PythonServiceConstSP Make(
        const spdoc::ServiceConstSP& service,
        const Options& options);

    std::string writeDeclSpecHeaderFile(const std::string& dirname) const;
    std::string writePydHeaderFile(const std::string& dirname) const;
    std::string writePydSourceFile(const std::string& dirname) const;
    std::string writePyImporter(const std::string& dirname) const;

    const std::string& name() const;
    const std::string& ns() const;
    const std::string& import() const;
    const spdoc::ServiceConstSP& service() const;

protected:
    PythonService(
        const spdoc::ServiceConstSP& service,
        const Options& options);

private:

    spdoc::ServiceConstSP m_service;
    std::string m_import;

public:
    const Options& options;
};

/**
***************************************************************************
** Defines the PythonModule class.
**
** Contains an instance of spdoc::Module and provides writer methods for
** the Python add-in build.
***************************************************************************
*/
class PythonModule : public spi::RefCounter
{
public:
    static PythonModuleConstSP Make(
        const PythonServiceConstSP& service,
        const spdoc::ModuleConstSP& module);

    std::string writeHeaderFile(const std::string& dirname) const;
    std::string writeSourceFile(const std::string& dirname) const;

protected:
    PythonModule(
        const PythonServiceConstSP& service,
        const spdoc::ModuleConstSP& module);

private:

    void declareFunction(GeneratedOutput& ostr,
                         const spdoc::Function* func) const;
    void implementFunction(GeneratedOutput& ostr,
                           const spdoc::Function* func) const;
    void registerFunction(GeneratedOutput& ostr,
                          const spdoc::Function* func) const;

    void declareClass(GeneratedOutput& ostr,
                      const spdoc::Class* cls) const;
    void implementClass(GeneratedOutput& ostr,
                        const spdoc::Class* cls) const;
    void registerClass(GeneratedOutput& ostr,
                       const spdoc::Class* cls) const;

    PythonServiceConstSP service;
    spdoc::ModuleConstSP module;
};

#endif
