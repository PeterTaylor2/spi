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
#include "xlWriter.hpp"

#include <spi_util/FileUtil.hpp>
#include <spi_util/Utils.hpp>
#include <spi_util/JSON.hpp>
#include <spi/StringUtil.hpp>
#include <spi/spdoc_configTypes.hpp>

#include <spgtools/commonTools.hpp>
#include <spgtools/licenseTools.hpp>
#include <spgtools/generatedOutput.hpp>
#include <spgtools/namespaceManager.hpp>

#include <iostream>
#include <map>

#define CONSTRUCT_CAST(T, out, in) \
    spdoc::T const* out = dynamic_cast<spdoc::T const*>(in.get());\
    if (!out)\
        throw spi::RuntimeError("%s is of type %s and not %s", \
                                #in, in->get_class_name(), #T)

using spi_util::StringFormat;
using spi_util::StringUpper;
using spi_util::StringStartsWith;

/*
***************************************************************************
** Implementation of ExcelService
***************************************************************************
*/
ExcelServiceConstSP ExcelService::Make(
    const spdoc::ServiceConstSP& service,
    const Options& options)
{
    return new ExcelService(service, options);
}

ExcelService::ExcelService(
    const spdoc::ServiceConstSP& service,
    const Options& options)
    :
    m_service(service),
    m_import(),
    m_options(options),
    m_nameMandatory(options.nameAtEnd)
{
    m_import = spi::StringFormat("XL_%s_IMPORT", m_service->declSpec.c_str());
}

std::string
ExcelService::writeDeclSpecHeaderFile(const std::string& dirname) const
{
    std::string basename = spi::StringFormat("xll_%s_decl_spec.h", m_service->name.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename), writeBackup());
    writeLicense(ostr, license());
    startHeaderFile(ostr, filename);

    writeStartCommentBlock(ostr, false);
    ostr << "** " << basename << "\n"
         << "**\n"
         << "** If XL_" << m_service->declSpec << "_EXPORT is defined "
         << "then XL_" << m_service->declSpec << "_IMPORT is dllexport.\n"
         << "** Otherwise XL_" << m_service->declSpec << "_IMPORT is dllimport.\n";
    writeEndCommentBlock(ostr);
    if (!noGeneratedCodeNotice())
        writeGeneratedCodeNotice(ostr, filename);

    ostr << "\n"
         << "#ifdef XL_" << m_service->declSpec << "_EXPORT\n"
         << "#define XL_" << m_service->declSpec
         << "_IMPORT __declspec(dllexport)\n"
         << "#else\n"
         << "#define XL_" << m_service->declSpec
         << "_IMPORT __declspec(dllimport)\n"
         << "#endif\n";

    endHeaderFile(ostr, filename);
    ostr.close();
    return filename;
}

std::string
ExcelService::writeXllHeaderFile(const std::string& dirname) const
{
    std::string basename = spi::StringFormat("xll_%s.hpp", m_service->name.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename), writeBackup());
    writeLicense(ostr, license());
    startHeaderFile(ostr, filename);

    writeStartCommentBlock(ostr, false);
    ostr << "** " << basename << "\n"
         << "**\n"
         << "** Top level XLL functions for " << m_service->name << "\n";
    writeEndCommentBlock(ostr);
    if (!noGeneratedCodeNotice())
        writeGeneratedCodeNotice(ostr, filename);

    ostr << "\n"
         << "#include \"xll_" << m_service->name << "_decl_spec.h\"\n"
         << "#include <spi/Namespace.hpp>\n"
         << "\n"
         << "typedef struct xloper XLOPER;\n"
         << "\n"
         << "SPI_BEGIN_NAMESPACE\n"
         << "class ExcelService;\n"
         << "struct FunctionCaller;\n"
         << "class InputContext;\n"
         << "SPI_END_NAMESPACE\n"
         << "\n"
         << "spi::FunctionCaller* get_function_caller(const char* name);\n"
         << "spi::InputContext* get_input_context();\n"
         //<< "namespace " << m_service->ns << "\n"
         //<< "{\n"
         //<< "    spi::ExcelService* " << m_service->name << "_excel_service();\n"
         //<< "}\n"
         << "extern spi::ExcelService* " << m_service->name << "_excel_service;\n"
         << "\n"
         << "extern \"C\"\n"
         << "{\n"
         << "\n"
         << "/*\n"
         << std::string(75, '*') << "\n"
         << "** Standard Excel initialisation function.\n"
         << std::string(75, '*') << "\n"
         << "*/\n"
         << m_import << "\n"
         << "short xlAutoOpen(void);\n"
         << "\n"
         << "/*\n"
         << std::string(75, '*') << "\n"
         << "** Standard Excel cleanup function.\n"
         << std::string(75, '*') << "\n"
         << "*/\n"
         << m_import << "\n"
         << "short xlAutoClose(void);\n"
         << "\n"
         << "/*\n"
         << std::string(75, '*') << "\n"
         << "** Standard Excel function to release returned XLOPER memory.\n"
         << std::string(75, '*') << "\n"
         << "*/\n"
         << m_import << "\n"
         << "void xlAutoFree(XLOPER*);\n"
         << "\n"
         << "} /* end of extern \"C\" */\n";

    endHeaderFile(ostr, filename);
    ostr.close();
    return filename;
}

std::string
ExcelService::writeXllSourceFile(const std::string& dirname) const
{
    std::string basename = spi::StringFormat("xll_%s.cpp", m_service->name.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename), writeBackup());
    writeLicense(ostr, license());
    startSourceFile(ostr, filename);
    if (!noGeneratedCodeNotice())
        writeGeneratedCodeNotice(ostr, filename);

    ostr << "\n"
        << "#include \"xll_" << m_service->name << ".hpp\"\n"
        << "\n";

    for (size_t i = 0; i < m_service->modules.size(); ++i)
    {
        const spdoc::ModuleConstSP& module = m_service->modules[i];
        ostr << "#include \"xl_" << m_service->ns << "_"
             << module->name << ".hpp\"\n";
    }

    std::string xlServiceName = m_service->name + "_excel_service";

    ostr << "\n"
        << "#include <spi/ObjectHandle.hpp>\n"
        << "#include <spi/excel/xlService.hpp>\n"
        << "#include <spi/excel/xlUtil.hpp>\n"
        << "\n"
        << "#include <" << m_service->name << "_dll_service.hpp>\n"
        << "\n"
        << "spi::ExcelService* " << xlServiceName << " = NULL;\n"
        << "\n"
        << "spi::FunctionCaller* get_function_caller(const char* name)\n"
        << "{\n"
        << "    return " << xlServiceName << "->getFunctionCaller(name);\n"
        << "}\n"
        << "\n"
        << "spi::InputContext* get_input_context()\n"
        << "{\n"
        << "    return " << xlServiceName << "->getInputContext();\n"
        << "}\n"
        << "\n"
        << "/*\n"
        << std::string(75, '*') << "\n"
        << "** Standard Excel initialisation function.\n"
        << std::string(75, '*') << "\n"
        << "*/\n"
        << "short xlAutoOpen(void)\n"
        << "{\n"
        << "  std::string xllName;\n"
        << "  try {\n"
        << "    xllName = spi::GetXLLName();\n"
        << "    spi::xlInitializeState();\n"
        << "\n"
        << "    /* add-in registration */\n"
        << "    " << xlServiceName << " = new spi::ExcelService(" << m_service->ns
        << "::" << m_service->name << "_exported_service(), ";

    ostr << "xllName, \"" << funcNameSep() << "\", "
        << (m_options.upperCase ? "true" : "false")
        << ", false, " << (m_options.errIsNA ? "true" : "false") << ");\n";

    ostr << "\n"
        << "    /* function registration */\n";

    if (!m_service->sharedService)
    {
        ostr << "    " << xlServiceName << "->RegisterStandardFunctions(xllName,\n"
            << "        \"" << m_options.helpFunc << "\","
            << " \"" << m_options.helpFuncList << "\","
            << " \"" << m_options.helpEnum << "\","
            << " \"" << m_options.objectCoerce << "\","
            << " \"" << m_options.startLogging << "\","
            << " \"" << m_options.stopLogging << "\","
            << "\n        "
            << "\"" << m_options.startTiming << "\","
            << " \"" << m_options.stopTiming << "\","
            << " \"" << m_options.clearTimings << "\","
            << " \"" << m_options.getTimings << "\","
            << " \"" << m_options.setErrorPopups << "\","
            << "\n        ";

        if (m_options.noObjectFuncs)
        {
            ostr << "\"\","
                << " \"\","
                << " \"\","
                << " \"\","
                << " \"\","
                << " \"\","
                << " \"\","
                << " \"\","
                << " \"\","
                << " \"\","
                << " \"\","
                << " \"\","
                << " \"\"";
        }
        else
        {
            ostr << "\"" << m_options.objectToString << "\","
                << " \"" << m_options.objectFromString << "\","
                << " \"" << m_options.objectGet << "\","
                << " \"" << m_options.objectPut << "\","
                << " \"\"," // objectPutMetaData not yet supported
                << "\n        "
                << "\"" << m_options.objectToFile << "\","
                << " \"" << m_options.objectFromFile << "\","
                << " \"" << m_options.objectCount << "\","
                << " \"" << m_options.objectFree << "\","
                << " \"" << m_options.objectFreeAll << "\","
                << "\n        "
                << "\"" << m_options.objectList << "\","
                << " \"" << m_options.objectClassName << "\","
                << " \"" << m_options.objectSHA << "\"";
        }
        ostr << "\n    );\n\n";
    }

    for (size_t i = 0; i < m_service->modules.size(); ++i)
    {
        const spdoc::ModuleConstSP& module = m_service->modules[i];
        ostr << "    xlfuncs_register_" << m_service->ns << "_"
             << module->name << "(xllName, " << xlServiceName << ");\n";
    }

    ostr << "\n"
         << "    return 1;\n"
         << "  }\n"
         << "  catch (std::exception& e)\n"
         << "  {\n"
         << "    spi::StartupError(xllName, e.what());\n"
         << "    return 0;\n"
         << "  }\n"
         << "}\n"
         << "\n"
         << "/*\n"
         << std::string(75, '*') << "\n"
         << "** Standard Excel cleanup function.\n"
         << std::string(75, '*') << "\n"
         << "*/\n"
         << "short xlAutoClose(void)\n"
         << "{\n"
         << "    spi::ObjectHandleFreeAll();\n"
         << "    if (" << xlServiceName << ")\n"
         << "    {\n"
         << "        " << xlServiceName << "->UnregisterAllFunctions();\n"
         << "        delete " << xlServiceName << ";\n"
         << "        " << xlServiceName << " = 0;\n"
         << "    }\n"
         << "\n"
         << "    return 1;\n"
         << "}\n"
         << "\n"
         << "/*\n"
         << std::string(75, '*') << "\n"
         << "** Standard Excel function to release returned XLOPER memory.\n"
         << std::string(75, '*') << "\n"
         << "*/\n"
         << "void xlAutoFree(XLOPER* x)\n"
         << "{\n"
         << "    spi::xloperFree(x);\n"
         << "}\n";

    if (!m_service->sharedService)
    {

        // the functions defined in this section need to match the functions
        // defined in ExcelService::RegisterStandardFunctions
        ostr << "\n"
            << "extern \"C\"\n"
            << "{\n"
            << "\n";

        // whether these functions get registered is a matter for the
        // call to RegisterStandardFunctions

        ostr << m_import << "\n"
            << "XLOPER* xl_" << m_service->ns << "_help_func(\n"
            << "    XLOPER* name)\n"
            << "{\n"
            << "    return " << xlServiceName << "->HelpFunc(name);\n"
            << "}\n"
            << "\n"
            << m_import << "\n"
            << "XLOPER* xl_" << m_service->ns << "_help_func_list()\n"
            << "{\n"
            << "    return " << xlServiceName << "->HelpFunc(NULL);\n"
            << "}\n"
            << "\n"
            << m_import << "\n"
            << "XLOPER* xl_" << m_service->ns << "_object_coerce(\n"
            << "    XLOPER* className, XLOPER* value, XLOPER* baseName)\n"
            << "{\n"
            << "    return " << xlServiceName << "->ObjectCoerce(baseName, className, value);\n"
            << "}\n"
            << "\n"
            << m_import << "\n"
            << "XLOPER* xl_" << m_service->ns << "_help_enum(\n"
            << "    XLOPER* name)\n"
            << "{\n"
            << "    return " << xlServiceName << "->HelpEnum(name);\n"
            << "}\n"
            << "\n";

        if (!m_options.noObjectFuncs)
        {
            ostr << m_import << "\n"
                << "XLOPER* xl_" << m_service->ns << "_object_to_string(\n"
                << "    XLOPER* handle,\n"
                << "    XLOPER* format,\n"
                << "    XLOPER* options,\n"
                << "    XLOPER* metaData,\n"
                << "    XLOPER* mergeMetaData)\n"
                << "{\n"
                << "    return " << xlServiceName
                << "->ObjectToString(handle, format, options, metaData, mergeMetaData);\n"
                << "}\n"
                << "\n"
                << m_import << "\n"
                << "XLOPER* xl_" << m_service->ns << "_object_from_string(\n"
                << "    XLOPER* strings,\n"
                << "    XLOPER* baseName)\n"
                << "{\n"
                << "    return " << xlServiceName << "->ObjectFromString(baseName, strings);\n"
                << "}\n"
                << "\n"
                << m_import << "\n"
                << "XLOPER* xl_" << m_service->ns << "_object_get(\n"
                << "    XLOPER* handle,\n"
                << "    XLOPER* name)\n"
                << "{\n"
                << "    return " << xlServiceName << "->ObjectGet(handle, name);\n"
                << "}\n"
                << "\n"
                << m_import << "\n"
                << "XLOPER* xl_" << m_service->ns << "_object_put(\n"
                << "    XLOPER* handle,\n"
                << "    XLOPER* baseName,\n"
                << "    XLOPER* names,\n"
                << "    XLOPER* v1,\n"
                << "    XLOPER* v2,\n"
                << "    XLOPER* v3,\n"
                << "    XLOPER* v4,\n"
                << "    XLOPER* v5,\n"
                << "    XLOPER* v6,\n"
                << "    XLOPER* v7,\n"
                << "    XLOPER* v8,\n"
                << "    XLOPER* v9,\n"
                << "    XLOPER* v10,\n"
                << "    XLOPER* v11,\n"
                << "    XLOPER* v12,\n"
                << "    XLOPER* v13,\n"
                << "    XLOPER* v14,\n"
                << "    XLOPER* v15,\n"
                << "    XLOPER* v16,\n"
                << "    XLOPER* v17,\n"
                << "    XLOPER* v18,\n"
                << "    XLOPER* v19,\n"
                << "    XLOPER* v20,\n"
                << "    XLOPER* v21,\n"
                << "    XLOPER* v22,\n"
                << "    XLOPER* v23,\n"
                << "    XLOPER* v24,\n"
                << "    XLOPER* v25)\n"
                << "{\n"
                << "    return " << xlServiceName
                << "->ObjectPut(handle, baseName, names,\n"
                << "        v1, v2, v3, v4, v5, v6, v7, v8, v9, v10,\n"
                << "        v11, v12, v13, v14, v15, v16, v17, v18, v19, v20,\n"
                << "        v21, v22, v23, v24, v25);\n"
                << "}\n"
                << "\n"
                << m_import << "\n"
                << "XLOPER* xl_" << m_service->ns << "_object_to_file(\n"
                << "    XLOPER* handle, XLOPER* fileName, XLOPER* format, "
                << "XLOPER* options, XLOPER* metaData, XLOPER* mergeMetaData)\n"
                << "{\n"
                << "    return " << xlServiceName
                << "->ObjectToFile(handle, fileName, format, options, metaData, mergeMetaData);\n"
                << "}\n"
                << "\n"
                << m_import << "\n"
                << "XLOPER* xl_" << m_service->ns << "_object_from_file(\n"
                << "    XLOPER* fileName, XLOPER* baseName)\n"
                << "{\n"
                << "    return " << xlServiceName << "->ObjectFromFile(baseName, fileName);\n"
                << "}\n"
                << "\n"
                << m_import << "\n"
                << "XLOPER* xl_" << m_service->ns << "_object_count(\n"
                << "    XLOPER* className)\n"
                << "{\n"
                << "    return " << xlServiceName << "->ObjectCount(className);\n"
                << "}\n"
                << "\n"
                << m_import << "\n"
                << "XLOPER* xl_" << m_service->ns << "_object_free(\n"
                << "    XLOPER* handle)\n"
                << "{\n"
                << "    return " << xlServiceName << "->ObjectFree(handle);\n"
                << "}\n"
                << "\n"
                << m_import << "\n"
                << "XLOPER* xl_" << m_service->ns << "_object_free_all()\n"
                << "{\n"
                << "    return " << xlServiceName << "->ObjectFreeAll();\n"
                << "}\n"
                << "\n"
                << m_import << "\n"
                << "XLOPER* xl_" << m_service->ns << "_object_list(\n"
                << "    XLOPER* prefix, XLOPER* className)\n"
                << "{\n"
                << "    return " << xlServiceName << "->ObjectList(prefix, className);\n"
                << "}\n"
                << "\n"
                << m_import << "\n"
                << "XLOPER* xl_" << m_service->ns << "_object_class_name(\n"
                << "    XLOPER* handle)\n"
                << "{\n"
                << "    return " << xlServiceName << "->ObjectClassName(handle);\n"
                << "}\n"
                << "\n"
                << m_import << "\n"
                << "XLOPER* xl_" << m_service->ns << "_object_sha(\n"
                << "    XLOPER* handle,\n"
                << "    XLOPER* version)\n"
                << "{\n"
                << "    return " << xlServiceName << "->ObjectSHA(handle, version);\n"
                << "}\n";
        }

        ostr << "\n"
            << m_import << "\n"
            << "XLOPER* xl_" << m_service->ns << "_start_logging(\n"
            << "    XLOPER* filename, XLOPER* options)\n"
            << "{\n"
            << "    return " << xlServiceName << "->StartLogging(filename, options);\n"
            << "}\n"
            << "\n"
            << m_import << "\n"
            << "XLOPER* xl_" << m_service->ns << "_stop_logging()\n"
            << "{\n"
            << "    return " << xlServiceName << "->StopLogging();\n"
            << "}\n"
            << "\n"
            << m_import << "\n"
            << "XLOPER* xl_" << m_service->ns << "_start_timing()\n"
            << "{\n"
            << "    return " << xlServiceName << "->StartTiming();\n"
            << "}\n"
            << "\n"
            << m_import << "\n"
            << "XLOPER* xl_" << m_service->ns << "_stop_timing()\n"
            << "{\n"
            << "    return " << xlServiceName << "->StopTiming();\n"
            << "}\n"
            << "\n"
            << m_import << "\n"
            << "XLOPER* xl_" << m_service->ns << "_clear_timings()\n"
            << "{\n"
            << "    return " << xlServiceName << "->ClearTimings();\n"
            << "}\n"
            << "\n"
            << m_import << "\n"
            << "XLOPER* xl_" << m_service->ns << "_get_timings()\n"
            << "{\n"
            << "    return " << xlServiceName << "->GetTimings();\n"
            << "}\n"
            << "\n"
            << m_import << "\n"
            << "XLOPER* xl_" << m_service->ns << "_set_error_popups(\n"
            << "    XLOPER* errorPopups)\n"
            << "{\n"
            << "    return " << xlServiceName << "->SetErrorPopups(errorPopups);\n"
            << "}\n"
            << "\n"
            << "} /* end of extern \"C\" */\n";
    }

    endSourceFile(ostr, filename);
    ostr.close();
    return filename;
}

namespace
{
    std::string xlfunc(
        const std::string& ns,
        bool upperCase,
        const std::string& sep,
        const std::string& name)
    {
        std::ostringstream oss;
        if (!ns.empty())
        {
            oss << ns << sep;
        }
        oss << name;

        std::string xlf = oss.str();
        if (upperCase)
            return StringUpper(xlf);
        return xlf;
    }
} // end of anonymous namespace

std::vector<std::string> ExcelService::translateVbaFiles(
    const std::string& outdir,
    const std::string& indir) const
{
    std::vector<std::string> fns;

    // translate the utils.bas file
    {
        std::string fn = spi_util::StringFormat("%sUtils.bas", name().c_str());
        std::string ifn = spi_util::path::join(indir.c_str(), "utils.bas", (char*)0);
        std::string ofn = spi_util::path::join(outdir.c_str(), fn.c_str(), (char*)0);

        std::map<std::string, std::string> values;

        values["ns"] = ns();
        values["name"] = name();

        std::ifstream istr(ifn.c_str());
        if (!istr)
            throw spi::RuntimeError("Could not open '%s'", ifn.c_str());
        generateFromTemplate(istr, ifn, values, ofn, outdir, writeBackup());
        fns.push_back(ofn);
    }

    // translate the functions.bas file
    {
        std::string fn = spi_util::StringFormat("%sFunctions.bas", name().c_str());
        std::string ifn = spi_util::path::join(indir.c_str(), "functions.bas", (char*)0);
        std::string ofn = spi_util::path::join(outdir.c_str(), fn.c_str(), (char*)0);

        std::map<std::string, std::string> values;
        values["ns"] = ns();
        values["name"] = name();
        values["serviceName"] = m_options.upperCase ? StringUpper(longName()) : longName();
        // I fear that the name of the XLL is not a guarantee
        values["xll"] = spi_util::StringFormat(
            "xl_%s.xll", name().c_str());

        bool upperCase = m_options.upperCase;
        const std::string& sep = m_options.funcNameSep;

        values["startLoggingFunction"] = xlfunc(ns(), upperCase, sep, m_options.startLogging);
        values["stopLoggingFunction"] = xlfunc(ns(), upperCase, sep, m_options.stopLogging);
        values["setErrorPopups"] = xlfunc(ns(), upperCase, sep, m_options.setErrorPopups);
        values["startTimingFunction"] = xlfunc(ns(), upperCase, sep, m_options.startTiming);
        values["stopTimingFunction"] = xlfunc(ns(), upperCase, sep, m_options.stopTiming);
        values["clearTimingsFunction"] = xlfunc(ns(), upperCase, sep, m_options.clearTimings);

        if (m_options.noObjectFuncs)
        {
            std::string nns;
            values["object_get"] = xlfunc(nns, upperCase, sep, m_options.objectGet);
            values["object_free"] = xlfunc(nns, upperCase, sep, m_options.objectFree);
            values["object_count"] = xlfunc(nns, upperCase, sep, m_options.objectCount);
            values["object_to_string"] = xlfunc(nns, upperCase, sep, m_options.objectToString);
        }
        else
        {
            values["object_get"] = xlfunc(ns(), upperCase, sep, m_options.objectGet);
            values["object_free"] = xlfunc(ns(), upperCase, sep, m_options.objectFree);
            values["object_count"] = xlfunc(ns(), upperCase, sep, m_options.objectCount);
            values["object_to_string"] = xlfunc(ns(), upperCase, sep, m_options.objectToString);
        }

        values["pdf"] = name();

        std::ifstream istr(ifn.c_str());
        if (!istr)
            throw spi::RuntimeError("Could not open '%s'", ifn.c_str());
        generateFromTemplate(istr, ifn, values, ofn, outdir, writeBackup());
        fns.push_back(ofn);
    }

    // translate the ObjectViewer.bas file
    {
        std::string fn = spi_util::StringFormat("%sObjectViewer.frm", name().c_str());
        std::string ifn = spi_util::path::join(indir.c_str(), "ObjectViewer.frm", (char*)0);
        std::string ofn = spi_util::path::join(outdir.c_str(), fn.c_str(), (char*)0);

        std::map<std::string, std::string> values;

        values["ns"] = ns();
        values["name"] = name();

        std::ifstream istr(ifn.c_str());
        if (!istr)
            throw spi::RuntimeError("Could not open '%s'", ifn.c_str());
        generateFromTemplate(istr, ifn, values, ofn, outdir, writeBackup());
        fns.push_back(ofn);
    }

    // copy the ObjectViewer.frx file
    {
        std::string fn = spi_util::StringFormat("%sObjectViewer.frx", name().c_str());
        std::string ifn = spi_util::path::join(indir.c_str(), "ObjectViewer.frx", (char*)0);
        std::string ofn = spi_util::path::join(outdir.c_str(), fn.c_str(), (char*)0);

        bool copied = copyNewFile(ifn, ofn);
        if (copied)
            std::cout << ofn << std::endl;

        fns.push_back(ofn);
    }

    return fns;
}

std::string ExcelService::writeVbaFile(const std::string& dirname) const
{
    std::string basename = spi::StringFormat(
        "%s_load_addins.bas", m_service->name.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename), writeBackup());

    ostr << "Attribute VB_Name = \"" << m_service->name << "_load_addins\"\n"
        << "\n"
        << "Public Sub Auto_Open()\n"
        << "\n"
        << "    " << m_service->ns << "Functions." << m_service->ns << "LoadXLL\n"
        << "    " << m_service->ns << "Utils.SetManualCalculation\n"
        << "    " << m_service->ns << "Functions." << m_service->ns << "AddMenu\n"
        << "End Sub\n"
        << "\n"
        << "Public Sub Auto_Close()\n"
        << "    " << m_service->ns << "Functions." << m_service->ns << "RemoveMenu\n"
        << "    " << m_service->ns << "Functions." << m_service->ns << "UnloadXLL\n"
        << "End Sub\n"
        << "\n";

    return filename;
}


const std::string& ExcelService::name() const
{
    return m_service->name;
}

const std::string& ExcelService::longName() const
{
    return m_service->longName;
}

const std::string& ExcelService::ns() const
{
    return m_service->ns;
}

const std::string& ExcelService::import() const
{
    return m_import;
}

const spdoc::ServiceConstSP& ExcelService::service() const
{
    return m_service;
}

bool ExcelService::noGeneratedCodeNotice() const
{
    return m_options.noGeneratedCodeNotice;
}

bool ExcelService::nameAtEnd() const
{
    return m_options.nameAtEnd;
}

bool ExcelService::nameMandatory() const
{
    return m_nameMandatory;
}

bool ExcelService::xlfuncNamesInUpperCase() const
{
    return m_options.upperCase;
}

const char* ExcelService::funcNameSep() const
{
    return m_options.funcNameSep.c_str();
}

const std::string& ExcelService::license() const
{
    return m_options.license;
}

bool ExcelService::writeBackup() const
{
    return m_options.writeBackup;
}

/*
***************************************************************************
** Implementation of ExcelModule
***************************************************************************
*/
ExcelModuleConstSP ExcelModule::Make(
    const ExcelServiceConstSP&  service,
    const spdoc::ModuleConstSP& module)
{
    return new ExcelModule(service, module);
}

ExcelModule::ExcelModule(
    const ExcelServiceConstSP&  service,
    const spdoc::ModuleConstSP& module)
    :
    service(service),
    module(module)
{}

std::string ExcelModule::writeHeaderFile(const std::string& dirname) const
{
    std::string basename = spi::StringFormat(
        "xl_%s_%s.hpp", service->ns().c_str(), module->name.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename),service->writeBackup());
    writeLicense(ostr, service->license());
    startHeaderFile(ostr, filename);

    ostr << "\n"
         << "#include <xll_" << service->name() << "_decl_spec.h>\n"
         << "#include <spi/Namespace.hpp>\n"
         << "#include <string>\n";

    ostr << "\n"
         << "typedef struct xloper XLOPER;\n"
         << "\n"
         << "SPI_BEGIN_NAMESPACE\n"
         << "class ExcelService;\n"
         << "SPI_END_NAMESPACE\n";

    ostr << "\n"
         << "void xlfuncs_register_" << service->ns() << "_" << module->name
         << "(const std::string& xllName, spi::ExcelService* svc);\n";

    ostr << "\n"
         << "extern \"C\"\n"
         << "{\n";

    size_t nbConstructs = module->constructs.size();
    for (size_t i = 0; i < nbConstructs; ++i)
    {
        spdoc::ConstructConstSP construct = module->constructs[i];
        const std::string& constructType = construct->getType();

        if (constructType == "FUNCTION")
        {
            CONSTRUCT_CAST(Function, func, construct);
            declareFunction(ostr, func);
        }
        else if (constructType == "CLASS")
        {
            CONSTRUCT_CAST(Class, cls, construct);
            declareClass(ostr, cls);
        }
        else if (constructType == "ENUM")
        {
            // ignore ENUM - we have Help.Enum instead
        }
        else if (constructType == "SIMPLE_TYPE")
        {
            // of no relevance
        }
        else
        {
            std::cout << "ignoring " << constructType << std::endl;
        }
    }

    ostr << "\n"
         << "} /* end of extern \"C\" */\n";

    endHeaderFile(ostr, filename);
    ostr.close();
    return filename;
}

std::string ExcelModule::writeSourceFile(const std::string& dirname) const
{
    std::string basename = spi::StringFormat(
        "xl_%s_%s.cpp", service->ns().c_str(), module->name.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename), service->writeBackup());
    writeLicense(ostr, service->license());
    startSourceFile(ostr, filename);
    if (!service->noGeneratedCodeNotice())
        writeGeneratedCodeNotice(ostr, filename);

    ostr << "\n"
         << "#include \"xl_" << service->ns() << "_" << module->name << ".hpp\"\n"
         << "#include \"xll_" << service->name() << ".hpp\"\n"
         << "\n"
         << "#include \"" << service->ns() << "_" << module->name << ".hpp\"\n"
         << "\n"
         << "#include <spi/excel/xlInput.hpp>\n"
         << "#include <spi/excel/xlOutput.hpp>\n"
         << "#include <spi/excel/xlService.hpp>\n"
         << "#include <spi/excel/xlValue.hpp>\n"
         << "#include <spi/excel/xlFuncWizard.hpp>\n";

    size_t nbConstructs = module->constructs.size();

    ostr << "\n"
         << "void xlfuncs_register_" << service->ns() << "_" << module->name
         << "(const std::string& xllName, spi::ExcelService* svc)\n"
         << "{\n"
         << "    std::vector<std::string> args;\n"
         << "    std::vector<std::string> help;\n";

    for (size_t i = 0; i < nbConstructs; ++i)
    {
        spdoc::ConstructConstSP construct     = module->constructs[i];
        const std::string&      constructType = construct->getType();

        if (constructType == "FUNCTION")
        {
            CONSTRUCT_CAST(Function, func, construct);
            registerFunction(ostr, func);
        }
        else if (constructType == "CLASS")
        {
            CONSTRUCT_CAST(Class, cls, construct);
            registerClass(ostr, cls);
        }
    }

    ostr << "}\n";

    for (size_t i = 0; i < nbConstructs; ++i)
    {
        spdoc::ConstructConstSP construct     = module->constructs[i];
        const std::string&      constructType = construct->getType();

        if (constructType == "FUNCTION")
        {
            CONSTRUCT_CAST(Function, func, construct);
            implementFunction(ostr, func);
        }
        else if (constructType == "CLASS")
        {
            CONSTRUCT_CAST(Class, cls, construct);
            implementClass(ostr, cls);
        }
    }

    endSourceFile(ostr, filename);
    ostr.close();
    return filename;
}

/*
***************************************************************************
** Anonymous helper functions
***************************************************************************
*/
namespace {

int funcBaseNamePosition(const spdoc::Function* func)
{
    size_t numInputs = func->inputs.size();
    for (size_t i = 0; i < numInputs; ++i)
    {
        if (func->inputs[i]->name == "base_name")
            return (int)(i+1);
    }
    return 0;
}

bool funcHasBaseName(const spdoc::Function* func)
{
    int pos = funcBaseNamePosition(func);
    return pos > 0;
}

bool funcIsSlow(const spdoc::Function* func)
{
    for (size_t i = 0; i < func->excelOptions.size(); ++i)
    {
        if (func->excelOptions[i] == "slow")
            return true;
    }

    return false;
}

void writeFunctionStart(
    GeneratedOutput& ostr,
    const ExcelServiceConstSP& service,
    const std::string& funcName,
    bool isSlow)
{
    ostr << "{\n"
         << "    static spi::FunctionCaller* func = 0;\n"
         << "    XLOPER* xl_my_output = 0;\n"
         << "    spi::ExcelTimer xl_timer(" << service->name()
         << "_excel_service, \"" << funcName << "\");\n";

    // excel timer should be before any function wizard checks
    // this is because function wizard tests are expensive
    if (isSlow)
    {
        ostr << "    if (inExcelFunctionWizard())\n"
             << "        return spi::xloperInFunctionWizard();\n";
    }

    ostr << "    try\n"
         << "    {\n"
         << "        if (!func)\n"
         << "            func = get_function_caller(xl_timer.Name());\n"
         << "\n";
}

void writePreCallChecks(
    GeneratedOutput& ostr,
    int objectCount,
    bool mandatoryBaseName,
    bool isSlow)
{
    std::string spaces(8, ' ');
    if (objectCount == 1 && mandatoryBaseName)
    {
        ostr << spaces << "{\n"
             << spaces << "    std::string baseName = x.baseName.getString(true);\n"
             << spaces << "    if (baseName.length() == 0)\n"
             << spaces << "        throw spi::RuntimeError(\"baseName is mandatory\");\n"
             << spaces << "}\n\n";
    }
    else if (objectCount > 1 && mandatoryBaseName)
    {
        ostr << spaces << "{\n"
             << spaces << "    std::vector<std::string> baseNames = x.baseName.getStringVector(true);\n"
             << spaces << "    if (baseNames.size() != " << objectCount << ")\n"
             << spaces << "        throw spi::RuntimeError(\"Need " << objectCount << "baseNames\");\n"
             << spaces << "    for (size_t i = 0; i < " << objectCount << "; ++i)\n"
             << spaces << "    {\n"
             << spaces << "        if (baseNames[i].length() == 0)\n"
             << spaces << "            throw spi::RuntimeError(\"baseName is mandatory\");\n"
             << spaces << "    }\n"
             << spaces << "}\n\n";
    }

    if (objectCount > 0 && !isSlow)
    {
        // we may have to make a function wizard check when constructing objects
        ostr << spaces << "if (x.noCallInWizard && inExcelFunctionWizard())\n"
             << spaces << "{\n"
             << spaces << "    xl_timer.SetNotCalled();\n"
             << spaces << "    return spi::xloperInFunctionWizard();\n"
             << spaces << "}\n\n";
    }
}

void writeExceptionHandlingAndReturn(
    GeneratedOutput&           ostr,
    const ExcelServiceConstSP& service)
{
    ostr << "    }\n"
         << "    catch (spi::ExcelInputError&)\n"
         << "    {\n"
         << "        xl_timer.SetNotCalled();\n"
         << "        return spi::xloperInputError();\n"
         << "    }\n"
         << "    catch (std::exception &e)\n"
         << "    {\n"
         << "        xl_timer.SetFailure();\n"
         << "        return " << service->name() << "_excel_service->ErrorHandler(e.what());\n"
         << "    }\n"
         << "    catch (...)\n" // last resort but shouldn't happen since functor calls catch all
         << "    {\n"
         << "        xl_timer.SetFailure();\n"
         << "        return " << service->name() 
         << "_excel_service->ErrorHandler(\"Unknown exception!\");\n"
         << "    }\n"
         << "\n"
         << "    return spi::xloperOutput(xl_my_output);\n"
         << "}\n";
}
} // end of anonymous namespace

/*
***************************************************************************
** Writer methods for functions
***************************************************************************
*/
void ExcelModule::declareFunction(
    GeneratedOutput& ostr,
    const spdoc::Function* func) const
{
    bool nameAtEnd = service->nameAtEnd();
    bool hasBaseName = funcHasBaseName(func);

    ostr << "\n"
         << service->import() << "\n"
         << "XLOPER* xl_" << service->ns() << "_" << makeNamespaceSep(module->ns, "_")
         << func->name << "(";

    const char* sep = "\n";

    if (func->returnsObject() && !hasBaseName && !nameAtEnd)
    {
        ostr << sep << "    XLOPER* base_name";
        sep = ",\n";
    }

    size_t nbInputs = func->inputs.size();
    for (size_t i = 0; i < nbInputs; ++i)
    {
        ostr << sep << "    XLOPER* " << func->inputs[i]->name;
        sep = ",\n";
    }

    if (func->returnsObject() && !hasBaseName && nameAtEnd)
    {
        ostr << sep << "    XLOPER* base_name";
        sep = ",\n";
    }

    ostr << ");\n";
}

void ExcelModule::implementFunction(
    GeneratedOutput& ostr,
    const spdoc::Function* func) const
{
    bool nameAtEnd = service->nameAtEnd();
    bool hasBaseName = funcHasBaseName(func);

    ostr << "\n"
         << "XLOPER* xl_" << service->ns() << "_" << makeNamespaceSep(module->ns, "_")
         << func->name << "(";

    size_t nbInputs = func->inputs.size();
    const char* sep = "\n";

    if (func->returnsObject() && !hasBaseName && !nameAtEnd)
    {
        ostr << sep << "    XLOPER* xl_base_name";
        sep = ",\n";
    }

    for (size_t i = 0; i < nbInputs; ++i)
    {
        ostr << sep << "    XLOPER* xl_" << func->inputs[i]->name;
        sep = ",\n";
    }

    if (func->returnsObject() && !hasBaseName && nameAtEnd)
    {
        ostr << sep << "    XLOPER* xl_base_name";
        sep = ",\n";
    }

    ostr << ")\n";

    int objectCount = func->objectCount();
    bool isSlow = funcIsSlow(func);
    writeFunctionStart(ostr, service,
        makeNamespaceSep(module->ns, ".") + func->name,
        isSlow);

    ostr << "        const spi::XLInputValues& x = spi::xlGetInputValues";

    std::vector<std::string> callerArgs;
    callerArgs.push_back("func");
    if (objectCount > 0)
    {
        int baseNamePos;
        if (hasBaseName)
            baseNamePos = funcBaseNamePosition(func);
        else if (nameAtEnd)
            baseNamePos = -1;
        else
            baseNamePos = spi_util::IntegerCast<int>(nbInputs) + 1;

        SPI_POST_CONDITION(baseNamePos != 0);
        callerArgs.push_back(spi_util::StringFormat("%d", baseNamePos));
    }
    else
    {
        callerArgs.push_back("0");
    }
    for (size_t i = 0; i < nbInputs; ++i)
        callerArgs.push_back("xl_" + func->inputs[i]->name);

    if (func->returnsObject() && !hasBaseName)
        callerArgs.push_back("xl_base_name");

    writeArgsCall(ostr, callerArgs, 58, 12);
    ostr << ";\n\n";

    writePreCallChecks(ostr, objectCount, service->nameMandatory(), funcIsSlow(func));

    ostr << "        spi::Value output = spi::CallInContext(func, x.iv, get_input_context());\n";

    if (func->outputs.size() > 0)
    {
        SPI_PRE_CONDITION(!func->returnType);
        ostr << "        xl_my_output = spi::xloperMakeFromValue(output"
             << ", false, " << func->outputs.size();
        if (func->returnsObject())
        {
            ostr << ", x.baseName";
            if (service->nameMandatory())
                ostr << ", true";
        }
        ostr << ");\n";
    }
    else if (!func->returnType)
    {
        ostr << "        xl_my_output = spi::xloperFromBool(true);\n";
    }
    else if (func->returnsObject())
    {
        ostr << "        xl_my_output = spi::xloperMakeFromValue(output"
             << ", false, 1, x.baseName";
        if (service->nameMandatory())
            ostr << ", true";
        ostr << ");\n";
    }
    else
    {
        ostr << "        xl_my_output = spi::xloperMakeFromValue(output);\n";
    }
    writeExceptionHandlingAndReturn(ostr, service);
}

void ExcelModule::registerFunction(
    GeneratedOutput& ostr,
    const spdoc::Function* func) const
{
    bool nameAtEnd = service->nameAtEnd();
    bool hasBaseName = funcHasBaseName(func);
    const char* funcNameSep = service->funcNameSep();

    ostr << "\n"
         << "    /* " << makeNamespaceSep(module->ns, funcNameSep)
         << func->name << " */\n"
         << "    args.clear();\n"
         << "    help.clear();\n";

    bool isVolatile = false;
    bool isHidden = false;

    for (size_t i = 0; i < func->excelOptions.size(); ++i)
    {
        const std::string& excelOption = func->excelOptions[i];
        if (excelOption == "volatile")
            isVolatile = true;
        else if (excelOption == "hidden")
            isHidden = true;
    }

    if (func->returnsObject() && !hasBaseName && !nameAtEnd)
    {
        ostr << "    args.push_back(\"baseName\");\n"
             << "    help.push_back(\"Base name for object handle\");\n";
    }

    for (size_t i = 0; i < func->inputs.size(); ++i)
    {
        const spdoc::AttributeConstSP& arg = func->inputs[i];
        std::string name = arg->name;
        std::string argHelp = GetFirstParagraph(arg->description);
        switch(arg->arrayDim)
        {
        case 2: name += "[,]"; break;
        case 1: name += "[]"; break;
        default:
            if (arg->isOptional)
                name += "?";
            break;
        }
        ostr << "    args.push_back(\"" << name << "\");\n"
             << "    help.push_back(\"" << spi::StringEscape(argHelp.c_str())
             << "\");\n";
    }

    if (func->returnsObject() && !hasBaseName && nameAtEnd)
    {
        ostr << "    args.push_back(\"baseName\");\n"
             << "    help.push_back(\"Base name for object handle\");\n";
    }

    std::string funcHelp = GetFirstParagraph(func->description);
    ostr << "    svc->RegisterFunction(xllName, \"xl_" << service->ns() << "_"
         << makeNamespaceSep(module->ns, "_") << func->name << "\", \""
         << service->ns() << funcNameSep
         << makeNamespaceSep(module->ns, funcNameSep) << func->name
         << "\", \"" << service->ns() << "\", args,\n"
         << "        \"" << spi::StringEscape(funcHelp.c_str()) << "\",\n"
         << "        help";

    if (isHidden || isVolatile)
    {
        // these are both optional parameters to RegisterFunction
        // if either is defined then we may as well define both
        ostr << "," << (isHidden ? "true" : "false") << ","
             << (isVolatile ? "true" : "false") << ");\n";
    }
    else
    {
        ostr << ");\n";
    }
}

/*
***************************************************************************
** Writer methods for classes
***************************************************************************
*/
void ExcelModule::declareClass(
    GeneratedOutput& ostr,
    const spdoc::Class* cls) const
{
    bool nameAtEnd = service->nameAtEnd();
    if (!cls->noMake)
    {
        const char* sep = "";
        const char* commaNewLine = ",\n";

        bool noBaseName = cls->asValue;

        ostr << "\n"
             << service->import() << "\n"
             << "XLOPER* xl_" << service->ns() << "_"
             << makeNamespaceSep(module->ns, "_") << cls->name << "(\n";

        if (!nameAtEnd && !noBaseName)
        {
             ostr << "    XLOPER* base_name";
             sep = commaNewLine;
        }

        size_t nbAttributes = cls->attributes.size();
        for (size_t i = 0; i < nbAttributes; ++i)
        {
            ostr << sep
                 << "    XLOPER* " << cls->attributes[i]->name;
            sep = commaNewLine;
        }

        if (nameAtEnd && !noBaseName)
        {
            ostr << sep
                 << "    XLOPER* base_name";
        }
        ostr << ");\n";
    }

    for (size_t i = 0; i < cls->methods.size(); ++i)
    {
        const spdoc::ClassMethodConstSP& method = cls->methods[i];
        if (method->isImplementation)
            continue;

        ostr << "\n"
             << service->import() << "\n"
             << "XLOPER* xl_" << service->ns() << "_"
             << makeNamespaceSep(module->ns, "_") << cls->name
             << "_" << method->function->name << "(";

        const char* sep = "\n";

        bool hasBaseName = funcHasBaseName(method->function.get());
        if (method->function->returnsObject() && !hasBaseName && !nameAtEnd)
        {
            ostr << sep << "    XLOPER* base_name";
            sep = ",\n";
        }

        if (!method->isStatic)
        {
            ostr << sep << "    XLOPER* self";
            sep = ",\n";
        }

        for (size_t i = 0; i < method->function->inputs.size(); ++i)
        {
            ostr << sep << "    XLOPER* " << method->function->inputs[i]->name;
            sep = ",\n";
        }

        if (method->function->returnsObject() && !hasBaseName && nameAtEnd)
        {
            ostr << sep << "    XLOPER* base_name";
            sep = ",\n";
        }
        ostr << ");\n";
    }
}

void ExcelModule::implementClass(
    GeneratedOutput& ostr,
    const spdoc::Class* cls) const
{
    bool nameAtEnd = service->nameAtEnd();
    if (!cls->noMake)
    {
        const char* sep = "";
        bool noBaseName = cls->asValue;

        ostr << "\n"
             << "XLOPER* xl_" << service->ns() << "_"
             << makeNamespaceSep(module->ns, "_") << cls->name << "(\n";

        if (!nameAtEnd && !noBaseName)
        {
            sep = ",\n";
            ostr << "    XLOPER* xl_base_name";
        }

        size_t nbAttributes = cls->attributes.size();
        for (size_t i = 0; i < nbAttributes; ++i)
        {
            ostr << sep
                 << "    XLOPER* xl_" << cls->attributes[i]->name;
            sep = ",\n";
        }
        if (nameAtEnd && !noBaseName)
        {
            ostr << sep
                 << "    XLOPER* xl_base_name";
        }

        ostr << ")\n";

        bool isSlow = false; // assume a simple constructor is fast enough
        writeFunctionStart(ostr, service,
            makeNamespaceSep(module->ns, ".") + cls->name,
            isSlow);

        ostr << "        const spi::XLInputValues& x = spi::xlGetInputValues";

        std::vector<std::string> callerArgs;
        callerArgs.push_back("func");
        if (noBaseName)
        {
            callerArgs.push_back("0");
        }
        else if (nameAtEnd)
        {
            callerArgs.push_back("-1");
        }
        else
        {
            callerArgs.push_back(spi_util::StringFormat("%d", (int)nbAttributes + 1));
        }

        for (size_t i = 0; i < nbAttributes; ++i)
            callerArgs.push_back("xl_" + cls->attributes[i]->name);

        if (!noBaseName)
            callerArgs.push_back("xl_base_name");

        writeArgsCall(ostr, callerArgs, 58, 12);
        ostr << ";\n\n";

        writePreCallChecks(ostr, noBaseName ? 0 : 1, service->nameMandatory(), isSlow);

        ostr << "        spi::Value output = spi::CallInContext(func, x.iv, get_input_context());\n";
        ostr << "        xl_my_output = spi::xloperMakeFromValue(output"
            << ", false, 1";

        if (!noBaseName)
        {
            ostr << ", x.baseName";
            if (service->nameMandatory())
                ostr << ", true";
        }

        ostr << ");\n";

        writeExceptionHandlingAndReturn(ostr, service);
    }


    for (size_t i = 0; i < cls->methods.size(); ++i)
    {
        const spdoc::ClassMethodConstSP& method = cls->methods[i];
        if (method->isImplementation)
            continue;

        ostr << "\n"
             << "XLOPER* xl_" << service->ns() << "_"
             << makeNamespaceSep(module->ns, "_") << cls->name
             << "_" << method->function->name << "(";

        const char* sep = "\n";
        bool hasBaseName = funcHasBaseName(method->function.get());
        if (method->function->returnsObject() && !hasBaseName && !nameAtEnd)
        {
            ostr << sep << "    XLOPER* xl_base_name";
            sep = ",\n";
        }

        if (!method->isStatic)
        {
            ostr << sep << "    XLOPER* xl_self";
            sep = ",\n";
        }

        for (size_t i = 0; i < method->function->inputs.size(); ++i)
        {
            ostr << sep << "    XLOPER* xl_" << method->function->inputs[i]->name;
            sep = ",\n";
        }
        if (method->function->returnsObject() && !hasBaseName && nameAtEnd)
        {
            ostr << sep << "    XLOPER* xl_base_name";
            sep = ",\n";
        }
        ostr << ")\n";

        int objectCount = method->function->objectCount();
        bool isSlow = funcIsSlow(method->function.get());
        writeFunctionStart(ostr, service,
            makeNamespaceSep(module->ns, ".") + cls->name
            + "." + method->function->name,
            isSlow);

        ostr << "        const spi::XLInputValues& x = spi::xlGetInputValues";

        std::vector<std::string> callerArgs;
        callerArgs.push_back("func");

        if (objectCount > 0)
        {
            int baseNamePos = 0;
            if (hasBaseName)
                baseNamePos = funcBaseNamePosition(method->function.get());
            else if (nameAtEnd)
                baseNamePos = -1;
            else
            {
                baseNamePos = spi_util::IntegerCast<int>(method->function->inputs.size()) + 1;
                if (!method->isStatic)
                    baseNamePos += 1;
            }

            SPI_POST_CONDITION(baseNamePos != 0);
            callerArgs.push_back(spi_util::StringFormat("%d", baseNamePos));
        }
        else
        {
            callerArgs.push_back("0");
        }

        if (!method->isStatic)
            callerArgs.push_back("xl_self");

        for (size_t i = 0; i < method->function->inputs.size(); ++i)
            callerArgs.push_back("xl_" + method->function->inputs[i]->name);

        if (method->function->returnsObject() && !hasBaseName)
            callerArgs.push_back("xl_base_name");

        writeArgsCall(ostr, callerArgs, 58, 12);
        ostr << ";\n\n";

        writePreCallChecks(ostr, objectCount, service->nameMandatory(), isSlow);

        ostr << "        spi::Value output = spi::CallInContext(func, x.iv, get_input_context());\n";

        if (!method->function->returnType)
        {
            ostr << "        xl_my_output = spi::xloperFromBool(true);\n";
        }
        else if (method->function->returnsObject())
        {
            ostr << "        xl_my_output = spi::xloperMakeFromValue(output"
                 << ", false, 1, x.baseName";
            if (service->nameMandatory())
                ostr << ", true";
            ostr << ");\n";
        }
        else
        {
            ostr << "        xl_my_output = spi::xloperMakeFromValue(output);\n";
        }

        writeExceptionHandlingAndReturn(ostr, service);
    }
}

void ExcelModule::registerClass(
    GeneratedOutput& ostr,
    const spdoc::Class* cls) const
{
    bool nameAtEnd = service->nameAtEnd();
    const char* funcNameSep = service->funcNameSep();
    const std::string& xlFuncPrefix = cls->name;

    if (!cls->noMake)
    {
        ostr << "\n"
             << "    /* " << makeNamespaceSep(module->ns, funcNameSep)
             << xlFuncPrefix << " */\n"
             << "    args.clear();\n"
             << "    help.clear();\n";

        bool noBaseName = cls->asValue;

        if (!nameAtEnd && !noBaseName)
        {
             ostr << "    args.push_back(\"baseName\");\n"
                  << "    help.push_back(\"Base name for object handle.\");\n";
        }

        for (size_t i = 0; i < cls->attributes.size(); ++i)
        {
            const spdoc::ClassAttributeConstSP& arg = cls->attributes[i];
            std::string name = arg->name;
            std::string argHelp = GetFirstParagraph(arg->description);
            switch(arg->arrayDim)
            {
            case 2: name += "[,]"; break;
            case 1: name += "[]"; break;
            default:
                if (arg->isOptional)
                    name += "?";
                break;
            }
            ostr << "    args.push_back(\"" << name << "\");\n"
                 << "    help.push_back(\""
                 << spi::StringEscape(argHelp.c_str()) << "\");\n";
        }
        if (nameAtEnd && !noBaseName)
        {
             ostr << "    args.push_back(\"baseName\");\n"
                  << "    help.push_back(\"Base name for object handle.\");\n";
        }

        bool isHidden = !cls->constructor.empty();

        std::string classHelp = GetFirstParagraph(cls->description);
        ostr << "    svc->RegisterFunction(xllName, \"xl_" << service->ns()
            << "_" << makeNamespaceSep(module->ns, "_") << cls->name
            << "\", \"" << service->ns() << funcNameSep
            << makeNamespaceSep(module->ns, funcNameSep)
            << xlFuncPrefix << "\", \"" << service->ns() << "\", args,\n"
            << "        \"" << classHelp << "\",\n"
            << "        help";

        if (isHidden)
        {
            ostr << "," << (isHidden ? "true" : "false");
        }
        ostr << ");\n";
    }

    for (size_t i = 0; i < cls->methods.size(); ++i)
    {
        const spdoc::ClassMethodConstSP& method = cls->methods[i];
        if (method->isImplementation)
            continue;

        ostr << "\n"
            << "    /* " << makeNamespaceSep(module->ns, funcNameSep)
            << cls->name << funcNameSep
            << method->function->name << " */\n"
            << "    args.clear();\n"
            << "    help.clear();\n";

        bool isVolatile = false;
        bool isHidden = false;

        for (size_t i = 0; i < method->function->excelOptions.size(); ++i)
        {
            const std::string& excelOption = method->function->excelOptions[i];
            if (excelOption == "volatile")
                isVolatile = true;
            else if (excelOption == "hidden")
                isHidden = true;
        }

        bool hasBaseName = funcHasBaseName(method->function.get());
        if (method->function->returnsObject() && !hasBaseName && !nameAtEnd)
        {
            ostr << "    args.push_back(\"baseName\");\n"
                << "    help.push_back(\"Base name for object handle\");\n";
        }

        if (!method->isStatic)
        {
            ostr << "    args.push_back(\"handle\");\n"
                << "    help.push_back(\"" << cls->name
                << " object handle.\");\n";
        }

        for (size_t i = 0; i < method->function->inputs.size(); ++i)
        {
            const spdoc::AttributeConstSP& arg = method->function->inputs[i];
            std::string name = arg->name;
            std::string argHelp = GetFirstParagraph(arg->description);
            switch (arg->arrayDim)
            {
            case 2: name += "[,]"; break;
            case 1: name += "[]"; break;
            default:
                if (arg->isOptional)
                    name += "?";
                break;
            }
            ostr << "    args.push_back(\"" << name << "\");\n"
                << "    help.push_back(\""
                << spi::StringEscape(argHelp.c_str()) << "\");\n";
        }
        if (method->function->returnsObject() && !hasBaseName && nameAtEnd)
        {
            ostr << "    args.push_back(\"baseName\");\n"
                << "    help.push_back(\"Base name for object handle\");\n";
        }
        std::string mHelp = GetFirstParagraph(method->function->description);
        ostr << "    svc->RegisterFunction(xllName, \"xl_" << service->ns()
            << "_" << makeNamespaceSep(module->ns, "_") << cls->name << "_"
            << method->function->name << "\", \"" << service->ns()
            << funcNameSep
            << makeNamespaceSep(module->ns, funcNameSep) << xlFuncPrefix
            << funcNameSep << method->function->name << "\", \""
            << service->ns() << "\", args,\n"
            << "        \"" << mHelp << "\",\n"
            << "        help";

        if (isHidden || isVolatile)
        {
            // these are both optional parameters to RegisterFunction
            // if either is defined then we may as well define both
            ostr << "," << (isHidden ? "true" : "false") << ","
                << (isVolatile ? "true" : "false") << ");\n";
        }
        else
        {
            ostr << ");\n";
        }
    }

}

namespace
{
    void JSONMapUpdateString(
        std::string& value,
        const spi_util::JSONMapConstSP& jm,
        const std::string& name)
    {
        spi_util::JSONValue jv = jm->Item(name, true);
        if (jv.GetType() == spi_util::JSONValue::Null)
            return;
        value = jv.GetString();
    }

    void JSONMapUpdateBool(
        bool& value,
        const spi_util::JSONMapConstSP& jm,
        const std::string& name)
    {
        spi_util::JSONValue jv = jm->Item(name, true);
        if (jv.GetType() == spi_util::JSONValue::Null)
            return;
        value = jv.GetBool();
    }

}

void Options::update(const std::string& fn)
{
    std::ifstream fp(fn.c_str());
    if (!fp)
    {
        SPI_UTIL_THROW_RUNTIME_ERROR("Cannot open Options file " << fn);
    }

    spi_util::JSONValue jv = spi_util::JSONParseValue(fp, fn);

    fp.close();

    spi_util::JSONMapConstSP jm = jv.GetMap();

    JSONMapUpdateString(funcNameSep, jm, "funcNameSep");

    JSONMapUpdateBool(noGeneratedCodeNotice, jm, "noGeneratedCodeNotice");
    JSONMapUpdateBool(nameAtEnd, jm, "nameAtEnd");
    JSONMapUpdateBool(upperCase, jm, "upperCase");
    JSONMapUpdateBool(noObjectFuncs, jm, "noObjectFuncs");
    JSONMapUpdateBool(errIsNA, jm, "errIsNA");

    JSONMapUpdateString(helpFunc, jm, "helpFunc");
    JSONMapUpdateString(helpFuncList, jm, "helpFuncList");
    JSONMapUpdateString(helpEnum, jm, "helpEnum");
    JSONMapUpdateString(objectCoerce, jm, "objectCoerce");
    JSONMapUpdateString(startLogging, jm, "startLogging");
    JSONMapUpdateString(stopLogging, jm, "stopLogging");
    JSONMapUpdateString(startTiming, jm, "startTiming");
    JSONMapUpdateString(stopTiming, jm, "stopTiming");
    JSONMapUpdateString(clearTimings, jm, "clearTimings");
    JSONMapUpdateString(getTimings, jm, "getTimings");
    JSONMapUpdateString(setErrorPopups, jm, "setErrorPopups");
    JSONMapUpdateString(objectToString, jm, "objectToString");
    JSONMapUpdateString(objectFromString, jm, "objectFromString");
    JSONMapUpdateString(objectGet, jm, "objectGet");
    JSONMapUpdateString(objectPut, jm, "objectPut");
    JSONMapUpdateString(objectToFile, jm, "objectToFile");
    JSONMapUpdateString(objectFromFile, jm, "objectFromFile");
    JSONMapUpdateString(objectCount, jm, "objectCount");
    JSONMapUpdateString(objectFree, jm, "objectFree");
    JSONMapUpdateString(objectFreeAll, jm, "objectFreeAll");
    JSONMapUpdateString(objectList, jm, "objectList");
    JSONMapUpdateString(objectClassName, jm, "objectClassName");
    JSONMapUpdateString(objectSHA, jm, "objectSHA");

}
