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
#ifndef GENERATOR_TOOLS_HPP
#define GENERATOR_TOOLS_HPP

#include <fstream>
#include <vector>
#include <spgtools/generatedOutput.hpp>
#include "attribute.hpp"
#include "serviceDefinition.hpp"
#include "options.hpp"

/**
 * Writes a list of function arguments to the output stream.
 * One argument per line.
 * Includes (..) but not the final ;
 *
 * Designed to be use either for declaring the function or for implementing
 * the function.
 *
 * indent defines the number of spaces to indent before each line.
 * innerContext determines whether you are declaring the function within the
 * inner context or the outer context
 *
 * at present we do not do anything to ensure that the argument names are
 * lined up - this is a potential future enhancment.
 */
void writeFunctionArgs(
    GeneratedOutput& ostr,
    bool innerContext,
    const std::vector<AttributeConstSP>& inputs,
    const std::vector<AttributeConstSP>& outputs,
    bool allowOptional,
    size_t indent=4,
    const char* prefix="");

void writeFunctionInputs(
    GeneratedOutput& ostr,
    bool innerContext,
    const std::vector<AttributeConstSP>& inputs,
    bool allowOptional,
    size_t indent=4,
    const char* prefix="");

void writeFunctionArg(
    GeneratedOutput& ostr,
    bool innerContext,
    const AttributeConstSP& arg,
    bool isOutput=false,
    const char* prefix="",
    bool provideDefault=false);

/**
 * Given variables that have previously been defined, this will write the
 * parameter list.
 *
 * innerContext is used in combination with dataType->needsTranslation
 *
 * if we are using variables from the inner context, and the data needed
 * translation then we prepend the translationPrefix to the variable name
 *
 * however if the variable did not need translation, then we do not prepend
 * the translation prefix - essentially just passing through the outer
 * context variable to the inner context function without translation
 *
 * startPos indicates where in the line we had reached before starting the
 * parameter list
 *
 * indent indicates where we should start a newline
 *
 * wherever possible we will not overflow beyond 78 characters, but we will
 * allow many variables on one line upto this limit
 */
void writeArgsCall(
    GeneratedOutput& ostr,
    bool innerContext,
    const std::vector<AttributeConstSP>& args,
    size_t startPos,
    size_t indent=4,
    const char* conversionPrefix="");

void writeArgsInitialisers(
    GeneratedOutput& ostr,
    bool started,
    const std::vector<AttributeConstSP>& args,
    size_t indent=4,
    const char* prefix="");

// writes a list of argument declarations to the output stream
void writeDeclareArgs(
    GeneratedOutput& ostr,
    bool innerContext,
    const std::vector<AttributeConstSP>& args,
    size_t indent,
    const char* prefix="",
    bool isConst=false,
    const char* conversionPrefix="",
    bool noDeclareNoConvert=false
);

// std::vector<AttributeConstSP> selectArgsNeedingConversion(
//     const std::vector<AttributeConstSP>& args);

#if 0
void writeToMap(
    GeneratedOutput& ostr,
    const std::vector<AttributeConstSP>& args,
    const char* objectMap="obj_map",
    const char* getDataFormat="%s",
    size_t indent=4);
#endif

void writeToMap(
    GeneratedOutput& ostr,
    const std::vector<ClassAttributeConstSP>& attrs,
    const char* objectSink = "obj_map",
    const char* getDataFormat = "%s",
    size_t indent = 4);

void writeGetClassAttributesForMap(
    GeneratedOutput& ostr,
    const std::vector<ClassAttributeConstSP>& attrs,
    std::set<std::string>& done,
    size_t indent = 4,
    bool isClosed = false);

void writePropertiesToMap(
    GeneratedOutput& ostr,
    const std::vector<ClassAttributeConstSP>& properties,
    const char* objectMap,
    const char* getDataFormat,
    size_t indent,
    bool hasDynamicProperties=false);

void writeFromMap(
    GeneratedOutput& ostr,
    const std::vector<ClassAttributeConstSP>& args,
    const std::string& constructor,
    const char* objectMap="obj_map",
    const char* valueToObject="value_to_object",
    size_t indent=4);

void writeInnerDeclarationsAndTranslations(
    GeneratedOutput& ostr,
    const std::vector<AttributeConstSP> args,
    const char* innerPrefix,
    bool declareAll = false,
    const char* outerPrefix = "",
    size_t indent = 4);

void writeAccessorDeclaration(
    GeneratedOutput& ostr,
    const ClassAttributeConstSP& attr,
    const std::string& className,
    bool innerContext=false,
    bool usePropertyName=false);

void writeOpenAccessor(
    GeneratedOutput& ostr,
    const ClassAttributeConstSP& attr,
    const std::string& className,
    bool innerContext,
    bool hasVerbatim,
    bool usePropertyName=false);

void writeClosedAccessor(
    GeneratedOutput& ostr,
    const ClassAttributeConstSP& attr,
    const std::string& className);

void writeObjectMethodDeclarations(
    GeneratedOutput& ostr,
    const std::string& className);

void writeObjectMethodImplementation(
    GeneratedOutput& ostr,
    const std::string& className);

void writeInclude(
    GeneratedOutput& ostr,
    const std::string& header);

void setGeneratorToolsOptions(const Options& options);
bool noGeneratedCodeNotice();
bool sessionLogging();

size_t firstOptionalInput(const std::vector<AttributeConstSP>& inputs);

std::string tupleOutput(const std::vector<AttributeConstSP>& outputs,
    bool innerContext = false);

void writeCallFromTupleOutputFunction(
    GeneratedOutput& ostr,
    const std::vector<AttributeConstSP>& inputs,
    const std::vector<AttributeConstSP>& outputs,
    const std::string& name);

void writeVerbatim(
    GeneratedOutput& ostr,
    const VerbatimConstSP& verbatim,
    size_t indent=0,
    bool skipClosure=false);

// this is the implementation of the actual call to the inner function
// returning the variable that represents the actual output
//
// I can foresee problems with functions using output variables
std::string writeCallToInnerFunction(
    GeneratedOutput& ostr,
    const DataTypeConstSP& returnType,
    int returnArrayDim,
    bool noConvert,
    const std::vector<AttributeConstSP>& inputs,
    const std::vector<AttributeConstSP>& outputs,
    const std::string& caller,
    bool declareOutputs);

void writeFromValueInContext(
    GeneratedOutput&       ostr,
    const std::string&     name,
    const std::string&     value,
    const DataTypeConstSP& dataType,
    int                    arrayDim,
    bool                   isOptional,
    const ConstantConstSP& defaultValue);

void writeFunctionCaller(
    GeneratedOutput&        ostr,
    const std::string&      ns,
    const std::string&      className,
    const std::string&      functionName,
    const DataTypeConstSP&  returnType,
    int                     returnArrayDim,
    const DataTypeConstSP&  instanceType,
    const ServiceDefinitionSP&           svc,
    const std::vector<AttributeConstSP>& inputs,
    const std::vector<AttributeConstSP>& outputs
        =std::vector<AttributeConstSP>());

void writeFunctionConstructor(
    GeneratedOutput& ostr,
    const std::string& name,
    const std::string& serviceName,
    const std::string& ns,
    const std::string& className,
    const std::string& functionName,
    const std::vector<AttributeConstSP>& inputs,
    int indent=0);

void writeFunctionObjectType(
    GeneratedOutput& ostr,
    const std::string& ns,
    const std::string& name,
    const std::string& serviceNamespace,
    const std::string& className = std::string());

void writeFunctionOutputLogging(
    GeneratedOutput& ostr,
    const std::string& serviceName,
    const DataTypeConstSP& returnType,
    int returnArrayDim,
    const std::string& returnValue,
    const std::vector<AttributeConstSP>& outputs,
    size_t indent=0);

void writeConstructorCatchBlock(
    GeneratedOutput&   ostr,
    const std::string& name);

void writeFunctionCatchBlock(
    GeneratedOutput&   ostr,
    const std::string& serviceName,
    const std::string& name,
    const std::string& className = std::string());

#endif
