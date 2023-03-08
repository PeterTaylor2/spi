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
#ifndef CLASS_METHOD_HPP
#define CLASS_METHOD_HPP

/*
***************************************************************************
** classMethod.hpp
***************************************************************************
** Defines the ClassMethod class used in the configuration parser.
***************************************************************************
*/

#include <vector>

#include <spi/Date.hpp>
#include <spi/Service.hpp>
#include <spi/ObjectSmartPtr.hpp>
#include <spi/spdoc_namespace.hpp>

SPI_DECLARE_RC_CLASS(DataType);
SPI_DECLARE_RC_CLASS(Attribute);
SPI_DECLARE_RC_CLASS(Construct);
SPI_DECLARE_RC_CLASS(Function);
SPI_DECLARE_RC_CLASS(ClassMethod);
SPI_DECLARE_RC_CLASS(Enum);
SPI_DECLARE_RC_CLASS(Struct);
SPI_DECLARE_RC_CLASS(ServiceDefinition);
class WrapperClass;

#include <spgtools/generatedOutput.hpp>

SPDOC_BEGIN_NAMESPACE
SPI_DECLARE_OBJECT_CLASS(Construct);
SPI_DECLARE_OBJECT_CLASS(Function);
SPI_DECLARE_OBJECT_CLASS(Enum);
SPI_DECLARE_OBJECT_CLASS(ClassMethod);
SPI_DECLARE_OBJECT_CLASS(Struct);
SPDOC_END_NAMESPACE

/**
 * Defines a class method.
 *
 * This is a function with extra information which pertains to methods.
 */
class ClassMethod : public spi::RefCounter
{
public:
    friend class Struct;
    friend class NewClass;

    static ClassMethodConstSP Make(
        const FunctionConstSP& function,
        bool isConst,
        bool isVirtual,
        bool isStatic,
        bool isDelegate,
        bool isImplementation,
        const std::string& implements,
        bool byValue);

    void declare(GeneratedOutput& ostr,
        const DataTypeConstSP& classType,
        const std::string& className,
                 bool types,
                 const ServiceDefinitionSP& svc,
                 bool alwaysVirtual=false,
                 bool innerContext=false) const;

    // implement as delegate only works for wrapper classes
    void implementAsDelegate(GeneratedOutput& ostr,
        const WrapperClass* wrapperClass,
        const ServiceDefinitionSP& svc) const;

    void declareHelper(GeneratedOutput& ostr,
        const DataTypeConstSP& classType,
        const std::string& className,
        bool types,
        const ServiceDefinitionSP& svc) const;

    void implement(GeneratedOutput& ostr,
        const DataTypeConstSP& classType,
        const std::string& className,
        const std::string& innerClassName,
        bool types,
        const ServiceDefinitionSP& svc,
        bool noHelper = false) const;

    void implementHelper(GeneratedOutput& ostr,
        const DataTypeConstSP& classType,
        const std::string& className,
        bool types,
        const ServiceDefinitionSP& svc) const;

    void implementRegistration(
        GeneratedOutput& ostr,
        const std::string& className,
        const char* serviceName,
        bool types) const;

    spdoc::ClassMethodConstSP getDoc() const;

protected:
    ClassMethod(
        const FunctionConstSP& function,
        bool isConst,
        bool isVirtual,
        bool isStatic,
        bool isDelegate,
        bool isImplementation,
        const std::string& implements,
        bool byValue);

private:
    FunctionConstSP m_function;
    bool m_isConst;
    bool m_isVirtual;
    bool m_isStatic;
    bool m_isDelegate;
    bool m_isImplementation;
    std::string m_implements;
    bool m_byValue;

    std::string implementHelperCaller(
        GeneratedOutput& str,
        const std::string& className) const;

public:
    const FunctionConstSP& function() const { return m_function; }
    bool isVirtual() const { return m_isVirtual; }
    bool isDelegate() const { return m_isDelegate; }

    void implementDeclaration(
        GeneratedOutput& ostr,
        const std::string& className,
        bool innerContext = false) const;

};

#endif
