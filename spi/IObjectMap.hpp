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
** IObjectMap.hpp
***************************************************************************
** Defines the map interface for objects.
***************************************************************************
*/

#ifndef SPI_IOBJECT_MAP_HPP
#define SPI_IOBJECT_MAP_HPP

#include "Object.hpp"
#include "Date.hpp"
#include "MatrixData.hpp"
#include "Variant.hpp"
#include "Map.hpp"

#include <string>
#include <vector>

SPI_BEGIN_NAMESPACE

class ValueToObject;
template<typename T> class MatrixData;

// IObjectMap is really two distinct interfaces.
//
// One is a set of functions used when converting an object to a map.
// We need to have methods to set fields inside the map.
//
// The other is a set of functions used when converting a map to an object.
// We need to have methods to get fields from the map.
//
// However if we separate the two interfaces we end up with double the number
// of classes, and in each case it transpires that the input and output classes
// are tightly bound to each other.
class SPI_IMPORT IObjectMap : public RefCounter
{
public:

    // functions used from object to map

    // all the Set... methods have the hidden parameter
    //
    // not all instances of IObjectMap supporting hiding so it is not simply
    // a matter of not calling the method when we want to hide a field
    //
    // for example, maps with named fields generally support hiding, but
    // binary maps will generally encode even the hidden fields
    virtual void SetChar(
        const char* name,
        char value,
        bool hidden = false) = 0;

    virtual void SetString(
        const char* name,
        const std::string& value,
        bool hidden = false) = 0;

    virtual void SetInt(
        const char* name,
        int value,
        bool hidden = false) = 0;

    virtual void SetBool(
        const char* name,
        bool value,
        bool hidden = false) = 0;

    virtual void SetDouble(
        const char* name,
        double value,
        bool hidden = false) = 0;

    virtual void SetDate(
        const char* name,
        Date value,
        bool hidden = false) = 0;

    virtual void SetDateTime(
        const char* name,
        DateTime value,
        bool hidden = false) = 0;

     virtual void SetObject(
        const char* name,
        const ObjectConstSP& value,
        bool hidden = false) = 0;

    virtual void SetVariant(
        const char* name,
        const Variant& value,
        bool hidden = false) = 0;

    virtual void SetStringVector(
        const char* name,
        const std::vector<std::string>& value,
        bool hidden = false) = 0;

    virtual void SetDoubleVector(
        const char* name,
        const std::vector<double>& value,
        bool hidden = false) = 0;

    virtual void SetIntVector(
        const char* name,
        const std::vector<int>& value,
        bool hidden = false) = 0;

    virtual void SetBoolVector(
        const char* name,
        const std::vector<bool>& value,
        bool hidden = false) = 0;

    virtual void SetDateVector(
        const char* name,
        const std::vector<Date>& value,
        bool hidden = false) = 0;

    virtual void SetDateTimeVector(
        const char* name,
        const std::vector<DateTime>& value,
        bool hidden = false) = 0;

    virtual void SetVariantVector(
        const char* name,
        const std::vector<Variant>& value,
        bool hidden = false) = 0;

    virtual void SetObjectVector(
        const char* name,
        const std::vector<ObjectConstSP>& value,
        bool hidden = false) = 0;

    template<class T>
        void SetInstanceVector(
            const char* name,
            const std::vector< typename T::outer_type >& value,
            bool hidden = false)
    {
        std::vector<ObjectConstSP> tmp;
        tmp.reserve(value.size());
        for (size_t i = 0; i < value.size(); ++i)
            tmp.push_back(ObjectConstSP(value[i].get()));
        SetObjectVector(name, tmp, hidden);
    }

    template<class T>
        void SetEnumVector(
            const char* name,
            const std::vector<T>& value,
            bool hidden=false)
    {
        std::vector<std::string> tmp;
        tmp.reserve(value.size());
        for (size_t i = 0; i < value.size(); ++i)
            tmp.push_back(value[i].to_string());
        SetStringVector(name, tmp, hidden);
    }

    virtual void SetBoolMatrix(
        const char* name,
        const MatrixData<bool>& value,
        bool hidden = false) = 0;

    virtual void SetIntMatrix(
        const char* name,
        const MatrixData<int>& value,
        bool hidden = false) = 0;

    virtual void SetDoubleMatrix(
        const char* name,
        const MatrixData<double>& value,
        bool hidden = false) = 0;

    virtual void SetStringMatrix(
        const char* name,
        const MatrixData<std::string>& value,
        bool hidden = false) = 0;

    virtual void SetDateMatrix(
        const char* name,
        const MatrixData<Date>& value,
        bool hidden = false) = 0;

    virtual void SetDateTimeMatrix(
        const char* name,
        const MatrixData<DateTime>& value,
        bool hidden = false) = 0;

    virtual void SetObjectMatrix(
        const char* name,
        const MatrixData<ObjectConstSP>& value,
        bool hidden = false) = 0;

    template<class T>
        void SetInstanceMatrix(
            const char* name,
            const MatrixData< typename T::outer_type >& value,
            bool hidden=false)
    {
        size_t nr = value.Rows();
        size_t nc = value.Cols();
        MatrixData<ObjectConstSP> tmp(nr, nc);
        for (size_t i = 0; i < nr; ++i)
            for (size_t j = 0; j < nc; ++j)
                tmp[i][j] = ObjectConstSP(value[i][j].get());
        SetObjectMatrix(name, tmp, hidden);
    }

    virtual void SetVariantMatrix(
        const char* name,
        const spi::MatrixData<Variant>& value,
        bool hidden = false) = 0;

    virtual void ImportMap(const Map* aMap) = 0;
    virtual void SetClassName(const std::string& classname) = 0;
    virtual std::string ClassName() = 0;

    virtual char GetChar(
        const char* name,
        bool optional = false,
        char defaultValue = '\0') = 0;

    virtual std::string GetString(
        const char* name,
        bool optional = false,
        const char* defaultValue = "") = 0;

    virtual int GetInt(
        const char* name,
        bool optional = false,
        int defaultValue = 0) = 0;

    virtual bool GetBool(
        const char* name,
        bool optional = false,
        bool defaultValue = false) = 0;

    virtual double GetDouble(
        const char* name,
        bool optional = false,
        double defaultValue = 0.0) = 0;

    virtual Date GetDate(
        const char* name,
        bool optional = false) = 0;

    virtual DateTime GetDateTime(
        const char* name,
        bool optional = false) = 0;

    virtual ObjectConstSP GetObject(
        const char* name,
        ObjectType* objectType,
        ValueToObject& mapToObject,
        bool optional = false) = 0;

    template<class T>
        typename T::outer_type GetInstance(
            const char* name,
            ValueToObject& mapToObject,
            bool optional=false)
    {
        ObjectConstSP obj = GetObject(name, &T::object_type, mapToObject, optional);
        return T::Coerce(obj);
    }

    virtual Variant GetVariant(
        const char* name,
        ValueToObject& mapToObject,
        bool optional = false) = 0;

    // all vectors are by implication optional - in other words if it is not there
    // then it has size zero
    //
    // however elements can be optional - this is what optional means - and in this
    // case elements have an optional value
    virtual std::vector<std::string> GetStringVector(
        const char* name) = 0;

    virtual std::vector<double> GetDoubleVector(
        const char* name) = 0;

    virtual std::vector<int> GetIntVector(
        const char* name) = 0;

    virtual std::vector<bool> GetBoolVector(
        const char* name) = 0;

    virtual std::vector<Date> GetDateVector(
        const char* name) = 0;

    virtual std::vector<DateTime> GetDateTimeVector(
        const char* name) = 0;

    virtual std::vector<Variant> GetVariantVector(
        const char* name,
        ValueToObject& mapToObject) = 0;

    virtual std::vector<ObjectConstSP> GetObjectVector(
        const char* name,
        ObjectType* objectType,
        ValueToObject& mapToObject,
        bool optional = false) = 0;

    template<class T>
        std::vector< typename T::outer_type > GetInstanceVector(
            const char* name,
            ValueToObject& mapToObject,
            bool optional=false)
    {
        std::vector<ObjectConstSP> tmp = GetObjectVector(
            name, &T::object_type, mapToObject, optional);
        std::vector<typename T::outer_type > output;
        output.reserve(tmp.size());
        for (size_t i = 0; i < tmp.size(); ++i)
            output.push_back(T::Coerce(tmp[i]));
        return output;
    }

    template<class T>
        std::vector<T> GetEnumVector(
            const char* name)
    {
        const std::vector<std::string>& tmp = GetStringVector(name);
        std::vector<T> output;
        output.reserve(tmp.size());
        for (size_t i = 0; i < tmp.size(); ++i)
            output.push_back(T(tmp[i]));
        return output;
    }

    virtual MatrixData<bool> GetBoolMatrix(
        const char* name) = 0;

    virtual MatrixData<int> GetIntMatrix(
        const char* name) = 0;

    virtual MatrixData<double> GetDoubleMatrix(
        const char* name) = 0;

    virtual MatrixData<std::string> GetStringMatrix(
        const char* name) = 0;

    virtual MatrixData<Date> GetDateMatrix(
        const char* name) = 0;

    virtual MatrixData<DateTime> GetDateTimeMatrix(
        const char* name) = 0;

    virtual MatrixData<ObjectConstSP> GetObjectMatrix(
        const char* name,
        ObjectType* objectType,
        ValueToObject& mapToObject,
        bool optional = false) = 0;

    template<class T>
        MatrixData< typename T::outer_type > GetInstanceMatrix(
            const char* name,
            ValueToObject& mapToObject,
            bool optional=false)
    {
        MatrixData<ObjectConstSP> tmp = GetObjectMatrix(
            name, &T::object_type, mapToObject, optional);

        const std::vector<ObjectConstSP>& tmpData = tmp.Data();

        size_t nr = tmp.Rows();
        size_t nc = tmp.Cols();
        size_t size = tmpData.size();

        std::vector< typename T::outer_type > data;
        data.reserve(size);
        for (size_t i = 0; i < size; ++i)
            data.push_back(T::Coerce(tmpData[i]));

        return MatrixData< typename T::outer_type >(nr, nc, data);
    }

    virtual spi::MatrixData<Variant> GetVariantMatrix(
        const char* name,
        ValueToObject& mapToObject) = 0;

    virtual bool Exists(const char* name) = 0;

    virtual MapSP ExportMap() = 0;
};

SPI_END_NAMESPACE

#endif
