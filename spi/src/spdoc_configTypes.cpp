/*

    Sartorial Programming Interface (SPI) runtime libraries
    Copyright (C) 2012-2023 Sartorial Programming Ltd.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA

*/

/**
****************************************************************************
* Source file: spdoc_configTypes.cpp
****************************************************************************
*/

#include "spdoc_configTypes.hpp"
#include "spdoc_configTypes_helper.hpp"
#include "spdoc_dll_service_manager.hpp"

#include <sstream>

SPDOC_BEGIN_NAMESPACE

/*
****************************************************************************
* Implementation of DataType
****************************************************************************
*/
DataTypeConstSP DataType::Make(
    const std::string& name,
    bool passByReference,
    const std::string& refTypeName,
    const std::string& valueTypeName,
    PublicType publicType,
    bool noDoc,
    const std::string& nsService,
    bool objectAsValue)
{
    spdoc_check_permission();
    return DataTypeConstSP(
        new DataType(name, passByReference, refTypeName, valueTypeName,
            publicType, noDoc, nsService, objectAsValue));
}

DataType::DataType(
    const std::string& name,
    bool passByReference,
    const std::string& refTypeName,
    const std::string& valueTypeName,
    PublicType publicType,
    bool noDoc,
    const std::string& nsService,
    bool objectAsValue)
    :
    name(name),
    passByReference(passByReference),
    refTypeName(refTypeName),
    valueTypeName(valueTypeName),
    publicType(publicType),
    noDoc(noDoc),
    nsService(nsService),
    objectAsValue(objectAsValue)
{}

/*
****************************************************************************
* Returns the value type (includes the namespace)
****************************************************************************
*/

std::string DataType::ValueType() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.DataType.ValueType");
  try
  {
    DataTypeConstSP self(this);
    const std::string& i_result = DataType_Helper::ValueType(self);
    clear_public_map();

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "DataType.ValueType", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "DataType.ValueType"); }
}

std::string DataType_Helper::ValueType(
    const DataTypeConstSP& in_self)
{
    const DataType* self = in_self.get();

    if (self->nsService.empty())
        return self->valueTypeName;

    std::ostringstream oss;
    oss << self->nsService << "::" << self->valueTypeName;
    return oss.str();
}

/*
****************************************************************************
* Returns the reference type (includes the namespace)
****************************************************************************
*/

std::string DataType::RefType() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.DataType.RefType");
  try
  {
    DataTypeConstSP self(this);
    const std::string& i_result = DataType_Helper::RefType(self);
    clear_public_map();

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "DataType.RefType", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "DataType.RefType"); }
}

std::string DataType_Helper::RefType(
    const DataTypeConstSP& in_self)
{
    const DataType* self = in_self.get();

    if (self->nsService.empty())
        return self->refTypeName;

    std::ostringstream oss;
    if (self->valueTypeName == self->refTypeName)
    {
        oss << self->nsService << "::" << self->refTypeName;
    }
    else
    {
        // the assumption is that refTypeName = "const valueTypeName&"
        oss << "const " << self->nsService << "::" 
            << self->valueTypeName << "&";
    }
    return oss.str();
}

/*
****************************************************************************
* Implementation of Attribute
****************************************************************************
*/
AttributeConstSP Attribute::Make(
    const std::string& name,
    const std::vector<std::string>& description,
    const DataTypeConstSP& dataType,
    int arrayDim,
    bool isOptional,
    const ConstantConstSP& defaultValue)
{
    spdoc_check_permission();
    return AttributeConstSP(
        new Attribute(name, description, dataType, arrayDim, isOptional,
            defaultValue));
}

Attribute::Attribute(
    const std::string& name,
    const std::vector<std::string>& description,
    const DataTypeConstSP& dataType,
    int arrayDim,
    bool isOptional,
    const ConstantConstSP& defaultValue)
    :
    spi::Object(true),
    name(name),
    description(description),
    dataType(dataType),
    arrayDim(arrayDim),
    isOptional(isOptional),
    defaultValue(defaultValue)
{
try
  {
    SPI_PRE_CONDITION(arrayDim >= 0 && arrayDim <= 2);
}
  catch (std::exception& e)
  { SPI_THROW_RUNTIME_ERROR("Could not create Attribute object:\n" << e.what()); }
  catch (...)
  { SPI_THROW_RUNTIME_ERROR("Could not create Attribute object:\n" << "Unknown exception"); }
}

std::string Attribute::encoding(
    bool isOutput) const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Attribute.encoding");
  try
  {
    AttributeConstSP self(this);
    const std::string& i_result = Attribute_Helper::encoding(self, isOutput);
    clear_public_map();

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Attribute.encoding", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Attribute.encoding"); }
}

std::string Attribute_Helper::encoding(
    const AttributeConstSP& in_self,
    bool isOutput)
{
    const Attribute* self = in_self.get();

    std::ostringstream oss;
    oss << self->dataType->name << " ";
    if (isOutput)
        oss << "&";
    oss << self->name;
    if (self->isOptional)
        oss << "?";
    for (int i = 0; i < self->arrayDim; ++i)
        oss << "[]";
    return oss.str();
}

bool Attribute::isArray() const
{
    return arrayDim > 0;
}

/*
****************************************************************************
* Implementation of ClassAttribute
****************************************************************************
*/
ClassAttributeConstSP ClassAttribute::Make(
    const std::string& name,
    const std::vector<std::string>& description,
    const DataTypeConstSP& dataType,
    int arrayDim,
    bool isOptional,
    const ConstantConstSP& defaultValue,
    bool accessible,
    const std::string& accessor)
{
    spdoc_check_permission();
    return ClassAttributeConstSP(
        new ClassAttribute(name, description, dataType, arrayDim, isOptional,
            defaultValue, accessible, accessor));
}

ClassAttribute::ClassAttribute(
    const std::string& name,
    const std::vector<std::string>& description,
    const DataTypeConstSP& dataType,
    int arrayDim,
    bool isOptional,
    const ConstantConstSP& defaultValue,
    bool accessible,
    const std::string& accessor)
    :
    spi::Object(true),
    name(name),
    description(description),
    dataType(dataType),
    arrayDim(arrayDim),
    isOptional(isOptional),
    defaultValue(defaultValue),
    accessible(accessible),
    accessor(accessor)
{
try
  {
    SPI_PRE_CONDITION(arrayDim >= 0 && arrayDim <= 2);
}
  catch (std::exception& e)
  { SPI_THROW_RUNTIME_ERROR("Could not create ClassAttribute object:\n" << e.what()); }
  catch (...)
  { SPI_THROW_RUNTIME_ERROR("Could not create ClassAttribute object:\n" << "Unknown exception"); }
}

ClassAttribute::operator AttributeConstSP() const
{
    return Attribute::Make(name, description, dataType, arrayDim, isOptional, defaultValue);
}

bool ClassAttribute::isArray() const
{
    return arrayDim > 0;
}

std::string ClassAttribute::encoding() const
{
    if (!accessible)
        return std::string();
    std::ostringstream oss;
    oss << dataType->name << " " << name;
    if (isOptional)
        oss << "?";
    for (int i = 0; i < arrayDim; ++i)
        oss << "[]";
    return oss.str();
}

/*
****************************************************************************
* Implementation of Construct
****************************************************************************
*/
Construct::Construct()
    : spi::Object(true)
{}

/*
****************************************************************************
* Implementation of SimpleType
****************************************************************************
*/
SimpleTypeConstSP SimpleType::Make(
    const std::string& name,
    const std::vector<std::string>& description,
    const std::string& typeName,
    bool noDoc)
{
    spdoc_check_permission();
    return SimpleTypeConstSP(
        new SimpleType(name, description, typeName, noDoc));
}

SimpleType::SimpleType(
    const std::string& name,
    const std::vector<std::string>& description,
    const std::string& typeName,
    bool noDoc)
    :
    name(name),
    description(description),
    typeName(typeName),
    noDoc(noDoc)
{}

/*
****************************************************************************
* Returns "SIMPLE_TYPE"
****************************************************************************
*/

std::string SimpleType::getType() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.SimpleType.getType");
  try
  {
    SimpleTypeConstSP self(this);
    const std::string& i_result = SimpleType_Helper::getType(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "SimpleType.getType", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "SimpleType.getType"); }
}

std::string SimpleType_Helper::getType(
    const SimpleTypeConstSP& in_self)
{
    const SimpleType* self = in_self.get();

    return "SIMPLE_TYPE";
}

std::vector<std::string> SimpleType::Summary() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.SimpleType.Summary");
  try
  {
    SimpleTypeConstSP self(this);
    const std::vector<std::string>& i_result = SimpleType_Helper::Summary(
        self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "SimpleType.Summary", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "SimpleType.Summary"); }
}

std::vector<std::string> SimpleType_Helper::Summary(
    const SimpleTypeConstSP& in_self)
{
    const SimpleType* self = in_self.get();

    std::vector<std::string> summary;
    if (!self->noDoc)
    {
        std::ostringstream oss;
        oss << "typedef " << self->typeName << " " << self->name << ";";
        summary.push_back(oss.str());
    }
    return summary;
}

/*
****************************************************************************
* Implementation of Function
****************************************************************************
*/
FunctionConstSP Function::Make(
    const std::string& name,
    const std::vector<std::string>& description,
    const std::vector<std::string>& returnTypeDescription,
    const DataTypeConstSP& returnType,
    int returnArrayDim,
    const std::vector<AttributeConstSP>& inputs,
    const std::vector<AttributeConstSP>& outputs,
    const std::vector<std::string>& excelOptions,
    bool optionalReturnType)
{
    spdoc_check_permission();
    return FunctionConstSP(
        new Function(name, description, returnTypeDescription, returnType,
            returnArrayDim, inputs, outputs, excelOptions, optionalReturnType));
}

Function::Function(
    const std::string& name,
    const std::vector<std::string>& description,
    const std::vector<std::string>& returnTypeDescription,
    const DataTypeConstSP& returnType,
    int returnArrayDim,
    const std::vector<AttributeConstSP>& inputs,
    const std::vector<AttributeConstSP>& outputs,
    const std::vector<std::string>& excelOptions,
    bool optionalReturnType)
    :
    name(name),
    description(description),
    returnTypeDescription(returnTypeDescription),
    returnType(returnType),
    returnArrayDim(returnArrayDim),
    inputs(inputs),
    outputs(outputs),
    excelOptions(excelOptions),
    optionalReturnType(optionalReturnType)
{}

/*
****************************************************************************
* Does this Function return an object or not?
****************************************************************************
*/

bool Function::returnsObject() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Function.returnsObject");
  try
  {
    FunctionConstSP self(this);
    bool i_result = Function_Helper::returnsObject(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Function.returnsObject", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Function.returnsObject"); }
}

bool Function_Helper::returnsObject(
    const FunctionConstSP& in_self)
{
    const Function* self = in_self.get();

    if (self->objectCount() > 0)
        return true;
    return false;
}

/*
****************************************************************************
* What does the function return (as an attribute)
****************************************************************************
*/

AttributeConstSP Function::returns() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Function.returns");
  try
  {
    FunctionConstSP self(this);
    const AttributeConstSP& i_result = Function_Helper::returns(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Function.returns", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Function.returns"); }
}

AttributeConstSP Function_Helper::returns(
    const FunctionConstSP& in_self)
{
    const Function* self = in_self.get();

    return self->returns();
}

/*
****************************************************************************
* How many objects does this function return?
****************************************************************************
*/

int Function::objectCount() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Function.objectCount");
  try
  {
    FunctionConstSP self(this);
    int i_result = Function_Helper::objectCount(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Function.objectCount", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Function.objectCount"); }
}

int Function_Helper::objectCount(
    const FunctionConstSP& in_self)
{
    const Function* self = in_self.get();

    if (self->returnType)
    {
        PublicType publicType = self->returnType->publicType;
        if (publicType == PublicType::CLASS ||
            publicType == PublicType::OBJECT ||
            publicType == PublicType::MAP)
        {
            if (self->returnType->objectAsValue)
                return 0;
            return 1;
        }
        return 0;
    }
    int objectCount = 0;
    for (size_t i = 0; i < self->outputs.size(); ++i)
    {
        PublicType publicType = self->outputs[i]->dataType->publicType;
        if (publicType == PublicType::CLASS ||
            publicType == PublicType::OBJECT ||
            publicType == PublicType::MAP)
        {
            if (!self->outputs[i]->dataType->objectAsValue)
                ++objectCount;
        }
    }
    return objectCount;
}

std::vector<std::string> Function::Summary() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Function.Summary");
  try
  {
    FunctionConstSP self(this);
    const std::vector<std::string>& i_result = Function_Helper::Summary(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Function.Summary", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Function.Summary"); }
}

std::vector<std::string> Function_Helper::Summary(
    const FunctionConstSP& in_self)
{
    const Function* self = in_self.get();

    std::vector<std::string> summary;
    std::ostringstream oss;
    if (self->returnType)
    {
        oss << self->returnType->name;
        for (int i = 0; i < self->returnArrayDim; ++i)
            oss << "[]";   
    }
    else
    {
        oss << "void";
    }
    oss << " " << self->name << "(";
    if (self->inputs.size() == 0 && self->outputs.size() == 0)
    {
        oss << ");";
        summary.push_back(oss.str());
    }
    else
    {
        summary.push_back(oss.str());
        std::vector<std::string> args;
        for (size_t i = 0; i < self->inputs.size(); ++i)
            args.push_back(self->inputs[i]->encoding(false));
        for (size_t i = 0; i < self->outputs.size(); ++i)
            args.push_back(self->outputs[i]->encoding(true));
        size_t numArgs = args.size();
        for (size_t i = 0; i < numArgs; ++i)
        {
            std::ostringstream oss;
            oss << "    " << args[i];
            if ((i+1) == numArgs)
            {
                oss << ");";
            }
            else
            {
                oss << ",";
            }
            summary.push_back(oss.str());
        }
    }
    return summary;
}

/*
****************************************************************************
* Returns "FUNCTION"
****************************************************************************
*/

std::string Function::getType() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Function.getType");
  try
  {
    FunctionConstSP self(this);
    const std::string& i_result = Function_Helper::getType(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Function.getType", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Function.getType"); }
}

std::string Function_Helper::getType(
    const FunctionConstSP& in_self)
{
    const Function* self = in_self.get();

    return "FUNCTION";
}

/*
****************************************************************************
* Implementation of Enumerand
****************************************************************************
*/
EnumerandConstSP Enumerand::Make(
    const std::string& code,
    const std::vector<std::string>& strings,
    const std::vector<std::string>& description)
{
    spdoc_check_permission();
    return EnumerandConstSP(
        new Enumerand(code, strings, description));
}

Enumerand::Enumerand(
    const std::string& code,
    const std::vector<std::string>& strings,
    const std::vector<std::string>& description)
    :
    spi::Object(true),
    code(code),
    strings(strings),
    description(description)
{}

/*
****************************************************************************
* Implementation of EnumConstructor
****************************************************************************
*/
EnumConstructorConstSP EnumConstructor::Make(
    PublicType constructorType,
    const std::vector<std::string>& description)
{
    spdoc_check_permission();
    return EnumConstructorConstSP(
        new EnumConstructor(constructorType, description));
}

EnumConstructor::EnumConstructor(
    PublicType constructorType,
    const std::vector<std::string>& description)
    :
    spi::Object(true),
    constructorType(constructorType),
    description(description)
{}

/*
****************************************************************************
* Implementation of Enum
****************************************************************************
*/
EnumConstSP Enum::Make(
    const std::string& name,
    const std::vector<std::string>& description,
    const std::vector<EnumerandConstSP>& enumerands,
    const std::vector<EnumConstructorConstSP>& constructors)
{
    spdoc_check_permission();
    return EnumConstSP(
        new Enum(name, description, enumerands, constructors));
}

Enum::Enum(
    const std::string& name,
    const std::vector<std::string>& description,
    const std::vector<EnumerandConstSP>& enumerands,
    const std::vector<EnumConstructorConstSP>& constructors)
    :
    name(name),
    description(description),
    enumerands(enumerands),
    constructors(constructors)
{}

/*
****************************************************************************
* Returns "ENUM"
****************************************************************************
*/

std::string Enum::getType() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Enum.getType");
  try
  {
    EnumConstSP self(this);
    const std::string& i_result = Enum_Helper::getType(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Enum.getType", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Enum.getType"); }
}

std::string Enum_Helper::getType(
    const EnumConstSP& in_self)
{
    const Enum* self = in_self.get();

    return "ENUM";
}

std::vector<std::string> Enum::Summary() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Enum.Summary");
  try
  {
    EnumConstSP self(this);
    const std::vector<std::string>& i_result = Enum_Helper::Summary(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Enum.Summary", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Enum.Summary"); }
}

std::vector<std::string> Enum_Helper::Summary(
    const EnumConstSP& in_self)
{
    const Enum* self = in_self.get();

    std::vector<std::string> summary;

    size_t numEnumerands = self->enumerands.size();
    
    if (numEnumerands > 0)
    {
        std::ostringstream oss;
        oss << "enum " << self->name;
        summary.push_back(oss.str());
        summary.push_back("{");
        for (size_t i = 0; i < numEnumerands; ++i)
        {
            std::ostringstream oss;
            oss << "    " << self->enumerands[i]->code;
            if ((i+1) < numEnumerands)
                oss << ",";
            summary.push_back(oss.str());
        }
        summary.push_back("};");
    }
    else
    {
        std::ostringstream oss;
        oss << "enum " << self->name << ";";
        summary.push_back(oss.str());
    }

    return summary;
}

/*
****************************************************************************
* Implementation of ClassMethod
****************************************************************************
*/
ClassMethodConstSP ClassMethod::Make(
    const FunctionConstSP& function,
    bool isConst,
    bool isVirtual,
    bool isStatic,
    bool isImplementation,
    const std::string& implements)
{
    spdoc_check_permission();
    return ClassMethodConstSP(
        new ClassMethod(function, isConst, isVirtual, isStatic,
            isImplementation, implements));
}

ClassMethod::ClassMethod(
    const FunctionConstSP& function,
    bool isConst,
    bool isVirtual,
    bool isStatic,
    bool isImplementation,
    const std::string& implements)
    :
    spi::Object(true),
    function(function),
    isConst(isConst),
    isVirtual(isVirtual),
    isStatic(isStatic),
    isImplementation(isImplementation),
    implements(implements)
{}

std::vector<std::string> ClassMethod::Summary() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.ClassMethod.Summary");
  try
  {
    ClassMethodConstSP self(this);
    const std::vector<std::string>& i_result = ClassMethod_Helper::Summary(
        self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "ClassMethod.Summary", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "ClassMethod.Summary"); }
}

std::vector<std::string> ClassMethod_Helper::Summary(
    const ClassMethodConstSP& in_self)
{
    const ClassMethod* self = in_self.get();

    std::vector<std::string> summary;
    std::vector<std::string> funcSummary = self->function->Summary();

    size_t numLines = funcSummary.size();
    if (numLines == 0)
        return summary;
    summary.reserve(numLines);

    std::ostringstream oss;
    oss << "    ";
    if (self->isVirtual)
        oss << "virtual ";
    if (self->isStatic)
        oss << "static ";

    if (self->isConst)
    {
        std::string lastLine = spi_util::StringReplace(funcSummary[numLines-1], ";", " const;");
        funcSummary[numLines-1] = lastLine;
    }
    oss << funcSummary[0];
    summary.push_back(oss.str());

    for (size_t i = 1; i < numLines; ++i)
    {
        std::ostringstream oss;
        oss << "    " << funcSummary[i];
        summary.push_back(oss.str());
    }

    return summary;
}

/*
****************************************************************************
* Implementation of CoerceFrom
****************************************************************************
*/
CoerceFromConstSP CoerceFrom::Make(
    const std::vector<std::string>& description,
    const AttributeConstSP& coerceFrom)
{
    spdoc_check_permission();
    return CoerceFromConstSP(
        new CoerceFrom(description, coerceFrom));
}

CoerceFrom::CoerceFrom(
    const std::vector<std::string>& description,
    const AttributeConstSP& coerceFrom)
    :
    spi::Object(true),
    description(description),
    coerceFrom(coerceFrom)
{}

std::vector<std::string> CoerceFrom::Summary() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.CoerceFrom.Summary");
  try
  {
    CoerceFromConstSP self(this);
    const std::vector<std::string>& i_result = CoerceFrom_Helper::Summary(
        self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "CoerceFrom.Summary", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "CoerceFrom.Summary"); }
}

std::vector<std::string> CoerceFrom_Helper::Summary(
    const CoerceFromConstSP& in_self)
{
    const CoerceFrom* self = in_self.get();

    std::string encoding = self->coerceFrom->encoding(false);
    std::vector<std::string> summary;
    std::ostringstream oss;
    oss << "    static Coerce(" << encoding << ");";
    summary.push_back(oss.str());
    return summary;
}

/*
****************************************************************************
* Implementation of CoerceTo
****************************************************************************
*/
CoerceToConstSP CoerceTo::Make(
    const std::vector<std::string>& description,
    const std::string& className,
    const DataTypeConstSP& classType)
{
    spdoc_check_permission();
    return CoerceToConstSP(
        new CoerceTo(description, className, classType));
}

CoerceTo::CoerceTo(
    const std::vector<std::string>& description,
    const std::string& className,
    const DataTypeConstSP& classType)
    :
    spi::Object(true),
    description(description),
    className(className),
    classType(classType)
{}

std::vector<std::string> CoerceTo::Summary() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.CoerceTo.Summary");
  try
  {
    CoerceToConstSP self(this);
    const std::vector<std::string>& i_result = CoerceTo_Helper::Summary(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "CoerceTo.Summary", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "CoerceTo.Summary"); }
}

std::vector<std::string> CoerceTo_Helper::Summary(
    const CoerceToConstSP& in_self)
{
    const CoerceTo* self = in_self.get();

    std::vector<std::string> summary;
    std::ostringstream oss;
    oss << "    operator " << self->className << "();";
    summary.push_back(oss.str());
    return summary;
}

/*
****************************************************************************
* Implementation of Class
****************************************************************************
*/
ClassConstSP Class::Make(
    const std::string& name,
    const std::string& ns,
    const std::vector<std::string>& description,
    const std::string& baseClassName,
    const std::vector<ClassAttributeConstSP>& attributes,
    const std::vector<ClassAttributeConstSP>& properties,
    const std::vector<ClassMethodConstSP>& methods,
    const std::vector<CoerceFromConstSP>& coerceFrom,
    const std::vector<CoerceToConstSP>& coerceTo,
    bool isAbstract,
    bool noMake,
    const std::string& objectName,
    const DataTypeConstSP& dataType,
    bool isDelegate,
    bool canPut,
    bool hasDynamicAttributes,
    bool asValue,
    const std::string& constructor)
{
    spdoc_check_permission();
    return ClassConstSP(
        new Class(name, ns, description, baseClassName, attributes,
            properties, methods, coerceFrom, coerceTo, isAbstract, noMake,
            objectName, dataType, isDelegate, canPut, hasDynamicAttributes,
            asValue, constructor));
}

Class::Class(
    const std::string& name,
    const std::string& ns,
    const std::vector<std::string>& description,
    const std::string& baseClassName,
    const std::vector<ClassAttributeConstSP>& attributes,
    const std::vector<ClassAttributeConstSP>& properties,
    const std::vector<ClassMethodConstSP>& methods,
    const std::vector<CoerceFromConstSP>& coerceFrom,
    const std::vector<CoerceToConstSP>& coerceTo,
    bool isAbstract,
    bool noMake,
    const std::string& objectName,
    const DataTypeConstSP& dataType,
    bool isDelegate,
    bool canPut,
    bool hasDynamicAttributes,
    bool asValue,
    const std::string& constructor)
    :
    name(name),
    ns(ns),
    description(description),
    baseClassName(baseClassName),
    attributes(attributes),
    properties(properties),
    methods(methods),
    coerceFrom(coerceFrom),
    coerceTo(coerceTo),
    isAbstract(isAbstract),
    noMake(noMake),
    objectName(objectName),
    dataType(dataType),
    isDelegate(isDelegate),
    canPut(canPut),
    hasDynamicAttributes(hasDynamicAttributes),
    asValue(asValue),
    constructor(constructor)
{}

std::vector<std::string> Class::Summary() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Class.Summary");
  try
  {
    ClassConstSP self(this);
    const std::vector<std::string>& i_result = Class_Helper::Summary(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Class.Summary", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Class.Summary"); }
}

std::vector<std::string> Class_Helper::Summary(
    const ClassConstSP& in_self)
{
    const Class* self = in_self.get();

    std::vector<std::string> summary;
    std::ostringstream oss;
    oss << "class " << self->name;
    if (!self->baseClassName.empty())
        oss << " : " << self->baseClassName;
    summary.push_back(oss.str());
    summary.push_back("{");

    bool started = false;
    for (size_t i = 0; i < self->attributes.size(); ++i)
    {
        std::ostringstream oss;
        std::string encoding = self->attributes[i]->encoding();
        if (!encoding.empty())
        {
            oss << "    " << encoding << ";";
            summary.push_back(oss.str());
            started = true;
        }
    }
    for (size_t i = 0; i < self->properties.size(); ++i)
    {
        std::ostringstream oss;
        std::string encoding = self->properties[i]->encoding();
        if (!encoding.empty())
        {
            oss << "    property " << encoding << ";";
            summary.push_back(oss.str());
            started = true;
        }
    }
    for (size_t i = 0; i < self->methods.size(); ++i)
    {
        std::vector<std::string> mSummary = self->methods[i]->Summary();
        if (started)
            summary.push_back("");
        else
            started = true;
        for (size_t j = 0; j < mSummary.size(); ++j)
            summary.push_back(mSummary[j]);
    }
    for (size_t i = 0; i < self->coerceFrom.size(); ++i)
    {
        std::vector<std::string> cfSummary = self->coerceFrom[i]->Summary();
        if (started)
            summary.push_back("");
        else
            started = true;
        summary.insert(summary.end(), cfSummary.begin(), cfSummary.end());
    }
    for (size_t i = 0; i < self->coerceTo.size(); ++i)
    {
        std::vector<std::string> ctSummary = self->coerceTo[i]->Summary();
        if (started)
            summary.push_back("");
        else
            started = true;
        summary.insert(summary.end(), ctSummary.begin(), ctSummary.end());
    }
    summary.push_back("};");

    return summary;
}

/*
****************************************************************************
* Returns "CLASS"
****************************************************************************
*/

std::string Class::getType() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Class.getType");
  try
  {
    ClassConstSP self(this);
    const std::string& i_result = Class_Helper::getType(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Class.getType", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Class.getType"); }
}

std::string Class_Helper::getType(
    const ClassConstSP& in_self)
{
    const Class* self = in_self.get();

    return "CLASS";
}

/*
****************************************************************************
* Returns the object name. If objectName is defined, then that is the
* object name. Otherwise name is the object name.
****************************************************************************
*/

std::string Class::ObjectName() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Class.ObjectName");
  try
  {
    ClassConstSP self(this);
    const std::string& i_result = Class_Helper::ObjectName(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Class.ObjectName", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Class.ObjectName"); }
}

std::string Class_Helper::ObjectName(
    const ClassConstSP& in_self)
{
    const Class* self = in_self.get();

    if (self->objectName.empty())
        return self->name;
    return self->objectName;
}

std::string Class::ServiceNamespace() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Class.ServiceNamespace");
  try
  {
    ClassConstSP self(this);
    const std::string& i_result = Class_Helper::ServiceNamespace(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Class.ServiceNamespace", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Class.ServiceNamespace"); }
}

std::string Class_Helper::ServiceNamespace(
    const ClassConstSP& in_self)
{
    const Class* self = in_self.get();

    if (!self->dataType)
        throw spi::RuntimeError("No data type for class %s",
            self->name.c_str());
    return self->dataType->nsService;
}

/*
****************************************************************************
* Implementation of Module
****************************************************************************
*/
ModuleConstSP Module::Make(
    const std::string& name,
    const std::vector<std::string>& description,
    const std::string& ns,
    const std::vector<ConstructConstSP>& constructs)
{
    spdoc_check_permission();
    return ModuleConstSP(
        new Module(name, description, ns, constructs));
}

Module::Module(
    const std::string& name,
    const std::vector<std::string>& description,
    const std::string& ns,
    const std::vector<ConstructConstSP>& constructs)
    :
    spi::Object(true),
    name(name),
    description(description),
    ns(ns),
    constructs(constructs)
{}

std::vector<std::string> Module::combineSummaries(
    const std::vector<ModuleConstSP>& modules,
    bool sort)
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Module.combineSummaries");
  try
  {
    const std::vector<std::string>& i_result = Module_Helper::combineSummaries(
        modules, sort);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Module.combineSummaries", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Module.combineSummaries"); }
}

std::vector<std::string> Module_Helper::combineSummaries(
    const std::vector<ModuleConstSP>& modules,
    bool sort)
{

    std::vector<std::string> summary;
    size_t numModules = modules.size();
    std::map<std::string, std::vector<std::string> > nameIndex;
    for (size_t i = 0; i < numModules; ++i)
    {
        ModuleConstSP m = modules[i];
        size_t numConstructs = m->constructs.size();
        for (size_t j = 0; j < numConstructs; ++j)
        {
            const ConstructConstSP& c = m->constructs[j];
            std::vector<std::string> cSummary = c->Summary();
            if (cSummary.size() > 0)
            {
                if (sort)
                {
                    nameIndex[c->getName()] = cSummary;
                }
                else
                {
                    summary.push_back("");
                    summary.insert(summary.end(), cSummary.begin(), cSummary.end());
                }
            }
        }
    }
    if (sort)
    {
        for (std::map<std::string, std::vector<std::string> >::const_iterator iter = nameIndex.begin();
             iter != nameIndex.end(); ++iter)
        {
            summary.push_back("");
            summary.insert(summary.end(), iter->second.begin(), iter->second.end());
        }
    }
    return summary;
}

/*
****************************************************************************
* Implementation of Service
****************************************************************************
*/
ServiceConstSP Service::Make(
    const std::string& name,
    const std::vector<std::string>& description,
    const std::string& longName,
    const std::string& ns,
    const std::string& declSpec,
    const std::string& version,
    const std::vector<ModuleConstSP>& modules,
    const std::vector<ClassConstSP>& importedBaseClasses,
    const std::vector<EnumConstSP>& importedEnums,
    bool sharedService)
{
    spdoc_check_permission();
    return ServiceConstSP(
        new Service(name, description, longName, ns, declSpec, version,
            modules, importedBaseClasses, importedEnums, sharedService));
}

Service::Service(
    const std::string& name,
    const std::vector<std::string>& description,
    const std::string& longName,
    const std::string& ns,
    const std::string& declSpec,
    const std::string& version,
    const std::vector<ModuleConstSP>& modules,
    const std::vector<ClassConstSP>& importedBaseClasses,
    const std::vector<EnumConstSP>& importedEnums,
    bool sharedService)
    :
    spi::Object(true),
    name(name),
    description(description),
    longName(longName),
    ns(ns),
    declSpec(declSpec),
    version(version),
    modules(modules),
    importedBaseClasses(importedBaseClasses),
    importedEnums(importedEnums),
    sharedService(sharedService)
{}

std::vector<std::string> Service::Summary(
    bool sort) const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Service.Summary");
  try
  {
    ServiceConstSP self(this);
    const std::vector<std::string>& i_result = Service_Helper::Summary(self,
        sort);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Service.Summary", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Service.Summary"); }
}

std::vector<std::string> Service_Helper::Summary(
    const ServiceConstSP& in_self,
    bool sort)
{
    const Service* self = in_self.get();

    std::vector<std::string> summary;
    size_t numModules = self->modules.size();
    std::ostringstream oss;
    oss << self->name << " namespace = " << self->ns << " version = " << self->version << ";";
    summary.push_back(oss.str());
    const std::vector<std::string>& moduleSummaries = Module::combineSummaries(self->modules, sort);
    summary.insert(summary.end(), moduleSummaries.begin(), moduleSummaries.end());
    return summary;
}

std::vector<std::string> Service::combineSummaries(
    const std::vector<ServiceConstSP>& services,
    bool sort)
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Service.combineSummaries");
  try
  {
    const std::vector<std::string>& i_result = Service_Helper::combineSummaries(
        services, sort);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Service.combineSummaries", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Service.combineSummaries"); }
}

std::vector<std::string> Service_Helper::combineSummaries(
    const std::vector<ServiceConstSP>& services,
    bool sort)
{

    SPI_PRE_CONDITION(services.size() > 0);

    size_t numServices = services.size();
    if (numServices == 1)
        return services[0]->Summary(sort);

    SPI_PRE_CONDITION(!services[0]->sharedService);

    std::vector<std::string> summary;
    std::ostringstream oss;
    oss << services[0]->name << " namespace = " << services[0]->ns << " version = " << services[0]->version << ";";
    summary.push_back(oss.str());

    std::vector<ModuleConstSP> modules = services[0]->modules;
    for (size_t i = 1; i < numServices; ++i)
    {
        if (!services[i]->sharedService)
            SPI_THROW_RUNTIME_ERROR("Second and subsequent services must be shared services");
        if (services[i]->ns != services[0]->ns)
            SPI_THROW_RUNTIME_ERROR("Services must have the same namespace");

        const std::vector<ModuleConstSP>& modules_i = services[i]->modules;
        modules.insert(modules.end(), modules_i.begin(), modules_i.end());
    }

    const std::vector<std::string>& moduleSummaries = Module::combineSummaries(modules, sort);
    summary.insert(summary.end(), moduleSummaries.begin(), moduleSummaries.end());
    return summary;
}

/*
****************************************************************************
* Returns a sorted list of enumerated type names.
****************************************************************************
*/

std::vector<std::string> Service::getEnums() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Service.getEnums");
  try
  {
    ServiceConstSP self(this);
    const std::vector<std::string>& i_result = Service_Helper::getEnums(self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Service.getEnums", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Service.getEnums"); }
}

std::vector<std::string> Service_Helper::getEnums(
    const ServiceConstSP& in_self)
{
    const Service* self = in_self.get();

    self->buildIndexEnums();
    std::vector<std::string> names;
    std::map<std::string, EnumConstSP>::const_iterator iter;
    for (iter = self->indexEnums.begin(); iter != self->indexEnums.end();
         ++iter)
    {
        names.push_back(iter->first);
    }
    return names;
}

/*
****************************************************************************
* Get an individual Enum by name.
****************************************************************************
*/

EnumConstSP Service::getEnum(
    const std::string& name) const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Service.getEnum");
  try
  {
    ServiceConstSP self(this);
    const EnumConstSP& i_result = Service_Helper::getEnum(self, name);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Service.getEnum", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Service.getEnum"); }
}

EnumConstSP Service_Helper::getEnum(
    const ServiceConstSP& in_self,
    const std::string& name)
{
    const Service* self = in_self.get();

    self->buildIndexEnums();
    std::map<std::string, EnumConstSP>::const_iterator iter;
    iter = self->indexEnums.find(name);
    if (iter == self->indexEnums.end())
        throw spi::RuntimeError("Cannot find enum '%s'", name.c_str());
    return iter->second;
}

/*
****************************************************************************
* Returns the enumerands for a given enumerated type.
****************************************************************************
*/

std::vector<std::string> Service::getEnumerands(
    const std::string& name) const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Service.getEnumerands");
  try
  {
    ServiceConstSP self(this);
    const std::vector<std::string>& i_result = Service_Helper::getEnumerands(
        self, name);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Service.getEnumerands", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Service.getEnumerands"); }
}

std::vector<std::string> Service_Helper::getEnumerands(
    const ServiceConstSP& in_self,
    const std::string& name)
{
    const Service* self = in_self.get();

    self->buildIndexEnums();

    std::map<std::string, EnumConstSP>::const_iterator iter
        = self->indexEnums.find(name);

    if (iter == self->indexEnums.end())
        throw spi::RuntimeError("%s is not an enumerated type",
                                name.c_str());

    std::vector<std::string> enumerands;
    for(size_t i = 0; i < iter->second->enumerands.size(); ++i)
        enumerands.push_back(iter->second->enumerands[i]->code);

    return enumerands;
}

/*
****************************************************************************
* Returns a sorted list of class names.
****************************************************************************
*/

std::vector<std::string> Service::getClasses() const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Service.getClasses");
  try
  {
    ServiceConstSP self(this);
    const std::vector<std::string>& i_result = Service_Helper::getClasses(
        self);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Service.getClasses", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Service.getClasses"); }
}

std::vector<std::string> Service_Helper::getClasses(
    const ServiceConstSP& in_self)
{
    const Service* self = in_self.get();

    self->buildIndexClasses();
    std::vector<std::string> names;
    std::map<std::string, ClassConstSP>::const_iterator iter;
    for (iter = self->indexClasses.begin();
         iter != self->indexClasses.end(); ++iter)
    {
        names.push_back(iter->first);
    }
    return names;
}

/*
****************************************************************************
* Returns the class details for a class name.
****************************************************************************
*/

ClassConstSP Service::getClass(
    const std::string& className) const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Service.getClass");
  try
  {
    ServiceConstSP self(this);
    const ClassConstSP& i_result = Service_Helper::getClass(self, className);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Service.getClass", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Service.getClass"); }
}

ClassConstSP Service_Helper::getClass(
    const ServiceConstSP& in_self,
    const std::string& className)
{
    const Service* self = in_self.get();

    self->buildIndexClasses();
    std::map<std::string, ClassConstSP>::const_iterator iter;
    iter = self->indexClasses.find(className);
    if (iter == self->indexClasses.end())
        throw spi::RuntimeError("Cannot find class '%s'",
            className.c_str());
    return iter->second;
}

/*
****************************************************************************
* Returns whether a given class is a sub-class of the data type of the given name.
* Needs to be a method on the Service since otherwise we cannot find base class.
****************************************************************************
*/

bool Service::isSubClass(
    const ClassConstSP& cls,
    const std::string& name) const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Service.isSubClass");
  try
  {
    ServiceConstSP self(this);
    bool i_result = Service_Helper::isSubClass(self, cls, name);
    clear_public_map();

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Service.isSubClass", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Service.isSubClass"); }
}

bool Service_Helper::isSubClass(
    const ServiceConstSP& in_self,
    const ClassConstSP& cls,
    const std::string& name)
{
    const Service* self = in_self.get();

    self->buildIndexClasses(); // this includes imported base classes

    if (cls->getName() == name)
        return true; // trivially the case

    std::string baseClassName = cls->baseClassName;
    while (!baseClassName.empty())
    {
        ClassConstSP c = self->getClass(baseClassName);
        if (c->getName() == name)
            return true;
        baseClassName = c->baseClassName;
    }
    return false;
}

/*
****************************************************************************
* Returns the name of the class for which the given fieldName is a property.
* If no such class exists then returns an empty string.
****************************************************************************
*/

std::string Service::getPropertyClass(
    const std::string& baseClassName,
    const std::string& fieldName) const
{
  bool isLogging = spdoc_begin_function();
  SPI_PROFILE("spdoc.Service.getPropertyClass");
  try
  {
    ServiceConstSP self(this);
    const std::string& i_result = Service_Helper::getPropertyClass(self,
        baseClassName, fieldName);

    spdoc_end_function();

    return i_result;
  }
  catch (std::exception& e)
  { throw spdoc_catch_exception(isLogging, "Service.getPropertyClass", e); }
  catch (...)
  { throw spdoc_catch_all(isLogging, "Service.getPropertyClass"); }
}

std::string Service_Helper::getPropertyClass(
    const ServiceConstSP& in_self,
    const std::string& baseClassName,
    const std::string& fieldName)
{
    const Service* self = in_self.get();

    if (baseClassName.empty())
        return std::string();

    ClassConstSP baseClass = self->getClass(baseClassName);
    
    const std::vector<ClassAttributeConstSP>& properties = baseClass->properties;
    size_t N = properties.size();
    for (size_t i = 0; i < N; ++i)
    {
        if (properties[i]->name == fieldName)
            return baseClassName;
    }

    if (baseClass->baseClassName.empty())
        return std::string();

    return self->getPropertyClass(baseClass->baseClassName, fieldName);
}

void Service::buildIndexEnums() const
{
    if (indexEnums.empty())
    {
        size_t N = modules.size();
        for (size_t i = 0; i < N; ++i)
        {
            const ModuleConstSP& module = modules[i];
            for (size_t j = 0; j < module->constructs.size(); ++j)
            {
                const ConstructConstSP& construct = module->constructs[j];
                if (construct->getType() == "ENUM")
                {
                    EnumConstSP type = Enum::Coerce(construct);
                    if (type)
                        indexEnums[type->name] = type;
                }
            }
        }
        N = importedEnums.size();
        for (size_t i = 0; i < N; ++i)
        {
            indexEnums[importedEnums[i]->name] = importedEnums[i];
        }
    }
}

void Service::buildIndexClasses() const
{
    if (indexClasses.empty())
    {
        for (size_t i = 0; i < importedBaseClasses.size(); ++i)
        {
            ClassConstSP cls = importedBaseClasses[i];
            indexClasses[cls->dataType->name] = cls;
        }
        for (size_t i = 0; i < modules.size(); ++i)
        {
            const ModuleConstSP& module = modules[i];
            for (size_t j = 0; j < module->constructs.size(); ++j)
            {
                const ConstructConstSP& construct = module->constructs[j];
                if (construct->getType() == "CLASS")
                {
                    ClassConstSP cls = Class::Coerce(construct);
                    if (cls)
                        // dataType->name includes the namespace
                        indexClasses[cls->dataType->name] = cls;
                }
            }
        }
    }
}

SPDOC_END_NAMESPACE

