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
/*
***************************************************************************
** classMethod.cpp
***************************************************************************
** Implements the ClassMethod class used in the configuration parser.
***************************************************************************
*/

#include "classMethod.hpp"
#include "function.hpp"
#include "wrapperClass.hpp"

#include <spi/RuntimeError.hpp>
#include <spi/Service.hpp>
#include <spi/StringUtil.hpp>
#include <spi/spdoc_configTypes.hpp>

#include "construct.hpp"
#include "serviceDefinition.hpp"
#include "generatorTools.hpp"
#include <spgtools/commonTools.hpp>

namespace
{
AttributeConstSP getInstance(const DataTypeConstSP& classType)
{
    std::vector<std::string> description;
    return Attribute::Make(description, classType, "self");
}

std::vector<AttributeConstSP> getExtendedArgs(
    const DataTypeConstSP& classType,
    const std::vector<AttributeConstSP>& args)
{
    std::vector<AttributeConstSP> xargs;
    xargs.push_back(getInstance(classType));
    xargs.insert(xargs.end(), args.begin(), args.end());
    return xargs;
}
}


/*
***************************************************************************
** Implementation of ClassMethod
***************************************************************************
*/
ClassMethodConstSP ClassMethod::Make(
    const FunctionConstSP& function,
    bool isConst,
    bool isVirtual,
    bool isStatic,
    bool isDelegate,
    bool isImplementation,
    const std::string& implements,
    bool byValue)
{
    return new ClassMethod(function, isConst, isVirtual, isStatic, isDelegate,
                           isImplementation, implements, byValue);
}

ClassMethod::ClassMethod(
    const FunctionConstSP& function,
    bool isConst,
    bool isVirtual,
    bool isStatic,
    bool isDelegate,
    bool isImplementation,
    const std::string& implements,
    bool byValue)
    :
    m_function(function),
    m_isConst(isConst),
    m_isVirtual(isVirtual),
    m_isStatic(isStatic),
    m_isDelegate(isDelegate),
    m_isImplementation(isImplementation),
    m_implements(implements),
    m_byValue(byValue)
{
    if (isStatic && isVirtual)
        throw spi::RuntimeError("Class method cannot be static and virtual");
    if (isConst && isStatic)
        throw spi::RuntimeError("Class method cannot be static and const");
    if (isImplementation && isStatic)
        throw spi::RuntimeError("Class method cannot be implementation and static");
    if (isImplementation && isVirtual)
        throw spi::RuntimeError("Class method cannot be implementation and virtual");
    if (isDelegate && isStatic)
        throw spi::RuntimeError("Class method cannot be a delegate and static");
}

void ClassMethod::declare(
    GeneratedOutput& ostr,
    const DataTypeConstSP& classType,
    const std::string& className,
    bool types,
    const ServiceDefinitionSP& svc,
    bool alwaysVirtual,
    bool innerContext) const
{
    if (types && !m_function->neededByTypesLibrary())
        return;

    writeStartCommentBlock(ostr, true, 4);
    if (m_function->m_description.size() == 0)
        ostr << "    * No description.\n";
    else
        writeComments(ostr, m_function->m_description, 0, 4);
    if (m_function->m_inputs.size() > 0)
        ostr << "    *\n";
    for (size_t i = 0; i < m_function->m_inputs.size(); ++i)
    {
        ostr << "    * @param " << m_function->m_inputs[i]->name() << "\n";
        writeComments(ostr, m_function->m_inputs[i]->description(), 4, 4);
    }
    if (m_function->m_returnType && m_function->m_returnTypeDescription.size() > 0)
    {
        ostr << "    *\n"
             << "    * @returns\n";
        writeComments(ostr, m_function->m_returnTypeDescription, 4, 4);
    }

    writeEndCommentBlock(ostr, 4);

    ostr << "    ";
    if (m_isStatic)
        ostr << "static ";
    else if (m_isVirtual || alwaysVirtual)
        ostr << "virtual ";
    if (!m_function->m_returnType)
        ostr << "void";
    else if (innerContext)
    {
        if (m_function->m_returnArrayDim > 0)
            ostr << m_function->m_returnType->innerArrayType(m_function->m_returnArrayDim);
        else
            ostr << m_function->m_returnType->innerValueType();
    }
    else if (m_function->m_returnArrayDim > 0)
        ostr << m_function->m_returnType->outerArrayType(m_function->m_returnArrayDim);
    else
        ostr << m_function->m_returnType->outerValueType();
    ostr << " " << m_function->m_name;
    writeFunctionArgs(ostr, innerContext, m_function->m_inputs, 
        m_function->m_outputs, true, 8);
    // since we declare all classes as ConstSP when you Make them it follows
    // that all non-static functions must be declared as const
    if (!m_isStatic)
        ostr << " const";
    if (m_isVirtual && !m_function->m_implementation)
        ostr << " = 0";
    ostr << ";\n";

    if (m_function->m_outputs.size() > 0 && !m_function->m_returnType)
    {
        ostr << "\n";

        ostr << "    " << tupleOutput(m_function->m_outputs, innerContext)
            << " " << m_function->m_name;
        writeFunctionArgs(ostr, innerContext, m_function->m_inputs, 
            std::vector<AttributeConstSP>(), true, 8);
        // since we declare all classes as ConstSP when you Make them it follows
        // that all non-static functions must be declared as const
        if (!m_isStatic)
            ostr << " const";
        if (m_isVirtual && !m_function->m_implementation)
            ostr << " = 0";
        ostr << ";\n";
    }
}

void ClassMethod::declareHelper(
    GeneratedOutput& ostr,
    const DataTypeConstSP& classType,
    const std::string& className,
    bool types,
    const ServiceDefinitionSP& svc) const
{
    if (types && !m_function->neededByTypesLibrary())
        return;

    //bool noFunctor = function->noLog;

    // we put the definition into the regular stream
    // we put the Functor into the helper stream
    const std::vector<AttributeConstSP> inputs = m_isStatic ?
        m_function->m_inputs :
        getExtendedArgs(classType, m_function->m_inputs);

    // the method is always declared
    // it uses the extended arguments in the inner context (depends on noConvert)
    ostr << "    ";
    ostr << "static ";

    if (!m_function->m_returnType)
        ostr << "void";
    else if (m_function->m_returnArrayDim > 0)
        if (m_function->m_noConvert)
            ostr << m_function->m_returnType->outerArrayType(m_function->m_returnArrayDim);
        else
            ostr << m_function->m_returnType->innerArrayType(m_function->m_returnArrayDim);
    else
        if (m_function->m_noConvert)
            ostr << m_function->m_returnType->outerValueType();
        else
            ostr << m_function->m_returnType->innerValueType();
    ostr << " " << m_function->m_name;
    writeFunctionArgs(ostr, !m_function->m_noConvert, inputs, m_function->m_outputs, false, 8);
    ostr << ";\n";
}

void ClassMethod::implementAsDelegate(
    GeneratedOutput& ostr,
    const WrapperClass* wrapperClass,
    const ServiceDefinitionSP& svc) const
{
    // inline representation
    // going backward from outer types -> inner types
    // calling method of the same name in the inner class
    // converting the inner type back to outer type
    //
    // for the moment we only support functions with an output return type

    bool noReturnType = !m_function->m_returnType;
    if (noReturnType)
        SPI_THROW_RUNTIME_ERROR("Cannot handle void delegate methods yet");

    if (m_isStatic)
        SPI_THROW_RUNTIME_ERROR("Static functions cannot be delegates");

    ostr << "\n"
        << "    ";

    if (!m_function->m_returnType)
        ostr << "void"; // in case we can handle this state later
    else if (m_function->m_returnArrayDim)
        ostr << m_function->m_returnType->innerArrayType(m_function->m_returnArrayDim);
    else
        ostr << m_function->m_returnType->innerValueType();
    ostr << " " << m_function->m_name;
    writeFunctionArgs(ostr, true, m_function->m_inputs, m_function->m_outputs, false, 8);
    ostr << " const\n"
        << "    {\n";

    size_t indent = 8;
    std::string indentString(8, ' ');
    size_t N = m_function->m_inputs.size();

    writeDeclareArgs(ostr, false, m_function->m_inputs, indent, "o_");

    if (N > 0)
        ostr << "\n";

    std::vector<std::string> argNames;

    for (size_t i = 0; i < N; ++i)
    {
        const AttributeConstSP& arg = m_function->m_inputs[i];
        const std::string& argName = arg->name();
        const DataTypeConstSP& dataType = arg->dataType();
        argNames.push_back("o_" + argName);
        if (!dataType->needsTranslation())
        {
            ostr << indentString << "o_" << argName << " = " << argName;
            continue;
        }

        int arrayDim = arg->arrayDim();

        switch (arrayDim)
        {
        case 2:
        {
            std::string matrixElement = argName + "[i_][j_]";
            std::string translation = dataType->translateInner(matrixElement);
            ostr << indentString << "{\n"
                << indentString << "  size_t nr_ = " << argName << ".Rows();\n"
                << indentString << "  size_t nc_ = " << argName << ".Cols();\n"
                << indentString << "  o_" << argName << ".Resize(nr_, nc_);\n"
                << indentString << "  for (size_t i_ = 0; i_ < nr_; ++i_)\n"
                << indentString << "  {\n"
                << indentString << "    for (size_t j_ = 0; j_ < nc_; ++j_)\n"
                << indentString << "    {\n"
                << indentString << "      " << argName << "[i_][j_] = " << translation << ";\n"
                << indentString << "    }\n"
                << indentString << "  }\n"
                << indentString << "}\n";
        }
        break;
        case 1:
        {
            std::string arrayElement = argName + "[i_]";
            std::string translation = dataType->translateInner(arrayElement);
            ostr << indentString << "{\n"
                << indentString << " size_t n_ = " << argName << ".size();\n"
                << indentString << "o_" << argName << ".reserve(n_);\n"
                << indentString << "for (size_t i_ = 0; i_ < n_; ++i_)\n"
                << indentString << "    o_" << argName << ".push_back(" << translation << ");\n"
                << indentString << "}\n";
        }
        break;
        case 0:
            ostr << indentString << "o_" << argName << " = " << dataType->translateInner(argName) << ";\n";
            break;
        }
    }

    std::string caller = "self->" + m_function->m_name;
    std::string output;

    if (!m_function->m_returnType)
    {
        ostr << indentString << caller;
        writeArgsCall(ostr, argNames, caller.length() + 4, indent, 78);
        ostr << ";\n";
    }
    else if (m_function->m_returnArrayDim > 0)
    {
        int returnArrayDim = m_function->m_returnArrayDim;
        std::string oDecl = m_function->m_returnType->outerArrayType(returnArrayDim);
        std::string iDecl = m_function->m_returnType->innerArrayType(returnArrayDim);
        if (m_function->m_returnType->needsTranslation())
        {
            ostr << indentString << "const " << oDecl << "& o_result = " << caller;
            writeArgsCall(ostr, argNames, caller.length() + 4, indent, 78);
            ostr << ";\n";
            switch (returnArrayDim)
            {
            case 2:
                ostr << indentString << iDecl << " i_result;\n"
                    << indentString << "    {\n"
                    << indentString << "      size_t nr = o_result.Rows();\n"
                    << indentString << "      size_t nc = o_result.Cols();\n"
                    << indentString << "      i_result.Resize(nr, nc);\n"
                    << indentString << "      for (size_t i = 0; i < nr; ++i)\n"
                    << indentString << "        for (size_t j = 0; j < nc; ++j)\n"
                    << indentString << "          i_result[i][j] = "
                    << indentString << m_function->m_returnType->translateOuter("o_result[i][j]") << ";\n"
                    << "    }\n";
                break;
            case 1:
                ostr << indentString << iDecl << " i_result;\n"
                    << indentString << "    {\n"
                    << indentString << "        size_t n_ = o_result.size();\n"
                    << indentString << "        for (size_t i_ = 0; i_ < n_; ++i_)\n"
                    << indentString << "            o_result.push_back("
                    << indentString << m_function->m_returnType->translateOuter("o_result[i_]") << ");\n";
            }
        }
        else
        {
            ostr << indentString << iDecl << " i_result = " << caller;
            writeArgsCall(ostr, argNames, caller.length() + 4, indent, 78);
            ostr << ";\n";
        }
        ostr << indentString << "return i_result;\n";
    }
    else
    {
        std::string idecl = m_function->m_returnType->innerValueType();

        if (m_function->m_returnType->needsTranslation())
        {
            // because we are setting the value via the return from a function
            // we can use reference types here rather than value types

            std::string decl = m_function->m_returnType->outerReferenceType();

            if (!spi::StringStartsWith(decl, "const "))
            {
                // the claim above is only true if the reference type is const
                decl = m_function->m_returnType->outerValueType();
            }

            ostr << indentString << decl << " o_result = " << caller;
            writeArgsCall(ostr, argNames, caller.length() + 4, indent, 78);
            ostr << ";\n";

            ostr << indentString << idecl << " i_result = "
                << m_function->m_returnType->translateOuter("o_result") << ";\n";

        }
        else
        {
            ostr << indentString << idecl << " i_result = " << caller;
            writeArgsCall(ostr, argNames, caller.length() + 4, indent, 78);
            ostr << ";\n";
        }
        ostr << indentString << "return i_result;\n";
    }
    ostr << "    }\n";
}

void ClassMethod::implement(
    GeneratedOutput& ostr,
    const DataTypeConstSP& classType,
    const std::string& className,
    const std::string& innerClassName,
    bool types,
    const ServiceDefinitionSP& svc,
    bool noHelper) const
{
    // we put everything needed to run the function into the regular stream
    // we put everything else into the helper stream

    // we are implementing three things here
    //
    // 1. the declared method inside the class (with external context)
    // 2. the declared method inside the helper class (with internal context)
    // 3. the functor call_func method
    //
    // For virtual base classes only (3) is defined
    // For classes without any inheritance we need (1), (2), (3)
    // For classes which are implementing a base class then we don't need (3)

    if (types && !m_function->neededByTypesLibrary())
        return;

    //bool noFunctor    = function->noLog;
    bool noReturnType = !m_function->m_returnType;
    bool isPrivateConstructor = noReturnType ?
        false :
        m_function->m_returnType->isClosed() && m_function->m_returnArrayDim == 0;

    if (noHelper)
    {
        // all we need to do is write the declaration and implementation
        // no functors, no 4-levels of indirection etc...
        if (!m_function->m_implementation)
        {
            throw spi::RuntimeError("No implementation provided for %s::%s",
                className.c_str(), m_function->m_name.c_str());
        }

        implementDeclaration(ostr, className);

        ostr << "{";
        ostr << "    " << svc->getName() << "_check_permission();\n";
        ostr << "    SPI_PROFILE(\"" << svc->getNamespace() << "." << classType->name()
            << "." << m_function->fullName() << "\");\n";

        if (sessionLogging())
        {
            ostr << "    SPI_SESSION_LOG(\"" << svc->getNamespace() << "." << classType->name()
                << "." << m_function->fullName() << "\");\n";
        }

        if (!m_isConst)
        {
            ostr << "    clear_public_map();\n";
        }
        writeVerbatim(ostr, m_function->m_implementation);
        return;
    }

    const std::vector<AttributeConstSP> inputs = m_isStatic ?
        m_function->m_inputs :
        getExtendedArgs(classType, m_function->m_inputs);

    bool isWrapperClass = !innerClassName.empty();
    if (!m_isVirtual || m_function->m_implementation)
    {
        implementDeclaration(ostr, className);

        ostr << "{\n";
        ostr << "  bool isLogging = " << svc->getName() << "_begin_function();\n"
            << "  SPI_PROFILE(\"" << svc->getNamespace() << "." << classType->name()
            << "." << m_function->fullName() << "\");\n";

        if (sessionLogging())
        {
            ostr << "  SPI_SESSION_LOG(\"" << svc->getNamespace() << "." << classType->name()
                << "." << m_function->fullName() << "\");\n";
        }

        ostr << "  try\n"
             << "  {\n";

        if (!m_isStatic)
        {
            writeDeclareArgs(ostr, false,
                             std::vector<AttributeConstSP>(1, inputs[0]),
                             4);
        }

        if (isPrivateConstructor)
        {
            writeFunctionConstructor(
                ostr, "_constructor", svc->getName(),
                m_function->m_ns, className, m_function->m_name, inputs);

            if (!m_function->m_noLog)
            {
                ostr << "\n"
                     << "    if (isLogging)\n"
                     << "        " << svc->getName()
                     << "_service()->log_inputs(_constructor);\n";
            }
            ostr << "\n";
        }
        else if (!m_function->m_noLog)
        {
            ostr << "    if (isLogging)\n"
                 << "    {\n";

            writeFunctionConstructor(
                ostr, "_func", svc->getName(), m_function->m_ns,
                className, m_function->m_name, inputs, 4);

            ostr << "        " << svc->getName()
                 << "_service()->log_inputs(_func);\n"
                 << "    }\n\n";
        }

        //if (!isStatic)
        //    ostr << "\n"; // we will inevitably convert self

        if (!m_function->m_noConvert)
        {
            writeInnerDeclarationsAndTranslations(ostr, inputs, "i_");
            writeDeclareArgs(ostr, true, m_function->m_outputs, 4, "", false,
                "i_", true);
        }

        std::string caller = implementHelperCaller(ostr, className);
        std::string output = writeCallToInnerFunction(
            ostr, m_function->m_returnType, m_function->m_returnArrayDim,
            m_function->m_noConvert, inputs, m_function->m_outputs, caller, false);

        if (!m_function->m_noLog)
        {
            writeFunctionOutputLogging(
                ostr, svc->getName(),
                m_function->m_returnType, m_function->m_returnArrayDim,
                output, m_function->m_outputs);
        }

        if (!m_isConst && !m_isStatic)
        {
            ostr << "    clear_public_map();\n";
        }

         ostr << "\n"
              << "    " << svc->getName() << "_end_function();\n";

        if (!output.empty())
        {
            ostr << "\n"
                    << "    return " << output << ";\n";
        }

        ostr << "  }\n";
        writeFunctionCatchBlock(ostr, svc->getName(), m_function->m_name, className);

        ostr << "}\n";

        if (m_function->m_outputs.size() > 0 && !m_function->m_returnType)
        {
            ostr << "\n";
            ostr << tupleOutput(m_function->m_outputs) << " " << className << "::" << m_function->m_name;
            writeFunctionArgs(ostr, false, m_function->m_inputs, std::vector<AttributeConstSP>(), false, 4);
            if (!m_isStatic)
                ostr << " const";
            ostr << "\n"
                << "{\n";

            // in cases the function name clashes with an argument name ensure that we call with qualifications
            std::string funcName = m_function->m_name;
            if (m_isStatic)
            {
                funcName = className + "::" + funcName;
            }
            else
            {
                funcName = "this->" + funcName;
            }
            writeCallFromTupleOutputFunction(ostr, m_function->m_inputs, m_function->m_outputs, funcName);

            ostr << "}\n";
        }

        /*
        *****************************************************************
        * The declared method in the helper class
        * This is in the inner context unless we specify noConvert
        *****************************************************************
        */
        ostr << "\n";
        if (noReturnType)
            ostr << "void";
        else if (m_function->m_returnArrayDim)
            if (m_function->m_noConvert)
                ostr << m_function->m_returnType->outerArrayType(m_function->m_returnArrayDim);
            else
                ostr << m_function->m_returnType->innerArrayType(m_function->m_returnArrayDim);
        else
            if (m_function->m_noConvert)
                ostr << m_function->m_returnType->outerValueType();
            else
                ostr << m_function->m_returnType->innerValueType();
        ostr << " " << className << "_Helper::" << m_function->m_name;

        std::vector<AttributeConstSP> helperInputs(inputs);
        if (!isWrapperClass && !m_isStatic)
            helperInputs[0] = helperInputs[0]->rename("in_self");
        writeFunctionArgs(ostr, !m_function->m_noConvert, helperInputs, m_function->m_outputs, false, 4);
        //if (isConst)
        //    ostr << " const";
        ostr << "\n"
             << "{\n";

        if (!isWrapperClass && !m_isStatic)
        {
            // if it is not a wrapper class, then the first argument is
            // a pointer to the outer class and needs to be handled with
            // care
            //
            // if it is a wrapper class, then the first argument is a
            // pointer to the inner class and is perfectly good
            ostr << "    const " << className << "* self = in_self.get();\n";
        }
        if (m_function->m_implementation)
        {
            writeVerbatim(ostr, m_function->m_implementation);
        }
        else if (isWrapperClass)
        {
            size_t startPos = 4;
            ostr << "    ";

            if (m_function->m_returnType)
            {
                ostr << "return ";
                startPos += 7;
            }

            if (m_isStatic)
            {
                startPos += innerClassName.length() + 2;
                ostr << innerClassName << "::";
            }
            else if (m_byValue)
            {
                startPos += 5;
                ostr << "self.";
            }
            else
            {
                startPos += 6;
                ostr << "self->";
            }
            startPos += m_function->m_name.length();
            ostr << m_function->m_name;

            writeArgsCall(ostr, true, m_function->m_inputs, startPos, 8);
            ostr << ";\n"
                 << "}\n";
        }
        else
        {
            throw spi::RuntimeError(
                "Must provide implementation for non-wrapper class");
        }
    }
}

void ClassMethod::implementDeclaration(
    GeneratedOutput& ostr,
    const std::string& className,
    bool innerContext) const
{
    /*
    *****************************************************************
    * The declared method in the regular class (outer context)
    ****************************************************************
    */
    if (m_function->m_description.size() > 0)
    {
        writeStartCommentBlock(ostr, false);
        writeComments(ostr, m_function->m_description);
        writeEndCommentBlock(ostr);
    }

    ostr << "\n";
    if (!m_function->m_returnType)
        ostr << "void";
    else if (innerContext)
    {
        if (m_function->m_returnArrayDim)
            ostr << m_function->m_returnType->innerArrayType(m_function->m_returnArrayDim);
        else
            ostr << m_function->m_returnType->innerValueType();
    }
    else if (m_function->m_returnArrayDim)
        ostr << m_function->m_returnType->outerArrayType(m_function->m_returnArrayDim);
    else
        ostr << m_function->m_returnType->outerValueType();

    ostr << " " << className << "::" << m_function->m_name;
    writeFunctionArgs(ostr, innerContext, m_function->m_inputs, 
        m_function->m_outputs, false, 4);
    if (!m_isStatic) // see classMethod::declare for logic behind this decision
        ostr << " const";
    ostr << "\n";
}

void ClassMethod::implementHelper(
    GeneratedOutput& ostr,
    const DataTypeConstSP& classType,
    const std::string& className,
    bool types,
    const ServiceDefinitionSP& svc) const
{
    // we put everything needed to run the function into the regular stream
    // we put everything else into the helper stream

    if (m_isImplementation)
        return;

    if (types && !m_function->neededByTypesLibrary())
        return;

    const std::vector<AttributeConstSP> inputs = m_isStatic ?
        m_function->m_inputs :
        getExtendedArgs(classType, m_function->m_inputs);

    DataTypeConstSP instanceType;
    if (!m_isStatic)
    {
        instanceType = classType;
    }

    writeFunctionCaller(
        ostr, m_function->m_ns, className,
        m_function->m_name, m_function->m_returnType,
        m_function->m_returnArrayDim, instanceType, svc,
        m_function->m_inputs, m_function->m_outputs);

    writeFunctionObjectType(
        ostr, m_function->m_ns, m_function->m_name, svc->getNamespace(), className);

}

void ClassMethod::implementRegistration(
    GeneratedOutput& ostr,
    const std::string& className,
    const char* serviceName,
    bool types) const
{
    if (m_isImplementation)
        return;

    if (types && !m_function->neededByTypesLibrary())
        return;

    //if (!function->noLog)
    //{
    //    std::string functorClass = spi::StringFormat("%s_Helper::Func_%s",
    //                                                 className.c_str(),
    //                                                 function->name.c_str());

    //    ostr << "    " << serviceName << "->add_object_type(&" << functorClass
    //         << "::object_type);\n";
    //}


    ostr << "    " << serviceName << "->add_object_type(&"
         << className << "_" << m_function->m_name
         << "_FunctionObjectType);\n"
         << "    " << serviceName << "->add_function_caller(&"
         << className << "_" << m_function->m_name
         << "_FunctionCaller);\n";

    //ostr << "    " << serviceName << "->add_function_caller(&" << className
    //     << "_" << function->name << "_FunctionCaller);\n";


}

std::string ClassMethod::implementHelperCaller(
    GeneratedOutput& ostr,
    const std::string& className) const
{
    std::string caller = spi::StringFormat(
        "%s_Helper::%s", className.c_str(), m_function->m_name.c_str());

    return caller;
}

spdoc::ClassMethodConstSP ClassMethod::getDoc() const
{
    m_function->getDoc(); // ensures it is generated

    return spdoc::ClassMethod::Make(
        m_function->m_doc,
        m_isConst, m_isVirtual, m_isStatic, m_isImplementation, m_implements);
}

