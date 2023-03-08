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
 * The goal is to enable an arbitrary platform to have all of its functions
 * implemented as follows (using Excel by example):
 *

xloper* xlFunc(xloper* a1, xloper* a2, xloper* a3)
{
    static FunctionCaller* func = 0;
    if (!func)
        func = svc->get_function_caller("Func");

    InputValues iv = ExcelInputValues(func, a1, a2, a3);
    Value ov = CallInContext(func, iv, ExcelContext());
    return ExcelOutputValue(ov);
}

 *
 * In this example, ExcelInputValues is responsible for converting from
 * xloper* to Value. This may result in an InputValues object which has
 * permuted inputs.
 *
 * Then CallInContext will be responsible for looping through the
 * permutations, and for each permutation will handle the late-binding of
 * the Value to some given type. Thus the FuncDef must have a method for
 * calling the function from a vector of Value of the correct type.
 *
 * Finally CallInContext will return an output as Value which then needs
 * to be converted to an xloper using ExcelOutputValue.
 *
 * Where are all these objects?
 *
 * FuncDef is defined within the DLL. There will be a look-up by name.
 * ExcelInputValues will be a generic function within spxl.lib.
 * CallInContext will be a generic function within spi.dll.
 * ExcelOutputValue will be a generic function within spxl.lib.
 *
 * Note that there are three types of function that we need to support.
 *
 * 1. Regular function.
 * 2. Class constructor.
 * 3. Class method.
 *
 * Class constructors will not support permuted arguments.
 * Regular functions will support permuted arguments.
 * Class methods will not allow the class instance to be permuted.
 */

#include "InputValues.hpp"

SPI_BEGIN_NAMESPACE

namespace
{
    void ValidateScalar(
        const char* functionName,
        const Value& value,
        const FuncArg& arg,
        bool allowUndefined=false)
    {
        if ((!arg.isOptional && !allowUndefined) && value.isUndefined())
        {
            throw RuntimeError("%s: Undefined input for %s",
                functionName, arg.name);
        }
    }

    void ValidateArray(
        const char* functionName,
        const Value& value,
        const FuncArg& arg)
    {
        switch (value.getType())
        {
        case Value::ARRAY:
        {
            const std::vector<Value>& values = value.getArray()->getVector();
            for (size_t i = 0; i < values.size(); ++i)
                ValidateScalar(functionName, values[i], arg, true);
            break;
        }
        case Value::UNDEFINED:
            break; // this is allowed
        default:
            // potentially scalars are allowed
            // we will have to let the InputContext to decide later whether
            // it will let us convert a scalar into an array of size one
            // however at this point we are not qualified to make the decision
            ValidateScalar(functionName, value, arg);
            break;
        }
    }

} // end of anonymous namespace

InputValues::InputValues(const char* name)
:
m_name(name),
m_inputs(),
m_isPermuted(false),
m_nbPermutations(0)
{}

void InputValues::AddValue(
    const FuncArg& arg,
    const Value&   value)
{
    if (arg.isArray)
    {
        ValidateArray(m_name, value, arg);
    }
    else if (arg.isPermutable && value.getType() == Value::ARRAY)
    {
        size_t arraySize = value.getArray()->size();

        if (!m_isPermuted)
        {
            m_isPermuted     = true;
            m_nbPermutations = arraySize;
        }
        else if (m_nbPermutations != arraySize)
        {
            throw RuntimeError("%s: Array size mismatch for %s (%ld) "
                "should be (%ld)", m_name, arg.name,
                (long)arraySize, (long)m_nbPermutations);
        }
        ValidateArray(m_name, value, arg);
    }
    else
    {
        ValidateScalar(m_name, value, arg, true);
    }
    m_inputs.push_back(value);
}

std::vector<Value>
InputValues::GetPermutation(
    const InputContext*   context,
    const FunctionCaller* func,
    size_t permutation) const
{
    std::vector<Value> inputs;
    for (size_t i = 0; i < func->nbArgs; ++i)
    {
        const FuncArg& arg = func->args[i];
        const Value& value = m_inputs[i];

        try
        {
            if (arg.isArray)
            {
                switch (value.getType())
                {
                case Value::ARRAY:
                    // we assume all elements validated
                    inputs.push_back(value);
                    break;
                case Value::UNDEFINED:
                    // we do allow an undefined input to be an array of size 0
                    inputs.push_back(Value(IArrayConstSP(new ValueArray(0))));
                    break;
                default:
                    if (!context->AcceptScalarForArray())
                    {
                        throw RuntimeError("%s: Scalar value for array input %s",
                            m_name, arg.name);
                    }
                    // we only convert the value to an array later
                    inputs.push_back(value);
                    break;
                }
            }
            else if (value.getType() == Value::ARRAY)
            {
                // we assume that by construction that a scalar argument
                // with a value of type array is a permutation
                Value item = value.getArray()->getItem(permutation);
                inputs.push_back(item);
            }
            else
            {
                inputs.push_back(value);
            }
        }
        catch (std::exception& e)
        {
            throw RuntimeError("Could not get parameter %s for function %s:\n%s",
                arg.name, func->name, e.what());
        }
    }
    return inputs;
}

bool InputValues::isPermuted() const
{
    return m_isPermuted;
}

size_t InputValues::nbPermutations() const
{
    return m_nbPermutations;
}

Value CallInContext(
    const FunctionCaller* func,
    const InputValues&  inputValues,
    const InputContext* inputContext)
{
    if (inputValues.isPermuted())
    {
        std::vector<Value> output;
        size_t nbCalls = inputValues.nbPermutations();
        for (size_t i = 0; i < nbCalls; ++i)
        {
            const std::vector<Value>& inputs =
                inputValues.GetPermutation(inputContext, func, i);
            output.push_back(func->caller(inputContext, inputs));
        }
        return Value(IArrayConstSP(new ValueArray(output, nbCalls)));
    }
    else
    {
        const std::vector<Value>& inputs =
            inputValues.GetPermutation(inputContext, func, 0);
        return func->caller(inputContext, inputs);
    }
}

SPI_END_NAMESPACE
