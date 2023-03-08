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
#ifndef INNER_CLASS_HPP
#define INNER_CLASS_HPP

/*
***************************************************************************
** innerClass.hpp
***************************************************************************
** Defines the InnerClass class used in the configuration parser.
***************************************************************************
*/

#include <vector>

#include <spi/Date.hpp>
#include <spi/Service.hpp>

SPI_DECLARE_RC_CLASS(CppType);
SPI_DECLARE_RC_CLASS(InnerClass);
SPI_DECLARE_RC_CLASS(InnerClassTemplate);
SPI_DECLARE_RC_CLASS(WrapperClass);
SPI_DECLARE_RC_CLASS(ServiceDefinition);

#include <spgtools/generatedOutput.hpp>
#include <spgtools/namespaceManager.hpp>

/**
 * Defines an inner class.
 */
class InnerClass : public spi::RefCounter
{
public:
    friend class WrapperClass;

    /**
     * Constructor for an inner class.
     *
     * @param typeName
     *    Raw type name. Could be a class or typedef struct.
     * @param ns
     *    Does this type exist within a particular namespace.
     * @param freeFunc
     *    Free function if provided (generally for typedef struct).
     *    By default we assume that we can use delete.
     * @param shared
     *    Does the inner library expect shared pointers when we pass
     *    values into the shared library.
     * @param open
     *    Is the class open or closed. By this we mean can we access the
     *    attributes of the class once it is created or not.
     *
     *    If we cannot access the attributes of the class, then this
     *    creates certain constraints on what we can do at the wrapper
     *    class level.
     */
    static InnerClassConstSP Make(
        const std::string& typeName,
        const std::string& ns,
        const std::string& freeFunc,
        const std::string& copyFunc,
        const std::string& preDeclaration,
        const std::string& sharedPtr,
        bool               isShared,
        bool               isConst,
        bool               isOpen,
        bool               isStruct,
        bool               isCached,
        bool               isTemplate,
        bool               byValue,
        const std::string& boolTest,
        bool               allowConst);

    /**
     * write the pre-declaration statements for this class
     */
    void preDeclare(GeneratedOutput& ostr,
                    NamespaceManager& nsman) const;

    const std::string& getCppType() const;
    const std::string& getCppRefType() const;
    std::string ToSharedPointer(const std::string& inner) const;
    InnerClassConstSP setSharedPtr(const std::string& sharedPtr) const;

protected:
    InnerClass(
        const std::string& typeName,
        const std::string& ns,
        const std::string& freeFunc,
        const std::string& copyFunc,
        const std::string& preDeclaration,
        const std::string& sharedPtr,
        bool               isShared,
        bool               isConst,
        bool               isOpen,
        bool               isStruct,
        bool               isCached,
        bool               isTemplate,
        bool               byValue,
        const std::string& boolTest,
        bool               allowConst);

private:
    std::string m_typeName;
    std::string m_ns;
    std::string m_freeFunc;
    std::string m_copyFunc;
    std::string m_preDeclaration;
    std::string m_sharedPtr; // this is the shared pointer template
    bool        m_isShared;
    bool        m_isConst;
    bool        m_isOpen;
    bool        m_isStruct;
    bool        m_isCached;
    bool        m_isTemplate;
    bool        m_byValue;
    std::string m_boolTest;
    bool        m_allowConst;

    // these are the actual inner types returned by get_inner
    std::string m_cppType;
    std::string m_cppRefType;

    // these are the actual inner types returned by get_const_inner
    std::string m_constCppType;
    std::string m_constCppRefType;

    std::string m_name;
    std::string m_fullTypeName;
    std::string m_fullTypeNameConst;
    std::string m_sharedPointer; // this is the actual instantiation of the template
    std::string m_constSharedPointer;
    std::string m_rawPointer;
    std::string m_constRawPointer;
    std::string m_spDynamicCast;

    void VerifyAndComplete();

public:
    const std::string& typeName() const { return m_typeName; }
    const std::string& ns() const { return m_ns; }
    const std::string& freeFunc() const{ return m_freeFunc; }
    const std::string& copyFunc() const { return m_copyFunc; }
    const std::string& preDeclaration() const { return m_preDeclaration; }
    const std::string& sharedPtr() const { return m_sharedPtr; }
    bool isShared() const { return m_isShared; }
    bool isConst() const { return m_isConst; }
    bool isOpen() const { return m_isOpen; }
    bool isStruct() const { return m_isStruct; }
    bool isCached() const { return m_isCached; }
    bool isTemplate() const { return m_isTemplate; }
    bool byValue() const { return m_byValue; }
    const std::string& boolTest() const { return m_boolTest; }
    bool allowConst() const { return m_allowConst; }

    const std::string& name() const { return m_name; }
};

/**
 * Defines an inner class template.
 */
class InnerClassTemplate : public spi::RefCounter
{
public:
    friend class WrapperClass;

    /**
     * Constructor for an inner class template.
     */
    static InnerClassTemplateConstSP Make(
        const std::string& name,
        const std::string& ns,
        const std::string& preDeclaration,
        const std::string& boolTest);

    /**
     * write the pre-declaration statements for the template class
     */
    void preDeclare(GeneratedOutput& ostr,
                    NamespaceManager& nsman) const;

    InnerClassConstSP MakeInnerClass(
        const std::string& T,
        const std::string& sharedPtr,
        bool               isShared,
        bool               isConst,
        bool               isOpen,
        bool               isStruct,
        bool               isCached,
        bool               byValue,
        bool               allowConst) const;

protected:
    InnerClassTemplate(
        const std::string& name,
        const std::string& ns,
        const std::string& preDeclaration,
        const std::string& boolTest);

private:
    std::string m_name;
    std::string m_ns;
    std::string m_preDeclaration;
    std::string m_boolTest;
    std::string m_fullName;

    void VerifyAndComplete();

public:

    const std::string& fullName() const { return m_fullName; }
};



#endif
