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
#include "cWriter.hpp"

#include <spi_util/FileUtil.hpp>
#include <spi/StringUtil.hpp>
#include <spi/spdoc_configTypes.hpp>

#include <spgtools/licenseTools.hpp>
#include <spgtools/generatedOutput.hpp>
#include <spgtools/namespaceManager.hpp>

#include <iostream>
#include <map>

#define BEGIN_ANONYMOUS_NAMESPACE namespace {
#define END_ANONYMOUS_NAMESPACE }

#define CONSTRUCT_CAST(T, out, in) \
    spdoc::T const* out = dynamic_cast<spdoc::T const*>(in.get());\
    if (!out)\
        throw spi::RuntimeError("%s is of type %s and not %s", \
                                #in, in->get_class_name(), #T)

using spi_util::StringFormat;

/*
***************************************************************************
** Implementation of CService
***************************************************************************
*/
CServiceConstSP CService::Make(
    const spdoc::ServiceConstSP& service,
    const Options& options)
{
    return new CService(service, options);
}

CService::CService(
    const spdoc::ServiceConstSP& service,
    const Options& options)
    :
    m_service(service),
    m_import(),
    m_options(options)
{
    m_import = spi::StringFormat("%s_C_IMPORT", m_service->declSpec.c_str());

    for (std::vector<spdoc::ModuleConstSP>::const_iterator iter = service->modules.begin();
        iter != service->modules.end(); ++iter)
    {
        const spdoc::ModuleConstSP& module = *iter;
        for (size_t i = 0; i < module->constructs.size(); ++i)
        {
            const spdoc::ConstructConstSP& construct = module->constructs[i];
            const std::string& constructType = construct->getType();
            if (constructType == "CLASS")
            {
                spdoc::ClassConstSP cls = spdoc::Class::Coerce(construct);
                for (size_t j = 0; j < cls->attributes.size(); ++j)
                {
                    spdoc::ClassAttributeConstSP attr = cls->attributes[j];
                    updateUsage(attr->dataType, attr->arrayDim);
                }
                for (size_t j = 0; j < cls->properties.size(); ++j)
                {
                    spdoc::ClassAttributeConstSP prop = cls->properties[j];
                    updateUsage(prop->dataType, prop->arrayDim);
                }
                for (size_t j = 0; j < cls->methods.size(); ++j)
                {
                    spdoc::ClassMethodConstSP method = cls->methods[j];
                    updateUsage(method->function);
                }
            }
            else if (constructType == "FUNCTION")
            {
                spdoc::FunctionConstSP func = spdoc::Function::Coerce(construct);
                updateUsage(func);
            }
        }
    }
}

void CService::updateUsage(
    const spdoc::DataTypeConstSP& dataType,
    int arrayDim)
{
    spdoc::PublicType::Enum publicType = dataType->publicType;
    switch (publicType)
    {
    case spdoc::PublicType::ENUM:
    case spdoc::PublicType::CLASS:
        break;
    default:
        return;
    }
}

void CService::updateUsage(
    const spdoc::FunctionConstSP & function)
{
    if (function->returnType)
    {
        updateUsage(function->returnType, function->returnArrayDim);
    }

    for (size_t i = 0; i < function->inputs.size(); ++i)
    {
        spdoc::AttributeConstSP inp = function->inputs[i];
        updateUsage(inp->dataType, inp->arrayDim);
    }

    for (size_t i = 0; i < function->outputs.size(); ++i)
    {
        spdoc::AttributeConstSP out = function->outputs[i];
        updateUsage(out->dataType, out->arrayDim);
    }
}

std::string
CService::writeDeclSpecHeaderFile(const std::string& dirname) const
{
    std::string basename = spi::StringFormat("c_dll_%s_decl_spec.h", m_service->name.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename));
    writeLicense(ostr, license());
    startHeaderFile(ostr, filename);

    writeStartCommentBlock(ostr, false);
    ostr << "** " << basename << "\n"
         << "**\n"
         << "** If " << m_service->declSpec << "_C_EXPORT is defined "
         << "then " << m_service->declSpec << "_C_IMPORT is dllexport.\n"
         << "** Otherwise " << m_service->declSpec << "_C_IMPORT is dllimport.\n";
    writeEndCommentBlock(ostr);
    if (!m_options.noGeneratedCodeNotice)
        writeGeneratedCodeNotice(ostr, filename);

    ostr << "\n"
         << "#ifdef _MSC_VER\n"
         << "#ifdef " << m_service->declSpec << "_C_EXPORT\n"
         << "#define " << m_service->declSpec << "_C_IMPORT __declspec(dllexport)\n"
         << "#else\n"
         << "#define " << m_service->declSpec << "_C_IMPORT __declspec(dllimport)\n"
         << "#endif\n"
         << "#else\n"
         << "#define " << m_service->declSpec << "_C_IMPORT\n"
         << "#endif\n";

    endHeaderFile(ostr, filename);
    ostr.close();
    return filename;
}

std::string CService::writePublicHeaderFile(const std::string& dirname,
    const std::vector<CModuleConstSP>& modules) const
{
    std::string basename = spi::StringFormat("c_dll_%s.h", m_service->name.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename));
    writeLicense(ostr, license());
    startHeaderFile(ostr, filename);

    writeStartCommentBlock(ostr, false);
    ostr << "** " << basename << "\n"
         << "**\n"
         << "** Top level C functions for " << m_service->name << "\n";
    writeEndCommentBlock(ostr);
    if (!m_options.noGeneratedCodeNotice)
        writeGeneratedCodeNotice(ostr, filename);

    ostr << "\n"
        << "#include <spi/c/spi.h>\n";

    size_t numImports = m_options.imports.size();
    for (size_t i = 0; i < numImports; ++i)
    {
        ostr << "#include \"c_dll_" << m_options.imports[i] << ".h\"\n";
    }

    ostr << "#include \"c_dll_" << m_service->name << "_decl_spec.h\"\n"
        << "\n"
        << "#ifdef __cplusplus\n"
        << "extern \"C\"\n"
        << "{\n"
        << "#endif\n";

    if (!m_service->sharedService)
    {
        ostr << "\n"
            << m_import << "\n"
            << "int init_" << m_service->ns << "(void);\n"
            << "\n"
            << m_import << "\n"
            << "int " << m_service->ns << "_start_logging(\n"
            << "    const char* filename,\n"
            << "    const char* options);\n"
            << "\n"
            << m_import << "\n"
            << "int " << m_service->ns << "_stop_logging();\n";
    }

    for (size_t i = 0; i < modules.size(); ++i)
    {
        const CModuleConstSP& module = modules[i];
        module->updatePublicHeaderFile(ostr);
    }

    ostr << "\n"
        << "#ifdef __cplusplus\n"
        << "} /* end of extern \"C\" */\n"
        << "#endif\n";

    endHeaderFile(ostr, filename);
    ostr.close();
    return filename;
}

std::string CService::writePrivateHeaderFile(const std::string& dirname,
    const std::vector<CModuleConstSP>& modules) const
{
    std::string basename = spi::StringFormat("c_dll_%s.hpp", m_service->name.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), "src", basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename));
    writeLicense(ostr, license());
    startHeaderFile(ostr, filename);

    writeStartCommentBlock(ostr, false);
    ostr << "** " << basename << "\n"
        << "**\n"
        << "** Private C++ classes and functions for " << m_service->name << "\n";
    writeEndCommentBlock(ostr);
    if (!m_options.noGeneratedCodeNotice)
        writeGeneratedCodeNotice(ostr, filename);

    ostr << "\n"
        << "#include \"c_dll_" << m_service->name << ".h\"\n"
        << "#include <spi/c/c_spi.hpp>\n"
        << "\n";

    for (size_t i = 0; i < modules.size(); ++i)
    {
        const CModuleConstSP& module = modules[i];
        ostr << "#include \"" << m_service->ns << "_" << module->module->name
            << "_classes.hpp\"\n";
    }

    for (size_t i = 0; i < modules.size(); ++i)
    {
        const CModuleConstSP& module = modules[i];
        module->updatePrivateHeaderFile(ostr);
    }

    endHeaderFile(ostr, filename);
    ostr.close();
    return filename;
}

std::string CService::writeSourceFile(const std::string& dirname) const
{
    SPI_PRE_CONDITION(!sharedService());

    std::string basename = spi::StringFormat("c_dll_%s.cpp", m_service->name.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), "src", basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename));
    writeLicense(ostr, license());
    startSourceFile(ostr, filename);
    if (!m_options.noGeneratedCodeNotice)
        writeGeneratedCodeNotice(ostr, filename);

    ostr << "\n"
        << "#include \"c_dll_" << m_service->name << ".h\"\n"
        << "\n"
        << "#include \"" << m_service->name << "_dll_service.hpp\"\n"
        << "\n"
        // FIXME: we may need to have a way of deleting this on demand
        << "static spi::ServiceSP g_service;\n";

    ostr << "\n"
        << "int init_" << m_service->ns << "(void)\n"
        << "{\n"
        << "    try\n"
        << "    {\n"
        << "        g_service = " << m_service->ns
        << "::" << m_service->name << "_exported_service();\n"
        << "    }\n"
        << "    catch (std::exception& e)\n"
        << "    {\n"
        << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
        << "        return -1;\n"
        << "    }\n"
        << "    return 0;\n"
        << "}\n";

    if (!m_service->sharedService)
    {
        ostr << "\n"
            << "int " << m_service->ns << "_start_logging(\n"
            << "    const char* filename,\n"
            << "    const char* options)\n"
            << "{\n"
            << "    try\n"
            << "    {\n"
            << "        if (!filename)\n"
            << "        {\n"
            << "            spi_Error_set_function(__FUNCTION__, \"NULL filename\");\n"
            << "            return -1;\n"
            << "        }\n"
            << "        if (!g_service)\n"
            << "        {\n"
            << "            if (init_" << m_service->ns << "())\n"
            << "                return -1;\n"
            << "        }\n"
            << "        SPI_POST_CONDITION(g_service);\n"
            << "        g_service->start_logging(filename, options);\n"
            << "        return 0;\n"
            << "    }\n"
            << "    catch (std::exception& e)\n"
            << "    {\n"
            << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
            << "        return -1;\n"
            << "    }\n"
            << "}\n";

        ostr << "\n"
            << "int " << m_service->ns << "_stop_logging()\n"
            << "{\n"
            << "    try\n"
            << "    {\n"
            << "        SPI_PRE_CONDITION(g_service);\n"
            << "        g_service->stop_logging();\n"
            << "        return 0;\n"
            << "    }\n"
            << "    catch (std::exception& e)\n"
            << "    {\n"
            << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
            << "        return -1;\n"
            << "    }\n"
            << "}\n";
    }

    endSourceFile(ostr, filename);
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

const std::string& CService::import() const
{
    return m_import;
}

bool CService::sharedService() const
{
    return m_service->sharedService;
}

const spdoc::ServiceConstSP& CService::service() const
{
    return m_service;
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

void CModule::updatePublicHeaderFile(GeneratedOutput& ostr) const
{
    ostr << "\n"
        << "/* Module: " << module->name << " */\n";

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
            CONSTRUCT_CAST(Enum, enumType, construct);
            declareEnum(ostr, enumType);
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
}

void CModule::updatePrivateHeaderFile(GeneratedOutput& ostr) const
{
    ostr << "\n"
        << "/* Module: " << module->name << " */\n";

    size_t nbConstructs = module->constructs.size();
    for (size_t i = 0; i < nbConstructs; ++i)
    {
        spdoc::ConstructConstSP construct = module->constructs[i];
        const std::string& constructType = construct->getType();

        if (constructType == "FUNCTION")
        {
            CONSTRUCT_CAST(Function, func, construct);
        }
        else if (constructType == "CLASS")
        {
            CONSTRUCT_CAST(Class, cls, construct);
            declarePrivateClass(ostr, cls);
        }
        else if (constructType == "ENUM")
        {
            CONSTRUCT_CAST(Enum, enumType, construct);
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
}

std::string CModule::writeSourceFile(const std::string& dirname) const
{
    std::string basename = spi::StringFormat(
        "c_%s_%s.cpp", service->ns().c_str(), module->name.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), "src", basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename));
    writeLicense(ostr, service->license());
    startSourceFile(ostr, filename);
    if (!service->noGeneratedCodeNotice())
        writeGeneratedCodeNotice(ostr, filename);

    ostr << "\n"
        << "#include \"c_dll_" << service->name() << ".hpp\"\n"
        << "\n"
        << "#include \"" << service->ns() << "_" << module->name
        << ".hpp\"\n"
        << "#include <spi/ObjectPut.hpp>\n"
        << "#include <spi_util/Utils.hpp>\n"
        << "\n"
        << "#define to_int spi_util::IntegerCast<int>\n"
        << "#define to_size_t spi_util::IntegerCast<size_t>\n";

    size_t nbConstructs = module->constructs.size();

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
        else if (constructType == "ENUM")
        {
            CONSTRUCT_CAST(Enum, enumType, construct);
            implementEnum(ostr, enumType);
        }
    }

    endSourceFile(ostr, filename);
    ostr.close();
    return filename;
}

/*
***************************************************************************
** Writer methods for functions
***************************************************************************
*/
void CModule::declareFunction(
    GeneratedOutput& ostr,
    const spdoc::Function* func,
    bool implementation) const
{
    ostr << "\n";

    if (!implementation)
        ostr << service->import() << "\n";

    std::ostringstream oss;
    oss << "int " << service->ns() << "_"
        << makeNamespaceSep(module->ns, "_") << func->name;

    std::string returnTypeAndName = oss.str();
    ostr << returnTypeAndName << "(";
    size_t pos = returnTypeAndName.size() + 1;

    std::vector<std::string> args;

    functionDeclareArgs(args, func);
    const char* sep = "";
    const char* gap = "";

    for (size_t i = 0; i < args.size(); ++i)
    {
        ostr << sep;
        pos += strlen(sep);
        std::string arg = args[i];
        if (arg.length() + pos > 4)
        {
            ostr << "\n    ";
            pos = 4;
            gap = "";
        }
        ostr << gap << arg;
        pos += strlen(gap) + arg.length();
        sep = ",";
        gap = " ";
    }
    ostr << ")";
    if (!implementation)
        ostr << ";";

    ostr << "\n";
}

void CModule::implementFunction(
    GeneratedOutput& ostr,
    const spdoc::Function* func) const
{
    declareFunction(ostr, func, true);

    const char* sep = "\n    ";
    const char* sep2 = ",\n    ";

    ostr << "{\n"
        << "    try\n"
        << "    {\n";

    for (size_t i = 0; i < func->outputs.size(); ++i)
    {
        const spdoc::AttributeConstSP& out = func->outputs[i];

        ostr << "        "
            << CDataType(out->dataType).cppType(out->arrayDim)
            << " i_" << out->name << ";\n";
    }

    if (func->returnType)
    {
        ostr << "        "
            << CDataType(func->returnType).cppType(func->returnArrayDim)
            << " i_out = ";
    }
    else
    {
        ostr << "        ";
    }

    ostr << service->ns() << "::" << makeNamespaceSep(module->ns, "::") << func->name << "(";

    sep = "\n            ";
    sep2 = ",\n            ";

    for (size_t i = 0; i < func->inputs.size(); ++i)
    {
        const spdoc::AttributeConstSP& arg = func->inputs[i];
        ostr << sep;
        sep = sep2;
        ostr << CDataType(arg->dataType).c_to_cpp(arg->arrayDim, arg->name);
    }

    for (size_t i = 0; i < func->outputs.size(); ++i)
    {
        const spdoc::AttributeConstSP& out = func->outputs[i];
        ostr << sep;
        sep = sep2;
        ostr << "i_" << out->name;
    }

    ostr << ");\n"
        << "\n";

    if (func->returnType)
    {
        ostr << "        *_out = "
            << CDataType(func->returnType).cpp_to_c(func->returnArrayDim, "i_out")
            << ";\n";
    }

    for (size_t i = 0; i < func->outputs.size(); ++i)
    {
        const spdoc::AttributeConstSP& out = func->outputs[i];
        ostr << "        *" << out->name << " = "
            << CDataType(out->dataType).cpp_to_c(out->arrayDim, "i_" + out->name)
            << ";\n";

    }

    ostr << "\n"
        << "        return 0;\n"
        << "    }\n"
        << "    catch (std::exception& e)\n"
        << "    {\n"
        << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
        << "        return -1;\n"
        << "    }\n"
        << "}\n";

}

void CModule::declareClassConstructor(
    GeneratedOutput & ostr,
    const spdoc::Class * cls,
    bool implementation) const
{
    std::ostringstream oss;
    oss << service->ns() << "_" << makeNamespaceSep(module->ns, "_") << cls->name;
    std::string cname = oss.str();

    if (!cls->noMake)
    {
        ostr << "\n";

        if (!implementation)
        {
            ostr << service->import() << "\n";
        }

        ostr << cname << "* " << cname << "_new(";
        size_t pos = 2 * cname.length() + 7;

        std::vector<std::string> args;
        for (size_t i = 0; i < cls->attributes.size(); ++i)
        {
            std::string arg = CFunctionArg(cls->attributes[i]->operator spdoc::AttributeConstSP());
            args.push_back(arg);
        }

        char* sep = "\n    ";
        char* sep2 = ",\n    ";

        for (size_t i = 0; i < args.size(); ++i)
        {
            const spdoc::ClassAttributeConstSP& attr = cls->attributes[i];
            ostr << sep;
            sep = sep2;
            ostr << args[i];
        }

        ostr << ")";
        if (!implementation)
            ostr << ";";
        ostr << "\n";
    }
}

/*
***************************************************************************
** Writer methods for classes
***************************************************************************
*/
void CModule::declareClass(
    GeneratedOutput& ostr,
    const spdoc::Class* cls) const
{
    std::ostringstream oss;
    oss << service->ns() << "_" << makeNamespaceSep(module->ns, "_") << cls->name;
    std::string cname = oss.str();

    ostr << "\n"
        << "typedef struct " << cname << " " << cname << ";\n";

    if (!cls->noMake)
    {
        declareClassConstructor(ostr, cls, false);
    }

    std::string cBaseClass;
    if (cls->baseClassName.empty())
    {
        cBaseClass = "spi_Object";
    }
    else
    {
        std::ostringstream oss2;
        oss2 << service->ns() << "_" << spi_util::StringReplace(cls->baseClassName, ".", "_");
        cBaseClass = oss2.str();
    }

    ostr << "\n"
        << "DECLARE_VECTOR_METHODS(" << cname << ", " << service->import() << ");\n"
        << "DECLARE_MATRIX_METHODS(" << cname << ", " << service->import() << ");\n";

    // FIXME - I think this should return cname*
    // actually not - a null input for spi_Object is legal and should return null
    ostr << "\n"
        << service->import() << "\n"
        << "int " << cname << "_coerce_from_object(\n"
        << "    spi_Object* o,\n"
        << "    " << cname << "** item /*(O)*/);\n";

    ostr << "\n"
        << service->import() << "\n"
        << cname << "* " << cname << "_from_string(const char* objectString);\n"
        << "\n"
        << service->import() << "\n"
        << cname << "* " << cname << "_from_file(const char* filename);\n";

    for (size_t i = 0; i < cls->coerceFrom.size(); ++i)
    {
        spdoc::CoerceFromConstSP cf = cls->coerceFrom[i];
        spdoc::AttributeConstSP cfa = cf->coerceFrom;
        spdoc::DataTypeConstSP dt = cfa->dataType;
        int arrayDim = cfa->arrayDim;

        ostr << "\n"
            << service->import() << "\n"
            << cname << "* " << cname << "_coerce_from_"
            << spi_util::StringReplace(dt->name, ".", "_");

        switch (arrayDim)
        {
        case 0:
            break;
        case 1:
            ostr << "_Vector";
            break;
        case 2:
            ostr << "_Matrix";
            break;
        default:
            SPI_THROW_RUNTIME_ERROR("ArrayDim out of range");
            break;
        }

        ostr << "(\n"
            << "    " << CFunctionArg(cfa) << ");\n";
    }

    for (size_t i = 0; i < cls->coerceTo.size(); ++i)
    {
        spdoc::CoerceToConstSP ct = cls->coerceTo[i];
        spdoc::DataTypeConstSP dt = ct->classType;

        ostr << "\n"
            << service->import() << "\n"
            << CDataType(dt).cType() << " " << cname << "_coerce_to_"
            << spi_util::StringReplace(dt->name, ".", "_");

        ostr << "(\n"
            << "    " << cname << "* self);\n";
    }

    for (size_t i = 0; i < cls->methods.size(); ++i)
    {
        const spdoc::ClassMethodConstSP& method = cls->methods[i];
        if (method->isImplementation)
            continue;

        declareClassMethod(ostr, cls, method.get());
    }

    for (size_t i = 0; i < cls->attributes.size(); ++i)
    {
        const spdoc::ClassAttributeConstSP attr = cls->attributes[i];
        if (attr->accessible)
        {
            ostr << "\n"
                << service->import() << "\n"
                << "int " << cname << "_get_" << attr->name << "(\n"
                << "    " << cname << "* self,\n"
                << "    " << CFunctionArg(attr, true) << ");\n";
        }
    }

    for (size_t i = 0; i < cls->properties.size(); ++i)
    {
        const spdoc::ClassAttributeConstSP prop = cls->properties[i];
        SPI_POST_CONDITION(prop->accessible);
        ostr << "\n"
            << service->import() << "\n"
            << "int " << cname << "_get_" << prop->name << "(\n"
            << "    " << cname << "* self,\n"
            << "    " << CFunctionArg(prop, true) << ");\n";
    }

    if (cls->canPut)
    {
        for (size_t i = 0; i < cls->attributes.size(); ++i)
        {
            const spdoc::ClassAttributeConstSP attr = cls->attributes[i];
            if (attr->accessible)
            {
                ostr << "\n"
                    << service->import() << "\n"
                    << cname << "* " << cname << "_set_" << attr->name << "(\n"
                    << "    " << cname << "* self,\n"
                    << "    " << CFunctionArg(attr) << ");\n";
            }
        }
    }

}

void CModule::declarePrivateClass(
    GeneratedOutput& ostr,
    const spdoc::Class * cls) const
{
    std::ostringstream oss;
    oss << service->ns() << "_" << makeNamespaceSep(module->ns, "_") << cls->name;
    std::string cname = oss.str();

    std::ostringstream oss2;
    oss2 << service->ns() << "::" << makeNamespaceSep(module->ns, "::") << cls->name;
    std::string cpptype = oss2.str();
    oss2 << "ConstSP";
    std::string cppname = oss2.str();
}

void CModule::implementClass(
    GeneratedOutput& ostr,
    const spdoc::Class* cls) const
{
    std::ostringstream oss;
    oss << service->ns() << "_" << makeNamespaceSep(module->ns, "_") << cls->name;
    std::string cname = oss.str();

    std::ostringstream oss2;
    oss2 << service->ns() << "::" << makeNamespaceSep(module->ns, "::") << cls->name;
    std::string cpptype = oss2.str();
    oss2 << "::outer_type";
    std::string cppname = oss2.str();

    if (!cls->noMake)
    {
        declareClassConstructor(ostr, cls, true);
        ostr << "{\n"
            << "    try\n"
            << "    {\n"
            << "        " << cppname << " self = " << cpptype << "::Make(";
        
        const char* sep = "\n            ";
        const char* sep2 = ",\n            ";

        for (size_t i = 0; i < cls->attributes.size(); ++i)
        {
            const spdoc::ClassAttributeConstSP& attr = cls->attributes[i];
            ostr << sep;
            sep = sep2;
            ostr << CDataType(attr->dataType).c_to_cpp(attr->arrayDim, attr->name);
        }

        ostr << ");\n"
            << "\n";

        ostr << "        return spi::convert_out<" << cname << ">(self);\n"
            << "    }\n"
            << "    catch (std::exception& e)\n"
            << "    {\n"
            << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
            << "        return nullptr;\n"
            << "    }\n"
            << "}\n";
    }

    std::string cBaseClass;
    if (cls->baseClassName.empty())
    {
        cBaseClass = "spi_Object";
    }
    else
    {
        std::ostringstream oss2;
        oss2 << service->ns() << "_" << spi_util::StringReplace(cls->baseClassName, ".", "_");
        cBaseClass = oss2.str();
    }

    ostr << "\n"
        << "void " << cname << "_Vector_delete(" << cname << "_Vector* v)\n"
        << "{\n"
        << "    spi::Vector_delete<" << cpptype << ">(v);\n"
        << "}\n";

    ostr << "\n"
        << cname << "_Vector* " << cname << "_Vector_new(int N)\n"
        << "{\n"
        << "    return spi::Vector_new<" << cpptype << ", " << cname << "_Vector>(N);\n"
        << "}\n";

#if 0
    ostr << "\n"
        << "int " << cname << "_Vector_item(\n"
        << "    " << cname << "_Vector* v,\n"
        << "    int i,\n"
        << "    " << cname << "** item)\n"
        << "{\n"
        << "    return spi::Vector_item<" << cpptype << ">(v, i, item);\n"
        << "}\n";

    ostr << "\n"
        << "int " << cname << "_Vector_set_item(\n"
        << "    " << cname << "_Vector* v,\n"
        << "    int i,\n"
        << "    " << cname << "* item)\n"
        << "{\n"
        << "    return spi::Vector_set_item<" << cpptype << ">(v, i, item);\n"
        << "}\n";

    ostr << "\n"
        << "int " << cname << "_Vector_size(\n"
        << "    " << cname << "_Vector* v,\n"
        << "    int* size)\n"
        << "{\n"
        << "    return spi::Vector_size<" << cpptype << ">(v, size);\n"
        << "}\n";
#endif

    ostr << "\n"
        << "void " << cname << "_Matrix_delete(" << cname << "_Matrix* m)\n"
        << "{\n"
        << "    spi::Matrix_delete<" << cpptype << ">(m);\n"
        << "}\n";

    ostr << "\n"
        << cname << "_Matrix* " << cname << "_Matrix_new(int nr, int nc)\n"
        << "{\n"
        << "    return spi::Matrix_new<" << cpptype << ", " << cname << "_Matrix>(nr, nc);\n"
        << "}\n";

#if 0
    ostr << "\n"
        << "int " << cname << "_Matrix_item(\n"
        << "    " << cname << "_Matrix* m,\n"
        << "    int i, int j,\n"
        << "    " << cname << "** item)\n"
        << "{\n"
        << "    return spi::Matrix_item<" << cpptype << ">(m, i, j, item);\n"
        << "}\n";

    ostr << "\n"
        << "int " << cname << "_Matrix_set_item(\n"
        << "    " << cname << "_Matrix* m,\n"
        << "    int i, int j,\n"
        << "    " << cname << "* item)\n"
        << "{\n"
        << "    return spi::Matrix_set_item<" << cpptype << ">(m, i, j, item);\n"
        << "}\n";

    ostr << "\n"
        << "int " << cname << "_Matrix_size(\n"
        << "    " << cname << "_Matrix* m,\n"
        << "    int* nr, int* nc)\n"
        << "{\n"
        << "    return spi::Matrix_size<" << cpptype << ">(m, nr, nc);\n"
        << "}\n";
#endif

    ostr << "\n"
        << "int " << cname << "_coerce_from_object(\n"
        << "    spi_Object* o,\n"
        << "    " << cname << "** item)\n"
        << "{\n"
        << "    if (!item)\n"
        << "    {\n"
        << "        spi_Error_set_function(__FUNCTION__, \"NULL pointer\");\n"
        << "        return -1;\n"
        << "    }\n"
        << "\n"
        << "    try\n"
        << "    {\n"
        << "        if (!o)\n"
        << "        {\n"
        << "            *item = nullptr;\n"
        << "        }\n"
        << "        else\n"
        << "        {\n"
        << "            *item = spi::convert_out<" << cname << ">(\n"
        << "                " << cpptype << "::Coerce(spi::ObjectConstSP((spi::Object*)o)));\n"
        << "        }\n"
        << "        return 0;\n"
        << "    }\n"
        << "    catch (std::exception& e)\n"
        << "    {\n"
        << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
        << "        return -1;\n"
        << "    }\n"
        << "}\n";

    ostr << "\n"
        << cname << "* " << cname << "_from_string(const char* objectString)\n"
        << "{\n"
        << "    try\n"
        << "    {\n"
        << "        if (!objectString)\n"
        << "        {\n"
        << "             spi_Error_set_function(__FUNCTION__, \"NULL inputs\");\n"
        << "             return nullptr;\n"
        << "        }\n"
        << "        spi::ObjectConstSP o = spi::Service::CommonService()->object_from_string(objectString);\n"
        << "        " << cppname << " obj = " << cpptype << "::Coerce(o);\n"
        << "        return spi::convert_out<" << cname << ">(obj);\n"
        << "    }\n"
        << "    catch (std::exception& e)\n"
        << "    {\n"
        << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
        << "        return nullptr;\n"
        << "    }\n"
        << "}\n"
        << "\n"
        << cname << "* " << cname << "_from_file(const char* filename)\n"
        << "{\n"
        << "    try\n"
        << "    {\n"
        << "        if (!filename)\n"
        << "        {\n"
        << "             spi_Error_set_function(__FUNCTION__, \"NULL inputs\");\n"
        << "             return nullptr;\n"
        << "        }\n"
        << "        spi::ObjectConstSP o = spi::Service::CommonService()->object_from_file(filename);\n"
        << "        " << cppname << " obj = " << cpptype << "::Coerce(o);\n"
        << "        return spi::convert_out<" << cname << ">(obj);\n"
        << "    }\n"
        << "    catch (std::exception& e)\n"
        << "    {\n"
        << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
        << "        return nullptr;\n"
        << "    }\n"
        << "}\n";

    for (size_t i = 0; i < cls->coerceFrom.size(); ++i)
    {
        spdoc::CoerceFromConstSP cf = cls->coerceFrom[i];
        spdoc::AttributeConstSP cfa = cf->coerceFrom;
        spdoc::DataTypeConstSP dt = cfa->dataType;
        int arrayDim = cfa->arrayDim;

        ostr << "\n"
            << cname << "* " << cname << "_coerce_from_"
            << spi_util::StringReplace(dt->name, ".", "_");

        switch (arrayDim)
        {
        case 0:
            break;
        case 1:
            ostr << "_Vector";
            break;
        case 2:
            ostr << "_Matrix";
            break;
        default:
            SPI_THROW_RUNTIME_ERROR("ArrayDim out of range");
            break;
        }

        CDataType cdt(dt);

        ostr << "(\n"
            << "    " << CFunctionArg(cfa) << ")\n";

        ostr << "{\n"
            << "    try\n"
            << "    {\n"
            << "        return spi::convert_out<" << cname << ">(\n"
            << "            " << cpptype << "::Coerce("
            << cdt.c_to_cpp(arrayDim, cfa->name) << "));\n"
            << "    }\n"
            << "    catch (std::exception& e)\n"
            << "    {\n"
            << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
            << "        return nullptr;\n"
            << "    }\n"
            << "}\n";
    }

    // FIXME
    for (size_t i = 0; i < cls->coerceTo.size(); ++i)
    {
        spdoc::CoerceToConstSP ct = cls->coerceTo[i];
        spdoc::DataTypeConstSP dt = ct->classType;

        CDataType cdt(dt);

        ostr << "\n"
            << cdt.cType() << " " << cname << "_coerce_to_"
            << spi_util::StringReplace(dt->name, ".", "_");

        ostr << "(\n"
            << "    " << cname << "* self)\n";

        ostr << "{\n"
            << "    try\n"
            << "    {\n"
            << "        " << cdt.cppType(0) << " out = \n"
            << "            (*(" << cppname << "*)(self))->operator " << cdt.cppType(0) << "();\n"
            << "        return " << cdt.cpp_to_c(0, "out") << ";\n"
            << "    }\n"
            << "    catch (std::exception& e)\n"
            << "    {\n"
            << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
            << "        return nullptr;\n"
            << "    }\n"
            << "}\n";
    }

    for (size_t i = 0; i < cls->methods.size(); ++i)
    {
        const spdoc::ClassMethodConstSP& method = cls->methods[i];
        if (method->isImplementation)
            continue;

        implementClassMethod(ostr, cls, method.get());
    }

    // FIXME: delegate classes

    for (size_t i = 0; i < cls->attributes.size(); ++i)
    {
        const spdoc::ClassAttributeConstSP attr = cls->attributes[i];
        if (attr->accessible)
        {
            CDataType cdt(attr->dataType);
            ostr << "\n"
                << "int " << cname << "_get_" << attr->name << "(\n"
                << "    " << cname << "* self,\n"
                << "    " << CFunctionArg(attr, true) << ")\n";

            ostr << "{\n"
                << "    try\n"
                << "    {\n"
                << "        if (!self || !" << attr->name << ")\n"
                << "        {\n"
                << "            SPI_THROW_RUNTIME_ERROR(\"NULL inputs\");\n"
                << "        }\n"
                << "\n"
                << "        " << cdt.cppType(attr->arrayDim) << " i_" << attr->name
                << " = ((" << cpptype << "*)self)->" << attr->accessor << ";\n"
                << "\n"
                << "        *" << attr->name << " = " 
                << cdt.cpp_to_c(attr->arrayDim, "i_" + attr->name) << ";\n"
                << "        return 0;\n"
                << "    }\n"
                << "    catch (std::exception& e)\n"
                << "    {\n"
                << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
                << "        return -1;\n"
                << "    }\n"
                << "}\n";
        }
    }

    for (size_t i = 0; i < cls->properties.size(); ++i)
    {
        const spdoc::ClassAttributeConstSP prop = cls->properties[i];
        SPI_POST_CONDITION(prop->accessible);
        CDataType cdt(prop->dataType);
        ostr << "\n"
            << "int " << cname << "_get_" << prop->name << "(\n"
            << "    " << cname << "* self,\n"
            << "    " << CFunctionArg(prop, true) << ")\n";

        ostr << "{\n"
            << "    try\n"
            << "    {\n"
            << "        if (!self || !" << prop->name << ")\n"
            << "        {\n"
            << "            SPI_THROW_RUNTIME_ERROR(\"NULL inputs\");\n"
            << "        }\n"
            << "\n"
            << "        " << cdt.cppType(prop->arrayDim) << " i_" << prop->name
            << " = ((" << cpptype << "*)self)->" << prop->accessor << ";\n"
            << "\n"
            << "        *" << prop->name << " = "
            << cdt.cpp_to_c(prop->arrayDim, "i_" + prop->name) << ";\n"
            << "        return 0;\n"
            << "    }\n"
            << "    catch (std::exception& e)\n"
            << "    {\n"
            << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
            << "        return -1;\n"
            << "    }\n"
            << "}\n";
    }

    if (cls->canPut)
    {
        for (size_t i = 0; i < cls->attributes.size(); ++i)
        {
            const spdoc::ClassAttributeConstSP attr = cls->attributes[i];
            if (attr->accessible)
            {
                CDataType cdt(attr->dataType);
                ostr << "\n"
                    << cname << "* " << cname << "_set_" << attr->name << "(\n"
                    << "    " << cname << "* self,\n"
                    << "    " << CFunctionArg(attr, false) << ")\n";

                ostr << "{\n"
                    << "    try\n"
                    << "    {\n"
                    << "        if (!self)\n"
                    << "        {\n"
                    << "            SPI_THROW_RUNTIME_ERROR(\"NULL inputs\");\n"
                    << "        }\n"
                    << "\n"
                    << "        spi::ObjectConstSP _obj = spi::ObjectPut(\n"
                    << "            spi::ObjectConstSP((spi::Object*)self),\n"
                    << "            \"" << attr->name << "\",\n"
                    << "            " << cdt.c_to_value(attr->arrayDim, attr->name) << ",\n"
                    << "            NULL);\n"
                    << "        return spi::convert_out<" << cname << ">(" << cpptype << "::Coerce(_obj));\n"
                    << "    }\n"
                    << "    catch (std::exception& e)\n"
                    << "    {\n"
                    << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
                    << "        return nullptr;\n"
                    << "    }\n"
                    << "}\n";
            }
        }
    }
}

void CModule::declareClassMethod(GeneratedOutput & ostr,
    const spdoc::Class* cls,
    const spdoc::ClassMethod* method,
    bool implementation) const
{
    std::ostringstream oss;
    oss << service->ns() << "_" << makeNamespaceSep(module->ns, "_") << cls->name;
    std::string cname = oss.str();

    const spdoc::FunctionConstSP func = method->function;
    ostr << "\n";
    if (!implementation)
    {
        ostr << service->import() << "\n";
    }

    std::ostringstream oss2;
    oss2 << "int " << service->ns() << "_"
        << makeNamespaceSep(module->ns, "_") 
        << cls->name << "_" << func->name;
    std::string returnTypeAndName = oss2.str();

    std::vector<std::string> args;

    if (!method->isStatic)
    {
        args.push_back(cname + "* self");
    }

    functionDeclareArgs(args, func.get());

    ostr << returnTypeAndName << "(";
    size_t pos = returnTypeAndName.size() + 1;

    const char* sep = "";
    const char* gap = "";

    for (size_t i = 0; i < args.size(); ++i)
    {
        ostr << sep;
        pos += strlen(sep);
        std::string arg = args[i];
        if (arg.length() + pos > 4)
        {
            ostr << "\n    ";
            pos = 4;
            gap = "";
        }
        ostr << gap << arg;
        pos += strlen(gap) + arg.length();
        sep = ",";
        gap = " ";
    }

    ostr << ")";
    if (!implementation)
        ostr << ";";
    ostr << "\n";

}

void CModule::implementClassMethod(
    GeneratedOutput& ostr, 
    const spdoc::Class* cls, 
    const spdoc::ClassMethod* method) const
{
    std::ostringstream oss;
    oss << service->ns() << "_" << makeNamespaceSep(module->ns, "_") << cls->name;
    std::string cname = oss.str();

    std::ostringstream oss2;
    oss2 << service->ns() << "::" << makeNamespaceSep(module->ns, "::") << cls->name;
    std::string cpptype = oss2.str();
    oss2 << "ConstSP";
    std::string cppname = oss2.str();

    const spdoc::FunctionConstSP func = method->function;

    declareClassMethod(ostr, cls, method, true);
    ostr << "{\n"
        << "    try\n"
        << "    {\n";

    if (!method->isStatic)
    {
        ostr << "        if (!self)\n"
            << "        {\n"
            << "            spi_Error_set_function(__FUNCTION__, \"NULL inputs\");\n"
            << "            return -1;\n"
            << "        }\n";
    }

    for (size_t i = 0; i < func->outputs.size(); ++i)
    {
        const spdoc::AttributeConstSP& out = func->outputs[i];

        ostr << "        "
            << CDataType(out->dataType).cppType(out->arrayDim)
            << " i_" << out->name << ";\n";
    }

    if (func->returnType)
    {
        ostr << "        "
            << CDataType(func->returnType).cppType(func->returnArrayDim)
            << " i_out = ";
    }
    else
    {
        ostr << "        ";
    }

    if (method->isStatic)
    {
        ostr << cpptype << "::" << func->name << "(";
    }
    else
    {
        ostr << "((" << cpptype << "*)self)->" << func->name << "(";
    }

    const char* sep = "\n            ";
    const char* sep2 = ",\n            ";

    for (size_t i = 0; i < func->inputs.size(); ++i)
    {
        const spdoc::AttributeConstSP& arg = func->inputs[i];
        ostr << sep;
        sep = sep2;
        ostr << CDataType(arg->dataType).c_to_cpp(arg->arrayDim, arg->name);
    }

    for (size_t i = 0; i < func->outputs.size(); ++i)
    {
        const spdoc::AttributeConstSP& out = func->outputs[i];
        ostr << sep;
        sep = sep2;
        ostr << "i_" << out->name;
    }

    ostr << ");\n"
        << "\n";

    if (func->returnType)
    {
        ostr << "        *_out = "
            << CDataType(func->returnType).cpp_to_c(func->returnArrayDim, "i_out")
            << ";\n";
    }

    for (size_t i = 0; i < func->outputs.size(); ++i)
    {
        const spdoc::AttributeConstSP& out = func->outputs[i];
        ostr << "        *" << out->name << " = "
            << CDataType(out->dataType).cpp_to_c(out->arrayDim, "i_" + out->name)
            << ";\n";

    }

    ostr << "\n"
        << "        return 0;\n"
        << "    }\n"
        << "    catch (std::exception& e)\n"
        << "    {\n"
        << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
        << "        return -1;\n"
        << "    }\n"
        << "}\n";
}

void CModule::declareEnum(GeneratedOutput& ostr, 
    const spdoc::Enum * enumType) const
{
    std::ostringstream oss;
    oss << service->ns() << "_" << makeNamespaceSep(module->ns, "_") << enumType->name;
    std::string cname = oss.str();

    ostr << "\n"
        << "enum class " << cname << " {";

    const char* sep = "\n    ";
    for (size_t i = 0; i < enumType->enumerands.size(); ++i)
    {
        ostr << sep << /* cname << "_" << */ enumType->enumerands[i]->code;
        sep = ",\n    ";
    }
    ostr << "};\n";

    ostr << "typedef struct _" << cname << "_Vector " << cname << "_Vector;\n";
    ostr << "typedef struct _" << cname << "_Matrix " << cname << "_Matrix;\n";

    ostr << "\n"
        << service->import() << "\n"
        << "void " << cname << "_Vector_delete(" << cname << "_Vector* v);\n"
        << "\n"
        << service->import() << "\n"
        << cname << "_Vector* " << cname << "_Vector_new(int size);\n";

#if 0
     ostr << "\n"
         << service->import() << "\n"
        << "int " << cname << "_Vector_item(\n"
        << "    const " << cname << "_Vector* v,\n"
        << "    int i,\n"
        << "    " << cname << "* item /*(O)*/);\n"
        << "\n"
        << service->import() << "\n"
        << "int " << cname << "_Vector_set_item(\n"
        << "    " << cname << "_Vector* v,\n"
        << "    int i,\n"
        << "    " << cname << " item);\n"
        << "\n"
        << service->import() << "\n"
        << "int " << cname << "_Vector_size(\n"
        << "    const " << cname << "_Vector* v,\n"
        << "    int* size /*(O)*/);\n";
#endif

     ostr << "\n"
         << service->import() << "\n"
         << "void " << cname << "_Matrix_delete("
         << cname << "_Matrix* m);\n"
         << "\n"
         << service->import() << "\n"
         << cname << "_Matrix* " << cname << "_Matrix_new(int nr, int nc);\n";

#if 0
     ostr << "\n"
         << service->import() << "\n"; 
         << "int " << cname << "_Matrix_item(\n"
        << "    const " << cname << "_Matrix* m,\n"
        << "    int r, int c,\n"
        << "    " << cname << "* item /*(O)*/);\n"
        << "\n"
        << service->import() << "\n"
        << "int " << cname << "_Matrix_set_item(\n"
        << "    " << cname << "_Matrix* m,\n"
        << "    int r, int c,\n"
        << "    " << cname << " item);\n"
        << "\n"
        << service->import() << "\n"
        << "int " << cname << "_Matrix_size(\n"
        << "    const " << cname << "_Matrix*,\n"
        << "    int* nr /*(O)*/,\n"
        << "    int* nc /*(O)*/);\n";
#endif
}

void CModule::implementEnum(GeneratedOutput & ostr,
    const spdoc::Enum* enumType) const
{
    std::ostringstream oss;
    oss << service->ns() << "_" << makeNamespaceSep(module->ns, "_") << enumType->name;
    std::string cname = oss.str();

    std::ostringstream oss2;
    oss2 << service->ns() << "::" << makeNamespaceSep(module->ns, "::") << enumType->name;
    std::string cppname = oss2.str();

    ostr << "\n"
        << "void " << cname << "_Vector_delete(" << cname << "_Vector* c)\n"
        << "{\n"
        << "    if (c)\n"
        << "    {\n"
        << "        auto cpp = (std::vector<" << cppname << ">*)(c);\n"
        << "        delete cpp;\n"
        << "    }\n"
        << "}\n";

    ostr << "\n"
        << cname << "_Vector* " << cname << "_Vector_new(int N)\n"
        << "{\n"
        << "    try\n"
        << "    {\n"
        << "        auto out = new std::vector<" << cppname << ">(to_size_t(N));\n"
        << "        return (" << cname << "_Vector*)(out);\n"
        << "    }\n"
        << "    catch (std::exception& e)\n"
        << "    {\n"
        << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
        << "        return nullptr;\n"
        << "    }\n"
        << "}\n";

#if 0
    ostr << "\n"
        << "int " << cname << "_Vector_item(\n"
        << "    const " << cname << "_Vector* v,\n"
        << "    int ii,\n"
        << "    " << cname << "* item)\n"
        << "{\n"
        << "    if (!v || !item)\n"
        << "    {\n"
        << "        spi_Error_set_function(__FUNCTION__, \"NULL pointer\");\n"
        << "        return -1;\n"
        << "    }\n"
        << "\n"
        << "    try\n"
        << "    {\n"
        << "        auto cpp = (const std::vector<" << cppname << ">*)(v);\n"
        << "        size_t i = to_size_t(ii);\n"
        << "        if (i >= cpp->size())\n"
        << "        {\n"
        << "             spi_Error_set_function(__FUNCTION__, \"Array bounds mismatch\");\n"
        << "             return -1;\n"
        << "        }\n"
        << "        *item = (" << cname << ")((" << cppname << "::Enum)((*cpp)[i]));\n"
        << "        return 0;\n"
        << "    }\n"
        << "    catch (std::exception& e)\n"
        << "    {\n"
        << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
        << "        return -1;\n"
        << "    }\n"
        << "}\n";

    ostr << "\n"
        << "int " << cname << "_Vector_set_item(\n"
        << "    " << cname << "_Vector* v,\n"
        << "    int ii,\n"
        << "    " << cname << " item)\n"
        << "{\n"
        << "    if (!v)\n"
        << "    {\n"
        << "        spi_Error_set_function(__FUNCTION__, \"NULL pointer\");\n"
        << "        return -1;\n"
        << "    }\n"
        << "\n"
        << "    try\n"
        << "    {\n"
        << "        auto cpp = (std::vector<" << cppname << ">*)(v);\n"
        << "        size_t i = to_size_t(ii);\n"
        << "        if (i >= cpp->size())\n"
        << "        {\n"
        << "             spi_Error_set_function(__FUNCTION__, \"Array bounds mismatch\");\n"
        << "             return -1;\n"
        << "        }\n"
        << "        (*cpp)[i] = (" << cppname << "::Enum)(item);\n"
        << "        return 0;\n"
        << "    }\n"
        << "    catch (std::exception& e)\n"
        << "    {\n"
        << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
        << "        return -1;\n"
        << "    }\n"
        << "}\n";

    ostr << "\n"
        << "int " << cname << "_Vector_size(\n"
        << "    const " << cname << "_Vector* v,\n"
        << "    int* size)\n"
        << "{\n"
        << "    if (!v)\n"
        << "        return 0;\n"
        << "\n"
        << "    auto cpp = (const std::vector<" << cppname << ">*)(v);\n"
        << "    *size = to_int(cpp->size());\n"
        << "    return 0;\n"
        << "}\n";
#endif

    ostr << "\n"
        << "void " << cname << "_Matrix_delete(" << cname << "_Matrix* c)\n"
        << "{\n"
        << "    if (c)\n"
        << "    {\n"
        << "        auto cpp = (spi::MatrixData<" << cppname << ">*)(c);\n"
        << "        delete cpp;\n"
        << "    }\n"
        << "}\n";

    ostr << "\n"
        << cname << "_Matrix* " << cname << "_Matrix_new(int nr, int nc)\n"
        << "{\n"
        << "    try\n"
        << "    {\n"
        << "        auto out = new spi::MatrixData<" << cppname << ">(to_size_t(nr), to_size_t(nc));\n"
        << "        return (" << cname << "_Matrix*)(out);\n"
        << "    }\n"
        << "    catch (std::exception& e)\n"
        << "    {\n"
        << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
        << "        return nullptr;\n"
        << "    }\n"
        << "}\n";

#if 0
    ostr << "\n"
        << "int " << cname << "_Matrix_item(\n"
        << "    const " << cname << "_Matrix* m,\n"
        << "    int ir, int ic,\n"
        << "    " << cname << "* item)\n"
        << "{\n"
        << "    if (!m || !item)\n"
        << "    {\n"
        << "        spi_Error_set_function(__FUNCTION__, \"NULL pointer\");\n"
        << "        return -1;\n"
        << "    }\n"
        << "\n"
        << "    try\n"
        << "    {\n"
        << "        auto cpp = (const spi::MatrixData<" << cppname << ">*)(m);\n"
        << "        size_t r = to_size_t(ir);\n"
        << "        size_t c = to_size_t(ic);\n"
        << "        if (r >= cpp->Rows() || c >= cpp->Cols())\n"
        << "        {\n"
        << "             spi_Error_set_function(__FUNCTION__, \"Array bounds mismatch\");\n"
        << "             return -1;\n"
        << "        }\n"
        << "        *item = (" << cname << ")((" << cppname << "::Enum)((*cpp)[r][c]));\n"
        << "        return 0;\n"
        << "    }\n"
        << "    catch (std::exception& e)\n"
        << "    {\n"
        << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
        << "        return -1;\n"
        << "    }\n"
        << "}\n";

    ostr << "\n"
        << "int " << cname << "_Matrix_set_item(\n"
        << "    " << cname << "_Matrix* m,\n"
        << "    int ir, int ic,\n"
        << "    " << cname << " item)\n"
        << "{\n"
        << "    if (!m)\n"
        << "    {\n"
        << "        spi_Error_set_function(__FUNCTION__, \"NULL pointer\");\n"
        << "        return -1;\n"
        << "    }\n"
        << "\n"
        << "    try\n"
        << "    {\n"
        << "        auto cpp = (spi::MatrixData<" << cppname << ">*)(m);\n"
        << "        size_t r = to_size_t(ir);\n"
        << "        size_t c = to_size_t(ic);\n"
        << "        if (r >= cpp->Rows() || c << cpp->Cols())\n"
        << "        {\n"
        << "             spi_Error_set_function(__FUNCTION__, \"Array bounds mismatch\");\n"
        << "             return -1;\n"
        << "        }\n"
        << "        (*cpp)[r][c] = (" << cppname << "::Enum)(item);\n"
        << "        return 0;\n"
        << "    }\n"
        << "    catch (std::exception& e)\n"
        << "    {\n"
        << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
        << "        return -1;\n"
        << "    }\n"
        << "}\n";

    ostr << "\n"
        << "int " << cname << "_Matrix_size(\n"
        << "    const " << cname << "_Matrix* m,\n"
        << "    int* nr, int* nc)\n"
        << "{\n"
        << "    if (!nr || !nc)\n"
        << "    {\n"
        << "        spi_Error_set_function(__FUNCTION__, \"NULL pointer\");\n"
        << "        return -1;\n"
        << "    }\n"
        << "\n"
        << "    if (!m)\n"
        << "    {\n"
        << "        *nr = 0;\n"
        << "        *nc = 0;\n"
        << "        return 0;\n"
        << "    }\n"
        << "\n"
        << "    try\n"
        << "    {\n"
        << "        auto cpp = (const spi::MatrixData<" << cppname << ">*)(m);\n"
        << "        *nr = to_int(cpp->Rows());\n"
        << "        *nc = to_int(cpp->Cols());\n"
        << "        return 0;\n"
        << "    }\n"
        << "    catch (std::exception& e)\n"
        << "    {\n"
        << "        spi_Error_set_function(__FUNCTION__, e.what());\n"
        << "        return -1;\n"
        << "    }\n"
        << "}\n";
#endif
}

void CModule::functionDeclareArgs(
    std::vector<std::string>& args,
    const spdoc::Function* func) const
{
    for (size_t i = 0; i < func->inputs.size(); ++i)
    {
        std::string arg = CFunctionArg(func->inputs[i]);
        args.push_back(arg);
    }

    if (func->returnType)
    {
        std::string arg = CFunctionArg(ReturnTypeAttribute(
            func->returnType, func->returnArrayDim), true);
        args.push_back(arg);
    }

    for (size_t i = 0; i < func->outputs.size(); ++i)
    {
        std::string arg = CFunctionArg(func->outputs[i], true);
        args.push_back(arg);
    }
}

CDataType::CDataType(const spdoc::DataTypeConstSP& dataType) 
    : 
    dataType(dataType),
    publicType(dataType->publicType)
{
    
}

std::string CDataType::cType(int arrayDim) const
{
    bool useConst = false;
    std::string scalarType;
    std::string arrayType;
    std::string cppType;
    std::string Const = useConst ? std::string("const ") : std::string();

    switch (publicType)
    {
    case spdoc::PublicType::BOOL:
        scalarType = "spi_Bool";
        arrayType = "spi_Bool";
        break;
    case spdoc::PublicType::CHAR:
        scalarType = "char";
        break;
    case spdoc::PublicType::INT:
        scalarType = "int";
        arrayType = "spi_Int";
        break;
    case spdoc::PublicType::DOUBLE:
        scalarType = "double";
        arrayType = "spi_Double";
        break;
    case spdoc::PublicType::STRING:
        scalarType = "char*";
        arrayType = "spi_String";
        break;
    case spdoc::PublicType::DATE:
        scalarType = "spi_Date";
        arrayType = "spi_Date";
        break;
    case spdoc::PublicType::DATETIME:
        scalarType = "spi_DateTime";
        arrayType = "spi_DateTime";
        break;
    case spdoc::PublicType::ENUM:
        scalarType = spi_util::StringFormat("%s_%s",
            dataType->nsService.c_str(),
            spi_util::StringReplace(dataType->name, ".", "_").c_str());
        arrayType = scalarType;
        break;
    case spdoc::PublicType::CLASS:
        cppType = spi_util::StringFormat("%s_%s",
            dataType->nsService.c_str(),
            spi_util::StringReplace(dataType->name, ".", "_").c_str());
        scalarType = cppType + "*";
        arrayType = cppType;
        break;
    case spdoc::PublicType::OBJECT:
        scalarType = "spi_Object*";
        arrayType = "spi_Object";
        break;
    case spdoc::PublicType::MAP:
        scalarType = "spi_Map*";
        arrayType = "spi_Map";
        break;
    case spdoc::PublicType::VARIANT:
        scalarType = "spi_Variant*";
        arrayType = "spi_Variant";
        break;
    default:
        break;
    }

    switch (arrayDim)
    {
    case 0:
        if (scalarType.empty())
            SPI_THROW_RUNTIME_ERROR("Scalars not implemented for " <<
                spdoc::PublicType::to_string(publicType));
        if (spi_util::StringEndsWith(scalarType, "*"))
            return Const + scalarType;
        return scalarType;
    case 1:
        if (arrayType.empty())
            SPI_THROW_RUNTIME_ERROR("Vectors not implemented for " <<
                spdoc::PublicType::to_string(publicType));
        return spi_util::StringFormat("%s%s_Vector*", Const.c_str(), arrayType.c_str());
    case 2:
        if (arrayType.empty())
            SPI_THROW_RUNTIME_ERROR("Matrix's not implemented for " <<
                spdoc::PublicType::to_string(publicType));
        return spi_util::StringFormat("%s%s_Matrix*", Const.c_str(), arrayType.c_str());
    default:
        SPI_THROW_RUNTIME_ERROR("ArrayDim " << arrayDim << " out of range [0,2]");
    }
}

std::string CDataType::cppName() const
{
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
        return "std::string";
    case spdoc::PublicType::DATE:
        return "spi::Date";
    case spdoc::PublicType::DATETIME:
        return "spi::DateTime";
    case spdoc::PublicType::ENUM:
        return spi_util::StringFormat("%s::%s",
            dataType->nsService.c_str(),
            spi_util::StringReplace(dataType->name, ".", "::").c_str());
    case spdoc::PublicType::CLASS:
        return spi_util::StringFormat("%s::%s",
            dataType->nsService.c_str(),
            spi_util::StringReplace(dataType->name, ".", "::").c_str());
    case spdoc::PublicType::OBJECT:
        return "sp::Object";
    case spdoc::PublicType::MAP:
        return "spi::MapObject";
    case spdoc::PublicType::VARIANT:
        return "spi::Variant";
    default:
        SPI_THROW_RUNTIME_ERROR("Not implemented for " <<
            spdoc::PublicType::to_string(publicType));
        break;
    }

}

std::string CDataType::cppType(int arrayDim) const
{
    std::string cppType = dataType->ValueType();

    switch (arrayDim)
    {
    case 0:
        return cppType;
    case 1:
        return spi_util::StringFormat("std::vector< %s >", cppType.c_str());
    case 2:
        return spi_util::StringFormat("spi::MatrixData< %s >", cppType.c_str());
    default:
        SPI_THROW_RUNTIME_ERROR("ArrayDim " << arrayDim << " out of range [0,2]");
    }
}

std::string CDataType::c_to_cpp(int arrayDim, const std::string& name) const
{
    std::string cppType = dataType->ValueType();

    switch (arrayDim)
    {
    case 0:
    {
        switch (publicType)
        {
        case spdoc::PublicType::INT:
        case spdoc::PublicType::DOUBLE:
        case spdoc::PublicType::CHAR:
            return name;
        case spdoc::PublicType::BOOL:
            return spi_util::StringFormat("%s != SPI_FALSE", name.c_str());
        case spdoc::PublicType::STRING:
            return spi_util::StringFormat("%s ? std::string(%s) : std::string()",
                name.c_str(), name.c_str());
        case spdoc::PublicType::DATE:
            return spi_util::StringFormat("spi::Date(%s)", name.c_str());
        case spdoc::PublicType::DATETIME:
            return spi_util::StringFormat("spi::DateTime(%s)", name.c_str());
        case spdoc::PublicType::ENUM:
            return spi_util::StringFormat("%s((%s::Enum)(%s))",
                cppType.c_str(), cppType.c_str(), name.c_str());
        case spdoc::PublicType::CLASS:
        {
            std::string cname = spi_util::StringFormat("%s_%s",
                dataType->nsService.c_str(),
                spi_util::StringReplace(dataType->name, ".", "_").c_str());

            return spi_util::StringFormat("spi::convert_in<%s>(%s)",
                cppName().c_str(), name.c_str());
        }
        case spdoc::PublicType::OBJECT:
            return spi_util::StringFormat("*(spi::ObjectConstSP*)(%s)", name.c_str());
        case spdoc::PublicType::MAP:
            return spi_util::StringFormat("spi::MapObjectSP((spi::MapObject*)(%s))",
                name.c_str());
        case spdoc::PublicType::VARIANT:
            return StringFormat("*((spi::Variant*)(%s))", name.c_str());
        default:
            SPI_THROW_RUNTIME_ERROR("SCALAR " <<
                spdoc::PublicType::to_string(publicType) << " not implemented");
        }
        break;
    }
    case 1:
    {
        switch (publicType)
        {
        case spdoc::PublicType::CHAR:
            SPI_THROW_RUNTIME_ERROR("Array type not supported for " <<
                spdoc::PublicType::to_string(publicType));
        case spdoc::PublicType::INT:
        case spdoc::PublicType::DOUBLE:
        case spdoc::PublicType::BOOL:
        case spdoc::PublicType::STRING:
        case spdoc::PublicType::DATE:
        case spdoc::PublicType::DATETIME:
        case spdoc::PublicType::ENUM:
        case spdoc::PublicType::CLASS:
        case spdoc::PublicType::OBJECT:
        case spdoc::PublicType::VARIANT:
            return spi_util::StringFormat("*(const std::vector< %s >*)(%s)",
                cppType.c_str(), name.c_str());
        default:
            SPI_THROW_RUNTIME_ERROR("ARRAY " <<
                spdoc::PublicType::to_string(publicType) << " not implemented");
        }
        break;
    }
    case 2:
    {
        switch (publicType)
        {
        case spdoc::PublicType::CHAR:
            SPI_THROW_RUNTIME_ERROR("Matrix type not supported for " <<
                spdoc::PublicType::to_string(publicType));
        case spdoc::PublicType::INT:
        case spdoc::PublicType::DOUBLE:
        case spdoc::PublicType::BOOL:
        case spdoc::PublicType::STRING:
        case spdoc::PublicType::DATE:
        case spdoc::PublicType::DATETIME:
        case spdoc::PublicType::ENUM:
        case spdoc::PublicType::CLASS:
        case spdoc::PublicType::OBJECT:
        case spdoc::PublicType::VARIANT:
            return spi_util::StringFormat("*(const spi::MatrixData< %s >*)(%s)",
                cppType.c_str(), name.c_str());
        default:
            SPI_THROW_RUNTIME_ERROR("MATRIX " <<
                spdoc::PublicType::to_string(publicType) << " not implemented");
        }
        break;
    }
    default:
        SPI_THROW_RUNTIME_ERROR("Bad value of arrayDim " << arrayDim
            << " should be 0,1 or 2");
    }

    SPI_THROW_RUNTIME_ERROR("Not implemented for " << cppType << "(" << arrayDim << ")");

}

std::string CDataType::cpp_to_c(int arrayDim, const std::string & name) const
{
    switch (arrayDim)
    {
    case 0:
    {
        switch (publicType)
        {
        case spdoc::PublicType::INT:
        case spdoc::PublicType::DOUBLE:
        case spdoc::PublicType::CHAR:
            return name;
        case spdoc::PublicType::BOOL:
            return spi_util::StringFormat("%s ? SPI_TRUE : SPI_FALSE", name.c_str());
        case spdoc::PublicType::STRING:
            return spi_util::StringFormat("spi_String_copy(%s.c_str())",
                name.c_str());
        case spdoc::PublicType::DATE:
            return spi_util::StringFormat("(spi_Date)(%s)", name.c_str());
        case spdoc::PublicType::DATETIME:
            return spi_util::StringFormat("(spi_DateTime)(%s)", name.c_str());
        case spdoc::PublicType::ENUM:
            return spi_util::StringFormat("%s((%s::Enum)(%s))",
                cType().c_str(), cppType(0).c_str(), name.c_str());
        case spdoc::PublicType::CLASS:
            return spi_util::StringFormat("spi::convert_out<%s_%s>(%s)", 
                dataType->nsService.c_str(),
                spi_util::StringReplace(dataType->name, ".", "_").c_str(),
                name.c_str());
        case spdoc::PublicType::OBJECT:
            return spi_util::StringFormat("(spi_Object*)(spi::RawPointer(%s))",
                name.c_str());
        case spdoc::PublicType::MAP:
            return spi_util::StringFormat("(spi_Map*)(spi::RawPointer(%s))",
                name.c_str());
        case spdoc::PublicType::VARIANT:
            return StringFormat("(spi_Variant*)(new spi::Variant(%s))", name.c_str());
        default:
            SPI_THROW_RUNTIME_ERROR("SCALAR " <<
                spdoc::PublicType::to_string(publicType) << " not implemented");
        }
        break;
    }
    case 1:
    {
        switch (publicType)
        {
        case spdoc::PublicType::CHAR:
            SPI_THROW_RUNTIME_ERROR("Array type not supported for " <<
                spdoc::PublicType::to_string(publicType));
        case spdoc::PublicType::INT:
        case spdoc::PublicType::DOUBLE:
        case spdoc::PublicType::BOOL:
        case spdoc::PublicType::STRING:
        case spdoc::PublicType::DATE:
        case spdoc::PublicType::DATETIME:
        case spdoc::PublicType::ENUM:
        case spdoc::PublicType::CLASS:
        case spdoc::PublicType::OBJECT:
        case spdoc::PublicType::VARIANT:
            return spi_util::StringFormat("(%s)(new %s(%s))",
                cType(1).c_str(), cppType(1).c_str(), name.c_str());
        default:
            SPI_THROW_RUNTIME_ERROR("ARRAY " <<
                spdoc::PublicType::to_string(publicType) << " not implemented");
        }
        break;
    }
    case 2:
    {
        switch (publicType)
        {
        case spdoc::PublicType::CHAR:
            SPI_THROW_RUNTIME_ERROR("Matrix type not supported for " <<
                spdoc::PublicType::to_string(publicType));
        case spdoc::PublicType::INT:
        case spdoc::PublicType::DOUBLE:
        case spdoc::PublicType::BOOL:
        case spdoc::PublicType::STRING:
        case spdoc::PublicType::DATE:
        case spdoc::PublicType::DATETIME:
        case spdoc::PublicType::ENUM:
        case spdoc::PublicType::CLASS:
        case spdoc::PublicType::OBJECT:
        case spdoc::PublicType::VARIANT:
            return spi_util::StringFormat("(%s)(new %s(%s))",
                cType(2).c_str(), cppType(2).c_str(), name.c_str());
        default:
            SPI_THROW_RUNTIME_ERROR("MATRIX " <<
                spdoc::PublicType::to_string(publicType) << " not implemented");
        }
        break;
    }
    default:
        SPI_THROW_RUNTIME_ERROR("Bad value of arrayDim " << arrayDim
            << " should be 0,1 or 2");
    }
}

std::string CDataType::c_to_value(int arrayDim, const std::string & name) const
{
    // cpp is the C++ equivalent of the C-type
    std::string cpp = c_to_cpp(arrayDim, name).c_str();
    switch (arrayDim)
    {
    case 0:
    {
        switch (publicType)
        {
        case spdoc::PublicType::INT:
        case spdoc::PublicType::DOUBLE:
        case spdoc::PublicType::CHAR:
        case spdoc::PublicType::BOOL:
        case spdoc::PublicType::STRING:
        case spdoc::PublicType::DATE:
        case spdoc::PublicType::DATETIME:
            break;
        case spdoc::PublicType::ENUM:
            return StringFormat("%s.to_value()", cpp.c_str());
        case spdoc::PublicType::CLASS:
            return spi_util::StringFormat("spi::Object::to_value(%s)", cpp.c_str());
        case spdoc::PublicType::OBJECT:
        case spdoc::PublicType::MAP:
        default:
            SPI_NOT_IMPLEMENTED;
        }
        break;
    }
    case 1:
    {
        switch (publicType)
        {
        case spdoc::PublicType::CHAR:
            SPI_THROW_RUNTIME_ERROR("Array type not supported for " <<
                spdoc::PublicType::to_string(publicType));
        case spdoc::PublicType::INT:
        case spdoc::PublicType::DOUBLE:
        case spdoc::PublicType::BOOL:
        case spdoc::PublicType::STRING:
        case spdoc::PublicType::DATE:
        case spdoc::PublicType::DATETIME:
        case spdoc::PublicType::ENUM:
            break;
        case spdoc::PublicType::CLASS:
            return spi_util::StringFormat("spi::Object::to_value(spi::MakeObjectVector< %s >(%s))", 
                cppName().c_str(), cpp.c_str());
        case spdoc::PublicType::OBJECT:
        default:
            SPI_NOT_IMPLEMENTED;
        }
        break;
    }
    case 2:
    {
        switch (publicType)
        {
        case spdoc::PublicType::CHAR:
            SPI_THROW_RUNTIME_ERROR("Matrix type not supported for " <<
                spdoc::PublicType::to_string(publicType));
        case spdoc::PublicType::INT:
        case spdoc::PublicType::DOUBLE:
        case spdoc::PublicType::BOOL:
        case spdoc::PublicType::STRING:
        case spdoc::PublicType::DATE:
        case spdoc::PublicType::DATETIME:
        case spdoc::PublicType::ENUM:
            break;
        case spdoc::PublicType::CLASS:
        case spdoc::PublicType::OBJECT:
        default:
            SPI_NOT_IMPLEMENTED;
        }
        break;
    }
    default:
        SPI_THROW_RUNTIME_ERROR("Bad value of arrayDim " << arrayDim
            << " should be 0,1 or 2");
    }

    return spi_util::StringFormat("spi::Value(%s)", cpp.c_str());
}

spdoc::AttributeConstSP ReturnTypeAttribute(const spdoc::DataTypeConstSP & dataType, int arrayDim)
{
    return spdoc::Attribute::Make("_out", std::vector<std::string>(),
        dataType, arrayDim, false, spdoc::ConstantConstSP());
}

std::string CFunctionArg(const spdoc::AttributeConstSP& attr, bool output)
{
    const spdoc::DataTypeConstSP dataType = attr->dataType;

    SPI_PRE_CONDITION(attr->arrayDim >= 0);
    SPI_PRE_CONDITION(attr->arrayDim <= 2);

    const char* Const = output ? "" : "const ";
    const char* Ref = output ? "*" : "";

    std::ostringstream oss;

    CDataType dt(attr->dataType);
    std::string ctype = dt.cType(attr->arrayDim);

    if (spi_util::StringEndsWith(ctype, "*") && !output)
        oss << "const ";

    oss << ctype;

    if (output)
        oss << "*";

    oss << " " << attr->name;

    return oss.str();
}

std::string CFunctionArg(const spdoc::ClassAttributeConstSP& attr, bool output)
{
    return CFunctionArg(attr->operator spdoc::AttributeConstSP(), output);
}

