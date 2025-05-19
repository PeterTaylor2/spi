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
#include "texModule.hpp"
#include "texIndex.hpp"
#include "texOptions.hpp"

#include <spgtools/texUtils.hpp>
#include <spgtools/generatedOutput.hpp>
#include <spgtools/commonTools.hpp>
#include <spgtools/namespaceManager.hpp>

#include <spi/RuntimeError.hpp>
#include <spi/StringUtil.hpp>
#include <spi/spdoc_configTypes.hpp>

#include <algorithm>
#include <sstream>

namespace {

    std::string moduleHeader(
        const spdoc::ServiceConstSP& service,
        const spdoc::ModuleConstSP& module)
    {
        std::ostringstream oss;
        oss << service->ns << "_" << module->name << ".hpp";
        return oss.str();
    }

    std::string moduleClassesHeader(
        const spdoc::ServiceConstSP& service,
        const spdoc::ModuleConstSP& module)
    {
        std::ostringstream oss;
        oss << service->ns << "_" << module->name << "_classes.hpp";
        return oss.str();
    }

    bool writeBackup()
    {
        return getOptions().writeBackup;
    }
}

template<class T>
T const* ConstructCast(
    const spdoc::ConstructConstSP& construct)
{
    T const* out = dynamic_cast<T const*>(construct.get());
    if (!out)
        throw spi::RuntimeError("Type mismatch");
    return out;
}

template<class T>
std::string ConstructName(
    const T* construct,
    const spdoc::ModuleConstSP& module)
{
    std::ostringstream oss;
    if (module->ns.length() > 0)
        oss << module->ns << ".";
    oss << construct->name;
    return oss.str();
}

static std::string translateTypename(const std::string& tn)
{
    static std::map<std::string, std::string> builtinTypes;
    if (builtinTypes.empty())
    {
        builtinTypes["bool"] = "bool";
        builtinTypes["int"] = "int";
        builtinTypes["double"] = "double";
        builtinTypes["string"] = "string";
        builtinTypes["date"] = "Date";
        builtinTypes["char"] = "char";
        builtinTypes["long"] = "int";
        builtinTypes["size_t"] = "unsigned int";
    }

    const std::map<std::string, std::string>::const_iterator iter =
        builtinTypes.find(tn);

    if (iter == builtinTypes.end())
        return tn;

    return iter->second;
}

static void writeArgsTable(
    GeneratedOutput& ostr,
    const std::vector<spdoc::AttributeConstSP>& args,
    std::set<std::string>& typesUsed)
{
    writeTexBeginTable(ostr, 2, 150, "Type and Name", 310, "Description");

    for (size_t i = 0; i < args.size(); ++i)
    {
        const spdoc::AttributeConstSP& arg       = args[i];
        const spdoc::DataTypeConstSP& dataType   = arg->dataType;
        const spdoc::PublicType::Enum publicType = dataType->publicType;

        ostr << "\\cline{1-2}\n";
        ostr << "\\raggedright " << texEscape(translateTypename(dataType->name)) << " "
             << texEscape(arg->name);
        switch(arg->arrayDim)
        {
        case 2:
            ostr << "\\texttt{[,]}";
            break;
        case 1:
            ostr << "\\texttt{[]}";
            break;
        }
        ostr << " &\n\\raggedright ";

        // Note: outputs won't be optional - this is the only part of this
        // little function which might be different for outputs rather
        // than inputs.
        if (arg->isOptional)
        {
            std::string defaultValue = arg->defaultValue ?
                arg->defaultValue->docString() : std::string();

            if (defaultValue.length() > 0)
            {
                ostr << "Optional (default = \\texttt{"
                     << texEscape(defaultValue, true)  << "}). ";
            }
            else
            {
                ostr << "Optional. ";
            }
        }

        writeTexDescription(ostr, arg->description);

        // for some reason imported types do not seem to provide the noDoc
        // attribute - perhaps we haven't wrapped it properly
        if (!dataType->noDoc)
        {
            ostr << "\nSee " << texEscape(dataType->name)
                 << " (page \\pageref{type_" << dataType->name << "}).\n";

            typesUsed.insert(dataType->name);
        }

#if 0
        switch(publicType)
        {
        case spdoc::PublicType::ENUM:
            ostr << "\nSee " << texEscape(dataType->name)
                 << " (page \\pageref{type_" << dataType->name << "}).\n";
            break;
        case spdoc::PublicType::CLASS:
            ostr << "\nSee " << texEscape(dataType->name)
                 << " (page \\pageref{type_" << dataType->name << "}).\n";
            break;
        default:
            break;
        }
#endif
        ostr << " & \\\\\n";
    }

    writeTexEndTable(ostr, 2);
}

static void writeAttributesTable(
    GeneratedOutput& ostr,
    const std::vector<spdoc::ClassAttributeConstSP>& attributes,
    bool noCrossRef)
{
    // annoyingly close to function arguments table
    // differences are that the list of attributes is of different type
    // and there are differences such as the access level
    //
    // we use this both for attributes and properties (which have a
    // separate table if they exist) - all properties are public
    // so the issue of accessibility is irrelevant
    writeTexBeginTable(ostr, 2, 150, "Type and Name", 310, "Description");

    for (size_t i = 0; i < attributes.size(); ++i)
    {
        const spdoc::ClassAttributeConstSP& attr = attributes[i];
        const spdoc::DataTypeConstSP& dataType   = attr->dataType;
        const spdoc::PublicType::Enum publicType = dataType->publicType;

        ostr << "\\cline{1-2}\n";
        ostr << "\\raggedright " << texEscape(translateTypename(dataType->name))
             << " " << texEscape(attr->name);
        switch(attr->arrayDim)
        {
        case 2:
            ostr << "\\texttt{[,]}";
            break;
        case 1:
            ostr << "\\texttt{[]}";
            break;
        }
        ostr << " &\n\\raggedright ";
        if (attr->isOptional)
        {
            std::string defaultValue = attr->defaultValue ?
                attr->defaultValue->docString() : std::string();

            if (defaultValue.length() > 0)
            {
                ostr << "Optional (default = \\texttt{"
                     << texEscape(defaultValue, true)  << "}). ";
            }
            else
            {
                ostr << "Optional. ";
            }
        }
        if (!attr->accessible)
            ostr << "Private. ";

        writeTexDescription(ostr, attr->description);

        if (!dataType->noDoc && !noCrossRef)
        {
            ostr << "\nSee " << texEscape(dataType->name)
                 << " (page \\pageref{type_" << dataType->name << "}).\n";
        }

#if 0
        switch(publicType)
        {
        case spdoc::PublicType::ENUM:
            ostr << "\nSee " << texEscape(dataType->name)
                 << " (page \\pageref{type_" << dataType->name << "}).\n";
            break;
        case spdoc::PublicType::CLASS:
            ostr << "\nSee " << texEscape(dataType->name)
                 << " (page \\pageref{type_" << dataType->name << "}).\n";
            break;
        case spdoc::PublicType::BOOL:
        case spdoc::PublicType::CHAR:
        case spdoc::PublicType::INT:
        case spdoc::PublicType::DOUBLE:
        case spdoc::PublicType::STRING:
        case spdoc::PublicType::DATE:
        case spdoc::PublicType::UNINITIALIZED_VALUE:
            break;
        }
#endif
        ostr << " & \\\\\n";
    }

    writeTexEndTable(ostr, 2);
}


void writeTexFunctionArgs(
    GeneratedOutput&       ostr,
    const spdoc::Function* func,
    std::set<std::string>& typesUsed)
{
    ostr << "\n"
         << "\\textbf{Inputs:}\n"
         << "\\nopagebreak\n";

    if (func->inputs.size() == 0)
    {
        ostr << "None\n\n";
    }
    else
    {
        writeArgsTable(ostr, func->inputs, typesUsed);
    }

    if (func->outputs.size() > 0)
    {
        ostr << "\\textbf{Outputs:}"
             << "\\nopagebreak\n";
    }
    else
    {
        ostr << "\\textbf{ReturnType:} ";
    }

    if (func->returnType)
    {
        ostr << texEscape(func->returnType->name);
        switch(func->returnArrayDim)
        {
        case 2:
            ostr << "\\texttt{[,]}";
            break;
        case 1:
            ostr << "\\texttt{[]}";
            break;
        }
        if (func->returnType->publicType == spdoc::PublicType::CLASS)
        {
            ostr << " (see page \\pageref{type_" << func->returnType->name << "})";
        }
        if (func->returnTypeDescription.size() > 0)
        {
            ostr << "\n\n";
            writeTexDescription(ostr, func->returnTypeDescription);
        }

    }
    else if (func->outputs.size() == 0)
    {
        ostr << "void";
    }
    else
    {
        // although one parameter outputs look the same in Excel and Python
        // there is a significant difference in .NET build
        // hence we always show the table even if there is only one output
        // besides in this way we can also show the description of the
        // output
        writeArgsTable(ostr, func->outputs, typesUsed);
    }
    ostr << "\n\n";
}

std::string writeTexSimpleType(
    const spdoc::SimpleType*     construct,
    const std::string&           dirname,
    const spdoc::ServiceConstSP& service,
    const spdoc::ModuleConstSP&  module,
    bool forImport,
    std::set<std::string>&       fns)
{
    if (construct->noDoc)
        return std::string();

    char filename[256];
    std::string constructName  = ConstructName(construct, module);
    std::string systemTypename = translateTypename(construct->typeName);

    sprintf(filename, "simpleType_%s.tex", constructName.c_str());

    GeneratedOutput ostr(texFileName(dirname, filename, forImport), writeBackup());
    ostr << "\\subsection{" << constructName << "} "
         << "\\label{type_" << constructName << "}\n";

    ostr << "\n"
         << constructName << " is represented by "
         << systemTypename << " in the public interface.\n";

    ostr << "\n";
    if (construct->description.size() == 0)
        ostr << "No description.\n";
    else
        writeTexDescription(ostr, construct->description);

    ostr << "\n\n";
    fns.insert(filename);
    return filename;
}

std::string writeTexEnum(
    const spdoc::Enum*           construct,
    const std::string&           dirname,
    const spdoc::ServiceConstSP& service,
    const spdoc::ModuleConstSP&  module,
    bool forImport,
    std::set<std::string>&       fns)
{
    char filename[256];
    std::string constructName = ConstructName(construct, module);
    sprintf(filename, "enum_%s.tex", constructName.c_str());

    GeneratedOutput ostr(texFileName(dirname, filename, forImport), writeBackup());
    ostr << "\\subsection{" << constructName << "} "
         << "\\label{type_" << constructName << "}\n";

    ostr << "\n";
    if (construct->description.size() == 0)
        ostr << "No description.\n\n";
    else
        writeTexDescription(ostr, construct->description);

    if (getOptions().writeIncludes)
    {
        ostr << "\n"
            << "\\verb|#include <" << moduleClassesHeader(service, module) << ">|\n";
    }

    ostr << "\\textbf{Possible Values:}\n"
         << "\\nopagebreak\n";

    writeTexBeginTable(ostr, 3, 100, "Code", 100, "Strings", 250, "Description");

    for (size_t i = 0; i < construct->enumerands.size(); ++i)
    {
        const spdoc::EnumerandConstSP enumerand = construct->enumerands[i];
        ostr << "\\cline{1-3}\n";
        ostr << "\\raggedright \\verb|" << enumerand->code << "| &\n";
        ostr << "\\raggedright ";
        for (size_t j = 0; j < enumerand->strings.size(); ++j)
        {
            if (j > 0)
                ostr << ", ";
            if (enumerand->strings[j].length() > 0)
            {
                ostr << "\\verb|" << enumerand->strings[j] << "|";
            }
            else
            {
                ostr << "\\verb|\"\"|";
            }

            //ostr << "``" << texEscape(enumerand->strings[j]) << "''";
        }
        ostr << " &\n";
        ostr << "\\raggedright ";
        writeTexDescription(ostr, enumerand->description);
        ostr << "& \\\\\n";
    }

    writeTexEndTable(ostr, 2);

    // FIXME - handle construct->constructors

    ostr << "\n\n";
    fns.insert(filename);
    return filename;
}

std::string writeTexClassMethod(
    const spdoc::Class*          cls,
    const spdoc::ClassMethod*    method,
    const std::string&           dirname,
    const spdoc::ServiceConstSP& service,
    const spdoc::ModuleConstSP&  module,
    std::set<std::string>&       typesUsed,
    std::set<std::string>&       fns)
{
    char filename[256];
    sprintf(filename, "classMethod_%s_%s.tex", cls->dataType->name.c_str(),
        method->function->name.c_str());

    GeneratedOutput ostr(texFileName(dirname, filename), writeBackup());

    ostr << "\\subsection{" << texEscape(cls->dataType->name) << "."
         << texEscape(method->function->name) << "} "
         << "\\label{function_" << cls->dataType->name << "."
         << method->function->name << "}\n";

    ostr << "\n";
    if (method->function->description.size() == 0)
        ostr << "No description.\n";
    else
        writeTexDescription(ostr, method->function->description);

    writeTexFunctionArgs(ostr, method->function.get(), typesUsed);

    fns.insert(filename);
    return filename;
}

std::string writeTexClass(
    const spdoc::Class*          cls,
    const std::string&           dirname,
    const spdoc::ServiceConstSP& service,
    const spdoc::ModuleConstSP&  module,
    std::vector<std::string>&    allClassMethods,
    ServiceIndex&                serviceIndex,
    bool                         forImport,
    std::set<std::string>&       typesUsed,
    std::set<std::string>&       fns)
{
    char filename[256];
    std::string className = ConstructName(cls, module);
    sprintf(filename, "class_%s.tex", className.c_str());

    GeneratedOutput ostr(texFileName(dirname, filename, forImport), writeBackup());
    ostr << "\\subsection{" << texEscape(className) << "} "
         << "\\label{type_" << className << "}\n";

    ostr << "\n";
    if (cls->description.size() == 0)
        ostr << "No description.\n";
    else
        writeTexDescription(ostr, cls->description);

    if (getOptions().writeIncludes)
    {
        ostr << "\n"
            << "\\verb|#include <" << moduleClassesHeader(service, module) << ">|\n";
    }

    if (!cls->baseClassName.empty())
    {
        ostr << "\n"
             << "\\textbf{Base Class:} "
             << cls->baseClassName
             << " (see page \\pageref{type_" << cls->baseClassName << "})\n";
    }

    const std::set<std::string>& derivedClasses = serviceIndex.DerivedClasses(className);
    if (derivedClasses.size() != 0)
    {
        ostr << "\n"
             << "\\textbf{Derived Classes:}\n"
             << "\\begin{itemize}\n";

        // Assume that std::set iterates in increasing order so no need to sort
        for (std::set<std::string>::const_iterator iter = derivedClasses.begin();
             iter != derivedClasses.end(); ++iter)
        {
            ostr << "  \\item " << texEscape(*iter)
                 << " (see page \\pageref{type_" << *iter << "})\n";
        }
        ostr << "\\end{itemize}\n";
    }

    ostr << "\n"
         << "\\textbf{Attributes:}\n"
         << "\\nopagebreak\n";

    if (cls->attributes.size() == 0)
    {
        ostr << "None\n\n";
    }
    else
    {
        writeAttributesTable(ostr, cls->attributes, forImport);
    }

    if (cls->properties.size() > 0)
    {
        ostr << "\n"
             << "\\textbf{Properties:}\n"
             << "\\nopagebreak\n";
        writeAttributesTable(ostr, cls->properties, forImport);
        ostr << "Note that properties can be accessed in the same way as "
             << "attributes, but are not part of the constructor or the "
             << "serialization of the class.\n"
             << "\n";
    }

    if (!forImport)
    {

    ostr << "\\textbf{Methods:}\n"
         << "\\nopagebreak\n";

    if (cls->methods.size() == 0 && cls->coerceTo.size() == 0)
    {
        ostr << "None\n\n";
    }
    else
    {
        writeTexBeginTable(ostr, 2, 150, "Prototype", 310, "Description");
        for (size_t i = 0; i < cls->methods.size(); ++i)
        {
            const spdoc::ClassMethodConstSP& method = cls->methods[i];
            std::vector<std::string> argTypes;
            for (size_t j = 0; j < method->function->inputs.size(); ++j)
            {
                const spdoc::AttributeConstSP& attr = method->function->inputs[j];
                std::string name = translateTypename(attr->dataType->name);
                if (attr->isArray())
                    name += "\\texttt{[]}";
                argTypes.push_back(name);
            }
            const spdoc::DataTypeConstSP& returnType = method->function->returnType;
            const std::string& returnTypename = returnType ?
                translateTypename(returnType->name) : "void";

            ostr << "\\cline{1-2}\n";
            ostr << "\\raggedright ";
            if (method->isStatic)
                ostr << "static ";
            if (method->isVirtual)
                ostr << "virtual ";

            if (returnType)
            {
                ostr << texEscape(translateTypename(returnType->name));
                switch (method->function->returnArrayDim)
                {
                case 2:
                    ostr << "\\texttt{[,]}";
                    break;
                case 1:
                    ostr << "\\texttt{[]}";
                    break;
                }
                if (returnType->publicType == spdoc::PublicType::CLASS)
                {
                    ostr << " (see page \\pageref{type_" << returnType->name << "})";
                }
            }
            else
            {
                ostr << "void";
            }

            ostr << " " << texEscape(method->function->name) << "(";
            ostr << spi::StringJoin(", ", argTypes);
            ostr << ") &\n";
            ostr << "\\raggedright ";
            if (!method->implements.empty())
                ostr << "Implements " << method->implements << ".\n";
            writeTexDescription(ostr, method->function->description);
            ostr << "\nSee page \\pageref{function_" << className << "."
                 << method->function->name << "} for further details.";
            ostr << " & \\\\\n";

            allClassMethods.push_back(
                writeTexClassMethod(
                    cls, method.get(), dirname, service, module, typesUsed, fns));
        }

        for (size_t i = 0; i < cls->coerceTo.size(); ++i)
        {
            const spdoc::CoerceToConstSP& coerceTo = cls->coerceTo[i];

            ostr << "\\cline{1-2}\n";
            ostr << "\\raggedright ";
            ostr << "operator " << texEscape(coerceTo->className) << "() &\n";
            ostr << "\\raggedright ";
            ostr << "Converts to " << texEscape(coerceTo->className) << ".\n";
            writeTexDescription(ostr, coerceTo->description);
#if 0
            ostr << "\nSee page \\pageref{function_" << className
                 << ".operator." << coerceTo->className
                 << "} for further details.";
#endif
            ostr << " & \\\\\n";
#if 0
            {
                char filename[256];
                sprintf(filename, "classMethod_%s_operator_%s.tex", className.c_str(),
                    coerceTo->className.c_str());

                GeneratedOutput out(texFileName(dirname, filename));

                out << "\\subsection{" << texEscape(className) << ".operator."
                    << texEscape(coerceTo->className) << "} "
                    << "\\label{function_" << className << ".operator."
                    << coerceTo->className << "}\n";

                out << "\n";
                out << "Converts " << texEscape(className) << " to "
                    << texEscape(coerceTo->className) << ".\n\n";
                writeTexDescription(out, coerceTo->description);

                out << "\n"
                    << "\\textbf{Inputs:}\n"
                    << "\\nopagebreak\n"
                    << "None\n\n"
                    << "\\textbf{ReturnType:} "
                    << texEscape(coerceTo->className)
                    << " (see page \\pageref{type_" << coerceTo->className << "})\n"
                    << "\n";

                fns.insert(filename);
                allClassMethods.push_back(filename);
            }
#endif
        }
        writeTexEndTable(ostr, 2);
    }

    const std::set<std::string>& constructors = serviceIndex.Constructors(className);
    if (constructors.size() != 0)
    {
        ostr << "\n"
             << "\\textbf{Constructors:}\n"
             << "\\begin{itemize}\n";

        // Assume that std::set iterates in increasing order so no need to sort
        for (std::set<std::string>::const_iterator iter = constructors.begin();
             iter != constructors.end(); ++iter)
        {
            ostr << "  \\item " << texEscape(*iter)
                 << " (see page \\pageref{function_" << *iter << "})\n";
        }
        ostr << "\\end{itemize}\n";
    }

    if (cls->coerceFrom.size() > 0)
    {
        ostr << "\n"
             << "\\textbf{Coercion:}\n"
             << "\n"
             << "Instances of " << texEscape(cls->name) << " can be coerced "
             << "from other types as follows:\n";

        writeTexBeginTable(ostr, 2, 150, "Type and Name", 310, "Description");

        for (size_t i = 0; i < cls->coerceFrom.size(); ++i)
        {
            const spdoc::CoerceFromConstSP& cf       = cls->coerceFrom[i];
            const spdoc::AttributeConstSP& attr      = cf->coerceFrom;
            const spdoc::DataTypeConstSP& dataType   = attr->dataType;
            const spdoc::PublicType::Enum publicType = dataType->publicType;

            ostr << "\\cline{1-2}\n";
            ostr << "\\raggedright " << texEscape(translateTypename(dataType->name)) << " "
                 << texEscape(attr->name);

            switch(attr->arrayDim)
            {
            case 2:
                ostr << "\\texttt{[,]}";
                break;
            case 1:
                ostr << "\\texttt{[]}";
                break;
            }
            ostr << " &\n\\raggedright ";

            writeTexDescription(ostr, cf->description);

            switch(publicType)
            {
            case spdoc::PublicType::ENUM_AS_STRING:
            case spdoc::PublicType::ENUM_AS_INT:
                ostr << "\nSee " << texEscape(dataType->name)
                     << " (page \\pageref{type_" << dataType->name << "}).\n";
                break;
            case spdoc::PublicType::CLASS:
                ostr << "\nSee " << texEscape(dataType->name)
                     << " (page \\pageref{type_" << dataType->name << "}).\n";
                break;
            default:
                break;
            }
            ostr << " & \\\\\n";
        }
        writeTexEndTable(ostr, 2);
    }
    } // !forImport

    if (cls->objectName.length() > 0 && cls->objectName != cls->name)
    {
        ostr << "\n"
             << "NOTE: When this class is serialized we will use the name `"
             << texEscape(cls->objectName) << "' instead of `"
             << texEscape(cls->name) << "'\n";
    }
    fns.insert(filename);
    return filename;
}

std::string writeTexFunction(
    const spdoc::Function*       func,
    const std::string&           dirname,
    const spdoc::ServiceConstSP& service,
    const spdoc::ModuleConstSP&  module,
    bool hasClassMethods,
    std::set<std::string>& typesUsed,
    std::set<std::string>& fns)
{
    std::string moduleNamespaceSep = makeNamespaceSep(module->ns, ".");

    char filename[256];
    sprintf(filename, "function_%s%s.tex",
        moduleNamespaceSep.c_str(), func->name.c_str());

    GeneratedOutput ostr(texFileName(dirname, filename), writeBackup());
    if (hasClassMethods)
        ostr << "\\subsection{";
    else
        ostr << "\\section{";

    ostr << texEscape(moduleNamespaceSep) << texEscape(func->name) << "} "
         << "\\label{function_" << moduleNamespaceSep << func->name << "}\n";

    ostr << "\n";
    if (func->description.size() == 0)
        ostr << "No description.\n";
    else
        writeTexDescription(ostr, func->description);

    if (getOptions().writeIncludes)
    {
        ostr << "\n"
            << "\\verb|#include <" << moduleHeader(service, module) << ">|\n";
    }

    writeTexFunctionArgs(ostr, func, typesUsed);

    fns.insert(filename);
    return filename;
}

void writeTexModuleConstructs(
    const std::string&           dirname,
    const spdoc::ServiceConstSP& service,
    const spdoc::ModuleConstSP&  module,
    ServiceIndex&                serviceIndex,
    std::vector<std::string>&    allSimpleTypes,
    std::vector<std::string>&    allEnums,
    std::vector<std::string>&    allClasses,
    std::vector<std::string>&    allFunctions,
    std::vector<std::string>&    allClassMethods,
    std::set<std::string>&       typesUsed,
    std::set<std::string>&       fns)
{
    bool hasClassMethods = serviceIndex.NumClassMethods() > 0;

    for (size_t i = 0; i < module->constructs.size(); ++i)
    {
        const spdoc::ConstructConstSP& construct = module->constructs[i];
        const std::string constructType = construct->getType();

        if (constructType == "SIMPLE_TYPE")
        {
            std::string fn = writeTexSimpleType(
                ConstructCast<spdoc::SimpleType>(construct),
                dirname, service, module, false, fns);
            if (!fn.empty())
                allSimpleTypes.push_back(fn);
            writeTexSimpleType(
                ConstructCast<spdoc::SimpleType>(construct),
                dirname, service, module, true, fns);
        }
        else if (constructType == "ENUM")
        {
            allEnums.push_back(writeTexEnum(
                ConstructCast<spdoc::Enum>(construct),
                dirname, service, module, false, fns));
            writeTexEnum(
                ConstructCast<spdoc::Enum>(construct),
                dirname, service, module, true, fns);
        }
        else if (constructType == "CLASS")
        {
            allClasses.push_back(writeTexClass(
                ConstructCast<spdoc::Class>(construct),
                dirname, service, module, allClassMethods, serviceIndex, 
                false, typesUsed, fns));
            writeTexClass(
                ConstructCast<spdoc::Class>(construct),
                dirname, service, module, allClassMethods, serviceIndex,
                true, typesUsed, fns);
        }
        else if (constructType == "FUNCTION")
        {
            allFunctions.push_back(writeTexFunction(
                ConstructCast<spdoc::Function>(construct),
                dirname, service, module, hasClassMethods, typesUsed, fns));
        }
    }
}

void addFunctionToServiceIndex(
    const spdoc::ModuleConstSP& module,
    const std::string& className,
    const spdoc::Function* func,
    ServiceIndex& serviceIndex)
{
    std::ostringstream oss;
    if (!module->ns.empty())
        oss << module->ns << ".";
    if (!className.empty())
        oss << className << ".";
    oss << func->name;
    std::string funcName = oss.str();

    if (func->returnType && func->returnType->publicType == spdoc::PublicType::CLASS)
    {
        serviceIndex.AddConstructor(func->returnType->name, funcName);
    }

    for (size_t i = 0; i < func->outputs.size(); ++i)
    {
        const spdoc::AttributeConstSP& output = func->outputs[i];
        if (output->dataType->publicType == spdoc::PublicType::CLASS)
            serviceIndex.AddConstructor(output->dataType->name, funcName);
    }
}

void addModuleToServiceIndex(
    const spdoc::ModuleConstSP& module,
    ServiceIndex& serviceIndex)
{
    for (size_t i = 0; i < module->constructs.size(); ++i)
    {
        const spdoc::ConstructConstSP& construct = module->constructs[i];
        const std::string constructType = construct->getType();

        if (constructType == "CLASS")
        {
            const spdoc::Class* cls = ConstructCast<spdoc::Class>(construct);
            if (!cls->baseClassName.empty())
            {
                std::ostringstream oss;
                if (!module->ns.empty())
                    oss << module->ns << ".";
                oss << cls->name;
                std::string className = oss.str();
                serviceIndex.AddDerivedClass(cls->baseClassName, className);
            }
            size_t nbMethods = cls->methods.size();
            for (size_t i = 0; i < nbMethods; ++i)
            {
                const spdoc::ClassMethodConstSP& method = cls->methods[i];
                if (method->implements.empty())
                    addFunctionToServiceIndex(module, cls->name, method->function.get(), serviceIndex);
                serviceIndex.IncrementNumClassMethods();
            }
#if 0
            for (size_t i = 0; i < cls->coerceTo.size(); ++i)
            {
                const spdoc::CoerceToConstSP& coerceTo = cls->coerceTo[i];
                std::string constructor = spi::StringFormat(
                    "%s.operator.%s", cls->name.c_str(), coerceTo->className.c_str());
                serviceIndex.AddConstructor(coerceTo->className, constructor);
            }
#endif
        }
        else if (constructType == "FUNCTION")
        {
            const spdoc::Function* func = ConstructCast<spdoc::Function>(construct);
            addFunctionToServiceIndex(module, "", func, serviceIndex);
        }
    }
}

