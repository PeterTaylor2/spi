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
/*
***************************************************************************
** mapClass.cpp
***************************************************************************
** Defines the MapClass classes used in the configuration parser.
** These are sub-classes of spi::IObjectMap defined solely by the name
** of the class and whether we can modify the object from the add-in layer.
***************************************************************************
*/

#include "mapClass.hpp"
#include "function.hpp"

#include <spi/RuntimeError.hpp>
#include <spi/Service.hpp>
#include <spi/StringUtil.hpp>
#include <spi/spdoc_configTypes.hpp>

#include "classMethod.hpp"
#include "coerceTo.hpp"
#include "constant.hpp"
#include "cppType.hpp"
#include "innerClass.hpp"
#include "moduleDefinition.hpp"
#include "serviceDefinition.hpp"
#include "verbatim.hpp"
#include "generatorTools.hpp"

/*
***************************************************************************
** Implementation of MapClass
***************************************************************************
*/
MapClassConstSP MapClass::Make(
    const std::vector<std::string>& description,
    const std::string&              name,
    const std::string&              ns,
    bool                            canPut,
    bool                            uuid)
{
    return new MapClass(
        description, name, ns, canPut, uuid);
}

MapClass::MapClass(
    const std::vector<std::string>& description,
    const std::string&              name,
    const std::string&              ns,
    bool                            canPut,
    bool                            uuid)
    :
    m_description(description),
    m_name(name),
    m_ns(ns),
    m_canPut(canPut),
    m_uuid(uuid),
    m_dataType(),
    m_doc()
{
    VerifyAndComplete();
}

int MapClass::preDeclare(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc) const
{
    ostr << "\n";
    ostr << "SPI_DECLARE_OBJECT_CLASS(" << m_name << ");";
    return 1;
}

void MapClass::declare(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    // we put the definition into the regular stream
    // we put the Functor into the helper stream
    writeStartCommentBlock(ostr, true);
    if (m_description.size() == 0)
        ostr << "* No description.\n";
    else
        writeComments(ostr, m_description);
    writeEndCommentBlock(ostr);

    ostr << "class " << svc->getImport() << " " << m_name
         << " : public spi::IMapObject\n";

    ostr << "{\n"
         << "public:\n"
         << "    static " << m_name << "ConstSP Make(const spi::MapSP& aMap);\n"
         << "    static " << m_name << "ConstSP Make(const spi::MapConstSP& aMap,\n"
         << "        spi::ValueToObject& valueToObject);\n"
         << "    SPI_DECLARE_MAP_OBJECT_TYPE(" << m_name << ");\n"
         << "protected:\n"
         << "    " << m_name << "(const spi::MapSP& aMap);\n"
         << "};\n";
}

bool MapClass::declareInClasses() const
{
    return true;
}

bool MapClass::hasNonConstMethods() const
{
    return false;
}

void MapClass::declareHelper(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{}

void MapClass::implement(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    // we put the constructor into the regular stream
    // we put the object functions into the helper stream

    writeStartCommentBlock(ostr);
    ostr << "* Implementation of " << m_name << "\n";
    writeEndCommentBlock(ostr);

    ostr << m_name << "ConstSP " << m_name << "::Make(const spi::MapSP& aMap)\n"
         << "{\n"
         << "    return " << m_name << "ConstSP("
         << "new " << m_name << "(aMap));\n"
         << "}\n"
         << "\n"
         << m_name << "ConstSP " << m_name << "::Make(const spi::MapConstSP& aMap,\n"
         << "    spi::ValueToObject& valueToObject)\n"
         << "{\n"
         << "    return " << m_name << "ConstSP("
         << "new " << m_name << "(aMap->Copy(valueToObject)));\n"
         << "}\n"
         << "\n"
         << m_name << "::" << m_name << "(const spi::MapSP& aMap)\n"
         << "    :\n"
         << "    spi::IMapObject(aMap)\n"
         << "{}\n";
}

void MapClass::implementHelper(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    ostr << "\n"
         << "SPI_IMPLEMENT_MAP_OBJECT_TYPE(" << m_name << ", \""
         << makeNamespaceSep(m_ns, ".") << m_name << "\", "
         << svc->getName() << "_service, "
         << (m_canPut ? "true" : "false")
         << ");\n";
}

void MapClass::implementRegistration(
    GeneratedOutput& ostr,
    const char* serviceName,
    bool types) const
{
    ostr << "    " << serviceName << "->add_object_type(&"
         << m_name << "::object_type);\n";
}

//const char* MapClass::type() const
//{
//    return "MAP";
//}

spdoc::ConstructConstSP MapClass::getDoc() const
{
    if (!m_doc)
    {
        std::vector<spdoc::ClassAttributeConstSP> attrDocs;
        std::vector<spdoc::ClassAttributeConstSP> propDocs;
        std::vector<spdoc::ClassMethodConstSP> methodDocs;
        std::vector<spdoc::CoerceFromConstSP> coerceFromDocs;
        std::vector<spdoc::CoerceToConstSP> coerceToDocs;

        std::string baseClassName;
        std::string constructor;

        m_doc = spdoc::Class::Make(
            m_name, m_ns, m_description, baseClassName,
            attrDocs, propDocs, methodDocs,
            coerceFromDocs, coerceToDocs,
            false, // isAbstract
            true, // noMake
            std::string(), // objectName
            m_dataType->getDoc(),
            false, // isDelegate
            m_canPut,
            true, // hasDynamicAttributes
            false, // asValue
            constructor);
    }
    return m_doc;
}

const DataTypeConstSP& MapClass::getDataType(const ServiceDefinitionSP& svc, bool ignored) const
{
    if (!m_dataType)
    {
        // outerType is called MapClassConstSP
        // innerType is the same
        //
        // needs to be a newly defined CppType
        //
        // in addition name needs to be not previously defined as DataType

        if (svc->getDataType(m_name))
        {
            throw spi::RuntimeError("DataType %s is already defined",
                                    m_name.c_str());
        }
        std::string cppName = m_ns.empty() ? m_name : m_ns + "::" + m_name;
        std::string outerType = cppName + "ConstSP&";
        m_dataType = DataType::Make(
            m_name, m_ns, svc->getNamespace(), cppName, outerType, 
            std::string(), std::string(),
            spdoc::PublicType::CLASS, "", false, false, "", "", "", DataTypeConstSP(),
            false, false, ignored);
        svc->addDataType(m_dataType);
    }
    return m_dataType;
}

std::string MapClass::getName(bool includeNamespace, const char* sep) const
{
    if (!includeNamespace || m_ns.empty())
        return m_name;

    return m_ns + sep + m_name;
}

std::string MapClass::getObjectName() const
{
    return m_name;
}

bool MapClass::isAbstract() const
{
    return false;
}

bool MapClass::isWrapperClass() const
{
    return false;
}

bool MapClass::isDelegate() const
{
    return false;
}

bool MapClass::isVirtualMethod(const std::string& methodName) const
{
    throw spi::RuntimeError("MapClass has no methods");
}

std::vector<CoerceFromConstSP> MapClass::getCoerceFrom() const
{
    return std::vector<CoerceFromConstSP>();
}

void MapClass::VerifyAndComplete()
{
}

ClassConstSP MapClass::getBaseClass() const
{
    return ClassConstSP();
}

bool MapClass::hasObjectId() const
{
    return m_uuid;
}

bool MapClass::byValue() const
{
    return false;
}

std::vector<AttributeConstSP> MapClass::AllAttributes() const
{
    SPI_THROW_RUNTIME_ERROR("AllAttributes not implemented for MapClass");
}
