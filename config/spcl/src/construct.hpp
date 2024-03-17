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
#ifndef CONSTRUCT_HPP
#define CONSTRUCT_HPP

/*
***************************************************************************
** construct.hpp
***************************************************************************
** Defines the Construct class and virtual sub-classes thereof.
***************************************************************************
*/

#include <vector>

#include "dataType.hpp"
#include <spgtools/generatedOutput.hpp>
#include <spgtools/namespaceManager.hpp>

SPI_DECLARE_RC_CLASS(Construct);
SPI_DECLARE_RC_CLASS(Class);
SPI_DECLARE_RC_CLASS(InnerClass);
SPI_DECLARE_RC_CLASS(ServiceDefinition);
SPI_DECLARE_RC_CLASS(ModuleDefinition);
SPI_DECLARE_RC_CLASS(CoerceFrom);
SPI_DECLARE_RC_CLASS(ClassMethod);

#include "coerceFrom.hpp" // FIXME

SPDOC_BEGIN_NAMESPACE
SPI_DECLARE_OBJECT_CLASS(Construct);
SPI_DECLARE_OBJECT_CLASS(Class);
SPDOC_END_NAMESPACE

/**
 * Interface class defining a construct - i.e. something that goes into
 * module. This could be a function, a class, or anything else really.
 *
 * The idea is that we want this list to be ordered, so that the generated
 * code can be in the same order as the configuration file.
 */
class Construct : public spi::RefCounter
{
public:
    virtual void declareInner(
        GeneratedOutput& ostr,
        NamespaceManager& nsman,
        const ServiceDefinitionSP& svc) const;

    virtual int preDeclare(
        GeneratedOutput& ostr,
        const ServiceDefinitionSP& svc) const;

    virtual bool declareInClasses() const = 0;

    virtual void declare(
        GeneratedOutput& ostr,
        const ServiceDefinitionSP& svc,
        bool types) const = 0;

    virtual void declareClassFunctions(
        GeneratedOutput& ostr,
        const ServiceDefinitionSP& svc) const;

    virtual void declareHelper(
        GeneratedOutput& ostr,
        const ServiceDefinitionSP& svc,
        bool types) const = 0;

    virtual void implement(
        GeneratedOutput& ostr,
        const ServiceDefinitionSP& svc,
        bool types) const = 0;

    virtual void implementHelper(
        GeneratedOutput& ostr,
        const ServiceDefinitionSP& svc,
        bool types) const = 0;

    virtual void implementRegistration(
        GeneratedOutput& ostr,
        const char* serviceName,
        bool types) const = 0;

    virtual void declareTypeConversions(
        GeneratedOutput& ostr,
        const ServiceDefinitionSP& svc) const;

    virtual void writeInnerHeader(
        GeneratedOutput& ostr) const;

    virtual void writeInnerPreDeclaration(
        GeneratedOutput& ostr,
        NamespaceManager& nsm) const;

    //virtual const char* type() const = 0;
    virtual spdoc::ConstructConstSP getDoc() const = 0;

    static void SplitTypedef(const std::string& td,
        std::string& td1,
        std::string& td2);

protected:
    Construct();
};

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

    void declareMethodAsFunction(
        GeneratedOutput& ostr,
        const ServiceDefinitionSP& svc,
        const ClassMethodConstSP& method,
        const std::string& className,
        const std::string& funcPrefix,
        const std::string& instance,
        const DataTypeConstSP& instanceType) const;

    void implementMethodAsFunction(
        GeneratedOutput& ostr,
        const ClassMethodConstSP& method,
        const std::string& className,
        const std::string& funcPrefix,
        const std::string& instance,
        const DataTypeConstSP& instanceType) const;

};



#endif
