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
** Defines the Construct base class
***************************************************************************
*/

#include <vector>

#include "dataType.hpp"
#include <spgtools/generatedOutput.hpp>
#include <spgtools/namespaceManager.hpp>

SPI_DECLARE_RC_CLASS(Construct);
SPI_DECLARE_RC_CLASS(ServiceDefinition);

SPDOC_BEGIN_NAMESPACE
SPI_DECLARE_OBJECT_CLASS(Construct);
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

    virtual spdoc::ConstructConstSP getDoc() const = 0;

    static void SplitTypedef(const std::string& td,
        std::string& td1,
        std::string& td2);

protected:
    Construct();
};

#endif
