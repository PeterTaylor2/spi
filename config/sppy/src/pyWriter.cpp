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
#include "pyWriter.hpp"

#include <spi_util/FileUtil.hpp>
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

/*
***************************************************************************
** Implementation of PythonService
***************************************************************************
*/
PythonServiceConstSP PythonService::Make(
    const spdoc::ServiceConstSP& service,
    const Options& options)
{
    return new PythonService(service, options);
}

PythonService::PythonService(
    const spdoc::ServiceConstSP& service,
    const Options& options)
    :
    m_service(service),
    options(options)
{
    m_import = spi::StringFormat("PY_%s_IMPORT", m_service->declSpec.c_str());
}

std::string
PythonService::writeDeclSpecHeaderFile(const std::string& dirname) const
{
    std::string basename = spi::StringFormat("pyd_%s_decl_spec.h", m_service->name.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename), options.writeBackup);
    writeLicense(ostr, options.license);
    startHeaderFile(ostr, filename);

    writeStartCommentBlock(ostr, false);
    ostr << "** " << basename << "\n"
         << "**\n"
         << "** If PY_" << m_service->declSpec << "_EXPORT is defined "
         << "then PY_" << m_service->declSpec << "_IMPORT is dllexport.\n"
         << "** Otherwise PY_" << m_service->declSpec
         << "_IMPORT is dllimport.\n";
    writeEndCommentBlock(ostr);
    if (!options.noGeneratedCodeNotice)
        writeGeneratedCodeNotice(ostr, filename);

    ostr << "\n"
         << "#include <spi_util/ImportExport.h>\n"
         << "#ifdef PY_" << m_service->declSpec << "_EXPORT\n"
         << "#define PY_" << m_service->declSpec
         << "_IMPORT SPI_UTIL_DECLSPEC_EXPORT\n"
         << "#else\n"
         << "#define PY_" << m_service->declSpec
         << "_IMPORT SPI_UTIL_DECLSPEC_IMPORT\n"
         << "#endif\n";

    endHeaderFile(ostr, filename);
    ostr.close();
    return filename;
}

std::string PythonService::writePydHeaderFile(const std::string& dirname) const
{
    std::string basename = spi::StringFormat("pyd_%s.hpp", m_service->name.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename), options.writeBackup);
    writeLicense(ostr, options.license);
    startHeaderFile(ostr, filename);

    writeStartCommentBlock(ostr, false);
    ostr << "** " << basename << "\n"
         << "**\n"
         << "** Top level PYD functions for " << m_service->name << "\n";
    writeEndCommentBlock(ostr);
    if (!options.noGeneratedCodeNotice)
        writeGeneratedCodeNotice(ostr, filename);

    ostr << "\n"
         << "#include \"pyd_" << m_service->name << "_decl_spec.h\"\n"
         << "#include <spi/Namespace.hpp>\n"
         << "\n"
         << "#include \"Python.h\"\n"
         << "\n"
         << "SPI_BEGIN_NAMESPACE\n"
         << "class PythonService;\n"
         << "struct FunctionCaller;\n"
         << "class InputContext;\n"
         << "SPI_END_NAMESPACE\n"
         << "\n"
         << "spi::FunctionCaller* get_function_caller(const char* name);\n"
         << "spi::InputContext* get_input_context();\n"
         << "spi::PythonService* get_python_service();\n"
         << "\n"
         << "extern \"C\"\n"
         << "{\n"
         << "\n"
         << "/*\n"
         << std::string(75, '*') << "\n"
         << "** Function called by Python to initialise the shared library.\n"
         << std::string(75, '*') << "\n"
         << "*/\n"
         << m_import << "\n"
         << "#if PY_MAJOR_VERSION >= 3\n"
         << "PyObject* PyInit_py_" << m_service->name << "(void);\n"
         << "#else\n"
         << "void initpy_" << m_service->name << "(void);\n"
         << "#endif\n"
         << "\n"
         << "} /* end of extern \"C\" */\n";

    endHeaderFile(ostr, filename);
    ostr.close();
    return filename;
}

std::string PythonService::writePydSourceFile(const std::string& dirname) const
{
    std::string basename = spi::StringFormat("pyd_%s.cpp", m_service->name.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename), options.writeBackup);
    writeLicense(ostr, options.license);
    startSourceFile(ostr, filename);
    if (!options.noGeneratedCodeNotice)
        writeGeneratedCodeNotice(ostr, filename);

    ostr << "\n"
         << "#include \"pyd_" << m_service->name << ".hpp\"\n"
         << "\n";

    for (size_t i = 0; i < m_service->modules.size(); ++i)
    {
        const spdoc::ModuleConstSP& module = m_service->modules[i];
        ostr << "#include \"py_" << m_service->ns << "_"
             << module->name << ".hpp\"\n";
    }

    ostr << "\n"
         << "#include <spi/ObjectHandle.hpp>\n"
         << "#include <spi/python/pyService.hpp>\n"
         << "#include <spi/python/pyUtil.hpp>\n"
         << "#include <spi/python/pyObject.h>\n"
         << "\n"
         << "#include \"" << m_service->name << "_dll_service.hpp\"\n"
         << "\n"
         // FIXME: we may need to have a way of deleting this on demand
         << "static spi_boost::shared_ptr<spi::PythonService> theService;\n"
         << "\n"
         << "spi::FunctionCaller* get_function_caller(const char* name)\n"
         << "{\n"
         << "    return theService->GetFunctionCaller(name);\n"
         << "}\n"
         << "\n"
         << "spi::InputContext* get_input_context()\n"
         << "{\n"
         << "    return theService->GetInputContext();\n"
         << "}\n"
         << "\n"

         // there is a get_python_service inside spi-dll-pyNN.dll
         // that version has access to all types but is not exported
         //
         // if we want a service which only has access to our types
         // then we will be using this one
         //
         // perhaps we should have get_python_service and
         // get_common_python_service

         << "spi::PythonService* get_python_service()\n"
         << "{\n"
         << "    return theService.get();\n"
         << "}\n";

    if (!m_service->sharedService)
    {
        ostr << "\n"
            << "extern \"C\"\n"
            << "{\n"
            << "\n"
            << "PyObject* py_" << m_service->ns << "_help_func(\n"
            << "    PyObject* self, PyObject* args)\n"
            << "{\n"
            << "    return theService->HelpFunc(args);\n"
            << "}\n"
            << "\n";

        if (options.helpFuncList)
        {
            ostr << "PyObject* py_" << m_service->ns << "_help_func_list(\n"
                << "    PyObject* self, PyObject* args)\n"
                << "{\n"
                << "    return theService->HelpFunc(NULL);\n"
                << "}\n"
                << "\n";
        }

        ostr << "PyObject* py_" << m_service->ns << "_help_enum(\n"
            << "    PyObject* self, PyObject* args)\n"
            << "{\n"
            << "    return theService->HelpEnum(args);\n"
            << "}\n"
            << "\n"
            << "PyObject* py_" << m_service->ns << "_object_coerce(\n"
            << "    PyObject* self, PyObject* args)\n"
            << "{\n"
            << "    return theService->ObjectCoerce(args);\n"
            << "}\n"
            //<< "\n"
            //<< "PyObject* py_" << m_service->ns << "_object_get(\n"
            //<< "    PyObject* self, PyObject* args)\n"
            //<< "{\n"
            //<< "    return theService->ObjectGet(args);\n"
            //<< "}\n"
            //<< "\n"
            //<< "PyObject* py_" << m_service->ns << "_object_put(\n"
            //<< "    PyObject* self, PyObject* args)\n"
            //<< "{\n"
            //<< "    return theService->ObjectPut(args);\n"
            //<< "}\n"
            << "\n"
            << "PyObject* py_" << m_service->ns << "_start_logging(\n"
            << "    PyObject* self, PyObject* args)\n"
            << "{\n"
            << "    return theService->StartLogging(args);\n"
            << "}\n"
            << "\n"
            << "PyObject* py_" << m_service->ns << "_stop_logging(\n"
            << "    PyObject*self, PyObject* args)\n"
            << "{\n"
            << "    return theService->StopLogging();\n"
            << "}\n"
            << "\n"
            << "} /* end of extern \"C\" */\n";
    }

    ostr << "\n"
        << "/*\n"
        << std::string(75, '*') << "\n"
        << "** Standard Python initialisation function.\n"
        << std::string(75, '*') << "\n"
        << "*/\n"
        << "#if PY_MAJOR_VERSION >= 3\n"
        << "PyObject* PyInit_py_" << m_service->name << "(void)\n"
        << "#else\n"
        << "void initpy_" << m_service->name << "(void)\n"
        << "#endif\n"
        << "{\n"
        << "  try {\n"
        // perhaps we could check an environment variable to see whether we
        // want to "enter to continue" at start-up time - this variable can
        // then be defined on the command line when launching Python
             //<< "#ifdef SPI_DEBUG\n"
             //<< "    std::string wait;\n"
             //<< "    std::cout << \"Enter to continue:\";\n"
             //<< "    std::cin >> wait;\n"
             //<< "#endif\n"
             //<< "\n"
        << "    theService.reset(new spi::PythonService(\"py_" << m_service->name << "\", "
        << m_service->ns << "::" << m_service->name << "_exported_service()));\n"
        << "    spi::PythonService* svc = theService.get();\n"
        << "\n";

    if (!m_service->sharedService)
    {
        ostr << "    /* function registration */\n"
            << "    /* help_func */\n"
            << "    svc->AddFunction(\"help_func\", py_" << m_service->ns
            << "_help_func,\n"
            << "        \"help_func(funcName?)\\n\\n\"\n"
            << "        \"Lists functions or function arguments.\");\n"
            << "\n";

        if (options.helpFuncList)
        {
            ostr << "    /* help_func_list */\n"
                << "    svc->AddFunction(\"help_func_list\", py_" << m_service->ns
                << "_help_func_list,\n"
                << "        \"help_func_list()\\n\\n\"\n"
                << "        \"Lists functions.\");\n"
                << "\n";
        }

        ostr << "    /* help_enum */\n"
            << "    svc->AddFunction(\"help_enum\", py_" << m_service->ns
            << "_help_enum,\n"
            << "        \"help_enum(enumName?)\\n\\n\"\n"
            << "        \"Find enumerated types or enumerands.\");\n"
            << "\n"
            << "    /* object_from_string */\n"
            << "    svc->AddFunction(\"object_from_string\", "
            << "spi_py_object_from_string,\n"
            << "        \"object_from_string(str)\\n\\n\"\n"
            << "        \"Create object from string - identical to Object.from_string.\\n\"\n"
            << "        \"Needs to be a top-level function for pickle/unpickle purposes.\");\n"
            << "\n"
            << "    /* object_get */\n"
            << "    svc->AddFunction(\"object_get\", "
            << "spi_py_object_get,\n"
            << "        \"object_get(obj,name?)\\n\\n\"\n"
            << "        \"Gets a value by name from an object.\");\n"
            << "\n"
            << "    /* object_put */\n"
            << "    svc->AddFunction(\"object_put\", "
            << "spi_py_object_put,\n"
            << "        \"object_put(obj,names[],value,...)\\n\\n\"\n"
            << "        \"Sets a value by name within an object.\");\n"
            << "\n";

        if (options.objectCoerce)
        {
            // each class has a static Coerce method which is much more convenient
            // so we only provide object_coerce on demand
            ostr << "    /* object_coerce */\n"
                << "    svc->AddFunction(\"object_coerce\", py_" << m_service->ns
                << "_object_coerce,\n"
                << "        \"object_coerce(classname, value)\\n\\n\"\n"
                << "        \"Attempts to convert an arbitrary value to the given classname.\");\n"
                << "\n";
        }

        ostr << "    /* start_logging */\n"
            << "    svc->AddFunction(\"start_logging\", py_" << m_service->ns
            << "_start_logging,\n"
            << "        \"start_logging(filename,options?)\\n\\n\"\n"
            << "        \"Start function logging - returns filename.\");\n"
            << "\n"
            << "    /* stop_logging */\n"
            << "    svc->AddFunction(\"stop_logging\", py_" << m_service->ns
            << "_stop_logging,\n"
            << "        \"stop_logging()\\n\\nStops function logging\"\n"
            << "        \" - returns whether logging was on previously.\");\n"
            << "\n";
    }

    for (size_t i = 0; i < m_service->modules.size(); ++i)
    {
        const spdoc::ModuleConstSP& module = m_service->modules[i];
        ostr << "    py_" << m_service->ns << "_"
             << module->name << "_update_functions(svc);\n";
    }

    ostr << "\n"
         << "    svc->RegisterFunctions();\n"
         << "#if PY_MAJOR_VERSION >= 3\n"
         << "    PyObject* module = svc->GetPythonModule();\n"
         << "    return module;\n"
         << "#endif\n"
         << "  }\n"
         << "  catch (std::exception& e)\n"
         << "  {\n"
         << "    spi::pyExceptionHandler(e.what());\n"
         << "#if PY_MAJOR_VERSION >= 3\n"
         << "    return NULL;\n"
         << "#endif\n"
         << "  }\n"
         << "  catch (...)\n"
         << "  {\n"
         << "    spi::pyExceptionHandler(\"Unknown exception\");\n"
         << "#if PY_MAJOR_VERSION >= 3\n"
         << "    return NULL;\n"
         << "#endif\n"
         << "  }\n"
         << "}\n"
         << "\n";


    endSourceFile(ostr, filename);
    ostr.close();
    return filename;
}

std::string PythonService::writePyImporter(const std::string& dirname) const
{
    std::string basename = spi::StringFormat(
        "%s.py", m_service->ns.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename), options.writeBackup);

    ostr << "\"\"\"\n";
    if (m_service->description.size() > 0)
    {
        for (size_t i = 0; i < m_service->description.size(); ++i)
            ostr << m_service->description[i] << "\n";
    }
    else
    {
        ostr << "No description\n";
    }
    ostr << "\"\"\"\n"
         << "\n";

    // this might be futile for non-windows platforms
    //
    // we change to the directory containing the shared library before trying
    // to import it
    //
    // we also found that for Python3 we need to put the current directory in the
    // path since otherwise it won't find the shared library - this is an annoying
    // difference but doesn't seem to be a major problem for Python2

    ostr << "import os\n"
        << "\n"
        << "cwd = os.getcwd()\n"
        << "try:\n"
        << "    os.chdir(os.path.dirname(os.path.abspath(__file__)))\n"
        << "    import sys\n"
        << "    sys.path.insert(0, os.getcwd()) # needed by python3\n"
        << "    try:\n"
        << "        from py_" << m_service->ns << " import *\n"
        << "    finally: sys.path.pop(0)\n"
        << "finally: os.chdir(cwd)\n"
        << "\n";

    ostr.close();
    return filename;
}

const std::string& PythonService::name() const
{
    return m_service->name;
}

const std::string& PythonService::ns() const
{
    return m_service->ns;
}

const std::string& PythonService::import() const
{
    return m_import;
}

const spdoc::ServiceConstSP& PythonService::service() const
{
    return m_service;
}

/*
***************************************************************************
** Implementation of PythonModule
***************************************************************************
*/
PythonModuleConstSP PythonModule::Make(
    const PythonServiceConstSP& service,
    const spdoc::ModuleConstSP& module)
{
    return new PythonModule(service, module);
}

PythonModule::PythonModule(
    const PythonServiceConstSP& service,
    const spdoc::ModuleConstSP& module)
    :
    service(service),
    module(module)
{}

std::string PythonModule::writeHeaderFile(const std::string& dirname) const
{
    std::string basename = spi::StringFormat(
        "py_%s_%s.hpp", service->ns().c_str(), module->name.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename), service->options.writeBackup);
    writeLicense(ostr, service->options.license);
    startHeaderFile(ostr, filename);

    ostr << "\n"
         << "#include \"pyd_" << service->name() << "_decl_spec.h\"\n"
         << "#include <spi/Namespace.hpp>\n";

    ostr << "\n"
         << "#include \"Python.h\"\n"
         << "#include <vector>\n"
         << "\n"
         << "SPI_BEGIN_NAMESPACE\n"
         << "class PythonService;\n"
         << "SPI_END_NAMESPACE\n";

    ostr << "\n"
         << "void py_" << service->ns() << "_" << module->name
         << "_update_functions(spi::PythonService* svc);\n";

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

std::string PythonModule::writeSourceFile(const std::string& dirname) const
{
    std::string basename = spi::StringFormat(
        "py_%s_%s.cpp", service->ns().c_str(), module->name.c_str());
    std::string filename = spi_util::path::join(
        dirname.c_str(), basename.c_str(), 0);

    GeneratedOutput ostr(filename, spi_util::path::dirname(filename), service->options.writeBackup);
    writeLicense(ostr, service->options.license);
    startSourceFile(ostr, filename);
    if (!service->options.noGeneratedCodeNotice)
        writeGeneratedCodeNotice(ostr, filename);

    ostr << "\n"
         << "#include \"py_" << service->ns() << "_" << module->name << ".hpp\"\n"
         << "#include \"pyd_" << service->name() << ".hpp\"\n"
         << "\n"
         << "#include <spi/python/pyUtil.hpp>\n"
         << "#include <spi/python/pyInput.hpp>\n"
         << "#include <spi/python/pyObject.hpp>\n"
         << "#include <spi/python/pyObjectMap.hpp>\n"
         << "#include <spi/python/pyOutput.hpp>\n"
         << "#include <spi/python/pyService.hpp>\n"
         << "#include <spi/python/pyValue.hpp>\n"
         << "\n"
         << "#include \"" << service->ns() << "_" << module->name
         << ".hpp\"\n";

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
    }

    ostr << "\n"
         << "void py_" << service->ns() << "_" << module->name
         << "_update_functions(spi::PythonService* svc)\n"
         << "{\n"
         << "    svc->SetNamespace(\"" << module->ns << "\");\n";

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

void writeExceptionHandling(
    GeneratedOutput&           ostr,
    const PythonServiceConstSP& service,
    bool  returnErrorCode = false)
{
    ostr << "    }\n"
         << "    catch (spi::PyException&)\n"
         << "    {\n";

    if (returnErrorCode)
    {
        ostr << "        return -1;\n";
    }
    else
    {
        ostr << "        return NULL;\n";
    }

    ostr << "    }\n"
         << "    catch (std::exception &e)\n"
         << "    {\n";

     if (returnErrorCode)
     {
         ostr << "        spi::pyExceptionHandler(e.what());\n"
              << "        return -1;\n";
     }
     else
     {
         ostr << "        return spi::pyExceptionHandler(e.what());\n";
     }

     ostr << "    }\n"
          << "    catch (...)\n"
          << "    {\n";

     if (returnErrorCode)
     {
         ostr << "        spi::pyExceptionHandler(\"Unknown exception\");\n"
              << "        return -1;\n";
     }
     else
     {
         ostr << "        return spi::pyExceptionHandler(\"Unknown exception\");\n";
     }

     ostr << "    }\n"
          << "}\n";
}
} // end of anonymous namespace

/*
***************************************************************************
** Writer methods for functions
***************************************************************************
*/
void PythonModule::declareFunction(
    GeneratedOutput& ostr,
    const spdoc::Function* func) const
{
    ostr << "\n"
         << "PyObject* py_" << service->ns() << "_"
         << makeNamespaceSep(module->ns, "_")
         << func->name << "(PyObject* self";

    if (service->options.fastCall)
        ostr << ", PyObject* const* args, Py_ssize_t nargs";
    else
        ostr << ", PyObject* args";

    if (service->options.keywords)
        ostr << ", PyObject* kwargs";

    ostr << ");\n";

}

void PythonModule::implementFunction(
    GeneratedOutput& ostr,
    const spdoc::Function* func) const
{
    ostr << "\n"
         << "PyObject* py_" << service->ns() << "_"
         << makeNamespaceSep(module->ns, "_")
         << func->name << "(PyObject* self";

    if (service->options.fastCall)
        ostr << ", PyObject* const* args, Py_ssize_t nargs";
    else
        ostr << ", PyObject* args";

    if (service->options.keywords)
        ostr << ", PyObject* kwargs";

    ostr << ")\n";

    ostr << "{\n"
        << "    static spi::FunctionCaller* func = 0;\n";

    ostr << "    try\n"
         << "    {\n"
         << "        if (!func)\n"
         << "            func = get_function_caller(\""
         << makeNamespaceSep(module->ns, ".") << func->name << "\");\n"
         << "\n";

    ostr << "        const spi::InputValues& iv = spi::pyGetInputValues(func";

    if (service->options.fastCall)
        ostr << ", args, nargs";
    else
        ostr << ", args";

    if (service->options.keywords)
        ostr << ", kwargs";

    ostr << ");\n";

    ostr << "        spi::Value output = spi::CallInContext(func, iv,"
         << " get_input_context());\n";

    if (func->outputs.size() > 0)
    {
        SPI_PRE_CONDITION(!func->returnType);
        ostr << "        return spi::pyoFromMultiValue(output, "
             << func->outputs.size() << ");\n";
    }
    else if (!func->returnType)
    {
        ostr << "        Py_RETURN_NONE;\n";
    }
    else
    {
        ostr << "        return spi::pyoFromValue(output);\n";
    }
    writeExceptionHandling(ostr, service);
}

void PythonModule::registerFunction(
    GeneratedOutput& ostr,
    const spdoc::Function* func) const
{
    std::string regName = service->options.lowerCase ?
        spi_util::StringLower(func->name) : func->name;

    ostr << "\n"
         << "    /* " << makeNamespaceSep(module->ns, ".") << regName
         << " */\n";

    std::string docString;
    std::vector<std::string> docStrings;
    std::vector<std::string> args;

    for (size_t i = 0; i < func->inputs.size(); ++i)
    {
        const spdoc::AttributeConstSP& arg = func->inputs[i];
        std::string name = arg->name;
        if (arg->isArray())
            name += "=[]";
        else if (arg->isOptional)
            name += "=None";
        args.push_back(name);
    }

    std::stringstream oss;
    oss << makeNamespaceSep(module->ns, ".") << regName << "("
        << spi::StringJoin(",", args) << ")";

    docStrings.push_back(oss.str());
    docStrings.push_back("");
    docStrings.push_back(GetFirstParagraph(func->description));
    //for (size_t i = 0; i < func->description.size(); ++i)
    //    docStrings.push_back(func->description[i]);

    docString = spi::StringStrip(spi::StringJoin("\n", docStrings));

    const char* functionCast = service->options.keywords || service->options.fastCall ?
        "(PyCFunction)" :
        "";

    ostr << "    svc->AddFunction(\"" << regName << "\",\n"
        << "        " << functionCast << "py_" << service->ns() << "_"
        << makeNamespaceSep(module->ns, "_") << func->name << ",\n"
        << "        \"" << spi::StringEscape(docString.c_str()) << "\",\n";

    if (service->options.fastCall)
        ostr << "        METH_FASTCALL";
    else
        ostr << "        METH_VARARGS";

    if (service->options.keywords)
        ostr << " | METH_KEYWORDS";

    ostr << ");\n";
}

/*
***************************************************************************
** Writer methods for classes
***************************************************************************
*/
void PythonModule::declareClass(
    GeneratedOutput& ostr,
    const spdoc::Class* cls) const
{
    ostr << "\n"
         << "PyObject* py_" << service->ns() << "_"
         << makeNamespaceSep(module->ns, "_") << cls->name << "_Coerce"
         << "(PyObject* self, PyObject* args);\n";

    for (size_t i = 0; i < cls->methods.size(); ++i)
    {
        const spdoc::ClassMethodConstSP& method = cls->methods[i];
        if (method->isImplementation)
            continue;

        ostr << "\n"
             << "PyObject* py_" << service->ns() << "_"
             << makeNamespaceSep(module->ns, "_") << cls->name
             << "_" << method->function->name
             << "(PyObject* self";

        if (service->options.fastCall)
            ostr << ", PyObject* const* args, Py_ssize_t nargs";
        else
            ostr << ", PyObject* args";

        if (service->options.keywords)
            ostr << ", PyObject* kwargs";

        ostr << ");\n";
    }
}

void PythonModule::implementClass(
    GeneratedOutput& ostr,
    const spdoc::Class* cls) const
{
    std::string cppTypename = service->ns() + "::"
        + makeNamespaceSep(module->ns, "::") + cls->name;
    std::string classname = makeNamespaceSep(module->ns, ".") + cls->name;
    std::string cTypename = makeNamespaceSep(module->ns, "_") + cls->name;
    std::string cTypenameFull = service->ns() + "_" + cTypename;

    std::vector<std::string> docStrings;
    docStrings.push_back(GetFirstParagraph(cls->description));

    std::string pythonDelegateClassName;
    if (cls->isDelegate)
    {
        std::ostringstream oss;
        oss << makeNamespaceSep(module->ns, "_") << cls->name
            << "_python_delegate";
        pythonDelegateClassName = oss.str();

        ostr << "\n"
             << "SPI_DECLARE_OBJECT_CLASS(" << pythonDelegateClassName << ");\n"
             << "\n"
             << "class " << pythonDelegateClassName << " : public "
             << cppTypename << "\n"
             << "{\n"
             << "public:\n"
             << "    // IMPLEMENTS spi::Object\n"
             << "    spi::ObjectType* get_object_type() const\n"
             << "    {\n"
             << "        return py_object_type.get();\n"
             << "    }\n"
             << "\n"
             << "    // IMPLEMENTS spi::Object\n"
             << "    void to_map(spi::IObjectMap* objMap, bool publicOnly) const\n"
             << "    {\n"
             << "        spi::pyObjectToMap(self, objMap, publicOnly);\n"
             << "    }\n"
             << "\n"
             << "    static spi::Object* Constructor(PyObject* pyo)\n"
             << "    {\n"
             << "        return new " << pythonDelegateClassName << "(pyo);\n"
             << "    }\n";

        ostr << "\n"
             << "    ~" << cls->name << "_python_delegate()\n"
             << "    {\n"
             << "        if (Py_IsInitialized())\n"
             << "        {\n"
             << "            spi::PyInterpreterLock lock;\n"
             << "            Py_XDECREF(self);\n"
             << "        }\n"
             << "    }\n";

        // declare all the methods of the base class
        for (size_t i = 0; i < cls->methods.size(); ++i)
        {
            spdoc::ClassMethodConstSP method = cls->methods[i];
            if (method->isStatic)
                continue;

            spdoc::FunctionConstSP function    = method->function;
            spdoc::DataTypeConstSP returnType  = function->returnType;
            int                    returnArrayDim = function->returnArrayDim;
            const std::string&     methodName  = function->name;
            const std::vector<spdoc::AttributeConstSP>& inputs = function->inputs;

            // we need to declare the parameters
            // we may as well implement in-line - saves declaring again later
            // same to apply to all methods of this class I would imagine
            // except for the ones declared by SPI_DECLARE_OBJECT_TYPE

            ostr << "\n"
                 << "    ";

            if (returnArrayDim == 2)
            {
                SPI_PRE_CONDITION(returnType);
                ostr << "spi::MatrixData< " << returnType->ValueType() << " >";
            }
            else if (returnArrayDim == 1)
            {
                SPI_PRE_CONDITION(returnType);
                ostr << "std::vector< " << returnType->ValueType() << " >";
            }
            else if (returnType)
            {
                ostr << returnType->ValueType();
            }
            else
            {
                ostr << "void";
            }
            ostr << " " << methodName << "(";
            const char* sep = "\n        ";
            for (size_t i = 0; i < inputs.size(); ++i)
            {
                ostr << sep;
                sep = ",\n        ";

                const spdoc::AttributeConstSP& arg = inputs[i];

                const spdoc::DataTypeConstSP& dataType = arg->dataType;
                bool                          isArray  = arg->isArray();
                const std::string&            argName  = arg->name;

                std::string valueTypeName = dataType->ValueType();
                std::string refTypeName   = dataType->RefType();
                if (isArray)
                {
                    ostr << "const std::vector< " << valueTypeName
                         << " >&";
                }
                else if (dataType->passByReference)
                {
                    ostr << refTypeName;
                }
                else
                {
                    ostr << valueTypeName;
                }
                ostr << " " << argName; // no lining up
            }
            ostr << ") const";

            //if (method->isConst)
            //    ostr << " const";

            ostr << "\n"
                 << "    {\n"
                 << "        spi::PyInterpreterLock lock;\n"
                 << "        PyObjectSP the_method = spi::pyoShare(PyObject_GetAttrString(self, "
                 << "\"" << methodName << "\"));\n"
                 << "\n"
                 << "        std::vector<spi::Value> input_values;\n";

            for (size_t i = 0; i < inputs.size(); ++i)
            {
                const spdoc::AttributeConstSP& arg = inputs[i];
                const spdoc::DataTypeConstSP& dataType = arg->dataType;
                bool  isArray = arg->isArray();
                ostr << "        input_values.push_back(";
                switch(dataType->publicType)
                {
                case spdoc::PublicType::CLASS:
                    if (isArray)
                    {
                        throw spi_util::RuntimeError("Cannot handle array of objects to delegate function");
                    }
                    ostr << "spi::Object::to_value(" << arg->name << "));\n";
                    break;
                default:
                    ostr << "spi::Value(" << arg->name << "));\n";
                    break;
                }
            }

            ostr << "        PyObjectSP method_args = pyTupleFromValueVector("
                 << "input_values);\n";

            ostr << "\n"
                 << "        PyObject* pyo_output = PyObject_CallObject"
                 << "(the_method.get(), method_args.get());\n"
                 << "        if (!pyo_output)\n"
                 << "            spi::pyPythonExceptionHandler();\n"
                 << "        PyObjectSP pyo_output_sp = spi::pyoShare"
                 << "(pyo_output);\n";

            if (returnType)
            {
                std::ostringstream oss;
                if (!returnType->nsService.empty())
                    oss << returnType->nsService << ".";
                oss << returnType->name;
                std::string name = oss.str();

                ostr << "\n"
                     << "        spi::Value o_value = spi::pyoToValue"
                     << "(pyo_output);\n"
                     << "        return "
                     << TranslateFromValue(returnType->publicType,
                                           name,
                                           "o_value",
                                           returnArrayDim)
                     << ";\n";
            }
            else
            {
                ostr << "        return;\n";
            }
            ostr << "    }\n";
        }

        ostr << "\n"
             << "    // RE-IMPLEMENTS spi::Object\n"
             << "    void* get_py_object() const\n"
             << "    {\n"
             << "        return (void*)self;\n"
             << "    }\n";

        ostr << "\n"
             << "private:\n"
             << "    PyObject* self;\n"
             << "    spi::ObjectTypeSP py_object_type;\n"
             << "\n"
             << "    " << pythonDelegateClassName << "(PyObject* pyo) : self(pyo), py_object_type()\n"
             << "    {\n"
             << "        if (!self)\n"
             << "            throw std::runtime_error(\"Attempt to wrap NULL pointer in '"
             << pythonDelegateClassName
             << "'\");\n"
             << "        py_object_type = spi::PythonService::FindRegisteredPythonClass(Py_TYPE(self));\n"
             << "        Py_INCREF(self);\n"
             << "    }\n"
             << "\n"
             << "};\n"
             << "\n";

        //ostr << pythonDelegateClassName << "::outer_type\n"
        //     << pythonDelegateClassName << "::Coerce(const spi::ObjectConstSP& o)\n"
        //     << "{\n"
        //     << "    SPI_NOT_IMPLEMENTED;\n" // perhaps it should be (is_instance check??)
        //     << "}\n"
        //     << "\n";
    }

    // it seems that tp_init does not have a fastcall option
    if (cls->noMake)
    {
        // I was surprised to discover that even if we don't provide tp_init
        // that Python enabled the constructor of the class - hence we have
        // to provide tp_init and make it throw an exception
        ostr << "\n"
             << "static int py_" << cTypenameFull << "_init"
            << "(SpiPyObject* self, PyObject* args";
        if (service->options.keywords)
            ostr << ", PyObject* kwargs";
        else
            ostr << ", PyObject* kwds"; // but these are ignored
        ostr << ")\n";
        ostr << "{\n"
             << "    try\n"
             << "    {\n"
             << "        throw spi::RuntimeError(\"Cannot construct class of type %s\", \""
             << classname
             << "\");\n";

        writeExceptionHandling(ostr, service, true);
    }
    else
    {
        ostr << "\n"
            << "static int py_" << cTypenameFull << "_init"
            << "(SpiPyObject* self, PyObject* args";
        if (service->options.keywords)
            ostr << ", PyObject* kwargs";
        else
            ostr << ", PyObject* kwds"; // but these are ignored
        ostr << ")\n";
        ostr << "{\n"
            << "    static spi::FunctionCaller* func = 0;\n"
            << "    try\n"
            << "    {\n"
            << "        if (!func)\n"
            << "            func = get_function_caller(\"" << classname
            << "\");\n"
            << "\n"
            << "        self->obj = spi::pyInitConstObject(args";

        if (service->options.keywords)
            ostr << ", kwargs";

        ostr << ", func, &" << cppTypename << "::object_type);\n"
             << "        return 0;\n";

        writeExceptionHandling(ostr, service, true);

        std::vector<std::string> args;
        args.push_back("self");
        for (size_t i = 0; i < cls->attributes.size(); ++i)
        {
            const spdoc::ClassAttributeConstSP& attr = cls->attributes[i];
            std::string name = attr->name;
            if (attr->isArray())
                name += "=[]";
            else if (attr->isOptional)
                name += "=None";
            args.push_back(name);
        }

        std::stringstream oss;
        oss << "__init__(" << spi::StringJoin(", ", args) << ")";
        docStrings.push_back("");
        docStrings.push_back(oss.str());
    }

    bool hasProperties = false;
    for (size_t i = 0; i < cls->attributes.size(); ++i)
    {
        const spdoc::ClassAttributeConstSP& attr = cls->attributes[i];
        if (!attr->accessible)
            continue;

        if (!hasProperties)
        {
            hasProperties = true;
            ostr << "\n"
                 << "static PyGetSetDef " << cTypename << "_properties[] = {\n";
        }
        std::string docString = GetFirstParagraph(attr->description);
        ostr << "    {\"" << attr->name
             << "\", (getter)(spi_py_object_getter), ";

        if (cls->canPut)
        {
            ostr << "(setter)(spi_py_object_setter),";
        }
        else
        {
            ostr << "NULL,";
        }

        if (docString.empty())
        {
            ostr << " NULL,\n";
        }
        else
        {
            ostr << "\n        \"" << spi::StringEscape(docString.c_str())
                 << "\",\n";
        }
        ostr << "        (void*) \"" << attr->name << "\"},\n";
    }
    for (size_t i = 0; i < cls->properties.size(); ++i)
    {
        const spdoc::ClassAttributeConstSP& prop = cls->properties[i];
        SPI_PRE_CONDITION(prop->accessible);

        if (!hasProperties)
        {
            hasProperties = true;
            ostr << "\n"
                 << "static PyGetSetDef " << cTypename << "_properties[] = {\n";
        }
        std::string docString = GetFirstParagraph(prop->description);
        ostr << "    {\"" << prop->name
             << "\", (getter)(spi_py_object_getter), NULL,";

        if (docString.empty())
        {
            ostr << " NULL,\n";
        }
        else
        {
            ostr << "\n        \"" << spi::StringEscape(docString.c_str())
                 << "\",\n";
        }
        ostr << "        (void*) \"" << prop->name << "\"},\n";
    }
    if (hasProperties)
    {
        ostr << "    {NULL} // sentinel\n"
             << "};\n";
    }

    ostr << "\n"
         << "PyObject* py_" << service->ns() << "_"
         << makeNamespaceSep(module->ns, "_") << cls->name << "_Coerce"
         << "(PyObject* self, PyObject* args)\n"
         << "{\n"
         << "    PyObject* pyo = get_python_service()->ObjectCoerce(\""
         << cls->ObjectName() << "\", args);\n"
         << "    return pyo;\n"
         << "}\n";

    std::map<std::string, std::string> classMethods;
    for (size_t i = 0; i < cls->methods.size(); ++i)
    {
        const spdoc::ClassMethodConstSP& method = cls->methods[i];
        if (method->isImplementation)
            continue;

        std::ostringstream oss;
        oss << "py_" << service->ns() << "_"
            << makeNamespaceSep(module->ns, "_") << cls->name << "_"
            << method->function->name;
        std::string funcName = oss.str();
        classMethods[method->function->name] = funcName;

        ostr << "\n"
             << "PyObject* " << funcName << "(PyObject* self";

        if (service->options.fastCall)
            ostr << ", PyObject* const* args, Py_ssize_t nargs";
        else
            ostr << ", PyObject* args";

        if (service->options.keywords)
            ostr << ", PyObject* kwargs";

        ostr << ")\n"
             << "{\n"
             << "    static spi::FunctionCaller* func = 0;\n"
             << "    try\n"
             << "    {\n"
             << "        if (!func)\n"
             << "            func = get_function_caller(\""
             << makeNamespaceSep(module->ns, ".") << cls->name
             << "." << method->function->name << "\");\n"
             << "\n";
        ostr << "        const spi::InputValues& iv = spi::pyGetInputValues(func";

        if (service->options.fastCall)
            ostr << ", args, nargs";
        else
            ostr << ", args";

        if (service->options.keywords)
            ostr << ", kwargs";
        else
            ostr << ", 0";

        ostr << ", self);\n";

        ostr << "        spi::Value output = spi::CallInContext(func, iv, "
             << "get_input_context());\n";

        if (method->function->outputs.size() > 0)
        {
            SPI_PRE_CONDITION(!method->function->returnType);
            ostr << "        return spi::pyoFromMultiValue(output, "
                << method->function->outputs.size() << ");\n";
        }
        else if (!method->function->returnType)
        {
            ostr << "        Py_RETURN_NONE;\n";
        }
        else
        {
            ostr << "        return spi::pyoFromValue(output);\n";
        }
        writeExceptionHandling(ostr, service);
    }

    std::string docString = spi::StringStrip(
        spi::StringJoin("\n", docStrings));

    ostr << "static PyMethodDef " << cTypename << "_methods[] = {\n";

    ostr << "    {\"Coerce\", (PyCFunction)"
         << "py_" << service->ns() << "_" << makeNamespaceSep(module->ns, "_")
         << cls->name << "_Coerce" << ", METH_VARARGS | METH_STATIC,\n"
         << "        \"Coerce " << cls->name << " from arbitrary value\"},\n";

    for (size_t i = 0; i < cls->methods.size(); ++i)
    {
        const spdoc::ClassMethodConstSP& method = cls->methods[i];
        if (method->isImplementation)
            continue;

        std::string funcName = classMethods[method->function->name];

        std::string docString;
        std::vector<std::string> docStrings;
        std::vector<std::string> args;

        if (!method->isStatic)
            args.push_back("self");
        for (size_t i = 0; i < method->function->inputs.size(); ++i)
        {
            const spdoc::AttributeConstSP& arg = method->function->inputs[i];
            std::string name = arg->name;
            if (arg->isArray())
                name += "=[]";
            else if (arg->isOptional)
                name += "=None";
            args.push_back(name);
        }

        std::stringstream oss;
        oss << method->function->name << "(" << spi::StringJoin(", ", args)
            << ")";

        docStrings.push_back(oss.str());
        docStrings.push_back("");
        docStrings.push_back(
            GetFirstParagraph(method->function->description));

        std::string methodName = method->function->name;
        if (service->options.lowerCaseMethod)
            methodName = spi_util::StringLower(methodName);

        docString = spi::StringStrip(spi::StringJoin("\n", docStrings));
        ostr << "    {\"" << methodName << "\", (PyCFunction)" << funcName << ", ";

        if (service->options.fastCall)
            ostr << "METH_FASTCALL";
        else
            ostr << "METH_VARARGS";

        if (service->options.keywords)
            ostr << " | METH_KEYWORDS";

        if (method->isStatic)
            ostr << " | METH_STATIC";

        ostr << ",\n        \"" << spi::StringEscape(docString.c_str())
             << "\"},\n";
    }
    ostr << "    {NULL, NULL, 0, NULL} // sentinel\n"
         << "};\n";

    // bool canSubclass = false;

    // note that spi_py_object_getattro calls PyObject_GenericGetAttr first
    // this enables us to find all the methods and static attributes
    const char* getattro = cls->hasDynamicAttributes ?
        "(getattrofunc)spi_py_object_getattro"
        : "0";

    ostr << "\n"
         << "static PyTypeObject " << cTypename << "_PyObjectType = {\n"
         << "    PyVarObject_HEAD_INIT(NULL, 0)\n"
         //<< "#if PY_MAJOR_VERSION < 3\n"
         //<< "    0, /*ob_size*/\n"
         //<< "#endif\n"
         << "    \"" << service->ns() << "." << classname << "\", /*tp_name*/\n"
         << "    sizeof(SpiPyObject), /*tp_basicsize*/\n"
         << "    0, /*tp_itemsize*/\n"
         << "    (destructor)spi_py_object_dealloc, /*tp_dealloc*/\n"
         << "    0, /*tp_print*/\n"
         << "    0, /*tp_getattr*/\n"
         << "    0, /*tp_setattr*/\n"
         << "    0, /*tp_compare*/\n"
         << "    0, /*tp_repr*/\n"
         << "    0, /*tp_as_number*/\n"
         << "    0, /*tp_as_sequence*/\n"
         << "    0, /*tp_as_mapping*/\n"
         << "    0, /*tp_hash */\n"
         << "    0, /*tp_call*/\n"
         << "    0, /*tp_str*/\n"
         << "    " << getattro << ", /*tp_getattro*/\n"
         << "    0, /*tp_setattro*/\n"
         << "    0, /*tp_as_buffer*/\n"
         << "    Py_TPFLAGS_DEFAULT";

    if (cls->isDelegate)
    {
        ostr << " | Py_TPFLAGS_BASETYPE";
    }

    ostr << ", /*tp_flags*/\n"
         << "    \"" << spi::StringEscape(docString.c_str())
         << "\", /* tp_doc */\n"
         << "    0, /* tp_traverse */\n"
         << "    0, /* tp_clear */\n"
         << "    0, /* tp_richcompare */\n"
         << "    0, /* tp_weaklistoffset */\n"
         << "    0, /* tp_iter */\n"
         << "    0, /* tp_iternext */\n";

    ostr << "    " << cTypename << "_methods, /* tp_methods */\n";

    ostr << "    0, /* tp_members */\n";

    if (hasProperties)
    {
        ostr << "    " << cTypename << "_properties, /* tp_getset */\n";
    }
    else
    {
        ostr << "    0, /* tp_getset */\n";
    }

    ostr << "    0, /* tp_base */\n"
         << "    0, /* tp_dict */\n"
         << "    0, /* tp_descr_get */\n"
         << "    0, /* tp_descr_set */\n"
         << "    0, /* tp_dictoffset */\n"
         << "    (initproc)py_" << cTypenameFull << "_init, /* tp_init */\n"
         << "    0, /* tp_alloc */\n"
         << "    PyType_GenericNew, /* tp_new */\n"
         << "};\n";

}

void PythonModule::registerClass(
    GeneratedOutput& ostr,
    const spdoc::Class* cls) const
{
    ostr << "\n"
         << "    svc->AddClass(\"" << makeNamespaceSep(module->ns, ".")
         << cls->name << "\", \"" << makeNamespaceSep(module->ns, ".")
         << cls->ObjectName() << "\",\n"
         << "        &" << makeNamespaceSep(module->ns, "_")
         << cls->name << "_PyObjectType";

    if (!cls->baseClassName.empty())
    {
        // need to use ObjectName() for base class instead of Name()
        // since we index the class using ObjectName()
        // hence we have to get the base class instead of just using baseClassName
        spdoc::ClassConstSP baseClass = service->service()->getClass(cls->baseClassName);
        const std::string& ns = baseClass->ns;
        ostr << ", \"";
        if (!ns.empty())
            ostr << ns << ".";
        ostr << baseClass->ObjectName() << "\"";
    }

    ostr << ");\n";

    if (cls->isDelegate)
    {
        ostr << "\n"
             << "    svc->AddDelegateClass(\""
             << makeNamespaceSep(module->ns, ".") << cls->name << "\", "
             << makeNamespaceSep(module->ns, "_") << cls->name
             << "_python_delegate::Constructor);\n";
    }
}

