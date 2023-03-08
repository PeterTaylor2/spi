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
* Source file: spi_replay_replay_helper.cpp
****************************************************************************
*/

#include "spi_replay_replay.hpp"
#include "spi_replay_replay_helper.hpp"
#include "replay_dll_service_manager.hpp"

SPI_REPLAY_BEGIN_NAMESPACE

/*
****************************************************************************
* Implementation of ReplayAction
****************************************************************************
*/

ReplayAction::outer_type
ReplayAction::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return ReplayAction::outer_type();

    // isInstance
    const ReplayAction* p = dynamic_cast<const ReplayAction*>(o.get());
    if (p)
        return ReplayAction::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const ReplayAction*>(o2.get());
            if (p)
                return ReplayAction::outer_type(p);
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
            p = dynamic_cast<const ReplayAction*>(o3.get());
            if (p)
                return ReplayAction::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void ReplayAction::to_map(
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

spi::ObjectConstSP ReplayAction::object_from_map(
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

SPI_IMPLEMENT_OBJECT_TYPE(ReplayAction, "ReplayAction", replay_service, false, 0);

std::vector<ReplayAction::sub_class_wrapper*> ReplayAction::g_sub_class_wrappers;

/*
****************************************************************************
* Implementation of ReplayFunctionAction
****************************************************************************
*/

ReplayFunctionAction::outer_type
ReplayFunctionAction::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return ReplayFunctionAction::outer_type();

    // isInstance
    const ReplayFunctionAction* p = dynamic_cast<const ReplayFunctionAction*>(o.get());
    if (p)
        return ReplayFunctionAction::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const ReplayFunctionAction*>(o2.get());
            if (p)
                return ReplayFunctionAction::outer_type(p);
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
            p = dynamic_cast<const ReplayFunctionAction*>(o3.get());
            if (p)
                return ReplayFunctionAction::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void ReplayFunctionAction::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    const spi::MapObjectSP& inputs = this->inputs();
    const spi::MapObjectSP& output = this->output();

    obj_map->SetObject("inputs", inputs);
    obj_map->SetObject("output", output);
}

spi::ObjectConstSP ReplayFunctionAction::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const spi::MapObjectSP& inputs
        = obj_map->GetInstance<spi::MapObject>("inputs", value_to_object);
    const spi::MapObjectSP& output
        = obj_map->GetInstance<spi::MapObject>("output", value_to_object);

    return ReplayFunctionAction::Make(inputs, output);
}

SPI_IMPLEMENT_OBJECT_TYPE(ReplayFunctionAction, "ReplayFunctionAction", replay_service, false, 0);

spi::Value ReplayFunctionAction_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const spi::MapObjectSP& inputs =
        in_context->ValueToInstance<spi::MapObject>(in_values[0]);
    const spi::MapObjectSP& output =
        in_context->ValueToInstance<spi::MapObject>(in_values[1]);

    const ReplayFunctionActionConstSP& o_result = spi_replay::ReplayFunctionAction::Make(
        inputs, output);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller ReplayFunctionAction_FunctionCaller = {
    "ReplayFunctionAction",
    2,
    {
        {"inputs", spi::ArgType::OBJECT, "Map", false, false, false},
        {"output", spi::ArgType::OBJECT, "Map", false, false, false}
    },
    ReplayFunctionAction_caller
};

/*
****************************************************************************
* Implementation of ReplayObjectAction
****************************************************************************
*/

ReplayObjectAction::outer_type
ReplayObjectAction::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return ReplayObjectAction::outer_type();

    // isInstance
    const ReplayObjectAction* p = dynamic_cast<const ReplayObjectAction*>(o.get());
    if (p)
        return ReplayObjectAction::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const ReplayObjectAction*>(o2.get());
            if (p)
                return ReplayObjectAction::outer_type(p);
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
            p = dynamic_cast<const ReplayObjectAction*>(o3.get());
            if (p)
                return ReplayObjectAction::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void ReplayObjectAction::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    const spi::MapObjectSP& inputs = this->inputs();

    obj_map->SetObject("inputs", inputs);
}

spi::ObjectConstSP ReplayObjectAction::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const spi::MapObjectSP& inputs
        = obj_map->GetInstance<spi::MapObject>("inputs", value_to_object);

    return ReplayObjectAction::Make(inputs);
}

SPI_IMPLEMENT_OBJECT_TYPE(ReplayObjectAction, "ReplayObjectAction", replay_service, false, 0);

spi::Value ReplayObjectAction_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const spi::MapObjectSP& inputs =
        in_context->ValueToInstance<spi::MapObject>(in_values[0]);

    const ReplayObjectActionConstSP& o_result = spi_replay::ReplayObjectAction::Make(
        inputs);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller ReplayObjectAction_FunctionCaller = {
    "ReplayObjectAction",
    1,
    {
        {"inputs", spi::ArgType::OBJECT, "Map", false, false, false}
    },
    ReplayObjectAction_caller
};

/*
****************************************************************************
* Implementation of ReplayCodeGenerator
****************************************************************************
*/

ReplayCodeGenerator::outer_type
ReplayCodeGenerator::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return ReplayCodeGenerator::outer_type();

    // isInstance
    const ReplayCodeGenerator* p = dynamic_cast<const ReplayCodeGenerator*>(o.get());
    if (p)
        return ReplayCodeGenerator::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const ReplayCodeGenerator*>(o2.get());
            if (p)
                return ReplayCodeGenerator::outer_type(p);
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
            p = dynamic_cast<const ReplayCodeGenerator*>(o3.get());
            if (p)
                return ReplayCodeGenerator::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void ReplayCodeGenerator::to_map(
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

spi::ObjectConstSP ReplayCodeGenerator::object_from_map(
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

SPI_IMPLEMENT_OBJECT_TYPE(ReplayCodeGenerator, "ReplayCodeGenerator", replay_service, false, 0);

std::vector<ReplayCodeGenerator::sub_class_wrapper*> ReplayCodeGenerator::g_sub_class_wrappers;

spi::Value ReplayCodeGenerator_GenerateFunction_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ReplayCodeGeneratorConstSP& self =
        in_context->ValueToInstance<ReplayCodeGenerator const>(in_values[0]);
    const ReplayFunctionActionConstSP& action =
        in_context->ValueToInstance<ReplayFunctionAction const>(in_values[1]);

    self->GenerateFunction(action);
    return spi::Value();
}

spi::FunctionCaller ReplayCodeGenerator_GenerateFunction_FunctionCaller = {
    "ReplayCodeGenerator.GenerateFunction",
    2,
    {
        {"self", spi::ArgType::OBJECT, "ReplayCodeGenerator", false, false, false},
        {"action", spi::ArgType::OBJECT, "ReplayFunctionAction", false, false, true}
    },
    ReplayCodeGenerator_GenerateFunction_caller
};

spi::ObjectType ReplayCodeGenerator_GenerateFunction_FunctionObjectType =
    spi::FunctionObjectType("spi_replay.ReplayCodeGenerator.GenerateFunction");

spi::Value ReplayCodeGenerator_GenerateObject_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ReplayCodeGeneratorConstSP& self =
        in_context->ValueToInstance<ReplayCodeGenerator const>(in_values[0]);
    const ReplayObjectActionConstSP& action =
        in_context->ValueToInstance<ReplayObjectAction const>(in_values[1]);

    self->GenerateObject(action);
    return spi::Value();
}

spi::FunctionCaller ReplayCodeGenerator_GenerateObject_FunctionCaller = {
    "ReplayCodeGenerator.GenerateObject",
    2,
    {
        {"self", spi::ArgType::OBJECT, "ReplayCodeGenerator", false, false, false},
        {"action", spi::ArgType::OBJECT, "ReplayObjectAction", false, false, true}
    },
    ReplayCodeGenerator_GenerateObject_caller
};

spi::ObjectType ReplayCodeGenerator_GenerateObject_FunctionObjectType =
    spi::FunctionObjectType("spi_replay.ReplayCodeGenerator.GenerateObject");

/*
****************************************************************************
* Implementation of ReplayLog
****************************************************************************
*/

ReplayLog::outer_type
ReplayLog::Coerce(const spi::ObjectConstSP& o)
{
    // isNull
    if (!o)
        return ReplayLog::outer_type();

    // isInstance
    const ReplayLog* p = dynamic_cast<const ReplayLog*>(o.get());
    if (p)
        return ReplayLog::outer_type(p);

    spi::ObjectType* ot = &object_type;
    spi::ObjectConstSP o2;

    // service coercion if the input object is from another service
    if (ot->get_service() != o->get_service())
    {
        o2 = o->service_coercion(ot->get_service());
        if (o2)
        {
            p = dynamic_cast<const ReplayLog*>(o2.get());
            if (p)
                return ReplayLog::outer_type(p);
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
            p = dynamic_cast<const ReplayLog*>(o3.get());
            if (p)
                return ReplayLog::outer_type(p);
        }
    }

    SPI_THROW_RUNTIME_ERROR(o->get_class_name()
        << " is neither " << ot->get_class_name()
        << " nor a sub-class of " << ot->get_class_name());
}

void ReplayLog::to_map(
    spi::IObjectMap* obj_map, bool public_only) const
{
    const std::vector<spi::MapObjectSP>& items = this->items();

    obj_map->SetInstanceVector<spi::MapObject>("items", items);
}

spi::ObjectConstSP ReplayLog::object_from_map(
    spi::IObjectMap* obj_map,
    spi::ValueToObject& value_to_object)
{
    const std::vector<spi::MapObjectSP>& items
        = obj_map->GetInstanceVector<spi::MapObject>("items", value_to_object);

    return ReplayLog::Make(items);
}

SPI_IMPLEMENT_OBJECT_TYPE(ReplayLog, "ReplayLog", replay_service, false, 0);

spi::Value ReplayLog_generateCode_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const ReplayLogConstSP& self =
        in_context->ValueToInstance<ReplayLog const>(in_values[0]);
    const ReplayCodeGeneratorConstSP& generator =
        in_context->ValueToInstance<ReplayCodeGenerator const>(in_values[1]);

    self->generateCode(generator);
    return spi::Value();
}

spi::FunctionCaller ReplayLog_generateCode_FunctionCaller = {
    "ReplayLog.generateCode",
    2,
    {
        {"self", spi::ArgType::OBJECT, "ReplayLog", false, false, false},
        {"generator", spi::ArgType::OBJECT, "ReplayCodeGenerator", false, false, true}
    },
    ReplayLog_generateCode_caller
};

spi::ObjectType ReplayLog_generateCode_FunctionObjectType =
    spi::FunctionObjectType("spi_replay.ReplayLog.generateCode");

spi::Value ReplayLog_Read_caller(
    const spi::InputContext*       in_context,
    const std::vector<spi::Value>& in_values)
{
    const std::string& infilename =
        in_context->ValueToString(in_values[0]);

    const ReplayLogConstSP& o_result = spi_replay::ReplayLog::Read(infilename);
    return spi::ObjectConstSP(o_result);
}

spi::FunctionCaller ReplayLog_Read_FunctionCaller = {
    "ReplayLog.Read",
    1,
    {
        {"infilename", spi::ArgType::STRING, "string", false, false, true}
    },
    ReplayLog_Read_caller
};

spi::ObjectType ReplayLog_Read_FunctionObjectType =
    spi::FunctionObjectType("spi_replay.ReplayLog.Read");

void replay_register_object_types(const spi::ServiceSP& svc)
{
    svc->add_object_type(&ReplayAction::object_type);
    svc->add_object_type(&ReplayFunctionAction::object_type);
    svc->add_function_caller(&ReplayFunctionAction_FunctionCaller);
    ReplayAction::g_sub_class_wrappers.push_back(ReplayFunctionAction::BaseWrap);
    svc->add_object_type(&ReplayObjectAction::object_type);
    svc->add_function_caller(&ReplayObjectAction_FunctionCaller);
    ReplayAction::g_sub_class_wrappers.push_back(ReplayObjectAction::BaseWrap);
    svc->add_object_type(&ReplayCodeGenerator::object_type);
    svc->add_object_type(&ReplayCodeGenerator_GenerateFunction_FunctionObjectType);
    svc->add_function_caller(&ReplayCodeGenerator_GenerateFunction_FunctionCaller);
    svc->add_object_type(&ReplayCodeGenerator_GenerateObject_FunctionObjectType);
    svc->add_function_caller(&ReplayCodeGenerator_GenerateObject_FunctionCaller);
    svc->add_object_type(&ReplayLog::object_type);
    svc->add_object_type(&ReplayLog_generateCode_FunctionObjectType);
    svc->add_function_caller(&ReplayLog_generateCode_FunctionCaller);
    svc->add_object_type(&ReplayLog_Read_FunctionObjectType);
    svc->add_function_caller(&ReplayLog_Read_FunctionCaller);
}

SPI_REPLAY_END_NAMESPACE

