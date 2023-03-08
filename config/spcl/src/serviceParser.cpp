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
#include "serviceParser.hpp"

#include <set>

#include "clibParser.hpp"
#include "configParser.hpp"
#include "configLexer.hpp"
#include "parserTools.hpp"
#include <spgtools/texUtils.hpp>
#include "types_types_helper.hpp"

#include <spi/Service.hpp>
#include <spi/RuntimeError.hpp>
#include <spi/StringUtil.hpp>
#include <spi_util/FileUtil.hpp>

namespace {

ServiceDefinitionSP serviceKeywordHandler(
    ConfigLexer& lexer,
    const std::vector<std::string>& description,
    const std::string& commandLineVersion,
    bool verbose)
{
    Options defaultOptions;

    ConfigLexer::Token token = getTokenOfType(
        lexer, SPI_CONFIG_TOKEN_TYPE_NAME, "Name", __FUNCTION__);
    std::string serviceName (token.value.aName);

    defaultOptions["namespace"] = StringConstant::Make(serviceName);
    defaultOptions["longName"] = StringConstant::Make(serviceName);
    defaultOptions["dllName"] = StringConstant::Make(serviceName);
    defaultOptions["version"] = commandLineVersion.empty() ?
        StringConstant::Make("1.0.0.0") :
        StringConstant::Make(commandLineVersion);
    defaultOptions["declSpec"]  = StringConstant::Make(
        spi::StringUpper(serviceName));
    defaultOptions["sharedPtr"] = StringConstant::Make("boost::shared_ptr");
    //defaultOptions["spDynamicCast"] = StringConstant::Make("boost::dynamic_pointer_cast");
    defaultOptions["sharedPtrInclude"] = StringConstant::Make("<boost/shared_ptr.hpp>");
    defaultOptions["baseService"] = StringConstant::Make("");
    defaultOptions["noLog"] = BoolConstant::Make(false);
    defaultOptions["useVersionedNamespace"] = BoolConstant::Make(false);
    Options options = parseOptions(lexer, ";", defaultOptions, verbose);
    getTokenOfType(lexer, ';');

    std::string baseServiceName = options["baseService"]->getString();

    ServiceDefinitionSP baseService;

    if (!baseServiceName.empty())
    {
        baseService = serviceParser(baseServiceName, commandLineVersion, verbose);
    }

    std::string version = baseServiceName.empty() ?
        options["version"]->getString() :
        baseService->getVersionString();

    ServiceDefinitionSP service = ServiceDefinition::Make(
        serviceName,
        options["dllName"]->getString(),
        options["longName"]->getString(),
        options["namespace"]->getString(),
        version,
        options["declSpec"]->getString(),
        options["sharedPtr"]->getString(),
        options["sharedPtrInclude"]->getString(),
        options["noLog"]->getBool(),
        options["useVersionedNamespace"]->getBool(),
        description);

    if (baseService)
    {
        service->setBaseService(baseService);
    }
    else
    {
        service->addStandardDataTypes();
    }
    return service;
}

void writeServiceKeywordDoc(
    GeneratedOutput& ostr,
    bool verbose)
{
    ostr << "\\subsubsection{Service Definition} \\label{service}\n"
         << "\n"
         << "The service command defines the overall characteristics of the "
         << "service. "
         << "The service is a collection of modules which each contain "
         << "data types, function definitions etc. "
         << "In total, the service is represented by a shared library for the "
         << "outer C++ layer, plus higher level shared libraries for the "
         << "different target platforms, e.g. Excel, Python, .NET.\n";

    writeTexBeginSyntax(ostr, true);
    ostr << "    %service \\emph{serviceName} ...;\n";
    writeTexEndSyntax(ostr);

    ostr << "\n"
         << "with the following optional arguments:\n";

    writeTexBeginOptionsTable(ostr);
    writeTexOptionsTableRow(
        ostr, "namespace", texValue("serviceName"), "Valid C++ identifier");
    writeTexOptionsTableRow(
        ostr, "dllName", texValue("serviceName"), "");
    writeTexOptionsTableRow(
        ostr, "longName", texValue("serviceName"), "");
    writeTexOptionsTableRow(
        ostr, "version", "1.0.0.0", "");
    writeTexOptionsTableRow(
        ostr, "declSpec",
        texValue("serviceName") + " converted to upper case",
        "Valid C++ macro definition");
    writeTexOptionsTableRow(
        ostr, "sharedPtr", "boost::shared_ptr",
        "Valid C++ class name");
    writeTexOptionsTableRow(
        ostr, "sharedPtrInclude",
        "<boost/shared_ptr.hpp>",
        "Valid C++ header file name");
    writeTexOptionsTableRow(
        ostr, "baseService", "", "");
    writeTexOptionsTableRow(
        ostr, "noLog", "False", "True or False");
    writeTexOptionsTableRow(
        ostr, "useVersionedNamespace", "False", "True or False");
    writeTexEndOptionsTable(ostr);

    ostr << "With the exception of the service description, the \\%service "
         << "command should be the first command in a service file.\n"
         << "\n"
         << "The \\texttt{serviceName} defines the name of the generated file "
         << "names for the C++ interface.\n"
         << "\n"
         << "The \\texttt{dllName} defines the name of the actual DLL and will "
         << "be defined in the properties.mk generated file.\n"
         << "The name excludes extensions such as .dll and .so which will be "
         << "added by the standard makefiles.\n"
         << "\n"
         << "The \\texttt{namespace} defines the name of functions seen by the "
         << "end-user, and also the name of some of the DLLs created for the "
         << "high level interfaces (e.g. Excel, Python).\n"
         << "\n"
         << "The parameters regarding shared pointer implementation indicate "
         << "how shared pointers are implemented in the inner library.\n"
         << "If the inner library does not implemented shared pointers, "
         << "or if it uses boost, then you can leave these options undefined.\n"
         << "\n"
         << "The \\texttt{baseName} indicates that this service relies upon "
         << "another service, and that objects created in the base service can "
         << "be used in this service.\n"
         << "\n"
         << "Setting \\texttt{noLog} to true will turn off the possibility of "
         << "logging for this service.\n"
         << "The code required to support logging is quite considerable, so "
         << "you may wish to reduce the size of your library if you have no "
         << "need for the logging feature.\n"
         << "\n\n";
}

void exportKeywordHandler(
    ConfigLexer& lexer,
    std::string& dnService,
    ServiceDefinitionSP& service,
    bool verbose)
{
    static Options defaultOptions;

    if (defaultOptions.empty())
    {
        defaultOptions["header"] = BoolConstant::Make(false);
    }

    ConfigLexer::Token token = getTokenOfType(
        lexer, SPI_CONFIG_TOKEN_TYPE_STRING, "String", __FUNCTION__);
    std::string fn = spi_util::path::join(dnService.c_str(), token.value.aString, 0);

    if (verbose)
        std::cout << "export file name: " << fn << std::endl;

    Options options = parseOptions(lexer, ";", defaultOptions, verbose);
    getTokenOfType(lexer, ';');

    if (options["header"]->getBool())
    {
        clibParser(fn, service, verbose);
    }
    else
    {
        configParser(fn, service, verbose);
    }
}

void writeExportKeywordDoc(
    GeneratedOutput& ostr,
    bool verbose)
{
    ostr << "\\subsubsection{Export Module} \\label{export}\n"
         << "\n"
         << "The \\%export command is used to define the name of a file which "
         << "contains module definitions. "
         << "Typically we will have a list of \\%export commands in a service "
         << "file. "
         << "The rules on dependency is that a subsequent module can use "
         << "data types defined by previous modules (or earlier in the same "
         << "module. "
         << "Hence the order of \\%export commands is significant. "
         << "\n"
         << "In addition (as we shall discover in more detail later), modules "
         << "can have their own namespaces. "
         << "For technical reasons it is essential that module namespaces "
         << "must be contiguous. "
         << "This means that two (or more) modules can share the same "
         << "namespace only if they are exported one after another.\n";

    writeTexBeginSyntax(ostr, true);
    ostr << "    %export \"\\emph{exportFileName}\" ...;\n";
    writeTexEndSyntax(ostr);

    ostr << "\n"
         << "with the following optional argument:\n";

    writeTexBeginOptionsTable(ostr);
    writeTexOptionsTableRow(
        ostr, "header", "False", "True or False");
    writeTexEndOptionsTable(ostr);

    ostr << "The \\%export commands must follow the \\%service command in the "
         << "service file.\n"
         << "\n"
         << "The \\texttt{exportFileName} is the name of a file expressed in "
         << "relative terms to the service file name which contains the "
         << "module definitions.\n"
         << "\n"
         << "The header option should not be used at this time.\n"
         << "\n\n";
}

void importKeywordHandler(
    ConfigLexer& lexer,
    std::string& dnService,
    ServiceDefinitionSP& service,
    bool verbose)
{
    ConfigLexer::Token token = lexer.getToken();
    bool publicImport = false;
    if (token.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD && strcmp(token.value.aKeyword, "public") == 0)
    {
        publicImport = true;
    }
    else
    {
        lexer.returnToken(token);
    }

    token = getTokenOfType(
        lexer, SPI_CONFIG_TOKEN_TYPE_STRING, "String", __FUNCTION__);
    std::string fn = spi_util::path::join(dnService.c_str(), token.value.aString, 0);

    if (verbose)
        std::cout << "import file name: " << fn << std::endl;

    types::TypesLibraryConstSP typesLibrary =
        types::TypesLibrary::from_file(fn.c_str());

    service->importTypesLibrary(typesLibrary->get_inner(), publicImport);
}

void writeImportKeywordDoc(
    GeneratedOutput& ostr,
    bool verbose)
{
    ostr << "\\subsubsection{Import Types Library} \\label{import}\n"
         << "\n"
         << "The \\%import command is used to import the types defined by "
         << "another service.\n"
         << "Whenever you generate the C++ source code for a service using "
         << "\\texttt{spcl} then a file containing information about the "
         << "data types provided by that service is also written.\n"
         << "The \\%import command will refer to that file, and as a result "
         << "in this new service you can re-use any types defined in the "
         << "previous service.\n"
         << "In particular you can use these types for inputs and outputs, "
         << "and you can sub-class any base classes.\n"
         << "\n";

    writeTexBeginSyntax(ostr, true);
    ostr << "    %import \"\\emph{importFileName}\" ...;\n";
    writeTexEndSyntax(ostr);

    ostr << "The \\%import command must follow the \\%service command in the "
         << "service file.\n"
         << "The \\%import command must be before any \\%export commands.\n"
         << "You can only have one \\%import command in the service file.\n"
         << "\n"
         << "In addition, the namespace of the types library that is loaded \n"
         << "via the \\%import command must be different from the namespace \n"
         << "for this service.\n"
         << "If you want the namespace to be the same, then you should be \n"
         << "using the \\texttt{\\$baseName} concept instead.\n"
         << "\n"
         << "Note that the generated type library file names have the "
         << "extension \\texttt{.svt}.\n"
         << "\n";
}

void voidKeywordHandler(
    ConfigLexer& lexer,
    ServiceDefinitionSP& service,
    bool verbose)
{
    // we are looking for void name() { verbatim }
    // name must be shutdown or startup

    ConfigLexer::Token token = getTokenOfType(
        lexer, SPI_CONFIG_TOKEN_TYPE_NAME);
    std::string name = token.toString();
    std::string dirname;

    if (name != "startup" && name != "shutdown" && name != "permission" && name != "init")
    {
        throw spi::RuntimeError("void %s is not understood - expecting "
                                "init, permission, startup or shutdown", name.c_str());
    }

    getTokenOfType(lexer, '(');
    if (name == "startup")
    {
        token = getTokenOfType(lexer, SPI_CONFIG_TOKEN_TYPE_NAME);
        dirname = token.toString();
    }

    getTokenOfType(lexer, ')');
    getTokenOfType(lexer, '{');

    VerbatimConstSP verbatim = lexer.getVerbatim();
    if (name == "startup")
        service->addStartup(verbatim, dirname);
    else if (name == "shutdown")
        service->addShutdown(verbatim);
    else if (name == "permission")
        service->addPermission(verbatim);
    else if (name == "init")
        service->addServiceInit(verbatim);
}

void writeStartupFunctionDoc(
    GeneratedOutput& ostr,
    bool verbose)
{
    ostr << "\\subsubsection{Startup function} \\label{startup}\n";

    writeTexBeginSyntax(ostr, false);
    ostr << "    void startup(\\emph{dirname}) \\{ verbatim \\}\n";
    writeTexEndSyntax(ostr);

    ostr << "\n"
         << "The void keyword indicates that we are defining either a startup "
         << "or shutdown function.\n"
         << "If defined, then the startup function is called (with one "
         << "parameter which is a string giving the directory name of the "
         << "DLL) when the service DLL is loaded.\n"
         << "\n"
         << "The purpose is to provide extra initialisation functionality "
         << "in addition to that provided as standard by SPI.\n"
         << "\n"
         << "For example, you may wish to read some initialisation files.\n"
         << "This is why the input parameter of the DLL is provided, so that "
         << "as part of your package you can bundle initialisation files in "
         << "the same directory as the DLLs.\n"
         << "\n\n";
}

void writeShutdownFunctionDoc(
    GeneratedOutput& ostr,
    bool verbose)
{
    ostr << "\\subsubsection{Shutdown function} \\label{shutdown}\n";

    writeTexBeginSyntax(ostr, false);
    ostr << "    void shutdown() \\{ verbatim \\}\n";
    writeTexEndSyntax(ostr);

    ostr << "\n"
         << "The void keyword indicates that we are defining either a startup "
         << "or shutdown function.\n"
         << "If defined, then the shutdown function is called (without "
         << "parameters) when the service is deleted.\n"
         << "\n"
         << "The purpose is to provide extra termination functionality "
         << "in addition to that provided as standard by SPI.\n"
         << "\n"
         << "For example, you may wish to release memory creating by various "
         << "caches defined by the application software.\n"
         << "\n\n";
}

void commandHandler(ServiceDefinitionSP& service,
                    const std::string& command,
                    bool verbose)
{
    if (spi::StringStartsWith(command, "include "))
        service->addInclude(command.substr(8));
    else
        throw spi::RuntimeError("Command %s must be #include command",
                                command.c_str());
}

} // end of anonymous namespace

ServiceDefinitionSP serviceParser(
    const std::string& fn,
    const std::string& commandLineVersion,
    bool verbose)
{
    std::ifstream istr(fn.c_str());
    if (!istr)
        throw spi::RuntimeError("Could not open %s", fn.c_str());

    std::string dn = spi_util::path::dirname(fn);
    ConfigLexer lexer(fn, &istr);
    DescriptionParser desc;

    ConfigLexer::Token token = desc.consume(lexer, verbose);
    ServiceDefinitionSP service;

    while (token.type)
    {
        if (token.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD)
        {
            std::string keyword = token.value.aKeyword;

            if (keyword == "%service")
            {
                if (service)
                    throw spi::RuntimeError("Multiple definitions of %%service");
                service = serviceKeywordHandler(lexer, desc.take(),
                    commandLineVersion, verbose);
            }
            else if (keyword == "%export")
            {
                if (!service)
                {
                    throw spi::RuntimeError("%%service must be defined before "
                                            "%%export");
                }
                desc.clear(); // description ignored here
                exportKeywordHandler(lexer, dn, service, verbose);
            }
            else if (keyword == "%import")
            {
                if (!service)
                {
                    throw spi::RuntimeError("%%service must be defined before "
                                            "%%import");
                }
                desc.clear(); // description ignored here
                importKeywordHandler(lexer, dn, service, verbose);
            }
            else if (keyword == "void")
            {
                if (!service)
                {
                    throw spi::RuntimeError("%%service must be defined before "
                                            "void");
                }
                desc.clear(); // description ignored here
                voidKeywordHandler(lexer, service, verbose);
            }
            else
            {
                throw spi::RuntimeError("%s: Unexpected keyword %s",
                                        __FUNCTION__, keyword.c_str());
            }
        }
        else if (token.type == SPI_CONFIG_TOKEN_TYPE_COMMAND)
        {
            commandHandler(service, std::string(token.value.aCommand), verbose);
        }
        else
        {
            std::cout << lexer.filename << ":" << lexer.getLineNumber()
                      << ": [" << token.toString() << "]" << std::endl;
            desc.clear(); // normally we would take the description
        }
        token = desc.consume(lexer);
    }
    if (lexer.num_errors > 0)
        throw spi::RuntimeError("Errors detected parsing %s", fn.c_str());
    return service;
}

void writeServiceParserDoc(
    GeneratedOutput& ostr,
    bool verbose)
{
    ostr << "\\newpage\n"
         << "\\subsection{SPI service file reference}\n\n";

    writeServiceKeywordDoc(ostr, verbose);
    writeImportKeywordDoc(ostr, verbose);
    writeExportKeywordDoc(ostr, verbose);
    writeStartupFunctionDoc(ostr, verbose);
    writeShutdownFunctionDoc(ostr, verbose);

    writeConfigParserDoc(ostr, verbose);
}

