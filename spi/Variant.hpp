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

#ifndef SPI_VARIANT_HPP
#define SPI_VARIANT_HPP

/*
***************************************************************************
** Variant.hpp
***************************************************************************
** Polymorphic class with value semantics, arbitrary type and late binding.
**
** Value semantics means that Variant can be assigned by value and will
** correctly handle all memory issues.
**
** Arbitrary type means any of the built-in types.
**
** Late binding means that sometimes you only discover the type in the
** context of the interface. For example, when using Excel you don't
** know whether a number should be treated as an int, a double or a date.
** Variant class won't know, but in context the correct type will be
** requested.
***************************************************************************
*/

#include "Value.hpp"
#include "MatrixData.hpp"

SPI_BEGIN_NAMESPACE

class InputContext;
class ValueToObject;

class SPI_IMPORT Variant
{
public:

    // this constructor of Variant is used by the interface layer
    // to create a Variant in the context of the calling application
    Variant (const Value& value,
             const InputContext* context);

    // this constructor of Variant is used when translating outputs
    // essentially there is no input context
    Variant (const Value& value);

    // this constructor of Variant is used when de-serializing
    // some field declared as Variant - which might be a Map
    // of some particular named object
    Variant(
        const Value& value,
        ValueToObject& mapToObject);

    /* Value semantics needed */
    ~Variant ();
    Variant (const Variant &value);
    Variant& operator= (const Variant &value);

    /**
     * Translation between Variant and Map for serialization. Inverse of ToMap method.
     * 
     * Should only be used when the Map has className "Variant"
     */
    Variant(const MapConstSP& m);

    /**
     * We use this to serialize a Variant which has a context to a two-element
     * Map with context and value and className "Variant"
     *
     * Inverse of Variant(const MapConstSP&) constructor.
     */
    MapConstSP ToMap() const;

    /**
     * Data accessors.
     *
     * In general these functions are as forgiving as possible if the
     * type is not an exact match.
     */
    Value::Type      ValueType() const;
    bool             IsUndefined() const;
    char             ToChar(bool optional=false, char defaultValue=' ') const;
    std::string      ToString(bool optional=false, const char* defaultValue="") const;
    int              ToInt(bool optional=false, int defaultValue=0) const;
    bool             ToBool(bool optional=false, bool defaultValue=false) const;
    double           ToDouble(bool optional=false, double defaultValue=0.0) const;
    Date             ToDate(bool optional=false, Date defaultValue=Date()) const;
    DateTime         ToDateTime(bool optional=false, DateTime defaultValue=DateTime()) const;
    ObjectConstSP    ToConstObject(ObjectType* objectType=0, bool optional=false) const;

    // for scalar access we can also use cast operators instead
    // of the To... methods
    operator char() const;
    operator std::string() const;
    operator int() const;
    operator bool() const;
    operator double() const;
    operator Date() const;
    operator ObjectConstSP() const;

    std::vector<std::string>      ToStringVector(bool optional = false, const char* defaultValue = "") const;
    std::vector<double>           ToDoubleVector(bool optional = false, double defaultValue = 0.0) const;
    std::vector<int>              ToIntVector(bool optional = false, int defaultValue = 0) const;
    std::vector<bool>             ToBoolVector(bool optional = false, bool defaultValue = false) const;
    std::vector<Date>             ToDateVector(bool optional = false, Date defaultValue = Date()) const;
    std::vector<DateTime>         ToDateTimeVector(bool optional = false, DateTime defaultValue = DateTime()) const;
    //std::vector<ObjectSP>         ToObjectVector(ObjectType* objectType = 0, bool optional = false) const;
    std::vector<ObjectConstSP>    ToConstObjectVector(ObjectType* objectType = 0, bool optional = false) const;

    /**
     * Converts a Variant of type array to a scalar.
     *
     * If the Variant is not an array, then this returns a copy of the
     * variant.
     *
     * If the Variant is an array of size 0, then this returns Undefined.
     * If the Variant is an array of size 1, then this returns that value.
     * If the Variant is an array of size > 1 then this will throw an
     * exception.
     */
    Variant ToScalar() const;

    // these constructors of Variant are designed for returning
    // outputs from functions when the output is declared as Variant
    Variant ();
    Variant (char value);
    Variant (const char *value);
    Variant (const std::string &value);
    Variant (int value);
    Variant (double value);
    Variant (bool value);
    Variant (const Date &value);
    Variant (const ObjectConstSP &value);

    template<typename T>
    Variant (const std::vector<T> &value);

    // this is used internally to convert Variant to Value on output
    // this function fails if the input context is defined
    operator Value() const;

    const Value& GetValue() const;
    const char* GetContext() const;
    const InputContext* GetInputContext() const;

    static Value VectorToValue(const std::vector<Variant>& in);
    static Value MatrixToValue(const spi::MatrixData<Variant>& in);

private:

    Value               m_value;
    const InputContext* m_context;
};

template<typename T>
Variant::Variant(const std::vector<T> &vec)
:
m_value(vec),
m_context(0)
{}

// note that we no longer encode a matrix via a map, but for backward
// compatibility we need to support this format
template<>
inline MatrixData<Variant> MatrixData<Variant>::FromMap(const MapConstSP& m)
{
    size_t nbRows = m->GetValue("rows").getInt();
    size_t nbCols = m->GetValue("cols").getInt();
    const std::vector<MapConstSP>& mdata = m->GetValue("data").getVector<MapConstSP>();

    std::vector<Variant> data;
    for (size_t i = 0; i < mdata.size(); ++i)
    {
        data.push_back(Variant(mdata[i]));
    }

    return MatrixData<Variant>(nbRows, nbCols, data);
}

// specialisation of MatrixData<Variant> templates
// these are needed since MatrixData<T> assumes that T can be converted to/from Value
// whereas for Variant we need to go via Map to do this conversion
template<>
inline IArrayConstSP MatrixData<Variant>::ToArray() const
{
    std::vector<size_t> dims;
    dims.push_back(Rows());
    dims.push_back(Cols());

    const std::vector<Variant>& data = Data();
    size_t size = data.size();
    std::vector<Value> values;
    values.reserve(size);
    for (size_t i = 0; i < size; ++i)
    {
        const Variant& item = data[i];
        const char* context = item.GetContext();
        if (!*context)
        {
            // NoInputContext
            values.push_back(item.GetValue());
        }
        else
        {
            // need to encode context and value - so we need to encode as Map
            values.push_back(data[i].ToMap());
        }
    }

    return IArrayConstSP(new ValueArray(values, dims));
}

template<>
inline MatrixData<Variant> MatrixData<Variant>::FromValueMatrix(const MatrixData<Value>& vmat)
{
    const std::vector<Value>& values = vmat.Data();

    std::vector<Variant> data;
    for (size_t i = 0; i < values.size(); ++i)
    {
        const Value& value = values[i];
        if (value.getType() == Value::MAP)
            data.push_back(Variant(value.getMap()));
        else
            data.push_back(Variant(value));
    }

    return MatrixData<Variant>(vmat.Rows(), vmat.Cols(), data);
}

SPI_END_NAMESPACE

#endif

