/*

    Sartorial Programming Interface (SPI) code generators
    Copyright (C) 2012-2023 Sartorial Programming Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/
#ifndef CLASS_HPP
#define CLASS_HPP

/*
***************************************************************************
** class.hpp
***************************************************************************
** Defines the Class base class
***************************************************************************
*/

#include "construct.hpp"
#include "coerceFrom.hpp"
#include "function.hpp"

SPI_DECLARE_RC_CLASS(Class);

SPDOC_BEGIN_NAMESPACE
SPI_DECLARE_OBJECT_CLASS(Class);
SPDOC_END_NAMESPACE

/**
 * Defines a class definition.
 *
 * From the outside it looks like a class. There are different implementations
 * inside.
 */
class Class : public Construct
{
public:
    virtual ~Class() {}
    virtual std::string getName(bool includeNamespace, const char* sep) const = 0;
    virtual std::string getObjectName() const = 0;
    virtual bool isAbstract() const = 0;
    virtual bool isWrapperClass() const = 0;
    virtual bool isDelegate() const = 0;
    virtual bool isVirtualMethod(const std::string& methodName) const = 0;
    virtual bool hasNonConstMethods() const = 0;
    virtual const DataTypeConstSP& getDataType(
        const ServiceDefinitionSP& svc, bool ignored) const = 0;
    virtual ClassConstSP getBaseClass() const = 0;
    virtual std::vector<CoerceFromConstSP> getCoerceFrom() const = 0;
    virtual bool byValue() const = 0;

    virtual std::vector<AttributeConstSP> AllAttributes() const = 0;

    void addClassFunction(const FunctionConstSP& func);

    const std::vector<FunctionConstSP>& classFunctions() const;

private:

    std::vector<FunctionConstSP> m_classFunctions;
};

#endif
