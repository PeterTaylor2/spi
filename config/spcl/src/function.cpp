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
** function.cpp
***************************************************************************
** Implements the Function class.
***************************************************************************
*/

#include "function.hpp"

#include <spi/RuntimeError.hpp>
#include <spi/Service.hpp>
#include <spi/StringUtil.hpp>
#include <spi/spdoc_configTypes.hpp>

#include "classMethod.hpp"
#include "constant.hpp"
#include "innerClass.hpp"
#include "serviceDefinition.hpp"
#include "verbatim.hpp"
#include "generatorTools.hpp"


/*
***************************************************************************
** Implementation of Function
***************************************************************************
*/
FunctionConstSP Function::Make(
    const std::vector<std::string>&      description,
    const std::vector<std::string>&      returnTypeDescription,
    const DataTypeConstSP&               returnType,
    int                                  returnArrayDim,
    const std::string&                   name,
    const std::string&                   ns,
    const std::vector<FunctionAttributeConstSP>& args,
    const VerbatimConstSP&               implementation,
    bool                                 noLog,
    bool                                 noConvert,
    const std::vector<std::string>&      excelOptions,
    int                                  cacheSize,
    bool                                 optionalReturnType)
{
    return new Function(description, returnTypeDescription, returnType,
        returnArrayDim, name, ns, args,
        implementation, noLog, noConvert, excelOptions, cacheSize,
        optionalReturnType);
}

Function::Function(
    const std::vector<std::string>& description,
    const std::vector<std::string>& returnTypeDescription,
    const DataTypeConstSP& returnType,
    int                                  returnArrayDim,
    const std::string& name,
    const std::string& ns,
    const std::vector<FunctionAttributeConstSP>& args,
    const VerbatimConstSP& implementation,
    bool                                 noLog,
    bool                                 noConvert,
    const std::vector<std::string>& excelOptions,
    int                                  cacheSize,
    bool                                 optionalReturnType)
    :
    m_description(description),
    m_returnTypeDescription(returnTypeDescription),
    m_returnType(returnType),
    m_returnArrayDim(returnArrayDim),
    m_name(name),
    m_ns(ns),
    m_inputs(),
    m_outputs(),
    m_implementation(implementation),
    m_noLog(noLog),
    m_noConvert(noConvert),
    m_excelOptions(),
    m_cacheSize(cacheSize),
    m_hasIgnored(),
    m_optionalReturnType(optionalReturnType)
{
    SPI_PRE_CONDITION(cacheSize >= 0);

    // inputs at beginning, outputs at end, never the twain shall mix
    if (returnType && returnType->ignored())
        m_hasIgnored = true;

    for (size_t i = 0; i < args.size(); ++i)
    {
        FunctionAttributeConstSP arg = args[i];
        if (arg->attribute()->dataType()->ignored())
            m_hasIgnored = true;

        bool isOutput = args[i]->isOutput();
        if (isOutput)
        {
            m_outputs.push_back(args[i]->attribute());
        }
        else
        {
            if (m_outputs.size() > 0)
                throw spi::RuntimeError("Cannot specify input after an output");
            m_inputs.push_back(args[i]->attribute());
        }
    }

    if (m_outputs.size() > 0 && returnType)
    {
        throw spi::RuntimeError(
            "returnType must be void if outputs are defined");
    }

    for (size_t i = 0; i < excelOptions.size(); ++i)
    {
        const std::string& excelOption = excelOptions[i];

        if (excelOption.empty())
            continue;

        if (excelOption != "volatile" &&
            excelOption != "hidden" &&
            excelOption != "slow")
        {
            throw spi::RuntimeError(
                "Unknown excelOption '%s'", excelOption.c_str());
        }

        m_excelOptions.push_back(excelOption);
    }

    if (m_optionalReturnType)
    {
        if (!m_returnType)
        {
            SPI_THROW_RUNTIME_ERROR("Cannot return optional void!");
        }
        switch (m_returnType->publicType())
        {
        case spdoc::PublicType::MAP:
        case spdoc::PublicType::OBJECT:
            break; // always allowed

        case spdoc::PublicType::CLASS:
            if (m_returnType->innerByValue())
            {
                SPI_THROW_RUNTIME_ERROR("Function cannot return optional type for " <<
                    m_returnType->name());
            }
            break; // otherwise allowed

        default:
            SPI_THROW_RUNTIME_ERROR("Function cannot return optional type for " <<
                m_returnType->publicType().to_string());
        }
    }
}

const std::string Function::fullName() const
{
    if (!m_ns.empty())
        return m_ns + "." + m_name;
    return m_name;
}

AttributeConstSP Function::returns() const
{
    if (!m_returnType)
        return AttributeConstSP();
    return Attribute::ReturnType(
        m_returnTypeDescription,
        m_returnType,
        m_returnArrayDim,
        m_optionalReturnType);
}

void Function::declare(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    // we put the definition into the regular stream
    // we put the Functor into the helper stream

    if (types && !neededByTypesLibrary())
        return;

    writeStartCommentBlock(ostr, true);
    if (m_description.size() == 0)
        ostr << "* No description.\n";
    else
        writeComments(ostr, m_description);
    if (m_inputs.size() > 0)
        ostr << "*\n";
    for (size_t i = 0; i < m_inputs.size(); ++i)
    {
        ostr << "* @param " << m_inputs[i]->name() << "\n";
        writeComments(ostr, m_inputs[i]->description(), 4);
    }
    if (m_outputs.size() > 0)
        ostr << "*\n";
    for (size_t i = 0; i < m_outputs.size(); ++i)
    {
        ostr << "* @output " << m_outputs[i]->name() << "\n";
        writeComments(ostr, m_outputs[i]->description(), 4);
    }
    if (m_returnType && m_returnTypeDescription.size() > 0)
    {
        ostr << "*\n"
             << "* @returns\n";
        writeComments(ostr, m_returnTypeDescription, 4);
    }

    writeEndCommentBlock(ostr);

    ostr << svc->getImport() << "\n";
    if (!m_returnType)
        ostr << "void";
    else if (m_returnArrayDim > 0)
        ostr << m_returnType->outerArrayType(m_returnArrayDim);
    else
        ostr << m_returnType->outerValueType();
    ostr << " " << m_name;
    writeFunctionArgs(ostr, false, m_inputs, m_outputs, true, 4);
    ostr << ";\n";

    if (m_outputs.size() > 0 && !m_returnType)
    {
        ostr << "\n"
            << svc->getImport() << "\n";

        ostr << tupleOutput(m_outputs) << " " << m_name;
        writeFunctionArgs(ostr, false, m_inputs, std::vector<AttributeConstSP>(), true, 4);
        ostr << ";\n";
    }
}

bool Function::declareInClasses() const
{
    return false;
}

void Function::declareHelper(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    // we put the definition into the regular stream
    // we put the Functor into the helper stream

    if (types && !neededByTypesLibrary())
        return;

    ostr << "\n";
    if (!m_returnType)
        ostr << "void";
    else if (m_returnArrayDim > 0)
        if (m_noConvert)
            ostr << m_returnType->outerArrayType(m_returnArrayDim);
        else
            ostr << m_returnType->innerArrayType(m_returnArrayDim);
    else
        if (m_noConvert)
            ostr << m_returnType->outerValueType();
        else
            ostr << m_returnType->innerValueType();
    ostr << " " << m_name << "_Helper";
    writeFunctionArgs(ostr, !m_noConvert, m_inputs, m_outputs, false, 4);
    ostr << ";\n";
}

void Function::implement(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    // we put everything needed to run the function into the regular stream
    // we put everything else into the helper stream

    if (types && !neededByTypesLibrary())
        return;

    //bool noFunctor = noLog;
    bool noReturnType = !m_returnType;
    bool isPrivateConstructor = false;

    if (m_description.size() > 0)
    {
        writeStartCommentBlock(ostr, false);
        writeComments(ostr, m_description);
        writeEndCommentBlock(ostr);
    }
    else
    {
        ostr << "\n";
    }

    // write the function
    //
    // the function is responsible for argument conversion and then calls
    // the corresponding helper function which uses the inner arguments
    // as inputs instead

    std::string cppReturnType;
    if (noReturnType)
    {
        ostr << "void";
    }
    else if (m_returnArrayDim > 0)
    {
        if (m_returnType->isClosed())
            SPI_THROW_RUNTIME_ERROR("Cannot return array of closed type "
                << m_returnType->name());

        cppReturnType = m_returnType->outerArrayType(m_returnArrayDim);
        ostr << cppReturnType;
    }
    else
    {
        cppReturnType = m_returnType->outerValueType();
        ostr << cppReturnType;
        if (m_returnType->isClosed())
            isPrivateConstructor = true;
    }
    ostr << " " << m_name;
    writeFunctionArgs(ostr, false, m_inputs, m_outputs, false, 4);

    ostr << "\n"
        << "{\n"
        << "  SPI_PROFILE(\"" << svc->getNamespace() << "." << fullName() << "\");\n";

    if (sessionLogging())
    {
        ostr << "  SPI_SESSION_LOG(\"" << svc->getNamespace() << "." << fullName() << "\");\n";
    }

    if (m_cacheSize > 0 && !isPrivateConstructor && !noReturnType)
    {
        ostr << "  static spi_util::LRUCache<std::string, " << cppReturnType
            << " > cache(" << m_cacheSize << ");\n\n";
        writeFunctionConstructor(
            ostr, "_func", svc->getName(), m_ns, "", m_name, m_inputs, -2);
        ostr << "\n"
            << "  std::string _funcHash = spi::SHA1(_func);\n"
            << "  {\n"
            << "    " << cppReturnType << " o_result;\n"
            << "    if (cache.find(_funcHash, o_result))\n"
            << "      return o_result;\n"
            << "  }\n"
            << "\n";
    }
    ostr << "  bool isLogging = " << svc->getName() << "_begin_function(";
    if (m_noLog)
        ostr << "true";
    ostr << ");\n"
         << "  try\n"
         << "  {\n";

    // we might want to create a Function object if we are logging
    // or if the Function object is required in the serialisation
    if (isPrivateConstructor)
    {
        writeFunctionConstructor(
            ostr, "_constructor", svc->getName(), m_ns, "", m_name, m_inputs);

        if (!m_noLog)
        {
            ostr << "\n"
                 << "    if (isLogging)\n"
                 << "        " << svc->getName()
                 << "_service()->log_inputs(_constructor);\n";
        }
        ostr << "\n";
    }
    else if (!m_noLog)
    {
        ostr << "    if (isLogging)\n"
             << "    {\n";

        if (m_cacheSize <= 0 || !m_returnType)
        {
            writeFunctionConstructor(
                ostr, "_func", svc->getName(), m_ns, "", m_name, m_inputs, 4);
        }

        ostr << "        " << svc->getName()
             << "_service()->log_inputs(_func);\n"
             << "    }\n\n";
    }

    if (!m_noConvert)
    {
        writeInnerDeclarationsAndTranslations(ostr, m_inputs, "i_");
        writeDeclareArgs(ostr, true, m_outputs, 4, "", false, "i_", true);
    }

    std::string caller = m_name + "_Helper";
    std::string output = writeCallToInnerFunction(
        ostr, returns(), m_noConvert,
        m_inputs, m_outputs, caller, false);

    if (!m_noLog)
    {
        writeFunctionOutputLogging(
            ostr, svc->getName(),
            m_returnType, m_returnArrayDim, output,
            m_outputs);
    }

    ostr << "\n"
         << "    " << svc->getName() << "_end_function();\n";

    if (!output.empty())
    {
        if (m_cacheSize > 0 && !isPrivateConstructor && !noReturnType)
        {
            ostr << "\n"
                << "    cache.insert(_funcHash, " << output << ");\n";
        }
        ostr << "\n"
             << "    return " << output << ";\n";


    }

    ostr << "  }\n";
    writeFunctionCatchBlock(ostr, svc->getName(), m_name);
    ostr << "}\n";

    if (m_outputs.size() > 0 && !m_returnType)
    {
        ostr << "\n";
        ostr << tupleOutput(m_outputs) << " " << m_name;
        writeFunctionArgs(ostr, false, m_inputs, std::vector<AttributeConstSP>(), false, 4);
        ostr << "\n"
            << "{\n";

        writeCallFromTupleOutputFunction(ostr, m_inputs, m_outputs, m_name);

        ostr << "}\n";
    }

    // write the helper function
    //
    // this takes the inner type inputs and simply uses the verbatim code
    // for the function as the implemention
    ostr << "\n";
    if (noReturnType)
        ostr << "void";
    else if (m_returnArrayDim > 0)
        if (m_noConvert)
            ostr << m_returnType->outerArrayType(m_returnArrayDim);
        else
            ostr << m_returnType->innerArrayType(m_returnArrayDim);
    else
        if (m_noConvert)
            ostr << m_returnType->outerValueType();
        else
            ostr << m_returnType->innerValueType();
    ostr << " " << m_name << "_Helper";
    writeFunctionArgs(ostr, !m_noConvert, m_inputs, m_outputs, false, 4);
    ostr << "\n"
         << "{";

    writeVerbatim(ostr, m_implementation);
}

void Function::implementHelper(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    // we put everything needed to run the function into the regular stream
    // we put everything else into the helper stream

    if (types && !neededByTypesLibrary())
        return;

    bool noFunctor = m_noLog;

    writeFunctionCaller(
        ostr, m_ns, std::string(), m_name, m_returnType,
        m_returnArrayDim, DataTypeConstSP(),
        svc, m_inputs, m_outputs);

    writeFunctionObjectType(
        ostr, m_ns, m_name, svc->getNamespace());
}

bool Function::neededByTypesLibrary() const
{
    if (!m_returnType)
        return false;

    return m_returnType->isClosed();
}

void Function::implementRegistration(
    GeneratedOutput& ostr,
    const char* serviceName,
    bool types) const
{
    if (types && !neededByTypesLibrary())
        return;

    ostr << "    " << serviceName << "->add_object_type(&" << m_name
         << "_FunctionObjectType);\n"
         << "    " << serviceName << "->add_function_caller(&" << m_name
         << "_FunctionCaller);\n";
}

const char* Function::type() const
{
    return "FUNCTION";
}

spdoc::ConstructConstSP Function::getDoc() const
{
    if (!m_doc)
    {
        std::vector<spdoc::AttributeConstSP> docInputs;
        std::vector<spdoc::AttributeConstSP> docOutputs;
        for (size_t i = 0; i < m_inputs.size(); ++i)
            docInputs.push_back(m_inputs[i]->getDoc());
        for (size_t i = 0; i < m_outputs.size(); ++i)
            docOutputs.push_back(m_outputs[i]->getDoc());

        m_doc = spdoc::Function::Make(
            m_name,
            m_description,
            m_returnTypeDescription,
            m_returnType ? m_returnType->getDoc() : spdoc::DataTypeConstSP(),
            m_returnArrayDim,
            docInputs,
            docOutputs,
            m_excelOptions,
            m_optionalReturnType);
    }
    return m_doc;
}

