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

#ifndef _SPI_REPLAY_REPLAY_HELPER_HPP_
#define _SPI_REPLAY_REPLAY_HELPER_HPP_

/**
****************************************************************************
* Header file: spi_replay_replay_helper.hpp
****************************************************************************
*/

#include "spi_replay_replay_classes.hpp"
#include "replay_dll_type_converters.hpp"

SPI_REPLAY_BEGIN_NAMESPACE

class ReplayAction_Helper
{
public:
};

class ReplayFunctionAction_Helper
{
public:
    static spi::MapConstSP get_inputs(const ReplayFunctionAction* o);
    static spi::MapConstSP get_output(const ReplayFunctionAction* o);
};

class ReplayObjectAction_Helper
{
public:
    static spi::MapConstSP get_inputs(const ReplayObjectAction* o);
};

class ReplayCodeGenerator_Helper
{
public:
    static void GenerateFunction(
        const spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayCodeGenerator >& self,
        SPI_NAMESPACE::ReplayFunctionAction const* action);
    static void GenerateObject(
        const spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayCodeGenerator >& self,
        SPI_NAMESPACE::ReplayObjectAction const* action);
};

class ReplayLog_Helper
{
public:
    static void generateCode(
        const spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayLog >& self,
        const spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayCodeGenerator >& generator);
    static spi_boost::intrusive_ptr< SPI_NAMESPACE::ReplayLog > Read(
        const std::string& infilename);
    static std::vector<spi::MapConstSP> get_items(const ReplayLog* o);
};

void replay_register_object_types(const spi::ServiceSP& svc);

SPI_REPLAY_END_NAMESPACE

#endif /* _SPI_REPLAY_REPLAY_HELPER_HPP_*/

