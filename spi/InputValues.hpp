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
#ifndef SPI_INPUT_VALUES_HPP
#define SPI_INPUT_VALUES_HPP

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

#include "Value.hpp"
#include "Object.hpp"

#include "InputContext.hpp"
#include "FunctionCaller.hpp"

SPI_BEGIN_NAMESPACE

SPI_DECLARE_RC_CLASS(Service);
class ObjectType;

class SPI_IMPORT InputValues
{
public:
    InputValues(const char* name);

    void AddValue(
        const FuncArg& arg,
        const Value&   value);

    std::vector<Value> GetPermutation(
        const InputContext*   context,
        const FunctionCaller* func,
        size_t permutation) const;
    bool isPermuted() const;
    size_t nbPermutations() const;

private:
    const char*        m_name;
    std::vector<Value> m_inputs;
    bool               m_isPermuted;
    size_t             m_nbPermutations;
};

SPI_IMPORT
Value CallInContext(
    const FunctionCaller* func,
    const InputValues&  inputValues,
    const InputContext* inputContext);

SPI_END_NAMESPACE

#endif

