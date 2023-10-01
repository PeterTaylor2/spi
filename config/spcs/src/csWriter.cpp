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
#include "csWriter.hpp"

#include <spi_util/FileUtil.hpp>
#include <spi_util/StringUtil.hpp>
#include <spi/spdoc_configTypes.hpp>

#include <spgtools/licenseTools.hpp>
#include <spgtools/generatedOutput.hpp>
#include <spgtools/namespaceManager.hpp>

#include <iostream>
#include <map>

using spi_util::StringReplace;
using spi_util::StringFormat;
using spi_util::StringStrip;
using spi_util::StringJoin;
using spi_util::StringUpper;

#define BEGIN_ANONYMOUS_NAMESPACE namespace {
#define END_ANONYMOUS_NAMESPACE }

#define CONSTRUCT_CAST(T, out, in) \
    spdoc::T const* out = dynamic_cast<spdoc::T const*>(in.get());\
    if (!out)\
        throw spi::RuntimeError("%s is of type %s and not %s", \
                                #in, in->get_class_name(), #T)

BEGIN_ANONYMOUS_NAMESPACE

/**
 * Adds escapes to plain text to enable it to be shown in tex.
 * Any genuine escapes would end up being doubled up - so only use this
 * for plain text without prior escapes.
 */
std::string xmlEscape(const std::string& plainText)
{
    std::ostringstream oss;

    const char* str = plainText.c_str();
    size_t len = plainText.length();
    for (size_t i = 0; i < len; ++i)
    {
        char c = str[i];
        switch (c)
        {
        case '&':
            oss << "&amp;";
            break;
        case '<':
            oss << "&lt;";
            break;
        case '>':
            oss << "&gt;";
            break;
        default:
            oss << c;
            break;
        }
    }

    return oss.str();
}

void xmlWriteDescription(
    GeneratedOutput& ostr,
    const char* tag,
    const std::string& tagValues,
    const std::string& spaces,
    const std::vector<std::string>& description)
{
    bool hasDescription = false;
    bool showEmptyLine = false;
    bool atStart = true;

    ostr << spaces << "/// <" << tag;
    if (!tagValues.empty())
    {
        ostr << " " << tagValues;
    }
    ostr << ">";
    for (size_t i = 0; i < description.size(); ++i)
    {
        if (StringStrip(description[i]).empty())
        {
            if (!atStart)
                showEmptyLine = true;
            continue;
        }
        hasDescription = true;
        atStart = false;
        if (showEmptyLine)
        {
            ostr << "\n" << spaces << "///";
            showEmptyLine = false;
        }
        ostr << "\n" << spaces << "/// " << xmlEscape(description[i]);
    }

    if (hasDescription)
    {
        ostr << "\n" << spaces << "/// </" << tag << ">\n";
    }
    else
    {
        ostr << "</" << tag << ">\n";
    }
}

void WriteDefaultValue(
    GeneratedOutput& ostr,
    const spdoc::AttributeConstSP& arg,
    const spdoc::ServiceConstSP& svc,
    const std::string& nsGlobal)
{
    SPI_PRE_CONDITION(arg->isOptional);
    SPI_PRE_CONDITION(!arg->isArray());
    spdoc::ConstantConstSP defaultValue = arg->defaultValue;
    spdoc::DataTypeConstSP dataType = arg->dataType;

    ostr << " = ";

    switch (dataType->publicType)
    {
    case spdoc::PublicType::BOOL:
        ostr << (defaultValue->getBool() ? "true" : "false");
        break;
    case spdoc::PublicType::CHAR:
        ostr << "'" << defaultValue->getChar() << "'";
        break;
    case spdoc::PublicType::INT:
        ostr << defaultValue->getInt();
        break;
    case spdoc::PublicType::DOUBLE:
        ostr << spi::StringFormat("%.15g", defaultValue->getDouble());
        break;
    case spdoc::PublicType::STRING:
        ostr << '"' << defaultValue->getString() << '"';
        break;
    case spdoc::PublicType::CLASS:
    case spdoc::PublicType::OBJECT:
    case spdoc::PublicType::MAP:
        ostr << "null";
        break;
    case spdoc::PublicType::DATE:
    case spdoc::PublicType::DATETIME:
        ostr << "default(System.DateTime)";
        break;
    case spdoc::PublicType::ENUM:
    {
        spdoc::EnumConstSP e = svc->getEnum(dataType->name);
        std::string str = defaultValue->getString();
        std::vector<spdoc::EnumerandConstSP> enumerands = e->enumerands;
        bool found = false;
        spdoc::EnumerandConstSP code;
        for (size_t i = 0; i < enumerands.size(); ++i)
        {
            spdoc::EnumerandConstSP enumerand = e->enumerands[i];
            for (size_t j = 0; j < enumerand->strings.size(); ++j)
            {
                if (enumerand->strings[j] == str)
                {
                    code = enumerand;
                    found = true;
                    break;
                }
            }
            if (found)
                break;
        }
        if (found)
        {
            ostr << nsGlobal << "." << dataType->nsService << "." << dataType->name << "." << code->code;
        }
        else
        {
            SPI_THROW_RUNTIME_ERROR("Could not find equivalent code to '" << str
                << "' for enumerated type name " << dataType->name);
        }
        break;
    }
    default:
        SPI_THROW_RUNTIME_ERROR("don't understand defaults for public type "
            << (int)dataType->publicType);
    }
}

END_ANONYMOUS_NAMESPACE

/*
***************************************************************************
** Implementation of CService
***************************************************************************
*/
CServiceConstSP CService::Make(
    const spdoc::ServiceConstSP& service,
    const std::string& nsGlobal,
    const std::string& dllName,
    const std::string& companyName,
    const std::vector<std::string>& exclusions,
    const Options& options)
{
    return new CService(service, nsGlobal, dllName, companyName, exclusions, options);
}

CService::CService(
    const spdoc::ServiceConstSP& service,
    const std::string& nsGlobal,
    const std::string& dllName,
    const std::string& companyName,
    const std::vector<std::string>& exclusions,
    const Options& options)
    :
    m_service(service),
    m_nsGlobal(nsGlobal),
    m_dllName(dllName),
    m_companyName(companyName),
    m_import(),
    m_csDllImport(),
    m_spiImport(),
    m_exclusions(),
    m_options(options)
{
    m_import = spi::StringFormat("%s_CSC_IMPORT", m_service->declSpec.c_str());
    std::ostringstream csDllImport;
    csDllImport << "[DllImport(\"" << m_service->name << "-c\""
        << ", CallingConvention = CallingConvention.Cdecl)]";
    
    m_csDllImport = csDllImport.str();

    std::ostringstream spiImport;
    spiImport << "[DllImport(\"spi-c\""
        << ", CallingConvention = CallingConvention.Cdecl)]";

    m_spiImport = spiImport.str();

    m_exclusions.insert(m_service->ns);
    m_exclusions.insert(exclusions.begin(), exclusions.end());
    m_exclusions.insert("fixed");
    m_exclusions.insert("object");
    m_exclusions.insert("params");
    m_exclusions.insert("in");
}

std::string CService::writeServiceFile(const std::string& dirname) const
{
    std::string basename = spi::StringFormat("cs_%s_service.cs", m_service->name.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename));
    writeLicense(ostr, license());
    startSourceFile(ostr, filename, false);
    if (!noGeneratedCodeNotice())
        writeGeneratedCodeNotice(ostr, filename);

    ostr << "using System;\n"
        << "using System.Runtime.InteropServices;\n"
        << "using SPI;\n";

    ostr << "\n"
        << "\n"
        << "namespace " << m_nsGlobal << " {\n";

    ostr << "\n"
        << "public partial class " << m_service->ns << " {\n";

    if (!m_service->sharedService)
    {
        ostr << "\n"
            << m_csDllImport << "\n"
            << "private static extern int init_" << m_service->ns << "();\n"
            << "\n"
            << "public static void start_service()\n"
            << "{\n"
            << "    if (init_" << m_service->ns << "() != 0)\n"
            << "    {\n"
            << "        throw spi.ErrorToException();\n"
            << "    }\n"
            << "}\n";

        ostr << "\n"
            << m_csDllImport << "\n"
            << "private static extern int " << m_service->ns << "_start_logging(\n"
            << "    string filename, string options);\n"
            << "\n"
            << "public static void start_logging(\n"
            << "    System.String filename,\n"
            << "    System.String options)\n"
            << "{\n"
            << "    if (" << m_service->ns << "_start_logging(filename, options) != 0)\n"
            << "    {\n"
            << "        throw spi.ErrorToException();\n"
            << "    }\n"
            << "}\n";

        ostr << "\n"
            << m_csDllImport << "\n"
            << "private static extern int " << m_service->ns << "_stop_logging();\n"
            << "\n"
            << "public static void stop_logging()\n"
            << "{\n"
            << "    if (" << m_service->ns << "_stop_logging() != 0)\n"
            << "    {\n"
            << "        throw spi.ErrorToException();\n"
            << "    }\n"
            << "}\n";
    }

    ostr << "\n"
        << m_spiImport << "\n"
        << "private static extern int spi_Instance_Vector_item(\n"
        << "    IntPtr v, int i, out IntPtr item);\n"
        << "\n"
        << m_spiImport << "\n"
        << "private static extern int spi_Instance_Vector_set_item(\n"
        << "    IntPtr v, int i, IntPtr item);\n"
        << "\n"
        << m_spiImport << "\n"
        << "private static extern int spi_Instance_Vector_size(\n"
        << "    IntPtr v, out int size);\n"
        << "\n"
        << m_spiImport << "\n"
        << "private static extern int spi_Instance_Matrix_item(\n"
        << "    IntPtr v, int r, int c, out IntPtr item);\n"
        << "\n"
        << m_spiImport << "\n"
        << "private static extern int spi_Instance_Matrix_set_item(\n"
        << "    IntPtr v, int r, int c, IntPtr item);\n"
        << "\n"
        << m_spiImport << "\n"
        << "private static extern int spi_Instance_Matrix_size(\n"
        << "    IntPtr v, out int nr, out int nc);\n";

    ostr << "\n"
        << m_spiImport << "\n"
        << "private static extern int spi_Enum_Vector_item(\n"
        << "    IntPtr v, int i, out int item);\n"
        << "\n"
        << m_spiImport << "\n"
        << "private static extern int spi_Enum_Vector_set_item(\n"
        << "    IntPtr v, int i, int item);\n"
        << "\n"
        << m_spiImport << "\n"
        << "private static extern int spi_Enum_Vector_size(\n"
        << "    IntPtr v, out int size);\n"
        << "\n"
        << m_spiImport << "\n"
        << "private static extern int spi_Enum_Matrix_item(\n"
        << "    IntPtr v, int r, int c, out int item);\n"
        << "\n"
        << m_spiImport << "\n"
        << "private static extern int spi_Enum_Matrix_set_item(\n"
        << "    IntPtr v, int r, int c, int item);\n"
        << "\n"
        << m_spiImport << "\n"
        << "private static extern int spi_Enum_Matrix_size(\n"
        << "    IntPtr v, out int nr, out int nc);\n";

    ostr << "\n"
        << "}\n";

    ostr << "\n"
        << "} // end namespace " << m_nsGlobal << "\n";

    endSourceFile(ostr, filename);
    ostr.close();
    return filename;
}

std::string CService::writeAssemblyInfo(const std::string& dirname) const
{
    std::string basename = StringFormat("cs_%s_AssemblyInfo.cs", m_service->name.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename));

    int year = spi::Date::Today().Year();

    ostr << "using System.Reflection;\n"
        << "using System.Runtime.CompilerServices;\n"
        << "using System.Runtime.InteropServices;\n"
        << "\n";

    ostr << "[assembly: AssemblyTitle(\"" << m_dllName << "\")]\n";
    ostr << "[assembly: AssemblyDescription(\"" << m_service->longName << "\")]\n";
    ostr << "[assembly: AssemblyConfiguration(\"\")]\n";
    ostr << "[assembly: AssemblyCompany(\"" << m_companyName << "\")]\n";
    ostr << "[assembly: AssemblyProduct(\"" << m_companyName << " C# Interface\")]\n";
    ostr << "[assembly: AssemblyCopyright(\"Copyright © " << m_companyName << " " << year << "\")]\n";
    ostr << "[assembly: AssemblyTrademark(\"\")]\n";
    ostr << "[assembly: AssemblyCulture(\"\")]\n";
    ostr << "[assembly: ComVisible(false)]\n";
    ostr << "[assembly: AssemblyVersion(\"" << m_service->version << "\")]\n";
    ostr << "[assembly: AssemblyFileVersion(\"" << m_service->version << "\")]\n";
    ostr << "\n";

    ostr.close();
    return filename;
}

const std::string& CService::name() const
{
    return m_service->name;
}

const std::string& CService::ns() const
{
    return m_service->ns;
}

const std::string & CService::nsGlobal() const
{
    return m_nsGlobal;
}

const std::string& CService::import() const
{
    return m_import;
}

const std::string& CService::csDllImport() const
{
    return m_csDllImport;
}

const std::string& CService::spiImport() const
{
    return m_spiImport;
}

const spdoc::ServiceConstSP& CService::service() const
{
    return m_service;
}

std::string CService::rename(const std::string& name) const
{
    if (m_exclusions.count(name) == 0)
        return name;

    //std::cout << "Renaming " << name << " as " << name << "_\n";
    return name + "_";
}

bool CService::noGeneratedCodeNotice() const
{
    return m_options.noGeneratedCodeNotice;
}

const std::string& CService::license() const
{
    return m_options.license;
}

/*
***************************************************************************
** Implementation of CModule
***************************************************************************
*/
CModuleConstSP CModule::Make(
    const CServiceConstSP& service,
    const spdoc::ModuleConstSP& module)
{
    return new CModule(service, module);
}

CModule::CModule(
    const CServiceConstSP& service,
    const spdoc::ModuleConstSP& module)
    :
    service(service),
    module(module)
{}

std::string CModule::writeModuleFile(const std::string & dirname) const
{
    std::string basename = spi::StringFormat(
        "%s_%s.cs", service->ns().c_str(), module->name.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename));
    writeLicense(ostr, service->license());
    startSourceFile(ostr, filename, false);
    if (!service->noGeneratedCodeNotice())
        writeGeneratedCodeNotice(ostr, filename);

    ostr << "using System;\n"
        << "using System.Runtime.InteropServices;\n"
        << "using SPI;\n";

    ostr << "\n"
        << "namespace " << service->nsGlobal() << " {\n"
        << "\n"
        << "public partial class " << service->ns() << " {\n";

    if (!module->ns.empty())
    {
        ostr << "\n"
            << "public partial class " << module->ns << " {\n";
    }

    size_t nbConstructs = module->constructs.size();

    for (size_t i = 0; i < nbConstructs; ++i)
    {
        spdoc::ConstructConstSP construct = module->constructs[i];
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
        else if (constructType == "ENUM")
        {
            CONSTRUCT_CAST(Enum, enumType, construct);
            implementEnum(ostr, enumType);
        }
    }

    if (!module->ns.empty())
    {
        ostr << "\n"
            << "}\n";
    }

    ostr << "\n"
        << "}\n"
        << "\n"
        << "} // end namespace " << service->nsGlobal() << "\n";

    endSourceFile(ostr, filename);
    ostr.close();
    return filename;

}

void CModule::implementFunction(
    GeneratedOutput& ostr,
    const spdoc::Function* func) const
{
    std::ostringstream oss;
    oss << service->ns() << "_" << makeNamespaceSep(module->ns, "_") << func->name;
    std::string cname = oss.str();

    /*
    ****************************************************************************
    * Declare the PINVOKE interface.
    ****************************************************************************
    */
    ostr << "\n"
        << service->csDllImport() << "\n"
        << "private static extern int " << cname << "(";

    declarePlatformInvokeArgs(ostr, func, false);

    // FIXME:
    //
    // 1. Handle void functions
    //    a. With no outputs no problem
    //    b. With one output return that in the C# interface
    //    c. With multiple outputs create a C# class containing the outputs
    //
    // 2. Convert inputs - but not always necessary
    //    a. Input arrays / matrices need to be converted first to spi.PointerHandle
    //    b. In the function call we need to extract the IntPtr from the PointerHandle
    //    c. Scalar built-in types need no conversion
    //
    // 3. Convert outputs - but not always necessary
    //    a. Functions which return IntPtr need to capture the IntPtr as PointerHandle
    //       This ensures that the IntPtr is deleted before we return from the function
    //    b. We then may need to convert the PointerHandle if the output is an array
    //
    // 4. We need to be able to convert IntPtr to PointerHandle - this requires us
    //    to have access to the IntPtr delete function.
    //
    //    a. For built-in array types we will need some public methods in spcs class library
    //    b. Unless we somehow incorporate the spcs.cs file within our project.
    //    c. Option (b) matches the behaviour of the C++/CLI .NET assembly.
    //    d. Hence in that case we can define the functions as internal instead of public.
    //
    // 5. So for each data type via CDataType class we will need the following:
    //    a. Code to declare internal variable (can be empty).
    //    b. Code to convert from either the raw input or the declared internal variable
    //       and provide the input needed by the external C-function call.
    //    c. Code to capture any output IntPtr types.
    //    d. Code to convert the output whether there was capturing or not.
    //       i. Might be converted spi.PointerHandle to array
    //      ii. Might be creating an object of multiple outputs
    //
    // Methods used for conversions must be re-usable for classes - i.e. constructors,
    // classes, attributes, properties.

    std::string csReturnType;
    std::string csOutputClass;
    if (!func->returnType)
    {
        csOutputClass = implementFunctionOutputClass(ostr, func, 0);
        csReturnType = csOutputClass.empty() ? "void" : csOutputClass;
    }
    else
    {
        csReturnType = CDataType(func->returnType, service).csType(func->returnArrayDim);
    }

    /*
    ****************************************************************************
    * Implement the C# function via PINVOKE.
    ****************************************************************************
    */
    implementFunctionSummary(ostr, func, 0);

    ostr << "public static " << csReturnType << " " << func->name << "(";

    implementFunctionBegin(ostr, func, 0);

    ostr << "    if (" << cname << "(";

    implementPlatformInvoke(ostr, func, 0, "");
    implementFunctionEnd(ostr, func, 0, csOutputClass);
}

void CModule::implementFunctionBegin(
    GeneratedOutput& ostr,
    const spdoc::Function* func,
    size_t indent) const
{
    std::string spaces(indent, ' ');
    std::string gap(4, ' ');

    const char* sep = "\n";
    size_t lastMandatory = 0;
    for (size_t i = 0; i < func->inputs.size(); ++i)
    {
        bool isArray = func->inputs[i]->isArray();
        bool isOptional = func->inputs[i]->isOptional;
        if (isArray || !isOptional)
            lastMandatory = i + 1;
    }

    for (size_t i = 0; i < func->inputs.size(); ++i)
    {
        const spdoc::AttributeConstSP& arg = func->inputs[i];
        ostr << sep << spaces << gap;
        sep = ",\n";
        ostr << CDataType(arg->dataType, service).csType(arg->arrayDim) << " "
            << service->rename(arg->name);

        if (i >= lastMandatory)
        {
            WriteDefaultValue(ostr, arg, service->service(), service->nsGlobal());
        }

    }
    ostr << ")\n"
        << spaces << "{\n";
}

void CModule::implementFunctionEnd(
    GeneratedOutput& ostr,
    const spdoc::Function* func,
    size_t indent,
    const std::string& outputClassName) const
{
    std::string spaces(indent, ' ');
    std::string gap(4, ' ');

    if (func->returnType)
    {
        SPI_PRE_CONDITION(func->outputs.size() == 0);

        std::string captureOutput = CDataType(func->returnType, service).c_to_csi(
            func->returnArrayDim, "out");

        if (!captureOutput.empty())
        {
            ostr << "\n"
                << spaces << gap << captureOutput << ";\n";
        }

        ostr << "\n"
            << spaces << gap << "return "
            << CDataType(func->returnType, service).csi_to_cs(func->returnArrayDim, "out")
            << ";\n";
    }
    else if (func->outputs.size() == 0)
    {
        ostr << spaces << gap << "return;\n";
    }
    else
    {
        std::vector<std::string> args;
        for (size_t i = 0; i < func->outputs.size(); ++i)
        {
            spdoc::AttributeConstSP out = func->outputs[i];
            CDataType cdt(out->dataType, service);

            std::string captureOutput = cdt.c_to_csi(out->arrayDim, service->rename(out->name));

            if (!captureOutput.empty())
            {
                ostr << "\n"
                    << spaces << gap << captureOutput << ";\n";
            }

            args.push_back(cdt.csi_to_cs(out->arrayDim, out->name));
        }

        ostr << "\n"
            << spaces << gap << "return new " << outputClassName << "("
            << StringJoin(", ", args) << ");\n";
    }
    ostr << spaces << "}\n";
}

std::string CModule::implementFunctionOutputClass(
    GeneratedOutput& ostr,
    const spdoc::Function* func,
    size_t indent) const
{
    if (func->outputs.size() == 0)
        return std::string();

    std::ostringstream oss;
    oss << func->name << "_outputs";
    std::string outputClassName = oss.str();

    std::string spaces(indent, ' ');

    const char* gap = "    ";
    ostr << "\n"
        << spaces << "public struct " << outputClassName << "\n"
        << spaces << "{\n"
        << spaces << gap << "public " << outputClassName << "(";

    const char* sep = "\n";
    for (size_t i = 0; i < func->outputs.size(); ++i)
    {
        spdoc::AttributeConstSP out = func->outputs[i];
        ostr << sep << spaces << gap << gap
            << CDataType(out->dataType, service).csType(out->arrayDim) << " in_" << out->name;
        sep = ",\n";
    }
    ostr << ")\n"
        << spaces << gap << "{\n";

    for (size_t i = 0; i < func->outputs.size(); ++i)
    {
        spdoc::AttributeConstSP out = func->outputs[i];
        ostr << spaces << gap << gap << service->rename(out->name) << " = in_" << out->name << ";\n";
    }
    ostr << spaces << gap << "}\n\n";

    for (size_t i = 0; i < func->outputs.size(); ++i)
    {
        spdoc::AttributeConstSP out = func->outputs[i];
        ostr << spaces << gap << "public " << CDataType(out->dataType, service).csType(out->arrayDim)
            << " " << service->rename(out->name) << ";\n";
    }

    ostr << spaces << "}\n";

    return outputClassName;
}

void CModule::implementPlatformInvoke(
    GeneratedOutput& ostr,
    const spdoc::Function* func,
    size_t indent,
    const char* sep) const
{
    // inputs
    std::string spaces(indent, ' ');
    std::string newline = "\n        " + spaces;

    for (size_t i = 0; i < func->inputs.size(); ++i)
    {
        const spdoc::AttributeConstSP& arg = func->inputs[i];
        std::string convertedArg = CDataType(arg->dataType, service).cs_to_c(
            arg->arrayDim, service->rename(arg->name));

        ostr << sep << newline << convertedArg;
        sep = ",";
    }

    // outputs
    if (func->returnType)
    {
        std::string outputArg = StringFormat("out %s c_out",
            CDataType(func->returnType, service).csiType(func->returnArrayDim).c_str());

        ostr << sep << newline << outputArg;
        sep = ",";
    }

    for (size_t i = 0; i < func->outputs.size(); ++i)
    {
        spdoc::AttributeConstSP out = func->outputs[i];
        std::string outputArg = StringFormat("out %s c_%s",
            CDataType(out->dataType, service).csiType(out->arrayDim).c_str(),
            out->name.c_str());

        ostr << sep << newline << outputArg;
        sep = ",";
    }

    // error handling
    ostr << ") != 0)\n"
        << spaces << "    {\n"
        << spaces << "        throw spi.ErrorToException();\n"
        << spaces << "    }\n";
}

void CModule::implementClass(
    GeneratedOutput& ostr,
    const spdoc::Class* cls) const
{
    std::ostringstream oss;
    oss << service->ns() << "_" << makeNamespaceSep(module->ns, "_") << cls->name;
    std::string cname = oss.str();
    bool hasBaseClass = !cls->baseClassName.empty();

    //Usage usage = service->usage(cls->dataType->name);

    if (!cls->noMake)
    {
        SPI_POST_CONDITION(!cls->isAbstract);

        ostr << "\n"
            << service->csDllImport() << "\n"
            << "private static extern IntPtr " << cname << "_new(";

        char* sep = "\n    ";
        char* sep2 = ",\n    ";

        for (size_t i = 0; i < cls->attributes.size(); ++i)
        {
            const spdoc::ClassAttributeConstSP& attr = cls->attributes[i];
            ostr << sep;
            sep = sep2;
            ostr << CDataType(attr->dataType, service).csiType(attr->arrayDim) << " "
                << service->rename(attr->name);
        }

        ostr << ");\n";
    }

    ostr << "\n"
        << service->csDllImport() << "\n"
        << "private static extern void " << cname << "_Vector_delete(IntPtr v);\n"
        << "\n"
        << service->csDllImport() << "\n"
        << "private static extern IntPtr " << cname << "_Vector_new(int N);\n";

    ostr << "\n"
        << service->csDllImport() << "\n"
        << "private static extern void " << cname << "_Matrix_delete(IntPtr m);\n"
        << "\n"
        << service->csDllImport() << "\n"
        << "private static extern IntPtr " << cname << "_Matrix_new(\n"
        << "    int nr, int nc);\n";

    ostr << "\n"
        << service->csDllImport() << "\n"
        << "private static extern int " << cname << "_coerce_from_object(\n"
        << "    IntPtr o, out IntPtr item);\n";

    ostr << "\n"
        << service->csDllImport() << "\n"
        << "private static extern IntPtr " << cname << "_from_string(\n"
        << "    string objectString);\n"
        << "\n"
        << service->csDllImport() << "\n"
        << "private static extern IntPtr " << cname << "_from_file(\n"
        << "    string filename);\n";

    std::vector<std::string> cfFuncNames;
    for (size_t i = 0; i < cls->coerceFrom.size(); ++i)
    {
        spdoc::CoerceFromConstSP cf = cls->coerceFrom[i];
        spdoc::AttributeConstSP cfa = cf->coerceFrom;
        spdoc::DataTypeConstSP dt = cfa->dataType;
        int arrayDim = cfa->arrayDim;

        std::ostringstream funcName;
        funcName << cname << "_coerce_from_" << StringReplace(dt->name, ".", "_");
        switch (arrayDim)
        {
        case 0:
            break;
        case 1:
            funcName << "_Vector";
            break;
        case 2:
            funcName << "_Matrix";
            break;
        default:
            SPI_THROW_RUNTIME_ERROR("ArrayDim out of range");
            break;
        }

        ostr << "\n"
            << service->csDllImport() << "\n"
            << "private static extern IntPtr " << funcName.str() << "(\n"
            << "    " << CDataType(dt, service).csiType(arrayDim) << " " << service->rename(cfa->name) << ");\n";

        cfFuncNames.push_back(funcName.str());
    }

    for (size_t i = 0; i < cls->coerceTo.size(); ++i)
    {
        spdoc::CoerceToConstSP ct = cls->coerceTo[i];
        spdoc::DataTypeConstSP dt = ct->classType;

        ostr << "\n"
            << service->csDllImport() << "\n"
            << "private static extern IntPtr " << cname << "_coerce_to_"
            << StringReplace(dt->name, ".", "_");

        ostr << "(IntPtr self);\n";
    }

    std::vector<std::string> cMethodFuncNames;
    for (size_t i = 0; i < cls->methods.size(); ++i)
    {
        const spdoc::ClassMethodConstSP& method = cls->methods[i];
        if (method->isImplementation)
            continue;

        const spdoc::FunctionConstSP func = method->function;

        std::ostringstream oss;
        oss << service->ns() << "_" << makeNamespaceSep(module->ns, "_")
            << cls->name << "_" << func->name;

        std::string cFuncName = oss.str();

        ostr << "\n"
            << service->csDllImport() << "\n"
            << "private static extern int " << cFuncName << "(";

        declarePlatformInvokeArgs(ostr, func.get(), !method->isStatic);

        cMethodFuncNames.push_back(cFuncName);
    }

    for (size_t i = 0; i < cls->attributes.size(); ++i)
    {
        const spdoc::ClassAttributeConstSP attr = cls->attributes[i];
        if (attr->accessible)
        {
            ostr << "\n"
                << service->csDllImport() << "\n"
                << "private static extern int " << cname << "_get_" << attr->name << "(\n"
                << "    IntPtr self,\n"
                << "    out " << CDataType(attr->dataType, service).csiType(attr->arrayDim)
                << " " << service->rename(attr->name) << ");\n";
        }
    }

    for (size_t i = 0; i < cls->properties.size(); ++i)
    {
        const spdoc::ClassAttributeConstSP prop = cls->properties[i];
        SPI_POST_CONDITION(prop->accessible);
        ostr << "\n"
            << service->csDllImport() << "\n"
            << "private static extern int " << cname << "_get_" << prop->name << "(\n"
            << "    IntPtr self,\n"
            << "    out " << CDataType(prop->dataType, service).csiType(prop->arrayDim)
            << " " << service->rename(prop->name) << ");\n";
    }

    if (cls->canPut)
    {
        for (size_t i = 0; i < cls->attributes.size(); ++i)
        {
            const spdoc::ClassAttributeConstSP attr = cls->attributes[i];
            if (attr->accessible)
            {
                ostr << "\n"
                    << service->csDllImport() << "\n"
                    << "private static extern IntPtr " << cname << "_set_" << attr->name << "(\n"
                    << "    IntPtr self,\n"
                    << "    " << CDataType(attr->dataType, service).csiType(attr->arrayDim)
                    << " " << service->rename(attr->name) << ");\n";
            }
        }
    }

    std::string Base("spi.Object");
    
    if (!cls->baseClassName.empty())
    {
        spdoc::ClassConstSP baseClass = service->service()->getClass(cls->baseClassName);

        Base = StringFormat("%s.%s", baseClass->dataType->nsService.c_str(),
            baseClass->dataType->name.c_str());
    }
    ostr << "\n";

    xmlWriteDescription(ostr, "summary", "", "", cls->description);

    ostr << "public class " << cls->name << " : " << Base << "\n"
        << "{";
    
    if (!cls->noMake)
    {
        ostr << "\n"
            << "    ///<summary>Constructor</summary>\n";
        for (size_t i = 0; i < cls->attributes.size(); ++i)
        {
            spdoc::ClassAttributeConstSP attr = cls->attributes[i];
            std::ostringstream oss;
            oss << "name=\"" << attr->name << "\"";
            xmlWriteDescription(ostr, "param", oss.str(), "    ", attr->description);
        }
        ostr << "    public " << cls->name << "(";

        char* sep = "\n        ";
        char* sep2 = ",\n        ";

        for (size_t i = 0; i < cls->attributes.size(); ++i)
        {
            const spdoc::ClassAttributeConstSP& attr = cls->attributes[i];
            ostr << sep;
            sep = sep2;
            ostr << CDataType(attr->dataType, service).csType(attr->arrayDim) << " " << service->rename(attr->name);
        }
        ostr << ")\n"
            << "    {\n";

        ostr << "        IntPtr inner = " << cname << "_new(";
        size_t pos = cname.length() + 28;

        sep = "";
        for (size_t i = 0; i < cls->attributes.size(); ++i)
        {
            const spdoc::ClassAttributeConstSP& arg = cls->attributes[i];
            ostr << sep;
            std::string convertedArg = CDataType(arg->dataType, service).cs_to_c(
                arg->arrayDim, arg->name);

            pos += strlen(sep);
            if (pos + convertedArg.length() > 75)
            {
                ostr << "\n            ";
                pos = 12;
            }
            else if (strcmp(sep, ",") == 0)
            {
                ostr << " ";
                pos += 1;
            }
            sep = ",";
            ostr << convertedArg;
            pos += convertedArg.length();
        }
        ostr << ");\n"
            << "\n"
            << "        if (inner == IntPtr.Zero)\n"
            << "        {\n"
            << "            throw spi.ErrorToException();\n"
            << "        }\n"
            << "        set_inner(inner);\n"
            << "    }\n";
    }

    // coerce from general object - this cannot be implicit since C# doesn't allow it
    ostr << "\n"
        << "    ///<summary>Coerce from object - validated downcast</summary>\n";
    std::string New = cls->baseClassName.empty() ? "" : "new ";
    ostr << "    static public " << New << cls->name << " Coerce(spi.Object obj)\n"
        << "    {\n"
        << "        if (" << cname << "_coerce_from_object(spi.Object.get_inner(obj), out IntPtr inner) != 0)\n"
        << "        {\n"
        << "            throw spi.ErrorToException();\n"
        << "        }\n"
        << "        return " << cls->name << ".Wrap(inner);\n"
        << "    }\n";

    // coerceFrom as implicit operator cast where possible - otherwise use static Coerce method
    for (size_t i = 0; i < cls->coerceFrom.size(); ++i)
    {
        spdoc::CoerceFromConstSP cf = cls->coerceFrom[i];
        spdoc::AttributeConstSP cfa = cf->coerceFrom;
        //spdoc::DataTypeConstSP dt = cfa->dataType;
        int arrayDim = cfa->arrayDim;

        CDataType cdt(cfa->dataType, service);

        bool isSubClass = cfa->isArray() ?
            false :
            service->service()->isSubClass(cls, cfa->dataType->name);

        ostr << "\n";
        xmlWriteDescription(ostr, "summary", "", "    ", cf->description);
        xmlWriteDescription(ostr, "param",
            StringFormat("name=\"%s\"", cfa->name.c_str()),
            "    ",
            cfa->description);

        if (isSubClass)
        {
            // C# does not allow implicit downcast even if the Coerce method will
            // almost certainly not be a downcast operator
            ostr << "    static public " << cls->name << " Coerce(";
        }
        else
        {
            ostr << "    static public implicit operator " << cls->name << "(";
        }

        ostr << cdt.csType(cfa->arrayDim) << " " << service->rename(cfa->name) << ")\n"
            << "    {\n"
            << "        IntPtr inner = " << cfFuncNames[i] << "("
            << cdt.cs_to_c(arrayDim, cfa->name) << ");\n"
            << "\n"
            << "        if (inner == IntPtr.Zero)\n"
            << "            throw spi.ErrorToException();\n"
            << "\n"
            << "        return " << cls->name << ".Wrap(inner);\n"
            << "    }\n";
    }

    // coerceTo as implicit cast operator
    for (size_t i = 0; i < cls->coerceTo.size(); ++i)
    {
        spdoc::CoerceToConstSP ct = cls->coerceTo[i];
        spdoc::DataTypeConstSP dt = ct->classType;

        ostr << "\n";
        xmlWriteDescription(ostr, "summary", "", "    ", ct->description);
        ostr << "    public static implicit operator " << dt->nsService << "." << dt->name << "(" << cls->name << " self)\n"
            << "    {\n"
            << "        IntPtr inner = " << cname << "_coerce_to_"
            << StringReplace(dt->name, ".", "_") << "(self.self);\n"
            << "\n"
            << "        if (inner == IntPtr.Zero)\n"
            << "            throw spi.ErrorToException();\n"
            << "\n"
            << "        return " << dt->nsService << "." << dt->name << ".Wrap(inner);\n"
            << "    }\n";
    }

    // from_string and from_file - these are implemented returning Object in the base class
    // hence we need to declare them as new in all cases
    ostr << "\n"
         << "    static public new " << cls->name << " from_string(string objectString)\n"
         << "    {\n"
         << "        IntPtr inner = " << cname << "_from_string(objectString);\n"
         << "        if (inner == IntPtr.Zero)\n"
         << "            throw spi.ErrorToException();\n"
         << "        return " << cls->name << ".Wrap(inner);\n"
         << "    }\n"
         << "\n"
         << "    static public new " << cls->name << " from_file(string filename)\n"
         << "    {\n"
         << "        IntPtr inner = " << cname << "_from_file(filename);\n"
         << "        if (inner == IntPtr.Zero)\n"
         << "            throw spi.ErrorToException();\n"
         << "        return " << cls->name << ".Wrap(inner);\n"
         << "    }\n";
    
    for (size_t i = 0; i < cls->methods.size(); ++i)
    {
        const spdoc::ClassMethodConstSP& method = cls->methods[i];
        if (method->isImplementation)
            continue;

        implementClassMethod(ostr, cls, method.get(), cMethodFuncNames[i]);
    }

    // attribute accessors
    // if can put we also get a set method
    for (size_t i = 0; i < cls->attributes.size(); ++i)
    {
        const spdoc::ClassAttributeConstSP attr = cls->attributes[i];
        if (attr->accessible)
        {
            const std::string& propertyClass = service->service()->getPropertyClass(
                cls->baseClassName, attr->name);

            bool isOverride = !propertyClass.empty();

            implementProperty(ostr, attr, cname, cls->canPut, isOverride);
        }
    }

    for (size_t i = 0; i < cls->properties.size(); ++i)
    {
        const spdoc::ClassAttributeConstSP prop = cls->properties[i];

        const std::string& propertyClass = service->service()->getPropertyClass(
            cls->baseClassName, prop->name);

        bool isOverride = !propertyClass.empty();

        implementProperty(ostr, prop, cname, false, isOverride);
    }

    // internal constructor via IntPtr
    ostr << "\n"
        << "    protected " << cls->name << "() { }\n"
        << "\n"
        << "    public";

    //if (hasBaseClass)
        ostr << " new";

    ostr << " static " << cls->name << " Wrap(IntPtr self)\n"
        << "    {\n"
        << "        if (self == IntPtr.Zero)\n"
        << "            return null;\n"
        << "\n"
        << "        " << cls->name << " obj = new " << cls->name << "();\n"
        << "        obj.set_inner(self);\n"
        << "        return obj;\n"
        << "    }\n";


    ostr << "}\n";

    ostr << "\n"
        << "public static spi.PointerHandle zz_" << cls->name << "_VectorToHandle(IntPtr v)\n"
        << "{\n"
        << "    return new spi.PointerHandle(v, " << cname << "_Vector_delete);\n"
        << "}\n"
        << "\n"
        << "public static spi.PointerHandle zz_" << cls->name << "_VectorFromArray(" << cls->name << "[] array)\n"
        << "{\n"
        << "    int size = array.Length;\n"
        << "    IntPtr v = " << cname << "_Vector_new(size);\n"
        << "    if (v == IntPtr.Zero)\n"
        << "    {\n"
        << "        throw spi.ErrorToException();\n"
        << "    }\n"
        << "\n"
        << "    spi.PointerHandle h = new spi.PointerHandle(v, " << cname << "_Vector_delete);\n"
        << ""
        << "    for (int i = 0; i < size; ++i)\n"
        << "    {\n"
        << "        if (spi_Instance_Vector_set_item(v, i, spi.Object.get_inner(array[i])) != 0)\n"
        << "        {\n"
        << "            throw spi.ErrorToException();\n"
        << "        }\n"
        << "    }\n"
        << "\n"
        << "    return h;\n"
        << "}\n";

    ostr << "\n"
        << "public static " << cls->name << "[] zz_" << cls->name << "_VectorToArray(spi.PointerHandle h)\n"
        << "{\n"
        << "    IntPtr v = h.get_inner();\n"
        << "    int size;\n"
        << "    if (spi_Instance_Vector_size(v, out size) != 0)\n"
        << "    {\n"
        << "        throw spi.ErrorToException();\n"
        << "    }\n"
        << "\n"
        << "    " << cls->name << "[] array = new " << cls->name << "[size];\n"
        << "    for (int i = 0; i < size; ++i)\n"
        << "    {\n"
        << "        if (spi_Instance_Vector_item(v, i, out IntPtr item) != 0)\n"
        << "        {\n"
        << "            throw spi.ErrorToException();\n"
        << "        }\n"
        << "        array[i] = " << cls->name << ".Wrap(item);\n"
        << "    }\n"
        << "    return array;\n"
        << "}\n";


}

void CModule::implementProperty(
    GeneratedOutput& ostr,
    const spdoc::ClassAttributeConstSP& attr,
    const std::string& cname,
    bool canPut,
    bool isOverride) const
{
    CDataType cdt(attr->dataType, service);
    SPI_PRE_CONDITION(attr->accessible);

    ostr << "\n";
    xmlWriteDescription(ostr, "summary", "", "    ", attr->description);
    std::string New = isOverride ? "new " : "";
    ostr << "    public " << New << cdt.csType(attr->arrayDim) << " " << service->rename(attr->name) << "\n"
        << "    {\n"
        << "        get\n"
        << "        {\n"
        << "            if (" << cname << "_get_" << attr->name
        << "(self, out " << cdt.csiType(attr->arrayDim) << " c_" << attr->name << ") != 0)\n"
        << "            {\n"
        << "                throw spi.ErrorToException();\n"
        << "            }\n";

    std::string captureOutput = cdt.c_to_csi(attr->arrayDim, attr->name);

    if (!captureOutput.empty())
    {
        ostr << "            " << captureOutput << ";\n\n";
    }

    ostr << "            return " << cdt.csi_to_cs(attr->arrayDim, attr->name) << ";\n"
        << "        }\n";

    if (canPut)
    {
        ostr << "\n"
            << "        set\n" // note that the input is arbitrary called value
            << "        {\n";

        ostr << "            IntPtr modified = " << cname << "_set_" << attr->name << "(self,";

        std::string arg = cdt.cs_to_c(attr->arrayDim, "value");
        if (arg.length() + cname.length() + attr->name.length() > 40)
        {
            ostr << "\n"
                << "                ";
        }
        else
        {
            ostr << " ";
        }
        ostr << arg << ");\n"
            << "            if (modified == IntPtr.Zero)\n"
            << "            {\n"
            << "                throw spi.ErrorToException();\n"
            << "            }\n"
            << "            this.set_inner(modified);\n" // we have changed the reference counted pointer inside self
            << "        }\n";
    }

    ostr << "    }\n";
}

void CModule::implementClassMethod(
    GeneratedOutput& ostr,
    const spdoc::Class* cls,
    const spdoc::ClassMethod* method,
    const std::string& cFuncName) const
{
    const spdoc::FunctionConstSP func = method->function;

    std::string csReturnType;
    std::string csOutputClass;
    if (!func->returnType)
    {
        csOutputClass = implementFunctionOutputClass(ostr, func.get(), 4);
        csReturnType = csOutputClass.empty() ? "void" : csOutputClass;
    }
    else
    {
        csReturnType = CDataType(func->returnType, service).csType(func->returnArrayDim);
    }

    implementFunctionSummary(ostr, func.get(), 4);
    ostr << "    public ";

    if (method->isStatic)
        ostr << "static ";

    ostr << csReturnType << " " << func->name << "(";

    implementFunctionBegin(ostr, func.get(), 4);

    ostr << "        if (" << cFuncName << "(";

    const char* sep = "";

    if (!method->isStatic)
    {
        ostr << "self";
        sep = ",";
    }

    implementPlatformInvoke(ostr, func.get(), 4, sep);
    implementFunctionEnd(ostr, func.get(), 4, csOutputClass);
}

void CModule::declarePlatformInvokeArgs(
    GeneratedOutput& ostr,
    const spdoc::Function* func, 
    bool includeSelf) const
{
    // we assume that we have just written the "(" to start the input/output arguments

    const char* sep = "\n    ";
    const char* sep2 = ",\n    ";

    if (includeSelf)
    {
        ostr << sep;
        sep = sep2;
        ostr << "IntPtr self";
    }

    for (size_t i = 0; i < func->inputs.size(); ++i)
    {
        const spdoc::AttributeConstSP& arg = func->inputs[i];
        ostr << sep;
        sep = sep2;
        ostr << CDataType(arg->dataType, service).csiType(arg->arrayDim) << " " << service->rename(arg->name);
    }

    if (func->returnType)
    {
        ostr << sep;
        sep = sep2;
        ostr << "out "
            << CDataType(func->returnType, service).csiType(func->returnArrayDim) << " _out";
    }

    for (size_t i = 0; i < func->outputs.size(); ++i)
    {
        const spdoc::AttributeConstSP& out = func->outputs[i];
        ostr << sep;
        sep = ",\n    ";
        ostr << "out "
            << CDataType(out->dataType, service).csiType(out->arrayDim) << " " << service->rename(out->name);
    }

    ostr << ");\n";
}

void CModule::implementEnum(
    GeneratedOutput& ostr,
    const spdoc::Enum* enumType) const
{
    std::ostringstream oss;
    oss << service->ns() << "_" << makeNamespaceSep(module->ns, "_") << enumType->name;
    std::string cname = oss.str();

    ostr << "\n";
    xmlWriteDescription(ostr, "summary", "", "", enumType->description);
    ostr << "public enum " << enumType->name << " {";

    const char* sep = "\n";
    for (size_t i = 0; i < enumType->enumerands.size(); ++i)
    {
        ostr << sep;
        xmlWriteDescription(ostr, "summary", "", "    ", enumType->enumerands[i]->description);
        ostr << "    " << enumType->enumerands[i]->code;
        sep = ",\n";
    }
    ostr << "}\n";

    ostr << "\n"
        << service->csDllImport() << "\n"
        << "private static extern void " << cname << "_Vector_delete(IntPtr v);\n"
        << "\n"
        << service->csDllImport() << "\n"
        << "private static extern IntPtr " << cname << "_Vector_new(int size);\n";

    ostr << "\n"
        << "public static spi.PointerHandle zz_" << enumType->name << "_VectorToHandle(IntPtr v)\n"
        << "{\n"
        << "    return new spi.PointerHandle(v, " << cname << "_Vector_delete);\n"
        << "}\n"
        << "\n"
        << "public static spi.PointerHandle zz_" << enumType->name << "_VectorFromArray("
        << enumType->name << "[] array)\n"
        << "{\n"
        << "    int size = array.Length;\n"
        << "    IntPtr v = " << cname << "_Vector_new(size);\n"
        << "    if (v == IntPtr.Zero)\n"
        << "    {\n"
        << "        throw spi.ErrorToException();\n"
        << "    }\n"
        << "\n"
        << "    spi.PointerHandle h = new spi.PointerHandle(v, " << cname << "_Vector_delete);\n"
        << ""
        << "    for (int i = 0; i < size; ++i)\n"
        << "    {\n"
        << "        if (spi_Enum_Vector_set_item(v, i, (int)array[i]) != 0)\n"
        << "        {\n"
        << "            throw spi.ErrorToException();\n"
        << "        }\n"
        << "    }\n"
        << "\n"
        << "    return h;\n"
        << "}\n"
        << "\n"
        << "public static " << enumType->name << "[] zz_" << enumType->name
        << "_VectorToArray(spi.PointerHandle h)\n"
        << "{\n"
        << "    IntPtr v = h.get_inner();\n"
        << "    int size;\n"
        << "    if (spi_Enum_Vector_size(v, out size) != 0)\n"
        << "    {\n"
        << "        throw spi.ErrorToException();\n"
        << "    }\n"
        << "\n"
        << "    " << enumType->name << "[] array = new " << enumType->name << "[size];\n"
        << "    for (int i = 0; i < size; ++i)\n"
        << "    {\n"
        << "        if (spi_Enum_Vector_item(v, i, out int e) != 0)\n"
        << "        {\n"
        << "            throw spi.ErrorToException();\n"
        << "        }\n"
        << "        array[i] = (" << enumType->name << ")e; \n"
        << "    }\n"
        << "    return array;\n"
        << "}\n";

    ostr << "\n"
        << service->csDllImport() << "\n"
        << "private static extern void " << cname << "_Matrix_delete(IntPtr m);"
        << "\n"
        << service->csDllImport() << "\n"
        << "private static extern IntPtr " << cname << "_Matrix_new(\n"
        << "    int nr, int nc);\n";
 
    ostr << "\n"
        << "public static spi.PointerHandle zz_" << enumType->name << "_MatrixToHandle(IntPtr v)\n"
        << "{\n"
        << "    return new spi.PointerHandle(v, " << cname << "_Matrix_delete);\n"
        << "}\n" 
        << "\n"
        << "public static spi.PointerHandle zz_" << enumType->name << "_MatrixFromArray("
        << enumType->name << "[,] array)\n"
        << "{\n"
        << "    int nr = array.GetLength(0);\n"
        << "    int nc = array.GetLength(1);\n"
        << "    IntPtr m = " << cname << "_Matrix_new(nr,nc);\n"
        << "    if (m == IntPtr.Zero)\n"
        << "    {\n"
        << "        throw spi.ErrorToException();\n"
        << "    }\n"
        << "\n"
        << "    spi.PointerHandle h = new spi.PointerHandle(m, " << cname << "_Matrix_delete);\n"
        << "    for (int i = 0; i < nr; ++i)\n"
        << "    {\n"
        << "        for (int j = 0; j < nc; ++j)\n"
        << "        {\n"
        << "            if (spi_Enum_Matrix_set_item(m, i, j, (int)array[i,j]) != 0)\n"
        << "            {\n"
        << "                throw spi.ErrorToException();\n"
        << "            }\n"
        << "        }\n"
        << "    }\n"
        << "    return h;\n"
        << "}\n"
        << "\n"
        << "public static " << enumType->name << "[,] zz_" << enumType->name
        << "_MatrixToArray(spi.PointerHandle h)\n"
        << "{\n"
        << "    IntPtr m = h.get_inner();\n"
        << "    int nr, nc;\n"
        << "    if (spi_Enum_Matrix_size(m, out nr, out nc) != 0)\n"
        << "    {\n"
        << "        throw spi.ErrorToException();\n"
        << "    }\n"
        << "\n"
        << "    " << enumType->name << "[,] array = new " << enumType->name << "[nr, nc];\n"
        << "    for (int i = 0; i < nr; ++i)\n"
        << "    {\n"
        << "        for (int j = 0; j < nc; ++j)\n"
        << "        {\n"
        << "            if (spi_Enum_Matrix_item(m, i, j, out int e) != 0)\n"
        << "            {\n"
        << "                throw spi.ErrorToException();\n"
        << "            }\n"
        << "            array[i,j] = (" << enumType->name << ")e; \n"
        << "        }\n"
        << "    }\n"
        << "    return array;\n"
        << "}\n";
}

void CModule::implementFunctionSummary(
    GeneratedOutput& ostr,
    const spdoc::Function* func,
    size_t indent) const
{
    std::string spaces(indent, ' ');
    ostr << "\n";
    xmlWriteDescription(ostr, "summary", "", spaces, func->description);
    for (size_t i = 0; i < func->inputs.size(); ++i)
    {
        spdoc::AttributeConstSP inp = func->inputs[i];
        std::ostringstream oss;
        oss << "name=\"" << inp->name << "\"";
        xmlWriteDescription(ostr, "param", oss.str(), spaces, inp->description);
    }
}

Usage::Usage() : hasVector(false), hasMatrix(false)
{
}

CDataType::CDataType(const spdoc::DataTypeConstSP& dataType, const CServiceConstSP& service) 
    : 
    dataType(dataType),
    publicType(dataType->publicType),
    service(service)
{
    
}

std::string CDataType::cName() const
{
    switch (publicType)
    {
    case spdoc::PublicType::BOOL:
        return "spi_Bool";
    case spdoc::PublicType::CHAR:
        return "spi_Char";
    case spdoc::PublicType::INT:
        return "spi_Int";
    case spdoc::PublicType::DOUBLE:
        return "spi_Double";
    case spdoc::PublicType::STRING:
        return "spi_String";
    case spdoc::PublicType::DATE:
        return "spi_Date";
    case spdoc::PublicType::DATETIME:
        return "spi_DateTime";
    case spdoc::PublicType::ENUM:
        return StringFormat("%s_%s",
            dataType->nsService.c_str(),
            StringReplace(dataType->name, ".", "_").c_str());
    case spdoc::PublicType::CLASS:
        return StringFormat("%s_%s",
            dataType->nsService.c_str(),
            StringReplace(dataType->name, ".", "_").c_str());
    case spdoc::PublicType::OBJECT:
        return "spi_Object";
    case spdoc::PublicType::MAP:
        return "spi_Map";
    case spdoc::PublicType::VARIANT:
        return "spi_Variant";
    default:
        SPI_THROW_RUNTIME_ERROR("Not implemented for " <<
            spdoc::PublicType::to_string(publicType));
        break;
    }

}

std::string CDataType::csiType(int arrayDim) const
{
    if (arrayDim != 0)
        return "IntPtr";

    std::ostringstream oss;

    switch (publicType)
    {
    case spdoc::PublicType::BOOL:
        return "bool";
    case spdoc::PublicType::CHAR:
        return "char";
    case spdoc::PublicType::INT:
        return "int";
    case spdoc::PublicType::DOUBLE:
        return "double";
    case spdoc::PublicType::STRING:
        return "string";
    case spdoc::PublicType::DATE:
        return "int";
    case spdoc::PublicType::DATETIME:
        return "double";
    case spdoc::PublicType::ENUM:
        oss << dataType->nsService << "." << dataType->name;
        return oss.str();
    case spdoc::PublicType::CLASS:
        return "IntPtr";
    case spdoc::PublicType::OBJECT:
        return "IntPtr";
    case spdoc::PublicType::MAP:
        return "IntPtr";
    case spdoc::PublicType::VARIANT:
        return "IntPtr";
    default:
        SPI_THROW_RUNTIME_ERROR("Scalars not implemented for " <<
            spdoc::PublicType::to_string(publicType));
    }

}

std::string CDataType::csType(int arrayDim) const
{
    std::string scalarType;
    std::ostringstream oss;

    switch (publicType)
    {
    case spdoc::PublicType::BOOL:
        scalarType = "bool";
        break;
    case spdoc::PublicType::CHAR:
        scalarType = "char";
        break;
    case spdoc::PublicType::INT:
        scalarType = "int";
        break;
    case spdoc::PublicType::DOUBLE:
        scalarType = "double";
        break;
    case spdoc::PublicType::STRING:
        scalarType = "string";
        break;
    case spdoc::PublicType::DATE:
        scalarType = "System.DateTime";
        break;
    case spdoc::PublicType::DATETIME:
        scalarType = "System.DateTime";
        break;
    case spdoc::PublicType::ENUM:
    case spdoc::PublicType::CLASS:
        oss << /* service->nsGlobal() << "." << */ dataType->nsService << "." << dataType->name;
        scalarType = oss.str();
        break;
    case spdoc::PublicType::OBJECT:
        scalarType = "spi.Object";
        break;
    case spdoc::PublicType::MAP:
        scalarType = "spi.Map";
        break;
    case spdoc::PublicType::VARIANT:
        scalarType = "spi.Variant";
        break;
    default:
        SPI_THROW_RUNTIME_ERROR("Scalars not implemented for " <<
            spdoc::PublicType::to_string(publicType));
    }

    switch (arrayDim)
    {
    case 0:
        return scalarType;
    case 1:
        return scalarType + "[]";
    case 2:
        return scalarType + "[,]";
    default:
        SPI_THROW_RUNTIME_ERROR("ArrayDim " << arrayDim << " out of range [0,2]");
    }

}

namespace
{
    std::string forArrayTranslations(const std::string& csType)
    {
        size_t pos = csType.rfind('.');
        if (pos == std::string::npos)
            return csType;

        return csType.substr(0, pos+1) + "zz_" + csType.substr(pos+1);
    }
}

// this function converts from C# type to PINVOKE type
//
// sometimes we don't have to do anything (the standard marshalling does the business)
// sometimes we can use a single function call to convert
// sometimes we have to calculate an intermediate variable
std::string CDataType::cs_to_c(int arrayDim, const std::string& name) const
{
    std::ostringstream oss;

    switch (arrayDim)
    {
    case 0: // scalars
        switch (publicType)
        {
        case spdoc::PublicType::BOOL:
            break;
        case spdoc::PublicType::CHAR:
            break;
        case spdoc::PublicType::INT:
            break;
        case spdoc::PublicType::DOUBLE:
            break;
        case spdoc::PublicType::STRING:
            break;
        case spdoc::PublicType::DATE:
            oss << "spi.DateToCDate(" << service->rename(name) << ")";
            break;
        case spdoc::PublicType::DATETIME:
            oss << "spi.DateTimeToCDateTime(" << service->rename(name) << ")";
            break;
        case spdoc::PublicType::CLASS:
            oss << csType(0) << ".get_inner(" << service->rename(name) << ")";
            break;
        case spdoc::PublicType::OBJECT:
            oss << "spi.Object.get_inner(" << service->rename(name) << ")";
            break;
        case spdoc::PublicType::ENUM:
            break;
        case spdoc::PublicType::MAP:
            oss << "spi.Map.get_inner(" << service->rename(name) << ")";
            break;
        case spdoc::PublicType::VARIANT:
            oss << "spi.Variant.get_inner(" << service->rename(name) << ")";
            break;
        default:
            oss << "// " << __FUNCTION__ << " Scalars not implemented for " <<
                spdoc::PublicType::to_string(publicType);
        }
        break;
    case 1: // vectors
        switch (publicType)
        {
        case spdoc::PublicType::INT:
            oss << "spi.IntVectorFromArray(" << service->rename(name) << ").get_inner()";
            break;
        case spdoc::PublicType::BOOL:
            oss << "spi.BoolVectorFromArray(" << service->rename(name) << ").get_inner()";
            break;
        case spdoc::PublicType::DOUBLE:
            oss << "spi.DoubleVectorFromArray(" << service->rename(name) << ").get_inner()";
            break;
        case spdoc::PublicType::STRING:
            oss << "spi.StringVectorFromArray(" << service->rename(name) << ").get_inner()";
            break;
        case spdoc::PublicType::DATE:
            oss << "spi.DateVectorFromArray(" << service->rename(name) << ").get_inner()";
            break;
        case spdoc::PublicType::DATETIME:
            oss << "spi.DateTimeVectorFromArray(" << service->rename(name) << ").get_inner()";
            break;
        case spdoc::PublicType::ENUM:
        case spdoc::PublicType::CLASS:
            oss << forArrayTranslations(csType(0)) << "_VectorFromArray(" << service->rename(name) << ").get_inner()";
            break;
        case spdoc::PublicType::VARIANT:
            oss << "spi.VariantVectorFromArray(" << service->rename(name) << ").get_inner()";
            break;
        case spdoc::PublicType::OBJECT:
            oss << "spi.ObjectVectorFromArray(" << service->rename(name) << ").get_inner()";
            break;
        case spdoc::PublicType::MAP:
        case spdoc::PublicType::CHAR:
        default:
            oss << "// " << __FUNCTION__ << " Vector not implemented for " <<
                spdoc::PublicType::to_string(publicType);
        }
        break;
    case 2: // matrix
        switch (publicType)
        {
        case spdoc::PublicType::BOOL:
            oss << "spi.BoolMatrixFromArray(" << service->rename(name) << ").get_inner()";
            break;
        case spdoc::PublicType::INT:
            oss << "spi.IntMatrixFromArray(" << service->rename(name) << ").get_inner()";
            break;
        case spdoc::PublicType::DOUBLE:
            oss << "spi.DoubleMatrixFromArray(" << service->rename(name) << ").get_inner()";
            break;
        case spdoc::PublicType::STRING:
            oss << "spi.StringMatrixFromArray(" << service->rename(name) << ").get_inner()";
            break;
        case spdoc::PublicType::DATE:
            oss << "spi.DateMatrixFromArray(" << service->rename(name) << ").get_inner()";
            break;
        case spdoc::PublicType::DATETIME:
            oss << "spi.DateTimeMatrixFromArray(" << service->rename(name) << ").get_inner()";
            break;
        case spdoc::PublicType::VARIANT:
            oss << "spi.VariantMatrixFromArray(" << service->rename(name) << ").get_inner()";
            break;
        case spdoc::PublicType::ENUM:
        case spdoc::PublicType::CLASS:
            oss << forArrayTranslations(csType(0)) << "_MatrixFromArray(" << service->rename(name) << ").get_inner()";
            break;
        case spdoc::PublicType::OBJECT:
            oss << "spi.ObjectMatrixFromArray(" << service->rename(name) << ").get_inner()";
            break;
        case spdoc::PublicType::MAP:
        case spdoc::PublicType::CHAR:
        default:
            oss << "// " << __FUNCTION__ << " Matrix not implemented for " <<
                spdoc::PublicType::to_string(publicType);
        }
        break;
    default:
        SPI_THROW_RUNTIME_ERROR("ArrayDim " << arrayDim << " out of range [0,2]");
    }

    std::string conversion = oss.str();
    if (conversion.empty())
        return service->rename(name);

    return conversion;
}

// this is code which converts from PINVOKE output to an intermediate C# type
// sometimes it isn't needed at all
// similar purpose to cs_to_csi accept this is data on the way out rather than in
std::string CDataType::c_to_csi(int arrayDim, const std::string& name) const
{
    std::ostringstream oss;

    switch (arrayDim)
    {
    case 0: // scalars
        switch (publicType)
        {
        case spdoc::PublicType::BOOL:
            break;
        case spdoc::PublicType::CHAR:
            break;
        case spdoc::PublicType::INT:
            break;
        case spdoc::PublicType::DOUBLE:
            break;
        case spdoc::PublicType::STRING:
            break;
        case spdoc::PublicType::DATE:
            break;
        case spdoc::PublicType::DATETIME:
            break;
        case spdoc::PublicType::ENUM:
            break;
        // should we capture these types at once?
        // this is in case there are failures in translating out
        case spdoc::PublicType::CLASS:
        case spdoc::PublicType::OBJECT:
        case spdoc::PublicType::MAP:
        case spdoc::PublicType::VARIANT:
            break;
        default:
            oss << "// " << __FUNCTION__ << " Scalars not implemented for " <<
                spdoc::PublicType::to_string(publicType);
        }
        break;
    case 1: // vectors
        switch (publicType)
        {
        case spdoc::PublicType::BOOL:
            oss << "var o_" << name << " = spi.BoolVectorToHandle(c_" << name << ")";
            break;
        case spdoc::PublicType::INT:
            oss << "var o_" << name << " = spi.IntVectorToHandle(c_" << name << ")";
            break;
        case spdoc::PublicType::DOUBLE:
            oss << "var o_" << name << " = spi.DoubleVectorToHandle(c_" << name << ")";
            break;
        case spdoc::PublicType::STRING:
            oss << "var o_" << name << " = spi.StringVectorToHandle(c_" << name << ")";
            break;
        case spdoc::PublicType::DATE:
            oss << "var o_" << name << " = spi.DateVectorToHandle(c_" << name << ")";
            break;
        case spdoc::PublicType::DATETIME:
            oss << "var o_" << name << " = spi.DateTimeVectorToHandle(c_" << name << ")";
            break;
        case spdoc::PublicType::ENUM:
        case spdoc::PublicType::CLASS:
            oss << "var o_" << name << " = " << forArrayTranslations(csType(0)) << "_VectorToHandle(c_" << name << ")";
            break;
        case spdoc::PublicType::VARIANT:
            oss << "var o_" << name << " = spi.VariantVectorToHandle(c_" << name << ")";
            break;
        case spdoc::PublicType::OBJECT:
            oss << "var o_" << name << " = spi.ObjectVectorToHandle(c_" << name << ")";
            break;
        case spdoc::PublicType::MAP:
        default:
            oss << "// " << __FUNCTION__ << " Vector not implemented for " <<
                spdoc::PublicType::to_string(publicType);
        }
        break;
    case 2: // matrix
        switch (publicType)
        {
        case spdoc::PublicType::BOOL:
            oss << "var o_" << name << " = spi.BoolMatrixToHandle(c_" << name << ")";
            break;
        case spdoc::PublicType::INT:
            oss << "var o_" << name << " = spi.IntMatrixToHandle(c_" << name << ")";
            break;
        case spdoc::PublicType::DOUBLE:
            oss << "var o_" << name << " = spi.DoubleMatrixToHandle(c_" << name << ")";
            break;
        case spdoc::PublicType::STRING:
            oss << "var o_" << name << " = spi.StringMatrixToHandle(c_" << name << ")";
            break;
        case spdoc::PublicType::DATE:
            oss << "var o_" << name << " = spi.DateMatrixToHandle(c_" << name << ")";
            break;
        case spdoc::PublicType::DATETIME:
            oss << "var o_" << name << " = spi.DateTimeMatrixToHandle(c_" << name << ")";
            break;
        case spdoc::PublicType::ENUM:
        case spdoc::PublicType::CLASS:
            oss << "var o_" << name << " = " << forArrayTranslations(csType(0)) << "_MatrixToHandle(c_" << name << ")";
            break;
        case spdoc::PublicType::VARIANT:
            oss << "var o_" << name << " = spi.VariantMatrixToHandle(c_" << name << ")";
            break;
        case spdoc::PublicType::OBJECT:
            oss << "var o_" << name << " = spi.ObjectMatrixToHandle(c_" << name << ")";
            break;
        case spdoc::PublicType::MAP:
        default:
            oss << "// " << __FUNCTION__ << " Matrix not implemented for " <<
                spdoc::PublicType::to_string(publicType);
        }
        break;
    default:
        SPI_THROW_RUNTIME_ERROR("ArrayDim " << arrayDim << " out of range [0,2]");
    }

    return oss.str();
}

std::string CDataType::csi_to_cs(int arrayDim, const std::string& name) const
{
    std::ostringstream oss;

    switch (arrayDim)
    {
    case 0: // scalars
        switch (publicType)
        {
        case spdoc::PublicType::BOOL:
            break;
        case spdoc::PublicType::CHAR:
            break;
        case spdoc::PublicType::INT:
            break;
        case spdoc::PublicType::DOUBLE:
            break;
        case spdoc::PublicType::STRING:
            break;
        case spdoc::PublicType::DATE:
            oss << "spi.DateFromCDate(c_" << name << ")";
            break;
        case spdoc::PublicType::DATETIME:
            oss << "spi.DateTimeFromCDateTime(c_" << name << ")";
            break;
        case spdoc::PublicType::ENUM:
            break;
        case spdoc::PublicType::CLASS:
            oss << service->nsGlobal() << "." << dataType->nsService << "." << dataType->name << ".Wrap(c_" << name << ")";
            break;
        case spdoc::PublicType::OBJECT:
            oss << "spi.Object.Wrap(c_" << name << ")";
            break;
        case spdoc::PublicType::MAP:
            oss << "spi.Map.Wrap(c_" << name << ")";
            break;
        case spdoc::PublicType::VARIANT:
            oss << "new spi.Variant(c_" << name << ")";
            break;
        default:
            oss << "// " << __FUNCTION__ << " Scalars not implemented for " <<
                spdoc::PublicType::to_string(publicType);
        }
        break;
    case 1: // vectors
        switch (publicType)
        {
        case spdoc::PublicType::INT:
            oss << "spi.IntVectorToArray(o_" << name << ")";
            break;
        case spdoc::PublicType::BOOL:
            oss << "spi.BoolVectorToArray(o_" << name << ")";
            break;
        case spdoc::PublicType::DOUBLE:
            oss << "spi.DoubleVectorToArray(o_" << name << ")";
            break;
        case spdoc::PublicType::DATE:
            oss << "spi.DateVectorToArray(o_" << name << ")";
            break;
        case spdoc::PublicType::STRING:
            oss << "spi.StringVectorToArray(o_" << name << ")";
            break;
        case spdoc::PublicType::DATETIME:
            oss << "spi.DateTimeVectorToArray(o_" << name << ")";
            break;
        case spdoc::PublicType::VARIANT:
            oss << "spi.VariantVectorToArray(o_" << name << ")";
            break;
        case spdoc::PublicType::ENUM:
        case spdoc::PublicType::CLASS:
            oss << forArrayTranslations(csType(0)) << "_VectorToArray(o_" << name << ")";
            break;
        case spdoc::PublicType::OBJECT:
            oss << "spi.ObjectVectorToArray(o_" << name << ")";
            break;
        case spdoc::PublicType::MAP:
        default:
            oss << "// " << __FUNCTION__ << " Vector not implemented for " <<
                spdoc::PublicType::to_string(publicType);
        }
        break;
    case 2: // matrix
        switch (publicType)
        {
        case spdoc::PublicType::BOOL:
            oss << "spi.BoolMatrixToArray(o_" << name << ")";
            break;
        case spdoc::PublicType::INT:
            oss << "spi.IntMatrixToArray(o_" << name << ")";
            break;
        case spdoc::PublicType::DOUBLE:
            oss << "spi.DoubleMatrixToArray(o_" << name << ")";
            break;
        case spdoc::PublicType::STRING:
            oss << "spi.StringMatrixToArray(o_" << name << ")";
            break;
        case spdoc::PublicType::DATE:
            oss << "spi.DateMatrixToArray(o_" << name << ")";
            break;
        case spdoc::PublicType::DATETIME:
            oss << "spi.DateTimeMatrixToArray(o_" << name << ")";
            break;
        case spdoc::PublicType::VARIANT:
            oss << "spi.VariantMatrixToArray(o_" << name << ")";
            break;
        case spdoc::PublicType::ENUM:
        case spdoc::PublicType::CLASS:
            oss << forArrayTranslations(csType(0)) << "_MatrixToArray(o_" << name << ")";
            break;
        case spdoc::PublicType::OBJECT:
            oss << "spi.ObjectMatrixToArray(o_" << name << ")";
            break;
        case spdoc::PublicType::MAP:
        default:
            oss << "// " << __FUNCTION__ << " Matrix not implemented for " <<
                spdoc::PublicType::to_string(publicType);
        }
        break;
    default:
        SPI_THROW_RUNTIME_ERROR("ArrayDim " << arrayDim << " out of range [0,2]");
    }

    std::string returnedOutput = oss.str();
    if (returnedOutput.empty())
    {
        oss << "c_" << name;
        return oss.str();
    }
    return returnedOutput;
}

