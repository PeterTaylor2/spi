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
#ifndef SPXL_WRITER_HPP
#define SPXL_WRITER_HPP

#include <spi/RefCounter.hpp>
#include <spi/ObjectHelper.hpp>
#include <spi/spdoc_namespace.hpp>

SPI_DECLARE_RC_CLASS(ExcelService);
SPI_DECLARE_RC_CLASS(ExcelModule);

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
        funcNameSep("."),
        noGeneratedCodeNotice(false),
        nameAtEnd(false),
        upperCase(false),
        noObjectFuncs(false),
        errIsNA(false),
        writeBackup(false),
        license(),
        helpFunc("help_func"),
        helpFuncList(""),
        helpEnum("help_enum"),
        objectCoerce("object_coerce"),
        startLogging("start_logging"),
        stopLogging("stop_logging"),
        startTiming("start_timing"),
        stopTiming("stop_timing"),
        clearTimings("clear_timings"),
        getTimings("get_timings"),
        setErrorPopups("set_error_popups"),
        objectToString("object_to_string"),
        objectFromString("object_from_string"),
        objectGet("object_get"),
        objectPut("object_put"),
        objectToFile("object_to_file"),
        objectFromFile("object_from_file"),
        objectCount("object_count"),
        objectFree("object_free"),
        objectFreeAll("object_free_all"),
        objectList("object_list"),
        objectClassName("object_class_name"),
        objectSHA("object_sha"),
        xlTargetVersion(4)
    {
        verify();
    }

    std::string funcNameSep;
    bool noGeneratedCodeNotice;
    bool nameAtEnd;
    bool upperCase;
    bool noObjectFuncs;
    bool errIsNA;
    bool writeBackup;
    std::string license;
    std::string helpFunc;
    std::string helpFuncList;
    std::string helpEnum;
    std::string objectCoerce;
    std::string startLogging;
    std::string stopLogging;
    std::string startTiming;
    std::string stopTiming;
    std::string clearTimings;
    std::string getTimings;
    std::string setErrorPopups;
    std::string objectToString;
    std::string objectFromString;
    std::string objectGet;
    std::string objectPut;
    std::string objectToFile;
    std::string objectFromFile;
    std::string objectCount;
    std::string objectFree;
    std::string objectFreeAll;
    std::string objectList;
    std::string objectClassName;
    std::string objectSHA;
    int xlTargetVersion;

    void update(const std::string& fn);
    void verify() const;
};


/**
***************************************************************************
** Defines the ExcelService class.
**
** Contains an instance of spdoc::Service and provides writer methods for
** the Excel add-in build.
***************************************************************************
*/
class ExcelService : public spi::RefCounter
{
public:
    static ExcelServiceConstSP Make(
        const spdoc::ServiceConstSP& service,
        const Options& options);

    std::string writeDeclSpecHeaderFile(const std::string& dirname) const;
    std::string writeXllHeaderFile(const std::string& dirname) const;
    std::string writeXllSourceFile(const std::string& dirname) const;
    std::string writeVbaFile(const std::string& dirname) const;
    std::vector<std::string> translateVbaFiles(
        const std::string& outdir,
        const std::string& indir) const;

    const std::string& name() const;
    const std::string& longName() const;
    const std::string& ns() const;
    const std::string& import() const;
    const spdoc::ServiceConstSP& service() const;

    bool noGeneratedCodeNotice() const;
    bool nameAtEnd() const;
    bool nameMandatory() const;
    bool xlfuncNamesInUpperCase() const;
    const char* funcNameSep() const;
    const std::string& license() const;
    bool writeBackup() const;

    const Options& options() const;

protected:
    ExcelService(
        const spdoc::ServiceConstSP& service,
        const Options& options);

private:

    spdoc::ServiceConstSP m_service;
    std::string m_import;

    Options m_options;
    bool m_nameMandatory;
};

/**
***************************************************************************
** Defines the ExcelModule class.
**
** Contains an instance of spdoc::Module and provides writer methods for
** the Excel add-in build.
***************************************************************************
*/
class ExcelModule : public spi::RefCounter
{
public:
    static ExcelModuleConstSP Make(
        const ExcelServiceConstSP&  service,
        const spdoc::ModuleConstSP& module);

    std::string writeHeaderFile(const std::string& dirname) const;
    std::string writeSourceFile(const std::string& dirname) const;

protected:
    ExcelModule(
        const ExcelServiceConstSP&  service,
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

    ExcelServiceConstSP  service;
    spdoc::ModuleConstSP module;
};

#endif
