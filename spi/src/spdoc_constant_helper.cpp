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
* Source file: spdoc_constant_helper.cpp
****************************************************************************
*/

#include "spdoc_constant.hpp"
#include "spdoc_constant_helper.hpp"
#include "spdoc_dll_service_manager.hpp"

SPDOC_BEGIN_NAMESPACE

/*
****************************************************************************
* Implementation of Constant
****************************************************************************
*/

Constant::outer_type
Constant::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return Constant::outer_type();

    // isInstance
    const Constant* p = dynamic_cast<const Constant*>(o.get());
    if (p)
        return Constant::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const Constant*>(o2.get());
            if (p)
                return Constant::outer_type(p);
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
            p = dynamic_cast<const Constant*>(o3.get());
            if (p)
                return Constant::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void Constant::to_map(
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

spi::ObjectConstSP Constant::object_from_map(
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

SPI_IMPLEMENT_OBJECT_TYPE(Constant, "Constant", spdoc_service, false, 0);

spi::Value Constant_typeName_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ConstantConstSP& self =
        in_context->ValueToInstance<Constant const>(in_values[0]);

    const std::string& o_result = self->typeName();
    return o_result;
}

spi::FunctionCaller Constant_typeName_FunctionCaller = {
    "Constant.typeName",
    1,
    {
        {"self", spi::ArgType::OBJECT, "Constant", false, false, false}
    },
    Constant_typeName_caller
};

spi::ObjectType Constant_typeName_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Constant.typeName");

spi::Value Constant_docString_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ConstantConstSP& self =
        in_context->ValueToInstance<Constant const>(in_values[0]);

    const std::string& o_result = self->docString();
    return o_result;
}

spi::FunctionCaller Constant_docString_FunctionCaller = {
    "Constant.docString",
    1,
    {
        {"self", spi::ArgType::OBJECT, "Constant", false, false, false}
    },
    Constant_docString_caller
};

spi::ObjectType Constant_docString_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Constant.docString");

spi::Value Constant_getInt_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ConstantConstSP& self =
        in_context->ValueToInstance<Constant const>(in_values[0]);

    int o_result = self->getInt();
    return o_result;
}

spi::FunctionCaller Constant_getInt_FunctionCaller = {
    "Constant.getInt",
    1,
    {
        {"self", spi::ArgType::OBJECT, "Constant", false, false, false}
    },
    Constant_getInt_caller
};

spi::ObjectType Constant_getInt_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Constant.getInt");

spi::Value Constant_getDate_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ConstantConstSP& self =
        in_context->ValueToInstance<Constant const>(in_values[0]);

    spi::Date o_result = self->getDate();
    return o_result;
}

spi::FunctionCaller Constant_getDate_FunctionCaller = {
    "Constant.getDate",
    1,
    {
        {"self", spi::ArgType::OBJECT, "Constant", false, false, false}
    },
    Constant_getDate_caller
};

spi::ObjectType Constant_getDate_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Constant.getDate");

spi::Value Constant_getDateTime_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ConstantConstSP& self =
        in_context->ValueToInstance<Constant const>(in_values[0]);

    spi::DateTime o_result = self->getDateTime();
    return o_result;
}

spi::FunctionCaller Constant_getDateTime_FunctionCaller = {
    "Constant.getDateTime",
    1,
    {
        {"self", spi::ArgType::OBJECT, "Constant", false, false, false}
    },
    Constant_getDateTime_caller
};

spi::ObjectType Constant_getDateTime_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Constant.getDateTime");

spi::Value Constant_getDouble_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ConstantConstSP& self =
        in_context->ValueToInstance<Constant const>(in_values[0]);

    double o_result = self->getDouble();
    return o_result;
}

spi::FunctionCaller Constant_getDouble_FunctionCaller = {
    "Constant.getDouble",
    1,
    {
        {"self", spi::ArgType::OBJECT, "Constant", false, false, false}
    },
    Constant_getDouble_caller
};

spi::ObjectType Constant_getDouble_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Constant.getDouble");

spi::Value Constant_getChar_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ConstantConstSP& self =
        in_context->ValueToInstance<Constant const>(in_values[0]);

    char o_result = self->getChar();
    return o_result;
}

spi::FunctionCaller Constant_getChar_FunctionCaller = {
    "Constant.getChar",
    1,
    {
        {"self", spi::ArgType::OBJECT, "Constant", false, false, false}
    },
    Constant_getChar_caller
};

spi::ObjectType Constant_getChar_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Constant.getChar");

spi::Value Constant_getString_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ConstantConstSP& self =
        in_context->ValueToInstance<Constant const>(in_values[0]);

    const std::string& o_result = self->getString();
    return o_result;
}

spi::FunctionCaller Constant_getString_FunctionCaller = {
    "Constant.getString",
    1,
    {
        {"self", spi::ArgType::OBJECT, "Constant", false, false, false}
    },
    Constant_getString_caller
};

spi::ObjectType Constant_getString_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Constant.getString");

spi::Value Constant_getBool_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ConstantConstSP& self =
        in_context->ValueToInstance<Constant const>(in_values[0]);

    bool o_result = self->getBool();
    return o_result;
}

spi::FunctionCaller Constant_getBool_FunctionCaller = {
    "Constant.getBool",
    1,
    {
        {"self", spi::ArgType::OBJECT, "Constant", false, false, false}
    },
    Constant_getBool_caller
};

spi::ObjectType Constant_getBool_FunctionObjectType =
    spi::FunctionObjectType("spdoc.Constant.getBool");

/*
****************************************************************************
* Implementation of IntConstant
****************************************************************************
*/

IntConstant::outer_type
IntConstant::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return IntConstant::outer_type();

    // isInstance
    const IntConstant* p = dynamic_cast<const IntConstant*>(o.get());
    if (p)
        return IntConstant::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const IntConstant*>(o2.get());
            if (p)
                return IntConstant::outer_type(p);
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
            p = dynamic_cast<const IntConstant*>(o3.get());
            if (p)
                return IntConstant::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void IntConstant::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    obj_map->SetInt("value", value);
}

spi::ObjectConstSP IntConstant::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    int value
        = obj_map->GetInt("value");

    return new IntConstant(value);
}

SPI_IMPLEMENT_OBJECT_TYPE(IntConstant, "IntConstant", spdoc_service, false, 0);

spi::Value IntConstant_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    int value =
        in_context->ValueToInt(in_values[0]);

    const IntConstantConstSP& o_result = spdoc::IntConstant::Make(value);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller IntConstant_FunctionCaller = {
    "IntConstant",
    1,
    {
        {"value", spi::ArgType::INT, "int", false, false, false}
    },
    IntConstant_caller
};

/*
****************************************************************************
* Implementation of DateConstant
****************************************************************************
*/

DateConstant::outer_type
DateConstant::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return DateConstant::outer_type();

    // isInstance
    const DateConstant* p = dynamic_cast<const DateConstant*>(o.get());
    if (p)
        return DateConstant::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const DateConstant*>(o2.get());
            if (p)
                return DateConstant::outer_type(p);
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
            p = dynamic_cast<const DateConstant*>(o3.get());
            if (p)
                return DateConstant::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void DateConstant::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    obj_map->SetDate("value", value);
}

spi::ObjectConstSP DateConstant::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    spi::Date value
        = obj_map->GetDate("value");

    return new DateConstant(value);
}

SPI_IMPLEMENT_OBJECT_TYPE(DateConstant, "DateConstant", spdoc_service, false, 0);

spi::Value DateConstant_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    spi::Date value =
        in_context->ValueToDate(in_values[0]);

    const DateConstantConstSP& o_result = spdoc::DateConstant::Make(value);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller DateConstant_FunctionCaller = {
    "DateConstant",
    1,
    {
        {"value", spi::ArgType::DATE, "date", false, false, false}
    },
    DateConstant_caller
};

/*
****************************************************************************
* Implementation of DateTimeConstant
****************************************************************************
*/

DateTimeConstant::outer_type
DateTimeConstant::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return DateTimeConstant::outer_type();

    // isInstance
    const DateTimeConstant* p = dynamic_cast<const DateTimeConstant*>(o.get());
    if (p)
        return DateTimeConstant::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const DateTimeConstant*>(o2.get());
            if (p)
                return DateTimeConstant::outer_type(p);
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
            p = dynamic_cast<const DateTimeConstant*>(o3.get());
            if (p)
                return DateTimeConstant::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void DateTimeConstant::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    obj_map->SetDateTime("value", value);
}

spi::ObjectConstSP DateTimeConstant::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    spi::DateTime value
        = obj_map->GetDateTime("value");

    return new DateTimeConstant(value);
}

SPI_IMPLEMENT_OBJECT_TYPE(DateTimeConstant, "DateTimeConstant", spdoc_service, false, 0);

spi::Value DateTimeConstant_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    spi::DateTime value =
        in_context->ValueToDateTime(in_values[0]);

    const DateTimeConstantConstSP& o_result = spdoc::DateTimeConstant::Make(
        value);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller DateTimeConstant_FunctionCaller = {
    "DateTimeConstant",
    1,
    {
        {"value", spi::ArgType::DATETIME, "datetime", false, false, false}
    },
    DateTimeConstant_caller
};

/*
****************************************************************************
* Implementation of DoubleConstant
****************************************************************************
*/

DoubleConstant::outer_type
DoubleConstant::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return DoubleConstant::outer_type();

    // isInstance
    const DoubleConstant* p = dynamic_cast<const DoubleConstant*>(o.get());
    if (p)
        return DoubleConstant::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const DoubleConstant*>(o2.get());
            if (p)
                return DoubleConstant::outer_type(p);
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
            p = dynamic_cast<const DoubleConstant*>(o3.get());
            if (p)
                return DoubleConstant::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void DoubleConstant::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    obj_map->SetDouble("value", value);
}

spi::ObjectConstSP DoubleConstant::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    double value
        = obj_map->GetDouble("value");

    return new DoubleConstant(value);
}

SPI_IMPLEMENT_OBJECT_TYPE(DoubleConstant, "DoubleConstant", spdoc_service, false, 0);

spi::Value DoubleConstant_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    double value =
        in_context->ValueToDouble(in_values[0]);

    const DoubleConstantConstSP& o_result = spdoc::DoubleConstant::Make(value);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller DoubleConstant_FunctionCaller = {
    "DoubleConstant",
    1,
    {
        {"value", spi::ArgType::DOUBLE, "double", false, false, false}
    },
    DoubleConstant_caller
};

/*
****************************************************************************
* Implementation of CharConstant
****************************************************************************
*/

CharConstant::outer_type
CharConstant::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return CharConstant::outer_type();

    // isInstance
    const CharConstant* p = dynamic_cast<const CharConstant*>(o.get());
    if (p)
        return CharConstant::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const CharConstant*>(o2.get());
            if (p)
                return CharConstant::outer_type(p);
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
            p = dynamic_cast<const CharConstant*>(o3.get());
            if (p)
                return CharConstant::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void CharConstant::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    obj_map->SetChar("value", value);
}

spi::ObjectConstSP CharConstant::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    char value
        = obj_map->GetChar("value");

    return new CharConstant(value);
}

SPI_IMPLEMENT_OBJECT_TYPE(CharConstant, "CharConstant", spdoc_service, false, 0);

spi::Value CharConstant_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    char value =
        in_context->ValueToChar(in_values[0]);

    const CharConstantConstSP& o_result = spdoc::CharConstant::Make(value);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller CharConstant_FunctionCaller = {
    "CharConstant",
    1,
    {
        {"value", spi::ArgType::CHAR, "char", false, false, false}
    },
    CharConstant_caller
};

/*
****************************************************************************
* Implementation of StringConstant
****************************************************************************
*/

StringConstant::outer_type
StringConstant::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return StringConstant::outer_type();

    // isInstance
    const StringConstant* p = dynamic_cast<const StringConstant*>(o.get());
    if (p)
        return StringConstant::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const StringConstant*>(o2.get());
            if (p)
                return StringConstant::outer_type(p);
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
            p = dynamic_cast<const StringConstant*>(o3.get());
            if (p)
                return StringConstant::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void StringConstant::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    obj_map->SetString("value", value);
}

spi::ObjectConstSP StringConstant::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::string& value
        = obj_map->GetString("value");

    return new StringConstant(value);
}

SPI_IMPLEMENT_OBJECT_TYPE(StringConstant, "StringConstant", spdoc_service, false, 0);

spi::Value StringConstant_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& value =
        in_context->ValueToString(in_values[0]);

    const StringConstantConstSP& o_result = spdoc::StringConstant::Make(value);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller StringConstant_FunctionCaller = {
    "StringConstant",
    1,
    {
        {"value", spi::ArgType::STRING, "string", false, false, false}
    },
    StringConstant_caller
};

/*
****************************************************************************
* Implementation of BoolConstant
****************************************************************************
*/

BoolConstant::outer_type
BoolConstant::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return BoolConstant::outer_type();

    // isInstance
    const BoolConstant* p = dynamic_cast<const BoolConstant*>(o.get());
    if (p)
        return BoolConstant::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const BoolConstant*>(o2.get());
            if (p)
                return BoolConstant::outer_type(p);
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
            p = dynamic_cast<const BoolConstant*>(o3.get());
            if (p)
                return BoolConstant::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void BoolConstant::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    obj_map->SetBool("value", value);
}

spi::ObjectConstSP BoolConstant::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    bool value
        = obj_map->GetBool("value");

    return new BoolConstant(value);
}

SPI_IMPLEMENT_OBJECT_TYPE(BoolConstant, "BoolConstant", spdoc_service, false, 0);

spi::Value BoolConstant_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    bool value =
        in_context->ValueToBool(in_values[0]);

    const BoolConstantConstSP& o_result = spdoc::BoolConstant::Make(value);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller BoolConstant_FunctionCaller = {
    "BoolConstant",
    1,
    {
        {"value", spi::ArgType::BOOL, "bool", false, false, false}
    },
    BoolConstant_caller
};

/*
****************************************************************************
* Implementation of UndefinedConstant
****************************************************************************
*/

UndefinedConstant::outer_type
UndefinedConstant::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return UndefinedConstant::outer_type();

    // isInstance
    const UndefinedConstant* p = dynamic_cast<const UndefinedConstant*>(o.get());
    if (p)
        return UndefinedConstant::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const UndefinedConstant*>(o2.get());
            if (p)
                return UndefinedConstant::outer_type(p);
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
            p = dynamic_cast<const UndefinedConstant*>(o3.get());
            if (p)
                return UndefinedConstant::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void UndefinedConstant::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
}

spi::ObjectConstSP UndefinedConstant::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{

    return new UndefinedConstant();
}

SPI_IMPLEMENT_OBJECT_TYPE(UndefinedConstant, "UndefinedConstant", spdoc_service, false, 0);

spi::Value UndefinedConstant_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const UndefinedConstantConstSP& o_result = spdoc::UndefinedConstant::Make();
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller UndefinedConstant_FunctionCaller = {
    "UndefinedConstant",
    0,
    {},
    UndefinedConstant_caller
};

void constant_register_object_types(const spi::ServiceSP& svc)
{
    svc->add_object_type(&Constant::object_type);
    svc->add_object_type(&Constant_typeName_FunctionObjectType);
    svc->add_function_caller(&Constant_typeName_FunctionCaller);
    svc->add_object_type(&Constant_docString_FunctionObjectType);
    svc->add_function_caller(&Constant_docString_FunctionCaller);
    svc->add_object_type(&Constant_getInt_FunctionObjectType);
    svc->add_function_caller(&Constant_getInt_FunctionCaller);
    svc->add_object_type(&Constant_getDate_FunctionObjectType);
    svc->add_function_caller(&Constant_getDate_FunctionCaller);
    svc->add_object_type(&Constant_getDateTime_FunctionObjectType);
    svc->add_function_caller(&Constant_getDateTime_FunctionCaller);
    svc->add_object_type(&Constant_getDouble_FunctionObjectType);
    svc->add_function_caller(&Constant_getDouble_FunctionCaller);
    svc->add_object_type(&Constant_getChar_FunctionObjectType);
    svc->add_function_caller(&Constant_getChar_FunctionCaller);
    svc->add_object_type(&Constant_getString_FunctionObjectType);
    svc->add_function_caller(&Constant_getString_FunctionCaller);
    svc->add_object_type(&Constant_getBool_FunctionObjectType);
    svc->add_function_caller(&Constant_getBool_FunctionCaller);
    svc->add_object_type(&IntConstant::object_type);
    svc->add_function_caller(&IntConstant_FunctionCaller);
    svc->add_object_type(&DateConstant::object_type);
    svc->add_function_caller(&DateConstant_FunctionCaller);
    svc->add_object_type(&DateTimeConstant::object_type);
    svc->add_function_caller(&DateTimeConstant_FunctionCaller);
    svc->add_object_type(&DoubleConstant::object_type);
    svc->add_function_caller(&DoubleConstant_FunctionCaller);
    svc->add_object_type(&CharConstant::object_type);
    svc->add_function_caller(&CharConstant_FunctionCaller);
    svc->add_object_type(&StringConstant::object_type);
    svc->add_function_caller(&StringConstant_FunctionCaller);
    svc->add_object_type(&BoolConstant::object_type);
    svc->add_function_caller(&BoolConstant_FunctionCaller);
    svc->add_object_type(&UndefinedConstant::object_type);
    svc->add_function_caller(&UndefinedConstant_FunctionCaller);
}

SPDOC_END_NAMESPACE

