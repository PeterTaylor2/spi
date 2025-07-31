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
#include "ObjectPut.hpp"

#include "IObjectMap.hpp"
#include "InputContext.hpp"
#include "MapObject.hpp"
#include "ObjectMap.hpp"
#include "RuntimeError.hpp"
#include "StringUtil.hpp"

#include <set>

#define BEGIN_ANONYMOUS_NAMESPACE namespace {
#define END_ANONYMOUS_NAMESPACE }

SPI_BEGIN_NAMESPACE

BEGIN_ANONYMOUS_NAMESPACE

class ObjectPutMap : public IObjectMap
{
public:
    ObjectPutMap(
        IObjectMap*                     original,
        const std::vector<std::string>& names,
        const std::vector<Value>&       values,
        const InputContext*             context)
        :
        original(original),
        context(context),
        indexValues(),
        unusedNames(),
        namesInOrder(names)
    {
        if (names.size() != values.size())
            throw RuntimeError("Array size mismatch: names, values");
        size_t N = names.size();
        for (size_t i = 0; i < N; ++i)
        {
            indexValues[names[i]] = values[i];
            unusedNames.insert(names[i]);
        }

        // we must remove the original object_id as part of object_put
        if (original->Exists("object_id"))
            indexValues["object_id"] = Value("");
    }

    const std::vector<std::string> Unused() const
    {
        std::vector<std::string> unused(
            unusedNames.begin(), unusedNames.end());
        return unused;
    }

    void SetChar(
        const char* name,
        char value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetString(
        const char* name,
        const std::string& value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetInt(
        const char* name,
        int value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetBool(
        const char* name,
        bool value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetDouble(
        const char* name,
        double value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetDate(
        const char* name,
        Date value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetDateTime(
        const char* name,
        DateTime value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetObject(
        const char* name,
        const ObjectConstSP& value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetVariant(
        const char* name,
        const Variant& value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetStringVector(
        const char* name,
        const std::vector<std::string>& value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetDoubleVector(
        const char* name,
        const std::vector<double>& value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetIntVector(
        const char* name,
        const std::vector<int>& value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetBoolVector(
        const char* name,
        const std::vector<bool>& value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetDateVector(
        const char* name,
        const std::vector<Date>& value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetDateTimeVector(
        const char* name,
        const std::vector<DateTime>& value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetVariantVector(
        const char* name,
        const std::vector<Variant>& value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetObjectVector(
        const char* name,
        const std::vector<ObjectConstSP>& value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetBoolMatrix(
        const char* name,
        const MatrixData<bool>& value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetIntMatrix(
        const char* name,
        const MatrixData<int>& value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetDoubleMatrix(
        const char* name,
        const MatrixData<double>& value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetStringMatrix(
        const char* name,
        const MatrixData<std::string>& value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetDateMatrix(
        const char* name,
        const MatrixData<Date>& value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetDateTimeMatrix(
        const char* name,
        const MatrixData<DateTime>& value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetObjectMatrix(
        const char* name,
        const MatrixData<ObjectConstSP>& value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetVariantMatrix(
        const char* name,
        const spi::MatrixData<Variant>& value,
        bool hidden)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void ImportMap(const Map* aMap)
    {
        SPI_NOT_IMPLEMENTED;
    }

    void SetClassName(const std::string& className)
    {
        SPI_NOT_IMPLEMENTED;
    }

    std::string ClassName()
    {
        return original->ClassName();
    }

    char GetChar(
        const char* name,
        bool optional,
        char defaultValue)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetChar(name, optional, defaultValue);
            return context->ValueToChar(value, optional, defaultValue);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    std::string GetString(
        const char* name,
        bool optional,
        const char* defaultValue)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetString(name, optional, defaultValue);
            return context->ValueToString(value, optional, defaultValue);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    int GetInt(
        const char* name,
        bool optional,
        int defaultValue)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetInt(name, optional, defaultValue);
            return context->ValueToInt(value, optional, defaultValue);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    bool GetBool(
        const char* name,
        bool optional,
        bool defaultValue)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetBool(name, optional, defaultValue);
            return context->ValueToBool(value, optional, defaultValue);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    double GetDouble(
        const char* name,
        bool optional,
        double defaultValue)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetDouble(name, optional, defaultValue);
            return context->ValueToDouble(value, optional, defaultValue);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    Date GetDate(
        const char* name,
        bool optional)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetDate(name, optional);
            return context->ValueToDate(value, optional);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    DateTime GetDateTime(
        const char* name,
        bool optional)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetDateTime(name, optional);
            return context->ValueToDateTime(value, optional);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    ObjectConstSP GetObject(
        const char* name,
        ObjectType* objectType,
        ValueToObject& mapToObject,
        bool optional)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetObject(
                    name, objectType, mapToObject, optional);
            return context->ValueToObject(value, objectType, optional);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    Variant GetVariant(
        const char* name,
        ValueToObject& mapToObject,
        bool optional)
    {
        SPI_NOT_IMPLEMENTED;
    }

    std::vector<std::string> GetStringVector(
        const char* name)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetStringVector(name);
            return context->ValueToStringVector(value);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    std::vector<double> GetDoubleVector(
        const char* name)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetDoubleVector(name);
            return context->ValueToDoubleVector(value);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    std::vector<int> GetIntVector(
        const char* name)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetIntVector(name);
            return context->ValueToIntVector(value);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    std::vector<bool> GetBoolVector(
        const char* name)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetBoolVector(name);
            return context->ValueToBoolVector(value);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    std::vector<Date> GetDateVector(
        const char* name)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetDateVector(name);
            return context->ValueToDateVector(value);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    std::vector<DateTime> GetDateTimeVector(
        const char* name)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetDateTimeVector(name);
            return context->ValueToDateTimeVector(value);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    std::vector<ObjectConstSP> GetObjectVector(
        const char* name,
        ObjectType* objectType,
        ValueToObject& mapToObject,
        bool optional)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetObjectVector(
                    name, objectType, mapToObject, optional);
            return context->ValueToObjectVector(value, objectType, optional);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    std::vector<Variant> GetVariantVector(
        const char* name,
        ValueToObject& mapToObject,
        bool optional)
    {
        SPI_NOT_IMPLEMENTED;
    }

    MatrixData<bool> GetBoolMatrix(
        const char* name)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetBoolMatrix(name);
            return context->ValueToBoolMatrix(value);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    MatrixData<int> GetIntMatrix(
        const char* name)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetIntMatrix(name);
            return context->ValueToIntMatrix(value);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    MatrixData<double> GetDoubleMatrix(
        const char* name)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetDoubleMatrix(name);
            return context->ValueToDoubleMatrix(value);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    MatrixData<std::string> GetStringMatrix(
        const char* name)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetStringMatrix(name);
            return context->ValueToStringMatrix(value);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    MatrixData<Date> GetDateMatrix(
        const char* name)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetDateMatrix(name);
            return context->ValueToDateMatrix(value);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    MatrixData<DateTime> GetDateTimeMatrix(
        const char* name)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetDateTimeMatrix(name);
            return context->ValueToDateTimeMatrix(value);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    MatrixData<ObjectConstSP> GetObjectMatrix(
        const char* name,
        ObjectType* objectType,
        ValueToObject& mapToObject,
        bool optional)
    {
        try
        {
            Value value;
            if (!ModifiedValue(name, value))
                return original->GetObjectMatrix(
                    name, objectType, mapToObject, optional);
            return context->ValueToObjectMatrix(value, objectType, optional);
        }
        catch (std::exception& e)
        {
            SPI_THROW_RUNTIME_ERROR("ObjectPut for '" << name << "' failed: " << e.what())
        }
    }

    MatrixData<Variant> GetVariantMatrix(
        const char* name,
        ValueToObject& mapToObject,
        bool optional)
    {
        SPI_NOT_IMPLEMENTED;
    }

    bool Exists(const char* name)
    {
        std::map<std::string, Value>::const_iterator iter = indexValues.find(name);
        if (iter != indexValues.end())
            return true;
        return original->Exists(name);
    }

    MapSP ExportMap()
    {
        MapSP xm = original->ExportMap();

        for(std::vector<std::string>::const_iterator iter = namesInOrder.begin();
            iter != namesInOrder.end(); ++iter)
        {
            const std::string& name = *iter;

            std::map<std::string,Value>::const_iterator pair = indexValues.find(name);
            if (pair == indexValues.end())
                throw RuntimeError("Mis-match between 'namesInOrder' and 'indexValues'");

            const Value& value = pair->second;

            std::vector<std::string> nameParts = spi_util::StringSplit(name, " ");
            std::vector<std::string> parts;
            for (size_t i = 0; i < nameParts.size(); ++i)
            {
                const std::string& part = spi_util::StringStrip(nameParts[i]);
                if (part.length() > 0)
                    parts.push_back(part);
            }

            switch(parts.size())
            {
            case 2:
                {
                    const std::string& dataType = spi_util::StringUpper(parts[0]);
                    const std::string& nm = parts[1];
                    switch(dataType[0])
                    {
                    case 'D':
                        if (dataType == "DATE")
                        {
                            xm->SetValue(nm, context->ValueToDate(value, true));
                            continue;
                        }
                        else if (dataType == "DOUBLE")
                        {
                            xm->SetValue(nm, context->ValueToDouble(value, true));
                            continue;
                        }
                        else if (dataType == "DATETIME")
                        {
                            xm->SetValue(nm, context->ValueToDateTime(value, true));
                            continue;
                        }
                        else if (dataType == "DATE[]")
                        {
                            xm->SetValue(nm, context->ValueToDateVector(value, true));
                            continue;
                        }
                        else if (dataType == "DOUBLE[]")
                        {
                            xm->SetValue(nm, context->ValueToDoubleVector(value, true));
                            continue;
                        }                        
                        else if (dataType == "DATETIME[]")
                        {
                            xm->SetValue(nm, context->ValueToDateTime(value, true));
                            continue;
                        }
                        break;
                    case 'S':
                        if (dataType == "STRING")
                        {
                            xm->SetValue(nm, context->ValueToString(value, true));
                            continue;
                        }
                        else if (dataType == "STRING[]")
                        {
                            xm->SetValue(nm, context->ValueToStringVector(value, true));
                            continue;
                        }
                        break;
                    case 'I':
                        if (dataType == "INT")
                        {
                            xm->SetValue(nm, context->ValueToInt(value, true));
                            continue;
                        }
                        else if (dataType == "INT[]")
                        {
                            xm->SetValue(nm, context->ValueToIntVector(value, true));
                            continue;
                        }
                        break;
                    case 'B':
                        if (dataType == "BOOL")
                        {
                            xm->SetValue(nm, context->ValueToBool(value, true));
                            continue;
                        }
                        else if (dataType == "BOOL[]")
                        {
                            xm->SetValue(nm, context->ValueToBoolVector(value, true));
                            continue;
                        }
                        break;
                    case 'O':
                        if (dataType == "OBJECT")
                        {
                            xm->SetValue(nm, context->ValueToObject(value, NULL, true));
                            continue;
                        }
                        else if (dataType == "OBJECT[]")
                        {
                            xm->SetValue(nm, context->ValueToObjectVector(value, NULL, true));
                            continue;
                        }
                        break;
                    default:
                        break;
                    }
                    throw RuntimeError("Unsupported dataType '%s'", dataType.c_str());
                }
                break;
            case 1:
                xm->SetValue(parts[0], value);
                break;
            case 0:
                throw RuntimeError("Empty name '%s'", name.c_str());
            default:
                throw RuntimeError("Bad format name '%s'", name.c_str());
            }
        }
        unusedNames.clear();

        return xm;
    }

private:

    IObjectMap* original;
    const InputContext* context;
    std::map<std::string, Value> indexValues;
    std::set<std::string> unusedNames;
    std::vector<std::string> namesInOrder;

    bool ModifiedValue(const std::string& name, Value& value)
    {
        std::map<std::string,Value>::const_iterator iter =
            indexValues.find(name);
        if (iter == indexValues.end())
            return false;

        unusedNames.erase(name);
        value = iter->second;
        return true;
    }
};

END_ANONYMOUS_NAMESPACE

ObjectConstSP ObjectPut(
    const ObjectConstSP& obj,
    const char*          name,
    const Value&         value,
    const InputContext*  context)
{
    std::vector<std::string> names;
    std::vector<Value>       values;

    names.push_back(name);
    values.push_back(value);

    return ObjectPut(obj, names, values, context);
}

ObjectConstSP ObjectPut(
    const ObjectConstSP& obj,
    const std::vector<std::string>& names,
    const std::vector<Value>&       values,
    const InputContext*             context)
{
    if (!context)
        context = InputContext::NoContext();

    ObjectType* ot;
    MapConstSP attributes;

    if (!obj)
    {
        ot = &MapObject::object_type;
        attributes.reset(new Map(ot->get_class_name()));
    }
    else
    {
        ot = obj->get_object_type();
        attributes = obj->get_attributes();
    }

    if (!ot->can_put())
        throw RuntimeError("Object of type '%s' does not support ObjectPut",
            ot->get_class_name());

    ObjectMap om(attributes);
    ObjectPutMap opm(&om, names, values, context);

    ValueToObject valueToObject(ot->get_service(), new ObjectRefCache());
    ObjectConstSP modifiedObject = ot->make_from_map(&opm, valueToObject);

    const std::vector<std::string>& unused = opm.Unused();
    if (unused.size() > 0)
        throw RuntimeError("Object of type '%s' does not support names '%s'",
            ot->get_class_name(),
            StringJoin(",", unused).c_str());

    return modifiedObject;
}

void ObjectUpdateMetaData(
    const ObjectConstSP& obj,
    const char* name,
    const Value& value,
    const InputContext* context)
{
    std::vector<std::string> names(1, name);
    std::vector<Value> values(1, value);

    ObjectUpdateMetaData(obj, names, values, context);
}

void ObjectUpdateMetaData(
    const ObjectConstSP& obj,
    const std::vector<std::string>& names,
    const std::vector<Value>& values,
    const InputContext* context)
{
    if (!context)
        context = InputContext::NoContext();

    const MapSP& omd = obj->get_meta_data();

    SPI_PRE_CONDITION(names.size() == values.size());

    ObjectMap om(omd);
    ObjectPutMap opm(&om, names, values, context);

    const MapSP& combined = opm.ExportMap();

    obj->update_meta_data(combined);
}

SPI_END_NAMESPACE
