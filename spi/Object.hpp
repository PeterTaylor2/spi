/*

    Sartorial Programming Interface (SPI) runtime libraries
    Copyright (C) 2012-2023 Sartorial Programming Ltd.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA

*/
/*
***************************************************************************
** Object.hpp
***************************************************************************
** Base Object class.
**
** The base class supports the concepts of reflection and serialisation.
***************************************************************************
*/

#ifndef SPI_OBJECT_HPP
#define SPI_OBJECT_HPP

#include "Declare.hpp"
#include "RefCounter.hpp"
#include "RuntimeError.hpp"
#include "DateTime.hpp"
#include <map>
#include <vector>
#include <string.h>

SPI_BEGIN_NAMESPACE

SPI_DECLARE_RC_CLASS(Map);
SPI_DECLARE_RC_CLASS(Object);
SPI_DECLARE_RC_CLASS(Functor);
SPI_DECLARE_RC_CLASS(Function);
SPI_DECLARE_RC_CLASS(IObjectWriter);
SPI_DECLARE_RC_CLASS(IObjectReader);
SPI_DECLARE_RC_CLASS(IObjectSink);
SPI_DECLARE_RC_CLASS(IObjectSource);
SPI_DECLARE_CLASS(ObjectType);
SPI_DECLARE_RC_CLASS(ObjectWrapperCache);

class IObjectMap;
class ValueToObject;
class Value;
class InputContext;
class Service;

// we will need to implement a special version of intrusive_ptr in order to
// support automatic type coercion within C++
//
// sub-classes of Object will use ObjectSmartPtr instead of intrusive_ptr
// for the shared pointer implementation
//
// TBD: implement ObjectSmartPtr

class SPI_IMPORT ObjectType
{
public:
    typedef ObjectConstSP (MakeFromMap)(IObjectMap* m, ValueToObject& m2o);
    typedef bool (IsInstance)(const ObjectConstSP& o);
    typedef ObjectConstSP (CoerceFromObject)(const ObjectConstSP&);
    typedef ObjectConstSP (CoerceFromValue)(const Value&, const InputContext*);

    // Used to register all the object types in a library.
    // Potentially two libraries can work together by sharing
    // a registry, or you can have two totally independent
    // libraries with separate registries.
    class SPI_IMPORT Registry
    {
    public:
        // initialisation will register all the system object types
        Registry();
        void import_types(const Registry& other);
        void add_type(ObjectType* type, bool overwrite=false);
        ObjectType* get_type(const char* className) const;
        ObjectType* get_type(const std::string& className) const;
        size_t size() const;
        void clear();
    private:
        std::map<std::string, ObjectType*> m_dict;
        // not implemented - prevents compiler construction
        Registry(const Registry&);
        Registry& operator=(const Registry&);
    };

    ObjectType(
        const char*       className,
        MakeFromMap*      makeFromMap,
        IsInstance*       isInstance,
        bool              canPut,
        CoerceFromObject* coerceFromObject=0,
        CoerceFromValue*  coerceFromValue=0);

    const char* get_class_name() const;
    ObjectConstSP make_from_map(IObjectMap* m, ValueToObject& m2o,
        const MapConstSP& metaData=MapConstSP()) const;
    bool is_instance(const ObjectConstSP& o) const;
    bool can_put() const;
    ObjectConstSP coerce_from_value(const Value& value, const InputContext* context=0) const;
    ObjectConstSP coerce_from_object(const ObjectConstSP& o) const;
    void set_service(const Service* svc);
    const Service* get_service() const;

private:
    // data section
    std::string       m_className;
    MakeFromMap*      m_makeFromMap;
    IsInstance*       m_isInstance;
    bool              m_canPut;
    CoerceFromObject* m_coerceFromObject;
    CoerceFromValue*  m_coerceFromValue;
    const Service*    m_service;
};

/**
 * Object is the base class of all object classes in the system.
 *
 * Note that all public methods of Object are const
 *
 * However this does not mean that a sub-class of Object needs to have all
 * methods as const.
 *
 * We will always use ObjectConstSP and will need to be able to support
 * coercion from ObjectConstSP to a sub-class which is either const or not.
 */
class SPI_IMPORT Object : public RefCounter
{
public:
    typedef ObjectConstSP outer_type;
    // we will use the convention that system methods include underscore
    // in the name - hence application methods on sub-classes should not
    // include underscores
    virtual void to_map(IObjectMap* objectMap, bool publicOnly) const = 0;
    virtual ObjectType* get_object_type() const = 0;
    virtual Value as_value() const;

    const char* get_class_name() const;
    // gets a map of public attributes
    const MapConstSP& get_public_map() const;
    // gets a public value
    Value get_value(const std::string& name) const;
    // gets a map of all attributes (private or public)
    MapConstSP get_attributes() const;
    int get_id() const;

    virtual ObjectConstSP coerce_to_object(const std::string& className) const;

    // writes to a stream in the given format
    // returns whether the stream was written in binary format
    bool to_stream(std::ostream& ostr,
        const char* format = "",
        const char* options = "",
        const MapConstSP& metaData = MapConstSP(),
        bool addObjectId = false,
        bool mergeMetaData = false) const;

    // converts object to string for the given format
    // fails if the format is a binary format
    std::string to_string(
        const char* format = "",
        const char* options = "",
        const MapConstSP& metaData = MapConstSP(),
        bool mergeMetaData = false) const;

    // writes object to file for the given format
    // for binary formats, this is done in binary format without a trailing new line
    // for text formats, this is done in text format with a trailing new line
    void to_file(const char* filename,
        const char* format = "",
        const char* options = "",
        const MapConstSP& metaData = MapConstSP(),
        bool mergeMetaData = false) const;

    // these two currently needed for .NET only
    // delete if possible
    static Value to_value(const ObjectConstSP& o);
    Value to_value() const;
    static Value to_value(const std::vector<ObjectConstSP>&);

    // all classes are expected to define
    // static ObjectType object_type;

    ~Object();

    ObjectConstSP service_coercion(const Service* svc) const;
    const Service* get_service() const;
    virtual void* get_py_object() const;

    const MapSP& get_meta_data() const;
    void update_meta_data(const MapConstSP& meta_data) const;
    const std::string& get_object_id() const;
    DateTime get_timestamp() const; // derived from object_id

    static bool g_add_object_id_file;
    static bool g_add_object_id_string;

    static void set_tracking(bool tracking);
    static size_t tracked_object_count();
    static std::map<std::string, size_t> tracked_object_by_class_name();

protected:
    Object(bool no_id=false);
    // sets the object_id - should only be called as part of the construction
    // of a sub-class of Object - if the object_id is empty then one will be provided
    // if you ask for object_id
    void set_object_id(const std::string& object_id) const;

    friend class ObjectType;

    void clear_public_map() const; // to be called by any non-const method
    void set_constructor(const FunctionConstSP& constructor) const;
    bool has_constructor() const;
    void constructor_to_map(IObjectMap* objectMap) const;
    static ObjectConstSP constructor_from_map(
        IObjectMap*    objectMap,
        ValueToObject& valueToObject);

private:
    int m_id;
    mutable FunctionConstSP m_constructor;
    mutable MapConstSP m_public_map;
    mutable std::map<const Service*, ObjectConstSP> m_serviceCoercion;
    MapSP m_meta_data;
    mutable std::string m_object_id;

    Object(const Object&);
    const Object& operator=(const Object&);
};


/**
 * IsInstance template is simply a dynamic cast check.
 */
template<class T>
bool IsInstance(const ObjectConstSP& o)
{
    if (!o)
        return false;

    const T* p = dynamic_cast<const T*>(o.get());
    if (!p)
        return false;

    return true;
}

/**
 * CoerceObjectVector simply calls T::Coerce for each element
 */
template<class T>
std::vector< typename T::outer_type >
CoerceObjectVector(const std::vector<ObjectConstSP>& inp)
{
    std::vector< typename T::outer_type > out;

    for (size_t i = 0; i < inp.size(); ++i)
        out.push_back(T::Coerce(inp[i]));

    return out;
}

/**
 * MakeObjectVector simply calls the auto-conversion for each element
 */
template<class T>
std::vector<ObjectConstSP>
MakeObjectVector(const std::vector< typename T::outer_type >& inp)
{
    std::vector< ObjectConstSP > out;

    for (size_t i = 0; i < inp.size(); ++i)
        out.push_back(ObjectConstSP(inp[i]));

    return out;
}

SPI_END_NAMESPACE

template<class T>
std::ostream& operator<< (
    std::ostream& ostr,
    const typename T::outer_type& obj)
{
    obj->to_stream(ostr);
    return ostr;
}

#endif

