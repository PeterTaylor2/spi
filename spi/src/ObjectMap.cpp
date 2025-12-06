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
#include "ObjectMap.hpp"
#include "RuntimeError.hpp"
#include "LexerReader.hpp"

#include <spi_util/Lexer.hpp>
#include <spi_util/StringUtil.hpp>

#include "Service.hpp"
#include "ValueToObject.hpp"
#include "InputContext.hpp"
#include "MatrixData.hpp"

#define BEGIN_ANONYMOUS_NAMESPACE namespace {
#define END_ANONYMOUS_NAMESPACE }

SPI_BEGIN_NAMESPACE

BEGIN_ANONYMOUS_NAMESPACE

void ThrowUndefinedValue(const char* name)
{
    throw RuntimeError(
        "Undefined value for non-optional field '%s'", name);
}

template<typename T>
MatrixData<T> MapGetMatrix(const MapConstSP& cm, const char* name)
{
    Value vm = cm->GetValue(name);
    if (vm.isUndefined())
        return MatrixData<T>();

    return MatrixData<T>::FromValue(vm);
}

END_ANONYMOUS_NAMESPACE

ObjectMap::ObjectMap(const MapSP& m, bool noHiding)
    :
    m_map(m),
    m_constMap(m),
    m_noHiding(noHiding)
{}

ObjectMap::ObjectMap(const MapConstSP& m)
    :
    m_map(),
    m_constMap(m),
    m_noHiding(false)
{}

// implementation of IObjectMap
void ObjectMap::SetChar(
    const char* name,
    char value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, Value(value));
    }
}

void ObjectMap::SetString(
    const char* name,
    const std::string& value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, Value(value));
    }
}

void ObjectMap::SetInt(
    const char* name,
    int value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, Value(value));
    }
}

void ObjectMap::SetBool(
    const char* name,
    bool value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, Value(value));
    }
}

void ObjectMap::SetDouble(
    const char* name,
    double value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, Value(value));
    }
}

void ObjectMap::SetDate(
    const char* name,
    Date value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, Value(value));
    }
}

void ObjectMap::SetDateTime(
    const char* name,
    DateTime value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, Value(value));
    }
}

void ObjectMap::SetObject(
    const char* name,
    const ObjectConstSP& value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        Value objectValue = value ? value->as_value() : Value();
        if (objectValue.isUndefined())
            objectValue = Value(value);
        m_map->SetValue(name, objectValue);
    }
}

void ObjectMap::SetVariant(
    const char* name,
    const Variant& value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, value.ToMap());
    }
}

void ObjectMap::SetStringVector(
    const char* name,
    const std::vector<std::string>& value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, Value(value));
    }
}

void ObjectMap::SetDoubleVector(
    const char* name,
    const std::vector<double>& value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, Value(value));
    }
}

void ObjectMap::SetIntVector(
    const char* name,
    const std::vector<int>& value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, Value(value));
    }
}

void ObjectMap::SetBoolVector(
    const char* name,
    const std::vector<bool>& value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, Value(value));
    }
}

void ObjectMap::SetDateVector(
    const char* name,
    const std::vector<Date>& value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, Value(value));
    }
}

void ObjectMap::SetDateTimeVector(
    const char* name,
    const std::vector<DateTime>& value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, Value(value));
    }
}

void ObjectMap::SetVariantVector(
    const char* name,
    const std::vector<Variant>& value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        std::vector<Value> values;
        for (size_t i = 0; i < value.size(); ++i)
            values.push_back(Value(value[i].ToMap()));
        m_map->SetValue(name, Value(values));
    }
}

void ObjectMap::SetObjectVector(
    const char* name,
    const std::vector<ObjectConstSP>& value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        std::vector<Value> values;
        values.reserve(value.size());
        for (std::vector<ObjectConstSP>::const_iterator iter = value.begin();
             iter != value.end(); ++iter)
        {
            const ObjectConstSP obj = *iter;
            if (obj)
            {
                Value ov = obj->as_value();
                if (ov.isUndefined())
                    ov = Value(obj);
                values.push_back(ov);
            }
            else
            {
                values.push_back(Value(obj));
            }
        }
        m_map->SetValue(name, values);
    }
}

void ObjectMap::SetBoolMatrix(
    const char* name,
    const MatrixData<bool>& value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, value.ToArray());
    }
}

void ObjectMap::SetIntMatrix(
    const char* name,
    const MatrixData<int>& value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, value.ToArray());
    }
}

void ObjectMap::SetDoubleMatrix(
    const char* name,
    const MatrixData<double>& value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, value.ToArray());
    }
}

void ObjectMap::SetStringMatrix(
    const char* name,
    const MatrixData<std::string>& value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, value.ToArray());
    }
}

void ObjectMap::SetDateMatrix(
    const char* name,
    const MatrixData<Date>& value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, value.ToArray());
    }
}

void ObjectMap::SetDateTimeMatrix(
    const char* name,
    const MatrixData<DateTime>& value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, value.ToArray());
    }
}

void ObjectMap::SetObjectMatrix(
    const char* name,
    const MatrixData<ObjectConstSP>& value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, value.ToArray());
    }
}

void ObjectMap::SetVariantMatrix(
    const char* name,
    const spi::MatrixData<Variant>& value,
    bool hidden)
{
    if (m_noHiding || !hidden)
    {
        m_map->SetValue(name, value.ToArray());
    }
}

void ObjectMap::ImportMap(const Map* aMap)
{
    const std::vector<std::string>& fieldNames = aMap->FieldNames();
    for (size_t i = 0; i < fieldNames.size(); ++i)
    {
        const std::string& name = fieldNames[i];
        const Value& value = aMap->GetValue(name);
        m_map->SetValue(name, value);
    }
}

void ObjectMap::SetClassName(const std::string & className)
{
    m_map->SetClassName(className);
}

std::string ObjectMap::ClassName()
{
    return m_constMap->ClassName();
}

char ObjectMap::GetChar(
    const char* name,
    bool optional,
    char defaultValue)
{
    Value value = m_constMap->GetValue(name);
    if (value.isUndefined())
    {
        if (optional)
            return defaultValue;
        ThrowUndefinedValue(name);
    }

    return value.getChar(m_constMap->Permissive());
}

std::string ObjectMap::GetString(
    const char* name,
    bool optional,
    const char* defaultValue)
{
    Value value = m_constMap->GetValue(name);
    if (value.isUndefined())
    {
        if (optional)
            return defaultValue;
        ThrowUndefinedValue(name);
    }

    return value.getString(m_constMap->Permissive());
}

int ObjectMap::GetInt(
    const char* name,
    bool optional,
    int defaultValue)
{
    Value value = m_constMap->GetValue(name);
    if (value.isUndefined())
    {
        if (optional)
            return defaultValue;
        ThrowUndefinedValue(name);
    }

    return value.getInt(m_constMap->Permissive());
}

bool ObjectMap::GetBool(
    const char* name,
    bool optional,
    bool defaultValue)
{
    Value value = m_constMap->GetValue(name);
    if (value.isUndefined())
    {
        if (optional)
            return defaultValue;
        ThrowUndefinedValue(name);
    }

    return value.getBool(m_constMap->Permissive());
}

double ObjectMap::GetDouble(
    const char* name,
    bool optional,
    double defaultValue)
{
    Value value = m_constMap->GetValue(name);
    if (value.isUndefined())
    {
        if (optional)
            return defaultValue;
        ThrowUndefinedValue(name);
    }

    return value.getDouble(m_constMap->Permissive());
}

Date ObjectMap::GetDate(
    const char* name,
    bool optional)
{
    Value value = m_constMap->GetValue(name);
    if (value.isUndefined())
    {
        if (optional)
            return Date();
        ThrowUndefinedValue(name);
    }

    return value.getDate(m_constMap->Permissive());
}

DateTime ObjectMap::GetDateTime(
    const char* name,
    bool optional)
{
    Value value = m_constMap->GetValue(name);
    if (value.isUndefined())
    {
        if (optional)
            return DateTime();
        ThrowUndefinedValue(name);
    }

    return value.getDateTime(m_constMap->Permissive());
}

ObjectConstSP ObjectMap::GetObject(
    const char* name,
    ObjectType* objectType,
    ValueToObject& mapToObject,
    bool optional)
{
    Value value = m_constMap->GetValue(name);
    if (value.isUndefined())
    {
        if (optional)
            return ObjectConstSP();
        ThrowUndefinedValue(name);
    }

    return mapToObject.ObjectFromValue(value, objectType);
}

Variant ObjectMap::GetVariant(
    const char* name,
    ValueToObject& mapToObject,
    bool optional)
{
    Value vm = m_constMap->GetValue(name);
    if (vm.isUndefined())
    {
        if (optional)
            return Variant();
        ThrowUndefinedValue(name);
    }

    return Variant(vm, mapToObject);
}

std::vector<std::string> ObjectMap::GetStringVector(
    const char* name)
{
    Value value = m_constMap->GetValue(name);
    if (value.isUndefined())
        return std::vector<std::string>();

    return value.getStringVector(m_constMap->Permissive());
}

std::vector<double> ObjectMap::GetDoubleVector(
    const char* name)
{
    Value value = m_constMap->GetValue(name);
    if (value.isUndefined())
        return std::vector<double>();

    return value.getDoubleVector(m_constMap->Permissive());
}

std::vector<int> ObjectMap::GetIntVector(
    const char* name)
{
    Value value = m_constMap->GetValue(name);
    if (value.isUndefined())
        return std::vector<int>();

    return value.getIntVector(m_constMap->Permissive());
}

std::vector<bool> ObjectMap::GetBoolVector(
    const char* name)
{
    Value value = m_constMap->GetValue(name);
    if (value.isUndefined())
        return std::vector<bool>();

    return value.getBoolVector(m_constMap->Permissive());
}

std::vector<Date> ObjectMap::GetDateVector(
    const char* name)
{
    Value value = m_constMap->GetValue(name);
    if (value.isUndefined())
        return std::vector<Date>();

    return value.getDateVector(m_constMap->Permissive());
}

std::vector<DateTime> ObjectMap::GetDateTimeVector(
    const char* name)
{
    Value value = m_constMap->GetValue(name);
    if (value.isUndefined())
        return std::vector<DateTime>();

    return value.getDateTimeVector(m_constMap->Permissive());
}

std::vector<ObjectConstSP> ObjectMap::GetObjectVector(
    const char* name,
    ObjectType* objectType,
    ValueToObject& mapToObject,
    bool optional)
{
    Value value = m_constMap->GetValue(name);
    if (value.isUndefined())
        return std::vector<ObjectConstSP>();

    IArrayConstSP valueArray = value.getArray();
    size_t size = valueArray->size();
    std::vector<ObjectConstSP> output;
    output.reserve(size);

    for (size_t i = 0; i < size; ++i)
    {
        Value item = valueArray->getItem(i);
        if (optional && item.isUndefined())
        {
            output.push_back(ObjectConstSP());
        }
        else
        {
            output.push_back(mapToObject.ObjectFromValue(item, objectType));
        }
    }
    return output;
}

std::vector<Variant> ObjectMap::GetVariantVector(
    const char* name,
    ValueToObject& mapToObject,
    bool optional)
{
    Value value = m_constMap->GetValue(name);
    if (value.isUndefined())
        return std::vector<Variant>();

    IArrayConstSP valueArray = value.getArray();
    size_t size = valueArray->size();
    std::vector<Variant> output;
    output.reserve(size);

    for (size_t i = 0; i < size; ++i)
    {
        Value item = valueArray->getItem(i);
        if (item.isUndefined() && !optional)
            SPI_THROW_RUNTIME_ERROR("Value must be defined for element [" << i << "]");
        output.push_back(Variant(item, mapToObject));
    }
    return output;
}

MatrixData<bool> ObjectMap::GetBoolMatrix(
    const char* name)
{
    return MapGetMatrix<bool>(m_constMap, name);
}

MatrixData<int> ObjectMap::GetIntMatrix(
    const char* name)
{
    return MapGetMatrix<int>(m_constMap, name);
}

MatrixData<double> ObjectMap::GetDoubleMatrix(
    const char* name)
{
    return MapGetMatrix<double>(m_constMap, name);
}

MatrixData<std::string> ObjectMap::GetStringMatrix(
    const char* name)
{
    return MapGetMatrix<std::string>(m_constMap, name);
}

MatrixData<Date> ObjectMap::GetDateMatrix(
    const char* name)
{
    return MapGetMatrix<Date>(m_constMap, name);
}

MatrixData<DateTime> ObjectMap::GetDateTimeMatrix(
    const char* name)
{
    return MapGetMatrix<DateTime>(m_constMap, name);
}

MatrixData<ObjectConstSP> ObjectMap::GetObjectMatrix(
    const char* name,
    ObjectType* objectType,
    ValueToObject& mapToObject,
    bool optional)
{
    const MatrixData<Value>& tmp = MapGetMatrix<Value>(
        m_constMap, name);

    size_t nr = tmp.Rows();
    size_t nc = tmp.Cols();

    if (nr == 0 && nc == 0)
        return MatrixData<ObjectConstSP>();

    MatrixData<ObjectConstSP> output(nr, nc);
    for (size_t i = 0; i < nr; ++i)
    {
        for (size_t j =0; j < nc; ++j)
        {
            const Value& item = tmp[i][j];
            if (optional && item.isUndefined())
            {
                output[i][j] = ObjectConstSP();
            }
            else
            {
                output[i][j] = mapToObject.ObjectFromValue(item, objectType);
            }
        }
    }
    return output;
}

MapSP ObjectMap::ExportMap()
{
    return m_constMap->Copy();
}

spi::MatrixData<Variant> ObjectMap::GetVariantMatrix(
    const char* name,
    ValueToObject& mapToObject,
    bool optional)
{
    const MatrixData<Value>& tmp = MapGetMatrix<Value>(
        m_constMap, name);

    size_t nr = tmp.Rows();
    size_t nc = tmp.Cols();

    if (nr == 0 && nc == 0)
        return MatrixData<Variant>();

    MatrixData<Variant> output(nr, nc);
    for (size_t i = 0; i < nr; ++i)
    {
        for (size_t j = 0; j < nc; ++j)
        {
            const Value& item = tmp[i][j];
            if (item.isUndefined() && !optional)
                SPI_THROW_RUNTIME_ERROR("Value must be defined for element [" << i << " , "
                    << j << "]");

            output[i][j] = Variant(item, mapToObject);
        }
    }
    return output;
}

bool ObjectMap::Exists(const char * name)
{
    return m_constMap->Exists(name);
}

SPI_END_NAMESPACE
