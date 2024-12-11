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
* Source file: spi_replay_replay.cpp
****************************************************************************
*/

#include "spi_replay_replay.hpp"
#include "spi_replay_replay_helper.hpp"
#include "replay_dll_service_manager.hpp"

#include <spi/Replay.hpp>

SPI_REPLAY_BEGIN_NAMESPACE

/*
****************************************************************************
* Implementation of ReplayAction
****************************************************************************
*/

ReplayAction::inner_type ReplayAction::get_inner() const
{
    return inner_value.get();
}

ReplayAction::inner_type ReplayAction::get_inner(const ReplayAction::outer_type& o)
{
    if (!o)
        return inner_type();
    return o->get_inner();
}

void ReplayAction::set_inner(const spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayAction const >& self)
{
    SPI_PRE_CONDITION(!inner_value);
    inner_value = self;
}

ReplayAction::ReplayAction()
{}

ReplayAction::~ReplayAction()
{}

ReplayActionConstSP ReplayAction::Wrap(const inner_type& inner)
{
    if (!inner)
        return ReplayActionConstSP();
    for (size_t i = 0; i < g_sub_class_wrappers.size(); ++i)
    {
        ReplayActionConstSP o = g_sub_class_wrappers[i](inner);
        if (o)
            return o;
    }
    throw spi::RuntimeError("%s: Failed!", __FUNCTION__);
}

/*
****************************************************************************
* Implementation of ReplayFunctionAction
****************************************************************************
*/
ReplayFunctionActionConstSP ReplayFunctionAction::Make(
    const spi::MapObjectSP& inputs,
    const spi::MapObjectSP& output)
{
  SPI_PROFILE("spi_replay.ReplayFunctionAction.Make");
  replay_check_permission();
  try
  {
    inner_type self = make_inner(inputs, output);
    return Wrap(self);
  }
  catch (std::exception& e)
  { SPI_THROW_RUNTIME_ERROR("Could not create ReplayFunctionAction object:\n" << e.what()); }
  catch (...)
  { SPI_THROW_RUNTIME_ERROR("Could not create ReplayFunctionAction object:\n" << "Unknown exception"); }
}

ReplayFunctionAction::inner_type ReplayFunctionAction::make_inner(
    const spi::MapObjectSP& inputs,
    const spi::MapObjectSP& output)
{
    spi::MapConstSP i_inputs;
    spi::MapConstSP i_output;

    i_inputs = spi::MapObject::get_inner(inputs);
    i_output = spi::MapObject::get_inner(output);
    ReplayFunctionAction::inner_type self (new SPI_NAMESPACE::ReplayFunctionAction(
        i_inputs, i_output));
    return self;
}

ReplayFunctionAction::inner_type ReplayFunctionAction::get_inner() const
{
    return inner_value.get();
}

ReplayFunctionAction::inner_type ReplayFunctionAction::get_inner(const ReplayFunctionAction::outer_type& o)
{
    if (!o)
        return inner_type();
    return o->get_inner();
}

void ReplayFunctionAction::set_inner(const spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayFunctionAction const >& self)
{
    SPI_PRE_CONDITION(!inner_value);
    inner_value = self;
    ReplayAction::set_inner(self);
}

ReplayFunctionAction::~ReplayFunctionAction()
{}

ReplayActionConstSP ReplayFunctionAction::BaseWrap(const ReplayAction::inner_type& baseInner)
{
    inner_type inner = dynamic_cast<inner_type>(baseInner);
    if (!inner)
        return ReplayActionConstSP();
    return Wrap(inner);
}

ReplayFunctionActionConstSP ReplayFunctionAction::Wrap(const inner_type& inner)
{
    if (!inner)
        return ReplayFunctionActionConstSP();
    return ReplayFunctionActionConstSP(new ReplayFunctionAction(inner));
}

ReplayFunctionAction::ReplayFunctionAction(const inner_type& inner)
{
    set_inner(spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayFunctionAction const >(inner));
}

spi::MapObjectSP ReplayFunctionAction::inputs() const
{
    spi::MapConstSP i_value = ReplayFunctionAction_Helper::get_inputs(this);
    return spi::MapObject::Make(i_value);
}

spi::MapConstSP ReplayFunctionAction_Helper::get_inputs(const ReplayFunctionAction* o)
{
    ReplayFunctionAction::inner_type self = o->get_inner();
    return self->Inputs();
}

spi::MapObjectSP ReplayFunctionAction::output() const
{
    spi::MapConstSP i_value = ReplayFunctionAction_Helper::get_output(this);
    return spi::MapObject::Make(i_value);
}

spi::MapConstSP ReplayFunctionAction_Helper::get_output(const ReplayFunctionAction* o)
{
    ReplayFunctionAction::inner_type self = o->get_inner();
    return self->Output();
}

/*
****************************************************************************
* Implementation of ReplayObjectAction
****************************************************************************
*/
ReplayObjectActionConstSP ReplayObjectAction::Make(
    const spi::MapObjectSP& inputs)
{
  SPI_PROFILE("spi_replay.ReplayObjectAction.Make");
  replay_check_permission();
  try
  {
    inner_type self = make_inner(inputs);
    return Wrap(self);
  }
  catch (std::exception& e)
  { SPI_THROW_RUNTIME_ERROR("Could not create ReplayObjectAction object:\n" << e.what()); }
  catch (...)
  { SPI_THROW_RUNTIME_ERROR("Could not create ReplayObjectAction object:\n" << "Unknown exception"); }
}

ReplayObjectAction::inner_type ReplayObjectAction::make_inner(
    const spi::MapObjectSP& inputs)
{
    spi::MapConstSP i_inputs;

    i_inputs = spi::MapObject::get_inner(inputs);
    ReplayObjectAction::inner_type self (new SPI_NAMESPACE::ReplayObjectAction(
        i_inputs));
    return self;
}

ReplayObjectAction::inner_type ReplayObjectAction::get_inner() const
{
    return inner_value.get();
}

ReplayObjectAction::inner_type ReplayObjectAction::get_inner(const ReplayObjectAction::outer_type& o)
{
    if (!o)
        return inner_type();
    return o->get_inner();
}

void ReplayObjectAction::set_inner(const spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayObjectAction const >& self)
{
    SPI_PRE_CONDITION(!inner_value);
    inner_value = self;
    ReplayAction::set_inner(self);
}

ReplayObjectAction::~ReplayObjectAction()
{}

ReplayActionConstSP ReplayObjectAction::BaseWrap(const ReplayAction::inner_type& baseInner)
{
    inner_type inner = dynamic_cast<inner_type>(baseInner);
    if (!inner)
        return ReplayActionConstSP();
    return Wrap(inner);
}

ReplayObjectActionConstSP ReplayObjectAction::Wrap(const inner_type& inner)
{
    if (!inner)
        return ReplayObjectActionConstSP();
    return ReplayObjectActionConstSP(new ReplayObjectAction(inner));
}

ReplayObjectAction::ReplayObjectAction(const inner_type& inner)
{
    set_inner(spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayObjectAction const >(inner));
}

spi::MapObjectSP ReplayObjectAction::inputs() const
{
    spi::MapConstSP i_value = ReplayObjectAction_Helper::get_inputs(this);
    return spi::MapObject::Make(i_value);
}

spi::MapConstSP ReplayObjectAction_Helper::get_inputs(const ReplayObjectAction* o)
{
    ReplayObjectAction::inner_type self = o->get_inner();
    return self->Inputs();
}

/*
****************************************************************************
* Implementation of ReplayCodeGenerator
****************************************************************************
*/

ReplayCodeGenerator::inner_type ReplayCodeGenerator::get_inner() const
{
    if (!inner_value)
        return get_delegate();
    return inner_value;
}

ReplayCodeGenerator::inner_type ReplayCodeGenerator::get_inner(const ReplayCodeGenerator::outer_type& o)
{
    if (!o)
        return inner_type();
    return o->get_inner();
}

void ReplayCodeGenerator::set_inner(const spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayCodeGenerator >& self)
{
    SPI_PRE_CONDITION(!inner_value);
    inner_value = self;
}

ReplayCodeGenerator::ReplayCodeGenerator()
{}

ReplayCodeGenerator::~ReplayCodeGenerator()
{}

ReplayCodeGeneratorConstSP ReplayCodeGenerator::Wrap(const inner_type& inner)
{
    if (!inner)
        return ReplayCodeGeneratorConstSP();
    for (size_t i = 0; i < g_sub_class_wrappers.size(); ++i)
    {
        ReplayCodeGeneratorConstSP o = g_sub_class_wrappers[i](inner);
        if (o)
            return o;
    }
    throw spi::RuntimeError("%s: Failed!", __FUNCTION__);
}

void ReplayCodeGenerator::GenerateFunction(
    const ReplayFunctionActionConstSP& action) const
{
  bool isLogging = replay_begin_function();
  SPI_PROFILE("spi_replay.ReplayCodeGenerator.GenerateFunction");
  try
  {
    ReplayCodeGeneratorConstSP self(this);
    spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayCodeGenerator > i_self;
    SPI_NAMESPACE::ReplayFunctionAction const* i_action;

    SPI_PRE_CONDITION(self);
    i_self = ReplayCodeGenerator::get_inner(self);
    SPI_PRE_CONDITION(action);
    i_action = ReplayFunctionAction::get_inner(action);
    ReplayCodeGenerator_Helper::GenerateFunction(i_self, i_action);
    clear_public_map();

    replay_end_function();
  }
  catch (std::exception& e)
  { throw replay_catch_exception(isLogging, "ReplayCodeGenerator.GenerateFunction", e); }
  catch (...)
  { throw replay_catch_all(isLogging, "ReplayCodeGenerator.GenerateFunction"); }
}

void ReplayCodeGenerator_Helper::GenerateFunction(
    const spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayCodeGenerator >& self,
    SPI_NAMESPACE::ReplayFunctionAction const* action)
{
    self->GenerateFunction(action);
}

void ReplayCodeGenerator::GenerateObject(
    const ReplayObjectActionConstSP& action) const
{
  bool isLogging = replay_begin_function();
  SPI_PROFILE("spi_replay.ReplayCodeGenerator.GenerateObject");
  try
  {
    ReplayCodeGeneratorConstSP self(this);
    spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayCodeGenerator > i_self;
    SPI_NAMESPACE::ReplayObjectAction const* i_action;

    SPI_PRE_CONDITION(self);
    i_self = ReplayCodeGenerator::get_inner(self);
    SPI_PRE_CONDITION(action);
    i_action = ReplayObjectAction::get_inner(action);
    ReplayCodeGenerator_Helper::GenerateObject(i_self, i_action);
    clear_public_map();

    replay_end_function();
  }
  catch (std::exception& e)
  { throw replay_catch_exception(isLogging, "ReplayCodeGenerator.GenerateObject", e); }
  catch (...)
  { throw replay_catch_all(isLogging, "ReplayCodeGenerator.GenerateObject"); }
}

void ReplayCodeGenerator_Helper::GenerateObject(
    const spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayCodeGenerator >& self,
    SPI_NAMESPACE::ReplayObjectAction const* action)
{
    self->GenerateObject(action);
}

class ReplayCodeGenerator_Delegate : public SPI_NAMESPACE::ReplayCodeGenerator
{
public:
    // constructor from outer class
    ReplayCodeGenerator_Delegate(const spi_replay::ReplayCodeGeneratorConstSP& self)
        : self(self)
    {}

    void GenerateFunction ( const SPI_NAMESPACE :: ReplayFunctionAction * action )
    {
        self->GenerateFunction(ReplayFunctionAction::Wrap(action));
    }

    void GenerateObject ( const SPI_NAMESPACE :: ReplayObjectAction * action )
    {
        self->GenerateObject(ReplayObjectAction::Wrap(action));
    }

private:
    // shared pointer to outer class
    spi_replay::ReplayCodeGeneratorConstSP self;
};

ReplayCodeGenerator::inner_type ReplayCodeGenerator::get_delegate() const
{
    return inner_type(new ReplayCodeGenerator_Delegate(outer_type(this)));
}

/*
****************************************************************************
* Implementation of ReplayLog
****************************************************************************
*/
ReplayLogConstSP ReplayLog::Make(
    const std::vector<spi::MapObjectSP>& items)
{
  SPI_PROFILE("spi_replay.ReplayLog.Make");
  replay_check_permission();
  try
  {
    inner_type self = make_inner(items);
    return Wrap(self);
  }
  catch (std::exception& e)
  { SPI_THROW_RUNTIME_ERROR("Could not create ReplayLog object:\n" << e.what()); }
  catch (...)
  { SPI_THROW_RUNTIME_ERROR("Could not create ReplayLog object:\n" << "Unknown exception"); }
}

ReplayLog::inner_type ReplayLog::make_inner(
    const std::vector<spi::MapObjectSP>& items)
{
    std::vector<spi::MapConstSP> i_items;

    for (size_t i_ = 0; i_ < items.size(); ++i_)
        i_items.push_back(spi::MapObject::get_inner(items[i_]));
    ReplayLog::inner_type self (new SPI_NAMESPACE::ReplayLog(i_items));
    return self;
}

ReplayLog::inner_type ReplayLog::get_inner() const
{
    return inner_value;
}

ReplayLog::inner_type ReplayLog::get_inner(const ReplayLog::outer_type& o)
{
    if (!o)
        return inner_type();
    return o->get_inner();
}

void ReplayLog::set_inner(const spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayLog >& self)
{
    SPI_PRE_CONDITION(!inner_value);
    inner_value = self;
}

ReplayLog::~ReplayLog()
{}

ReplayLogConstSP ReplayLog::Wrap(const inner_type& inner)
{
    if (!inner)
        return ReplayLogConstSP();
    return ReplayLogConstSP(new ReplayLog(inner));
}

ReplayLog::ReplayLog(const inner_type& inner)
{
    set_inner(inner);
}

void ReplayLog::generateCode(
    const ReplayCodeGeneratorConstSP& generator) const
{
  bool isLogging = replay_begin_function();
  SPI_PROFILE("spi_replay.ReplayLog.generateCode");
  try
  {
    ReplayLogConstSP self(this);
    spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayLog > i_self;
    spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayCodeGenerator > i_generator;

    SPI_PRE_CONDITION(self);
    i_self = ReplayLog::get_inner(self);
    SPI_PRE_CONDITION(generator);
    i_generator = ReplayCodeGenerator::get_inner(generator);
    ReplayLog_Helper::generateCode(i_self, i_generator);
    clear_public_map();

    replay_end_function();
  }
  catch (std::exception& e)
  { throw replay_catch_exception(isLogging, "ReplayLog.generateCode", e); }
  catch (...)
  { throw replay_catch_all(isLogging, "ReplayLog.generateCode"); }
}

void ReplayLog_Helper::generateCode(
    const spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayLog >& self,
    const spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayCodeGenerator >& generator)
{

    self->generateCode(generator.get());
}

ReplayLogConstSP ReplayLog::Read(
    const std::string& infilename)
{
  bool isLogging = replay_begin_function();
  SPI_PROFILE("spi_replay.ReplayLog.Read");
  try
  {
    const spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayLog >& i_result = ReplayLog_Helper::Read(
        infilename);
    ReplayLogConstSP o_result = ReplayLog::Wrap(i_result);
    if (!o_result)
        SPI_THROW_RUNTIME_ERROR("Null pointer returned");

    replay_end_function();

    return o_result;
  }
  catch (std::exception& e)
  { throw replay_catch_exception(isLogging, "ReplayLog.Read", e); }
  catch (...)
  { throw replay_catch_all(isLogging, "ReplayLog.Read"); }
}

spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayLog > ReplayLog_Helper::Read(
    const std::string& infilename)
{
    return SPI_NAMESPACE::ReplayLog::Read(infilename);
}

std::vector<spi::MapObjectSP> ReplayLog::items() const
{
    const std::vector<spi::MapConstSP>& i_values = ReplayLog_Helper::get_items(this);

    std::vector<spi::MapObjectSP> values;

    for (size_t i_ = 0; i_ < i_values.size(); ++i_)
        values.push_back(spi::MapObject::Make(i_values[i_]));

    return values;
}

std::vector<spi::MapConstSP> ReplayLog_Helper::get_items(const ReplayLog* o)
{
    ReplayLog::inner_type self = o->get_inner();
    return self->items();
}

SPI_REPLAY_END_NAMESPACE

