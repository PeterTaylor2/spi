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
** wrapperClass.cpp
***************************************************************************
** Implements the WrapperClass.
***************************************************************************
*/

#include "wrapperClass.hpp"
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
** Implementation of WrapperClass
***************************************************************************
*/
WrapperClassSP WrapperClass::Make(
    const std::vector<std::string>& description,
    const std::string&              name,
    const std::string&              ns,
    const InnerClassConstSP&        innerClass,
    const WrapperClassConstSP&      baseClass,
    bool                            isVirtual,
    bool                            noMake,
    const std::string&              objectName,
    bool                            isDelegate,
    bool                            canPut,
    bool                            noId,
    bool                            asValue,
    bool                            uuid,
    bool incomplete,
    const std::string& accessorFormat,
    const std::string& propertyFormat,
    const std::string& constructor)
{
    return new WrapperClass(
        description, name, ns, innerClass, baseClass, isVirtual, noMake,
        objectName, isDelegate, canPut, noId, asValue, uuid, incomplete,
        accessorFormat, propertyFormat, constructor);
}

WrapperClass::WrapperClass(
    const std::vector<std::string>& description,
    const std::string&              name,
    const std::string&              ns,
    const InnerClassConstSP&        innerClass,
    const WrapperClassConstSP&      baseClass,
    bool                            isVirtual,
    bool                            noMake,
    const std::string&              objectName,
    bool                            isDelegate,
    bool                            canPut,
    bool                            noId,
    bool                            asValue,
    bool                            uuid,
    bool incomplete,
    const std::string& accessorFormat,
    const std::string& propertyFormat,
    const std::string& constructor)
    :
    m_description(description),
    m_name(name),
    m_ns(ns),
    m_innerClass(innerClass),
    m_baseClass(baseClass),
    m_isVirtual(isVirtual),
    m_noMake(noMake),
    m_objectName(objectName),
    m_isDelegate(isDelegate),
    m_canPut(canPut),
    m_noId(noId),
    m_asValue(asValue),
    m_uuid(uuid),
    m_incomplete(incomplete),
    m_accessorFormat(accessorFormat),
    m_propertyFormat(propertyFormat),
    m_constructor(constructor),
    m_verbatimConstructor(),
    m_classAttributes(),
    m_methods(),
    m_delegatePrototypes(),
    m_delegateImplementations(),
    m_coerceFromVector(),
    m_coerceToVector(),
    m_validateInnerName(),
    m_validateInnerCode(),
    m_dataType(),
    m_hasVirtualMethods(false),
    m_attributes(),
    m_doc()
{
    VerifyAndComplete();
}

void WrapperClass::addClassAttribute(
    const ClassAttributeConstSP& classAttribute)
{
    bool isProperty =
        classAttribute->accessLevel() == ClassAttributeAccess::PROPERTY;

    // this restriction is because we haven't implemented the code to
    // generate a call to the base-class initializer with its attributes
    //
    // in fact we used to prevent attributes in base classes at all until
    // a client use case seemed to require it
    if (m_isVirtual && !isProperty && !m_innerClass->m_isOpen)
        throw spi::RuntimeError("Cannot add attributes to a closed virtual class");

    if (isProperty)
    {
        m_classProperties.push_back(classAttribute);
    }
    else
    {
        m_classAttributes.push_back(classAttribute);
        m_attributes.push_back(classAttribute->attribute());
    }
}

void WrapperClass::addMethod(const ClassMethodConstSP& method)
{
    if (method->isVirtual())
        m_hasVirtualMethods = true;

    m_methods.push_back(method);
}

void WrapperClass::addVerbatim(const std::string& verbatimStart,
                               const VerbatimConstSP& verbatim)
{
    this->m_verbatim = verbatim;
    this->m_verbatimStart = verbatimStart;
}

void WrapperClass::addCoerceFrom(const CoerceFromConstSP& coerceFrom)
{
    // TBD: validate that we aren't trying to use the same data type again
    m_coerceFromVector.push_back(coerceFrom);
}

void WrapperClass::addCoerceTo(const CoerceToConstSP& coerceTo)
{
    // TBD: validate that we aren't trying to use the same data type again
    m_coerceToVector.push_back(coerceTo);
}

void WrapperClass::addValidateInner(const std::string& name,
    const VerbatimConstSP& code)
{
    if (m_validateInnerName.empty() && !m_validateInnerCode)
    {
        m_validateInnerName = name;
        m_validateInnerCode = code;
    }
    else
    {
        throw spi::RuntimeError("validate_inner already defined");
    }
}

#if 0
void WrapperClass::addSetInner(const std::string& name,
    const VerbatimConstSP& code)
{
    if (setInnerName.empty() && !setInnerCode)
    {
        setInnerName = name;
        setInnerCode = code;
    }
    else
    {
        throw spi::RuntimeError("set_inner already defined");
    }
}
#endif

void WrapperClass::addDynamicProperties(const VerbatimConstSP& verbatim)
{
    if (!m_dynamicPropertiesCode)
        m_dynamicPropertiesCode = verbatim;
    else
        throw spi::RuntimeError("dynamic_properties already defined");
}

void WrapperClass::addAsValueCode(const VerbatimConstSP& verbatim)
{
    if (!m_asValueCode)
        m_asValueCode = verbatim;
    else
        throw spi::RuntimeError("as_value already defined");
}

void WrapperClass::addDelegate(
    const std::string& prototype,
    const VerbatimConstSP& implementation)
{
    m_delegatePrototypes.push_back(prototype);
    m_delegateImplementations.push_back(implementation);
}

void WrapperClass::declareInner(
    GeneratedOutput& ostr,
    NamespaceManager& nsman,
    const ServiceDefinitionSP& svc) const
{
    m_innerClass->preDeclare(ostr, nsman);
}

int WrapperClass::preDeclare(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc) const
{
    ostr << "\n";
    ostr << "SPI_DECLARE_OBJECT_CLASS(" << m_name << ");";
    return 1;
}

void WrapperClass::declareClassFunctions(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc) const
{
}

bool WrapperClass::declareInClasses() const
{
    // it would be nice if we could only declare abstract wrapper classes in
    // the classes header file, but in practice that would mean too many
    // specific includes of the non-classes header file that it is impractical
    // in practice (NOTE: it was tried and it failed to compile)
    return true;
}

bool WrapperClass::hasNonConstMethods() const
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

void WrapperClass::declare(
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

    const std::vector<AttributeConstSP>& attributes = AllAttributes();
    if (hasConstructor() && !m_noMake)
    {
        ostr << "    static " << m_name << "ConstSP Make";
        writeFunctionInputs(ostr, false, attributes, true, 8);
        ostr << ";\n";
    }

    for (size_t i = 0; i < m_coerceFromVector.size(); ++i)
        m_coerceFromVector[i]->declare(ostr, m_name, svc);

    for (size_t i = 0; i < m_coerceToVector.size(); ++i)
        m_coerceToVector[i]->declare(ostr, m_name);

    for (size_t i = 0; i < m_methods.size(); ++i)
        m_methods[i]->declare(ostr, m_dataType, m_name, types, svc, m_isDelegate);

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

    if (m_asValueCode)
    {
        ostr << "\n"
             << "    spi::Value as_value() const;\n";
    }

    ostr << "\n"
        << "    typedef spi::ObjectSmartPtr<" << m_name << "> outer_type; \n";
    writeObjectMethodDeclarations(ostr, m_name);
    CoerceTo::declare(ostr, m_name, m_coerceToVector);

    ostr << "\n";

    if (m_innerClass->m_allowConst)
    {
        // we will allow construction from const innerClass via Wrap only
        // so we need to define the const_inner_type for that call
        ostr << "    typedef " << m_innerClass->m_constCppType << " const_inner_type; \n";
    }

    ostr << "    typedef " << m_innerClass->m_cppType << " inner_type;\n"
         << "    inner_type get_inner() const;\n"
         << "    static inner_type get_inner(const outer_type& o);\n";

    ostr << "\n"
         << "    ~" << m_name << "();\n";

    ostr << "\n"
         << "    static " << m_name
         << "ConstSP Wrap(const inner_type& inner";

    if (!m_innerClass->m_isOpen)
        ostr << ", const spi::FunctionConstSP& func";

    ostr << ");\n";

    if (m_innerClass->m_allowConst)
    {
        // this function is called when we try to wrap a const inner pointer
        // under circumstances where we normally wrap non-const inner pointers
        ostr << "    static " << m_name
            << "ConstSP Wrap(const const_inner_type& inner";

        if (!m_innerClass->m_isOpen)
            ostr << ", const spi::FunctionConstSP& func";

        ostr << ");\n";
    }

    if (m_baseClass)
    {
        ostr << "    static " << m_baseClass->m_name
             << "ConstSP BaseWrap(const " << m_baseClass->m_name
             << "::inner_type& inner";

        if (!m_baseClass->m_innerClass->m_isOpen)
            ostr << ", const spi::FunctionConstSP& func";

        ostr << ");\n";
    }

    if (isAbstract())
    {
        ostr << "\n"
             << "    typedef " << m_name
             << "ConstSP (sub_class_wrapper)(const inner_type& inner";

        if (!m_innerClass->m_isOpen)
            ostr << ", const spi::FunctionConstSP& func";

        ostr << ");\n"
             << "\n"
             << "    static std::vector<sub_class_wrapper*> "
             << "g_sub_class_wrappers;\n";
    }

    if (hasConstructor() && m_noMake)
    {
        // consider constructing as non-const if inner type is non-const
        ostr << "    static " << m_name << "ConstSP Make";
        writeFunctionInputs(ostr, false, attributes, true, 8);
        ostr << ";\n";
    }

    ostr << "\n"
         << "protected:\n"
         << "\n"
         << "    void set_inner(const " << m_innerClass->m_sharedPointer << "&);\n";

    if (m_validateInnerName.length() > 0)
    {
        ostr << "    static bool validate_inner(const inner_type&);\n";
    }

    if (isAbstract())
    {
        ostr << "    " << m_name << "();\n";
    }

    // for an open class we will construct by calling make_inner from the Make
    // function and then wrapping the resulting inner type
    if (hasConstructor() && !m_innerClass->m_isOpen)
    {
        ostr << "\n"
             << "    " << m_name;
        writeFunctionInputs(ostr, false, attributes, false, 8);
        ostr << ";\n";
    }

    if (m_innerClass->m_isOpen)
    {
        if (!isAbstract())
        {
            ostr << "\n"
                << "    " << m_name << "(const inner_type& inner);\n";
        }
    }
    else
    {
        ostr << "\n"
            << "    " << m_name << "(const inner_type& inner, "
            << "const spi::FunctionConstSP& func);\n";
    }

    if (m_isDelegate)
    {
        ostr << "\n"
             << "    inner_type get_delegate() const;\n";
    }

    if (m_dynamicPropertiesCode)
    {
        ostr << "\n"
             << "    void dynamic_properties(spi::IObjectMap* obj_map) const;\n";
    }

    ostr << "\n"
         << "private:\n";

    if (hasConstructor())
    {
        ostr << "\n"
             << "    static inner_type make_inner";
        writeFunctionInputs(ostr, false, attributes, false, 8);
        ostr << ";\n";
    }

    if (!m_innerClass->m_isOpen)
    {
        // we only need attributes declared for closed inner classes
        if (attributes.size() > 0)
        {
            ostr << "\n";
            writeDeclareArgs(ostr, false, attributes, 4, "m_");
        }
    }

    first = true;
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

    ostr << "\n"
         << "    /* shared pointer to implementation */\n"
         << "    " << m_innerClass->m_sharedPointer << " inner_value;\n";

    if (m_innerClass->m_isCached && !m_isVirtual)
    {
        ostr << "\n"
            << "    spi::ObjectWrapperCacheSP wrapper_cache;\n";
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


void WrapperClass::declareHelper(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    // we need a helper class (FIXME - clearly there are cases were we don't need it)
    // the helper class has non-static methods but no data
    // hence we always pass through the class instance as first parameter
    ostr << "\n"
         << "class " << m_name << "_Helper";

    ostr << "\n"
         << "{\n"
         << "public:\n";

    for (size_t i = 0; i < m_methods.size(); ++i)
        m_methods[i]->declareHelper(ostr, m_dataType, m_name, types, svc);

    if ((m_classAttributes.size() > 0 && m_innerClass->m_isOpen) ||
        m_classProperties.size() > 0)
    {
        if (m_innerClass->m_isOpen)
        {
            for (size_t i = 0; i < m_classAttributes.size(); ++i)
                writeAccessorDeclaration(ostr, m_classAttributes[i], m_name, true);
        }

        for (size_t i = 0; i < m_classProperties.size(); ++i)
            writeAccessorDeclaration(ostr, m_classProperties[i], m_name, true);
    }

    if (m_innerClass->m_isCached && !m_isVirtual)
    {
        ostr << "\n"
             << "    static spi::ObjectWrapperCacheSP cache;\n";
    }

    ostr << "};\n";

}

void WrapperClass::implement(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    writeStartCommentBlock(ostr);
    ostr << "* Implementation of " << m_name << "\n";
    writeEndCommentBlock(ostr);

    const std::vector<AttributeConstSP>& attributes = AllAttributes();
    if (hasConstructor())
    {
        // write the static Make method
        ostr << m_name << "ConstSP " << m_name << "::Make";
        writeFunctionInputs(ostr, false, attributes, false, 4);

        ostr << "\n"
            << "{\n"
            << "  SPI_PROFILE(\"" << svc->getNamespace() << "." << getName(true, ".") << ".Make\");\n";

        if (m_innerClass->m_isOpen)
        {
            // we need to take account of the fact that the constructor
            // might return a singleton and hence we create the inner type
            // and then wrap it in order to use wrapper cache mechanism
            ostr << "  " << svc->getName() << "_check_permission();\n";
            ostr << "  try\n"
                 << "  {\n"
                 << "    inner_type self = make_inner";

            writeArgsCall(ostr, false, attributes, 33, 8);
            ostr << ";\n";
            ostr << "    return Wrap(self);\n";
            ostr << "  }\n";
            writeConstructorCatchBlock(ostr, m_name);
        }
        else
        {
            ostr << "    " << svc->getName() << "_check_permission();\n";
            ostr << "    return " << m_name << "ConstSP(\n"
                 << "        new " << m_name;

            if (attributes.size() > 0)
            {
                writeArgsCall(ostr, false, attributes, m_name.length() + 14, 12);
            }
            ostr << ");\n";
        }
        ostr << "}\n";
    }

    if (hasConstructor() && !m_innerClass->m_isOpen)
    {
        // write the constructor

        // TBD: potentially even classes which don't have a Make method
        // should have a constructor which potentially does nothing
        // but initialise the implementation
        //
        // however we haven't declared a constructor so we cannot
        // implement one
        ostr << "\n"
             << m_name << "::" << m_name;
        writeFunctionInputs(ostr, false, attributes, false, 4);
        ostr << "\n";
        if (m_noId)
            ostr << "    :\n"
                 << "    spi::Object(true)";

        if (!m_innerClass->m_isOpen)
        {
            // FIXME: closed class with attributes in base class
            // currently we prevent this in addClassAttribute

            // we only need argument initialisers if the inner class is closed
            writeArgsInitialisers(ostr, m_noId, attributes, 4, "m_");
            ostr << "\n";
        }
        ostr << "{\n"
             << "  try\n"
             << "  {\n"
             << "    inner_type self = make_inner";
        writeArgsCall(ostr, true, attributes, 32, 8);
        ostr << ";\n"
             << "    set_inner(" << m_innerClass->ToSharedPointer("self") << ");\n"
             << "  }\n";
        writeConstructorCatchBlock(ostr, m_name);
        ostr << "}\n";
    }

    if (hasConstructor())
    {
        // make_inner creates the inner type from the constructor
        // we need it as a separate function so that verbatim code
        // can return or throw, and we deal with this inside the
        // constructor itself which has no verbatim code
        ostr << "\n"
             << m_name << "::inner_type " << m_name << "::make_inner";

        if (m_verbatimConstructor)
        {
            writeFunctionInputs(ostr, false, attributes, false, 4, "o_");
            ostr << "\n";
            ostr << "{\n";

            writeInnerDeclarationsAndTranslations(ostr, attributes,
                "", true, "o_");

            writeVerbatim(ostr, m_verbatimConstructor);
        }
        else
        {
            writeFunctionInputs(ostr, false, attributes, false, 4);
            ostr << "\n";
            ostr << "{\n";

            // we need to construct the inner type at this point
            // we will therefore need to convert all the inputs

            writeInnerDeclarationsAndTranslations(ostr, attributes, "i_");

            // we assume that we can just create the inner type using new
            //
            // if there is implementation for the class then we will need
            // to add a method to the helper class
            std::string fullTypeName =
                m_innerClass->m_ns + "::" + m_innerClass->m_typeName;

            ostr << "    " << m_name << "::inner_type self";

            if (m_innerClass->m_byValue)
            {
                writeArgsCall(ostr, true, attributes,
                    m_name.length() + 23,
                    8, "i_");
                ostr << ";\n";
            }
            else
            {
                ostr << " (new " << fullTypeName;
                writeArgsCall(ostr, true, attributes,
                    m_name.length() + fullTypeName.length() + 26,
                    8, "i_");
                ostr << ");\n";
            }
            ostr << "    return self;\n"
                 << "}\n";
        }
    }

    ostr << "\n"
        << m_name << "::inner_type " << m_name << "::get_inner() const\n"
        << "{\n";

    if (m_innerClass->m_isShared)
    {
        if (m_isDelegate)
        {
            ostr << "    if (!inner_value)\n"
                << "        return get_delegate();\n";
        }

        ostr << "    return inner_value;\n";
    }
    else if (m_innerClass->m_byValue)
    {
        ostr << "    return inner_value ? *inner_value : inner_type();\n";
    }
    else
    {
        ostr << "    return inner_value.get();\n";
    }
    ostr << "}\n";

    ostr << "\n"
        << m_name << "::inner_type " << m_name << "::get_inner(const " << m_name
        << "::outer_type& o)\n"
        << "{\n"
        << "    if (!o)\n"
        << "        return inner_type();\n"
        << "    return o->get_inner();\n"
        << "}\n";

    ostr << "\n"
        << "void " << m_name << "::set_inner(const " << m_innerClass->m_sharedPointer
        << "& self)\n"
        << "{\n"
        << "    SPI_PRE_CONDITION(!inner_value);\n"
        << "    inner_value = self;\n";

#if 0
    if (setInnerName.length() > 0)
    {
        // we need to put this code into its own scope to avoid name clashes
        ostr << "    {\n"
            << "        inner_type " << setInnerName << " = get_inner();";
        writeVerbatim(ostr, setInnerCode, 4);
    }
#endif

    if (m_innerClass->m_isCached && !m_isVirtual)
    {
        ostr << "    wrapper_cache = " << m_name << "_Helper::cache;\n"
            << "    wrapper_cache->Insert(inner_value.get(), this);\n";
    }

    if (m_baseClass)
    {
        ostr << "    " << m_baseClass->m_name << "::set_inner(self);\n";
    }

    ostr << "}\n";

    if (m_validateInnerName.length() > 0)
    {
        ostr << "\n"
            << "bool " << m_name << "::validate_inner(const inner_type& " << m_validateInnerName << ")\n"
            << "{";
        writeVerbatim(ostr, m_validateInnerCode);
    }

    if (isAbstract())
    {
        ostr << "\n"
             << m_name << "::" << m_name << "()\n";
        if (m_noId)
            ostr << "    : spi::Object(true)\n";
        ostr << "{}\n";
    }

    ostr << "\n"
         << m_name << "::~" << m_name << "()\n"
         << "{";

    if (m_innerClass->m_isCached && !m_isVirtual)
    {
        ostr << "\n"
            << "    if (wrapper_cache)\n"
            << "        wrapper_cache->Delete(inner_value.get(), this);\n";
    }

    ostr << "}\n";

    if (m_innerClass->m_isOpen)
    {
        if (m_baseClass)
        {
            ostr << "\n"
                << m_baseClass->m_name << "ConstSP " << m_name
                << "::BaseWrap(const " << m_baseClass->m_name
                << "::inner_type& baseInner";

            if (!m_baseClass->m_innerClass->m_isOpen)
            {
                ostr << ", const spi::FunctionConstSP& func";
            }

            ostr << ")\n"
                 << "{\n";

            if (m_innerClass->m_isShared)
            {
                ostr << "    inner_type inner = " << m_innerClass->m_spDynamicCast
                     << "< " << m_innerClass->m_fullTypeName << " >(baseInner);\n";
            }
            else
            {
                ostr << "    inner_type inner = dynamic_cast<inner_type>"
                     << "(baseInner);\n";
            }

            ostr << "    if (!inner";

            if (m_validateInnerName.length() > 0)
            {
                ostr << " || !validate_inner(inner)";
            }

            ostr << ")\n"
                << "        return " << m_baseClass->m_name << "ConstSP();\n";

            ostr << "    return Wrap(inner);\n";
            ostr << "}\n";
        }

        const char* innerPtr = m_innerClass->m_isShared ? "inner.get()" : "inner";

        ostr << "\n"
            << m_name << "ConstSP " << m_name
            << "::Wrap(const inner_type& inner)\n"
            << "{\n";

        if (m_innerClass->m_boolTest == "true")
        {
            // don't test anything and hence don't allow return of an empty shared pointer
        }
        else
        {
            if (m_innerClass->m_boolTest.empty())
            {
                ostr << "    if (!inner)\n";
            }
            else
            {
                ostr << "    if (!(" << m_innerClass->m_boolTest << "))\n";
            }
            ostr << "        return " << m_name << "ConstSP();\n";
        }

        if (m_innerClass->m_isCached && !m_isVirtual)
        {
            ostr << "    const spi::Object* obj = " << m_name << "_Helper::cache->Find("
                 << innerPtr << ");\n"
                 << "    if (obj)\n"
                 << "        return Coerce(spi::ObjectConstSP(obj));\n";
        }

        if (isAbstract())
        {
            ostr << "    for (size_t i = 0; i < g_sub_class_wrappers.size(); "
                 << "++i)\n"
                 << "    {\n"
                 << "        " << m_name
                 << "ConstSP o = g_sub_class_wrappers[i](inner);\n"
                 << "        if (o)\n"
                 << "            return o;\n"
                 << "    }\n"
                 << "    throw spi::RuntimeError(\"%s: Failed!\", "
                 << "__FUNCTION__);\n";
        }
        else
        {
            if (m_validateInnerName.length() > 0)
            {
                ostr << "    if (!validate_inner(inner))\n"
                     << "        throw spi::RuntimeError(\"%s: Invalid inner type\","
                     << "__FUNCTION__);\n";
            }
            ostr << "    return " << m_name << "ConstSP(new "
                << m_name << "(inner));\n";
        }
        ostr << "}\n";

        if (m_innerClass->m_allowConst)
        {
            SPI_PRE_CONDITION(!m_innerClass->m_byValue); // hence we can skip the boolTest stuff
            ostr << "\n"
                << m_name << "ConstSP " << m_name
                << "::Wrap(const const_inner_type& const_inner)\n"
                << "{\n"
                << "    if (!const_inner)\n"
                << "        return " << m_name << "ConstSP(); \n";

            // call the copy constructor and wrap as non-const
            if (m_innerClass->m_isShared)
            {
                ostr << "    inner_type inner(new " << m_innerClass->m_name << "(*const_inner));\n";
            }
            else
            {
                ostr << "    inner_type inner = new " << m_innerClass->m_name << "(*const_inner);\n";
            }
            ostr << "    return Wrap(inner);\n"
                << "}\n";
        }
    }
    else
    {
        // we can create instances of the class by wrapping the inner
        // type, and keeping the function which created it around as well
        // for serialisation purposes
        ostr << "\n"
            << m_name << "ConstSP " << m_name
            << "::Wrap(\n"
            << "    const inner_type& inner,\n"
            << "    const spi::FunctionConstSP& func)\n"
            << "{\n"
            << "    return " << m_name << "ConstSP(\n"
            << "        new " << m_name << "(inner, func));\n"
             << "}\n";

        ostr << "\n"
            << m_name << "::" << m_name << "(\n"
            << "    const inner_type& inner,\n"
            << "    const spi::FunctionConstSP& func)\n"
            << "{\n"
            << "    set_inner(" << m_innerClass->ToSharedPointer("inner") << ");\n"
            << "    set_constructor(func);\n"
            << "}\n";

        if (m_innerClass->m_allowConst)
        {
            SPI_PRE_CONDITION(!m_innerClass->m_byValue); // hence we can skip the boolTest stuff
            ostr << "\n"
                << m_name << "ConstSP " << m_name
                << "::Wrap(\n"
                << "    const const_inner_type& const_inner,\n"
                << "    const spi::FunctionConstSP& func)\n"
                << "{\n"
                << "    if (!const_inner)\n"
                << "        return " << m_name << "ConstSP(); \n";

            // call the copy constructor and wrap as non-const
            if (m_innerClass->m_isShared)
            {
                ostr << "    inner_type inner(new " << m_innerClass->m_name << "(*const_inner));\n";
            }
            else
            {
                ostr << "    inner_type inner = new " << m_innerClass->m_name << "(*const_inner);\n";
            }
            ostr << "    return Wrap(inner, func);\n"
                << "}\n";
        }
    }

    if (m_innerClass->m_isOpen && !isAbstract())
    {
        ostr << "\n"
            << m_name << "::" << m_name << "(const inner_type& inner)\n";

        if (m_noId)
            ostr << "    : spi::Object(true)\n";

        ostr << "{\n"
            << "    set_inner(" << m_innerClass->ToSharedPointer("inner") << ");\n"
            << "}\n";
    }

    for (size_t i = 0; i < m_coerceFromVector.size(); ++i)
        m_coerceFromVector[i]->implement(ostr, m_name, svc);

    for (size_t i = 0; i < m_coerceToVector.size(); ++i)
        m_coerceToVector[i]->implement(ostr, m_name);

    // innerClass->fullTypeName includes const which we don't want here
    std::string innerClassName = m_innerClass->m_ns + "::" + m_innerClass->m_typeName;
    for (size_t i = 0; i < m_methods.size(); ++i)
        m_methods[i]->implement(ostr, m_dataType, m_name, innerClassName, types, svc);

    for (size_t i = 0; i < m_classAttributes.size(); ++i)
    {
        if (m_innerClass->m_isOpen)
        {
            writeOpenAccessor(ostr, m_classAttributes[i], m_name, false, !!m_verbatim, false, accessorFormat());
            writeOpenAccessor(ostr, m_classAttributes[i], m_name, true, false, false, accessorFormat());
        }
        else
        {
            writeClosedAccessor(ostr, m_classAttributes[i], m_name);
        }
    }
    for (size_t i = 0; i < m_classProperties.size(); ++i)
    {
        writeOpenAccessor(ostr, m_classProperties[i], m_name, false, !!m_verbatim, false, propertyFormat());
        writeOpenAccessor(ostr, m_classProperties[i], m_name, true, false, false, propertyFormat());
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

    if (m_isDelegate)
    {
        // we need to define the Delegate class before the function which
        // returns an instance of it is implemented
        //
        // note that the concept only works if the inner_type is always
        // returned as a shared pointer, since if we keep the delegate within
        // the wrapper class we get a circular reference
        //
        // thus whenever the delegate is requested, we create a new one from
        // scratch and return as a shared pointer - thus its ownership can be
        // safely managed as it goes into the wild - as long as the delegate
        // exists the original wrapper class instance will not get deleted


        // if you don't put in the full typename the compiler might get
        // confused if the inner typename is the same (upto namespace)
        // as the outer typename
        std::ostringstream oss;
        oss << svc->getNamespace() << "::";
        if (!m_ns.empty())
            oss << m_ns << "::";
        oss << m_name << "ConstSP";
        std::string fullTypename = oss.str();

        ostr << "\n"
             << "class " << m_name << "_Delegate"
             << " : public " << m_innerClass->m_name << "\n"
             << "{\n"
             << "public:\n"
             << "    // constructor from outer class\n"
             << "    " << m_name << "_Delegate(const " << fullTypename << "& self)\n"
             << "        : self(self)\n"
             << "    {}\n";

        for (size_t i = 0; i < m_delegatePrototypes.size(); ++i)
        {
            ostr << "\n"
                 << "    " << spi::StringStrip(m_delegatePrototypes[i]) << "\n"
                 << "    {";
            writeVerbatim(ostr, m_delegateImplementations[i], 4);
        }

        // new approach - mark regular methods as delegates
        // then we declare everything using inner types rather than outer types
        //
        // the implementation translates from inner types to outer types, makes the
        // function call and converts the outer type returned back to an inner type
        for (size_t i = 0; i < m_methods.size(); ++i)
        {
            ClassMethodConstSP method = m_methods[i];
            if (!method->isDelegate())
                continue;

            method->implementAsDelegate(ostr, this, svc);
        }

        //ostr << "\n"
        //     << "    ~" << name << "_Delegate()\n"
        //     << "    {\n"
        //     << "        int wait = 1;\n"
        //     << "    }\n";

        ostr << "\n"
             << "private:\n"
             << "    // shared pointer to outer class\n"
             << "    " << fullTypename << " self;\n"
             << "};\n";

        // note that inner_type must be a shared pointer
        // this is validated by WrapperClass::VerifyAndComplete
        ostr << "\n"
             << m_name << "::inner_type " << m_name << "::get_delegate() const\n"
             << "{\n"
             << "    return inner_type(new " << m_name
             << "_Delegate(outer_type(this)));\n"
             << "}\n";
    }
}

void WrapperClass::implementHelper(
    GeneratedOutput& ostr,
    const ServiceDefinitionSP& svc,
    bool types) const
{
    writeStartCommentBlock(ostr);
    ostr << "* Implementation of " << m_name << "\n";
    writeEndCommentBlock(ostr);

    bool hasCoerceFrom = CoerceFrom::implement(ostr, m_name, svc, m_coerceFromVector);
    CoerceTo::implement(ostr, m_name, m_coerceToVector);

    std::string coerce_from_value;
    const std::vector<ClassAttributeConstSP>& allClassAttributes = AllClassAttributes();

    if (hasCoerceFrom)
    {
        coerce_from_value = m_name + "::coerce_from_value";
    }
    else
    {
        coerce_from_value = "0";
    }

    if (isAbstract())
    {
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
             << ObjectName() << "\", "
             << svc->getName() << "_service, "
             << (m_canPut ? "true" : "false") << ", "
             << coerce_from_value << ");\n";

        ostr << "\n"
             << "std::vector<" << m_name << "::sub_class_wrapper*> "
             << m_name << "::g_sub_class_wrappers;\n";

        // FIXME: abstract classes could have properties
        // but in that case the to_map method of the sub-class will
        // show them
    }
    else
    {
        ostr << "\n"
             << "void " << m_name << "::to_map(\n"
             << "    spi::IObjectMap* obj_map, bool public_only) const\n"
             << "{\n";

        if (!m_innerClass->m_isOpen)
        {
            ostr << "    if (has_constructor())\n"
                 << "    {\n"
                 << "        constructor_to_map(obj_map);\n"
                 << "        return;\n"
                 << "    }\n";

            // we need to declare properties - we can avoid duplicates using
            // done, but there is a slight risk that base class properties
            // will match class variables - I don't think this is all that
            // serious a problem in fact - it just means that the local
            // variable will trump the class variable and as long as it is
            // logically correct you won't go far wrong
            std::set<std::string> done;
            writeGetClassAttributesForMap(ostr, getBaseClassProperties(), done, 4);
            writeGetClassAttributesForMap(ostr, allClassAttributes, done, 4, true);
            writeGetClassAttributesForMap(ostr, m_classProperties, done, 4);
        }
        else
        {
            // use done to avoid duplicates
            // NOTE: we still add them twice (in case of duplicates) but we
            // won't fetch or declare them twice!
            // NOTE: duplicates can arise if the base class has a property
            // which is an attribute of the concrete class
            std::set<std::string> done;
            writeGetClassAttributesForMap(ostr, getBaseClassProperties(), done, 4);
            writeGetClassAttributesForMap(ostr, allClassAttributes, done, 4);
            writeGetClassAttributesForMap(ostr, m_classProperties, done, 4);
        }
        // FIXME: validate there are no clashing names between baseClassProperties
        // and attributes/properties defined for this class
        writePropertiesToMap(ostr, getBaseClassProperties(), "obj_map", "%s", 4);
        writeToMap(ostr, allClassAttributes, "obj_map", "%s", 4);
        writePropertiesToMap(ostr, m_classProperties, "obj_map", "%s", 4,
            !!m_dynamicPropertiesCode );
        ostr << "}\n";

        ostr << "\n"
             << "spi::ObjectConstSP " << m_name << "::object_from_map(\n"
             << "    spi::IObjectMap* obj_map,\n"
             << "    spi::ValueToObject& value_to_object)\n"
             << "{\n";

        if (!m_innerClass->m_isOpen)
        {
            ostr << "    {\n"
                 << "        spi::ObjectConstSP obj = spi::Object::constructor_from_map(\n"
                 << "            obj_map, value_to_object);\n"
                 << "        if (obj)\n"
                 << "            return obj;\n"
                 << "    }\n";
        }
        if (!hasConstructor())
        {
            ostr << "    throw spi::RuntimeError("
                 << "\"Cannot create object from map without constructor\");\n";
        }
        else
        {
            std::string constructor = spi::StringFormat("%s::Make", m_name.c_str());
            writeFromMap(ostr, allClassAttributes, constructor, "obj_map", "value_to_object", 4);
        }
        ostr << "}\n";

        ostr << "\n"
             << "SPI_IMPLEMENT_OBJECT_TYPE(" << m_name << ", \""
             << makeNamespaceSep(m_ns, ".") << ObjectName() << "\", "
             << svc->getName() << "_service, "
             << (m_canPut ? "true" : "false") << ", "
             << coerce_from_value << ");\n";

        writeObjectMethodImplementation(ostr, m_name);

        if (!m_noMake)
        {
            writeFunctionCaller(ostr, m_ns, m_name, std::string(),
                m_dataType, 0, DataTypeConstSP(), svc, AllAttributes());
        }
    }

    for (size_t i = 0; i < m_methods.size(); ++i)
        m_methods[i]->implementHelper(ostr, m_dataType, m_name, types, svc);

    if (m_innerClass->m_isCached && !m_isVirtual)
    {
        ostr << "\n"
             << "spi::ObjectWrapperCacheSP " << m_name << "_Helper::cache("
             << "new spi::ObjectWrapperCache(\"" << m_name << "\"));\n";
    }
}

void WrapperClass::implementRegistration(
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

    if (m_baseClass && m_innerClass->m_isOpen)
    {
        ostr << "    " << m_baseClass->m_name << "::g_sub_class_wrappers.push_back("
             << m_name << "::BaseWrap);\n";
    }
}

const char* WrapperClass::type() const
{
    return "WRAPPER_CLASS";
}

spdoc::ConstructConstSP WrapperClass::getDoc() const
{
    if (!m_doc)
    {
        std::vector<spdoc::ClassAttributeConstSP> attrDocs;
        std::vector<spdoc::ClassAttributeConstSP> propDocs;
        std::vector<spdoc::ClassMethodConstSP> methodDocs;
        std::vector<spdoc::CoerceFromConstSP> coerceFromDocs;
        std::vector<spdoc::CoerceToConstSP> coerceToDocs;

        // as far as the higher level interfaces are concerned
        // abstract base classes cannot have attributes and all the
        // attributes are transferred to the concrete derived classes
        std::vector<ClassAttributeConstSP> allClassAttributes;
        if (!isAbstract())
            allClassAttributes = AllClassAttributes();

        for (size_t i = 0; i < allClassAttributes.size(); ++i)
            attrDocs.push_back(allClassAttributes[i]->getDoc(false, false));
        for (size_t i = 0; i < m_classProperties.size(); ++i)
            propDocs.push_back(m_classProperties[i]->getDoc(false, true));
        for (size_t i = 0; i < m_methods.size(); ++i)
            methodDocs.push_back(m_methods[i]->getDoc());
        for (size_t i = 0; i < m_coerceFromVector.size(); ++i)
            coerceFromDocs.push_back(m_coerceFromVector[i]->getDoc());
        for (size_t i = 0; i < m_coerceToVector.size(); ++i)
            coerceToDocs.push_back(m_coerceToVector[i]->getDoc());

        std::string baseClassName;
        if (m_baseClass)
            baseClassName = m_baseClass->getName(true, ".");

        m_doc = spdoc::Class::Make(
            m_name, m_ns, m_description, baseClassName,
            attrDocs, propDocs, methodDocs,
            coerceFromDocs, coerceToDocs, isAbstract(),
            m_noMake || isAbstract(), m_objectName,
            !m_dataType ? spdoc::DataTypeConstSP() : m_dataType->getDoc(),
            isDelegate(), m_canPut,
            !!m_dynamicPropertiesCode,
            m_asValue, m_constructor);
    }
    return m_doc;
}

const DataTypeConstSP& WrapperClass::getDataType(
    const ServiceDefinitionSP& svc, bool ignored) const
{
    if (!m_dataType)
    {
        std::string dataTypeName = m_ns.empty() ? m_name : m_ns + "." + m_name;

        if (svc->getDataType(m_name) && !svc->isIncompleteType(m_name))
        {
            SPI_THROW_RUNTIME_ERROR("DataType " << dataTypeName << " is already defined");
        }
        std::string cppName = m_ns.empty() ?
            m_name :
            m_ns + "::" + m_name;

        // this might be a problem when comparing the data types
        std::string outerTypeName = hasNonConstMethods() ?
            cppName + "SP" :
            cppName + "ConstSP";

        std::string outerType = outerTypeName + "&";

        std::string convertIn;
        std::string convertOut;
        std::string copyInner;

        convertIn  = m_name + "::get_inner(%s)"; // it uses intrusive_ptr
        if (m_innerClass->m_isShared)
        {
            if (m_innerClass->m_isOpen)
            {
                convertOut = m_name + "::Wrap(%s)";
            }
            else
            {
                // should only be called from within a functor
                convertOut = m_name + "::Wrap(%s, _constructor)";
            }
        }
        else if (m_innerClass->m_byValue)
        {
            if (m_innerClass->m_isOpen)
            {
                convertOut = m_name + "::Wrap(%s)";
            }
        }
        else
        {
            if (m_innerClass->m_isOpen)
            {
                convertOut = m_name + "::Wrap(%s)";
                if (m_innerClass->m_copyFunc.empty())
                {
                    if (m_innerClass->m_freeFunc.empty())
                    {
                        // this version SafeCopy uses the copy constructor
                        // however when an explicit free function exists this is
                        // probably inappropriate
                        copyInner = "SafeCopy(%s)";
                    }
                    else
                    {
                        // this won't compile and may cause some confusion
                        // however we don't enforce the rule that
                        //     freeFunc => copyFunc needed
                        // in case the copyFunc is never actually required
                        copyInner = "PLEASE_PROVIDE_COPY_FUNC_FOR_" + m_innerClass->m_typeName + "(%s)";
                    }
                }
                else
                {
                    copyInner = "SafeCopy(%s, " + m_innerClass->m_copyFunc + ")";
                }
            }
            else if (m_noMake)
            {
                // should only be called from within a functor
                convertOut = m_name + "::Wrap(%s, _constructor)";
            }
        }
        if (!m_ns.empty())
            convertOut = m_ns + "::" + convertOut;

        m_dataType = DataType::Make(
            m_name, m_ns, svc->getNamespace(), cppName, outerType,
            m_innerClass->getCppType(), m_innerClass->getCppRefType(),
            spdoc::PublicType::CLASS, m_objectName,
            !m_innerClass->m_isOpen,
            false,
            InputConverter::Class(convertIn),
            convertOut,
            copyInner,
            DataTypeConstSP(),
            false,
            m_asValue,
            ignored);

        m_dataType = svc->addDataType(m_dataType, m_incomplete);

        if (!m_incomplete)
        {
            DataTypeConstSP publicDataType = DataType::Make(
                m_name, m_ns, svc->getNamespace(), cppName, outerType,
                std::string(), std::string(),
                spdoc::PublicType::CLASS, m_objectName, false, false,
                std::string(), std::string(), std::string(), DataTypeConstSP(), false,
                m_asValue, ignored);
            publicDataType->setDoc(m_dataType->getDoc());
            svc->addPublicDataType(publicDataType);
        }
    }
    return m_dataType;
}

std::string WrapperClass::getName(bool includeNamespace, const char* sep) const
{
    if (!includeNamespace || m_ns.empty())
        return m_name;

    return m_ns + sep + m_name;
}


std::string WrapperClass::getObjectName() const
{
    if (m_objectName.length() > 0)
        return m_objectName;

    return getName(true, ".");
}

bool WrapperClass::isAbstract() const
{
    return m_isVirtual || m_hasVirtualMethods;
}

bool WrapperClass::isWrapperClass() const
{
    return true;
}

bool WrapperClass::isDelegate() const
{
    return m_isDelegate;
}

bool WrapperClass::isVirtualMethod(const std::string& methodName)
    const
{
    for (size_t i = 0; i < m_methods.size(); ++i)
    {
        const ClassMethodConstSP& method = m_methods[i];
        if (methodName == method->function()->name())
        {
            if (m_isDelegate)
                return true;
            if (method->isVirtual())
                return true;
        }
    }
    return false;
}

std::vector<CoerceFromConstSP> WrapperClass::getCoerceFrom() const
{
    return m_coerceFromVector;
}

bool WrapperClass::byValue() const
{
    return false;
}

std::string WrapperClass::ObjectName() const
{
    if (m_objectName.empty())
        return m_name;
    return m_objectName;
}

bool WrapperClass::hasConstructor() const
{
    if (isAbstract())
        return false;

    if (m_noMake && !m_innerClass->m_isOpen && m_attributes.size() == 0)
        return false;

    return true;
}

void WrapperClass::setVerbatimConstructor(const VerbatimConstSP& verbatimConstructor)
{
    m_verbatimConstructor = verbatimConstructor;
}

void WrapperClass::setDataType(const DataTypeConstSP& dataType)
{
    m_dataType = dataType;
}

const char* WrapperClass::accessorFormat() const
{
    if (m_accessorFormat.empty())
        return NULL;

    return m_accessorFormat.c_str();
}

const char* WrapperClass::propertyFormat() const
{
    if (!m_propertyFormat.empty())
        return m_propertyFormat.c_str();

    return accessorFormat();
}

void WrapperClass::VerifyAndComplete()
{
    m_hasVirtualMethods = false;
    if (m_baseClass)
    {
        SPI_PRE_CONDITION(m_baseClass->isAbstract());
        if (m_baseClass->m_innerClass->m_isOpen)
        {
            // if the base class is open so must the sub-class be open
            // however we will allow the sub-class to be open even if the base class is not
            SPI_PRE_CONDITION(m_innerClass->m_isOpen);
        }
        SPI_PRE_CONDITION(m_innerClass->m_isShared == m_baseClass->m_innerClass->m_isShared);
        SPI_PRE_CONDITION(m_innerClass->m_isConst == m_baseClass->m_innerClass->m_isConst);

        if (m_noId)
            SPI_THROW_RUNTIME_ERROR("Cannot set noId for a derived class");
    }

    if (m_isDelegate)
    {
        SPI_PRE_CONDITION(m_innerClass->m_isShared);
    }
}

ClassConstSP WrapperClass::getBaseClass() const
{
    return m_baseClass;
}

std::vector<ClassAttributeConstSP> WrapperClass::getBaseClassProperties() const
{
    std::vector<ClassAttributeConstSP> properties;
    if (!m_baseClass)
        return properties;

    properties = m_baseClass->getBaseClassProperties();
    properties.insert(properties.end(), m_baseClass->m_classProperties.begin(),
        m_baseClass->m_classProperties.end());

    return properties;
}

std::vector<AttributeConstSP> WrapperClass::AllAttributes() const
{
    std::vector<AttributeConstSP> all;
    if (m_baseClass)
        all = m_baseClass->AllAttributes();
    all.insert(all.end(), m_attributes.begin(), m_attributes.end());
    return all;
}

std::vector<ClassAttributeConstSP> WrapperClass::AllClassAttributes() const
{
    if (!m_baseClass)
        return m_classAttributes;

    std::vector<ClassAttributeConstSP> all = m_baseClass->AllClassAttributes();
    all.insert(all.end(), m_classAttributes.begin(), m_classAttributes.end());

    return all;
}

