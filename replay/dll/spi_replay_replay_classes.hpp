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

#ifndef _SPI_REPLAY_REPLAY_CLASSES_HPP_
#define _SPI_REPLAY_REPLAY_CLASSES_HPP_

/**
****************************************************************************
* Header file: spi_replay_replay_classes.hpp
****************************************************************************
*/

#include <spi/spi.hpp>
#include "spi_replay_map_classes.hpp"

namespace SPI_NAMESPACE {
    class ReplayAction;
    class ReplayFunctionAction;
    class ReplayObjectAction;
    class ReplayCodeGenerator;
    class ReplayLog;
} // end of namespace SPI_NAMESPACE

SPI_REPLAY_BEGIN_NAMESPACE

SPI_DECLARE_OBJECT_CLASS(ReplayAction);
SPI_DECLARE_OBJECT_CLASS(ReplayFunctionAction);
SPI_DECLARE_OBJECT_CLASS(ReplayObjectAction);
SPI_DECLARE_OBJECT_CLASS(ReplayCodeGenerator);
SPI_DECLARE_OBJECT_CLASS(ReplayLog);

/**
****************************************************************************
* No description.
****************************************************************************
*/
class REPLAY_IMPORT ReplayAction : public spi::Object
{
public:

    typedef spi::ObjectSmartPtr<ReplayAction> outer_type; 

    SPI_DECLARE_OBJECT_TYPE(ReplayAction);

    typedef SPI_NAMESPACE::ReplayAction const* inner_type;
    inner_type get_inner() const;
    static inner_type get_inner(const outer_type& o);

    ~ReplayAction();

    static ReplayActionConstSP Wrap(const inner_type& inner);

    typedef ReplayActionConstSP (sub_class_wrapper)(const inner_type& inner);

    static std::vector<sub_class_wrapper*> g_sub_class_wrappers;

protected:

    void set_inner(const spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayAction const >&);
    ReplayAction();

private:

    /* shared pointer to implementation */
    spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayAction const > inner_value;
};

/**
****************************************************************************
* No description.
****************************************************************************
*/
class REPLAY_IMPORT ReplayFunctionAction : public ReplayAction
{
public:
    static ReplayFunctionActionConstSP Make(
        const spi::MapObjectSP& inputs,
        const spi::MapObjectSP& output);

    /* public accessor methods */
    spi::MapObjectSP inputs() const;
    spi::MapObjectSP output() const;

    typedef spi::ObjectSmartPtr<ReplayFunctionAction> outer_type; 

    SPI_DECLARE_OBJECT_TYPE(ReplayFunctionAction);

    typedef SPI_NAMESPACE::ReplayFunctionAction const* inner_type;
    inner_type get_inner() const;
    static inner_type get_inner(const outer_type& o);

    ~ReplayFunctionAction();

    static ReplayFunctionActionConstSP Wrap(const inner_type& inner);
    static ReplayActionConstSP BaseWrap(const ReplayAction::inner_type& inner);

protected:

    void set_inner(const spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayFunctionAction const >&);

    ReplayFunctionAction(const inner_type& inner);

private:

    static inner_type make_inner(
        const spi::MapObjectSP& inputs,
        const spi::MapObjectSP& output);

    /* shared pointer to implementation */
    spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayFunctionAction const > inner_value;
};

/**
****************************************************************************
* No description.
****************************************************************************
*/
class REPLAY_IMPORT ReplayObjectAction : public ReplayAction
{
public:
    static ReplayObjectActionConstSP Make(
        const spi::MapObjectSP& inputs);

    /* public accessor methods */
    spi::MapObjectSP inputs() const;

    typedef spi::ObjectSmartPtr<ReplayObjectAction> outer_type; 

    SPI_DECLARE_OBJECT_TYPE(ReplayObjectAction);

    typedef SPI_NAMESPACE::ReplayObjectAction const* inner_type;
    inner_type get_inner() const;
    static inner_type get_inner(const outer_type& o);

    ~ReplayObjectAction();

    static ReplayObjectActionConstSP Wrap(const inner_type& inner);
    static ReplayActionConstSP BaseWrap(const ReplayAction::inner_type& inner);

protected:

    void set_inner(const spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayObjectAction const >&);

    ReplayObjectAction(const inner_type& inner);

private:

    static inner_type make_inner(
        const spi::MapObjectSP& inputs);

    /* shared pointer to implementation */
    spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayObjectAction const > inner_value;
};

/**
****************************************************************************
* No description.
****************************************************************************
*/
class REPLAY_IMPORT ReplayCodeGenerator : public spi::Object
{
public:

    /**
    ************************************************************************
    * No description.
    *
    * @param action
    ************************************************************************
    */
    virtual void GenerateFunction(
        const ReplayFunctionActionConstSP& action) const;

    /**
    ************************************************************************
    * No description.
    *
    * @param action
    ************************************************************************
    */
    virtual void GenerateObject(
        const ReplayObjectActionConstSP& action) const;

    typedef spi::ObjectSmartPtr<ReplayCodeGenerator> outer_type; 

    SPI_DECLARE_OBJECT_TYPE(ReplayCodeGenerator);

    typedef spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayCodeGenerator > inner_type;
    inner_type get_inner() const;
    static inner_type get_inner(const outer_type& o);

    ~ReplayCodeGenerator();

    static ReplayCodeGeneratorConstSP Wrap(const inner_type& inner);

    typedef ReplayCodeGeneratorConstSP (sub_class_wrapper)(const inner_type& inner);

    static std::vector<sub_class_wrapper*> g_sub_class_wrappers;

protected:

    void set_inner(const spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayCodeGenerator >&);
    ReplayCodeGenerator();

    inner_type get_delegate() const;

private:

    /* shared pointer to implementation */
    spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayCodeGenerator > inner_value;
};

/**
****************************************************************************
* No description.
****************************************************************************
*/
class REPLAY_IMPORT ReplayLog : public spi::Object
{
public:

    /**
    ************************************************************************
    * No description.
    *
    * @param generator
    ************************************************************************
    */
    void generateCode(
        const ReplayCodeGeneratorConstSP& generator) const;

    /**
    ************************************************************************
    * No description.
    *
    * @param infilename
    ************************************************************************
    */
    static ReplayLogConstSP Read(
        const std::string& infilename);

    /* public accessor methods */
    std::vector<spi::MapObjectSP> items() const;

    typedef spi::ObjectSmartPtr<ReplayLog> outer_type; 

    SPI_DECLARE_OBJECT_TYPE(ReplayLog);

    typedef spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayLog > inner_type;
    inner_type get_inner() const;
    static inner_type get_inner(const outer_type& o);

    ~ReplayLog();

    static ReplayLogConstSP Wrap(const inner_type& inner);
    static ReplayLogConstSP Make(
        const std::vector<spi::MapObjectSP>& items);

protected:

    void set_inner(const spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayLog >&);

    ReplayLog(const inner_type& inner);

private:

    static inner_type make_inner(
        const std::vector<spi::MapObjectSP>& items);

    /* shared pointer to implementation */
    spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayLog > inner_value;
};

SPI_REPLAY_END_NAMESPACE

#endif /* _SPI_REPLAY_REPLAY_CLASSES_HPP_*/

