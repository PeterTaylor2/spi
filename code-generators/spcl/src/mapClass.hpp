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

#ifndef MAP_CLASS_HPP
#define MAP_CLASS_HPP

/*
***************************************************************************
** mapClass.hpp
***************************************************************************
** Defines the MapClass classes used in the configuration parser.
** These are sub-classes of spi::IObjectMap defined solely by the name
** of the class and whether we can modify the object from the add-in layer.
***************************************************************************
*/

#include "class.hpp"

#include <vector>
#include <string>

SPI_DECLARE_RC_CLASS(MapClass);

/**
 * MapClass type.
 *
 * This is an object with data implemented entirely as a Map - it will be
 * a sub-class of spi::IMapClass with a class name.
 */
class MapClass : public Class
{
public:
    static MapClassConstSP Make(
        const std::vector<std::string>& description,
        const std::string&              name,
        const std::string&              ns,
        bool                            canPut);

    // re-implementation of Construct
    //void declareInner(
    //    GeneratedOutput& ostr,
    //    NamespaceManager& nsman,
    //    const ServiceDefinitionSP& svc) const;

    int preDeclare(GeneratedOutput& ostr,
                   const ServiceDefinitionSP& svc) const override;

    // implementation of Construct
    void declare(GeneratedOutput& ostr,
                 const ServiceDefinitionSP& svc,
                 bool types) const override;

    bool declareInClasses() const override;

    void implement(GeneratedOutput& ostr,
        const ServiceDefinitionSP& svc,
        bool types) const override;

    void implementHelper(GeneratedOutput& ostr,
        const ServiceDefinitionSP& svc,
        bool types) const override;

    void implementRegistration(
        GeneratedOutput& ostr,
        const char* serviceName,
        const ServiceDefinitionSP& svc,
        bool types) const override;

    spdoc::ConstructConstSP getDoc() const override;

    // implementation of Class
    std::string getName(bool includeNamespace, const char* sep) const override;
    std::string getObjectName() const override;
    bool isAbstract() const override;
    bool isWrapperClass() const override;
    bool isDelegate() const override;
    bool isVirtualMethod(const std::string& methodName) const override;
    bool hasNonConstMethods() const override;
    const DataTypeConstSP& getDataType(const ServiceDefinitionSP& svc, bool ignored) const override;
    ClassConstSP getBaseClass() const override;
    std::vector<CoerceFromConstSP> getCoerceFrom() const override;
    bool byValue() const override;

    std::vector<AttributeConstSP> AllAttributes() const override;

protected:
    MapClass(
        const std::vector<std::string>& description,
        const std::string&              name,
        const std::string&              ns,
        bool                            canPut);

private:
    std::vector<std::string> m_description;
    std::string              m_name;
    std::string              m_ns;
    bool                     m_canPut;

    void VerifyAndComplete();

    mutable DataTypeConstSP m_dataType;
    mutable spdoc::ClassConstSP m_doc;
};

#endif
