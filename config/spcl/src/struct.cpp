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
** struct.cpp
***************************************************************************
** Implements the Struct class.
***************************************************************************
*/

#include "struct.hpp"
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
#include "inputConverter.hpp"

/*
***************************************************************************
** Implementation of Struct
***************************************************************************
*/
StructSP Struct::Make(
    const std::vector<std::string>& description,
    const std::string&              name,
    const std::string&              ns,
    const ClassConstSP&             baseClass,
    bool                            noMake,
    const std::string&              objectName,
    bool                            canPut,
    bool                            noId,
    bool                            isVirtual,
    bool                            asValue,
    bool                            uuid,
    bool                            byValue,
    bool useAccessors)
{
    return new Struct(
        description, name, ns, baseClass, noMake, objectName, canPut, noId,
        isVirtual, asValue, uuid, byValue, useAccessors);
}

Struct::Struct(
    const std::vector<std::string>& description,
    const std::string&              name,
    const std::string&              ns,
    const ClassConstSP&             baseClass,
    bool                            noMake,
    const std::string&              objectName,
    bool                            canPut,
    bool                            noId,
    bool                            isVirtual,
    bool                            asValue,
    bool                            uuid,
    bool                            byValue,
    bool useAccessors)
    :
    m_description(description),
    m_name(name),
    m_ns(ns),
    m_baseClass(baseClass),
    m_noMake(noMake),
    m_objectName(objectName),
    m_canPut(canPut),
    m_noId(noId),
    m_isVirtual(isVirtual),
    m_asValue(asValue),
    m_uuid(uuid),
    m_byValue(byValue),
    m_useAccessors(useAccessors),
    m_attributes(),
    m_methods(),
    m_verbatimStart(),
    m_verbatim(),
    m_validation(),
    m_dataType(),
    m_hasVirtualMethods(false),
    m_needsHelperClass(false),
    m_classAttributes(),
    m_doc()
{
    VerifyAndComplete();
}

void Struct::addClassAttribute(const ClassAttributeConstSP& ca)
{
    // FIXME: inefficient
    addAttribute(ca->attribute(),
        ca->canHide(),
        ca->hideIf(),
        ca->accessLevel(),
        ca->fromInnerCode(),
        ca->converter());
}

void Struct::addAttribute(const AttributeConstSP& attribute,
    bool canHide,
    const std::string& hideIf,
    ClassAttributeAccess accessLevel,
    const VerbatimConstSP& code,
    const ConverterConstSP& converter)
{
    if (accessLevel == ClassAttributeAccess::PROPERTY)
    {
        if (!code)
            SPI_THROW_RUNTIME_ERROR("Code must be defined for property "
                << attribute->name() << " of struct " << m_name);

        m_classProperties.push_back(
            ClassAttribute::Make(attribute, accessLevel, code,
                true, canHide, hideIf, false, converter));
    }
    else
    {
        if (code)
            SPI_THROW_RUNTIME_ERROR("Code must not be defined for attribute "
                << attribute->name() << " of struct " << m_name);

        m_attributes.push_back(attribute);
        m_classAttributes.push_back(
            ClassAttribute::Make(attribute, accessLevel, code,
                true, canHide, hideIf, false, converter));
    }
}

void Struct::addMethod(const ClassMethodConstSP& method)
{
    if (method->m_isVirtual)
    {
        if (!m_isVirtual)
        {
            SPI_THROW_RUNTIME_ERROR("Cannot define virtual methods for struct "
                << m_name
                << " - you need to define it using virtual keyword");
        }
        m_hasVirtualMethods = true;
    }

    if (methodNeedsHelper(method))
        m_needsHelperClass = true;

    m_methods.push_back(method);
}

void Struct::addVerbatim(const std::string& verbatimStart,
                         const VerbatimConstSP& verbatim)
{
    this->m_verbatim = verbatim;
    this->m_verbatimStart = verbatimStart;
}

void Struct::addValidation(const VerbatimConstSP& validation)
{
    this->m_validation = validation;
}

void Struct::addCoerceFrom(const CoerceFromConstSP& coerceFrom)
{
    // TBD: validate that we aren't trying to use the same data type again
    m_coerceFromVector.push_back(coerceFrom);
}

void Struct::addCoerceTo(const CoerceToConstSP& coerceTo)
{
    // TBD: validate that we aren't trying to use the same data type again
    m_coerceToVector.push_back(coerceTo);
}

void Struct::addDynamicProperties(const VerbatimConstSP& verbatim)
{
    if (!m_dynamicPropertiesCode)
        m_dynamicPropertiesCode = verbatim;
    else
        throw spi::RuntimeError("dynamic_properties already defined");
}

void Struct::addAsValueCode(const VerbatimConstSP& verbatim)
{
    if (!m_asValueCode)
        m_asValueCode = verbatim;
    else
        throw spi::RuntimeError("as_value already defined");
}

int Struct::preDeclare(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc) const
{
    ostr << "\n";
    ostr << "SPI_DECLARE_OBJECT_CLASS(" << m_name << ");";
    return 1;
}

void Struct::declareClassFunctions(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc) const
{
}

bool Struct::declareInClasses() const
{
    return true;
}

bool Struct::hasNonConstMethods() const
{
    if (m_baseClass && m_baseClass->hasNonConstMethods())
        return true;

    //size_t N = methods.size();
    //for (size_t i = 0; i < N; ++i)
    //{
    //    ClassMethodConstSP method = methods[i];
    //    if (!method->isStatic && !method->isConst)
    //        return true;
    //}

    return false;
}

void Struct::declare(
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

    ostr << "class " << svc->getImport() << " " << m_name;
    if (m_baseClass)
    {
        ostr << " : public " << m_baseClass->getName(true, "::") << "\n";
    }
    else
    {
        ostr << " : public spi::Object\n";
    }

    ostr << "{\n"
         << "public:\n";

    const std::vector<AttributeConstSP>& allAttributes = AllAttributes();
    if (!isAbstract())
    {
        if (m_byValue)
        {
            ostr << "    static " << m_name << " Make";
        }
        else
        {
            ostr << "    static " << m_name << "ConstSP Make";
        }
        writeFunctionInputs(ostr, false, allAttributes, true, 8);
        ostr << ";\n";
    }

    for (size_t i = 0; i < m_coerceFromVector.size(); ++i)
        m_coerceFromVector[i]->declare(ostr, m_name, svc);

    for (size_t i = 0; i < m_coerceToVector.size(); ++i)
        m_coerceToVector[i]->declare(ostr, m_name);

    for (size_t i = 0; i < m_methods.size(); ++i)
        m_methods[i]->declare(ostr, m_dataType, m_name, types, svc);

    if (m_useAccessors)
    {
        bool first = true;
        for (size_t i = 0; i < m_classAttributes.size(); ++i)
        {
            const ClassAttributeConstSP attr = m_classAttributes[i];
            if (attr->isAccessible())
            {
                if (first)
                {
                    ostr << "\n";
                    ostr << "    /* public accessor methods */\n";

                    first = false;
                }
                writeAccessorDeclaration(ostr, attr, m_name);
            }
        }
        for (size_t i = 0; i < m_classProperties.size(); ++i)
        {
            const ClassAttributeConstSP prop = m_classProperties[i];
            SPI_PRE_CONDITION(prop->isAccessible());
            if (first)
            {
                ostr << "\n";
                ostr << "    /* public accessor methods */\n";

                first = false;
            }
            writeAccessorDeclaration(ostr, prop, m_name);
        }
    }

    if (m_asValueCode)
    {
        ostr << "\n"
             << "    spi::Value as_value() const;\n";
    }

    if (m_byValue)
    {
        ostr << "\n"
            << "    typedef " << m_name << " outer_type;\n";
    }
    else
    {
        ostr << "\n"
            << "    typedef spi::ObjectSmartPtr<" << m_name << "> outer_type;\n";
    }

    writeObjectMethodDeclarations(ostr, m_name);
    CoerceTo::declare(ostr, m_name, m_coerceToVector);

    if (allAttributes.size() > 0 || m_classProperties.size() > 0)
    {
        ostr << "\n"
             << "protected:\n"
             << "\n"
             << "    " << m_name;
        writeFunctionInputs(ostr, false, allAttributes, false, 8);
        ostr << ";\n"
            << "\n";

        if (m_useAccessors)
        {
            ostr << "private:\n"
                << "\n";
            // this is the declaration of the class attributes
            writeDeclareArgs(ostr, false, m_attributes, 4, "m_", true);

            bool first = true;
            for (size_t i = 0; i < m_classAttributes.size(); ++i)
            {
                const ClassAttributeConstSP attr = m_classAttributes[i];
                if (!attr->isAccessible())
                {
                    if (first)
                    {
                        ostr << "\n"
                            << "    /* private accessor methods */\n";
                        first = false;
                    }
                    writeAccessorDeclaration(ostr, attr, m_name);
                }
            }
        }
        else
        {
            ostr << "public:\n"
                << "\n";
            // this is the declaration of the struct attributes
            writeDeclareArgs(ostr, false, m_attributes, 4, "", true);
            for (size_t i = 0; i < m_classProperties.size(); ++i)
            {
                const ClassAttributeConstSP prop = m_classProperties[i];
                writeAccessorDeclaration(ostr, prop, m_name, false, true);
            }
        }
    }
    else if (isAbstract())
    {
        if (m_noId)
        {
            ostr << "\n"
                 << "protected:\n"
                 << "\n"
                 << "    " << m_name << "();\n";
        }
    }

    if (m_dynamicPropertiesCode)
    {
        ostr << "\n"
             << "protected:\n"
             << "\n"
             << "    void dynamic_properties(spi::IObjectMap* obj_map) const;\n";
    }

    if (m_verbatimStart.length())
    {
        ostr << "\n" << m_verbatimStart;
        writeVerbatim(ostr, m_verbatim);
    }
    else
    {
        ostr << "};\n";
    }
}

void Struct::declareHelper(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    if (m_needsHelperClass)
    {
        // we need a helper class
        // the helper class has non-static methods but no data
        // hence we always pass through the class instance as first parameter
        ostr << "\n"
             << "class " << m_name << "_Helper";

        ostr << "\n"
             << "{\n"
             << "public:\n";

        if (isAbstract())
        {
            ostr << "\n"
                 << "    virtual ~" << m_name << "_Helper() {}\n";
        }

        for (size_t i = 0; i < m_methods.size(); ++i)
        {
            if (methodNeedsHelper(m_methods[i]))
                m_methods[i]->declareHelper(ostr, m_dataType, m_name, types, svc);
        }

        ostr << "};\n";
    }
}

void Struct::implement(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    // we put the constructor into the regular stream
    // we put the object functions into the helper stream

    writeStartCommentBlock(ostr);
    ostr << "* Implementation of " << m_name << "\n";
    writeEndCommentBlock(ostr);

    const std::vector<AttributeConstSP>& allAttributes = AllAttributes();
    if (!isAbstract())
    {
        if (m_byValue)
        {
            ostr << m_name << " " << m_name << "::Make";
        }
        else
        {
            ostr << m_name << "ConstSP " << m_name << "::Make";
        }
        writeFunctionInputs(ostr, false, allAttributes, false, 4);
        ostr << "\n"
            << "{\n"
            << "    " << svc->getName() << "_check_permission();\n";
        if (m_byValue)
        {
            ostr << "    return " << m_name;
        }
        else
        {
            ostr << "    return " << m_name << "ConstSP(\n"
                << "        new " << m_name;
        }

        if (allAttributes.size() > 0)
        {
            writeArgsCall(ostr, false, allAttributes, m_name.length() + 14, 12);
        }
        if (!m_byValue)
        {
            ostr << ")";
        }
        ostr << ";\n";
        ostr << "}\n";
    }
    else
    {
        if (m_noId)
        {
            ostr << m_name << "::" << m_name << "()\n"
                 << "    : spi::Object(true)\n"
                 << "{}\n";
        }
    }

    if (allAttributes.size() > 0)
    {
        ostr << "\n"
             << m_name << "::" << m_name;
        writeFunctionInputs(ostr, false, allAttributes, false, 4);
        ostr << "\n";

        if (m_noId)
        {
            ostr << "    :\n"
                 << "    spi::Object(true)";
        }

        writeArgsInitialisers(ostr, m_noId, m_attributes, 4, m_useAccessors ? "m_" : "");
        ostr << "\n";

        if (m_validation)
        {
            ostr << "{\n";
            ostr << "try\n"
                 << "  {";
            writeVerbatim(ostr, m_validation, 0);
            writeConstructorCatchBlock(ostr, m_name);
            ostr << "}\n";
        }
        else
        {
            ostr << "{";

            ostr << "}\n";
        }
    }

    for (size_t i = 0; i < m_coerceFromVector.size(); ++i)
        m_coerceFromVector[i]->implement(ostr, m_name, svc);

    for (size_t i = 0; i < m_coerceToVector.size(); ++i)
        m_coerceToVector[i]->implement(ostr, m_name);

    for (size_t i = 0; i < m_methods.size(); ++i)
    {
        bool noHelper = !methodNeedsHelper(m_methods[i]);
        m_methods[i]->implement(ostr, m_dataType, m_name, "", types, svc, noHelper);
    }

    if (m_dynamicPropertiesCode)
    {
        ostr << "\n"
             << "void " << m_name << "::dynamic_properties(spi::IObjectMap* obj_map) const\n"
             << "{";
        writeVerbatim(ostr, m_dynamicPropertiesCode, 0, false);
    }

    if (m_asValueCode)
    {
        ostr << "\n"
             << "spi::Value " << m_name << "::as_value() const\n"
             << "{";
        writeVerbatim(ostr, m_asValueCode, 0, false);
    }

    if (m_useAccessors)
    {
        for (size_t i = 0; i < m_classAttributes.size(); ++i)
        {
            writeClosedAccessor(ostr, m_classAttributes[i], m_name);
        }
    }

    for (size_t i = 0; i < m_classProperties.size(); ++i)
    {
        writeOpenAccessor(ostr, m_classProperties[i], m_name, false, true, true);
    }
}

void Struct::implementHelper(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    // we put the constructor into the regular stream
    // we put the object functions into the helper stream
    writeStartCommentBlock(ostr);
    ostr << "* Implementation of " << m_name << "\n";
    writeEndCommentBlock(ostr);

    CoerceFrom::implement(ostr, m_name, svc, m_coerceFromVector);
    CoerceTo::implement(ostr, m_name, m_coerceToVector);

    std::string coerce_from_value;
    if (m_coerceFromVector.size() > 0)
    {
        coerce_from_value = m_name + "::coerce_from_value";
    }
    else
    {
        coerce_from_value = "0";
    }

    if (isAbstract())
    {
        // this code is replicated in WrapperClass - FIXME
        ostr << "\n"
             << "void " << m_name << "::to_map(\n"
             << "    spi::IObjectMap* obj_map, bool public_only) const\n"
             << "{\n"
             << "    if (has_constructor())\n"
             << "    {\n"
             << "        constructor_to_map(obj_map);\n"
             << "        return;\n"
             << "    }\n"
             << "    throw spi::RuntimeError(\n"
             << "        \"Cannot convert abstract class with unknown "
             << "constructor to map\");\n"
             << "}\n";

        ostr << "\n"
             << "spi::ObjectConstSP " << m_name << "::object_from_map(\n"
             << "    spi::IObjectMap* obj_map,\n"
             << "    spi::ValueToObject& value_to_object)\n"
             << "{\n"
             << "    spi::ObjectConstSP obj = spi::Object::constructor_from_map(\n"
             << "        obj_map, value_to_object);\n"
             << "    if (obj)\n"
             << "        return obj;\n"
             << "    throw spi::RuntimeError(\n"
             << "        \"Cannot create abstract class without constructor\");\n"
             << "}\n";

        ostr << "\n"
             << "SPI_IMPLEMENT_OBJECT_TYPE(" << m_name << ", \""
             << makeNamespaceSep(m_ns, ".")
             << (m_objectName.empty() ? m_name : m_objectName) << "\", "
             << svc->getName() << "_service, "
             << (m_canPut ? "true" : "false") << ", "
             << coerce_from_value << ");\n";
    }
    else
    {
        ostr << "\n"
             << "void " << m_name << "::to_map(\n"
             << "    spi::IObjectMap* obj_map, bool public_only) const\n"
             << "{\n";
        writeToMap(ostr, m_classAttributes, "obj_map", 
            m_useAccessors ? "m_%s" : "%s", 4);
        writePropertiesToMap(ostr, m_classProperties,
            "obj_map", "%s()", 4, !!m_dynamicPropertiesCode);
        ostr << "}\n";

        ostr << "\n"
             << "spi::ObjectConstSP " << m_name << "::object_from_map(\n"
             << "    spi::IObjectMap* obj_map,\n"
             << "    spi::ValueToObject& value_to_object)\n"
             << "{\n";
        std::string constructor = spi::StringFormat("new %s", m_name.c_str());
        writeFromMap(ostr, m_classAttributes, constructor, "obj_map", "value_to_object", 4);

        ostr << "}\n";

        ostr << "\n"
             << "SPI_IMPLEMENT_OBJECT_TYPE(" << m_name << ", \""
             << makeNamespaceSep(m_ns, ".")
             << (m_objectName.empty() ? m_name : m_objectName) << "\", "
             << svc->getName() << "_service, "
             << (m_canPut ? "true" : "false") << ", "
             << coerce_from_value << ");\n";

        writeObjectMethodImplementation(ostr, m_name);

        if (!m_noMake)
        {
            // FIXME: why might we want to use a Functor here?
            writeFunctionCaller(ostr, /*false,*/ m_ns, m_name, std::string(),
                m_dataType, 0, DataTypeConstSP(),
                svc, m_attributes);
        }
    }

    for (size_t i = 0; i < m_methods.size(); ++i)
    {
        if (methodNeedsHelper(m_methods[i]))
            m_methods[i]->implementHelper(ostr, m_dataType, m_name, types, svc);
    }

}

void Struct::implementRegistration(
    GeneratedOutput& ostr,
    const char* serviceName,
    bool types) const
{
    ostr << "    " << serviceName << "->add_object_type(&"
         << m_name << "::object_type);\n";

    if (!isAbstract() && !m_noMake)
    {
        ostr << "    " << serviceName << "->add_function_caller(&" << m_name
             << "_FunctionCaller);\n";
    }

    for (size_t i = 0; i < m_methods.size(); ++i)
        m_methods[i]->implementRegistration(ostr, m_name, serviceName, types);
}

const char* Struct::type() const
{
    return "STRUCT";
}

spdoc::ConstructConstSP Struct::getDoc() const
{
    if (!m_doc)
    {
        std::vector<spdoc::ClassAttributeConstSP> attrDocs;
        std::vector<spdoc::ClassAttributeConstSP> propDocs;
        std::vector<spdoc::ClassMethodConstSP> methodDocs;
        std::vector<spdoc::CoerceFromConstSP> coerceFromDocs;
        std::vector<spdoc::CoerceToConstSP> coerceToDocs;
        for (size_t i = 0; i < m_classAttributes.size(); ++i)
            attrDocs.push_back(m_classAttributes[i]->getDoc(!m_useAccessors, false));
        for (size_t i = 0; i < m_classProperties.size(); ++i)
            propDocs.push_back(m_classProperties[i]->getDoc(!m_useAccessors, true));
        for (size_t i = 0; i < m_methods.size(); ++i)
            methodDocs.push_back(m_methods[i]->getDoc());
        for (size_t i = 0; i < m_coerceFromVector.size(); ++i)
            coerceFromDocs.push_back(m_coerceFromVector[i]->getDoc());
        for (size_t i = 0; i < m_coerceToVector.size(); ++i)
            coerceToDocs.push_back(m_coerceToVector[i]->getDoc());

        std::string baseClassName = m_baseClass ?
            m_baseClass->getName(true, ".") : "";

        m_doc = spdoc::Class::Make(m_name, m_ns, m_description, baseClassName,
            attrDocs, propDocs, methodDocs,
            coerceFromDocs, coerceToDocs, isAbstract(),
            m_noMake || isAbstract(), m_objectName,
            m_dataType->getDoc(), isDelegate(), m_canPut,
            !!m_dynamicPropertiesCode,
            m_asValue);
    }
    return m_doc;
}

const DataTypeConstSP& Struct::getDataType(const ServiceDefinitionSP& svc, bool ignored) const
{
    if (!m_dataType)
    {
        // outerType is called StructConstSP
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
        std::string cppName = m_ns.empty() ?
            m_name :
            m_ns + "::" + m_name;

        std::string outerTypeName = hasNonConstMethods() ?
            cppName + "ConstSP" :
            cppName + "ConstSP";

        //if (svc->getCppType(outerTypeName))
        //{
        //    throw spi::RuntimeError("CppType %s is already defined",
        //                            outerTypeName.c_str());
        //}
        //CppTypeConstSP outerType = CppType::Make(outerTypeName, true);
        //
        //svc->addCppType(outerType);

        if (m_byValue)
        {
            // the outer type is <struct>ConstSP as before
            // the inner type is <struct>
            //
            // outer -> inner = *outer
            // inner -> outer = <struct>ConstSP(new <struct>(inner))
#if 0
            std::string outerType = outerTypeName + "&";
            std::string innerType = m_ns.empty() ? m_name : m_ns + "::" + m_name;
            std::string innerRefType = "const " + innerType + "&";
            std::string convertOut = "*%s";
            std::string convertIn = outerTypeName + "(new " + innerType + "(%s))";
            std::string copyInner;
            m_dataType = DataType::Make(
                m_name, m_ns, svc->getNamespace(), outerType,
                innerType, innerRefType,
                spdoc::PublicType::CLASS, m_objectName,
                false, false,
                InputConverter::Class(convertOut),
                convertIn,
                copyInner, 
                DataTypeConstSP(), false,
                m_asValue, ignored);
#endif
            // we need something to be able to convert <struct>& to spi::Value
            m_dataType = DataType::Make(
                m_name, m_ns, svc->getNamespace(), cppName, cppName,
                std::string(), std::string(),
                spdoc::PublicType::CLASS, m_objectName,
                false, false,
                InputConverter::Class(std::string()),
                std::string(), std::string(),
                DataTypeConstSP(), false,
                true, ignored);
        }
        else
        {
            std::string outerType = outerTypeName + "&";
            m_dataType = DataType::Make(
                m_name, m_ns, svc->getNamespace(), cppName, outerType,
                std::string(), std::string(),
                spdoc::PublicType::CLASS, m_objectName, false, false,
                InputConverter::Class(std::string()),
                std::string(), std::string(), DataTypeConstSP(), false,
                m_asValue, ignored);
        }

        svc->addDataType(m_dataType);
    }
    return m_dataType;
}

std::string Struct::getName(bool includeNamespace, const char* sep) const
{
    if (!includeNamespace || m_ns.empty())
        return m_name;

    return m_ns + sep + m_name;
}

std::string Struct::getObjectName() const
{
    if (m_objectName.length() > 0)
        return m_objectName;

    return getName(true, ".");
}

bool Struct::isAbstract() const
{
    return m_isVirtual || m_hasVirtualMethods;
}

bool Struct::isWrapperClass() const
{
    return false;
}

bool Struct::isDelegate() const
{
    return false;
}

bool Struct::isVirtualMethod(const std::string& methodName) const
{
    for (size_t i = 0; i < m_methods.size(); ++i)
    {
        if (methodName == m_methods[i]->m_function->name() && m_methods[i]->m_isVirtual)
            return true;
    }
    return false;
}

std::vector<CoerceFromConstSP> Struct::getCoerceFrom() const
{
    return std::vector<CoerceFromConstSP>();
}

std::vector<AttributeConstSP> Struct::AllAttributes() const
{
    return m_attributes;
}

bool Struct::methodNeedsHelper(const ClassMethodConstSP& method) const
{
    if (!m_baseClass)
        return true;

    if (!m_baseClass->isDelegate())
        return true;

    if (m_baseClass->isVirtualMethod(method->m_function->name()))
        return false;

    return true;
}

void Struct::VerifyAndComplete()
{
    if (m_byValue)
    {
        SPI_THROW_RUNTIME_ERROR("byValue not supported yet");
    }
    if (m_baseClass)
    {
        SPI_PRE_CONDITION(m_baseClass->isAbstract());
        if (m_baseClass->getBaseClass())
            throw spi::RuntimeError("Only allow one level of inheritance");
        if (m_baseClass->isWrapperClass() && !m_baseClass->isDelegate())
            throw spi::RuntimeError("Cannot sub-class an undelegated wrapper class");
        if (m_noId)
            throw spi::RuntimeError("Cannot have noId for a derived class");
    }
}

ClassConstSP Struct::getBaseClass() const
{
    return m_baseClass;
}

