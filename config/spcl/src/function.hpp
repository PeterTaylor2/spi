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
#ifndef FUNCTION_HPP
#define FUNCTION_HPP

/*
***************************************************************************
** function.hpp
***************************************************************************
** Defines the Function class
***************************************************************************
*/

#include "construct.hpp"

#include <vector>
#include <string>

SPI_DECLARE_RC_CLASS(Attribute);
SPI_DECLARE_RC_CLASS(FunctionAttribute);
SPI_DECLARE_RC_CLASS(Verbatim);
SPI_DECLARE_RC_CLASS(Function);

SPDOC_BEGIN_NAMESPACE
SPI_DECLARE_OBJECT_CLASS(Function);
SPDOC_END_NAMESPACE

/**
 * Defines a Function.
 *
 * A function has description, a return type (empty = void), a function
 * name, some input parameters and implementation.
 */
class Function : public Construct
{
public:
    friend class ClassMethod;
    friend class NewClass;

    static FunctionConstSP Make(
        const std::vector<std::string>&      description,
        const std::vector<std::string>&      returnTypeDescription,
        const DataTypeConstSP&               returnType,
        bool                                 returnInnerConst,
        int                                  returnArrayDim,
        const std::string&                   name,
        const std::string&                   ns,
        const std::vector<FunctionAttributeConstSP>& args,
        const VerbatimConstSP&               implementation,
        bool                                 noLog,
        bool                                 noConvert,
        const std::vector<std::string>&      excelOptions,
        int                                  cacheSize);

    AttributeConstSP returns() const;

    // implementation of Construct
    void declare(GeneratedOutput& ostr,
                 const ServiceDefinitionSP& svc,
                 bool types) const;

    bool declareInClasses() const;

    void declareHelper(GeneratedOutput& ostr,
                       const ServiceDefinitionSP& svc,
                       bool types) const;

    void implement(GeneratedOutput& ostr,
                   const ServiceDefinitionSP& svc,
                   bool types) const;

    void implementHelper(GeneratedOutput& ostr,
                         const ServiceDefinitionSP& svc,
                         bool types) const;

    bool neededByTypesLibrary() const;

    void implementRegistration(
        GeneratedOutput& ostr,
        const char* serviceName,
        bool types) const;

    const char* type() const;
    spdoc::ConstructConstSP getDoc() const;

protected:
    Function(
        const std::vector<std::string>&      description,
        const std::vector<std::string>&      returnTypeDescription,
        const DataTypeConstSP&               returnType,
        bool                                 returnInnerConst,
        int                                  returnArrayDim,
        const std::string&                   name,
        const std::string&                   ns,
        const std::vector<FunctionAttributeConstSP>& args,
        const VerbatimConstSP&               implementation,
        bool                                 noLog,
        bool                                 noConvert,
        const std::vector<std::string>&      excelOptions,
        int                                  cacheSize);

private:
    std::vector<std::string>      m_description;
    std::vector<std::string>      m_returnTypeDescription;
    DataTypeConstSP               m_returnType;
    bool                          m_returnInnerConst;
    int                           m_returnArrayDim;
    std::string                   m_name;
    std::string                   m_ns;
    std::vector<AttributeConstSP> m_inputs;
    std::vector<AttributeConstSP> m_outputs;
    VerbatimConstSP               m_implementation;
    bool                          m_noLog;
    bool                          m_noConvert;
    std::vector<std::string>      m_excelOptions;
    int                           m_cacheSize;
    bool                          m_hasIgnored;

    mutable spdoc::FunctionConstSP m_doc;

    const std::string fullName() const;

public:
    const std::string& name() const { return m_name; }
    bool hasIgnored() const { return m_hasIgnored; }
};

#endif
