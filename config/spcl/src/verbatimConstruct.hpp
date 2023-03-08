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
#ifndef VERBATIM_CONSTRUCT_HPP
#define VERBATIM_CONSTRUCT_HPP

/*
***************************************************************************
** verbatimConstruct.hpp
***************************************************************************
** Defines a verbatim construct. This is verbatim code in the middle of
** config file that we want to appear in a particular order within the
** generated code.
***************************************************************************
*/

#include "construct.hpp"

#include <vector>
#include <string>

SPI_DECLARE_RC_CLASS(Verbatim);
SPI_DECLARE_RC_CLASS(VerbatimConstruct);

/**
 * Defines a verbatim construct.
 */
class VerbatimConstruct : public Construct
{
public:
    static VerbatimConstructSP Make(
        const VerbatimConstSP& verbatim);

    // implementation of construct
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

    void implementRegistration(
        GeneratedOutput& ostr,
        const char* serviceName,
        bool types) const;

    //const char* type() const;
    spdoc::ConstructConstSP getDoc() const;

protected:
    VerbatimConstruct(
        const VerbatimConstSP& verbatim);

public:
    VerbatimConstSP m_verbatim;
};

#endif
