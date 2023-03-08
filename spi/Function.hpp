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
#ifndef SPI_FUNCTION_HPP
#define SPI_FUNCTION_HPP

/*
***************************************************************************
** Function.hpp
***************************************************************************
** Defines the Function class which is a sub-class of Object.
**
** The function will contain a function caller and a map of inputs.
** This will be enough for serialization and for calling the function.
** The question of registration is something else entirely.
***************************************************************************
*/

#include "Object.hpp"
#include "Value.hpp"
#include "Map.hpp"
#include "InputValues.hpp"

SPI_BEGIN_NAMESPACE

class Service;

SPI_DECLARE_RC_CLASS(Function);

class SPI_IMPORT Function : public Object
{
public:
    Function(
        const Service* service,
        const char* name,
        const std::vector<Value>& inputs);
    Function(
        const ServiceConstSP& service,
        FunctionCaller* caller,
        const std::vector<Value>& inputs);
    static ObjectConstSP MakeFromMap(
        IObjectMap* m,
        ValueToObject& valueToObject);
    static bool IsInstance(const ObjectConstSP& o);

    Value call() const;

    // implementation of Object interface
    // get_object_type could be a problem
    void to_map(IObjectMap* objectMap, bool publicOnly) const;
    ObjectType* get_object_type() const;

private:
    ServiceConstSP m_service;
    FunctionCaller* m_caller;
    std::vector<Value> m_inputs;
    ObjectType* m_objectType;
};

SPI_IMPORT
ObjectType FunctionObjectType(const char* name);

SPI_END_NAMESPACE

#endif
