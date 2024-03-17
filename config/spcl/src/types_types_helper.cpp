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

/**
****************************************************************************
* Source file: types_types_helper.cpp
****************************************************************************
*/

/*
****************************************************************************
* Generated code - do not edit
****************************************************************************
*/

#include "types_types.hpp"
#include "types_types_helper.hpp"
#include "types_dll_service_manager.hpp"

TYPES_BEGIN_NAMESPACE

spi::EnumInfo* PublicType::get_enum_info()
{
    static spi::EnumInfo the_info;
    if (!the_info.Initialised())
    {
        std::vector<std::string> enumerands;
        enumerands.push_back("BOOL");
        enumerands.push_back("CHAR");
        enumerands.push_back("INT");
        enumerands.push_back("DOUBLE");
        enumerands.push_back("STRING");
        enumerands.push_back("DATE");
        enumerands.push_back("DATETIME");
        enumerands.push_back("ENUM");
        enumerands.push_back("CLASS");
        enumerands.push_back("OBJECT");
        enumerands.push_back("MAP");
        enumerands.push_back("VARIANT");
        the_info.Initialise("PublicType", enumerands);
    }
    return &the_info;
}

PublicType::Enum PublicType::from_string(const char* str)
{
    std::string uc_ = spi::StringUpper(str);
    switch(uc_[0])
    {
    case 'B':
        if (uc_ == "BOOL")
            return PublicType::BOOL;
        break;
    case 'C':
        if (uc_ == "CHAR")
            return PublicType::CHAR;
        if (uc_ == "CLASS")
            return PublicType::CLASS;
        break;
    case 'D':
        if (uc_ == "DATE")
            return PublicType::DATE;
        if (uc_ == "DATETIME")
            return PublicType::DATETIME;
        if (uc_ == "DOUBLE")
            return PublicType::DOUBLE;
        break;
    case 'E':
        if (uc_ == "ENUM")
            return PublicType::ENUM;
        break;
    case 'I':
        if (uc_ == "INT")
            return PublicType::INT;
        break;
    case 'M':
        if (uc_ == "MAP")
            return PublicType::MAP;
        break;
    case 'O':
        if (uc_ == "OBJECT")
            return PublicType::OBJECT;
        break;
    case 'S':
        if (uc_ == "STRING")
            return PublicType::STRING;
        break;
    case 'V':
        if (uc_ == "VARIANT")
            return PublicType::VARIANT;
        break;
    }
    throw spi::RuntimeError("Cannot convert '%s' to PublicType. Possible values:\n"
       "'BOOL', 'CHAR', 'INT', 'DOUBLE', 'STRING', 'DATE', 'DATETIME', 'ENUM', 'CLASS', 'OBJECT', 'MAP', 'VARIANT'",
        str);
}

const char* PublicType::to_string(PublicType::Enum v_)
{
    switch(v_)
    {
    case PublicType::BOOL:
        return "BOOL";
    case PublicType::CHAR:
        return "CHAR";
    case PublicType::INT:
        return "INT";
    case PublicType::DOUBLE:
        return "DOUBLE";
    case PublicType::STRING:
        return "STRING";
    case PublicType::DATE:
        return "DATE";
    case PublicType::DATETIME:
        return "DATETIME";
    case PublicType::ENUM:
        return "ENUM";
    case PublicType::CLASS:
        return "CLASS";
    case PublicType::OBJECT:
        return "OBJECT";
    case PublicType::MAP:
        return "MAP";
    case PublicType::VARIANT:
        return "VARIANT";
    case PublicType::UNINITIALIZED_VALUE:
        throw std::runtime_error("Uninitialized value for PublicType");
    }
    throw std::runtime_error("Bad enumerated value");
}

/*
****************************************************************************
* Implementation of InputConverter
****************************************************************************
*/

InputConverter::outer_type
InputConverter::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return InputConverter::outer_type();

    // isInstance
    const InputConverter* p = dynamic_cast<const InputConverter*>(o.get());
    if (p)
        return InputConverter::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const InputConverter*>(o2.get());
            if (p)
                return InputConverter::outer_type(p);
        }
        else
        {
            o2 = o;
        }
    }
    else
    {
        o2 = o;
    }

    // coerceTo if the input object has a different name
    if (strcmp(ot->get_class_name(), o2->get_class_name()) != 0)
    {
        spi::ObjectConstSP o3 = o2->coerce_to_object(ot->get_class_name());
        if (o3)
        {
            p = dynamic_cast<const InputConverter*>(o3.get());
            if (p)
                return InputConverter::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

spi::ObjectConstSP InputConverter::coerce_from_value(
    const spi::Value&  value,
    const spi::InputContext* context)
{
    try {
        std::string format = context->ValueToString(value);
        InputConverterConstSP o = InputConverter::Coerce(format);
        return spi::ObjectConstSP(o.get());
    } catch (...) {}

    return spi::ObjectConstSP();
}

void InputConverter::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    if (has_constructor())
    {
        constructor_to_map(obj_map);
        return;
    }
    throw spi::RuntimeError(
        "Cannot convert abstract class with unknown constructor to map");
}

spi::ObjectConstSP InputConverter::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    spi::ObjectConstSP obj = spi::Object::constructor_from_map(
        obj_map, value_to_object);
    if (obj)
        return obj;
    throw spi::RuntimeError(
        "Cannot create abstract class without constructor");
}

SPI_IMPLEMENT_OBJECT_TYPE(InputConverter, "InputConverter", types_service, false, InputConverter::coerce_from_value);

std::vector<InputConverter::sub_class_wrapper*> InputConverter::g_sub_class_wrappers;

/*
****************************************************************************
* Implementation of InputConverterStringFormat
****************************************************************************
*/

InputConverterStringFormat::outer_type
InputConverterStringFormat::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return InputConverterStringFormat::outer_type();

    // isInstance
    const InputConverterStringFormat* p = dynamic_cast<const InputConverterStringFormat*>(o.get());
    if (p)
        return InputConverterStringFormat::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const InputConverterStringFormat*>(o2.get());
            if (p)
                return InputConverterStringFormat::outer_type(p);
        }
        else
        {
            o2 = o;
        }
    }
    else
    {
        o2 = o;
    }

    // coerceTo if the input object has a different name
    if (strcmp(ot->get_class_name(), o2->get_class_name()) != 0)
    {
        spi::ObjectConstSP o3 = o2->coerce_to_object(ot->get_class_name());
        if (o3)
        {
            p = dynamic_cast<const InputConverterStringFormat*>(o3.get());
            if (p)
                return InputConverterStringFormat::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void InputConverterStringFormat::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    const std::string& format = this->format();

    obj_map->SetString("format", format);
}

spi::ObjectConstSP InputConverterStringFormat::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::string& format
        = obj_map->GetString("format");

    return InputConverterStringFormat::Make(format);
}

SPI_IMPLEMENT_OBJECT_TYPE(InputConverterStringFormat, "InputConverterStringFormat", types_service, false, 0);

spi::Value InputConverterStringFormat_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& format =
        in_context->ValueToString(in_values[0]);

    const InputConverterStringFormatConstSP& o_result = types::InputConverterStringFormat::Make(
        format);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller InputConverterStringFormat_FunctionCaller = {
    "InputConverterStringFormat",
    1,
    {
        {"format", spi::ArgType::STRING, "string", false, false, false}
    },
    InputConverterStringFormat_caller
};

/*
****************************************************************************
* Implementation of InputConverterClass
****************************************************************************
*/

InputConverterClass::outer_type
InputConverterClass::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return InputConverterClass::outer_type();

    // isInstance
    const InputConverterClass* p = dynamic_cast<const InputConverterClass*>(o.get());
    if (p)
        return InputConverterClass::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const InputConverterClass*>(o2.get());
            if (p)
                return InputConverterClass::outer_type(p);
        }
        else
        {
            o2 = o;
        }
    }
    else
    {
        o2 = o;
    }

    // coerceTo if the input object has a different name
    if (strcmp(ot->get_class_name(), o2->get_class_name()) != 0)
    {
        spi::ObjectConstSP o3 = o2->coerce_to_object(ot->get_class_name());
        if (o3)
        {
            p = dynamic_cast<const InputConverterClass*>(o3.get());
            if (p)
                return InputConverterClass::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void InputConverterClass::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    const std::string& format = this->format();

    obj_map->SetString("format", format);
}

spi::ObjectConstSP InputConverterClass::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::string& format
        = obj_map->GetString("format");

    return InputConverterClass::Make(format);
}

SPI_IMPLEMENT_OBJECT_TYPE(InputConverterClass, "InputConverterClass", types_service, false, 0);

spi::Value InputConverterClass_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& format =
        in_context->ValueToString(in_values[0]);

    const InputConverterClassConstSP& o_result = types::InputConverterClass::Make(
        format);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller InputConverterClass_FunctionCaller = {
    "InputConverterClass",
    1,
    {
        {"format", spi::ArgType::STRING, "string", false, false, false}
    },
    InputConverterClass_caller
};

/*
****************************************************************************
* Implementation of DataType
****************************************************************************
*/

DataType::outer_type
DataType::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return DataType::outer_type();

    // isInstance
    const DataType* p = dynamic_cast<const DataType*>(o.get());
    if (p)
        return DataType::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const DataType*>(o2.get());
            if (p)
                return DataType::outer_type(p);
        }
        else
        {
            o2 = o;
        }
    }
    else
    {
        o2 = o;
    }

    // coerceTo if the input object has a different name
    if (strcmp(ot->get_class_name(), o2->get_class_name()) != 0)
    {
        spi::ObjectConstSP o3 = o2->coerce_to_object(ot->get_class_name());
        if (o3)
        {
            p = dynamic_cast<const DataType*>(o3.get());
            if (p)
                return DataType::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void DataType::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    const std::string& name = this->name();
    const std::string& nsService = this->nsService();
    const std::string& cppName = this->cppName();
    const std::string& outerType = this->outerType();
    const std::string& innerType = this->innerType();
    const std::string& innerRefType = this->innerRefType();
    PublicType publicType = this->publicType();
    const std::string& objectName = this->objectName();
    bool isClosed = this->isClosed();
    bool noDoc = this->noDoc();
    const InputConverterConstSP& convertIn = this->convertIn();
    const std::string& convertOut = this->convertOut();
    const std::string& copyInner = this->copyInner();
    bool ignored = this->ignored();

    if (!public_only)
    {
        obj_map->SetString("name", name);
        obj_map->SetString("nsService", nsService, !public_only && (nsService == std::string("")));
        obj_map->SetString("cppName", cppName);
        obj_map->SetString("outerType", outerType);
        obj_map->SetString("innerType", innerType, !public_only && (innerType == std::string("")));
        obj_map->SetString("innerRefType", innerRefType, !public_only && (innerRefType == std::string("")));
        obj_map->SetString("publicType", publicType);
        obj_map->SetString("objectName", objectName, !public_only && (objectName == std::string("")));
        obj_map->SetBool("isClosed", isClosed);
        obj_map->SetBool("noDoc", noDoc);
        obj_map->SetObject("convertIn", convertIn, !public_only && (!convertIn));
        obj_map->SetString("convertOut", convertOut, !public_only && (convertOut == std::string("")));
        obj_map->SetString("copyInner", copyInner, !public_only && (copyInner == std::string("")));
        obj_map->SetBool("ignored", ignored, !public_only && (ignored == false));
    }
}

spi::ObjectConstSP DataType::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::string& name
        = obj_map->GetString("name");
    const std::string& nsService
        = obj_map->GetString("nsService", true, "");
    const std::string& cppName
        = obj_map->GetString("cppName");
    const std::string& outerType
        = obj_map->GetString("outerType");
    const std::string& innerType
        = obj_map->GetString("innerType", true, "");
    const std::string& innerRefType
        = obj_map->GetString("innerRefType", true, "");
    PublicType publicType
        = obj_map->GetString("publicType");
    const std::string& objectName
        = obj_map->GetString("objectName", true, "");
    bool isClosed
        = obj_map->GetBool("isClosed");
    bool noDoc
        = obj_map->GetBool("noDoc");
    const InputConverterConstSP& convertIn
        = obj_map->GetInstance<InputConverter const>("convertIn", value_to_object, true);
    const std::string& convertOut
        = obj_map->GetString("convertOut", true, "");
    const std::string& copyInner
        = obj_map->GetString("copyInner", true, "");
    bool ignored
        = obj_map->GetBool("ignored", true, false);

    return DataType::Make(name, nsService, cppName, outerType, innerType,
        innerRefType, publicType, objectName, isClosed, noDoc, convertIn,
        convertOut, copyInner, ignored);
}

SPI_IMPLEMENT_OBJECT_TYPE(DataType, "DataType", types_service, false, 0);

spi::Value DataType_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& name =
        in_context->ValueToString(in_values[0]);
    const std::string& nsService =
        in_context->ValueToString(in_values[1], true, "");
    const std::string& cppName =
        in_context->ValueToString(in_values[2]);
    const std::string& outerType =
        in_context->ValueToString(in_values[3]);
    const std::string& innerType =
        in_context->ValueToString(in_values[4], true, "");
    const std::string& innerRefType =
        in_context->ValueToString(in_values[5], true, "");
    PublicType publicType =
        in_context->ValueToString(in_values[6]);
    const std::string& objectName =
        in_context->ValueToString(in_values[7], true, "");
    bool isClosed =
        in_context->ValueToBool(in_values[8]);
    bool noDoc =
        in_context->ValueToBool(in_values[9]);
    const InputConverterConstSP& convertIn =
        in_context->ValueToInstance<InputConverter const>(in_values[10], true);
    const std::string& convertOut =
        in_context->ValueToString(in_values[11], true, "");
    const std::string& copyInner =
        in_context->ValueToString(in_values[12], true, "");
    bool ignored =
        in_context->ValueToBool(in_values[13], true, false);

    const DataTypeConstSP& o_result = types::DataType::Make(name, nsService,
        cppName, outerType, innerType, innerRefType, publicType, objectName,
        isClosed, noDoc, convertIn, convertOut, copyInner, ignored);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller DataType_FunctionCaller = {
    "DataType",
    14,
    {
        {"name", spi::ArgType::STRING, "string", false, false, false},
        {"nsService", spi::ArgType::STRING, "string", false, true, false},
        {"cppName", spi::ArgType::STRING, "string", false, false, false},
        {"outerType", spi::ArgType::STRING, "string", false, false, false},
        {"innerType", spi::ArgType::STRING, "string", false, true, false},
        {"innerRefType", spi::ArgType::STRING, "string", false, true, false},
        {"publicType", spi::ArgType::ENUM, "PublicType", false, false, false},
        {"objectName", spi::ArgType::STRING, "string", false, true, false},
        {"isClosed", spi::ArgType::BOOL, "bool", false, false, false},
        {"noDoc", spi::ArgType::BOOL, "bool", false, false, false},
        {"convertIn", spi::ArgType::OBJECT, "InputConverter", false, true, false},
        {"convertOut", spi::ArgType::STRING, "string", false, true, false},
        {"copyInner", spi::ArgType::STRING, "string", false, true, false},
        {"ignored", spi::ArgType::BOOL, "bool", false, true, false}
    },
    DataType_caller
};

/*
****************************************************************************
* Implementation of Attribute
****************************************************************************
*/

Attribute::outer_type
Attribute::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return Attribute::outer_type();

    // isInstance
    const Attribute* p = dynamic_cast<const Attribute*>(o.get());
    if (p)
        return Attribute::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const Attribute*>(o2.get());
            if (p)
                return Attribute::outer_type(p);
        }
        else
        {
            o2 = o;
        }
    }
    else
    {
        o2 = o;
    }

    // coerceTo if the input object has a different name
    if (strcmp(ot->get_class_name(), o2->get_class_name()) != 0)
    {
        spi::ObjectConstSP o3 = o2->coerce_to_object(ot->get_class_name());
        if (o3)
        {
            p = dynamic_cast<const Attribute*>(o3.get());
            if (p)
                return Attribute::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void Attribute::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    const DataTypeConstSP& dataType = this->dataType();
    bool innerConst = this->innerConst();
    const std::string& name = this->name();
    int arrayDim = this->arrayDim();

    if (!public_only)
    {
        obj_map->SetObject("dataType", dataType);
        obj_map->SetBool("innerConst", innerConst, !public_only && (innerConst == false));
        obj_map->SetString("name", name);
        obj_map->SetInt("arrayDim", arrayDim, !public_only && (arrayDim == 0));
    }
}

spi::ObjectConstSP Attribute::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const DataTypeConstSP& dataType
        = obj_map->GetInstance<DataType const>("dataType", value_to_object);
    bool innerConst
        = obj_map->GetBool("innerConst", true, false);
    const std::string& name
        = obj_map->GetString("name");
    int arrayDim
        = obj_map->GetInt("arrayDim", true, 0);

    return Attribute::Make(dataType, innerConst, name, arrayDim);
}

SPI_IMPLEMENT_OBJECT_TYPE(Attribute, "Attribute", types_service, false, 0);

spi::Value Attribute_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const DataTypeConstSP& dataType =
        in_context->ValueToInstance<DataType const>(in_values[0]);
    bool innerConst =
        in_context->ValueToBool(in_values[1], true, false);
    const std::string& name =
        in_context->ValueToString(in_values[2]);
    int arrayDim =
        in_context->ValueToInt(in_values[3], true, 0);

    const AttributeConstSP& o_result = types::Attribute::Make(dataType,
        innerConst, name, arrayDim);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller Attribute_FunctionCaller = {
    "Attribute",
    4,
    {
        {"dataType", spi::ArgType::OBJECT, "DataType", false, false, false},
        {"innerConst", spi::ArgType::BOOL, "bool", false, true, false},
        {"name", spi::ArgType::STRING, "string", false, false, false},
        {"arrayDim", spi::ArgType::INT, "int", false, true, false}
    },
    Attribute_caller
};

/*
****************************************************************************
* Implementation of ClassProperty
****************************************************************************
*/

ClassProperty::outer_type
ClassProperty::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return ClassProperty::outer_type();

    // isInstance
    const ClassProperty* p = dynamic_cast<const ClassProperty*>(o.get());
    if (p)
        return ClassProperty::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const ClassProperty*>(o2.get());
            if (p)
                return ClassProperty::outer_type(p);
        }
        else
        {
            o2 = o;
        }
    }
    else
    {
        o2 = o;
    }

    // coerceTo if the input object has a different name
    if (strcmp(ot->get_class_name(), o2->get_class_name()) != 0)
    {
        spi::ObjectConstSP o3 = o2->coerce_to_object(ot->get_class_name());
        if (o3)
        {
            p = dynamic_cast<const ClassProperty*>(o3.get());
            if (p)
                return ClassProperty::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void ClassProperty::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    const AttributeConstSP& attribute = this->attribute();

    if (!public_only)
    {
        obj_map->SetObject("attribute", attribute);
    }
}

spi::ObjectConstSP ClassProperty::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const AttributeConstSP& attribute
        = obj_map->GetInstance<Attribute const>("attribute", value_to_object);

    return ClassProperty::Make(attribute);
}

SPI_IMPLEMENT_OBJECT_TYPE(ClassProperty, "ClassProperty", types_service, false, 0);

spi::Value ClassProperty_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const AttributeConstSP& attribute =
        in_context->ValueToInstance<Attribute const>(in_values[0]);

    const ClassPropertyConstSP& o_result = types::ClassProperty::Make(
        attribute);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller ClassProperty_FunctionCaller = {
    "ClassProperty",
    1,
    {
        {"attribute", spi::ArgType::OBJECT, "Attribute", false, false, false}
    },
    ClassProperty_caller
};

/*
****************************************************************************
* Implementation of Enumerand
****************************************************************************
*/

Enumerand::outer_type
Enumerand::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return Enumerand::outer_type();

    // isInstance
    const Enumerand* p = dynamic_cast<const Enumerand*>(o.get());
    if (p)
        return Enumerand::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const Enumerand*>(o2.get());
            if (p)
                return Enumerand::outer_type(p);
        }
        else
        {
            o2 = o;
        }
    }
    else
    {
        o2 = o;
    }

    // coerceTo if the input object has a different name
    if (strcmp(ot->get_class_name(), o2->get_class_name()) != 0)
    {
        spi::ObjectConstSP o3 = o2->coerce_to_object(ot->get_class_name());
        if (o3)
        {
            p = dynamic_cast<const Enumerand*>(o3.get());
            if (p)
                return Enumerand::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void Enumerand::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    const std::string& code = this->code();
    const std::vector<std::string>& strings = this->strings();

    obj_map->SetString("code", code);
    obj_map->SetStringVector("strings", strings);
}

spi::ObjectConstSP Enumerand::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::string& code
        = obj_map->GetString("code");
    const std::vector<std::string>& strings
        = obj_map->GetStringVector("strings");

    return Enumerand::Make(code, strings);
}

SPI_IMPLEMENT_OBJECT_TYPE(Enumerand, "Enumerand", types_service, false, 0);

spi::Value Enumerand_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& code =
        in_context->ValueToString(in_values[0]);
    std::vector<std::string> strings =
        in_context->ValueToStringVector(in_values[1]);

    const EnumerandConstSP& o_result = types::Enumerand::Make(code, strings);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller Enumerand_FunctionCaller = {
    "Enumerand",
    2,
    {
        {"code", spi::ArgType::STRING, "string", false, false, false},
        {"strings", spi::ArgType::STRING, "string", true, false, false}
    },
    Enumerand_caller
};

/*
****************************************************************************
* Implementation of Enum
****************************************************************************
*/

Enum::outer_type
Enum::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return Enum::outer_type();

    // isInstance
    const Enum* p = dynamic_cast<const Enum*>(o.get());
    if (p)
        return Enum::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const Enum*>(o2.get());
            if (p)
                return Enum::outer_type(p);
        }
        else
        {
            o2 = o;
        }
    }
    else
    {
        o2 = o;
    }

    // coerceTo if the input object has a different name
    if (strcmp(ot->get_class_name(), o2->get_class_name()) != 0)
    {
        spi::ObjectConstSP o3 = o2->coerce_to_object(ot->get_class_name());
        if (o3)
        {
            p = dynamic_cast<const Enum*>(o3.get());
            if (p)
                return Enum::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void Enum::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    const std::string& name = this->name();
    const std::vector<EnumerandConstSP>& enumerands = this->enumerands();

    obj_map->SetString("name", name);
    obj_map->SetInstanceVector<Enumerand const>("enumerands", enumerands);
}

spi::ObjectConstSP Enum::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::string& name
        = obj_map->GetString("name");
    const std::vector<EnumerandConstSP>& enumerands
        = obj_map->GetInstanceVector<Enumerand const>("enumerands", value_to_object);

    return Enum::Make(name, enumerands);
}

SPI_IMPLEMENT_OBJECT_TYPE(Enum, "Enum", types_service, false, 0);

spi::Value Enum_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& name =
        in_context->ValueToString(in_values[0]);
    std::vector<EnumerandConstSP> enumerands =
        in_context->ValueToInstanceVector<Enumerand const>(in_values[1]);

    const EnumConstSP& o_result = types::Enum::Make(name, enumerands);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller Enum_FunctionCaller = {
    "Enum",
    2,
    {
        {"name", spi::ArgType::STRING, "string", false, false, false},
        {"enumerands", spi::ArgType::OBJECT, "Enumerand", true, false, false}
    },
    Enum_caller
};

/*
****************************************************************************
* Implementation of BaseClass
****************************************************************************
*/

BaseClass::outer_type
BaseClass::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return BaseClass::outer_type();

    // isInstance
    const BaseClass* p = dynamic_cast<const BaseClass*>(o.get());
    if (p)
        return BaseClass::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const BaseClass*>(o2.get());
            if (p)
                return BaseClass::outer_type(p);
        }
        else
        {
            o2 = o;
        }
    }
    else
    {
        o2 = o;
    }

    // coerceTo if the input object has a different name
    if (strcmp(ot->get_class_name(), o2->get_class_name()) != 0)
    {
        spi::ObjectConstSP o3 = o2->coerce_to_object(ot->get_class_name());
        if (o3)
        {
            p = dynamic_cast<const BaseClass*>(o3.get());
            if (p)
                return BaseClass::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void BaseClass::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    if (has_constructor())
    {
        constructor_to_map(obj_map);
        return;
    }
    throw spi::RuntimeError(
        "Cannot convert abstract class with unknown constructor to map");
}

spi::ObjectConstSP BaseClass::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    spi::ObjectConstSP obj = spi::Object::constructor_from_map(
        obj_map, value_to_object);
    if (obj)
        return obj;
    throw spi::RuntimeError(
        "Cannot create abstract class without constructor");
}

SPI_IMPLEMENT_OBJECT_TYPE(BaseClass, "BaseClass", types_service, false, 0);

std::vector<BaseClass::sub_class_wrapper*> BaseClass::g_sub_class_wrappers;

/*
****************************************************************************
* Implementation of BaseStruct
****************************************************************************
*/

BaseStruct::outer_type
BaseStruct::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return BaseStruct::outer_type();

    // isInstance
    const BaseStruct* p = dynamic_cast<const BaseStruct*>(o.get());
    if (p)
        return BaseStruct::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const BaseStruct*>(o2.get());
            if (p)
                return BaseStruct::outer_type(p);
        }
        else
        {
            o2 = o;
        }
    }
    else
    {
        o2 = o;
    }

    // coerceTo if the input object has a different name
    if (strcmp(ot->get_class_name(), o2->get_class_name()) != 0)
    {
        spi::ObjectConstSP o3 = o2->coerce_to_object(ot->get_class_name());
        if (o3)
        {
            p = dynamic_cast<const BaseStruct*>(o3.get());
            if (p)
                return BaseStruct::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void BaseStruct::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    const std::vector<std::string>& description = this->description();
    const std::string& name = this->name();
    const std::string& ns = this->ns();
    const BaseClassConstSP& baseClass = this->baseClass();
    bool noMake = this->noMake();
    const std::string& objectName = this->objectName();
    bool canPut = this->canPut();
    bool noId = this->noId();
    bool isVirtual = this->isVirtual();
    bool asValue = this->asValue();
    bool uuid = this->uuid();
    bool byValue = this->byValue();
    bool useAccessors = this->useAccessors();
    const std::string& funcPrefix = this->funcPrefix();
    const std::string& constructor = this->constructor();
    const std::string& instance = this->instance();

    if (!public_only)
    {
        obj_map->SetStringVector("description", description);
        obj_map->SetString("name", name);
        obj_map->SetString("ns", ns);
        obj_map->SetObject("baseClass", baseClass);
        obj_map->SetBool("noMake", noMake);
        obj_map->SetString("objectName", objectName);
        obj_map->SetBool("canPut", canPut);
        obj_map->SetBool("noId", noId);
        obj_map->SetBool("isVirtual", isVirtual);
        obj_map->SetBool("asValue", asValue, !public_only && (asValue == false));
        obj_map->SetBool("uuid", uuid, !public_only && (uuid == false));
        obj_map->SetBool("byValue", byValue, !public_only && (byValue == false));
        obj_map->SetBool("useAccessors", useAccessors, !public_only && (useAccessors == false));
        obj_map->SetString("funcPrefix", funcPrefix, !public_only && (funcPrefix == std::string("")));
        obj_map->SetString("constructor", constructor, !public_only && (constructor == std::string("")));
        obj_map->SetString("instance", instance, !public_only && (instance == std::string("")));
    }
}

spi::ObjectConstSP BaseStruct::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::vector<std::string>& description
        = obj_map->GetStringVector("description");
    const std::string& name
        = obj_map->GetString("name");
    const std::string& ns
        = obj_map->GetString("ns");
    const BaseClassConstSP& baseClass
        = obj_map->GetInstance<BaseClass const>("baseClass", value_to_object, true);
    bool noMake
        = obj_map->GetBool("noMake");
    const std::string& objectName
        = obj_map->GetString("objectName", true, "");
    bool canPut
        = obj_map->GetBool("canPut", true, false);
    bool noId
        = obj_map->GetBool("noId", true, false);
    bool isVirtual
        = obj_map->GetBool("isVirtual", true, false);
    bool asValue
        = obj_map->GetBool("asValue", true, false);
    bool uuid
        = obj_map->GetBool("uuid", true, false);
    bool byValue
        = obj_map->GetBool("byValue", true, false);
    bool useAccessors
        = obj_map->GetBool("useAccessors", true, false);
    const std::string& funcPrefix
        = obj_map->GetString("funcPrefix", true, "");
    const std::string& constructor
        = obj_map->GetString("constructor", true, "");
    const std::string& instance
        = obj_map->GetString("instance", true, "");

    return BaseStruct::Make(description, name, ns, baseClass, noMake,
        objectName, canPut, noId, isVirtual, asValue, uuid, byValue,
        useAccessors, funcPrefix, constructor, instance);
}

SPI_IMPLEMENT_OBJECT_TYPE(BaseStruct, "BaseStruct", types_service, false, 0);

spi::Value BaseStruct_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    std::vector<std::string> description =
        in_context->ValueToStringVector(in_values[0]);
    const std::string& name =
        in_context->ValueToString(in_values[1]);
    const std::string& ns =
        in_context->ValueToString(in_values[2]);
    const BaseClassConstSP& baseClass =
        in_context->ValueToInstance<BaseClass const>(in_values[3], true);
    bool noMake =
        in_context->ValueToBool(in_values[4]);
    const std::string& objectName =
        in_context->ValueToString(in_values[5], true, "");
    bool canPut =
        in_context->ValueToBool(in_values[6], true, false);
    bool noId =
        in_context->ValueToBool(in_values[7], true, false);
    bool isVirtual =
        in_context->ValueToBool(in_values[8], true, false);
    bool asValue =
        in_context->ValueToBool(in_values[9], true, false);
    bool uuid =
        in_context->ValueToBool(in_values[10], true, false);
    bool byValue =
        in_context->ValueToBool(in_values[11], true, false);
    bool useAccessors =
        in_context->ValueToBool(in_values[12], true, false);
    const std::string& funcPrefix =
        in_context->ValueToString(in_values[13], true, "");
    const std::string& constructor =
        in_context->ValueToString(in_values[14], true, "");
    const std::string& instance =
        in_context->ValueToString(in_values[15], true, "");

    const BaseStructConstSP& o_result = types::BaseStruct::Make(description,
        name, ns, baseClass, noMake, objectName, canPut, noId, isVirtual,
        asValue, uuid, byValue, useAccessors, funcPrefix, constructor,
        instance);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller BaseStruct_FunctionCaller = {
    "BaseStruct",
    16,
    {
        {"description", spi::ArgType::STRING, "string", true, false, false},
        {"name", spi::ArgType::STRING, "string", false, false, false},
        {"ns", spi::ArgType::STRING, "string", false, false, false},
        {"baseClass", spi::ArgType::OBJECT, "BaseClass", false, true, false},
        {"noMake", spi::ArgType::BOOL, "bool", false, false, false},
        {"objectName", spi::ArgType::STRING, "string", false, true, false},
        {"canPut", spi::ArgType::BOOL, "bool", false, true, false},
        {"noId", spi::ArgType::BOOL, "bool", false, true, false},
        {"isVirtual", spi::ArgType::BOOL, "bool", false, true, false},
        {"asValue", spi::ArgType::BOOL, "bool", false, true, false},
        {"uuid", spi::ArgType::BOOL, "bool", false, true, false},
        {"byValue", spi::ArgType::BOOL, "bool", false, true, false},
        {"useAccessors", spi::ArgType::BOOL, "bool", false, true, false},
        {"funcPrefix", spi::ArgType::STRING, "string", false, true, false},
        {"constructor", spi::ArgType::STRING, "string", false, true, false},
        {"instance", spi::ArgType::STRING, "string", false, true, false}
    },
    BaseStruct_caller
};

/*
****************************************************************************
* Implementation of InnerClass
****************************************************************************
*/

InnerClass::outer_type
InnerClass::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return InnerClass::outer_type();

    // isInstance
    const InnerClass* p = dynamic_cast<const InnerClass*>(o.get());
    if (p)
        return InnerClass::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const InnerClass*>(o2.get());
            if (p)
                return InnerClass::outer_type(p);
        }
        else
        {
            o2 = o;
        }
    }
    else
    {
        o2 = o;
    }

    // coerceTo if the input object has a different name
    if (strcmp(ot->get_class_name(), o2->get_class_name()) != 0)
    {
        spi::ObjectConstSP o3 = o2->coerce_to_object(ot->get_class_name());
        if (o3)
        {
            p = dynamic_cast<const InnerClass*>(o3.get());
            if (p)
                return InnerClass::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void InnerClass::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    const std::string& typeName = this->typeName();
    const std::string& ns = this->ns();
    const std::string& freeFunc = this->freeFunc();
    const std::string& copyFunc = this->copyFunc();
    const std::string& preDeclaration = this->preDeclaration();
    const std::string& sharedPtr = this->sharedPtr();
    bool isShared = this->isShared();
    bool isConst = this->isConst();
    bool isOpen = this->isOpen();
    bool isStruct = this->isStruct();
    bool isCached = this->isCached();
    bool isTemplate = this->isTemplate();
    bool byValue = this->byValue();
    const std::string& boolTest = this->boolTest();
    bool allowConst = this->allowConst();

    if (!public_only)
    {
        obj_map->SetString("typeName", typeName);
        obj_map->SetString("ns", ns);
        obj_map->SetString("freeFunc", freeFunc);
        obj_map->SetString("copyFunc", copyFunc);
        obj_map->SetString("preDeclaration", preDeclaration);
        obj_map->SetString("sharedPtr", sharedPtr);
        obj_map->SetBool("isShared", isShared);
        obj_map->SetBool("isConst", isConst);
        obj_map->SetBool("isOpen", isOpen);
        obj_map->SetBool("isStruct", isStruct);
        obj_map->SetBool("isCached", isCached);
        obj_map->SetBool("isTemplate", isTemplate);
        obj_map->SetBool("byValue", byValue);
        obj_map->SetString("boolTest", boolTest, !public_only && (boolTest == std::string("")));
        obj_map->SetBool("allowConst", allowConst, !public_only && (allowConst == false));
    }
}

spi::ObjectConstSP InnerClass::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::string& typeName
        = obj_map->GetString("typeName");
    const std::string& ns
        = obj_map->GetString("ns");
    const std::string& freeFunc
        = obj_map->GetString("freeFunc");
    const std::string& copyFunc
        = obj_map->GetString("copyFunc");
    const std::string& preDeclaration
        = obj_map->GetString("preDeclaration");
    const std::string& sharedPtr
        = obj_map->GetString("sharedPtr");
    bool isShared
        = obj_map->GetBool("isShared");
    bool isConst
        = obj_map->GetBool("isConst");
    bool isOpen
        = obj_map->GetBool("isOpen");
    bool isStruct
        = obj_map->GetBool("isStruct");
    bool isCached
        = obj_map->GetBool("isCached");
    bool isTemplate
        = obj_map->GetBool("isTemplate");
    bool byValue
        = obj_map->GetBool("byValue");
    const std::string& boolTest
        = obj_map->GetString("boolTest", true, "");
    bool allowConst
        = obj_map->GetBool("allowConst", true, false);

    return InnerClass::Make(typeName, ns, freeFunc, copyFunc, preDeclaration,
        sharedPtr, isShared, isConst, isOpen, isStruct, isCached, isTemplate,
        byValue, boolTest, allowConst);
}

SPI_IMPLEMENT_OBJECT_TYPE(InnerClass, "InnerClass", types_service, false, 0);

spi::Value InnerClass_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& typeName =
        in_context->ValueToString(in_values[0]);
    const std::string& ns =
        in_context->ValueToString(in_values[1]);
    const std::string& freeFunc =
        in_context->ValueToString(in_values[2]);
    const std::string& copyFunc =
        in_context->ValueToString(in_values[3]);
    const std::string& preDeclaration =
        in_context->ValueToString(in_values[4]);
    const std::string& sharedPtr =
        in_context->ValueToString(in_values[5]);
    bool isShared =
        in_context->ValueToBool(in_values[6]);
    bool isConst =
        in_context->ValueToBool(in_values[7]);
    bool isOpen =
        in_context->ValueToBool(in_values[8]);
    bool isStruct =
        in_context->ValueToBool(in_values[9]);
    bool isCached =
        in_context->ValueToBool(in_values[10]);
    bool isTemplate =
        in_context->ValueToBool(in_values[11]);
    bool byValue =
        in_context->ValueToBool(in_values[12]);
    const std::string& boolTest =
        in_context->ValueToString(in_values[13], true, "");
    bool allowConst =
        in_context->ValueToBool(in_values[14], true, false);

    const InnerClassConstSP& o_result = types::InnerClass::Make(typeName, ns,
        freeFunc, copyFunc, preDeclaration, sharedPtr, isShared, isConst,
        isOpen, isStruct, isCached, isTemplate, byValue, boolTest, allowConst);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller InnerClass_FunctionCaller = {
    "InnerClass",
    15,
    {
        {"typeName", spi::ArgType::STRING, "string", false, false, false},
        {"ns", spi::ArgType::STRING, "string", false, false, false},
        {"freeFunc", spi::ArgType::STRING, "string", false, false, false},
        {"copyFunc", spi::ArgType::STRING, "string", false, false, false},
        {"preDeclaration", spi::ArgType::STRING, "string", false, false, false},
        {"sharedPtr", spi::ArgType::STRING, "string", false, false, false},
        {"isShared", spi::ArgType::BOOL, "bool", false, false, false},
        {"isConst", spi::ArgType::BOOL, "bool", false, false, false},
        {"isOpen", spi::ArgType::BOOL, "bool", false, false, false},
        {"isStruct", spi::ArgType::BOOL, "bool", false, false, false},
        {"isCached", spi::ArgType::BOOL, "bool", false, false, false},
        {"isTemplate", spi::ArgType::BOOL, "bool", false, false, false},
        {"byValue", spi::ArgType::BOOL, "bool", false, false, false},
        {"boolTest", spi::ArgType::STRING, "string", false, true, false},
        {"allowConst", spi::ArgType::BOOL, "bool", false, true, false}
    },
    InnerClass_caller
};

/*
****************************************************************************
* Implementation of BaseWrapperClass
****************************************************************************
*/

BaseWrapperClass::outer_type
BaseWrapperClass::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return BaseWrapperClass::outer_type();

    // isInstance
    const BaseWrapperClass* p = dynamic_cast<const BaseWrapperClass*>(o.get());
    if (p)
        return BaseWrapperClass::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const BaseWrapperClass*>(o2.get());
            if (p)
                return BaseWrapperClass::outer_type(p);
        }
        else
        {
            o2 = o;
        }
    }
    else
    {
        o2 = o;
    }

    // coerceTo if the input object has a different name
    if (strcmp(ot->get_class_name(), o2->get_class_name()) != 0)
    {
        spi::ObjectConstSP o3 = o2->coerce_to_object(ot->get_class_name());
        if (o3)
        {
            p = dynamic_cast<const BaseWrapperClass*>(o3.get());
            if (p)
                return BaseWrapperClass::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void BaseWrapperClass::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    const std::vector<std::string>& description = this->description();
    const std::string& name = this->name();
    const std::string& ns = this->ns();
    const InnerClassConstSP& innerClass = this->innerClass();
    const BaseWrapperClassConstSP& baseClass = this->baseClass();
    bool isVirtual = this->isVirtual();
    bool noMake = this->noMake();
    const std::string& objectName = this->objectName();
    bool isDelegate = this->isDelegate();
    bool canPut = this->canPut();
    bool noId = this->noId();
    const DataTypeConstSP& dataType = this->dataType();
    bool asValue = this->asValue();
    const std::vector<ClassPropertyConstSP>& classProperties = this->classProperties();
    bool uuid = this->uuid();
    const std::string& funcPrefix = this->funcPrefix();
    const std::string& constructor = this->constructor();
    const std::string& instance = this->instance();

    if (!public_only)
    {
        obj_map->SetStringVector("description", description);
        obj_map->SetString("name", name);
        obj_map->SetString("ns", ns);
        obj_map->SetObject("innerClass", innerClass);
        obj_map->SetObject("baseClass", baseClass);
        obj_map->SetBool("isVirtual", isVirtual);
        obj_map->SetBool("noMake", noMake);
        obj_map->SetString("objectName", objectName);
        obj_map->SetBool("isDelegate", isDelegate);
        obj_map->SetBool("canPut", canPut);
        obj_map->SetBool("noId", noId);
        obj_map->SetObject("dataType", dataType);
        obj_map->SetBool("asValue", asValue, !public_only && (asValue == false));
        obj_map->SetInstanceVector<ClassProperty const>("classProperties", classProperties, !public_only && (classProperties.size() == 0));
        obj_map->SetBool("uuid", uuid, !public_only && (uuid == false));
        obj_map->SetString("funcPrefix", funcPrefix, !public_only && (funcPrefix == std::string("")));
        obj_map->SetString("constructor", constructor, !public_only && (constructor == std::string("")));
        obj_map->SetString("instance", instance, !public_only && (instance == std::string("")));
    }
}

spi::ObjectConstSP BaseWrapperClass::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::vector<std::string>& description
        = obj_map->GetStringVector("description");
    const std::string& name
        = obj_map->GetString("name");
    const std::string& ns
        = obj_map->GetString("ns");
    const InnerClassConstSP& innerClass
        = obj_map->GetInstance<InnerClass const>("innerClass", value_to_object);
    const BaseWrapperClassConstSP& baseClass
        = obj_map->GetInstance<BaseWrapperClass const>("baseClass", value_to_object, true);
    bool isVirtual
        = obj_map->GetBool("isVirtual");
    bool noMake
        = obj_map->GetBool("noMake");
    const std::string& objectName
        = obj_map->GetString("objectName", true, "");
    bool isDelegate
        = obj_map->GetBool("isDelegate");
    bool canPut
        = obj_map->GetBool("canPut", true, false);
    bool noId
        = obj_map->GetBool("noId", true, false);
    const DataTypeConstSP& dataType
        = obj_map->GetInstance<DataType const>("dataType", value_to_object);
    bool asValue
        = obj_map->GetBool("asValue", true, false);
    const std::vector<ClassPropertyConstSP>& classProperties
        = obj_map->GetInstanceVector<ClassProperty const>("classProperties", value_to_object);
    bool uuid
        = obj_map->GetBool("uuid", true, false);
    const std::string& funcPrefix
        = obj_map->GetString("funcPrefix", true, "");
    const std::string& constructor
        = obj_map->GetString("constructor", true, "");
    const std::string& instance
        = obj_map->GetString("instance", true, "");

    return BaseWrapperClass::Make(description, name, ns, innerClass, baseClass,
        isVirtual, noMake, objectName, isDelegate, canPut, noId, dataType,
        asValue, classProperties, uuid, funcPrefix, constructor, instance);
}

SPI_IMPLEMENT_OBJECT_TYPE(BaseWrapperClass, "BaseWrapperClass", types_service, false, 0);

spi::Value BaseWrapperClass_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    std::vector<std::string> description =
        in_context->ValueToStringVector(in_values[0]);
    const std::string& name =
        in_context->ValueToString(in_values[1]);
    const std::string& ns =
        in_context->ValueToString(in_values[2]);
    const InnerClassConstSP& innerClass =
        in_context->ValueToInstance<InnerClass const>(in_values[3]);
    const BaseWrapperClassConstSP& baseClass =
        in_context->ValueToInstance<BaseWrapperClass const>(in_values[4], true);
    bool isVirtual =
        in_context->ValueToBool(in_values[5]);
    bool noMake =
        in_context->ValueToBool(in_values[6]);
    const std::string& objectName =
        in_context->ValueToString(in_values[7], true, "");
    bool isDelegate =
        in_context->ValueToBool(in_values[8]);
    bool canPut =
        in_context->ValueToBool(in_values[9], true, false);
    bool noId =
        in_context->ValueToBool(in_values[10], true, false);
    const DataTypeConstSP& dataType =
        in_context->ValueToInstance<DataType const>(in_values[11]);
    bool asValue =
        in_context->ValueToBool(in_values[12], true, false);
    std::vector<ClassPropertyConstSP> classProperties =
        in_context->ValueToInstanceVector<ClassProperty const>(in_values[13]);
    bool uuid =
        in_context->ValueToBool(in_values[14], true, false);
    const std::string& funcPrefix =
        in_context->ValueToString(in_values[15], true, "");
    const std::string& constructor =
        in_context->ValueToString(in_values[16], true, "");
    const std::string& instance =
        in_context->ValueToString(in_values[17], true, "");

    const BaseWrapperClassConstSP& o_result = types::BaseWrapperClass::Make(
        description, name, ns, innerClass, baseClass, isVirtual, noMake,
        objectName, isDelegate, canPut, noId, dataType, asValue,
        classProperties, uuid, funcPrefix, constructor, instance);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller BaseWrapperClass_FunctionCaller = {
    "BaseWrapperClass",
    18,
    {
        {"description", spi::ArgType::STRING, "string", true, false, false},
        {"name", spi::ArgType::STRING, "string", false, false, false},
        {"ns", spi::ArgType::STRING, "string", false, false, false},
        {"innerClass", spi::ArgType::OBJECT, "InnerClass", false, false, false},
        {"baseClass", spi::ArgType::OBJECT, "BaseWrapperClass", false, true, false},
        {"isVirtual", spi::ArgType::BOOL, "bool", false, false, false},
        {"noMake", spi::ArgType::BOOL, "bool", false, false, false},
        {"objectName", spi::ArgType::STRING, "string", false, true, false},
        {"isDelegate", spi::ArgType::BOOL, "bool", false, false, false},
        {"canPut", spi::ArgType::BOOL, "bool", false, true, false},
        {"noId", spi::ArgType::BOOL, "bool", false, true, false},
        {"dataType", spi::ArgType::OBJECT, "DataType", false, false, false},
        {"asValue", spi::ArgType::BOOL, "bool", false, true, false},
        {"classProperties", spi::ArgType::OBJECT, "ClassProperty", true, false, false},
        {"uuid", spi::ArgType::BOOL, "bool", false, true, false},
        {"funcPrefix", spi::ArgType::STRING, "string", false, true, false},
        {"constructor", spi::ArgType::STRING, "string", false, true, false},
        {"instance", spi::ArgType::STRING, "string", false, true, false}
    },
    BaseWrapperClass_caller
};

/*
****************************************************************************
* Implementation of TypesLibrary
****************************************************************************
*/

TypesLibrary::outer_type
TypesLibrary::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return TypesLibrary::outer_type();

    // isInstance
    const TypesLibrary* p = dynamic_cast<const TypesLibrary*>(o.get());
    if (p)
        return TypesLibrary::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const TypesLibrary*>(o2.get());
            if (p)
                return TypesLibrary::outer_type(p);
        }
        else
        {
            o2 = o;
        }
    }
    else
    {
        o2 = o;
    }

    // coerceTo if the input object has a different name
    if (strcmp(ot->get_class_name(), o2->get_class_name()) != 0)
    {
        spi::ObjectConstSP o3 = o2->coerce_to_object(ot->get_class_name());
        if (o3)
        {
            p = dynamic_cast<const TypesLibrary*>(o3.get());
            if (p)
                return TypesLibrary::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void TypesLibrary::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    const std::string& name = this->name();
    const std::string& ns = this->ns();
    const std::string& version = this->version();
    const std::string& lastModuleName = this->lastModuleName();
    const std::vector<DataTypeConstSP>& dataTypes = this->dataTypes();
    const std::vector<DataTypeConstSP>& publicDataTypes = this->publicDataTypes();
    const std::vector<BaseClassConstSP>& baseClasses = this->baseClasses();
    const std::vector<EnumConstSP>& enums = this->enums();

    if (!public_only)
    {
        obj_map->SetString("name", name);
        obj_map->SetString("ns", ns);
        obj_map->SetString("version", version);
        obj_map->SetString("lastModuleName", lastModuleName);
        obj_map->SetInstanceVector<DataType const>("dataTypes", dataTypes);
        obj_map->SetInstanceVector<DataType const>("publicDataTypes", publicDataTypes);
        obj_map->SetInstanceVector<BaseClass const>("baseClasses", baseClasses);
        obj_map->SetInstanceVector<Enum const>("enums", enums);
    }
}

spi::ObjectConstSP TypesLibrary::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::string& name
        = obj_map->GetString("name");
    const std::string& ns
        = obj_map->GetString("ns");
    const std::string& version
        = obj_map->GetString("version");
    const std::string& lastModuleName
        = obj_map->GetString("lastModuleName");
    const std::vector<DataTypeConstSP>& dataTypes
        = obj_map->GetInstanceVector<DataType const>("dataTypes", value_to_object);
    const std::vector<DataTypeConstSP>& publicDataTypes
        = obj_map->GetInstanceVector<DataType const>("publicDataTypes", value_to_object);
    const std::vector<BaseClassConstSP>& baseClasses
        = obj_map->GetInstanceVector<BaseClass const>("baseClasses", value_to_object);
    const std::vector<EnumConstSP>& enums
        = obj_map->GetInstanceVector<Enum const>("enums", value_to_object);

    return TypesLibrary::Make(name, ns, version, lastModuleName, dataTypes,
        publicDataTypes, baseClasses, enums);
}

SPI_IMPLEMENT_OBJECT_TYPE(TypesLibrary, "TypesLibrary", types_service, false, 0);

spi::Value TypesLibrary_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& name =
        in_context->ValueToString(in_values[0]);
    const std::string& ns =
        in_context->ValueToString(in_values[1]);
    const std::string& version =
        in_context->ValueToString(in_values[2]);
    const std::string& lastModuleName =
        in_context->ValueToString(in_values[3]);
    std::vector<DataTypeConstSP> dataTypes =
        in_context->ValueToInstanceVector<DataType const>(in_values[4]);
    std::vector<DataTypeConstSP> publicDataTypes =
        in_context->ValueToInstanceVector<DataType const>(in_values[5]);
    std::vector<BaseClassConstSP> baseClasses =
        in_context->ValueToInstanceVector<BaseClass const>(in_values[6]);
    std::vector<EnumConstSP> enums =
        in_context->ValueToInstanceVector<Enum const>(in_values[7]);

    const TypesLibraryConstSP& o_result = types::TypesLibrary::Make(name, ns,
        version, lastModuleName, dataTypes, publicDataTypes, baseClasses,
        enums);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller TypesLibrary_FunctionCaller = {
    "TypesLibrary",
    8,
    {
        {"name", spi::ArgType::STRING, "string", false, false, false},
        {"ns", spi::ArgType::STRING, "string", false, false, false},
        {"version", spi::ArgType::STRING, "string", false, false, false},
        {"lastModuleName", spi::ArgType::STRING, "string", false, false, false},
        {"dataTypes", spi::ArgType::OBJECT, "DataType", true, false, false},
        {"publicDataTypes", spi::ArgType::OBJECT, "DataType", true, false, false},
        {"baseClasses", spi::ArgType::OBJECT, "BaseClass", true, false, false},
        {"enums", spi::ArgType::OBJECT, "Enum", true, false, false}
    },
    TypesLibrary_caller
};

void types_register_object_types(const spi::ServiceSP& svc)
{
    svc->add_enum_info(PublicType::get_enum_info());
    svc->add_object_type(&InputConverter::object_type);
    svc->add_object_type(&InputConverterStringFormat::object_type);
    svc->add_function_caller(&InputConverterStringFormat_FunctionCaller);
    InputConverter::g_sub_class_wrappers.push_back(InputConverterStringFormat::BaseWrap);
    svc->add_object_type(&InputConverterClass::object_type);
    svc->add_function_caller(&InputConverterClass_FunctionCaller);
    InputConverter::g_sub_class_wrappers.push_back(InputConverterClass::BaseWrap);
    svc->add_object_type(&DataType::object_type);
    svc->add_function_caller(&DataType_FunctionCaller);
    svc->add_object_type(&Attribute::object_type);
    svc->add_function_caller(&Attribute_FunctionCaller);
    svc->add_object_type(&ClassProperty::object_type);
    svc->add_function_caller(&ClassProperty_FunctionCaller);
    svc->add_object_type(&Enumerand::object_type);
    svc->add_function_caller(&Enumerand_FunctionCaller);
    svc->add_object_type(&Enum::object_type);
    svc->add_function_caller(&Enum_FunctionCaller);
    svc->add_object_type(&BaseClass::object_type);
    svc->add_object_type(&BaseStruct::object_type);
    svc->add_function_caller(&BaseStruct_FunctionCaller);
    BaseClass::g_sub_class_wrappers.push_back(BaseStruct::BaseWrap);
    svc->add_object_type(&InnerClass::object_type);
    svc->add_function_caller(&InnerClass_FunctionCaller);
    svc->add_object_type(&BaseWrapperClass::object_type);
    svc->add_function_caller(&BaseWrapperClass_FunctionCaller);
    BaseClass::g_sub_class_wrappers.push_back(BaseWrapperClass::BaseWrap);
    svc->add_object_type(&TypesLibrary::object_type);
    svc->add_function_caller(&TypesLibrary_FunctionCaller);
}

TYPES_END_NAMESPACE

