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
#include "configParser.hpp"

#include <set>

#include "configLexer.hpp"
#include "parserTools.hpp"
#include "construct.hpp"
#include "enum.hpp"
#include "converter.hpp"
#include "function.hpp"
#include "simpleType.hpp"
#include "struct.hpp"
#include "wrapperClass.hpp"
#include "mapClass.hpp"
#include "moduleDefinition.hpp"
#include "attribute.hpp"
#include "innerClass.hpp"
#include "classMethod.hpp"
#include <spgtools/texUtils.hpp>
#include "coerceTo.hpp"

#include <spi/Service.hpp>
#include <spi/RuntimeError.hpp>
#include <spi/StringUtil.hpp>

#include <string.h>

namespace {

ModuleDefinitionSP moduleKeywordHandler(
    ConfigLexer& lexer,
    const std::vector<std::string>& description,
    bool verbose)
{
    ConfigLexer::Token token = getTokenOfType(
        lexer, SPI_CONFIG_TOKEN_TYPE_NAME, "Name", __FUNCTION__);
    std::string moduleName (token.value.aName);

    ParserOptions defaultOptions;
    defaultOptions["namespace"] = StringConstant::Make("");

    ParserOptions options = parseOptions(lexer, ";", defaultOptions, verbose);
    getTokenOfType(lexer, ';');

    return ModuleDefinition::Make(moduleName, description,
        options["namespace"]->getString());
}

void writeModuleDoc(GeneratedOutput& ostr, bool verbose)
{
    ostr << "\\subsubsection{Module} \\label{module}\n"
         << "\n"
         << "A module defines a collection of constructs (functions, classes, "
         << "other data types etc.). "
         << "The module corresponds to a set of C++ file names to be "
         << "generated for each module.\n"
         << "\n";
    writeTexBeginSyntax(ostr, true);
    ostr << "    \\%module \\emph{\\$moduleName} ...;\n";
    writeTexEndSyntax(ostr);

    ostr << "\n"
         << "with the following optional arguments:\n";

    writeTexBeginOptionsTable(ostr);
    writeTexOptionsTableRow(
        ostr, "namespace", "", "Valid C++ identifier");
    writeTexEndOptionsTable(ostr);

    ostr << "Note that the module definition must come before any other "
         << "definitions within the file.\n"
         << "\n"
         << "The optional module namespace provides a namespace for all the "
         << "definitions within the file. "
         << "Two or more modules can share the same namespace, but the "
         << "namespace definitions using the same namespace must be "
         << "contiguous in the list of modules exported from the service "
         << "file.\n"
         << "\n"
         << "The reason for this constraint is that not all target platforms "
         << "support the concept of a namespace in the same manner as C++. "
         << "In particular, for the .NET interface we implement a namespace "
         << "by creating a class instead. "
         << "Since we cannot split the definition of a class, it is therefore "
         << "becomes a requirement that the namespace definitions are "
         << "contiguous.\n"
         << "\n"
         << "\n";
}

// call this function when you are expecting some inner typename
//
// it will accept any tokens up to a matching ">" with the initial
// "<" taken as read
//
// it returns the full string which you may need to re-parse
std::string parseInnerType(
    ConfigLexer& lexer)
{
    VerbatimConstSP verbatim = lexer.getVerbatim('<', '>');
    std::string tmp = spi::StringJoin("\n", verbatim->getCode());
    SPI_POST_CONDITION(spi::StringEndsWith(tmp, ">"));
    return spi::StringStrip(tmp.substr(0, tmp.length()-1));
}

// call this function when you are expecting a DataType
//
// it will accept tokens of type keyword or name
// and will then see if this string already matches a DataType defined
// in the service
DataTypeConstSP parseDataType(
    const ConfigLexer::Token& token,
    ConfigLexer& lexer,
    const ModuleDefinitionSP& module,
    const ServiceDefinitionSP& service,
    bool allowVoid=false)
{
    std::string typeName;
    bool isPublic = false;
    ConfigLexer::Token thisToken = token;

    if (token.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD && strcmp(token.value.aKeyword, "public") == 0)
    {
        isPublic = true;
        thisToken = lexer.getToken();
    }

    switch(thisToken.type)
    {
    case SPI_CONFIG_TOKEN_TYPE_NAME:
        typeName = thisToken.value.aName;
        break;
    case SPI_CONFIG_TOKEN_TYPE_KEYWORD:
        typeName = thisToken.value.aKeyword;
        break;
    default:
        SPI_THROW_RUNTIME_ERROR("Expecting DataType - actual was (" << thisToken.toString() << ")");
    }

    if (allowVoid && typeName == "void")
        return DataTypeConstSP();

    // we allow compound data types using '.' as the namespace separator
    ConfigLexer::Token nextToken = lexer.getToken();
    while (nextToken.type == '.')
    {
        nextToken = lexer.getToken();
        if (nextToken.type == SPI_CONFIG_TOKEN_TYPE_NAME)
        {
            typeName = typeName + "." + nextToken.value.aName;
        }
        else
        {
            throw spi::RuntimeError(
                "Expecting next part of DataType - actual was (%s)",
                nextToken.toString().c_str());
        }
        nextToken = lexer.getToken();
    }
    lexer.returnToken(nextToken);

    DataTypeConstSP dataType = service->getDataType(
        typeName, module->moduleNamespace(), isPublic);

    if (!dataType)
    {
        if (isPublic)
        {
            dataType = service->getDataType(typeName, module->moduleNamespace());
            if (dataType)
            {
                SPI_THROW_RUNTIME_ERROR("DataType " << typeName << " is not defined as public");
            }
        }
        SPI_THROW_RUNTIME_ERROR("DataType " << typeName << " is not defined");
    }

    return dataType;
}

void writeDataTypeDoc(
    GeneratedOutput& ostr,
    bool verbose)
{
    ostr << "\\subsubsection{DataType} \\label{dataType}\n"
         << "\n"
         << "A data type is defined by a sequence of tokens separated by "
         << "\\texttt{`.'} where we use the \\texttt{`.'} to separate the "
         << "namespace elements of the data type.\n"
         << "\n"
         << "Data types can be user defined or built-in data types. "
         << "The following data types are built into the SPI language:\n";

    writeTexBeginTable(ostr, 2, 80, "Typename", 300, "Description");
    writeTexTableRow(ostr, 2,
                     "int", "int in C++");
    writeTexTableRow(ostr, 2,
                     "long", "long in C++ - represented as int in the public "
                     "interface - but converted automatically to/from int");
    writeTexTableRow(ostr, 2,
                     "double", "double in C++");
    writeTexTableRow(ostr, 2,
                     "char", "char in C++");
    writeTexTableRow(ostr, 2,
                     "bool", "bool in C++");
    writeTexTableRow(ostr, 2,
                     "size_t", "size_t in C++ - represented as int in the "
                     "public interface - but converted (and validated) "
                     "automatically to/from int");
    writeTexTableRow(ostr, 2,
                     "string", "std::string in C++");
    writeTexTableRow(ostr, 2,
                     "date", "spi::Date in C++");
    writeTexTableRow(ostr, 2,
        "Variant", "spi::Variant in C++ - this is a type that can represent "
                   "any type and generally the interface code will resolve "
                   "this to one or more concrete types as required");
    writeTexTableRow(ostr, 2,
                     "void",
                     "For return types only - indicates no value is returned "
                     "from the function or class method.");
    writeTexEndTable(ostr, 2);
    ostr << "User defined data types can be defined using the \\texttt{enum} "
         << "keyword (see page \\pageref{enum}), the \\texttt{typedef} "
         << "keyword (see page \\pageref{simpleType}), the \\texttt{struct} "
         << "keyword (see page \\pageref{struct}) or the \\texttt{class} "
         << "keyword (see page \\pageref{class}).\n"
         << "\n";
}

// call this function when you are expecting an array definition
// expect to do this immediately after a DataType definition
int parseArrayDim(ConfigLexer& lexer)
{
    int arrayDim = 0;
    ConfigLexer::Token token = lexer.getToken();
    while (token.type == '[')
    {
        getTokenOfType(lexer, ']');
        arrayDim += 1;
        token = lexer.getToken();
    }
    lexer.returnToken(token);

    if (arrayDim > 2)
        throw spi::RuntimeError(
            "Does not support %d-dimensional arrays",
            arrayDim);

    return arrayDim;
}

void writeArrayDimDoc(
    GeneratedOutput& ostr,
    bool verbose)
{
    ostr << "\\subsubsection{ArrayDim} \\label{arrayDim}\n"
         << "\n"
         << "Array dimensions can be used following a data type to indicate "
         << "that the attribute (or return type) is in factor an array of "
         << "some sort."
         << "\n"
         << "The simplest use is \\texttt{[]} to indicate a vector.\n"
         << "\n"
         << "The sequence \\texttt{[][]} indicates a vector of vectors.\n"
         << "Each vector can be a different size - in other words this is a "
         << "ragged 2-dimensional array.\n"
         << "\n"
         << "The sequence \\texttt{[,]} indicates a rectangular matrix.\n";

    ostr << "\n"
         << "\\textit{Actually the last two paragraphs are aspirational - "
         << "at present we only support rectangular matrix using the "
         << "\\texttt{[][]} syntax.}\n";
    ostr << "\n"
         << "We only support arrays of either one or two-dimensions.\n";
}

bool isNamedToken(ConfigLexer::Token& token, const char* name)
{
    if (token.type != SPI_CONFIG_TOKEN_TYPE_NAME)
        return false;
    if (strcmp(token.value.aName, name) == 0)
        return true;
    return false;
}

// call this function with the description and data type of an attribute
// the attribute might well be an array (indicated by [] after the name)
// other than that the next token is returned to the lexer ready for the
// next consumer
FunctionAttributeConstSP parseFunctionAttribute(
    ConfigLexer& lexer,
    const ModuleDefinitionSP& module,
    const ServiceDefinitionSP& service)
{
    DescriptionParser desc;
    ConfigLexer::Token token = desc.consume(lexer);

    bool innerConst = false;
    if (token.toString() == "const")
    {
        innerConst = true;
        token = lexer.getToken();
    }

    DataTypeConstSP dataType = parseDataType(
        token, lexer, module, service);
    int arrayDim = parseArrayDim(lexer);

    token = lexer.getToken();
    bool isOutput = false;
    if (token.type == '&')
        isOutput = true;
    else
        lexer.returnToken(token);

    token = getTokenOfType(lexer, SPI_CONFIG_TOKEN_TYPE_NAME, "Name");
    std::string name(token.value.aString);

    bool isOptional = false;
    ConstantConstSP defaultValue;

    token = lexer.getToken();
    switch (token.type)
    {
    case '=': // optional with default
        isOptional = true;
        defaultValue = parseConstant(lexer);
        break;
    case '?': // optional without default
        isOptional = true;
        defaultValue = UndefinedConstant::Make();
        break;
    default:
        lexer.returnToken(token);
        break;
    }

    AttributeConstSP attr = Attribute::Make(
        desc.take(), dataType, name, arrayDim, isOptional, defaultValue);

    return FunctionAttribute::Make(attr, isOutput);
}

// call this function when you are expecting a list of attributes as
// arguments to a function declaration
// the initial ( is assumed
std::vector<FunctionAttributeConstSP> parseFunctionArgs(
    ConfigLexer& lexer,
    const ModuleDefinitionSP& module,
    const ServiceDefinitionSP& service,
    bool verbose)
{
    std::vector<FunctionAttributeConstSP> args;

    ConfigLexer::Token token = lexer.getToken();
    bool commaExpected = false;
    while (token.type != ')')
    {
        if (commaExpected)
        {
            if (token.type != ',')
            {
                throw spi::RuntimeError(
                    "Comma expected to separate function args");
            }
        }
        else
        {
            lexer.returnToken(token);
        }

        FunctionAttributeConstSP arg = parseFunctionAttribute(lexer, module, service);
        args.push_back(arg);

        commaExpected = true;
        token = lexer.getToken();
    }

    return args;
}

// call this function when you are expecting a list of attributes as
// arguments to a function declaration and outputs are not allowed
// the initial ( is assumed
std::vector<AttributeConstSP> parseFunctionInputArgs(
    ConfigLexer& lexer,
    const ModuleDefinitionSP& module,
    const ServiceDefinitionSP& service,
    bool verbose)
{
    std::vector<FunctionAttributeConstSP> funcArgs = parseFunctionArgs(
        lexer, module, service, verbose);

    size_t N = funcArgs.size();

    for (size_t i = 0; i < N; ++i)
    {
        if (funcArgs[i]->isOutput())
            throw spi::RuntimeError("Cannot define parameter %s as an output",
                funcArgs[i]->attribute()->name());
    }

    return FunctionAttribute::getAttributeVector(funcArgs);
}

// call this function when you want to parse converter code for struct
// or class attribute
//
// converter code should appear after accessor code
//
// hence we need to test for "convert" at the point we test for ; or {
// and again after the accessor code
ConverterConstSP parseConverterCode(
    ConfigLexer& lexer,
    const ModuleDefinitionSP& module,
    const ServiceDefinitionSP& service,
    bool verbose)
{
    ConfigLexer::Token token = lexer.getToken();
    if (token.type != '(')
        throw spi::RuntimeError("Expecting ( to define convert attribute(s)");

    std::vector<AttributeConstSP> args = parseFunctionInputArgs(lexer, module, service, verbose);

    token = lexer.getToken();

    if (token.type != '{')
        throw spi::RuntimeError("Expecting { to start convert code");
    VerbatimConstSP code = lexer.getVerbatim();
    return Converter::Make(args, code);
};


ClassAttributeConstSP parseClassAttribute(
    ConfigLexer& lexer,
    ClassAttributeAccess accessLevel,
    const std::vector<std::string>& description,
    const ModuleDefinitionSP& module,
    const ServiceDefinitionSP& service)
{
    ConfigLexer::Token token = lexer.getToken();

    bool innerConst = false;
    if (token.toString() == "const")
    {
        innerConst = true;
        token = lexer.getToken();
    }

    DataTypeConstSP dataType = parseDataType(token, lexer, module, service);
    int arrayDim = parseArrayDim(lexer);

    token = getTokenOfType(lexer, SPI_CONFIG_TOKEN_TYPE_NAME, "Name");
    std::string name(token.value.aString);

    bool isOptional = false;
    ConstantConstSP defaultValue;

    token = lexer.getToken();
    switch (token.type)
    {
    case '=': // optional with default
        isOptional = true;
        defaultValue = parseConstant(lexer);
        break;
    case '?': // optional without default
        isOptional = true;
        defaultValue = UndefinedConstant::Make();
        break;
    default:
        lexer.returnToken(token);
        break;
    }

    AttributeConstSP attr = Attribute::Make(
        description, dataType, name, arrayDim, isOptional, defaultValue);

    VerbatimConstSP code;

    static ParserOptions accessorDefaultOptions;
    if (accessorDefaultOptions.size() == 0)
    {
        accessorDefaultOptions["noConvert"] = BoolConstant::Make(false);
        accessorDefaultOptions["canHide"] = BoolConstant::Make(false);
        accessorDefaultOptions["hideIf"] = StringConstant::Make();
        accessorDefaultOptions["noCopy"] = BoolConstant::Make(false);
    }
    ParserOptions accessorOptions;
    accessorOptions = parseOptions(lexer, "{;", accessorDefaultOptions, false);

    bool noConvert = getOption(accessorOptions, "noConvert")->getBool();
    bool canHide = getOption(accessorOptions, "canHide")->getBool();
    std::string hideIf = getOption(accessorOptions, "hideIf")->getString();
    bool noCopy = getOption(accessorOptions, "noCopy")->getBool();
    token = lexer.getToken();
    ConverterConstSP converter;
    if (token.type == '{')
    {
        code = lexer.getVerbatim();
        token = lexer.getToken();
        if (isNamedToken(token, "convert"))
        {
            converter = parseConverterCode(lexer, module, service, false);
        }
        else
        {
            lexer.returnToken(token);
        }
    }
    else if (isNamedToken(token, "convert"))
    {
        converter = parseConverterCode(lexer, module, service, false);
    }
    else if (token.type != ';')
    {
        throw spi::RuntimeError(
            "Expecting code or ';' - actual was (%s)",
            token.toString().c_str());
    }
    ClassAttributeConstSP classAttribute = ClassAttribute::Make(
        attr, accessLevel, code, noConvert, canHide, hideIf, noCopy, converter);

    return classAttribute;
}

void writeFunctionParamDoc(
    GeneratedOutput& ostr,
    bool verbose)
{
    ostr << "\\subsubsection{Function Parameter} \\label{functionParam}\n"
         << "\n"
         << "A function parameter consists of a data type followed by an "
         << "optional array indicator followed by an identifier. "
         << "The parameter can be an input parameter or an output parameter "
         << "(to be returned by reference) for a function or class method.\n"
         << "\n"
         << "Parameters are comma separated and enclosed in parantheses.\n"
         << "\n";
    writeTexBeginSyntax(ostr, true);
    ostr << "    \\emph{dataType}[[]] [&] \\emph{name} [?|=\\emph{default}]\n";
    writeTexEndSyntax(ostr);

    ostr << "\n"
         << "Use of an array definition starting with \\texttt{[} indicates that "
         << "the attribute is an array.\n"
         << "See ArrayDim (page \\pageref{arrayDim}) for further details.\n"
         << "\n"
         << "Use of \\& after the \\texttt{\\$dataType} indicates that this is "
         << "an output parameter to a function or class method. "
         << "This is an unusual requirement. "
         << "In general outputs are returned by value via the return type, "
         << "but if you want to return more than one value then via C++ the "
         << "simplest way is to use output parameters passed by reference. "
         << "In the higher level interfaces, output parameters might well "
         << "returned from the function in some sort of array format. "
         << "For example, in Excel you have no other choice. "
         << "In Python, we return multiple outputs via a tuple.\n"
         << "\n"
         << "Use of = followed by a valid value indicates that the parameter "
         << "is optional and the value following the equal sign is the default "
         << "value to be used in case the user has not provided a value.\n"
         << "\n"
         << "Use of ? indicates that the parameter is optional and the default "
         << "value is unspecified.\n"
         << "For attributes of type Object the default value would be a NULL "
         << "Object and therefore not need to be specified.\n"
         << "For date attributes, the default value would be date zero.\n"
         << "\n";
}

// call this function when you are expecting some implementation
// no implementation can be marked by ;
// in some cases you must have implementation
VerbatimConstSP parseImplementation(
    ConfigLexer& lexer, bool mustHaveImplementation=false)
{
    ConfigLexer::Token token = lexer.getToken();
    switch (token.type)
    {
    case ';':
        if (!mustHaveImplementation)
            return VerbatimConstSP();
        break;
    case '{':
        return lexer.getVerbatim();
    }
    throw spi::RuntimeError("Implementation expected - actual was (%s)",
                            token.toString().c_str());
}

VerbatimConstSP parseAsValueCode(
    ConfigLexer& lexer)
{
    ConfigLexer::Token token = lexer.getToken();
    if (token.type != '(')
        throw spi::RuntimeError("Expecting '(' after as_value");
    token = lexer.getToken();
    if (token.type != ')')
        throw spi::RuntimeError("Expecting ')' after as_value");

    return parseImplementation(lexer, true);
}
//
//// call this function when you are expecting a list of attributes as
//// arguments to a function declaration
//// the initial ( is assumed
//std::vector<FunctionAttributeConstSP> parseFunctionArgs(
//    ConfigLexer& lexer,
//    const ModuleDefinitionSP& module,
//    const ServiceDefinitionSP& service,
//    bool verbose)
//{
//    std::vector<FunctionAttributeConstSP> args;
//
//    ConfigLexer::Token token = lexer.getToken();
//    bool commaExpected = false;
//    while (token.type != ')')
//    {
//        if (commaExpected)
//        {
//            if (token.type != ',')
//            {
//                throw spi::RuntimeError(
//                    "Comma expected to separate function args");
//            }
//        }
//        else
//        {
//            lexer.returnToken(token);
//        }
//
//        FunctionAttributeConstSP arg = parseFunctionAttribute(lexer, module, service);
//        args.push_back(arg);
//
//        commaExpected = true;
//        token = lexer.getToken();
//    }
//
//    return args;
//}

CoerceFromConstSP parseCoerceFrom(
    ConfigLexer& lexer,
    const std::string& className,
    const std::vector<std::string>& description,
    const ModuleDefinitionSP& module,
    const ServiceDefinitionSP& service,
    bool verbose)
{
    // we have already parsed the typename
    // hence we first expect (
    // then one parameter
    // then )
    // then verbatim (mandatory)
    ConfigLexer::Token token = lexer.getToken();

    if (token.type != '(')
    {
        throw spi::RuntimeError(
            "Expect '(' after %s for CoerceFrom constructor",
            className.c_str());
    }

    std::vector<FunctionAttributeConstSP> args = parseFunctionArgs(
        lexer, module, service, verbose);

    if (args.size() != 1)
    {
        throw spi::RuntimeError(
            "Expect one parameter for CoerceFrom constructor for %s",
            className.c_str());
    }

    if (args[0]->isOutput())
        throw spi::RuntimeError(
            "Parameter for CoerceFrom should not be an output");

    static ParserOptions defaultOptions;
    if (defaultOptions.size() == 0)
    {
        defaultOptions["convert"] = BoolConstant::Make(false);
    }

    ParserOptions options;
    options = parseOptions(lexer, "{", defaultOptions, verbose);
    bool convert = getOption(options, "convert")->getBool();

    VerbatimConstSP code = parseImplementation(lexer, true);

    return CoerceFrom::Make(description, args[0]->attribute(), code, convert);
}

CoerceToConstSP parseCoerceTo(
    ConfigLexer& lexer,
    const std::string& className,
    const std::vector<std::string>& description,
    const ModuleDefinitionSP& module,
    const ServiceDefinitionSP& service,
    bool verbose)
{
    // we have already parsed the operator keyword
    //
    // hence we expect TargetClass() { verbatimCode }
    DataTypeConstSP targetType = parseDataType(lexer.getToken(),
        lexer, module, service);

    if (targetType->publicType() != spdoc::PublicType::CLASS)
        throw spi::RuntimeError("%s is not a class", targetType->name().c_str());

    std::string targetClassName = targetType->name();
    ConfigLexer::Token token = lexer.getToken();
    if (token.type != '(')
    {
        throw spi::RuntimeError(
            "Expect '(' after operator %s", targetClassName.c_str());
    }

    token = lexer.getToken();
    if (token.type != ')')
    {
        throw spi::RuntimeError(
            "Expect ')' after operator %s(", targetClassName.c_str());
    }
    VerbatimConstSP code = parseImplementation(lexer, true);
    return CoerceTo::Make(description, targetType, code);
}


/**
 * Defines information about a function definition when the function is in
 * fact a method of a class.
 */
struct MethodContext
{
    MethodContext(
        const std::string&  className,
        const ClassConstSP& baseClass,
        bool isVirtual,
        bool isStatic,
        bool isDelegate)
        :
        className(className),
        baseClass(baseClass),
        isVirtual(isVirtual),
        isStatic(isStatic),
        isDelegate(isDelegate),
        isConst(false),
        implements()
    {}

    const std::string  className;
    const ClassConstSP baseClass;
    const bool isVirtual;
    const bool isStatic;
    const bool isDelegate;

    bool        isConst;
    std::string implements;

    bool isImplementation(const std::string& name)
    {
        if (!baseClass)
            return false;

        return baseClass->isVirtualMethod(name);
    }
};

FunctionConstSP parseFunction(
    ConfigLexer&                    lexer,
    const std::vector<std::string>& description,
    ModuleDefinitionSP&             module,
    ServiceDefinitionSP&            service,
    bool                            mustImplement,
    MethodContext*                  methodContext,
    bool                            verbose)
{
    // an extern function has the following tokens
    //
    // extern <return_type> Name(
    //    <description> data_type name, ...)
    // { code }
    //
    // possibility of there not being any code in which case we expect ;
    // or no inputs
    //
    // if methodContext is provided then we are dealing with a method of
    // a class instead of a stand-alone function
    //
    // the extern token is taken as read (or else static or virtual as well
    // for class methods)
    //
    // however there are a lot of similarities so this justifies using the
    // same routine

    DescriptionParser returnTypeDesc;
    ConfigLexer::Token token   = returnTypeDesc.consume(lexer, verbose);

    bool returnInnerConst = false;
    if (token.toString() == "const")
    {
        returnInnerConst = true;
        token = lexer.getToken();
    }

    DataTypeConstSP returnType = parseDataType(
        token, lexer, module, service, true);
    int returnArrayDim = parseArrayDim(lexer);

    token = getTokenOfType(lexer, SPI_CONFIG_TOKEN_TYPE_NAME, "Name");
    std::string name(token.value.aName);

    if (name == "BinaryOp")
    {
        int wait = 1;
    }

    token = getTokenOfType(lexer, '(', "(");

    const std::vector<FunctionAttributeConstSP>& args =
        parseFunctionArgs(lexer, module, service, verbose);

    // for class methods we need to check for whether the method is
    // declared as const
    if (methodContext && !methodContext->isStatic)
    {
        token = lexer.getToken();
        if (token.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD &&
            strcmp(token.value.aKeyword, "const") == 0)
        {
            methodContext->isConst = true;
        }
        else
        {
            methodContext->isConst = false;
            lexer.returnToken(token);
        }
    }

    static ParserOptions defaultOptions;
    if (defaultOptions.size() == 0)
    {
        defaultOptions["noLog"]        = BoolConstant::Make(service->noLog());
        defaultOptions["noConvert"]    = BoolConstant::Make(false);
        defaultOptions["excelOptions"] = StringConstant::Make("");
        defaultOptions["ignore"]       = BoolConstant::Make(false);
        defaultOptions["cache"]        = IntConstant::Make(0);
    }

    ParserOptions options;
    options = parseOptions(lexer, "{;", defaultOptions, verbose);
    bool ignore = getOption(options, "ignore")->getBool();

    // at the end of the function we either have some code started by '{'
    // or the function is not implemented indicated by ;
    const VerbatimConstSP& implementation =
        parseImplementation(lexer, mustImplement);

    if (methodContext)
    {
        bool isImplementation = methodContext->isImplementation(name);
        std::string ns = module->moduleNamespace();

        if (isImplementation)
            methodContext->implements = methodContext->baseClass->getName(true, ".");

        // TBD: match prototypes when isImplementation == true
    }

    if (ignore)
        return FunctionConstSP();

    FunctionConstSP func = Function::Make(
        description, returnTypeDesc.take(),
        returnType, returnArrayDim, name, module->moduleNamespace(),
        args, implementation,
        getOption(options, "noLog")->getBool(),
        getOption(options, "noConvert")->getBool(),
        spi_util::StringSplit(getOption(options, "excelOptions")->getString(), ';'),
        getOption(options, "cache")->getInt());

    if (func->hasIgnored())
    {
        std::cerr << "Function " << name << " uses ignored dataTypes - function not added" << std::endl;
        return FunctionConstSP();
    }
    return func;
}

void externKeywordHandler(
    ConfigLexer& lexer,
    const std::vector<std::string>& description,
    ModuleDefinitionSP& module,
    ServiceDefinitionSP& service,
    bool mustImplement,
    bool verbose)
{
    FunctionConstSP func = parseFunction(
        lexer,
        description,
        module,
        service,
        mustImplement,
        NULL,
        verbose);

    if (func)
        module->addConstruct(func);
}

void writeFunctionDoc(
    GeneratedOutput& ostr,
    bool verbose)
{
    ostr << "\\subsubsection{Function} \\label{extern}\n"
         << "\n"
         << "Wrapper functions are defined by the \\texttt{extern} keyword. "
         << "The function definition has a return type, function name, input "
         << "and output parameters, plus the implementation code.\n"
         << "\n"
         << "The definition of the return type and function parameters are in "
         << "terms of the outer data types. "
         << "The implementation code should be written in terms of the inner "
         << "data types. "
         << "The conversion between outer and inner types (and vice versa) is "
         << "handled by the middle layer generated by the SPCL code "
         << "generator.\n";

    writeTexBeginSyntax(ostr, true);
    ostr << "    extern \\emph{returnType} [ [] ] \\emph{functionName} (\\emph{params}) ...\n"
         << "    \\{ implementation \\}\n";
    writeTexEndSyntax(ostr);

    ostr << "with the following optional arguments:\n";

    writeTexBeginOptionsTable(ostr);
    writeTexOptionsTableRow(
        ostr, "noLog", "False", "True or False");
    writeTexOptionsTableRow(
        ostr, "noConvert", "False", "True or False");
    writeTexOptionsTableRow(
        ostr, "excelOptions", "",
        "Comma separated options specifically for excel, "
        "namely volatile or hidden.");
    writeTexEndOptionsTable(ostr);

    ostr << "The \\emph{extern} keyword defines a stand-alone function call.\n"
         << "Note that if \\texttt{extern} is used inside a class then it "
         << "defines a method of the class (also known as member function).\n"
         << "\n"
         << "\\texttt{\\$returnType} is the data type (see page "
         << "\\pageref{dataType}), returned by the function although it can "
         << "also be \\texttt{void}.\n"
         << "Use of \\texttt{[]} after the \\texttt{\\$returnType} indicates "
         << "that the function returns an array, typically represented by "
         << "\\texttt{std::vector} within the C++ code.\n"
         << "\n"
         << "\\texttt{\\$functionName} is the function name visible at the "
         << "outer C++ layer and higher level interfaces. "
         << "At the outer C++ layer it will be within the top level namespace "
         << "of the service, and also within the (optional) namespace defined "
         << "at the module level. "
         << "By choosing the service namespace wisely you can avoid any symbol "
         << "name clashes between the outer library symbol names and the inner "
         << "library symbol names. "
         << "As a result you will often be able to use the same name for the "
         << "function in the outer library as in the inner library.\n"
         << "\n"
         << "\\texttt{\\$params} defines a comma separated list of function "
         << "parameters (see page \\pageref{functionParam}). "
         << "The function parameters are enclosed in $(...)$.\n"
         << "\n"
         << "Options for the function appear after the function parameters and "
         << "before the function implementation.\n"
         << "\n"
         << "Normally logging is available by default for all functions.\n"
         << "However for certain simple functions it is better in practice to "
         << "turn logging off at all times - this can avoid clogging up log "
         << "files with trivial functions.\n"
         << "This feature is controlled by the \\texttt{\\$noLog} option.\n"
         << "\n"
         << "Normally the function call uses values of the corresponding inner "
         << "type for each of the inputs.\n"
         << "The generated code in such cases is responsible for converting "
         << "the input value from the outer type to the inner type.\n"
         << "However sometimes you do not wish to convert the parameters.\n"
         << "One typical case is when you have a function calling the "
         << "constructor of a wrapper class.\n"
         << "In such cases you can set the \\texttt{\\$noConvert} option to "
         << "True.\n"
         << "\n"
         << "For Excel usage we can also mark a function as "
         << "\\texttt{volatile}.\n"
         << "This means that whenever the sheet is re-calculated this "
         << "particular function is calculated even it has no dependencies "
         << "which have changed.\n"
         << "We can also mark a function as \\texttt{hidden} in Excel.\n"
         << "This means that it will not appear in the Excel function wizard.\n"
         << "These features are controilled by the \\texttt{\\$excelOptions} "
         << "option.\n"
         << "\n"
         << "Finally you must provide an implementation for the function. "
         << "The implementation code should be surrounded by braces just "
         << "like a standard function implementation in C++.\n"
         << "\n"
         << "The code generator will have converted all the inputs from the "
         << "outer data type to the corresponding inner data type before "
         << "invoking your implementation code. "
         << "The names of the inputs will be the same as the names defined "
         << "in the list of parameters. "
         << "All you need to do is write the code fragment that calls the "
         << "corresponding function within the inner library, and return "
         << "the output (unless the function is defined as \\texttt{void}).\n"
         << "\n"
         << "Errors should be handled by throwing an exception. "
         << "Ideally the exception should be a sub-class of "
         << "\\texttt{std::exception} since in that case the text of the "
         << "exception will be propagated outwards and be visible to the "
         << "user of the outer library.\n"
         << "\n"
         << "\n";


}


ClassMethodConstSP classMethodKeywordHandler(
    ConfigLexer& lexer,
    const std::vector<std::string>& description,
    ModuleDefinitionSP& module,
    ServiceDefinitionSP& service,
    const std::string& className,
    const ClassConstSP& baseClass,
    bool mustImplement,
    bool isVirtual,
    bool isStatic,
    bool verbose,
    bool byValue)
{
    // a class method is very close in definition as a Function
    //
    // the differences are that we can have post declaration items such as
    //   const
    //   = 0
    //
    // extern|static|virtual <return_type> Name(
    //    <description> data_type name, ...)
    // { code }
    //
    // possibility of there not being any code in which case we expect ;
    // or no inputs
    //
    // the extern|static|virtual token is taken as read
    // don't allow compound types yet

    bool isDelegate = false;
    if (!isStatic)
    {
        ConfigLexer::Token token = lexer.getToken();
        if (token.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD && token.toString() == "delegate")
        {
            isDelegate = true;
        }
        else
        {
            lexer.returnToken(token);
        }
    }

    MethodContext methodContext(className, baseClass, isVirtual, isStatic, isDelegate);
    FunctionConstSP function = parseFunction(
        lexer, description, module, service, mustImplement,
        &methodContext, verbose);

    if (!function)
        return ClassMethodConstSP();

    return ClassMethod::Make(
        function,
        methodContext.isConst,
        isVirtual,
        isStatic,
        methodContext.isDelegate,
        methodContext.isImplementation(function->name()),
        methodContext.implements,
        byValue);
}


EnumerandConstSP parseEnumerand(
    ConfigLexer& lexer,
    bool verbose)
{
    // looks for an enumerand
    //
    // if it returns an empty object then it found something it didn't like
    // returns any lookahead tokens

    DescriptionParser desc;
    ConfigLexer::Token token = desc.consume(lexer, verbose);
    if (token.type != SPI_CONFIG_TOKEN_TYPE_NAME)
    {
        lexer.returnToken(token);
        // perhaps we should object if there is some dangling description
        return EnumerandConstSP();
    }

    std::string enumerand(token.value.aName);
    std::string value;
    std::vector<std::string> alternates;

    token = lexer.getToken();
    if (token.type == '=')
    {
        value = getConstantValue(lexer);
    }
    else
    {
        lexer.returnToken(token);
    }

    token = lexer.getToken();
    while (token.type == SPI_CONFIG_TOKEN_TYPE_STRING)
    {
        alternates.push_back(token.value.aString);
        token = lexer.getToken();
    }
    lexer.returnToken(token);

    return Enumerand::Make(desc.take(), enumerand, value, alternates);
}

void writeEnumerandDoc(
    GeneratedOutput& ostr,
    bool verbose)
{
    ostr << "\\subsubsection{Enumerand} \\label{enumerand}\n"
         << "\n"
         << "Enumerated types consist of enumerands. "
         << "Each enumerand has a code, plus optional string representation, "
         << "and usually the equivalent value in the inner library.\n";

    writeTexBeginSyntax(ostr, true);
    ostr << "    \\emph{outerValue} [ = \\emph{innerValue} ] "
         << "[ \"\\emph{alternate}\" ... ] \n";
    writeTexEndSyntax(ostr);

    ostr << "\n"
         << "\\texttt{\\$outerValue} is a valid C++ identifier that represents "
         << "the enumerand that will be available in the outer library. "
         << "It will be defined within the namespace of the enumerated type, "
         << "so you only need to make it unique within the list of enumerands "
         << "and not globally unique.\n"
         << "\n"
         << "\\texttt{\\$innerValue} is the equivalent value within the inner "
         << "library. This will be needed if the enumerated type being "
         << "defined involves wrapping an enumerated type from the inner "
         << "library (this is the expected use case).\n"
         << "\n"
         << "\\texttt{\\$alternate} defines one or more alternative strings "
         << "for the enumerand. "
         << "If you do not provide these, then the string corresponding to "
         << "the \\texttt{\\$outerValue} will be the string representation "
         << "for this value. "
         << "Within higher level interfaces such as Excel and Python, you "
         << "will be using the string representation to provide the input "
         << "values for enumerated types. "
         << "Serialization also uses the string representation. "
         << "From the outer C++ layer you can use the symbolic type or the "
         << "string values.\n"
         << "\n"
         << "\n";
}

void enumKeywordHandler(
    ConfigLexer& lexer,
    const std::vector<std::string>& description,
    ModuleDefinitionSP& module,
    ServiceDefinitionSP& service,
    bool verbose)
{
    // an enum has the following tokens
    //
    // enum Name [<inner_type>]
    // {
    //    name,...
    // };
    //
    // the enum token is taken as read

    ConfigLexer::Token token = getTokenOfType(lexer, SPI_CONFIG_TOKEN_TYPE_NAME, "Name");
    std::string name(token.value.aName);
    std::string innerName;
    std::string innerHeader;
    std::string enumTypedef;

    token = lexer.getToken();
    bool ignore = false;
    bool enumDeclared = false;

    if (token.type == '<')
    {
        token = lexer.getToken();

        if (token.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD &&
            strcmp(token.value.aKeyword, "enum") == 0)
        {
            // we allow enum class
            // or enum : <int-type>
            // or just enum
            // if an enum is declared then the inner type cannot be an integer type
            enumDeclared = true;
            token = lexer.getToken();
            if (token.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD &&
                strcmp(token.value.aKeyword, "class") == 0)
            {
                enumTypedef = "enum class";
            }
            else
            {
                lexer.returnToken(token);
            }
        }
        else
        {
            lexer.returnToken(token);
        }

        innerName = getCppTypeName(lexer);
        token = lexer.getToken();
        if (enumDeclared && enumTypedef.empty())
        {
            if (token.type == ':')
            {
                // expect something like int
                token = getTokenOfType(lexer, SPI_CONFIG_TOKEN_TYPE_KEYWORD, "enumSize");
                if (strcmp(token.value.aKeyword, "int") == 0 ||
                    strcmp(token.value.aKeyword, "char") == 0 ||
                    strcmp(token.value.aKeyword, "long") == 0 ||
                    strcmp(token.value.aKeyword, "size_t") == 0)
                {
                    enumTypedef = spi_util::StringFormat("enum : %s", token.value.aKeyword);
                }
                else
                {
                    throw spi::RuntimeError("%s is not a valid enum size", token.value.aKeyword);
                }
                token = lexer.getToken();
            }
        }
        if (token.type != '>')
        {
            throw spi::RuntimeError(
                "Expected '>' to match '<' for enum %s - actual was (%s)",
                name.c_str(), token.toString().c_str());
        }
        ParserOptions defaultOptions;
        defaultOptions["innerHeader"]  = StringConstant::Make("");
        bool integerType = innerName == "int" ||
            innerName == "long" ||
            innerName == "size_t" ||
            innerName == "char";

        if (enumDeclared)
        {
            if (integerType)
            {
                throw spi::RuntimeError("enum declared is not consistent with integer type %s",
                    innerName.c_str());
            }
        }
        else if (!integerType)
        {
            // cannot define enumType in more than one manner
            // this is really for backward compatibility
            // better is to declare the type inside <...>
            defaultOptions["typedef"] = StringConstant::Make("enum : int");
        }
        defaultOptions["ignore"] = BoolConstant::Make(false);

        ParserOptions options;
        options = parseOptions(lexer, "{", defaultOptions, verbose);
        ignore = getOption(options, "ignore")->getBool();

        innerHeader = getOption(options, "innerHeader")->getString();
        if (!enumDeclared && !integerType)
        { 
            enumTypedef = getOption(options, "typedef")->getString();
        }

        token = lexer.getToken();
    }
    else
    {
        static ParserOptions defaultOptions;
        if (defaultOptions.size() == 0)
        {
            defaultOptions["ignore"] = BoolConstant::Make(false);
        }

        ParserOptions options;
        lexer.returnToken(token);
        options = parseOptions(lexer, "{", defaultOptions, verbose);
        ignore = getOption(options, "ignore")->getBool();
        token = lexer.getToken();
    }

    if (token.type != '{')
        throw spi::RuntimeError("Expected '{' for enum - actual was (%s)",
                           token.toString().c_str());

    std::vector<EnumerandConstSP> enumerands;

    // we cannot have an enumerand with no types - hence always expect
    // the first name

    EnumerandConstSP enumerand = parseEnumerand(lexer, verbose);
    if (enumerand)
    {
        enumerands.push_back(enumerand);
        while (true)
        {
            token = lexer.getToken();
            if (token.type == ',')
            {
                enumerand = parseEnumerand(lexer, verbose);
                if (!enumerand)
                    throw spi::RuntimeError("No enumerand after ','");
                enumerands.push_back(enumerand);
            }
            else
            {
                lexer.returnToken(token);
                break;
            }
        }
    }
    token = lexer.getToken();
    if (token.type != '}')
        throw spi::RuntimeError("%s: Expected } - actual was (%s)",
                                __FUNCTION__, token.toString().c_str());

    getTokenOfType(lexer, ';');

    EnumConstSP type = Enum::Make(description, name, module->moduleNamespace(),
        innerName, innerHeader, enumTypedef, enumerands);
    type->dataType(service, ignore);

    if (!ignore)
    {
        module->addEnum(type);
    }
}

void writeEnumDoc(
    GeneratedOutput& ostr,
    bool verbose)
{
    ostr << "\\subsubsection{Enumerated Type} \\label{enum}\n"
         << "\n"
         << "An enumerated type consists of a number of pre-defined constant "
         << "values and is represented by an \\texttt{enum} in the outer C++ "
         << "library. "
         << "The \\texttt{enum} is enclosed within a \\texttt{class} in the "
         << "outer C++ library. "
         << "This approach provides a namespace for the enumerands, plus the "
         << "ability to allow automatic type conversion via the C++ class "
         << "constructor from strings or the enumerands.\n"
         << "\n"
         << "The expected use case is that it will be used to wrap an "
         << "equivalent type from the inner C++ library. "
         << "Note that the inner type can be an enumerated type or any "
         << "integer type. "
         << "For example, in some cases the inner library might be using "
         << "\\texttt{int} or \\texttt{long} for the equivalent data "
         << "representation, with a bunch of \\texttt{\\#define} statements "
         << "to define the possible values.\n"
         << "\n";
    writeTexBeginSyntax(ostr, true);
    ostr << "    enum \\emph{outerTypename} [ <\\emph{innerTypeDefinition}> ]\n"
         << "    \\{ enumerands...\\}; \n";
    writeTexEndSyntax(ostr);

    ostr << "\n"
         << "\\texttt{\\$outerTypename} is a valid C++ identifier which "
         << "defines the typename visible to the outer C++ library. "
         << "It is contained within the service namespace (and optional module "
         << "namespace) so it can be the same as the core name of the inner "
         << "typename if you wish.\n"
         << "\n"
         << "\\texttt{\\$innerTypeDefinition} is optionally enclosed in $<..>$ "
         << "and is a valid C++ typename understood by the inner C++ library. "
         << "It can be an integer type (such as \\texttt{int} or "
         << "\\texttt{long}) or some user defined type visibale to the inner "
         << "C++ library. "
         << "We do not expose this type at all to the outer C++ library user "
         << "via its header files.\n"
         << "\n"
         << "\\texttt{\\$enumerands} is a comma separated list of enumerands "
         << "(see page \\pageref{enumerand}) defining the possible values "
         << "for the enumerated type. "
         << "At least one enumerand must be provided.\n"
         << "\n"
         << "\n";
}

void typedefKeywordHandler(
    ConfigLexer& lexer,
    const std::vector<std::string>& description,
    ModuleDefinitionSP& module,
    ServiceDefinitionSP& service,
    bool verbose)
{
    // parse the following:
    //
    //  typedef outer_type name <inner_type>
    //  {
    //      convert_in(vname)
    //      {
    //          ...
    //      }
    //      convert_out(vname)
    //      {
    //          ...
    //      }
    //  };
    //
    // inner_type is optional (=outer_type in that case)
    // convert_in is mandatory
    // convert_out is optional (if inner_type == outer_type)
    //

    ConfigLexer::Token token = lexer.getToken();

    DataTypeConstSP outerDataType = parseDataType(token, lexer, module, service);

    //std::string outerTypeName = getCppTypeName(lexer);
    token = getTokenOfType(lexer, SPI_CONFIG_TOKEN_TYPE_NAME, "name");
    std::string dataTypeName(token.value.aName);

    token = lexer.getToken();

    std::string innerTypeName;
    std::string innerHeader;
    std::string innerTypedef;
    bool noDoc = false;
    bool ignore = false;
    if (token.type == '<')
    {
        innerTypeName = parseInnerType(lexer);
        //getTokenOfType(lexer, '>');

        static ParserOptions defaultOptions;
        if (defaultOptions.size() == 0)
        {
            defaultOptions["innerHeader"] = StringConstant::Make("");
            defaultOptions["typedef"] = StringConstant::Make();
            defaultOptions["noDoc"] = BoolConstant::Make(false);
            defaultOptions["ignore"] = BoolConstant::Make(false);
        }

        ParserOptions options;
        options = parseOptions(lexer, "{", defaultOptions, verbose);

        innerHeader = getOption(options, "innerHeader")->getString();
        innerTypedef = getOption(options, "typedef")->getString();
        noDoc = getOption(options, "noDoc")->getBool();
        ignore = getOption(options, "ignore")->getBool();
    }
    else
    {
        lexer.returnToken(token);
        innerTypeName = outerDataType->name();

        static ParserOptions defaultOptions;
        if (defaultOptions.size() == 0)
        {
            defaultOptions["noDoc"] = BoolConstant::Make(false);
            defaultOptions["ignore"] = BoolConstant::Make(false);
        }

        ParserOptions options;
        options = parseOptions(lexer, "{", defaultOptions, verbose);

        noDoc = getOption(options, "noDoc")->getBool();
        ignore = getOption(options, "ignore")->getBool();
    }

    getTokenOfType(lexer, '{');

    // now we expect code of the form convert_in(varname) {...}
    // followed by the optional convert_out(varname) {...}
    //
    // actually we do not care about the order...

    std::string convertInVarName;
    VerbatimConstSP convertIn;
    std::string convertOutVarName;
    VerbatimConstSP convertOut;

    token = lexer.getToken();
    while (token.type != '}')
    {
        lexer.returnToken(token);
        token = getTokenOfType(lexer, SPI_CONFIG_TOKEN_TYPE_NAME);
        std::string funcName(token.value.aName);
        if (funcName == "convert_in")
        {
            if (convertIn)
                throw spi::RuntimeError("convert_in already defined");
            getTokenOfType(lexer, '(');
            token = getTokenOfType(lexer, SPI_CONFIG_TOKEN_TYPE_NAME);
            convertInVarName = token.value.aName;
            getTokenOfType(lexer, ')');
            getTokenOfType(lexer, '{');
            convertIn = lexer.getVerbatim();
        }
        else if (funcName == "convert_out")
        {
            if (convertOut)
                throw spi::RuntimeError("convert_out already defined");
            getTokenOfType(lexer, '(');
            token = getTokenOfType(lexer, SPI_CONFIG_TOKEN_TYPE_NAME);
            convertOutVarName = token.value.aName;
            getTokenOfType(lexer, ')');
            getTokenOfType(lexer, '{');
            convertOut = lexer.getVerbatim();
        }
        else
        {
            throw spi::RuntimeError("Unexpected function name %s - should be "
                "convert_in or convert_out", funcName.c_str());
        }
        token = lexer.getToken();
    }
    token = getTokenOfType(lexer, ';');

    SimpleTypeConstSP type = SimpleType::Make(
        description, dataTypeName, module->moduleNamespace(),
        outerDataType, innerTypeName, innerHeader, innerTypedef, noDoc,
        convertInVarName, convertIn, convertOutVarName, convertOut);

    type->getDataType(service, ignore);

    if (!ignore)
    {
        module->addConstruct(type);
    }
}

void writeSimpleTypeDoc(GeneratedOutput& ostr, bool verbose)
{
    ostr << "\\subsubsection{Simple Type} \\label{simpleType}\n"
         << "\n"
         << "A simple type consists of the name of an inner type from the "
         << "inner C++ library plus one of the built-in types from SPI. "
         << "In practice, the user of the outer C++ library uses the built-in "
         << "type and this gets converted to an instance of the inner type "
         << "before the implementation of a particular function or class "
         << "method.\n"
         << "\n"
         << "Since the conversion happens everytime a variable of a simple "
         << "type is used you should only use the simple type mechanism "
         << "for basic scalar types."
         << "The classic use case is for converting from the Date format "
         << "provided as standard by SPI, and the Date format used within "
         << "the inner C++ library which is unlikely to be exactly the same. "
         << "Dates are very often represented as an offset from date zero. "
         << "This indeed is how they are represented within SPI. "
         << "Hence converting between Date formats would often be a matter of "
         << "re-basing the integer offset from the zero date.\n";

    writeTexBeginSyntax(ostr, true);
    ostr << "    typedef \\emph{outerTypename} \\emph{name} "
         << "[ <\\emph{innerTypename}> ]\n"
         << "    \\{ conversionRoutines \\}; \n";
    writeTexEndSyntax(ostr);

    ostr << "\n"
         << "\\texttt{\\$outerTypename} is the name of one of the built-in "
         << "types provided by SPI (see page \\pageref{dataType} for a list "
         << "of the built-in data types).\n"
         << "\n"
         << "\\texttt{\\$name} is the data type name defined here.\n"
         << "It should be unique at the service level - hence cannot be the "
         << "same as the name of the built-in types.\n"
         << "\n"
         << "\\texttt{\\$innerTypename} is the name of a C++ type defined by "
         << "the inner C++ library.\n"
         << "Note that it can be omitted - in which case the inner type name "
         << "is the same as output type name.\n"
         << "That might seem fairly pointless, but it would enable you to "
         << "possibly provide some consistent validation rules throughout "
         << "your project. "
         << "For example, if you were describing a correlation parameter then "
         << "there is a range of possible values for correlation from -1 to 1. "
         << "In this case your \\emph{convertIn} routine (see below) could "
         << "simply provide validation of the value, and then return the "
         << "input value as the converted value.\n"
         << "\n"
         << "Within braces you would then need to provide two conversion "
         << "routines. "
         << "If the \\texttt{\\$innerTypename} is missing, then the output "
         << "conversion are optional, and you would probably only provide the "
         << "input conversion routine (which would perform some validation as "
         << "suggested above). "
         << "The general use case is that you would need to provide both "
         << "conversion routines.\n"
         << "\n"
         << "\\subsubsection*{Simple Type Input Conversion Routine}\n";

    writeTexBeginSyntax(ostr, false);
    ostr << "    convert\\_in(\\emph{outerVarName}) \\{ implementation \\}\n";
    writeTexEndSyntax(ostr);

    ostr << "\n"
         << "\\texttt{\\$outerVarname} is the input to the input conversion "
         << "function and is a variable of C++ type given by "
         << "\\texttt{\\$outerTypename}.\n"
         << "\n"
         << "The function implementation should return a variable of C++ type "
         << "given by \\texttt{\\$innerTypename}. "
         << "If an error is detected, then the routine should throw an "
         << "exception derived from \\texttt{std::exception} in order to "
         << "enable the exception to be correctly propagated to the end user.\n"
         << "\n"
         << "The \\texttt{\\$convert\\_in} routine will be called before any "
         << "variable for this data type is passed into code to be used within "
         << "the context of the inner library.\n"
         << "\n"
         << "\\subsubsection*{Simple Type Output Conversion Routine}\n";

    writeTexBeginSyntax(ostr, false);
    ostr << "{convert\\_out(\\emph{innerVarName})\\{ implementation \\}}\n";
    writeTexEndSyntax(ostr);

    ostr << "\n"
         << "\\texttt{\\$innerVarname} is the input to the output conversion "
         << "function and is a variable of C++ type "
         << "\\texttt{\\$innerTypename}.\n"
         << "\n"
         << "The function implementation should return a variable of C++ type "
         << "corresponding to \\texttt{\\$outerTypename}. "
         << "If an error is detected, then the routine should throw an "
         << "exception derived from \\texttt{std::exception} in order to "
         << "enable the exception to be correctly propagated to the end user.\n"
         << "\n"
         << "\n";
}

void typenameKeywordHandler(
    ConfigLexer& lexer,
    const std::vector<std::string>& description,
    ModuleDefinitionSP& module,
    ServiceDefinitionSP& service,
    bool verbose)
{
    // parse the following:
    //
    //  typename <definition> name;
    //
    // <definition> is a sequence of symbols
    // name is the typename
    //
    // example:
    //  typename struct _TCurve TCurve;
    //
    // definition = struct _TCurve
    // name = TCurve
    //
    // we do not support complex definitions which require semi-colons
    // the first semi-colon terminates the expression
    //
    // the code generated will replace typename with typedef and put spaces
    // between all tokens
    //
    // the typename will be available to be used as the inner type
    //
    // thus we are creating an inner type by the typename keyword
    //
    ConfigLexer::Token token;

    std::vector<ConfigLexer::Token> tokens;

    token = lexer.getToken();
    while (token.type != ';' && token.type != '(')
    {
        tokens.push_back(token);
        token = lexer.getToken();
    }
    bool hasOptions = token.type == '(';

    if (tokens.size() < 2)
        throw spi::RuntimeError("typename requires definition and name "
                                "followed by ;");

    ConfigLexer::Token nameToken = tokens.back();
    tokens.pop_back();

    // TBD: allow the inner typename to be within a namespace
    // Need to detect "::" at the end of tokens and then eat prior tokens

    if (nameToken.type != SPI_CONFIG_TOKEN_TYPE_NAME)
        throw spi::RuntimeError("Name token '%s' is not of type name",
            nameToken.toString().c_str());

    std::string name(nameToken.value.aName);

    std::ostringstream oss;
    oss << "typedef ";
    for (size_t i = 0; i < tokens.size(); ++i)
    {
        oss << tokens[i].toString() << " ";
    }
    oss << name << ";";
    std::string preDeclaration = oss.str();

    static ParserOptions defaultOptions;
    if (defaultOptions.size() == 0)
    {
        defaultOptions["freeFunc"] = StringConstant::Make("");
        defaultOptions["copyFunc"] = StringConstant::Make("");
        defaultOptions["isOpen"]   = BoolConstant::Make(false);
        defaultOptions["isShared"] = BoolConstant::Make(false);
        defaultOptions["isConst"]  = BoolConstant::Make(false);
        defaultOptions["isCached"] = BoolConstant::Make(false);
        defaultOptions["sharedPtr"] = StringConstant::Make("");
        defaultOptions["byValue"] = BoolConstant::Make(false);
        defaultOptions["boolTest"] = StringConstant::Make("");
    }

    std::string freeFunc;
    std::string copyFunc;
    bool isOpen(false);
    bool isShared(false);
    bool isConst(false);
    bool isCached(false);
    bool byValue(false);
    std::string sharedPtr;
    std::string boolTest;

    if (hasOptions)
    {
        ParserOptions options;
        options  = parseOptions(lexer, ")", defaultOptions, verbose);
        getTokenOfType(lexer, ')');
        freeFunc = getOption(options, "freeFunc")->getString();
        copyFunc = getOption(options, "copyFunc")->getString();
        isOpen   = getOption(options, "isOpen")->getBool();
        isShared = getOption(options, "isShared")->getBool();
        isConst  = getOption(options, "isConst")->getBool();
        isCached = getOption(options, "isCached")->getBool();
        sharedPtr = getOption(options, "sharedPtr")->getString();
        byValue  = getOption(options, "byValue")->getBool();
        boolTest = getOption(options, "boolTest")->getString();
        token    = getTokenOfType(lexer, ';');
    }
    SPI_POST_CONDITION(token.type == ';');

    if (sharedPtr.empty())
        sharedPtr = service->getSharedPtr();

    bool isStruct(false);
    bool allowConst(false);
    InnerClassConstSP innerType = InnerClass::Make(
        name, "", freeFunc, copyFunc, preDeclaration, sharedPtr,
        isShared, isConst, isOpen, isStruct, isCached, false, byValue, boolTest, allowConst);

    service->addInnerClass(innerType);
    module->addInnerClass(innerType);
}



void writeTypenameDoc(GeneratedOutput& ostr, bool verbose)
{
    ostr << "\\subsubsection{Typename} \\label{typename}\n"
         << "\n"
         << "When we define wrapper classes we usually wrap a class and we can "
         << "rely on the usual class operations for construction and "
         << "deletion. "
         << "In such cases we do not need the \\texttt{typename} command. "
         << "However in other cases we are wrapping a struct or some sort "
         << "of instantiated class template, then we may need to define "
         << "the type before we try to wrap it.\n"
         << "\n"
         << "In these circumstances we use the \\texttt{typename} command.\n"
         << "\n"
         << "Note that we are defining an existing inner type by use of the "
         << "\\texttt{typename} keyword. "
         << "However if we do not subsequently wrap the defined type then the "
         << "definition of the inner type is without purpose.\n";

// the parser accepts the description but doesn't use it
    writeTexBeginSyntax(ostr, false);
    ostr << "    typename \\emph{typeDefinition} \\emph{name} [ (...) ];\n";
    writeTexEndSyntax(ostr);

    ostr << "\n"
         << "with the following optional arguments (which should be enclosed "
         << "in parantheses before the final semi-colon):\n";

    writeTexBeginOptionsTable(ostr);
    writeTexOptionsTableRow(
        ostr, "freeFunc", "", "Valid C++ identifier");
    writeTexOptionsTableRow(
        ostr, "copyFunc", "", "Valid C++ identifier");
    writeTexOptionsTableRow(
        ostr, "isOpen", "false", "true or false");
    writeTexOptionsTableRow(
        ostr, "isShared", "false", "true or false");
    writeTexOptionsTableRow(
        ostr, "isConst", "false", "true or false");
    writeTexEndOptionsTable(ostr);

    ostr << "\\texttt{\\$typeDefinition} is a sequence of tokens which define "
         << "the type in terms of the inner library.\n"
         << "\n"
         << "\\texttt{\\$name} is the name of the inner type that you can "
         << "subsequently use when wrapping this type in a class.\n"
         << "\n"
         << "Note that the code generated in the public header files is to "
         << "replace typename with typedef, and ignore the optional "
         << "arguments.\n"
         << "\n"
         << "An example perhaps will make things a bit clearer:\n"
         << "\\begin{verbatim}\n"
         << "typename struct _TCurve TCurve(isOpen=true "
         << "freeFunc=GtoFreeCurve\n"
         << "                               copyFunc=GtoCopyCurve);\n"
         << "\\end{verbatim}\n"
         << "\n"
         << "In the public header file the following would be defined:"
         << "\\begin{verbatim}\n"
         << "typedef struct _TCurve TCurve;\n"
         << "\\end{verbatim}\n"
         << "\n"
         << "and in the generated code we would use the freeFunc to know how "
         << "to free the memory associated with data of type \\texttt{TCurve*}.\n"
         << "\n"
         << "The copyFunc is used on occasion to copy the memory associated with "
         << "data of type \\texttt{TCurve*}.\n"
         << "\n"
         << "We would be able to declare values of type \\texttt{TCurve*} (or "
         << "perhaps \\texttt{boost::shared\\_ptr$<$TCurve$>$}) without "
         << "needing to expose the internals of the \\texttt{struct \\_TCurve} "
         << "to the users of the outer library.\n"
         << "\n"
         << "To re-iterate - if you are wrapping a class then you will not "
         << "need to use the \\texttt{typename} command. "
         << "Hence it is quite plausible that you would never use this "
         << "feature of SPI. "
         << "The main use case is when you are wrapping a C-library rather "
         << "than a C++-library.\n"
         << "\n"
         << "\n";
}


void templateKeywordHandler(
    ConfigLexer& lexer,
    const std::vector<std::string>& description,
    ModuleDefinitionSP& module,
    ServiceDefinitionSP& service,
    bool verbose)
{
    // parse the following:
    // template <class|typename T> name;
    //
    // name can include namespace
    // T can be any name symbol
    //
    // create an InnerClassTemplate object that is added to the service and is
    // available for use as a template within an inner class definition
    //
    // template is given as read
    ConfigLexer::Token token;

    getTokenOfType(lexer, '<');
    token = getTokenOfType(lexer, SPI_CONFIG_TOKEN_TYPE_KEYWORD, "class or typename");
    std::string classOrTypename = token.value.aKeyword;
    if (classOrTypename != "class" && classOrTypename != "typename")
    {
        throw spi::RuntimeError("Expecting class or typename for template definition");
    }
    token = getTokenOfType(lexer, SPI_CONFIG_TOKEN_TYPE_NAME);
    std::string templateArgument = token.value.aName;
    getTokenOfType(lexer, '>');

    token = getTokenOfType(lexer, SPI_CONFIG_TOKEN_TYPE_KEYWORD, "class or struct");
    std::string classOrStruct = token.value.aKeyword;
    if (classOrStruct != "class" && classOrStruct != "struct")
    {
        throw spi::RuntimeError("Expecting class or struct for template definition");
    }

    std::string fullName = getCppTypeName(lexer);

    static ParserOptions defaultOptions;
    if (defaultOptions.size() == 0)
    {
        defaultOptions["boolTest"] = StringConstant::Make("");
    }

    ParserOptions options;
    options = parseOptions(lexer, ";", defaultOptions, verbose);
    std::string boolTest = getOption(options, "boolTest")->getString();

    getTokenOfType(lexer, ';');

    std::string name;
    std::string ns;
    std::string preDeclaration;
    splitCppTypeName(fullName, name, ns);

    if (ns != "std")
    {
        std::ostringstream oss;
        oss << "template <" << classOrTypename << " " << templateArgument << "> "
            << classOrStruct << " " << name << ";";
        preDeclaration = oss.str();
    }

    InnerClassTemplateConstSP innerClassTemplate = InnerClassTemplate::Make(
        name, ns, preDeclaration, boolTest);

    service->addInnerClassTemplate(innerClassTemplate);
    module->addInnerClassTemplate(innerClassTemplate);
}

void writeTemplateDoc(GeneratedOutput& ostr, bool verbose)
{
    ostr << "\\subsubsection{Template} \\label{template}\n"
         << "\n"
         << "When we define wrapper classes we usually wrap a class and we can "
         << "use a simple pre-declaration statement to declare the class in "
         << "the generated public header file.\n"
         << "However if we wish to wrap a class defined using a class template "
         << "then the pre-declaration requires two parts.\n"
         << "First we need to pre-declare the class template, and then we need "
         << "to pre-declare the actual class based on that template.\n"
         << "\n"
         << "In order to resolve this conundrum you need to define the class "
         << "template using the \\texttt{template} keyword.\n"
         << "\n";

// the parser accepts the description but doesn't use it
    writeTexBeginSyntax(ostr, false);
    ostr << "    template<class|typename T> class|struct \\emph{templateName};\n";
    writeTexEndSyntax(ostr);

    ostr << "Note that we are defining an existing inner class template "
         << "by use of the \\texttt{typename} keyword.\n"
         << "However if we do not subsequently wrap something that uses "
         << "this class template, then the definition of the inner class "
         << "template is without purpose.\n"
         << "\n"
         << "In the generated code the class template will be pre-declared "
         << "exactly as defined - unless you have pre-declared a class "
         << "template from the \\texttt{std} namespace.\n"
         << "The reason that we don't pre-declare types such as \\texttt{"
         << "std::vector} is that these templates tend to have more complex "
         << "definitions with optional second (or more) arguments, and we "
         << "don't support that syntax here.\n"
         << "\n"
         << "Subsequently when defining a wrapper class, you will be able to "
         << "use the \\texttt{\\$templateName} as the inner class with the "
         << "relevant typename as template argument.\n"
         << "\n"
         << "An example perhaps will make things a bit clearer:\n"
         << "\\begin{verbatim}\n"
         << "template<typename T> class CubicSpline;\n"
         << "\\end{verbatim}\n"
         << "\n"
         << "In the public header file we wouldn't simply replicate this "
         << "definition, and now the \\texttt{\\$templateName} equal to "
         << "CubicSpline is available as an inner type.\n"
         << "Suppose we were only interested in a CubicSpline of doubles "
         << "(in practice the typename is the x-argument for the spline "
         << "whereas the y-argument of the spline will always be double).\n"
         << "\n"
         << "In that case we could use \\verb|CubicSpline<double>| as the inner type "
         << "within a wrapper class definition (see page \\pageref{class}).\n"
         << "\n";
}

void initClassStructOptions(
    ParserOptions& defaultOptions,
    bool wrapperClass)
{
    defaultOptions["noMake"] = BoolConstant::Make(false);
    defaultOptions["objectName"] = StringConstant::Make("");
    defaultOptions["canPut"] = BoolConstant::Make(false);
    defaultOptions["noId"] = BoolConstant::Make(false);
    defaultOptions["asValue"] = BoolConstant::Make(false);
    defaultOptions["ignore"] = BoolConstant::Make(false);
    defaultOptions["uuid"] = BoolConstant::Make(false);
    defaultOptions["xlFuncName"] = StringConstant::Make(""); // for backward compatibility - should be funcPrefix instead
    defaultOptions["constructor"] = StringConstant::Make("");
    defaultOptions["funcPrefix"] = StringConstant::Make(""); // same as xlFuncName
    defaultOptions["instance"] = StringConstant::Make(""); // name of the instance field in functions

    if (wrapperClass)
        defaultOptions["sharedPtr"] = StringConstant::Make("");
    else
        defaultOptions["byValue"] = BoolConstant::Make(false);
}

// given a class (or struct) with defined constructor this creates the corresponding function
void addConstructorFunction(
    const ClassConstSP& cls,
    const std::string& constructor,
    ModuleDefinitionSP& module,
    ServiceDefinitionSP& svc,
    bool verbose)
{
    if (constructor.empty())
        return;

    std::vector<AttributeConstSP> attributes = cls->AllAttributes();
    std::vector<FunctionAttributeConstSP> funcAttributes;
    funcAttributes.reserve(attributes.size());
    size_t N = attributes.size();
    for (size_t i = 0; i < N; ++i)
        funcAttributes.push_back(FunctionAttribute::Make(attributes[i], false));

    std::vector<std::string> description;
    std::vector<std::string> returnTypeDescription;

    description.push_back("Constructor for " + cls->getName(true, "."));

    bool ignored = false; // if the class is ignored we shouldn't get here
    DataTypeConstSP returnType = cls->getDataType(svc, ignored);

    bool noLog = false;
    bool noConvert = true;
    std::vector<std::string> excelOptions; // we don't know whether this is slow or not
    int cacheSize = 0;

    std::ostringstream code;
    code << "    return " << cls->getName(true, "::") << "::Make";
    char* sep = "(";
    for (size_t i = 0; i < N; ++i)
    {
        code << sep << "\n        " << attributes[i]->name();
        sep = ",";
    }
    code << ");\n"
        << "}";

    VerbatimConstSP implementation = Verbatim::Make(
        std::string(), // no filename
        0,
        spi_util::StringSplit(code.str(), "\n"));

    std::string ns;
    FunctionConstSP func = Function::Make(
        description,
        returnTypeDescription,
        returnType,
        0,
        constructor,
        ns,
        funcAttributes,
        implementation,
        noLog,
        noConvert,
        excelOptions,
        cacheSize);

    module->addConstruct(func);
}

void structKeywordHandler(
    ConfigLexer& lexer,
    const std::vector<std::string>& description,
    ModuleDefinitionSP& module,
    ServiceDefinitionSP& service,
    bool verbose)
{
    // a struct has the following tokens
    //
    // struct Name
    // {
    //    type name;
    //    ...
    // };
    //
    // struct is taken as read
    //
    // struct does not wrap
    //
    // struct might be short lived - use it as a way of parsing this type
    // of structure and preparing for its older brother - class

    ConfigLexer::Token token = lexer.getToken();
    bool isVirtual(false);

    if (token.toString() == "virtual")
    {
        isVirtual = true;
    }
    else
    {
        lexer.returnToken(token);
    }

    token = getTokenOfType(lexer, SPI_CONFIG_TOKEN_TYPE_NAME, "Name");
    std::string name(token.value.aName);

    ClassConstSP baseClass;

    token = lexer.getToken();
    if (token.type == ':')
    {
        token = getTokenOfType(lexer, SPI_CONFIG_TOKEN_TYPE_NAME, "BaseClass");
        std::string baseClassName(token.value.aName);
        baseClass = service->getClass(baseClassName);
        if (!baseClass)
        {
            throw spi::RuntimeError("Undefined base class: %s",
                                    baseClassName.c_str());
        }
        //baseStruct = spi_boost::dynamic_pointer_cast<const Struct>(baseClass);
        //if (!baseStruct)
        //{
        //    throw spi::RuntimeError("Base class %s is not of type struct",
        //        baseClassName.c_str());
        //}
    }
    else
    {
        lexer.returnToken(token);
    }

    static ParserOptions defaultOptions;
    if (defaultOptions.size() == 0)
    {
        initClassStructOptions(defaultOptions, false);
    }
    ParserOptions options;
    options = parseOptions(lexer, "{", defaultOptions, verbose);
    getTokenOfType(lexer, '{');
    bool noMake = getOption(options, "noMake")->getBool();
    bool canPut = getOption(options, "canPut")->getBool();
    bool noId = getOption(options, "noId")->getBool();
    bool asValue = getOption(options, "asValue")->getBool();
    bool byValue = getOption(options, "byValue")->getBool();
    bool ignore = getOption(options, "ignore")->getBool();
    bool uuid = getOption(options, "uuid")->getBool();
    std::string xlFuncName = getOption(options, "xlFuncName")->getString();
    std::string funcPrefix = getOption(options, "funcPrefix")->getString();
    std::string constructor = getOption(options, "constructor")->getString();
    std::string instance = getOption(options, "instance")->getString();

    StructSP type = Struct::Make(
        description, name, module->moduleNamespace(), baseClass, noMake,
        getOption(options, "objectName")->getString(),
        canPut, noId, isVirtual, asValue, uuid, byValue, false,
        funcPrefix.empty() ? xlFuncName : funcPrefix, constructor, instance);

    // we need to register the type before parsing the contents in order to
    // allow references to itself in the structure definition

    type->getDataType(service, ignore);

    // there is of course a strong case for supporting pre-declaration

    //token = getTokenOfType(lexer, '{');

    token = lexer.getToken();
    while (token.type != '}')
    {
        lexer.returnToken(token);

        DescriptionParser desc;
        token = desc.consume(lexer, verbose);
        if (token.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD)
        {
            std::string keyword(token.value.aKeyword);
            if (keyword == "virtual" ||
                keyword == "extern" ||
                keyword == "static")
            {
                bool isVirtual = keyword == "virtual";
                bool isStatic  = keyword == "static";
                bool isDelegate = keyword == "delegate";
                ClassMethodConstSP classMethod = classMethodKeywordHandler(
                    lexer, desc.take(), module, service, name, baseClass, false,
                    isVirtual, isStatic, verbose, byValue);
                if (classMethod)
                    type->addMethod(classMethod);
                token = lexer.getToken();
                continue;
            }
            else if (keyword == "private:" ||
                     keyword == "protected:" ||
                     keyword == "public:")
            {
                const std::string& verbatimStart = keyword;
                VerbatimConstSP    verbatim      = lexer.getVerbatim('{', '}', ";");
                type->addVerbatim(verbatimStart, verbatim);
                break;
            }
            else if (keyword == "operator")
            {
                CoerceToConstSP coerceTo = parseCoerceTo(lexer, name, desc.take(),
                    module, service, verbose);
                type->addCoerceTo(coerceTo);
                token = lexer.getToken();
                continue;
            }
        }
        else if (token.type == SPI_CONFIG_TOKEN_TYPE_NAME &&
                 name == token.value.aName)
        {
            CoerceFromConstSP coerceFrom = parseCoerceFrom(
                lexer, name, desc.take(), module, service, verbose);
            type->addCoerceFrom(coerceFrom);
            token = lexer.getToken();
            continue;
        }
        else if (token.type == SPI_CONFIG_TOKEN_TYPE_NAME &&
                 strcmp(token.value.aName, "dynamic_properties") == 0)
        {
            VerbatimConstSP code = parseImplementation(lexer, true);
            type->addDynamicProperties(code);
            token = lexer.getToken();
            continue;
        }
        else if (token.type == SPI_CONFIG_TOKEN_TYPE_NAME &&
                 strcmp(token.value.aName, "as_value") == 0)
        {
            VerbatimConstSP code = parseAsValueCode(lexer);
            type->addAsValueCode(code);
            token = lexer.getToken();
            continue;
        }

        // by default attributes are public for struct
        ClassAttributeAccess::Enum accessLevel = ClassAttributeAccess::PUBLIC;
        if (token.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD)
        {
            std::string keyword(token.value.aKeyword);
            if (keyword == "private" ||
                keyword == "public" ||
                keyword == "property")
            {
                if (keyword == "private")
                {
                    accessLevel = ClassAttributeAccess::PRIVATE;
                }
                else if (keyword == "public")
                {
                    accessLevel = ClassAttributeAccess::PUBLIC;
                }
                else
                {
                    accessLevel = ClassAttributeAccess::PROPERTY;
                }
            }
            else
            {
                lexer.returnToken(token);
            }
        }
        else
        {
            lexer.returnToken(token);
        }

        ClassAttributeConstSP attr = parseClassAttribute(lexer, accessLevel, desc.take(), module, service);
        type->addClassAttribute(attr);
        token = lexer.getToken();
    }

    token = lexer.getToken();
    if (token.type == '{')
    {
        VerbatimConstSP verbatim = lexer.getVerbatim();
        type->addValidation(verbatim);
    }
    else if (token.type != ';')
    {
        throw spi::RuntimeError("Expected ';' to end struct definition");
    }

    if (!ignore)
    {
        service->addClass(type);
        module->addConstruct(type);
        if (!constructor.empty())
        {
            addConstructorFunction(type, constructor, module, service, verbose);
        }
    }
}


void writeStructDoc(GeneratedOutput& ostr, bool verbose)
{
    ostr << "\\newpage\n"
         << "\\subsubsection{Standalone class} \\label{struct}\n"
         << "\n"
         << "Standalone classes can be defined for the outer library.\n"
         << "These are classes that do not wrap an equivalent class from the "
         << "inner library, and hence all functionality must be implemented "
         << "in the outer library.\n"
         << "\n"
         << "Classes have data and functions - so when we define a standalone "
         << "class we must define the attributes of the class, and implement "
         << "any methods of the class in the SPI configuration file.\n"
         << "\n"
         << "In order to distinguish a standalone class from a wrapper class "
         << "(see page \\pageref{class}), we use the keyword \\texttt{struct} "
         << "to indicate a standalone class, as opposed to the keyword "
         << "\\texttt{class} that we will be using for wrapper classes.\n";

    writeTexBeginSyntax(ostr, true);
    ostr << "    struct [virtual] \\emph{className} [ : \\emph{baseClassName} ] ...\n"
         << "    \\{\n"
         << "        // attributes\n"
         << "        \\emph{/** description */}\n"
         << "        \\emph{dataType} [\\emph{arrayDim}] \\emph{attrName};\n"
         << "        ...\n"
         << "        // member functions\n"
         << "        \\emph{/** description */}\n"
         << "        extern|static|virtual \\emph{returnType} [\\emph{arrayDim}] "
         << "\\emph{methodName}\n"
         << "        \\{ \\emph{implementation} \\}\n"
         << "        ...\n"
         << "        // coerce from other types\n"
         << "        \\emph{/** description */}\n"
         << "        $className (\\emph{dataType} \\emph{varName})\n"
         << "        \\{ \\emph{implementation} \\}\n"
         << "        ...\n"
         << "        // verbatim code\n"
         << "    private|public|protected:\n"
         << "        \\emph{verbatim}\n"
         << "    \\}\n"
         << "    \\{ \\emph{constructorValidationCode} \\} | ;\n";
    writeTexEndSyntax(ostr);

    ostr << "\n"
         << "with the following optional parameters (defined before the "
         << "opening \\{ which defines the internal details of the class):";

    writeTexBeginOptionsTable(ostr);
    writeTexOptionsTableRow(
        ostr, "noMake", "False", "True or False");
    writeTexOptionsTableRow(
        ostr, "objectName", "", "Alternative string used in object serialization");
    writeTexEndOptionsTable(ostr);

    ostr << "\n"
         << "The \\texttt{struct} definition has many components which we"
         << "will describe in detail.\n"
         << "\\begin{itemize}\n"
         << "\\item \\texttt{\\$className}\n"
         << "\\item \\texttt{\\$baseClassName} (optional)\n"
         << "\\item \\texttt{\\$attributes} (any number)\n"
         << "\\item \\texttt{\\$methods} (any number)\n"
         << "\\item \\texttt{\\$coerceFrom} (any number)\n"
         << "\\item \\texttt{\\$verbatim} (optional)\n"
         << "\\item \\texttt{\\$constructorValidation} (optional)\n"
         << "\\end{itemize}\n"
         << "\n"
         << "The \\texttt{struct} keyword defines a C++ class in the outer "
         << "library with the name \\texttt{\\$className}.\n"
         << "This name must be unique and also defines a new data type.\n"
         << "The class can be a sub-class of another previously defined "
         << "class via the definition of \\texttt{\\$baseClassName}.\n"
         << "We define inheritance using the C++ syntax by the use of `:'.\n"
         << "All inheritance in SPI is \\texttt{public} so we don't need to "
         << "specify it in the configuration file.\n"
         << "\n"
         << "Before the \\texttt{\\$className} there is one optional keyword:\n"
         << "\\begin{description}\n"
         << "\\item[virtual:] This means that we are defining a base class "
         << "and that in practice we expect to define sub-classes of this "
         << "class which will provide the actual implementation.\n"
         << "\\end{description}\n"
         << "\n"
         << "In this context we will use \\texttt{struct} to refer to the "
         << "definition within SPI and \\texttt{class} to refer to the "
         << "generated code.\n"
         << "\n"
         << "The optional \\texttt{\\$noMake} parameter indicates that the "
         << "class cannot be constructed in directly in Excel, Python and "
         << ".NET implementations.\n"
         << "\n"
         << "Note that in all cases we generate in the C++ a public static "
         << "Make function that creates a shared pointer to the class.\n"
         << "The actual constructor of the class is always protected rather "
         << "than public to prevent direct use of the constructor for "
         << "creating raw pointers.\n"
         << "\n"
         << "However even if we set \\texttt{\\$noMake} to \\texttt{True}, "
         << "we still provide the public static function Make in the C++ "
         << "interface.\n"
         << "The reason is that sometimes we define a struct to use "
         << "\\texttt{noMake=True}, but we want another function to be "
         << "able to create the class, in which case that function might "
         << "well be defined with \\texttt{noConvert=True} and call the Make "
         << "function directly.\n"
         << "\n"
         << "The struct can have any number of \\texttt{\\$attributes}.\n"
         << "An attribute consists of a data type followed by an optional "
         << "array indicator followed by the name of the attribute.\n"
         << "The attribute definition is terminated by `;'.\n"
         << "\n"
         << "The list of attributes defines the constructor of the class "
         << "as well as the serialization of the class.\n"
         << "\n"
         << "The generated constructor takes as input the parameters "
         << "corresponding to the attributes of the class (in the order "
         << "that the attributes are defined).\n"
         << "As mentioned above the constructor is actually in the "
         << "protected section of the class, and in the public section of "
         << "the class there is a static method called Make which returns "
         << "a shared pointer to the class and simply calls the constructor.\n"
         << "See below for discussion of validation of the inputs.\n"
         << "\n"
         << "The struct can have any number of \\texttt{\\$methods}.\n"
         << "To indicate a method you start with one of the keywords "
         << "\\texttt{extern} or \\texttt{static} or \\texttt{virtual} "
         << "which have the following meanings:\n"
         << "\\begin{description}\n"
         << "\\item[extern:] Defines a normal member function.\n"
         << "We need the \\texttt{extern} keyword to distinguish an attribute "
         << "from a member function in the way discussed when we introduced "
         << "the common SPI syntax rules.\n"
         << "\\item[static:] Defines a static function of the class.\n"
         << "Similar to a stand-alone function except that we have access\n"
         << "to private functions and private global data of the class.\n"
         << "\\item[virtual:] Defines a virtual member function.\n"
         << "This is a function that we expect to be implemented by \n"
         << "sub-classes of this class.\n"
         << "\\end{description}\n"
         << "The remainder of the definition of a method is the same as the "
         << "definition of a standalone function (see page \\pageref{extern}) "
         << "with returnType, name, function parameters, options and "
         << "implementation.\n"
         << "For \\texttt{struct} the implementation is required unless the "
         << "method has been marked as \\texttt{virtual} in which we expect "
         << "the method to be implemented in a derived class.\n"
         << "\n"
         << "The major difference between a method and a function is that "
         << "(unless the method has been marked as \\texttt{static}) is that "
         << "we have instance variables available.\n"
         << "\n"
         << "These will have the same names as the attributes defined in the "
         << "\\texttt{struct} definition.\n"
         << "Note that for a \\texttt{struct} there is no concept of wrapping "
         << "and hence there is no inner type.\n"
         << "\n"
         << "The \\texttt{struct} type also supports the concept of coercion "
         << "from other types.\n"
         << "If the \\texttt{struct} type is simple or standard, then you "
         << "might be able to coerce it from (for example) a number or a "
         << "string.\n"
         << "This is indicated in SPI by defining what looks like a "
         << "constructor to the struct which takes a single parameter.\n"
         << "Then you provide the code which converts that parameter into "
         << "an instance of the class.\n"
         << "In your implementation code you are expected to return a "
         << "shared pointer to the class.\n"
         << "Typically you will actually call the Make method based on the "
         << "value provided.\n"
         << "\n"
         << "One example of coercion in a financial context would be if you "
         << "have a type which groups together some market parameters.\n"
         << "You might provide some default instances of the type based on "
         << "a string representing some market characteristic, e.g. currency, "
         << "contract type etc.\n"
         << "\n"
         << "Finally within the \\{..\\} of the struct definition you can also "
         << "define some verbatim code.\n"
         << "This is code that is simply placed within the generated code "
         << "without any modifications.\n"
         << "To indicate verbatim code, start with \\texttt{protected}, "
         << "\\texttt{private} or \\texttt{public} followed immediately by "
         << "`:'.\n"
         << "The verbatim block ends when the configuration parser detects "
         << "the closing \\} for the class definition.\n"
         << "\n"
         << "If you need to validate the inputs when constructing the class "
         << "then you must provide \\texttt{\\$constructorValidationCode} at "
         << "the end of the struct definition.\n"
         << "This is verbatim code which appears within \\{\\}.\n"
         << "For example, you may wish to check that certain array inputs "
         << "have the same size, or that particular values are in range.\n"
         << "\n"
         << "If there is no need for validation code, then you will need to "
         << "end the class with `;'\n"
         << "\n"
         << "As a final point - it is quite possible that if you are wrapping "
         << "a C/C++ library that you can do without the use of the "
         << "\\texttt{struct} keyword entirely.\n"
         << "For your information, we do use it in some of the implementation "
         << "of the code generation mechanism.\n"
         << "SPCL creates an output file which defines the interface for "
         << "the subsequent code generators (e.g. SPPY, SPXL etc).\n"
         << "This output file consists of standard SPI-text serialization "
         << "of a set of classes defined by using an SPI-configuration "
         << "file which uses \\texttt{struct} to define all the types.\n"
         << "\n";
}

/**
 * Parses the inner class definition. If we don't read '<' first, then returns
 * an empty object.
 */
InnerClassConstSP parseInnerClass(
    ConfigLexer& lexer,
    ModuleDefinitionSP& module,
    ServiceDefinitionSP& service,
    bool verbose)
{
    ConfigLexer::Token token = getTokenOfType(lexer, '<');
    //if (token.type != '<')
    //{
    //    lexer.returnToken(token);
    //    return InnerClassConstSP();
    //}

    std::string innerName;
    InnerClassConstSP innerClass;

    token = lexer.getToken();
    if (token.toString() == "typename")
    {
        token = getTokenOfType(
            lexer, SPI_CONFIG_TOKEN_TYPE_NAME, "InnerClass");
        innerClass = service->getInnerClass(token.value.aName);
        if (!innerClass)
        {
            SPI_THROW_RUNTIME_ERROR("undefined typename " << token.value.aName);
        }
        getTokenOfType(lexer, '>');
    }
    else
    {
        bool isOpen = false;
        bool isShared = false;
        bool isConst = false;
        bool isStruct = false;
        bool isCached = false;
        bool allowConst = false;
        std::string sharedPtr = service->getSharedPtr();

        while (true)
        {
            std::string tokenString = token.toString();
            if (tokenString == "open")
            {
                isOpen = true;
            }
            else if (tokenString == "shared")
            {
                isShared = true;
            }
            else if (tokenString == "const")
            {
                isConst = true;
            }
            else if (tokenString == "allowConst")
            {
                allowConst = true;
            }
            else if (tokenString == "struct")
            {
                isStruct = true;
            }
            else if (tokenString == "cached")
            {
                isCached = true;
            }
            else if (tokenString == "sharedPtr")
            {
                token = getTokenOfType(lexer, '=');
                token = getTokenOfType(lexer, SPI_CONFIG_TOKEN_TYPE_STRING, "sharedPtr");
                sharedPtr = token.value.aString;
                isShared = true;
            }
            else
            {
                lexer.returnToken(token);
                break;
            }
            token = lexer.getToken();
        }

        std::string fullInnerName = getCppTypeName(lexer, true);

        std::string ns;
        splitCppTypeName(fullInnerName, innerName, ns);
        //std::cout << "InnerClass parsing ns:" << ns << " innerName:" << innerName << std::endl;

        //std::vector<std::string> namespaceParts;
        //if (token.toString() == "::")
        //{
        //    namespaceParts.push_back("");
        //    token = lexer.getToken();
        //}
        //lexer.returnToken(token);
        //token = getTokenOfType(
        //    lexer, SPI_CONFIG_TOKEN_TYPE_NAME, "InnerClass");

        //innerName = token.value.aName;
        //token = lexer.getToken();
        //while (token.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD &&
        //       strcmp(token.value.aKeyword, "::") == 0)
        //{
        //    namespaceParts.push_back(innerName);
        //    token = getTokenOfType(
        //        lexer, SPI_CONFIG_TOKEN_TYPE_NAME, "InnerClass");
        //    innerName = token.value.aName;
        //    token = lexer.getToken();
        //}
        token = lexer.getToken();
        bool byValue = false;
        if (token.type == '<')
        {
            std::string templateArg = getCppTypeName(lexer, true);
            token = lexer.getToken();
            if (token.type != '>')
                throw spi::RuntimeError("Expecting '>' actual was %s",
                    token.toString().c_str());
            token = lexer.getToken();

            InnerClassTemplateConstSP innerClassTemplate = service->getInnerClassTemplate(fullInnerName);
            if (!innerClassTemplate)
            {
                throw spi::RuntimeError("template '%s' has not been defined",
                    fullInnerName.c_str());
            }
            if (token.type == '&')
            {
                byValue = true;
                token = lexer.getToken();
            }
            innerClass = innerClassTemplate->MakeInnerClass(templateArg,
                sharedPtr, isShared, isConst, isOpen, isStruct, isCached, byValue, allowConst);
        }
        else
        {
            std::string boolTest;
            if (token.type == '&')
            {
                byValue = true;
                token = lexer.getToken();
            }
            if (token.type == SPI_CONFIG_TOKEN_TYPE_STRING)
            {
                boolTest = token.toString();
                token = lexer.getToken();
            }
            innerClass = InnerClass::Make(
                innerName, ns, "", "", "", sharedPtr,
                isShared, isConst, isOpen, isStruct, isCached, false, byValue, boolTest, allowConst);
        }

        if (token.type != '>')
            throw spi::RuntimeError("Expecting '>' actual was %s",
                                    token.toString().c_str());

        //std::string ns = spi::StringJoin("::", namespaceParts);
        service->addInnerClass(innerClass);
        module->addInnerClass(innerClass);
    }
    return innerClass;
}

void classNoWrapHandler(
    ConfigLexer& lexer,
    const std::vector<std::string>& description,
    const std::string& className,
    bool isVirtual,
    ModuleDefinitionSP& module,
    ServiceDefinitionSP& service,
    bool verbose)
{
    // a class without wrapping is very similar to a struct
    // the difference is that fields are declared privately
    // with accessor methods
    // 
    // in addition classes require private/public for attributes
    // 
    // we start parsing via classKeywordHandler and for the
    // case without wrapping we call this function
    ConfigLexer::Token token = lexer.getToken();
    SPI_PRE_CONDITION(token.type != '<');

    const std::string& ns = module->moduleNamespace();

    ClassConstSP baseClass;


    // implement as Struct except that we use accessors for attributes
    // and specify these as public or private like in WrapperClass

    if (token.type == ':')
    {
        std::string baseClassName(token.value.aName);
        baseClass = service->getClass(baseClassName);
        if (!baseClass)
        {
            throw spi::RuntimeError("Undefined base class: %s",
                baseClassName.c_str());
        }
    }
    else
    {
        lexer.returnToken(token);
    }

    static ParserOptions defaultOptions;
    if (defaultOptions.size() == 0)
    {
        initClassStructOptions(defaultOptions, false);
    }

    ParserOptions options;
    options = parseOptions(lexer, "{", defaultOptions, verbose);
    getTokenOfType(lexer, '{');

    bool noMake = getOption(options, "noMake")->getBool();
    bool canPut = getOption(options, "canPut")->getBool();
    bool noId = getOption(options, "noId")->getBool();
    bool asValue = getOption(options, "asValue")->getBool();
    bool byValue = getOption(options, "byValue")->getBool();
    bool ignore = getOption(options, "ignore")->getBool();
    bool uuid = getOption(options, "uuid")->getBool();
    std::string xlFuncName = getOption(options, "xlFuncName")->getString();
    std::string funcPrefix = getOption(options, "funcPrefx")->getString();
    std::string constructor = getOption(options, "constructor")->getString();
    std::string instance = getOption(options, "instance")->getString();

    StructSP type = Struct::Make(
        description, className, module->moduleNamespace(),
        baseClass, noMake,
        getOption(options, "objectName")->getString(),
        canPut, noId, isVirtual, asValue, uuid, byValue, true,
        funcPrefix.empty() ? xlFuncName : funcPrefix, constructor, instance);

    // we need to register the type before parsing the contents in order to
    // allow references to itself in the class definition

    type->getDataType(service, ignore);

    token = lexer.getToken();
    while (token.type != '}')
    {
        lexer.returnToken(token);

        DescriptionParser desc;
        token = desc.consume(lexer, verbose);
        if (token.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD)
        {
            std::string keyword(token.value.aKeyword);
            if (keyword == "virtual" ||
                keyword == "extern" ||
                keyword == "static")
            {
                bool isVirtual = keyword == "virtual";
                bool isStatic = keyword == "static";
                ClassMethodConstSP classMethod = classMethodKeywordHandler(
                    lexer, desc.take(), module, service, className, baseClass, false,
                    isVirtual, isStatic, verbose, byValue);
                if (classMethod)
                    type->addMethod(classMethod);
            }
            else if (keyword == "private" ||
                keyword == "public" ||
                keyword == "property")
            {
                ClassAttributeAccess::Enum accessLevel;
                if (keyword == "private")
                {
                    accessLevel = ClassAttributeAccess::PRIVATE;
                }
                else if (keyword == "public")
                {
                    accessLevel = ClassAttributeAccess::PUBLIC;
                }
                else
                {
                    accessLevel = ClassAttributeAccess::PROPERTY;
                }

                ClassAttributeConstSP attr = parseClassAttribute(lexer, accessLevel, desc.take(), module, service);
                type->addClassAttribute(attr);
            }
            else if (keyword == "private:" ||
                keyword == "protected:" ||
                keyword == "public:")
            {
                const std::string& verbatimStart = keyword;
                VerbatimConstSP    verbatim = lexer.getVerbatim('{', '}', ";");
                type->addVerbatim(verbatimStart, verbatim);
                break;
            }
            else if (keyword == "operator")
            {
                CoerceToConstSP coerceTo = parseCoerceTo(
                    lexer, className, desc.take(), module, service, verbose);
                type->addCoerceTo(coerceTo);
            }
            else
            {
                throw spi::RuntimeError("invalid keyword %s", keyword.c_str());
            }
        }
        else if (token.type == SPI_CONFIG_TOKEN_TYPE_NAME &&
            className == token.value.aName)
        {
            CoerceFromConstSP coerceFrom = parseCoerceFrom(
                lexer, className, desc.take(), module, service, verbose);
            type->addCoerceFrom(coerceFrom);
        }
        else if (token.type == SPI_CONFIG_TOKEN_TYPE_NAME &&
            strcmp(token.value.aName, "dynamic_properties") == 0)
        {
            VerbatimConstSP code = parseImplementation(lexer, true);
            type->addDynamicProperties(code);
        }
        else if (token.type == SPI_CONFIG_TOKEN_TYPE_NAME &&
            strcmp(token.value.aName, "as_value") == 0)
        {
            VerbatimConstSP code = parseAsValueCode(lexer);
            type->addAsValueCode(code);
        }
        else
        {
            throw spi::RuntimeError("expecting keyword - actual was (%s)",
                token.toString().c_str());
        }
        token = lexer.getToken();
    }

    token = lexer.getToken();
    if (token.type == '{')
    {
        // we need to parse verbatim code for constructing the inner type
        VerbatimConstSP verbatim = lexer.getVerbatim();
        type->addValidation(verbatim);
    }
    else if (token.type != ';')
    {
        throw spi::RuntimeError("expecting ; or {} after class definition");
    }

    if (!ignore)
    {
        service->addClass(type);
        module->addConstruct(type);
        if (!constructor.empty())
        {
            addConstructorFunction(type, constructor, module, service, verbose);
        }
    }
}

void classKeywordHandler(
    ConfigLexer& lexer,
    const std::vector<std::string>& description,
    ModuleDefinitionSP& module,
    ServiceDefinitionSP& service,
    bool verbose)
{
    // a class has the following tokens
    //
    // class Name <inner-class-definition>
    // {
    //    type name;
    //    ...
    // } options;
    //
    // class is taken as read
    ConfigLexer::Token token = lexer.getToken();

    bool isVirtual(false);
    bool isDelegate(false);
    bool isMap(false);

    const std::string& ns = module->moduleNamespace();

    if (token.toString() == "virtual")
    {
        isVirtual = true;
        token = lexer.getToken();
        if (token.toString() == "delegate")
        {
            isDelegate = true;
        }
        else
        {
            lexer.returnToken(token);
        }
    }
    else if (token.toString() == "map")
    {
        isMap = true;
    }
    else
    {
        lexer.returnToken(token);
    }

    token = getTokenOfType(lexer, SPI_CONFIG_TOKEN_TYPE_NAME, "Name");
    std::string name(token.value.aName);

    if (isMap)
    {
        static ParserOptions defaultMapOptions;
        if (defaultMapOptions.size() == 0)
        {
            defaultMapOptions["canPut"] = BoolConstant::Make(false);
            defaultMapOptions["ignore"] = BoolConstant::Make(false);
            defaultMapOptions["uuid"] = BoolConstant::Make(false);
        }
        ParserOptions options;
        options = parseOptions(lexer, ";", defaultMapOptions, verbose);
        getTokenOfType(lexer, ';');
        bool ignore = getOption(options, "ignore")->getBool();

        MapClassConstSP type = MapClass::Make(
            description,
            name,
            module->moduleNamespace(),
            getOption(options, "canPut")->getBool(),
            getOption(options, "uuid")->getBool());

        type->getDataType(service, ignore);
        if (!ignore)
        {
            service->addClass(type);
            module->addConstruct(type);
        }
        return;
    }

    token = lexer.getToken();
    if (token.type != '<')
    {
        if (isDelegate)
            SPI_THROW_RUNTIME_ERROR("Cannot define no-wrap class '" << name << "' as a delegate");
        lexer.returnToken(token);
        classNoWrapHandler(lexer, description, name, isVirtual, module, service, verbose);
        return;
    }

    WrapperClassConstSP baseWrapperClass;

    static ParserOptions defaultOptions;
    if (defaultOptions.size() == 0)
    {
        initClassStructOptions(defaultOptions, true);
    }

    lexer.returnToken(token);
    InnerClassConstSP innerClass = parseInnerClass(lexer, module, service, verbose);

    token = lexer.getToken();
    if (token.type == ':')
    {
        token = getTokenOfType(lexer, SPI_CONFIG_TOKEN_TYPE_NAME, "BaseClass");
        std::string baseClassName = token.value.aName;
        bool hasNamespace = false;

        token = lexer.getToken();
        if (token.type == '.')
        {
            // note only one level of namespace supported
            hasNamespace = true;
            token = getTokenOfType(lexer, SPI_CONFIG_TOKEN_TYPE_NAME, "BaseClass");
            baseClassName = baseClassName + "." + token.value.aName;
        }
        else
        {
            lexer.returnToken(token);
        }
        ClassConstSP baseClass;

        // classes defined in the current namespace take precedence over a class
        // with the same name in the global namespace
        if (!hasNamespace && !ns.empty())
        {
            std::string baseClassNameNS = ns + "." + baseClassName;
            baseClass = service->getClass(baseClassNameNS);
        }

        if (!baseClass)
            baseClass = service->getClass(baseClassName);

        if (!baseClass)
        {
            throw spi::RuntimeError("Undefined base class: %s",
                baseClassName.c_str());
        }
        baseWrapperClass = spi_boost::dynamic_pointer_cast<
            const WrapperClass>(baseClass);
        if (!baseWrapperClass)
        {
            throw spi::RuntimeError("Base class %s is not of type class",
                baseClassName.c_str());
        }
    }
    else
    {
        lexer.returnToken(token);
    }

    ParserOptions options;
    options = parseOptions(lexer, "{", defaultOptions, verbose);
    getTokenOfType(lexer, '{');

    bool ignore = getOption(options, "ignore")->getBool();
    bool uuid = getOption(options, "uuid")->getBool();
    std::string sharedPtr = getOption(options, "sharedPtr")->getString();
    if (!sharedPtr.empty())
    {
        innerClass = innerClass->setSharedPtr(sharedPtr);
    }

    std::string funcPrefix = getOption(options, "funcPrefix")->getString();
    std::string constructor = getOption(options, "constructor")->getString();
    WrapperClassSP type = WrapperClass::Make(
        description, name, module->moduleNamespace(), innerClass,
        baseWrapperClass, isVirtual,
        getOption(options, "noMake")->getBool(),
        getOption(options, "objectName")->getString(),
        isDelegate,
        getOption(options, "canPut")->getBool(),
        getOption(options, "noId")->getBool(),
        getOption(options, "asValue")->getBool(),
        uuid,
        funcPrefix.empty() ? getOption(options, "xlFuncName")->getString() : funcPrefix,
        constructor,
        getOption(options, "instance")->getString());

    // we need to register the type before parsing the contents in order to
    // allow references to itself in the class definition

    type->getDataType(service, ignore);

    //token = getTokenOfType(lexer, '{');

    token = lexer.getToken();
    while (token.type != '}')
    {
        lexer.returnToken(token);

        DescriptionParser desc;
        token = desc.consume(lexer, verbose);
        if (token.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD)
        {
            std::string keyword(token.value.aKeyword);
            if (keyword == "virtual" ||
                keyword == "extern" ||
                keyword == "static")
            {
                bool isVirtual = keyword == "virtual";
                bool isStatic = keyword == "static";
                ClassMethodConstSP classMethod = classMethodKeywordHandler(
                    lexer, desc.take(), module, service, name,
                    StructConstSP(), false,
                    isVirtual, isStatic, verbose, innerClass->byValue());
                if (classMethod)
                    type->addMethod(classMethod);
            }
            else if (keyword == "private" ||
                keyword == "public" ||
                keyword == "property")
            {
                ClassAttributeAccess::Enum accessLevel;
                if (keyword == "private")
                {
                    accessLevel = ClassAttributeAccess::PRIVATE;
                }
                else if (keyword == "public")
                {
                    accessLevel = ClassAttributeAccess::PUBLIC;
                }
                else
                {
                    accessLevel = ClassAttributeAccess::PROPERTY;
                }

                ClassAttributeConstSP attr = parseClassAttribute(lexer, accessLevel, desc.take(), module, service);
                type->addClassAttribute(attr);
            }
            else if (keyword == "delegate")
            {
                // we need to do a fairly loose parsing for the delegate
                //
                // we assume that if you are so advanced as to be writing
                // delegate classes that you can write the necessary
                // conversion routines
                //
                // thus the main thing that we need is to be able to detect
                // the prototype and the implementation

                std::ostringstream oss;

                token = lexer.getToken();
                while (token.type != '{')
                {
                    oss << token.toString() << " ";
                    token = lexer.getToken();
                }

                std::string prototype = oss.str();

                VerbatimConstSP implementation = lexer.getVerbatim();
                type->addDelegate(prototype, implementation);
            }
            else if (keyword == "private:" ||
                keyword == "protected:" ||
                keyword == "public:")
            {
                const std::string& verbatimStart = keyword;
                VerbatimConstSP    verbatim = lexer.getVerbatim('{', '}', ";");
                type->addVerbatim(verbatimStart, verbatim);
                break;
            }
            else if (keyword == "operator")
            {
                CoerceToConstSP coerceTo = parseCoerceTo(
                    lexer, name, desc.take(), module, service, verbose);
                type->addCoerceTo(coerceTo);
            }
            else
            {
                throw spi::RuntimeError("invalid keyword %s", keyword.c_str());
            }
        }
        else if (token.type == SPI_CONFIG_TOKEN_TYPE_NAME &&
            name == token.value.aName)
        {
            CoerceFromConstSP coerceFrom = parseCoerceFrom(
                lexer, name, desc.take(), module, service, verbose);
            type->addCoerceFrom(coerceFrom);
        }
        else if (token.type == SPI_CONFIG_TOKEN_TYPE_NAME &&
            strcmp(token.value.aName, "validate_inner") == 0)
        {
            // idea is that we might want to wrap the same underlying type
            // more than once in the same class hierarchy
            //
            // for example, if the inner type is represented using a union
            //
            // in such cases dynamic casts are not enough - we need extra
            // validation for the inner type
            //
            // we expect the following: validate_inner(inner) { ... }
            //
            // validate_inner will return true/false rather than throw
            // an exception (of course it can always throw an exception
            // as well, but this won't be treated as false)

            ConfigLexer::Token token = lexer.getToken();
            if (token.type != '(')
            {
                throw spi::RuntimeError(
                    "Expect '(' after validate_inner");
            }

            token = lexer.getToken();
            if (token.type != SPI_CONFIG_TOKEN_TYPE_NAME)
            {
                throw spi::RuntimeError(
                    "Expect variable name after validate_inner(");
            }
            std::string innerName = token.value.aName;

            token = lexer.getToken();
            if (token.type != ')')
            {
                throw spi::RuntimeError(
                    "Expect ')' after validate_inner(%s",
                    innerName.c_str());
            }
            VerbatimConstSP code = parseImplementation(lexer, true);
            type->addValidateInner(innerName, code);
        }
#if 0
        else if (token.type == SPI_CONFIG_TOKEN_TYPE_NAME &&
            strcmp(token.value.aName, "set_inner") == 0)
        {
            // we have changed the constructor code so that there is a static
            // function called make_inner and then we call the constructor
            // with the inner type as the parameter
            //
            // this doesn't allow the constructor code to access data fields
            // of the class
            //
            // hence set_inner - which has exactly the same syntax as
            // validate_inner
            ConfigLexer::Token token = lexer.getToken();
            if (token.type != '(')
            {
                throw spi::RuntimeError(
                    "Expect '(' after set_inner");
            }

            token = lexer.getToken();
            if (token.type != SPI_CONFIG_TOKEN_TYPE_NAME)
            {
                throw spi::RuntimeError(
                    "Expect variable name after validate_inner(");
            }
            std::string innerName = token.value.aName;

            token = lexer.getToken();
            if (token.type != ')')
            {
                throw spi::RuntimeError(
                    "Expect ')' after set_inner(%s",
                    innerName.c_str());
            }
            VerbatimConstSP code = parseImplementation(lexer, true);
            type->addSetInner(innerName, code);
        }
#endif
        else if (token.type == SPI_CONFIG_TOKEN_TYPE_NAME &&
            strcmp(token.value.aName, "dynamic_properties") == 0)
        {
            VerbatimConstSP code = parseImplementation(lexer, true);
            type->addDynamicProperties(code);
        }
        else if (token.type == SPI_CONFIG_TOKEN_TYPE_NAME &&
            strcmp(token.value.aName, "as_value") == 0)
        {
            VerbatimConstSP code = parseAsValueCode(lexer);
            type->addAsValueCode(code);
        }
        else
        {
            throw spi::RuntimeError("expecting keyword - actual was (%s)",
                token.toString().c_str());
        }
        token = lexer.getToken();
    }

    token = lexer.getToken();
    if (token.type == '{')
    {
        // we need to parse verbatim code for constructing the inner type
        type->setVerbatimConstructor(lexer.getVerbatim());
    }
    else if (token.type != ';')
    {
        throw spi::RuntimeError("expecting ; or {} after class definition");
    }

    if (!ignore)
    {
        service->addClass(type);
        module->addConstruct(type);
        if (!constructor.empty())
        {
            addConstructorFunction(type, constructor, module, service, verbose);
        }
    }
}

void writeClassDoc(GeneratedOutput& ostr, bool verbose)
{
    ostr << "\\newpage\n"
         << "\\subsubsection{Wrapper class} \\label{class}\n"
         << "\n"
         << "Wrapper classes can be defined for the outer library.\n"
         << "These are classes that wrap an equivalent class or type from the "
         << "inner library.\n"
         << "In general we implement functionality by delegation using the "
         << "pointer to implementation pattern.\n"
         << "\n"
         << "Classes have data and functions - so when we define a wrapper "
         << "class we must define the attributes of the class, and implement "
         << "any methods of the class in the SPI configuration file.\n"
         << "\n"
         << "In the context of a wrapper class there are two classes "
         << "involved.\n"
         << "We will call these the inner class and the outer class.\n"
         << "Sometimes the inner class will not be a class and instead "
         << "will be a pre-defined type (via the \\texttt{typename} keyword "
         << "(see page \\pageref{typename})).\n"
         << "Nevertheless we will still refer to the inner type generally as "
         << "the inner class.\n";

    writeTexBeginSyntax(ostr, true);
    ostr << "    class [virtual [delegate]] \\emph{className} "
         << "[ : \\emph{baseClassName} ]\n"
         << "    < [open] [shared] [const] [struct] \\emph{innerClassName} |\n"
         << "      typename \\emph{typeName} >\n"
         << "    ...\n"
         << "    \\{\n"
         << "        // attributes and properties\n"
         << "        \\emph{/** description */}\n"
         << "        public|private|property \\emph{dataType} "
         << "[\\emph{arrayDim}] \\emph{attrName}\n"
         << "        \\{ \\emph{accessorCode} \\} | ;\n"
         << "        ...\n"
         << "        // member functions\n"
         << "        \\emph{/** description */}\n"
         << "        extern|static|virtual \\emph{returnType} [\\emph{arrayDim}] "
         << "\\emph{methodName}\n"
         << "        \\{ \\emph{implementation} \\} | ;\n"
         << "        ...\n"
         << "        delegate \\emph{innerMethodDeclaration} \n"
         << "        \\{ \\emph{implementation} \\}\n"
         << "        ...\n"
         << "        // coerce from other types\n"
         << "        \\emph{/** description */}\n"
         << "        $className (\\emph{dataType} \\emph{varName})\n"
         << "        \\{ \\emph{implementation} \\}\n"
         << "        ...\n"
         << "        // verbatim code\n"
         << "    private|public|protected:\n"
         << "        \\emph{verbatim}\n"
         << "    \\}\n"
         << "    \\{ \\emph{innerClassConstructorCode} \\} | ;\n";
    writeTexEndSyntax(ostr);

    ostr << "\n"
         << "with the following optional parameters (defined before the "
         << "opening \\{ which defines the internal details of the class):";

    writeTexBeginOptionsTable(ostr);
    writeTexOptionsTableRow(
        ostr, "noMake", "False", "True or False");
    writeTexOptionsTableRow(
        ostr, "objectName", "", "Alternative string used in object serialization");
    writeTexEndOptionsTable(ostr);

    ostr << "\n"
         << "The \\texttt{class} definition has many components which we "
         << "will describe in detail.\n"
         << "\\begin{itemize}\n"
         << "\\item \\texttt{\\$className}\n"
         << "\\item \\texttt{\\$baseClassName} (optional)\n"
         << "\\item \\texttt{\\$innerClass}\n"
         << "\\item \\texttt{\\$attributes} (any number)\n"
         << "\\item \\texttt{\\$methods} (any number)\n"
         << "\\item \\texttt{\\$delegateMethods} (any number)\n"
         << "\\item \\texttt{\\$coerceFrom} (any number)\n"
         << "\\item \\texttt{\\$verbatim} (optional)\n"
         << "\\item \\texttt{\\$innerClassConstructorCode} (optional)\n"
         << "\\end{itemize}\n"
         << "\n"
         << "The \\texttt{class} keyword defines a C++ class in the outer "
         << "library with the name \\texttt{\\$className}.\n"
         << "The class will be implemented by wrapper an inner class and "
         << "we will call it the `outer class'.\n"
         << "\n"
         << "Before the \\texttt{\\$className} there are two optional "
         << "keywords:\n"
         << "\\begin{description}\n"
         << "\\item[virtual:] This means that we are defining a base class "
         << "and that in practice we expect to define sub-classes of this "
         << "class which will provide the actual implementation.\n"
         << "We will be wrapping the base class of the equivalent class "
         << "hierarchy from the inner library.\n"
         << "\\item[delegate:] We can only define a class to be a delegate "
         << "class if it also defined as virtual.\n"
         << "Delegate classes are an advanced feature that allow you to "
         << "sub-class the outer class and create a sub-class of the inner "
         << "class via delegation. See page \\pageref{delegateClass} for "
         << "more details about delegate classes.\n"
         << "\\end{description}\n"
         << "\n"
         << "The name \\texttt{\\$className} must be unique and also defines "
         << "a new data type.\n"
         << "\n"
         << "The class can be a sub-class of another previously defined "
         << "class via the definition of \\texttt{\\$baseClassName}.\n"
         << "In this case the \\texttt{\\$baseClassName} must also be a "
         << "wrapper class, i.e. defined using the \\texttt{class} keyword.\n"
         << "We define inheritance using the C++ syntax by the use of `:'.\n"
         << "All inheritance in SPI is \\texttt{public} so we don't need to "
         << "specify it in the configuration file.\n"
         << "\n";

    ostr << "\\subsubsection*{Inner class definition} \\label{innerClass}\n"
         << "\n"
         << "After the optional base class definition, we define the inner "
         << "class with $<..>$ notation - i.e. similar to the template "
         << "notation of C++.\n"
         << "\n"
         << "There are two options for the inner class definition.\n"
         << "\n"
         << "Normally you define the inner class name, and then by the use "
         << "of four optional keywords you define various properties of the "
         << "inner class as follows:\n"
         << "\\begin{description}\n"
         << "\\item[open:] This is an extremely important part of the "
         << "definition.\n"
         << "Declaring the inner class as \\texttt{open} means that given an "
         << "instance of the inner class you can access the data from that "
         << "class in such a way that you have enough data to be able to "
         << "reconstruct the class by calling one of its constructors.\n"
         << "In other words, you can see the `state' of the class.\n"
         << "Not all classes are implemented in this manner.\n"
         << "In the traditional C++ paradigm, a class is a black box and "
         << "you cannot necessarily see its state - it contains functions "
         << "and data together behind the encapsulation layer.\n"
         << "If a class is not marked as \\texttt{open} then it is treated "
         << "as a \\texttt{closed} class - and by default we assume that "
         << "classes are \\texttt{closed}.\n"
         << "\n"
         << "Whether a class is open or closed will greatly affect how we "
         << "serialize the class.\n"
         << "For an \\texttt{open} class, we do not need to remember how the "
         << "object was created - we can look inside the inner class and "
         << "determine its state and serialize it accordingly.\n"
         << "However for a \\texttt{closed} class, we must remember how the "
         << "object was created and serialize the attributes that were used "
         << "to construct the class.\n"
         << "\n"
         << "As far as SPI is concerned it is probably better if the inner "
         << "class is \\texttt{open}, but one of the guiding principles of "
         << "SPI is that we can cope with the library we are wrapping as it "
         << "is without needing it to be modified.\n"
         << "Hence we support both \\texttt{open} and \\texttt{closed} inner "
         << "classes.\n"
         << "The one thing we do not allow is a mixture of \\texttt{open} and "
         << "\\texttt{closed} in the same class hierarchy (this may change).\n"
         << "\n"
         << "\\item[shared:] The pointer to implementation used in the "
         << "outer class to point to the inner class is always a shared "
         << "pointer.\n"
         << "However the inner library might not in general expect the value "
         << "to be provided as a shared pointer.\n"
         << "If the inner library does use shared pointers, then specify the "
         << "inner class to be \\texttt{shared}.\n"
         << "Otherwise you will receive the inner class as a raw pointer.\n"
         << "\n"
         << "Note that by default we assume that shared pointers for the inner "
         << "library are defined by \\verb|boost::shared_ptr|.\n"
         << "However this can be overridden at the service level when the "
         << "\\texttt{\\%service} keyword (see page \\pageref{service}) was "
         << "defined.\n"
         << "What we do not allow is more than one type of shared pointer "
         << "class in the inner library.\n"
         << "\n"
         << "\\item[const:] The pointer to implementation can either be "
         << "const or not-const.\n"
         << "If the inner library uses this type as const, then indicate this "
         << "by using the \\texttt{const keyword}.\n"
         << "\n"
         << "\\item[struct:] The inner class is pre-declared in the header "
         << "files of the generated code.\n"
         << "Usually the pre-declaration is done using the \\texttt{class} "
         << "keyword.\n"
         << "However if the class is declared in the inner library as "
         << "\\texttt{struct} then you need to declare the inner class as "
         << "such by using the \\texttt{struct} keyword at this point.\n"
         << "\n"
         << "\\end{description}\n"
         << "\n"
         << "The alternative way to define the inner type is by using the "
         << "\\texttt{typename} keyword to use a type that was previously "
         << "defined using \\texttt{typename}.\n"
         << "In this case you simply using the notation "
         << "\\verb|<typename inner_type>| where \\texttt{\\$inner\\_type} "
         << "was defined earlier (see page \\pageref{typename} for the "
         << "syntax).\n"
         << "Note that the \\texttt{typename} command has options for "
         << "defining the equivalent of \\texttt{open}, \\texttt{shared} and "
         << "\\texttt{const}.\n"
         << "\n"
         << "Note that in the generated code we \\texttt{typedef inner\\_type} "
         << "to be the type that is actually returned by the outer class "
         << "when we requested the corresponding inner class.\n"
         << "Sometimes the typename \\texttt{\\$className::inner\\_type} will "
         << "be easier to use than the full shared pointer declaration.\n"
         << "\n";

    ostr << "\\subsubsection*{Wrapper class options}\n"
         << "\n"
         << "The optional \\texttt{\\$noMake} parameter indicates that the "
         << "class cannot be constructed in directly in Excel, Python and "
         << ".NET implementations.\n"
         << "\n"
         << "Note that in all cases we generate in the C++ a public static "
         << "Make function that creates a shared pointer to the class.\n"
         << "The actual constructor of the class is always protected rather "
         << "than public to prevent direct use of the constructor for "
         << "creating raw pointers.\n"
         << "\n"
         << "However even if we set \\texttt{\\$noMake} to \\texttt{True}, "
         << "we still provide the public static function Make in the C++ "
         << "interface.\n"
         << "The reason is that sometimes we define a class to use "
         << "\\texttt{noMake=True}, but we want another function to be "
         << "able to create the class, in which case that function might "
         << "well be defined with \\texttt{noConvert=True} and call the Make "
         << "function directly.\n"
         << "\n";

    ostr << "\\subsubsection*{Wrapper class attributes and properties}\n"
         << "\n"
         << "The class can have any number of \\texttt{\\$attributes} and "
         << "\\texttt{\\$properties}.\n"
         << "An attribute or property is defined using one of the keywords "
         << "\\texttt{public}, \\texttt{private} or \\texttt{property} "
         << "followed by a data type (with optional array indicator) "
         << "and the name of the attribute (or property).\n"
         << "In many cases, the definition is followed by a code fragment "
         << "which describes how to get the data item from the class, but "
         << "if this is not necessary then the definition is terminatd by "
         << "`;'.\n"
         << "\n"
         << "The three possible keywords have the following meanings:\n"
         << "\n"
         << "\\begin{description}\n"
         << "\\item[public:] Defines an attribute of the class which will be "
         << "available to be fetched from the class and is used in the "
         << "construction of the class.\n"
         << "It will appear in the serialization to file of the class.\n"
         << "\\item[private:] Defines an attribute of the class which will "
         << "not be available to be fetched from the class and is used in "
         << "the construction of the class.\n"
         << "Despite not being available to be fetched from the class, it "
         << "will appear in the serialization to file of the class.\n"
         << "\\item[property:] Defines a property of the class which will "
         << "be available to be fetched from the class, but is not used in "
         << "the construction of the class.\n"
         << "It will not appear in the serialization to file of the class.\n"
         << "\\end{description}\n"
         << "\n"
         << "The list of attributes defines the constructor of the class "
         << "as well as the serialization of the class.\n"
         << "\n"
         << "Each attribute or property has an optional code fragment.\n"
         << "Sometimes you won't need to provide a code fragment, in which "
         << "case the code generated will be equivalent to:\n"
         << "\\verb|{ return self->name; }| for an attribute called "
         << "\\texttt{name}.\n"
         << "The variable \\texttt{self} corresponds to the instance of the "
         << "inner type.\n"
         << "We use \\texttt{self} since it is the traditional name of the "
         << "class variable in Python code (the equivalent is called "
         << "\\texttt{this} in C++ code), and hence we would not expect "
         << "any class attributes to be actually called \\texttt{self}.\n"
         << "\n"
         << "So using \\texttt{self} you can write any code fragment to "
         << "return the value of the attribute (or property).\n"
         << "The value returned should be of the inner type, and the code "
         << "generator will write the code which translates this value (as "
         << "required) into the corresponding outer type.\n"
         << "\n";

    ostr << "\\subsubsection*{Wrapper class constructor}\n"
         << "\n"
         << "The generated constructor takes as input the parameters "
         << "corresponding to the attributes of the class (in the order "
         << "that the attributes are defined).\n"
         << "The constructor is declared in the protected section of the "
         << "class in the generated code, and in the public section of "
         << "the class there is a static method called Make which returns "
         << "a shared pointer to the class and simply calls the constructor.\n"
         << "\n"
         << "If the attributes of the class (once converted to the "
         << "inner types) correspond to an existing public constructor of the "
         << "inner class, then you don't need to provide the constructor for "
         << "the wrapper class.\n"
         << "However if this is not the case then you will need to provide "
         << "some constructor code.\n"
         << "This is defined after the end of the class definition.\n"
         << "Normally the class definition ends with `;' as it would in C++ "
         << "header files, but if you need special constructor code then "
         << "you will need to provide it in a verbatim code block enclosed "
         << "with \\{...\\} at the end of the class definition (in place of "
         << "the trailing `;'.\n"
         << "\n";

#if 0
         << "The struct can have any number of \\texttt{\\$methods}.\n"
         << "To indicate a method you start with one of the keywords "
         << "\\texttt{extern} or \\texttt{static} or \\texttt{virtual} "
         << "which have the following meanings:\n"
         << "\\begin{description}\n"
         << "\\item[extern:] Defines a normal member function.\n"
         << "We need the \\texttt{extern} keyword to distinguish an attribute "
         << "from a member function in the way discussed when we introduced "
         << "the common SPI syntax rules.\n"
         << "\\item[static:] Defines a static function of the class.\n"
         << "Similar to a stand-alone function except that we have access\n"
         << "to private functions and private global data of the class.\n"
         << "\\item[virtual:] Defines a virtual member function.\n"
         << "This is a function that we expect to be implemented by \n"
         << "sub-classes of this class.\n"
         << "\\end{description}\n"
         << "The remainder of the definition of a method is the same as the "
         << "definition of a standalone function (see page \\pageref{extern}) "
         << "with returnType, name, function parameters, options and "
         << "implementation.\n"
         << "For \\texttt{struct} the implementation is required unless the "
         << "method has been marked as \\texttt{virtual} in which we expect "
         << "the method to be implemented in a derived class.\n"
         << "\n"
         << "The major difference between a method and a function is that "
         << "(unless the method has been marked as \\texttt{static}) is that "
         << "we have instance variables available.\n"
         << "\n"
         << "These will have the same names as the attributes defined in the "
         << "\\texttt{struct} definition.\n"
         << "Note that for a \\texttt{struct} there is no concept of wrapping "
         << "and hence there is no inner type.\n"
         << "\n"
         << "The \\texttt{struct} type also supports the concept of coercion "
         << "from other types.\n"
         << "If the \\texttt{struct} type is simple or standard, then you "
         << "might be able to coerce it from (for example) a number or a "
         << "string.\n"
         << "This is indicated in SPI by defining what looks like a "
         << "constructor to the struct which takes a single parameter.\n"
         << "Then you provide the code which converts that parameter into "
         << "an instance of the class.\n"
         << "In your implementation code you are expected to return a "
         << "shared pointer to the class.\n"
         << "Typically you will actually call the Make method based on the "
         << "value provided.\n"
         << "\n"
         << "One example of coercion in a financial context would be if you "
         << "have a type which groups together some market parameters.\n"
         << "You might provide some default instances of the type based on "
         << "a string representing some market characteristic, e.g. currency, "
         << "contract type etc.\n"
         << "\n"
         << "Finally within the \\{..\\} of the struct definition you can also "
         << "define some verbatim code.\n"
         << "This is code that is simply placed within the generated code "
         << "without any modifications.\n"
         << "To indicate verbatim code, start with \\texttt{protected}, "
         << "\\texttt{private} or \\texttt{public} followed immediately by "
         << "`:'.\n"
         << "The verbatim block ends when the configuration parser detects "
         << "the closing \\} for the class definition.\n"
         << "\n"
         << "If you need to validate the inputs when constructing the class "
         << "then you must provide \\texttt{\\$constructorValidationCode} at "
         << "the end of the struct definition.\n"
         << "This is verbatim code which appears within \\{\\}.\n"
         << "For example, you may wish to check that certain array inputs "
         << "have the same size, or that particular values are in range.\n"
         << "\n"
         << "If there is no need for validation code, then you will need to "
         << "end the class with `;'\n"
         << "\n"
         << "As a final point - it is quite possible that if you are wrapping "
         << "a C/C++ library that you can do without the use of the "
         << "\\texttt{struct} keyword entirely.\n"
         << "For your information, we do use it in some of the implementation "
         << "of the code generation mechanism.\n"
         << "SPCL creates an output file which defines the interface for "
         << "the subsequent code generators (e.g. SPPY, SPXL etc).\n"
         << "This output file consists of standard SPI-text serialization "
         << "of a set of classes defined by using an SPI-configuration "
         << "file which uses \\texttt{struct} to define all the types.\n"
         << "\n";
#endif




}

void writeDelegateClassDoc(GeneratedOutput& ostr, bool verbose)
{
    ostr << "\\newpage\n"
         << "\\subsubsection{Delegate class} \\label{delegateClass}\n"
         << "\n"
         << "Delegate classes are a special type of wrapper class where "
         << "we can also create an inner type given an outer type.\n"
         << "The assumption is that the inner type has a narrow public "
         << "interface, and by implementing all of the functions required "
         << "for that interface we can create a sub-class of the inner type "
         << "which is implemented by delegation to a sub-class of the outer "
         << "type.\n"
         << "\n"
         << "Why might we want to allow this?\n"
         << "\n"
         << "The purpose is for a dynamic language such as Python to allow "
         << "the Python developer to sub-class one of the types provided "
         << "by the SPI-built Python extension and use this sub-class "
         << "(which is written purely in Python) to be used within an "
         << "algorithm which uses the inner type.\n"
         << "\n"
         << "For example, suppose we have a DiscountCurve class which "
         << "is used in finance to provide discount factors for a particular "
         << "date in the future.\n"
         << "We will have many algorithms that use a DiscountCurve as one "
         << "of its parameters.\n"
         << "The main body of the existing algorithm is written in the inner "
         << "library using C++ via the methods of the inner DiscountCurve "
         << "class.\n"
         << "\n"
         << "If we have declared the DiscountCurve to be a delegate class, "
         << "then we can sub-class the Python DiscountCurve and pass this "
         << "object into the SPI-built Python function calls.\n"
         << "These calls will then attempt to extract an instance of the "
         << "inner class from the Python object.\n"
         << "When this happens, we construct a sub-class of the inner class "
         << "for which all the methods needed by the inner class interface "
         << "will be implemented by calling back into Python to use the "
         << "implementation of the Python class we have written.\n"
         << "\n"
         << "The downside of the approach is that we will lose the logging "
         << "capability for such functions, and the Python implementation "
         << "will generally be an order of magnitude slower than the "
         << "equivalent C++ code.\n"
         << "\n"
         << "However the upside is that this approach enables the Python "
         << "developer to develop prototype algorithms very rapidly and "
         << "test them out with real pricing algorithms.\n"
         << "Subsequently if the prototype is a success the code can be "
         << "re-written for speed in C++.\n"
         << "\n";
}

void commandHandler(ModuleDefinitionSP& module,
                    const std::string& command,
                    bool verbose)
{
    if (spi::StringStartsWith(command, "include "))
        module->addInclude(command.substr(8));
    else
        throw spi::RuntimeError("Command %s must be #include command",
                                command.c_str());
}

void mainLoop(const std::string& fn,
              ConfigLexer& lexer,
              ServiceDefinitionSP& service,
              bool verbose)
{
    DescriptionParser desc;

    ConfigLexer::Token token = desc.consume(lexer, verbose);
    ModuleDefinitionSP module;

    // %module must come first
    if (token.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD &&
        strcmp(token.value.aKeyword, "%module") == 0)
    {
        module = moduleKeywordHandler(lexer, desc.take(), verbose);
        service->addModule(module);
    }
    else
    {
        throw spi::RuntimeError("First significant keyword inside module "
                                "definition must be %%module");
    }

    token = desc.consume(lexer,verbose);

    while (token.type)
    {
        if (token.type == SPI_CONFIG_TOKEN_TYPE_KEYWORD)
        {
            std::string keyword(token.value.aKeyword);
            if (keyword == "extern")
            {
                externKeywordHandler(
                    lexer, desc.take(), module, service, true, verbose);
            }
            else if (keyword == "enum")
            {
                enumKeywordHandler(
                    lexer, desc.take(), module, service, verbose);
            }
            else if (keyword == "struct")
            {
                structKeywordHandler(
                    lexer, desc.take(), module, service, verbose);
            }
            else if (keyword == "typedef")
            {
                typedefKeywordHandler(
                    lexer, desc.take(), module, service, verbose);
            }
            else if (keyword == "class")
            {
                classKeywordHandler(
                    lexer, desc.take(), module, service, verbose);
            }
            else if (keyword == "typename")
            {
                typenameKeywordHandler(
                    lexer, desc.take(), module, service, verbose);
            }
            else if (keyword == "template")
            {
                templateKeywordHandler(
                    lexer, desc.take(), module, service, verbose);
            }
            else
            {
                throw spi::RuntimeError("Cannot handle keyword (%s)",
                                        keyword.c_str());
            }
        }
        else if (token.type == SPI_CONFIG_TOKEN_TYPE_COMMAND)
        {
            desc.clear(); // should this validate there is no description?
            commandHandler(module, std::string(token.value.aCommand), verbose);
        }
        else if (token.type == '{')
        {
            VerbatimConstSP verbatim = lexer.getVerbatim();
            module->addVerbatim(verbatim);
        }
        else
        {
            throw spi::RuntimeError("Cannot handle token (%s)",
                               token.toString().c_str());
            desc.clear();
            if (token.type == '{')
            {
                std::vector<std::string> code = lexer.getVerbatim()->getCode();
                std::cout << "\nCODE_BEGIN" << std::endl;
                std::cout << "{" << spi::StringJoin("\n", code) << std::endl;
                std::cout << "CODE_END" << std::endl;
            }
            else
            {
                std::cout << token.toString() << std::endl;
            }
        }
        token = desc.consume(lexer, verbose);
    }
}

} // end of anonymous namespace



void configParser(const std::string& fn,
                  ServiceDefinitionSP& service,
                  bool verbose)
{
    std::ifstream istr(fn.c_str());
    if (!istr)
        throw spi::RuntimeError("Could not open %s", fn.c_str());

    ConfigLexer lexer(fn, &istr);

    try
    {
        mainLoop(fn, lexer, service, verbose);
    }
    catch (std::exception &e)
    {
        throw spi::RuntimeError("%s(%d): %s", fn.c_str(), lexer.getLineNumber(),
            e.what());
    }
    if (lexer.num_errors > 0)
    {
        throw spi::RuntimeError("%d error%s detected in parsing %s",
            lexer.num_errors, lexer.num_errors > 1 ? "s" : "", fn.c_str());
    }
}

void writeConfigParserDoc(
    GeneratedOutput& ostr,
    bool verbose)
{
    ostr << "\\newpage\n"
         << "\\subsection{SPI module file reference}\n"
         << "\n";

    // struct keyword
    // class keyword
    // typename keyword
    // commandHandler
    // verbatim

    // split attribute to functionArgument and classAttribute
    // function arguments can be defined as outputs
    // class attributes have public/private/property plus implementation

    writeModuleDoc(ostr, verbose);
    writeDataTypeDoc(ostr, verbose);
    writeArrayDimDoc(ostr, verbose);
    writeFunctionParamDoc(ostr, verbose);
    writeFunctionDoc(ostr, verbose);
    writeEnumerandDoc(ostr, verbose);
    writeEnumDoc(ostr, verbose);
    writeSimpleTypeDoc(ostr, verbose);
    writeTypenameDoc(ostr, verbose);
    writeTemplateDoc(ostr, verbose);
    writeClassDoc(ostr, verbose);
    writeDelegateClassDoc(ostr, verbose);
    writeStructDoc(ostr, verbose);

}
