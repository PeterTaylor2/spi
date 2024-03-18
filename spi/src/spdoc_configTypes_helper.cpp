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
* Source file: spdoc_configTypes_helper.cpp
****************************************************************************
*/

#include "spdoc_configTypes.hpp"
#include "spdoc_configTypes_helper.hpp"
#include "spdoc_dll_service_manager.hpp"

SPDOC_BEGIN_NAMESPACE

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
    obj_map->SetString("name", name);
    obj_map->SetBool("passByReference", passByReference);
    obj_map->SetString("refTypeName", refTypeName);
    obj_map->SetString("valueTypeName", valueTypeName);
    obj_map->SetString("publicType", publicType);
    obj_map->SetBool("noDoc", noDoc);
    obj_map->SetString("nsService", nsService);
    obj_map->SetBool("objectAsValue", objectAsValue, !public_only && (objectAsValue == false));
}

spi::ObjectConstSP DataType::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::string& name
        = obj_map->GetString("name");
    bool passByReference
        = obj_map->GetBool("passByReference");
    const std::string& refTypeName
        = obj_map->GetString("refTypeName");
    const std::string& valueTypeName
        = obj_map->GetString("valueTypeName");
    PublicType publicType
        = obj_map->GetString("publicType");
    bool noDoc
        = obj_map->GetBool("noDoc");
    const std::string& nsService
        = obj_map->GetString("nsService", true, "");
    bool objectAsValue
        = obj_map->GetBool("objectAsValue", true, false);

    return new DataType(name, passByReference, refTypeName, valueTypeName,
        publicType, noDoc, nsService, objectAsValue);
}

SPI_IMPLEMENT_OBJECT_TYPE(DataType, "DataType", spdoc_service, false, 0);

spi::Value DataType_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& name =
        in_context->ValueToString(in_values[0]);
    bool passByReference =
        in_context->ValueToBool(in_values[1]);
    const std::string& refTypeName =
        in_context->ValueToString(in_values[2]);
    const std::string& valueTypeName =
        in_context->ValueToString(in_values[3]);
    PublicType publicType =
        in_context->ValueToString(in_values[4]);
    bool noDoc =
        in_context->ValueToBool(in_values[5]);
    const std::string& nsService =
        in_context->ValueToString(in_values[6], true, "");
    bool objectAsValue =
        in_context->ValueToBool(in_values[7], true, false);

    const DataTypeConstSP& o_result = spdoc::DataType::Make(name,
        passByReference, refTypeName, valueTypeName, publicType, noDoc,
        nsService, objectAsValue);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller DataType_FunctionCaller = {
    "DataType",
    8,
    {
        {"name", spi::ArgType::STRING, "string", false, false, false},
        {"passByReference", spi::ArgType::BOOL, "bool", false, false, false},
        {"refTypeName", spi::ArgType::STRING, "string", false, false, false},
        {"valueTypeName", spi::ArgType::STRING, "string", false, false, false},
        {"publicType", spi::ArgType::ENUM, "PublicType", false, false, false},
        {"noDoc", spi::ArgType::BOOL, "bool", false, false, false},
        {"nsService", spi::ArgType::STRING, "string", false, true, false},
        {"objectAsValue", spi::ArgType::BOOL, "bool", false, true, false}
    },
    DataType_caller
};

spi::Value DataType_ValueType_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const DataTypeConstSP& self =
        in_context->ValueToInstance<DataType const>(in_values[0]);

    const std::string& o_result = self->ValueType();
    return o_result;
}

spi::FunctionCaller DataType_ValueType_FunctionCaller = {
    "DataType.ValueType",
    1,
    {
        {"self", spi::ArgType::OBJECT, "DataType", false, false, false}
    },
    DataType_ValueType_caller
};

spi::ObjectType DataType_ValueType_FunctionObjectType =
    spi::FunctionObjectType("spdoc.DataType.ValueType");

spi::Value DataType_RefType_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const DataTypeConstSP& self =
        in_context->ValueToInstance<DataType const>(in_values[0]);

    const std::string& o_result = self->RefType();
    return o_result;
}

spi::FunctionCaller DataType_RefType_FunctionCaller = {
    "DataType.RefType",
    1,
    {
        {"self", spi::ArgType::OBJECT, "DataType", false, false, false}
    },
    DataType_RefType_caller
};

spi::ObjectType DataType_RefType_FunctionObjectType =
    spi::FunctionObjectType("spdoc.DataType.RefType");

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
    obj_map->SetString("name", name);
    obj_map->SetStringVector("description", description);
    obj_map->SetObject("dataType", dataType);
    obj_map->SetInt("arrayDim", arrayDim);
    obj_map->SetBool("isOptional", isOptional);
    obj_map->SetObject("defaultValue", defaultValue);
    if (public_only)
    {
        obj_map->SetBool("isArray", isArray());
    }
}

spi::ObjectConstSP Attribute::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::string& name
        = obj_map->GetString("name");
    const std::vector<std::string>& description
        = obj_map->GetStringVector("description");
    const DataTypeConstSP& dataType
        = obj_map->GetInstance<DataType const>("dataType", value_to_object);
    int arrayDim
        = obj_map->GetInt("arrayDim");
    bool isOptional
        = obj_map->GetBool("isOptional");
    const ConstantConstSP& defaultValue
        = obj_map->GetInstance<Constant const>("defaultValue", value_to_object, true);

    return new Attribute(name, description, dataType, arrayDim, isOptional,
        defaultValue);
}

SPI_IMPLEMENT_OBJECT_TYPE(Attribute, "Attribute", spdoc_service, false, 0);

spi::Value Attribute_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& name =
        in_context->ValueToString(in_values[0]);
    std::vector<std::string> description =
        in_context->ValueToStringVector(in_values[1]);
    const DataTypeConstSP& dataType =
        in_context->ValueToInstance<DataType const>(in_values[2]);
    int arrayDim =
        in_context->ValueToInt(in_values[3]);
    bool isOptional =
        in_context->ValueToBool(in_values[4]);
    const ConstantConstSP& defaultValue =
        in_context->ValueToInstance<Constant const>(in_values[5], true);

    const AttributeConstSP& o_result = spdoc::Attribute::Make(name,
        description, dataType, arrayDim, isOptional, defaultValue);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller Attribute_FunctionCaller = {
    "Attribute",
    6,
    {
        {"name", spi::ArgType::STRING, "string", false, false, false},
        {"description", spi::ArgType::STRING, "string", true, false, false},
        {"dataType", spi::ArgType::OBJECT, "DataType", false, false, false},
        {"arrayDim", spi::ArgType::INT, "int", false, false, false},
        {"isOptional", spi::ArgType::BOOL, "bool", false, false, false},
        {"defaultValue", spi::ArgType::OBJECT, "Constant", false, true, false}
    },
    Attribute_caller
};

spi::Value Attribute_encoding_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const AttributeConstSP& self =
        in_context->ValueToInstance<Attribute const>(in_values[0]);
    bool isOutput =
        in_context->ValueToBool(in_values[1], true, false);

    const std::string& o_result = self->encoding(isOutput);
    return o_result;
}

spi::FunctionCaller Attribute_encoding_FunctionCaller = {
    "Attribute.encoding",
    2,
    {
        {"self", spi::ArgType::OBJECT, "Attribute", false, false, false},
        {"isOutput", spi::ArgType::BOOL, "bool", false, true, true}
    },
    Attribute_encoding_caller
};

spi::ObjectType Attribute_encoding_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Attribute.encoding");

/*
****************************************************************************
* Implementation of ClassAttribute
****************************************************************************
*/

ClassAttribute::outer_type
ClassAttribute::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return ClassAttribute::outer_type();

    // isInstance
    const ClassAttribute* p = dynamic_cast<const ClassAttribute*>(o.get());
    if (p)
        return ClassAttribute::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const ClassAttribute*>(o2.get());
            if (p)
                return ClassAttribute::outer_type(p);
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
            p = dynamic_cast<const ClassAttribute*>(o3.get());
            if (p)
                return ClassAttribute::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

spi::ObjectConstSP ClassAttribute::coerce_to_object(
    const std::string& className) const
{
    if (className == "Attribute")
        return operator AttributeConstSP();

    return spi::ObjectConstSP();
}

void ClassAttribute::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    obj_map->SetString("name", name);
    obj_map->SetStringVector("description", description);
    obj_map->SetObject("dataType", dataType);
    obj_map->SetInt("arrayDim", arrayDim);
    obj_map->SetBool("isOptional", isOptional);
    obj_map->SetObject("defaultValue", defaultValue);
    obj_map->SetBool("accessible", accessible);
    obj_map->SetString("accessor", accessor);
    if (public_only)
    {
        obj_map->SetBool("isArray", isArray());
        obj_map->SetString("encoding", encoding());
    }
}

spi::ObjectConstSP ClassAttribute::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::string& name
        = obj_map->GetString("name");
    const std::vector<std::string>& description
        = obj_map->GetStringVector("description");
    const DataTypeConstSP& dataType
        = obj_map->GetInstance<DataType const>("dataType", value_to_object);
    int arrayDim
        = obj_map->GetInt("arrayDim");
    bool isOptional
        = obj_map->GetBool("isOptional");
    const ConstantConstSP& defaultValue
        = obj_map->GetInstance<Constant const>("defaultValue", value_to_object, true);
    bool accessible
        = obj_map->GetBool("accessible");
    const std::string& accessor
        = obj_map->GetString("accessor");

    return new ClassAttribute(name, description, dataType, arrayDim,
        isOptional, defaultValue, accessible, accessor);
}

SPI_IMPLEMENT_OBJECT_TYPE(ClassAttribute, "ClassAttribute", spdoc_service, false, 0);

spi::Value ClassAttribute_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& name =
        in_context->ValueToString(in_values[0]);
    std::vector<std::string> description =
        in_context->ValueToStringVector(in_values[1]);
    const DataTypeConstSP& dataType =
        in_context->ValueToInstance<DataType const>(in_values[2]);
    int arrayDim =
        in_context->ValueToInt(in_values[3]);
    bool isOptional =
        in_context->ValueToBool(in_values[4]);
    const ConstantConstSP& defaultValue =
        in_context->ValueToInstance<Constant const>(in_values[5], true);
    bool accessible =
        in_context->ValueToBool(in_values[6]);
    const std::string& accessor =
        in_context->ValueToString(in_values[7]);

    const ClassAttributeConstSP& o_result = spdoc::ClassAttribute::Make(name,
        description, dataType, arrayDim, isOptional, defaultValue, accessible,
        accessor);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller ClassAttribute_FunctionCaller = {
    "ClassAttribute",
    8,
    {
        {"name", spi::ArgType::STRING, "string", false, false, false},
        {"description", spi::ArgType::STRING, "string", true, false, false},
        {"dataType", spi::ArgType::OBJECT, "DataType", false, false, false},
        {"arrayDim", spi::ArgType::INT, "int", false, false, false},
        {"isOptional", spi::ArgType::BOOL, "bool", false, false, false},
        {"defaultValue", spi::ArgType::OBJECT, "Constant", false, true, false},
        {"accessible", spi::ArgType::BOOL, "bool", false, false, false},
        {"accessor", spi::ArgType::STRING, "string", false, false, false}
    },
    ClassAttribute_caller
};

/*
****************************************************************************
* Implementation of Construct
****************************************************************************
*/

Construct::outer_type
Construct::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return Construct::outer_type();

    // isInstance
    const Construct* p = dynamic_cast<const Construct*>(o.get());
    if (p)
        return Construct::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const Construct*>(o2.get());
            if (p)
                return Construct::outer_type(p);
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
            p = dynamic_cast<const Construct*>(o3.get());
            if (p)
                return Construct::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void Construct::to_map(
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

spi::ObjectConstSP Construct::object_from_map(
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

SPI_IMPLEMENT_OBJECT_TYPE(Construct, "Construct", spdoc_service, false, 0);

spi::Value Construct_getType_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ConstructConstSP& self =
        in_context->ValueToInstance<Construct const>(in_values[0]);

    const std::string& o_result = self->getType();
    return o_result;
}

spi::FunctionCaller Construct_getType_FunctionCaller = {
    "Construct.getType",
    1,
    {
        {"self", spi::ArgType::OBJECT, "Construct", false, false, false}
    },
    Construct_getType_caller
};

spi::ObjectType Construct_getType_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Construct.getType");

spi::Value Construct_Summary_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ConstructConstSP& self =
        in_context->ValueToInstance<Construct const>(in_values[0]);

    const std::vector<std::string>& o_result = self->Summary();
    return o_result;
}

spi::FunctionCaller Construct_Summary_FunctionCaller = {
    "Construct.Summary",
    1,
    {
        {"self", spi::ArgType::OBJECT, "Construct", false, false, false}
    },
    Construct_Summary_caller
};

spi::ObjectType Construct_Summary_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Construct.Summary");

/*
****************************************************************************
* Implementation of SimpleType
****************************************************************************
*/

SimpleType::outer_type
SimpleType::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return SimpleType::outer_type();

    // isInstance
    const SimpleType* p = dynamic_cast<const SimpleType*>(o.get());
    if (p)
        return SimpleType::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const SimpleType*>(o2.get());
            if (p)
                return SimpleType::outer_type(p);
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
            p = dynamic_cast<const SimpleType*>(o3.get());
            if (p)
                return SimpleType::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void SimpleType::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    obj_map->SetString("name", name);
    obj_map->SetStringVector("description", description);
    obj_map->SetString("typeName", typeName);
    obj_map->SetBool("noDoc", noDoc);
}

spi::ObjectConstSP SimpleType::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::string& name
        = obj_map->GetString("name");
    const std::vector<std::string>& description
        = obj_map->GetStringVector("description");
    const std::string& typeName
        = obj_map->GetString("typeName");
    bool noDoc
        = obj_map->GetBool("noDoc");

    return new SimpleType(name, description, typeName, noDoc);
}

SPI_IMPLEMENT_OBJECT_TYPE(SimpleType, "SimpleType", spdoc_service, true, 0);

spi::Value SimpleType_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& name =
        in_context->ValueToString(in_values[0]);
    std::vector<std::string> description =
        in_context->ValueToStringVector(in_values[1]);
    const std::string& typeName =
        in_context->ValueToString(in_values[2]);
    bool noDoc =
        in_context->ValueToBool(in_values[3]);

    const SimpleTypeConstSP& o_result = spdoc::SimpleType::Make(name,
        description, typeName, noDoc);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller SimpleType_FunctionCaller = {
    "SimpleType",
    4,
    {
        {"name", spi::ArgType::STRING, "string", false, false, false},
        {"description", spi::ArgType::STRING, "string", true, false, false},
        {"typeName", spi::ArgType::STRING, "string", false, false, false},
        {"noDoc", spi::ArgType::BOOL, "bool", false, false, false}
    },
    SimpleType_caller
};

/*
****************************************************************************
* Implementation of Function
****************************************************************************
*/

Function::outer_type
Function::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return Function::outer_type();

    // isInstance
    const Function* p = dynamic_cast<const Function*>(o.get());
    if (p)
        return Function::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const Function*>(o2.get());
            if (p)
                return Function::outer_type(p);
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
            p = dynamic_cast<const Function*>(o3.get());
            if (p)
                return Function::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void Function::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    obj_map->SetString("name", name);
    obj_map->SetStringVector("description", description);
    obj_map->SetStringVector("returnTypeDescription", returnTypeDescription);
    obj_map->SetObject("returnType", returnType);
    obj_map->SetInt("returnArrayDim", returnArrayDim);
    obj_map->SetInstanceVector<Attribute const>("inputs", inputs);
    obj_map->SetInstanceVector<Attribute const>("outputs", outputs);
    obj_map->SetStringVector("excelOptions", excelOptions);
}

spi::ObjectConstSP Function::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::string& name
        = obj_map->GetString("name");
    const std::vector<std::string>& description
        = obj_map->GetStringVector("description");
    const std::vector<std::string>& returnTypeDescription
        = obj_map->GetStringVector("returnTypeDescription");
    const DataTypeConstSP& returnType
        = obj_map->GetInstance<DataType const>("returnType", value_to_object);
    int returnArrayDim
        = obj_map->GetInt("returnArrayDim");
    const std::vector<AttributeConstSP>& inputs
        = obj_map->GetInstanceVector<Attribute const>("inputs", value_to_object);
    const std::vector<AttributeConstSP>& outputs
        = obj_map->GetInstanceVector<Attribute const>("outputs", value_to_object);
    const std::vector<std::string>& excelOptions
        = obj_map->GetStringVector("excelOptions");

    return new Function(name, description, returnTypeDescription, returnType,
        returnArrayDim, inputs, outputs, excelOptions);
}

SPI_IMPLEMENT_OBJECT_TYPE(Function, "Function", spdoc_service, true, 0);

spi::Value Function_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& name =
        in_context->ValueToString(in_values[0]);
    std::vector<std::string> description =
        in_context->ValueToStringVector(in_values[1]);
    std::vector<std::string> returnTypeDescription =
        in_context->ValueToStringVector(in_values[2]);
    const DataTypeConstSP& returnType =
        in_context->ValueToInstance<DataType const>(in_values[3]);
    int returnArrayDim =
        in_context->ValueToInt(in_values[4]);
    std::vector<AttributeConstSP> inputs =
        in_context->ValueToInstanceVector<Attribute const>(in_values[5]);
    std::vector<AttributeConstSP> outputs =
        in_context->ValueToInstanceVector<Attribute const>(in_values[6]);
    std::vector<std::string> excelOptions =
        in_context->ValueToStringVector(in_values[7]);

    const FunctionConstSP& o_result = spdoc::Function::Make(name, description,
        returnTypeDescription, returnType, returnArrayDim, inputs, outputs,
        excelOptions);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller Function_FunctionCaller = {
    "Function",
    8,
    {
        {"name", spi::ArgType::STRING, "string", false, false, false},
        {"description", spi::ArgType::STRING, "string", true, false, false},
        {"returnTypeDescription", spi::ArgType::STRING, "string", true, false, false},
        {"returnType", spi::ArgType::OBJECT, "DataType", false, false, false},
        {"returnArrayDim", spi::ArgType::INT, "int", false, false, false},
        {"inputs", spi::ArgType::OBJECT, "Attribute", true, false, false},
        {"outputs", spi::ArgType::OBJECT, "Attribute", true, false, false},
        {"excelOptions", spi::ArgType::STRING, "string", true, false, false}
    },
    Function_caller
};

spi::Value Function_returnsObject_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const FunctionConstSP& self =
        in_context->ValueToInstance<Function const>(in_values[0]);

    bool o_result = self->returnsObject();
    return o_result;
}

spi::FunctionCaller Function_returnsObject_FunctionCaller = {
    "Function.returnsObject",
    1,
    {
        {"self", spi::ArgType::OBJECT, "Function", false, false, false}
    },
    Function_returnsObject_caller
};

spi::ObjectType Function_returnsObject_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Function.returnsObject");

spi::Value Function_objectCount_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const FunctionConstSP& self =
        in_context->ValueToInstance<Function const>(in_values[0]);

    int o_result = self->objectCount();
    return o_result;
}

spi::FunctionCaller Function_objectCount_FunctionCaller = {
    "Function.objectCount",
    1,
    {
        {"self", spi::ArgType::OBJECT, "Function", false, false, false}
    },
    Function_objectCount_caller
};

spi::ObjectType Function_objectCount_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Function.objectCount");

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
    obj_map->SetString("code", code);
    obj_map->SetStringVector("strings", strings);
    obj_map->SetStringVector("description", description);
}

spi::ObjectConstSP Enumerand::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::string& code
        = obj_map->GetString("code");
    const std::vector<std::string>& strings
        = obj_map->GetStringVector("strings");
    const std::vector<std::string>& description
        = obj_map->GetStringVector("description");

    return new Enumerand(code, strings, description);
}

SPI_IMPLEMENT_OBJECT_TYPE(Enumerand, "Enumerand", spdoc_service, false, 0);

spi::Value Enumerand_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& code =
        in_context->ValueToString(in_values[0]);
    std::vector<std::string> strings =
        in_context->ValueToStringVector(in_values[1]);
    std::vector<std::string> description =
        in_context->ValueToStringVector(in_values[2]);

    const EnumerandConstSP& o_result = spdoc::Enumerand::Make(code, strings,
        description);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller Enumerand_FunctionCaller = {
    "Enumerand",
    3,
    {
        {"code", spi::ArgType::STRING, "string", false, false, false},
        {"strings", spi::ArgType::STRING, "string", true, false, false},
        {"description", spi::ArgType::STRING, "string", true, false, false}
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
    obj_map->SetString("name", name);
    obj_map->SetStringVector("description", description);
    obj_map->SetInstanceVector<Enumerand const>("enumerands", enumerands);
}

spi::ObjectConstSP Enum::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::string& name
        = obj_map->GetString("name");
    const std::vector<std::string>& description
        = obj_map->GetStringVector("description");
    const std::vector<EnumerandConstSP>& enumerands
        = obj_map->GetInstanceVector<Enumerand const>("enumerands", value_to_object);

    return new Enum(name, description, enumerands);
}

SPI_IMPLEMENT_OBJECT_TYPE(Enum, "Enum", spdoc_service, true, 0);

spi::Value Enum_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& name =
        in_context->ValueToString(in_values[0]);
    std::vector<std::string> description =
        in_context->ValueToStringVector(in_values[1]);
    std::vector<EnumerandConstSP> enumerands =
        in_context->ValueToInstanceVector<Enumerand const>(in_values[2]);

    const EnumConstSP& o_result = spdoc::Enum::Make(name, description,
        enumerands);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller Enum_FunctionCaller = {
    "Enum",
    3,
    {
        {"name", spi::ArgType::STRING, "string", false, false, false},
        {"description", spi::ArgType::STRING, "string", true, false, false},
        {"enumerands", spi::ArgType::OBJECT, "Enumerand", true, false, false}
    },
    Enum_caller
};

/*
****************************************************************************
* Implementation of ClassMethod
****************************************************************************
*/

ClassMethod::outer_type
ClassMethod::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return ClassMethod::outer_type();

    // isInstance
    const ClassMethod* p = dynamic_cast<const ClassMethod*>(o.get());
    if (p)
        return ClassMethod::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const ClassMethod*>(o2.get());
            if (p)
                return ClassMethod::outer_type(p);
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
            p = dynamic_cast<const ClassMethod*>(o3.get());
            if (p)
                return ClassMethod::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void ClassMethod::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    obj_map->SetObject("function", function);
    obj_map->SetBool("isConst", isConst);
    obj_map->SetBool("isVirtual", isVirtual);
    obj_map->SetBool("isStatic", isStatic);
    obj_map->SetBool("isImplementation", isImplementation);
    obj_map->SetString("implements", implements, !public_only && (implements == std::string("")));
}

spi::ObjectConstSP ClassMethod::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const FunctionConstSP& function
        = obj_map->GetInstance<Function const>("function", value_to_object);
    bool isConst
        = obj_map->GetBool("isConst");
    bool isVirtual
        = obj_map->GetBool("isVirtual");
    bool isStatic
        = obj_map->GetBool("isStatic");
    bool isImplementation
        = obj_map->GetBool("isImplementation");
    const std::string& implements
        = obj_map->GetString("implements", true, "");

    return new ClassMethod(function, isConst, isVirtual, isStatic,
        isImplementation, implements);
}

SPI_IMPLEMENT_OBJECT_TYPE(ClassMethod, "ClassMethod", spdoc_service, false, 0);

spi::Value ClassMethod_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const FunctionConstSP& function =
        in_context->ValueToInstance<Function const>(in_values[0]);
    bool isConst =
        in_context->ValueToBool(in_values[1]);
    bool isVirtual =
        in_context->ValueToBool(in_values[2]);
    bool isStatic =
        in_context->ValueToBool(in_values[3]);
    bool isImplementation =
        in_context->ValueToBool(in_values[4]);
    const std::string& implements =
        in_context->ValueToString(in_values[5], true, "");

    const ClassMethodConstSP& o_result = spdoc::ClassMethod::Make(function,
        isConst, isVirtual, isStatic, isImplementation, implements);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller ClassMethod_FunctionCaller = {
    "ClassMethod",
    6,
    {
        {"function", spi::ArgType::OBJECT, "Function", false, false, false},
        {"isConst", spi::ArgType::BOOL, "bool", false, false, false},
        {"isVirtual", spi::ArgType::BOOL, "bool", false, false, false},
        {"isStatic", spi::ArgType::BOOL, "bool", false, false, false},
        {"isImplementation", spi::ArgType::BOOL, "bool", false, false, false},
        {"implements", spi::ArgType::STRING, "string", false, true, false}
    },
    ClassMethod_caller
};

spi::Value ClassMethod_Summary_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ClassMethodConstSP& self =
        in_context->ValueToInstance<ClassMethod const>(in_values[0]);

    const std::vector<std::string>& o_result = self->Summary();
    return o_result;
}

spi::FunctionCaller ClassMethod_Summary_FunctionCaller = {
    "ClassMethod.Summary",
    1,
    {
        {"self", spi::ArgType::OBJECT, "ClassMethod", false, false, false}
    },
    ClassMethod_Summary_caller
};

spi::ObjectType ClassMethod_Summary_FunctionObjectType =
    spi::FunctionObjectType("spdoc.ClassMethod.Summary");

/*
****************************************************************************
* Implementation of CoerceFrom
****************************************************************************
*/

CoerceFrom::outer_type
CoerceFrom::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return CoerceFrom::outer_type();

    // isInstance
    const CoerceFrom* p = dynamic_cast<const CoerceFrom*>(o.get());
    if (p)
        return CoerceFrom::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const CoerceFrom*>(o2.get());
            if (p)
                return CoerceFrom::outer_type(p);
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
            p = dynamic_cast<const CoerceFrom*>(o3.get());
            if (p)
                return CoerceFrom::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void CoerceFrom::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    obj_map->SetStringVector("description", description);
    obj_map->SetObject("coerceFrom", coerceFrom);
}

spi::ObjectConstSP CoerceFrom::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::vector<std::string>& description
        = obj_map->GetStringVector("description");
    const AttributeConstSP& coerceFrom
        = obj_map->GetInstance<Attribute const>("coerceFrom", value_to_object);

    return new CoerceFrom(description, coerceFrom);
}

SPI_IMPLEMENT_OBJECT_TYPE(CoerceFrom, "CoerceFrom", spdoc_service, false, 0);

spi::Value CoerceFrom_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    std::vector<std::string> description =
        in_context->ValueToStringVector(in_values[0]);
    const AttributeConstSP& coerceFrom =
        in_context->ValueToInstance<Attribute const>(in_values[1]);

    const CoerceFromConstSP& o_result = spdoc::CoerceFrom::Make(description,
        coerceFrom);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller CoerceFrom_FunctionCaller = {
    "CoerceFrom",
    2,
    {
        {"description", spi::ArgType::STRING, "string", true, false, false},
        {"coerceFrom", spi::ArgType::OBJECT, "Attribute", false, false, false}
    },
    CoerceFrom_caller
};

spi::Value CoerceFrom_Summary_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const CoerceFromConstSP& self =
        in_context->ValueToInstance<CoerceFrom const>(in_values[0]);

    const std::vector<std::string>& o_result = self->Summary();
    return o_result;
}

spi::FunctionCaller CoerceFrom_Summary_FunctionCaller = {
    "CoerceFrom.Summary",
    1,
    {
        {"self", spi::ArgType::OBJECT, "CoerceFrom", false, false, false}
    },
    CoerceFrom_Summary_caller
};

spi::ObjectType CoerceFrom_Summary_FunctionObjectType =
    spi::FunctionObjectType("spdoc.CoerceFrom.Summary");

/*
****************************************************************************
* Implementation of CoerceTo
****************************************************************************
*/

CoerceTo::outer_type
CoerceTo::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return CoerceTo::outer_type();

    // isInstance
    const CoerceTo* p = dynamic_cast<const CoerceTo*>(o.get());
    if (p)
        return CoerceTo::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const CoerceTo*>(o2.get());
            if (p)
                return CoerceTo::outer_type(p);
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
            p = dynamic_cast<const CoerceTo*>(o3.get());
            if (p)
                return CoerceTo::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void CoerceTo::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    obj_map->SetStringVector("description", description);
    obj_map->SetString("className", className);
    obj_map->SetObject("classType", classType);
}

spi::ObjectConstSP CoerceTo::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::vector<std::string>& description
        = obj_map->GetStringVector("description");
    const std::string& className
        = obj_map->GetString("className");
    const DataTypeConstSP& classType
        = obj_map->GetInstance<DataType const>("classType", value_to_object);

    return new CoerceTo(description, className, classType);
}

SPI_IMPLEMENT_OBJECT_TYPE(CoerceTo, "CoerceTo", spdoc_service, false, 0);

spi::Value CoerceTo_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    std::vector<std::string> description =
        in_context->ValueToStringVector(in_values[0]);
    const std::string& className =
        in_context->ValueToString(in_values[1]);
    const DataTypeConstSP& classType =
        in_context->ValueToInstance<DataType const>(in_values[2]);

    const CoerceToConstSP& o_result = spdoc::CoerceTo::Make(description,
        className, classType);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller CoerceTo_FunctionCaller = {
    "CoerceTo",
    3,
    {
        {"description", spi::ArgType::STRING, "string", true, false, false},
        {"className", spi::ArgType::STRING, "string", false, false, false},
        {"classType", spi::ArgType::OBJECT, "DataType", false, false, false}
    },
    CoerceTo_caller
};

spi::Value CoerceTo_Summary_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const CoerceToConstSP& self =
        in_context->ValueToInstance<CoerceTo const>(in_values[0]);

    const std::vector<std::string>& o_result = self->Summary();
    return o_result;
}

spi::FunctionCaller CoerceTo_Summary_FunctionCaller = {
    "CoerceTo.Summary",
    1,
    {
        {"self", spi::ArgType::OBJECT, "CoerceTo", false, false, false}
    },
    CoerceTo_Summary_caller
};

spi::ObjectType CoerceTo_Summary_FunctionObjectType =
    spi::FunctionObjectType("spdoc.CoerceTo.Summary");

/*
****************************************************************************
* Implementation of Class
****************************************************************************
*/

Class::outer_type
Class::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return Class::outer_type();

    // isInstance
    const Class* p = dynamic_cast<const Class*>(o.get());
    if (p)
        return Class::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const Class*>(o2.get());
            if (p)
                return Class::outer_type(p);
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
            p = dynamic_cast<const Class*>(o3.get());
            if (p)
                return Class::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void Class::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    obj_map->SetString("name", name);
    obj_map->SetString("ns", ns, !public_only && (ns == std::string("")));
    obj_map->SetStringVector("description", description);
    obj_map->SetString("baseClassName", baseClassName);
    obj_map->SetInstanceVector<ClassAttribute const>("attributes", attributes);
    obj_map->SetInstanceVector<ClassAttribute const>("properties", properties);
    obj_map->SetInstanceVector<ClassMethod const>("methods", methods);
    obj_map->SetInstanceVector<CoerceFrom const>("coerceFrom", coerceFrom);
    obj_map->SetInstanceVector<CoerceTo const>("coerceTo", coerceTo);
    obj_map->SetBool("isAbstract", isAbstract);
    obj_map->SetBool("noMake", noMake);
    obj_map->SetString("objectName", objectName);
    obj_map->SetObject("dataType", dataType);
    obj_map->SetBool("isDelegate", isDelegate);
    obj_map->SetBool("canPut", canPut);
    obj_map->SetBool("hasDynamicAttributes", hasDynamicAttributes);
    obj_map->SetBool("asValue", asValue, !public_only && (asValue == false));
}

spi::ObjectConstSP Class::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::string& name
        = obj_map->GetString("name");
    const std::string& ns
        = obj_map->GetString("ns", true, "");
    const std::vector<std::string>& description
        = obj_map->GetStringVector("description");
    const std::string& baseClassName
        = obj_map->GetString("baseClassName");
    const std::vector<ClassAttributeConstSP>& attributes
        = obj_map->GetInstanceVector<ClassAttribute const>("attributes", value_to_object);
    const std::vector<ClassAttributeConstSP>& properties
        = obj_map->GetInstanceVector<ClassAttribute const>("properties", value_to_object);
    const std::vector<ClassMethodConstSP>& methods
        = obj_map->GetInstanceVector<ClassMethod const>("methods", value_to_object);
    const std::vector<CoerceFromConstSP>& coerceFrom
        = obj_map->GetInstanceVector<CoerceFrom const>("coerceFrom", value_to_object);
    const std::vector<CoerceToConstSP>& coerceTo
        = obj_map->GetInstanceVector<CoerceTo const>("coerceTo", value_to_object);
    bool isAbstract
        = obj_map->GetBool("isAbstract");
    bool noMake
        = obj_map->GetBool("noMake");
    const std::string& objectName
        = obj_map->GetString("objectName");
    const DataTypeConstSP& dataType
        = obj_map->GetInstance<DataType const>("dataType", value_to_object);
    bool isDelegate
        = obj_map->GetBool("isDelegate");
    bool canPut
        = obj_map->GetBool("canPut");
    bool hasDynamicAttributes
        = obj_map->GetBool("hasDynamicAttributes");
    bool asValue
        = obj_map->GetBool("asValue", true, false);

    return new Class(name, ns, description, baseClassName, attributes,
        properties, methods, coerceFrom, coerceTo, isAbstract, noMake,
        objectName, dataType, isDelegate, canPut, hasDynamicAttributes,
        asValue);
}

SPI_IMPLEMENT_OBJECT_TYPE(Class, "Class", spdoc_service, true, 0);

spi::Value Class_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& name =
        in_context->ValueToString(in_values[0]);
    const std::string& ns =
        in_context->ValueToString(in_values[1], true, "");
    std::vector<std::string> description =
        in_context->ValueToStringVector(in_values[2]);
    const std::string& baseClassName =
        in_context->ValueToString(in_values[3]);
    std::vector<ClassAttributeConstSP> attributes =
        in_context->ValueToInstanceVector<ClassAttribute const>(in_values[4]);
    std::vector<ClassAttributeConstSP> properties =
        in_context->ValueToInstanceVector<ClassAttribute const>(in_values[5]);
    std::vector<ClassMethodConstSP> methods =
        in_context->ValueToInstanceVector<ClassMethod const>(in_values[6]);
    std::vector<CoerceFromConstSP> coerceFrom =
        in_context->ValueToInstanceVector<CoerceFrom const>(in_values[7]);
    std::vector<CoerceToConstSP> coerceTo =
        in_context->ValueToInstanceVector<CoerceTo const>(in_values[8]);
    bool isAbstract =
        in_context->ValueToBool(in_values[9]);
    bool noMake =
        in_context->ValueToBool(in_values[10]);
    const std::string& objectName =
        in_context->ValueToString(in_values[11]);
    const DataTypeConstSP& dataType =
        in_context->ValueToInstance<DataType const>(in_values[12]);
    bool isDelegate =
        in_context->ValueToBool(in_values[13]);
    bool canPut =
        in_context->ValueToBool(in_values[14]);
    bool hasDynamicAttributes =
        in_context->ValueToBool(in_values[15]);
    bool asValue =
        in_context->ValueToBool(in_values[16], true, false);

    const ClassConstSP& o_result = spdoc::Class::Make(name, ns, description,
        baseClassName, attributes, properties, methods, coerceFrom, coerceTo,
        isAbstract, noMake, objectName, dataType, isDelegate, canPut,
        hasDynamicAttributes, asValue);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller Class_FunctionCaller = {
    "Class",
    17,
    {
        {"name", spi::ArgType::STRING, "string", false, false, false},
        {"ns", spi::ArgType::STRING, "string", false, true, false},
        {"description", spi::ArgType::STRING, "string", true, false, false},
        {"baseClassName", spi::ArgType::STRING, "string", false, false, false},
        {"attributes", spi::ArgType::OBJECT, "ClassAttribute", true, false, false},
        {"properties", spi::ArgType::OBJECT, "ClassAttribute", true, false, false},
        {"methods", spi::ArgType::OBJECT, "ClassMethod", true, false, false},
        {"coerceFrom", spi::ArgType::OBJECT, "CoerceFrom", true, false, false},
        {"coerceTo", spi::ArgType::OBJECT, "CoerceTo", true, false, false},
        {"isAbstract", spi::ArgType::BOOL, "bool", false, false, false},
        {"noMake", spi::ArgType::BOOL, "bool", false, false, false},
        {"objectName", spi::ArgType::STRING, "string", false, false, false},
        {"dataType", spi::ArgType::OBJECT, "DataType", false, false, false},
        {"isDelegate", spi::ArgType::BOOL, "bool", false, false, false},
        {"canPut", spi::ArgType::BOOL, "bool", false, false, false},
        {"hasDynamicAttributes", spi::ArgType::BOOL, "bool", false, false, false},
        {"asValue", spi::ArgType::BOOL, "bool", false, true, false}
    },
    Class_caller
};

spi::Value Class_ObjectName_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ClassConstSP& self =
        in_context->ValueToInstance<Class const>(in_values[0]);

    const std::string& o_result = self->ObjectName();
    return o_result;
}

spi::FunctionCaller Class_ObjectName_FunctionCaller = {
    "Class.ObjectName",
    1,
    {
        {"self", spi::ArgType::OBJECT, "Class", false, false, false}
    },
    Class_ObjectName_caller
};

spi::ObjectType Class_ObjectName_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Class.ObjectName");

spi::Value Class_ServiceNamespace_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ClassConstSP& self =
        in_context->ValueToInstance<Class const>(in_values[0]);

    const std::string& o_result = self->ServiceNamespace();
    return o_result;
}

spi::FunctionCaller Class_ServiceNamespace_FunctionCaller = {
    "Class.ServiceNamespace",
    1,
    {
        {"self", spi::ArgType::OBJECT, "Class", false, false, false}
    },
    Class_ServiceNamespace_caller
};

spi::ObjectType Class_ServiceNamespace_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Class.ServiceNamespace");

/*
****************************************************************************
* Implementation of Module
****************************************************************************
*/

Module::outer_type
Module::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return Module::outer_type();

    // isInstance
    const Module* p = dynamic_cast<const Module*>(o.get());
    if (p)
        return Module::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const Module*>(o2.get());
            if (p)
                return Module::outer_type(p);
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
            p = dynamic_cast<const Module*>(o3.get());
            if (p)
                return Module::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void Module::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    obj_map->SetString("name", name);
    obj_map->SetStringVector("description", description);
    obj_map->SetString("ns", ns);
    obj_map->SetInstanceVector<Construct const>("constructs", constructs);
}

spi::ObjectConstSP Module::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::string& name
        = obj_map->GetString("name");
    const std::vector<std::string>& description
        = obj_map->GetStringVector("description");
    const std::string& ns
        = obj_map->GetString("ns");
    const std::vector<ConstructConstSP>& constructs
        = obj_map->GetInstanceVector<Construct const>("constructs", value_to_object);

    return new Module(name, description, ns, constructs);
}

SPI_IMPLEMENT_OBJECT_TYPE(Module, "Module", spdoc_service, false, 0);

spi::Value Module_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& name =
        in_context->ValueToString(in_values[0]);
    std::vector<std::string> description =
        in_context->ValueToStringVector(in_values[1]);
    const std::string& ns =
        in_context->ValueToString(in_values[2]);
    std::vector<ConstructConstSP> constructs =
        in_context->ValueToInstanceVector<Construct const>(in_values[3]);

    const ModuleConstSP& o_result = spdoc::Module::Make(name, description, ns,
        constructs);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller Module_FunctionCaller = {
    "Module",
    4,
    {
        {"name", spi::ArgType::STRING, "string", false, false, false},
        {"description", spi::ArgType::STRING, "string", true, false, false},
        {"ns", spi::ArgType::STRING, "string", false, false, false},
        {"constructs", spi::ArgType::OBJECT, "Construct", true, false, false}
    },
    Module_caller
};

spi::Value Module_combineSummaries_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    std::vector<ModuleConstSP> modules =
        in_context->ValueToInstanceVector<Module const>(in_values[0]);
    bool sort =
        in_context->ValueToBool(in_values[1]);

    const std::vector<std::string>& o_result = spdoc::Module::combineSummaries(
        modules, sort);
    return o_result;
}

spi::FunctionCaller Module_combineSummaries_FunctionCaller = {
    "Module.combineSummaries",
    2,
    {
        {"modules", spi::ArgType::OBJECT, "Module", true, false, false},
        {"sort", spi::ArgType::BOOL, "bool", false, false, false}
    },
    Module_combineSummaries_caller
};

spi::ObjectType Module_combineSummaries_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Module.combineSummaries");

/*
****************************************************************************
* Implementation of Service
****************************************************************************
*/

Service::outer_type
Service::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return Service::outer_type();

    // isInstance
    const Service* p = dynamic_cast<const Service*>(o.get());
    if (p)
        return Service::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const Service*>(o2.get());
            if (p)
                return Service::outer_type(p);
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
            p = dynamic_cast<const Service*>(o3.get());
            if (p)
                return Service::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void Service::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    obj_map->SetString("name", name);
    obj_map->SetStringVector("description", description);
    obj_map->SetString("longName", longName);
    obj_map->SetString("ns", ns);
    obj_map->SetString("declSpec", declSpec);
    obj_map->SetString("version", version);
    obj_map->SetInstanceVector<Module const>("modules", modules);
    obj_map->SetInstanceVector<Class const>("importedBaseClasses", importedBaseClasses);
    obj_map->SetInstanceVector<Enum const>("importedEnums", importedEnums);
    obj_map->SetBool("sharedService", sharedService, !public_only && (sharedService == false));
}

spi::ObjectConstSP Service::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::string& name
        = obj_map->GetString("name");
    const std::vector<std::string>& description
        = obj_map->GetStringVector("description");
    const std::string& longName
        = obj_map->GetString("longName");
    const std::string& ns
        = obj_map->GetString("ns");
    const std::string& declSpec
        = obj_map->GetString("declSpec");
    const std::string& version
        = obj_map->GetString("version");
    const std::vector<ModuleConstSP>& modules
        = obj_map->GetInstanceVector<Module const>("modules", value_to_object);
    const std::vector<ClassConstSP>& importedBaseClasses
        = obj_map->GetInstanceVector<Class const>("importedBaseClasses", value_to_object);
    const std::vector<EnumConstSP>& importedEnums
        = obj_map->GetInstanceVector<Enum const>("importedEnums", value_to_object);
    bool sharedService
        = obj_map->GetBool("sharedService", true, false);

    return new Service(name, description, longName, ns, declSpec, version,
        modules, importedBaseClasses, importedEnums, sharedService);
}

SPI_IMPLEMENT_OBJECT_TYPE(Service, "Service", spdoc_service, false, 0);

spi::Value Service_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& name =
        in_context->ValueToString(in_values[0]);
    std::vector<std::string> description =
        in_context->ValueToStringVector(in_values[1]);
    const std::string& longName =
        in_context->ValueToString(in_values[2]);
    const std::string& ns =
        in_context->ValueToString(in_values[3]);
    const std::string& declSpec =
        in_context->ValueToString(in_values[4]);
    const std::string& version =
        in_context->ValueToString(in_values[5]);
    std::vector<ModuleConstSP> modules =
        in_context->ValueToInstanceVector<Module const>(in_values[6]);
    std::vector<ClassConstSP> importedBaseClasses =
        in_context->ValueToInstanceVector<Class const>(in_values[7]);
    std::vector<EnumConstSP> importedEnums =
        in_context->ValueToInstanceVector<Enum const>(in_values[8]);
    bool sharedService =
        in_context->ValueToBool(in_values[9], true, false);

    const ServiceConstSP& o_result = spdoc::Service::Make(name, description,
        longName, ns, declSpec, version, modules, importedBaseClasses,
        importedEnums, sharedService);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller Service_FunctionCaller = {
    "Service",
    10,
    {
        {"name", spi::ArgType::STRING, "string", false, false, false},
        {"description", spi::ArgType::STRING, "string", true, false, false},
        {"longName", spi::ArgType::STRING, "string", false, false, false},
        {"ns", spi::ArgType::STRING, "string", false, false, false},
        {"declSpec", spi::ArgType::STRING, "string", false, false, false},
        {"version", spi::ArgType::STRING, "string", false, false, false},
        {"modules", spi::ArgType::OBJECT, "Module", true, false, false},
        {"importedBaseClasses", spi::ArgType::OBJECT, "Class", true, false, false},
        {"importedEnums", spi::ArgType::OBJECT, "Enum", true, false, false},
        {"sharedService", spi::ArgType::BOOL, "bool", false, true, false}
    },
    Service_caller
};

spi::Value Service_Summary_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ServiceConstSP& self =
        in_context->ValueToInstance<Service const>(in_values[0]);
    bool sort =
        in_context->ValueToBool(in_values[1]);

    const std::vector<std::string>& o_result = self->Summary(sort);
    return o_result;
}

spi::FunctionCaller Service_Summary_FunctionCaller = {
    "Service.Summary",
    2,
    {
        {"self", spi::ArgType::OBJECT, "Service", false, false, false},
        {"sort", spi::ArgType::BOOL, "bool", false, false, false}
    },
    Service_Summary_caller
};

spi::ObjectType Service_Summary_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Service.Summary");

spi::Value Service_combineSummaries_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    std::vector<ServiceConstSP> services =
        in_context->ValueToInstanceVector<Service const>(in_values[0]);
    bool sort =
        in_context->ValueToBool(in_values[1]);

    const std::vector<std::string>& o_result = spdoc::Service::combineSummaries(
        services, sort);
    return o_result;
}

spi::FunctionCaller Service_combineSummaries_FunctionCaller = {
    "Service.combineSummaries",
    2,
    {
        {"services", spi::ArgType::OBJECT, "Service", true, false, false},
        {"sort", spi::ArgType::BOOL, "bool", false, false, false}
    },
    Service_combineSummaries_caller
};

spi::ObjectType Service_combineSummaries_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Service.combineSummaries");

spi::Value Service_getEnums_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ServiceConstSP& self =
        in_context->ValueToInstance<Service const>(in_values[0]);

    const std::vector<std::string>& o_result = self->getEnums();
    return o_result;
}

spi::FunctionCaller Service_getEnums_FunctionCaller = {
    "Service.getEnums",
    1,
    {
        {"self", spi::ArgType::OBJECT, "Service", false, false, false}
    },
    Service_getEnums_caller
};

spi::ObjectType Service_getEnums_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Service.getEnums");

spi::Value Service_getEnum_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ServiceConstSP& self =
        in_context->ValueToInstance<Service const>(in_values[0]);
    const std::string& name =
        in_context->ValueToString(in_values[1]);

    const EnumConstSP& o_result = self->getEnum(name);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller Service_getEnum_FunctionCaller = {
    "Service.getEnum",
    2,
    {
        {"self", spi::ArgType::OBJECT, "Service", false, false, false},
        {"name", spi::ArgType::STRING, "string", false, false, true}
    },
    Service_getEnum_caller
};

spi::ObjectType Service_getEnum_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Service.getEnum");

spi::Value Service_getEnumerands_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ServiceConstSP& self =
        in_context->ValueToInstance<Service const>(in_values[0]);
    const std::string& name =
        in_context->ValueToString(in_values[1]);

    const std::vector<std::string>& o_result = self->getEnumerands(name);
    return o_result;
}

spi::FunctionCaller Service_getEnumerands_FunctionCaller = {
    "Service.getEnumerands",
    2,
    {
        {"self", spi::ArgType::OBJECT, "Service", false, false, false},
        {"name", spi::ArgType::STRING, "string", false, false, false}
    },
    Service_getEnumerands_caller
};

spi::ObjectType Service_getEnumerands_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Service.getEnumerands");

spi::Value Service_getClasses_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ServiceConstSP& self =
        in_context->ValueToInstance<Service const>(in_values[0]);

    const std::vector<std::string>& o_result = self->getClasses();
    return o_result;
}

spi::FunctionCaller Service_getClasses_FunctionCaller = {
    "Service.getClasses",
    1,
    {
        {"self", spi::ArgType::OBJECT, "Service", false, false, false}
    },
    Service_getClasses_caller
};

spi::ObjectType Service_getClasses_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Service.getClasses");

spi::Value Service_getClass_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ServiceConstSP& self =
        in_context->ValueToInstance<Service const>(in_values[0]);
    const std::string& className =
        in_context->ValueToString(in_values[1]);

    const ClassConstSP& o_result = self->getClass(className);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller Service_getClass_FunctionCaller = {
    "Service.getClass",
    2,
    {
        {"self", spi::ArgType::OBJECT, "Service", false, false, false},
        {"className", spi::ArgType::STRING, "string", false, false, true}
    },
    Service_getClass_caller
};

spi::ObjectType Service_getClass_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Service.getClass");

spi::Value Service_isSubClass_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ServiceConstSP& self =
        in_context->ValueToInstance<Service const>(in_values[0]);
    const ClassConstSP& cls =
        in_context->ValueToInstance<Class const>(in_values[1]);
    const std::string& name =
        in_context->ValueToString(in_values[2]);

    bool o_result = self->isSubClass(cls, name);
    return o_result;
}

spi::FunctionCaller Service_isSubClass_FunctionCaller = {
    "Service.isSubClass",
    3,
    {
        {"self", spi::ArgType::OBJECT, "Service", false, false, false},
        {"cls", spi::ArgType::OBJECT, "Class", false, false, true},
        {"name", spi::ArgType::STRING, "string", false, false, true}
    },
    Service_isSubClass_caller
};

spi::ObjectType Service_isSubClass_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Service.isSubClass");

spi::Value Service_getPropertyClass_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ServiceConstSP& self =
        in_context->ValueToInstance<Service const>(in_values[0]);
    const std::string& baseClassName =
        in_context->ValueToString(in_values[1]);
    const std::string& fieldName =
        in_context->ValueToString(in_values[2]);

    const std::string& o_result = self->getPropertyClass(baseClassName,
        fieldName);
    return o_result;
}

spi::FunctionCaller Service_getPropertyClass_FunctionCaller = {
    "Service.getPropertyClass",
    3,
    {
        {"self", spi::ArgType::OBJECT, "Service", false, false, false},
        {"baseClassName", spi::ArgType::STRING, "string", false, false, true},
        {"fieldName", spi::ArgType::STRING, "string", false, false, true}
    },
    Service_getPropertyClass_caller
};

spi::ObjectType Service_getPropertyClass_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Service.getPropertyClass");

void configTypes_register_object_types(const spi::ServiceSP& svc)
{
    svc->add_object_type(&DataType::object_type);
    svc->add_function_caller(&DataType_FunctionCaller);
    svc->add_object_type(&DataType_ValueType_FunctionObjectType);
    svc->add_function_caller(&DataType_ValueType_FunctionCaller);
    svc->add_object_type(&DataType_RefType_FunctionObjectType);
    svc->add_function_caller(&DataType_RefType_FunctionCaller);
    svc->add_object_type(&Attribute::object_type);
    svc->add_function_caller(&Attribute_FunctionCaller);
    svc->add_object_type(&Attribute_encoding_FunctionObjectType);
    svc->add_function_caller(&Attribute_encoding_FunctionCaller);
    svc->add_object_type(&ClassAttribute::object_type);
    svc->add_function_caller(&ClassAttribute_FunctionCaller);
    svc->add_object_type(&Construct::object_type);
    svc->add_object_type(&Construct_getType_FunctionObjectType);
    svc->add_function_caller(&Construct_getType_FunctionCaller);
    svc->add_object_type(&Construct_Summary_FunctionObjectType);
    svc->add_function_caller(&Construct_Summary_FunctionCaller);
    svc->add_object_type(&SimpleType::object_type);
    svc->add_function_caller(&SimpleType_FunctionCaller);
    svc->add_object_type(&Function::object_type);
    svc->add_function_caller(&Function_FunctionCaller);
    svc->add_object_type(&Function_returnsObject_FunctionObjectType);
    svc->add_function_caller(&Function_returnsObject_FunctionCaller);
    svc->add_object_type(&Function_objectCount_FunctionObjectType);
    svc->add_function_caller(&Function_objectCount_FunctionCaller);
    svc->add_object_type(&Enumerand::object_type);
    svc->add_function_caller(&Enumerand_FunctionCaller);
    svc->add_object_type(&Enum::object_type);
    svc->add_function_caller(&Enum_FunctionCaller);
    svc->add_object_type(&ClassMethod::object_type);
    svc->add_function_caller(&ClassMethod_FunctionCaller);
    svc->add_object_type(&ClassMethod_Summary_FunctionObjectType);
    svc->add_function_caller(&ClassMethod_Summary_FunctionCaller);
    svc->add_object_type(&CoerceFrom::object_type);
    svc->add_function_caller(&CoerceFrom_FunctionCaller);
    svc->add_object_type(&CoerceFrom_Summary_FunctionObjectType);
    svc->add_function_caller(&CoerceFrom_Summary_FunctionCaller);
    svc->add_object_type(&CoerceTo::object_type);
    svc->add_function_caller(&CoerceTo_FunctionCaller);
    svc->add_object_type(&CoerceTo_Summary_FunctionObjectType);
    svc->add_function_caller(&CoerceTo_Summary_FunctionCaller);
    svc->add_object_type(&Class::object_type);
    svc->add_function_caller(&Class_FunctionCaller);
    svc->add_object_type(&Class_ObjectName_FunctionObjectType);
    svc->add_function_caller(&Class_ObjectName_FunctionCaller);
    svc->add_object_type(&Class_ServiceNamespace_FunctionObjectType);
    svc->add_function_caller(&Class_ServiceNamespace_FunctionCaller);
    svc->add_object_type(&Module::object_type);
    svc->add_function_caller(&Module_FunctionCaller);
    svc->add_object_type(&Module_combineSummaries_FunctionObjectType);
    svc->add_function_caller(&Module_combineSummaries_FunctionCaller);
    svc->add_object_type(&Service::object_type);
    svc->add_function_caller(&Service_FunctionCaller);
    svc->add_object_type(&Service_Summary_FunctionObjectType);
    svc->add_function_caller(&Service_Summary_FunctionCaller);
    svc->add_object_type(&Service_combineSummaries_FunctionObjectType);
    svc->add_function_caller(&Service_combineSummaries_FunctionCaller);
    svc->add_object_type(&Service_getEnums_FunctionObjectType);
    svc->add_function_caller(&Service_getEnums_FunctionCaller);
    svc->add_object_type(&Service_getEnum_FunctionObjectType);
    svc->add_function_caller(&Service_getEnum_FunctionCaller);
    svc->add_object_type(&Service_getEnumerands_FunctionObjectType);
    svc->add_function_caller(&Service_getEnumerands_FunctionCaller);
    svc->add_object_type(&Service_getClasses_FunctionObjectType);
    svc->add_function_caller(&Service_getClasses_FunctionCaller);
    svc->add_object_type(&Service_getClass_FunctionObjectType);
    svc->add_function_caller(&Service_getClass_FunctionCaller);
    svc->add_object_type(&Service_isSubClass_FunctionObjectType);
    svc->add_function_caller(&Service_isSubClass_FunctionCaller);
    svc->add_object_type(&Service_getPropertyClass_FunctionObjectType);
    svc->add_function_caller(&Service_getPropertyClass_FunctionCaller);
}

SPDOC_END_NAMESPACE

