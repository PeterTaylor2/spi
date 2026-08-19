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
#include "Function.hpp"

#include "Map.hpp"
#include "IObjectMap.hpp"
#include "RuntimeError.hpp"
#include "ValueToObject.hpp"
#include "Service.hpp"
#include "ObjectMap.hpp"
#include "ObjectPutMap.hpp"

#include <stdarg.h>
#include <spi_util/StringUtil.hpp>
#include <spi_util/RootBrent.hpp>

SPI_BEGIN_NAMESPACE

namespace {

    ObjectType* get_object_type_from_service(
        const ServiceConstSP& service,
        const char* name)
    {
        std::string fullName = spi_util::StringFormat("%s.%s",
            service->get_namespace().c_str(), name);

        return service->get_object_type(fullName);
    }

    class ObjectiveFunction : public spi_util::FunctionOfX
    {
    public:
        ObjectiveFunction(
            const FunctionSP& func,
            size_t pos,
            const std::string& oname)
            :
            m_func(func),
            m_pos(pos),
            m_oname(oname)
        {
        }

        double operator()(double x) const
        {
            NoInputContext context;

            m_func->set_value(m_pos, spi::Value(x));

            spi::Value value = m_func->call();

            if (m_oname.empty())
                return value.getDouble();

            spi::ObjectConstSP objValue = value.getObject();

            return objValue->get_value(m_oname).getDouble();
        }

    private:
        FunctionSP m_func;
        size_t m_pos;
        std::string m_oname;
    };

} // end of anonymous namespace

Function::Function(
    const Service* service,
    const char* name,
    const std::vector<Value>& inputs)
    :
    Object(true), // no-id on serialization
    m_service(service),
    m_caller(),
    m_inputs(inputs),
    m_objectType()
{
    SPI_PRE_CONDITION(service);
    SPI_PRE_CONDITION(name);

    m_caller = service->get_function_caller(name);
    m_objectType = get_object_type_from_service(service, name);
}

Function::Function(
    const ServiceConstSP& service,
    FunctionCaller* caller,
    const std::vector<Value>& inputs)
    :
    Object(true), // no-id on serialization
    m_service(service),
    m_caller(caller),
    m_inputs(inputs),
    m_objectType()
{
    SPI_PRE_CONDITION(service);
    SPI_PRE_CONDITION(caller);
    SPI_PRE_CONDITION(inputs.size() == caller->nbArgs);

    m_objectType = get_object_type_from_service(service, caller->name);
}

ObjectConstSP Function::MakeFromMap(
    IObjectMap* m,
    ValueToObject& valueToObject)
{
    MapSP exportMap = m->ExportMap();

    // this could be the common service which contains all the object types
    ServiceConstSP service = valueToObject.Service();

    std::string functionName = exportMap->ClassName();
    std::vector<std::string> functionParts = spi_util::StringParser(
        functionName, ".");

    std::string serviceName = functionParts[0];
    std::string shortName   = functionParts[1];

    ObjectType* ot = service->get_object_type(functionName);

    // this is the specific service corresponding to the function object
    // which contains the function callers but doesn't contain all object types
    const Service* functionService = ot->get_service();
    FunctionCaller* caller = functionService->get_function_caller(shortName.c_str());

    std::vector<Value> inputs;
    for (size_t i = 0; i < caller->nbArgs; ++i)
    {
        Value value = valueToObject.ResolveObject(
            exportMap->GetValue(caller->args[i].name));
        inputs.push_back(value);
    }

    return ObjectConstSP(new Function(functionService, caller, inputs));
}

void Function::to_map(IObjectMap* m, bool publicOnly) const
{
    // no concept of public or private fields
    // the inputs to the function are all public

    MapSP importMap(new Map(m_caller->name));

    for (size_t i = 0; i < m_caller->nbArgs; ++i)
    {
        const Value& value = m_inputs[i];
        const char* name   = m_caller->args[i].name;

        importMap->SetValue(name, value);
    }

    m->ImportMap(importMap.get());
}

ObjectType* Function::get_object_type() const
{
    return m_objectType;
}

bool Function::IsInstance(const ObjectConstSP& obj)
{
    return spi::IsInstance<Function>(obj);
}

Value Function::call() const
{
    NoInputContext context;
    Value output = m_caller->caller(&context, m_inputs);
    return output;
}

FunctionSP Function::MakeEmpty(
    const Service* service, // cannot be the common service
    const char* name)
{
    return FunctionSP(new Function(service, name, {}));
}

void Function::add_input(const spi::Variant& var)
{
    size_t numInputs = m_inputs.size();
    if (numInputs < m_caller->nbArgs)
    {
        const FuncArg& arg = m_caller->args[numInputs];
        spi::Value value = arg.coerce(var);
        m_inputs.push_back(value);
    }
    else
    {
        const spi::Value& value = var.GetValue();
        if (!value.isUndefined())
        {
            SPI_THROW_RUNTIME_ERROR("Attempting to add more inputs than supported by '"
                << m_caller->name << "'");
        }
    }
}

void Function::set_value(size_t pos, const spi::Value& value)
{
    if (pos < m_inputs.size())
        m_inputs[pos] = value;
}

double Function::solve(
    const std::string& name,
    double target,
    double guess,
    double loBound,
    double hiBound,
    int maxIters,
    double xAcc,
    double fAcc,
    const std::string& oname) const
{
    // shallow copy this function so that we can amend one of its inputs
    FunctionSP func(new Function(m_service, m_caller, m_inputs));

    bool found = false;
    size_t pos;
    for (size_t i = 0; i < m_caller->nbArgs; ++i)
    {
        if (strcmp(name.c_str(), m_caller->args[i].name) == 0)
        {
            found = true;
            pos = i;
            break;
        }
    }

    if (!found)
    {
        SPI_THROW_RUNTIME_ERROR("name '" << name << "' is not an input to '"
            << m_caller->name << "'");
    }

    ObjectiveFunction objFunc(func, pos, oname);

    double solution = spi_util::RootFindBrent(
        objFunc,
        target,
        guess,
        loBound,
        hiBound,
        maxIters,
        xAcc,
        fAcc);

    return solution;
}

ObjectType FunctionObjectType(const char* name)
{
    return ObjectType(
        name,
        Function::MakeFromMap,
        Function::IsInstance,
        false);
}

SPI_END_NAMESPACE
