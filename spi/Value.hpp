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
** Value.hpp
***************************************************************************
** Polymorphic class with value semantics and arbitrary type.
**
** Values go into Maps and hence the items must either be fundamental types
** or (essentially) maps of fundamental types.
**
** Our fundamental types will be as follows:
**
**    int
**    double
**    bool
**    Date
**    String
**
** The following complex types are also supported:
**
**    Map      - name/value pairs where the value is itself of type Value
**    Object   - generic class where the items can be converted into
**               an instance of Map
**    Array    - array of value of any dimensionality - supports the
**               interface that we can extract the array dimensions as well
**               as an accessor
**
** These classes are all defined by an interface defined within this file.
** Concrete implementations of these classes are defined in their own files.
***************************************************************************
*/

#ifndef SPI_VALUE_HPP
#define SPI_VALUE_HPP

#include <vector>

#include "DeclSpec.h"
#include "RefCounter.hpp"
#include "Date.hpp"
#include "DateTime.hpp"
#include "ObjectRef.hpp"
#include "RuntimeError.hpp"
#include "Object.hpp"
#include "Map.hpp"
#include "ObjectSmartPtr.hpp"

SPI_BEGIN_NAMESPACE

template<typename T> class MatrixData;
class Value;

SPI_DECLARE_RC_CLASS(IString);
SPI_DECLARE_RC_CLASS(Map);
SPI_DECLARE_RC_CLASS(Object);
SPI_DECLARE_RC_CLASS(IArray);

class SPI_IMPORT IString : public RefCounter
{
public:
    virtual const char* str() const = 0;
};

class SPI_IMPORT Value
{
public:
    enum Type
    {
        UNDEFINED,
        CHAR,
        SHORT_STRING,
        STRING,
        INT,
        DOUBLE,
        BOOL,
        DATE,
        DATETIME,
        MAP,
        OBJECT,
        OBJECT_REF,
        ARRAY,
        ERROR
    };

    static Type StringToType(const char* str);
    static const char* TypeToString(Type valueType);

private:
    /*
     * Note that anything within a union has to be plain old data.
     * This means we can use raw pointers and manage the memory
     * independently, but we cannot use any form of shared pointers.
     *
     * Of course within C++ we don't need to give the union a name.
     */
    union
    {
        char          aChar;
        char          aShortString[sizeof(double)];
        IString*      aString;
        int           anInt;
        double        aDouble;
        bool          aBool;
        int           aDate;
        int           aDateTime[2];
        const Map*    aMap;
        const Object* anObject;
        int           anObjectRef;
        const IArray* anArray;
        IString*      anError;
    };
    Type type;

public:
    Type getType() const;

    // we can construct Value in many ways
    //
    // these should not be defined as explicit since we want to make it easy
    // to use the fundamental types within Value

    Value ();
    Value (char value);
    Value (const char *value);
    Value (const std::string &value);
    Value (int value);
    Value (double value);
    Value (bool value);
    Value (const Date &value);
    Value (const DateTime& value);
    Value (const MapConstSP &value);
    Value (const ObjectConstSP &value);
    Value (const ObjectRef &value);
    Value (const IArrayConstSP &value);
    Value (const std::exception &e);
    Value (const char* value, bool isError);
    // Value (Type type);

    template<class T> 
    Value(const ObjectSmartPtr<T>& obj) : type(Value::UNDEFINED)
    {
        setObject(obj);
    }

    // this template uses the Array class - hence it gets implemented in Array.hpp
    template<typename T> Value (const std::vector<T> &value);
    Value(const std::vector<Value>& values);
    Value(const std::vector<Value>& values, const std::vector<size_t>& dimensions);

    /* Value semantics needed */
    ~Value ();
    Value (const Value &value);
    Value& operator= (const Value &value);

    bool isUndefined() const;
    /* Get specific value types out of a value */
    /* These functions can be very forgiving if the type is not an exact match */
    char             getChar(bool permissive=false) const;
    std::string      getString(bool permissive=false) const;
    int              getInt(bool permissive=false) const;
    bool             getBool(bool permissive=false) const;
    double           getDouble(bool permissive=false) const;
    Date             getDate(bool permissive=false) const;
    DateTime         getDateTime(bool permissive=false) const;
    MapConstSP       getMap() const;
    ObjectConstSP    getObject() const;
    ObjectRef        getObjectRef() const;
    IArrayConstSP    getArray(bool permissive=false) const;
    std::string      getError() const;

    // std::vector<char>             getCharVector() const;
    std::vector<std::string>      getStringVector(bool permissive=false) const;
    std::vector<double>           getDoubleVector(bool permissive=false) const;
    std::vector<int>              getIntVector(bool permissive=false) const;
    std::vector<bool>             getBoolVector(bool permissive=false) const;
    std::vector<Date>             getDateVector(bool permissive=false) const;
    std::vector<DateTime>         getDateTimeVector(bool permissive=false) const;
    std::vector<MapConstSP>       getMapVector() const;
    std::vector<ObjectConstSP>    getObjectVector() const;
    // this function only works if the Value is a rectangular array
    spi::MatrixData<Value>        getMatrix() const;

    /* Swapping is useful for efficient memory management */
    Value& swap(Value &value);

    std::string toString(const char* indent="") const;

    /**
     * Converts a Value of type array to a scalar. This only works
     * if the Value is undefined (returns Undefined), or size 0 (returns
     * Undefined) or an array of size 1 (returns that value).
     *
     * In all other cases this will throw an exception.
     */
    Value convertArrayToScalar() const;

private:
    void freeContents();
    void setString(const char* value, bool neverShort=false);
    void setObject(const ObjectConstSP& value);

public:
    // unary methods for use within templates
    static char ToChar(const Value &v, bool permissive=false);
    static std::string ToString(const Value &v, bool permissive=false);
    static int ToInt(const Value &v, bool permissive=false);
    static bool ToBool(const Value &v, bool permissive=false);
    static double ToDouble(const Value &v, bool permissive=false);
    static Date ToDate(const Value &v, bool permissive=false);
    static DateTime ToDateTime(const Value &v, bool permissive=false);
    static MapConstSP ToMap(const Value &v);
    static ObjectConstSP ToObject(const Value &v);
    static IArrayConstSP ToArray(const Value &v);

    // cast operators
    operator char() const;
    operator std::string() const;
    operator int() const;
    operator bool() const;
    operator double() const;
    operator Date() const;
    operator DateTime() const;
    operator MapConstSP() const;
    operator ObjectConstSP() const;

    // designed to get around differences between compiler versions
    // when you want to use a Cast operation for Value from a template
    void Cast(char& t) const;
    void Cast(bool& t) const;
    void Cast(std::string& t) const;
    void Cast(int& t) const;
    void Cast(double& t) const;
    void Cast(Date& t) const;
    void Cast(DateTime& t) const;
    void Cast(MapConstSP& t) const;
    void Cast(ObjectConstSP& t) const;
    void Cast(Value& t) const;
    void Translate(char& t, bool permissive) const;
    void Translate(bool& t, bool permissive) const;
    void Translate(std::string& t, bool permissive) const;
    void Translate(int& t, bool permissive) const;
    void Translate(double& t, bool permissive) const;
    void Translate(Date& t, bool permissive) const;
    void Translate(DateTime& t, bool permissive) const;
    void Translate(Value& t, bool permissive) const;

    template<typename T>
    std::vector<T> getVector() const;

    template<typename T>
    std::vector<T> getVector(bool permissive) const;
};

class SPI_IMPORT IArray : public RefCounter
{
public:
    virtual size_t size() const = 0;
    virtual std::vector<size_t> dimensions() const = 0;
    virtual Value::Type elementType() const;
    // in the future we will add a Dimensions class
    //
    // we expect the Array to be implemented internally as a vector (for
    // example), but we can fold it up in a variety of manners to create
    // multi-dimensional arrays
    //
    // in the first implementation it will just be a one-dimensional array
    //
    // there will be a sub-class of IArray which represents the data as
    // a vector of Value, but other sub-classes for specific types could
    // be represented more efficiently
    virtual Value getItem(size_t i) const = 0;
    // operator[] is implemented in terms of getItem
    Value operator[] (size_t i) const;
    // by default getVector is implemented by repeated application of getItem
    virtual std::vector<Value> getVector() const;
    // add to an existing array
    virtual void assign(size_t i, const Value& value) = 0;
    // returns the common element type (non-virtual)
    // returns Value::UNDEFINED if size 0 or non-uniform
    Value::Type commonElementType() const;
};

/**
 * Array of Value.
 */
class SPI_IMPORT ValueArray : public IArray
{
public:
    ValueArray(size_t size);
    ValueArray(const std::vector<size_t>& dimensions);
    ValueArray(const std::vector<Value>& vec, size_t size);
    ValueArray(const std::vector<Value>& vec, const std::vector<size_t>& dimensions);

    // implementation of IArray
    size_t size() const;
    std::vector<size_t> dimensions() const;
    Value getItem(size_t i) const;
    std::vector<Value> getVector() const;
    void assign(size_t i, const Value& value);

private:
    std::vector<Value> m_vec;
    std::vector<size_t> m_dimensions;
};

template<typename T>
Value::Type ValueType()
{
    return Value::UNDEFINED;
}

template<>
inline Value::Type ValueType<int>()
{
    return Value::INT;
}

template<>
inline Value::Type ValueType<char>()
{
    return Value::CHAR;
}

template<>
inline Value::Type ValueType<std::string>()
{
    return Value::STRING;
}

template<>
inline Value::Type ValueType<bool>()
{
    return Value::BOOL;
}

template<>
inline Value::Type ValueType<Date>()
{
    return Value::DATE;
}

template<>
inline Value::Type ValueType<double>()
{
    return Value::DOUBLE;
}

template<>
inline Value::Type ValueType<DateTime>()
{
    return Value::DATETIME;
}

/**
 * Array of a specific type of Value. This only works if the Value can be
 * constructed from the typename T, and can be cast to the typename T.
 *
 * This is true for char, std::string, int, bool, double, Date.
 */
template <typename T>
class Array : public IArray
{
public:
    Array()
        : IArray(),
          m_vec()
    {}

    Array(const std::vector<T>& vec)
        : IArray(),
          m_vec(vec),
          m_dims(1, vec.size())
    {}

    Array(const std::vector<T>& vec,
          const std::vector<size_t>& dims)
        : IArray(),
          m_vec(vec),
          m_dims(dims)
    {
        size_t size = m_vec.size();
        size_t size2 = 1;
        for (size_t i = 0; i < m_dims.size(); ++i)
            size2 *= m_dims[i];

        if (size != size2)
            throw RuntimeError("Mismatch between vector size and dimensions");
    }

    // implementation of IArray
    size_t size() const
    {
        return m_vec.size();
    }

    std::vector<size_t> dimensions() const
    {
        return m_dims;
    }

    Value::Type elementType() const
    {
        Value::Type type = ValueType<T>();
        if (type != Value::UNDEFINED)
            return type;
        return commonElementType();
    }

    Value getItem(size_t i) const
    {
        if (i >= m_vec.size())
            throw RuntimeError("Index %ld out of range", (long)i);
        return m_vec[i];
    }

    void assign(size_t i, const Value& value)
    {
        if (i >= m_vec.size())
            throw RuntimeError("Index %ld out of range", (long)i);

        const T& tv = value;
        m_vec[i] = tv;
    }

    // implementation-specific functions
    // cannot be called getVector since that returns vector<Value>
    const std::vector<T>& getVectorOfType() const
    {
        return m_vec;
    }

private:
    std::vector<T> m_vec;
    std::vector<size_t> m_dims;
    Value::Type m_type;
};

template<typename T>
Value::Value(const std::vector<T> &vec)
{
    type = Value::ARRAY;
    anArray = new Array<T>(vec);
    anArray->incRefCount();
}

template<typename T>
std::vector<T> ConvertValueVector(const std::vector<Value>& in)
{
    std::vector<T> out;
    size_t N = in.size();
    for (size_t i = 0; i < N; ++i)
    {
        T item;
        in[i].Cast(item);
        out.push_back(item);
    }
    return out;
}


template<typename T>
std::vector<T> Value::getVector() const
{
    std::vector<T> result;

    IArrayConstSP anArray = getArray();
    size_t arraySize = anArray->size();
    result.reserve(arraySize);

    for (size_t i = 0; i < arraySize; ++i)
    {
        const Value& item = anArray->getItem(i);
        result.push_back((T)item);
    }
    return result;
}

template<typename T>
std::vector<T> Value::getVector(bool permissive) const
{
    std::vector<T> result;

    IArrayConstSP anArray = getArray();
    size_t arraySize = anArray->size();
    result.reserve(arraySize);

    for (size_t i = 0; i < arraySize; ++i)
    {
        const Value& item = anArray->getItem(i);
        T t;
        item.Translate(t, permissive);
        result.push_back(t);
    }
    return result;
}

SPI_END_NAMESPACE

SPI_IMPORT
std::ostream& operator << (std::ostream& ostr, const spi::Value& value);

#endif

