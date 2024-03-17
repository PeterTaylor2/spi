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
** construct.cpp
***************************************************************************
** Implements the Construct classes used in the configuration parser.
***************************************************************************
*/

#include "construct.hpp"

#include <spi/RuntimeError.hpp>
#include <spi/Service.hpp>
#include <spi/StringUtil.hpp>
#include <spi/spdoc_configTypes.hpp>

#include "classMethod.hpp"
#include "constant.hpp"
#include "innerClass.hpp"
#include "moduleDefinition.hpp"
#include "serviceDefinition.hpp"
#include "verbatim.hpp"
#include "generatorTools.hpp"
#include "function.hpp"

/*
***************************************************************************
** Implementation of Construct
***************************************************************************
*/
Construct::Construct()
{}

void Construct::declareInner(GeneratedOutput& ostr,
                             NamespaceManager& nsman,
                             const ServiceDefinitionSP& svc) const
{}

int Construct::preDeclare(GeneratedOutput& ostr,
                           const ServiceDefinitionSP& svc) const
{
    return 0;
}

void Construct::declareClassFunctions(GeneratedOutput& ostr, const ServiceDefinitionSP& svc) const
{
    // null operator except for classes
}

void Construct::declareTypeConversions(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc) const
{}

void Construct::writeInnerHeader(
    GeneratedOutput& ostr) const
{}

void Construct::writeInnerPreDeclaration(
    GeneratedOutput& ostr,
    NamespaceManager& nsm) const
{}

void Construct::SplitTypedef(const std::string& td,
    std::string& td1,
    std::string& td2)
{
    size_t pos = td.find_first_of(':');
    if (pos != std::string::npos)
    {
        td1 = spi::StringStrip(td.substr(0, pos));
        td2 = std::string(" ") + td.substr(pos); // includes :
    }
    else
    {
        td1 = td;
        td2 = "";
    }
}

// declares a stand-alone function which uses all the attributes of the class
// and returns a shared pointer for that class
void Class::declareConstructor(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    const std::string& className,
    const std::string& constructor,
    const std::vector<AttributeConstSP>& attributes,
    bool returnByValue) const
{
    if (constructor.empty())
        return;

    std::vector<std::string> description;
    description.push_back(spi_util::StringFormat(
        "Stand alone constructor equivalent to %s::Make",
        className.c_str()));
    writeStartCommentBlock(ostr, true);
    writeComments(ostr, description);
    writeEndCommentBlock(ostr);

    ostr << svc->getImport() << " " << className;
    if (!returnByValue)
        ostr << "ConstSP";
    ostr << " " << constructor;

    writeFunctionInputs(ostr, false, attributes, true, 4);
    ostr << ";\n";
}

void Class::implementConstructor(
    GeneratedOutput& ostr,
    const std::string& className,
    const std::string& constructor,
    const std::vector<AttributeConstSP>& attributes,
    bool returnByValue) const
{
    if (constructor.empty())
        return;

    ostr << "\n";

    ostr << className;
    if (!returnByValue)
        ostr << "ConstSP";
    ostr << " " << constructor;
    writeFunctionInputs(ostr, false, attributes, true, 4);
    ostr << "\n"
        << "{\n";

    ostr << "    return " << className << "::Make";
    std::vector<AttributeConstSP> outputs;
    writeArgsCall(ostr, false, attributes, 16 + className.length(), 8);
    ostr << ";\n";

    ostr << "}\n";
}

void Class::declareMethodAsFunction(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    const ClassMethodConstSP& method,
    const std::string& className,
    const std::string& funcPrefix,
    const std::string& instance,
    const DataTypeConstSP& instanceType) const
{
    if (funcPrefix.empty())
        return;

    const FunctionConstSP methodFunc = method->function();

    std::vector<std::string> description;
    description.push_back(spi_util::StringFormat(
        "Stand alone function equivalent to %s::%s",
        className.c_str(), methodFunc->name().c_str()));
    writeStartCommentBlock(ostr, true);
    writeComments(ostr, description);
    writeEndCommentBlock(ostr);

    ostr << svc->getImport() << " ";

    AttributeConstSP returns = methodFunc->returns();

    if (!returns)
    {
        ostr << "void";
    }
    else if (returns->arrayDim() > 0)
    {
        ostr << returns->dataType()->outerArrayType(returns->arrayDim());
    }
    else
    {
        ostr << returns->dataType()->outerValueType();
    }
    ostr << " " << funcPrefix << "_" << methodFunc->name();

    std::vector<AttributeConstSP> inputs;
    if (!method->isStatic())
    {
        std::vector<std::string> description;
        std::string name = instance.empty() ? "handle" : instance;
        description.push_back(spi_util::StringFormat("instance variable"));
        inputs.push_back(Attribute::Make(description, instanceType, true, name));
    }
    std::vector<AttributeConstSP> methodInputs = methodFunc->inputs();
    inputs.insert(inputs.end(), methodInputs.begin(), methodInputs.end());

    writeFunctionArgs(ostr, false, inputs, methodFunc->outputs(), true, 4);
    ostr << ";\n";

    if (methodFunc->outputs().size() > 0 && !returns)
    {
        ostr << "\n";

        ostr << svc->getImport() << "\n"
            << tupleOutput(methodFunc->outputs(), false)
            << " " << funcPrefix << "_" << methodFunc->name();
        writeFunctionArgs(ostr, false, inputs, std::vector<AttributeConstSP>(), true, 4);
        ostr << ";\n";
    }

}

void Class::implementMethodAsFunction(
    GeneratedOutput& ostr,
    const ClassMethodConstSP& method,
    const std::string& className,
    const std::string& funcPrefix,
    const std::string& instance,
    const DataTypeConstSP& instanceType) const
{
    if (funcPrefix.empty())
        return;

    const FunctionConstSP methodFunc = method->function();

    AttributeConstSP returns = methodFunc->returns();

    ostr << "\n";

    if (!returns)
    {
        ostr << "void";
    }
    else if (returns->arrayDim() > 0)
    {
        ostr << returns->dataType()->outerArrayType(returns->arrayDim());
    }
    else
    {
        ostr << returns->dataType()->outerValueType();
    }
    ostr << " " << funcPrefix << "_" << methodFunc->name();

    std::vector<AttributeConstSP> inputs;
    std::string instanceName = instance.empty() ? "handle" : instance;
    if (!method->isStatic())
    {
        std::vector<std::string> description;
        bool innerConst = true; // explain
        inputs.push_back(Attribute::Make(description, instanceType, innerConst, instanceName));
    }
    std::vector<AttributeConstSP> methodInputs = methodFunc->inputs();
    std::vector<AttributeConstSP> methodOutputs = methodFunc->outputs();
    inputs.insert(inputs.end(), methodInputs.begin(), methodInputs.end());

    writeFunctionArgs(ostr, false, inputs, methodOutputs, true, 4);

    ostr << "\n"
        << "{\n";

    if (!method->isStatic() && !byValue())
    {
        ostr << "    SPI_PRE_CONDITION(" << instanceName << ");\n";
    }

    size_t startPos = 0;
    ostr << "    ";
    startPos += 4;

    if (returns)
    {
        ostr << "return ";
        startPos += 7;
    }

    if (method->isStatic())
    {
        ostr << className << "::";
        startPos += (className.length() + 2);
    }
    else if (byValue())
    {
        ostr << instanceName << ".";
        startPos += (instanceName.length() + 1);
    }
    else
    {
        ostr << instanceName << "->";
        startPos += (instanceName.length() + 2);
    }

    ostr << methodFunc->name();
    startPos += methodFunc->name().length();

    std::vector<AttributeConstSP> args(methodInputs.begin(), methodInputs.end());
    args.insert(args.end(), methodOutputs.begin(), methodOutputs.end());
    writeArgsCall(ostr, false, args, startPos, 8);
    ostr << ";\n"
        << "}\n";

    // TBD: add version with tuple output

    if (methodFunc->outputs().size() > 0 && !returns)
    {
        ostr << "\n";

        ostr << tupleOutput(methodFunc->outputs(), false)
            << " " << funcPrefix << "_" << methodFunc->name();
        writeFunctionArgs(ostr, false, inputs, std::vector<AttributeConstSP>(), true, 4);
        ostr << "\n"
            << "{\n";

        if (!method->isStatic() && !byValue())
        {
            ostr << "    SPI_PRE_CONDITION(" << instanceName << ");\n";
        }

        size_t startPos = 0;
        ostr << "    ";
        startPos += 4;

        ostr << "return ";
        startPos += 7;

        if (method->isStatic())
        {
            ostr << className << "::";
            startPos += (className.length() + 2);
        }
        else if (byValue())
        {
            ostr << instanceName << ".";
            startPos += (instanceName.length() + 1);
        }
        else
        {
            ostr << instanceName << "->";
            startPos += (instanceName.length() + 2);
        }

        ostr << methodFunc->name();
        startPos += methodFunc->name().length();

        writeArgsCall(ostr, false, methodInputs, startPos, 8);
        ostr << ";\n"
            << "}\n";
    }


#if 0
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
#endif


}
