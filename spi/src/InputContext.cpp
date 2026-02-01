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
#include "InputContext.hpp"

#include "Array.hpp"
#include "Service.hpp"
#include "StringUtil.hpp"
#include "EnumHelper.hpp"
#include "ObjectTextStreamer.hpp"
#include "MatrixData.hpp"
#include "ObjectHandle.hpp"
#include "ObjectPut.hpp"
#include "ObjectMap.hpp"
#include "MapObject.hpp"
#include "InputValues.hpp"
#include "SHA.hpp"

#include "ExcelInputContext.hpp"
#include "PythonInputContext.hpp"

#define BEGIN_ANONYMOUS_NAMESPACE namespace {
#define END_ANONYMOUS_NAMESPACE }

SPI_BEGIN_NAMESPACE

BEGIN_ANONYMOUS_NAMESPACE

// for each scalar type we create a class for converting Value to the specific
// type given the input context
//
// this enables us to implement Vector and Matrix equivalents with a simple
// function template

template<typename T>
class ConvertValue
{
public:
    T operator()(const InputContext* context, const Value& value) const
    {
        SPI_NOT_IMPLEMENTED;
    }
};

template<>
class ConvertValue<bool>
{
public:
    ConvertValue(bool optional=false, bool defaultValue=false)
        : optional(optional), defaultValue(defaultValue)
    {}

    bool operator()(const InputContext* context, const Value& value) const
    {
        return context->ValueToBool(value, optional, defaultValue);
    }

private:
    bool optional;
    bool defaultValue;
};

template<>
class ConvertValue<char>
{
public:
    ConvertValue(bool optional=false, char defaultValue=' ')
        : optional(optional), defaultValue(defaultValue)
    {}

    char operator()(const InputContext* context, const Value& value) const
    {
        return context->ValueToChar(value, optional, defaultValue);
    }

private:
    bool optional;
    char defaultValue;
};

template<>
class ConvertValue<int>
{
public:
    ConvertValue(bool optional=false, int defaultValue=0)
        : optional(optional), defaultValue(defaultValue)
    {}

    int operator()(const InputContext* context, const Value& value) const
    {
        return context->ValueToInt(value, optional, defaultValue);
    }

private:
    bool optional;
    int  defaultValue;
};

template<>
class ConvertValue<double>
{
public:
    ConvertValue(bool optional=false, double defaultValue=0.0)
        : optional(optional), defaultValue(defaultValue)
    {}

    double operator()(const InputContext* context, const Value& value) const
    {
        return context->ValueToDouble(value, optional, defaultValue);
    }

private:
    bool   optional;
    double defaultValue;
};

template<>
class ConvertValue<std::string>
{
public:
    ConvertValue(bool optional=false, const char* defaultValue="")
        : optional(optional), defaultValue(defaultValue)
    {}

    std::string operator()(const InputContext* context, const Value& value) const
    {
        return context->ValueToString(value, optional, defaultValue);
    }

private:
    bool        optional;
    const char* defaultValue;
};

template<>
class ConvertValue<Date>
{
public:
    ConvertValue(bool optional=false, Date defaultValue=Date())
        : optional(optional), defaultValue(defaultValue)
    {}

    Date operator()(const InputContext* context, const Value& value) const
    {
        return context->ValueToDate(value, optional, defaultValue);
    }

private:
    bool optional;
    Date defaultValue;
};

template<>
class ConvertValue<DateTime>
{
public:
    ConvertValue(bool optional=false, DateTime defaultValue=DateTime())
        : optional(optional), defaultValue(defaultValue)
    {}

    DateTime operator()(const InputContext* context, const Value& value) const
    {
        return context->ValueToDateTime(value, optional, defaultValue);
    }

private:
    bool optional;
    DateTime defaultValue;
};



template<>
class ConvertValue<Variant>
{
public:
    ConvertValue(bool optional=false)
        : optional(optional)
    {}

    Variant operator()(const InputContext* context, const Value& value) const
    {
        return context->ValueToVariant(value, optional);
    }

private:
    bool optional;
};


template<>
class ConvertValue<ObjectConstSP>
{
public:
    ConvertValue(ObjectType* objectType=0, bool optional=false)
        : objectType(objectType), optional(optional)
    {}

    ObjectConstSP
    operator()(const InputContext* context, const Value& value) const
    {
        return context->ValueToObject(value, objectType, optional);
    }

private:
    ObjectType* objectType;
    bool        optional;
};


std::vector<Value> StripValueArray(const std::vector<Value>& in)
{
    std::vector<Value> out;
    bool started = false;
    bool stripRight = false;

    // strip left
    size_t i = 0;
    while (i < in.size())
    {
        if (!in[i].isUndefined())
            break;
        ++i;
    }

    // strip right
    size_t j = in.size();
    while (j > i)
    {
        if (!in[j-1].isUndefined())
            break;
        --j;
    }

    while (i < j)
    {
        out.push_back(in[i]);
        ++i;
    }

    return out;
}

template<typename T>
std::vector<T> ValueToVectorOfType(
    const Value& value,
    const InputContext* context,
    const ConvertValue<T>& converter)
{
    if (context->AcceptScalarForArray() && value.getType() != Value::ARRAY)
    {
        return std::vector<T>(1,converter(context,value));
    }

    bool stripArray = context->StripArray();
    const std::vector<Value>& values = stripArray ?
        StripValueArray(value.getArray()->getVector()) :
        value.getArray()->getVector();

    size_t size = values.size();
    std::vector<T> output;
    for (size_t i = 0; i < size; ++i)
        output.push_back(converter(context, values[i]));
    return output;
}

template<typename T>
MatrixData<T> ValueToMatrixOfType(
    const Value& value,
    const InputContext* context,
    const ConvertValue<T>& converter)
{
    if (value.isUndefined())
        return MatrixData<T>();

    if (context->AcceptScalarForArray() && value.getType() != Value::ARRAY)
    {
        return MatrixData<T>(1,1,std::vector<T>(1,converter(context,value)));
    }

    // do we need to support StripArray()?
    const MatrixData<Value>&  vm        = value.getMatrix();
    const std::vector<Value>& valueData = vm.Data();

    std::vector<T> data;
    size_t size = valueData.size();
    for (size_t i = 0; i < size; ++i)
        data.push_back(converter(context,valueData[i]));
    return MatrixData<T>(vm.Rows(), vm.Cols(), data);
}

END_ANONYMOUS_NAMESPACE

InputContext::~InputContext()
{}

InputContext* InputContext::Find(const std::string& name)
{
    static std::map<std::string, InputContext*> contexts;

    if (contexts.size() == 0)
    {
        InputContext* excelContext = ExcelContext();
        InputContext* pythonContext = PythonContext();
        InputContext* noContext = NoContext();

        contexts[excelContext->Context()] = excelContext;
        contexts[pythonContext->Context()] = pythonContext;
        contexts[noContext->Context()] = noContext;
    }

    std::map<std::string, InputContext*>::const_iterator iter =
        contexts.find(name);

    if (iter == contexts.end())
        throw RuntimeError("No context available for '%s'", name.c_str());

    return iter->second;
}

InputContext* InputContext::ExcelContext()
{
    static ExcelInputContext excelContext;

    return &excelContext;
}

InputContext* InputContext::PythonContext()
{
    static PythonInputContext pythonContext;

    return &pythonContext;
}

InputContext* InputContext::NoContext()
{
    static NoInputContext noContext;

    return &noContext;
}

void InputContext::SetAllOptional(bool allOptional)
{
    PythonInputContext::allOptional = allOptional;
    ExcelInputContext::allOptional = allOptional;
}

std::vector<bool> InputContext::ValueToBoolVector(const Value& value,
    bool optional,
    bool defaultValue) const
{
    return ValueToVectorOfType(value, this,
        ConvertValue<bool>(optional, defaultValue));
}

Variant InputContext::ValueToVariant(
    const Value& value,
    bool optional) const
{
    if (value.isUndefined() && !optional)
        throw spi::RuntimeError("Undefined value");

    return Variant(value, this);
}



std::vector<int> InputContext::ValueToIntVector(const Value& value,
    bool optional,
    int defaultValue) const
{
    return ValueToVectorOfType(value, this,
        ConvertValue<int>(optional, defaultValue));
}

std::vector<double> InputContext::ValueToDoubleVector(const Value& value,
    bool optional,
    double defaultValue) const
{
    return ValueToVectorOfType(value, this,
        ConvertValue<double>(optional, defaultValue));
}

std::vector<std::string> InputContext::ValueToStringVector(const Value& value,
    bool optional,
    const char* defaultValue) const
{
    return ValueToVectorOfType(value, this,
        ConvertValue<std::string>(optional, defaultValue));
}

std::vector<Date> InputContext::ValueToDateVector(const Value& value,
    bool optional,
    Date defaultValue) const
{
    return ValueToVectorOfType(value, this,
        ConvertValue<Date>(optional, defaultValue));
}

std::vector<DateTime> InputContext::ValueToDateTimeVector(
    const Value& value,
    bool optional,
    DateTime defaultValue) const
{
    return ValueToVectorOfType(value, this,
        ConvertValue<DateTime>(optional, defaultValue));
}

std::vector<Variant> InputContext::ValueToVariantVector(
    const Value& value,
    bool optional) const
{
    return ValueToVectorOfType(value, this, ConvertValue<Variant>(optional));
}

std::vector<ObjectConstSP> InputContext::ValueToObjectVector(
    const Value& value,
    ObjectType* objectType,
    bool optional) const
{
    return ValueToVectorOfType(value, this,
        ConvertValue<ObjectConstSP>(objectType, optional));
}

MatrixData<bool> InputContext::ValueToBoolMatrix(
    const Value& value,
    bool optional) const
{
    return ValueToMatrixOfType(value, this, ConvertValue<bool>(optional));
}

MatrixData<int> InputContext::ValueToIntMatrix(
    const Value& value,
    bool optional) const
{
    return ValueToMatrixOfType(value, this, ConvertValue<int>(optional));
}

MatrixData<double> InputContext::ValueToDoubleMatrix(
    const Value& value,
    bool optional) const
{
    return ValueToMatrixOfType(value, this, ConvertValue<double>(optional));
}

MatrixData<std::string> InputContext::ValueToStringMatrix(
    const Value& value,
    bool optional) const
{
    return ValueToMatrixOfType(value, this, ConvertValue<std::string>(optional));
}

MatrixData<Date> InputContext::ValueToDateMatrix(
    const Value& value,
    bool optional) const
{
    return ValueToMatrixOfType(value, this, ConvertValue<Date>(optional));
}

MatrixData<DateTime> InputContext::ValueToDateTimeMatrix(
    const Value& value,
    bool optional) const
{
    return ValueToMatrixOfType(value, this, ConvertValue<DateTime>(optional));
}

MatrixData<Variant> InputContext::ValueToVariantMatrix(
    const Value& value,
    bool optional) const
{
    return ValueToMatrixOfType(value, this, ConvertValue<Variant>(optional));
}

MatrixData<ObjectConstSP> InputContext::ValueToObjectMatrix(
    const Value& value,
    ObjectType* objectType,
    bool optional) const
{
    return ValueToMatrixOfType(value, this,
        ConvertValue<ObjectConstSP>(objectType, optional));
}


std::vector<Value> InputContext::ValueToVector(const Value& value) const
{
    const IArrayConstSP& anArray = value.getArray();

    if (StripArray())
        return StripValueArray(anArray->getVector());

    return anArray->getVector();
}

NoInputContext::NoInputContext()
{}

const char* NoInputContext::Context() const
{
    return "";
}

bool NoInputContext::ValueToBool(
    const Value& value,
    bool optional,
    bool defaultValue) const
{
    if (optional && value.isUndefined())
        return defaultValue;

    return value.getBool();
}

char NoInputContext::ValueToChar(
    const Value& value,
    bool optional,
    char defaultValue) const
{
    if (optional && value.isUndefined())
        return defaultValue;

    return value.getChar();
}

int NoInputContext::ValueToInt(const Value& value,
    bool optional,
    int defaultValue) const
{
    if (optional && value.isUndefined())
        return defaultValue;

    return value.getInt();
}

double NoInputContext::ValueToDouble(const Value& value,
    bool optional,
    double defaultValue) const
{
    if (optional && value.isUndefined())
        return defaultValue;

    return value.getDouble();
}

std::string NoInputContext::ValueToString(
    const Value& value,
    bool optional,
    const char* defaultValue) const
{
    if (optional && value.isUndefined())
        return defaultValue;

    return value.getString();
}

Date NoInputContext::ValueToDate(
    const Value& value,
    bool optional,
    Date defaultValue) const
{
    if (optional && value.isUndefined())
        return defaultValue;

    return value.getDate(true);
}

DateTime NoInputContext::ValueToDateTime(
    const Value& value,
    bool optional,
    DateTime defaultValue) const
{
    if (optional && value.isUndefined())
        return defaultValue;

    return value.getDateTime(true);
}

ObjectConstSP NoInputContext::ValueToObject(
    const Value& value,
    ObjectType* objectType,
    bool optional) const
{
    if (optional && value.isUndefined())
        return ObjectConstSP();

    if (objectType)
    {
        ObjectConstSP obj = objectType->coerce_from_value(value, this);
        if (obj)
            return obj;
    }

    return value.getObject();
}

bool NoInputContext::AcceptScalarForArray() const
{
    return false;
}

bool NoInputContext::StripArray() const
{
    return false;
}

/*
*********************************************************************
* Various generic functions that can be used from any InputContext
* which has converted its inputs to Value.
*********************************************************************
*/
Value StartLogging(
    const ServiceSP&    service,
    const Value&        filename,
    const Value&        options,
    const InputContext* context)
{
    service->start_logging(
        context->ValueToString(filename).c_str(),
        context->ValueToString(options, true).c_str());

    return filename;
}

Value StopLogging(const ServiceSP& service)
{
    bool wasLogging = service->is_logging();
    service->stop_logging();
    return wasLogging;
}

Value IsLogging(const ServiceSP& service)
{
    bool isLogging = service->is_logging();
    return isLogging;
}

Value HelpFunc(
    const ServiceSP&    service,
    const Value&        name,
    const InputContext* context)
{
    std::string funcName = context->ValueToString(name, true);
    if (funcName.empty())
        return service->get_function_names();

    FunctionCaller* func = service->get_function_caller(funcName.c_str());
    if (!func)
        throw RuntimeError("Unknown function %s", funcName.c_str());

    std::vector<std::string> argNames;
    std::vector<std::string> argTypes;
    std::vector<std::string> arrayTypes;
    std::vector<std::string> ioTypes;

    for (size_t i = 0; i < func->nbArgs; ++i)
    {
        const FuncArg& arg = func->args[i];
        argNames.push_back(arg.name);
        argTypes.push_back(arg.typeName);
        if (arg.isArray)
            arrayTypes.push_back("array");
        else
            arrayTypes.push_back("scalar");
        ioTypes.push_back("input");
    }

    std::vector<Value> values;
    values.push_back(Value(argNames));
    values.push_back(Value(argTypes));
    values.push_back(Value(arrayTypes));
    values.push_back(Value(ioTypes));

    return Value(values);
}

Value HelpEnum(
    const ServiceSP&    service,
    const Value&        name,
    const InputContext* context)
{
    std::string my_name = context->ValueToString(name, true);
    if (my_name == "")
        return service->get_enums();
    return service->get_enum_info(my_name)->Enumerands();
}

Value ObjectToString(
    const Value&        objectHandle,
    const Value&        in_format,
    const Value&        in_options,
    const Value&        metaDataHandle,
    const Value&        in_mergeMetaData,
    const InputContext* context,
    bool splitString)
{
    ObjectConstSP object = context->ValueToObject(objectHandle);
    std::string   format = context->ValueToString(in_format, true);
    std::string   options = context->ValueToString(in_options, true);
    ObjectConstSP oMeta = context->ValueToObject(metaDataHandle, &MapObject::object_type, true);
    MapConstSP metaData = oMeta ? MapObject::Coerce(oMeta)->get_inner() : MapConstSP();
    bool mergeMetaData  = context->ValueToBool(in_mergeMetaData, true, false);
    bool isBinary = false;

    std::string str = object->to_string(
        format.c_str(),
        options.c_str(), 
        metaData,
        mergeMetaData,
        &isBinary);

    if (isBinary)
        return Value(str, true);

    if (!splitString)
        return Value(str);

    return Value(StringSplit(str, '\n'));
}

Value ObjectFromString(
    const ServiceSP&    service,
    const Value&        objectString,
    const InputContext* context,
    bool                joinStrings)
{
    ObjectConstSP object;
    if (joinStrings)
    {
        const std::vector<std::string>& strings =
            context->ValueToStringVector(objectString);
        object = service->object_from_string(spi::StringJoin("\n", strings));
    }
    else
    {
        object = service->object_from_string(objectString.getConstString());
    }

    return Value(object);
}

Value ObjectToFile(
    const Value&        objectHandle,
    const Value&        in_fileName,
    const Value&        in_format,
    const Value&        in_options,
    const Value&        metaDataHandle,
    const Value&        in_mergeMetaData,
    const InputContext* context)
{
    ObjectConstSP object = context->ValueToObject(objectHandle);
    std::string fileName = context->ValueToString(in_fileName);
    std::string format   = context->ValueToString(in_format, true);
    std::string options  = context->ValueToString(in_options, true);
    ObjectConstSP oMeta  = context->ValueToObject(metaDataHandle, &MapObject::object_type, true);
    MapConstSP metaData  = oMeta ? MapObject::Coerce(oMeta)->get_inner() : MapConstSP();
    bool mergeMetaData   = context->ValueToBool(in_mergeMetaData, true, false);

    object->to_file(fileName.c_str(), format.c_str(), options.c_str(), metaData, mergeMetaData);

    return Value(fileName);
}

Value ObjectFromFile(
    const ServiceSP&    service,
    const Value&        in_fileName,
    const InputContext* context)
{
    std::string fileName = context->ValueToString(in_fileName);
    ObjectConstSP object = service->object_from_file(fileName);

    return Value(object);
}

static Value ObjectMapGetDefinedValue(
    const MapConstSP& objMap,
    const std::string& name)
{
    const Value& value = objMap->GetValue(name);
    if (value.isUndefined())
    {
        std::ostringstream oss;
        oss << "Could not find field with name '" << name << "'";

        const std::string& className = objMap->ClassName();
        if (className.length() > 0)
            oss << " in object of type '" << className << "'";

        throw std::runtime_error(oss.str());
    }
    return value;
}

static Value ObjectMapGet(
    const MapConstSP& objMap,
    const std::string& name)
{
    if (name.empty())
    {
        if (objMap->ClassName() == "Map")
            return Value(objMap->FieldTypesAndNames());
        return Value(objMap->FieldNames());
    }

    std::vector<std::string> typeNameParts = StringSplit(name, ' ');
    std::vector<std::string> parts;
    for (size_t i = 0; i < typeNameParts.size(); ++i)
    {
        const std::string& part = StringStrip(typeNameParts[i]);
        if (part.length() > 0)
            parts.push_back(part);
    }

    std::string nameNoType;
    std::string dataType;
    switch(parts.size())
    {
    case 2:
        dataType = StringUpper(parts[0]);
        nameNoType = parts[1];
        break;
    case 1:
        nameNoType = parts[0];
        break;
    case 0:
        throw RuntimeError("Empty name '%s'", name.c_str());
    default:
        throw RuntimeError("Bad format name '%s'", name.c_str());
    }

    std::vector<std::string> nameParts = StringSplit(nameNoType, '.');
    size_t numParts = nameParts.size();
    SPI_POST_CONDITION(numParts >= 1);

    MapConstSP current = objMap;
    for (size_t i = 1; i < numParts; ++i)
    {
        const std::string& thisPart = nameParts[i-1];
        if (!current)
        {
            throw RuntimeError("Cannot get field '%s' from NULL map",
                thisPart.c_str());
        }
        const Value& value = ObjectMapGetDefinedValue(current, thisPart);
        Value::Type valueType = value.getType();
        switch(valueType)
        {
        case Value::MAP:
            current = value.getMap();
            break;
        case Value::OBJECT:
            {
                ObjectConstSP obj = value.getObject();
                if (!obj)
                    current = MapConstSP();
                else
                    current = obj->get_public_map();
            }
            break;
        default:
            {
                std::ostringstream oss;
                oss << "Field '" << thisPart << "'";

                const std::string& className = current->ClassName();
                if (className.length() > 0)
                    oss << " in object of type '" << className << "'";

                oss << " is '" << Value::TypeToString(valueType)
                    << "' and not an Object (or Map)";

                throw std::runtime_error(oss.str());
            }
            break;
        }
    }
    Value value = ObjectMapGetDefinedValue(current, nameParts.back());

    if (dataType.empty())
        return value;

    // validate value is of the correct type
    //
    // we do this by trying to convert to the requested type and then
    // constructing a new value
    switch(dataType[0])
    {
    case 'D':
        if (dataType == "DATE")
        {
            return Value(value.getDate());
        }
        else if (dataType == "DOUBLE")
        {
            return Value(value.getDouble());
        }
        else if (dataType == "DATETIME")
        {
            return Value(value.getDateTime());
        }
        else if (dataType == "DATE[]")
        {
            return Value(value.getDateVector());
        }
        else if (dataType == "DOUBLE[]")
        {
            return Value(value.getDoubleVector());
        }
        else if (dataType == "DATETIME[]")
        {
            return Value(value.getDateTimeVector());
        }
        break;
    case 'S':
        if (dataType == "STRING")
        {
            return Value(value.getString());
        }
        else if (dataType == "STRING[]")
        {
            return Value(value.getStringVector());
        }
        break;
    case 'I':
        if (dataType == "INT")
        {
            return Value(value.getInt());
        }
        else if (dataType == "INT[]")
        {
            return Value(value.getIntVector());
        }
        break;
    case 'B':
        if (dataType == "BOOL")
        {
            return Value(value.getBool());
        }
        else if (dataType == "BOOL[]")
        {
            return Value(value.getBoolVector());
        }
        break;
    case 'O':
        if (dataType == "OBJECT")
        {
            return Value(value.getObject());
        }
        else if (dataType == "OBJECT[]")
        {
            return Value(value.getObjectVector());
        }
        break;
    default:
        break;
    }
    throw RuntimeError("Unsupported dataType '%s'", dataType.c_str());
}

Value ObjectGet(
    const Value&        objectHandle,
    const Value&        name,
    const InputContext* context)
{
    IArrayConstSP objects = objectHandle.getArray(true);

    size_t numObjects = objects->size();
    if (numObjects == 0)
        throw RuntimeError("%s: No objects provided", __FUNCTION__);
    else if (numObjects > 1)
        throw RuntimeError("%s: Array of objects provided of size %d",
            __FUNCTION__, (int)numObjects);

    ObjectConstSP object  = context->ValueToObject(objects->getItem(0));
    MapConstSP    objMap  = object->get_public_map();
    IArrayConstSP names   = name.getArray(true); // true => allowScalar or undefined
    size_t nbNames = names->size();

    if (nbNames == 0)
    {
        return ObjectMapGet(objMap, "");
    }
    else if (nbNames == 1)
    {
        return ObjectMapGet(objMap, context->ValueToString(names->getItem(0), true));
    }

    std::vector<Value> values;
    for (size_t i = 0; i < nbNames; ++i)
    {
        values.push_back(ObjectMapGet(objMap,
            context->ValueToString(names->getItem(i), true)));
    }
    return Value(values);
}

Value ObjectCount(
    const Value&        className,
    const InputContext* context)
{
    int count = ObjectHandleCount(
        context->ValueToString(className, true));

    return Value(count);
}

Value ObjectFree(
    const Value&        handle,
    const InputContext* context)
{
    try
    {
        ObjectHandleFree(context->ValueToString(handle));
        return Value(true);
    }
    catch (std::exception& e)
    {
        return Value(e);
    }
}

Value ObjectFreeAll()
{
    int count = ObjectHandleFreeAll();
    return Value(count);
}

Value ObjectList(
    const Value&        prefix,
    const Value&        className,
    const InputContext* context)
{
    std::vector<std::string> handles = ObjectHandleList(
        context->ValueToString(prefix, true),
        context->ValueToString(className, true));

    return Value(handles);
}

Value ObjectClassName(
    const Value&        handle,
    const InputContext* context)
{
    std::string className = ObjectHandleClassName(
        context->ValueToString(handle));

    return Value(className);
}

Value ObjectCoerce(
    const ServiceSP&    service,
    const Value&        in_className,
    const Value&        value,
    const InputContext* context)
{
    std::string className = context->ValueToString(in_className);

    if (value.isUndefined())
        return Value(ObjectConstSP());

    if (value.getType() == Value::ARRAY)
    {
        IArrayConstSP values = value.getArray();

        // if input is pseudo-scalar try the scalar first
        if (values->size() == 1)
        {
            ObjectConstSP object = service->object_coerce(
                className, values->getItem(0), context);

            if (object)
                return Value(object);
        }
    }

    ObjectConstSP object = service->object_coerce(className, value, context);
    if (object)
        return Value(object);

    throw RuntimeError("Could not coerce to %s", className.c_str());
}

Value ObjectPut(
    const Value& objectHandle,
    const Value& namesValue,
    const Value& v1,
    const Value& v2,
    const Value& v3,
    const Value& v4,
    const Value& v5,
    const Value& v6,
    const Value& v7,
    const Value& v8,
    const Value& v9,
    const Value& v10,
    const Value& v11,
    const Value& v12,
    const Value& v13,
    const Value& v14,
    const Value& v15,
    const Value& v16,
    const Value& v17,
    const Value& v18,
    const Value& v19,
    const Value& v20,
    const Value& v21,
    const Value& v22,
    const Value& v23,
    const Value& v24,
    const Value& v25,
    const InputContext* context)
{
    ObjectConstSP object = context->ValueToObject(objectHandle, NULL, true);

    Value values[25] = {v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14,
        v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25};

    std::vector<std::string> names = namesValue.getStringVector(true);

    size_t numNames = names.size();

    if (numNames > 25)
        throw RuntimeError("More than 25 names provided");

    std::vector<std::string> newNames;
    std::vector<Value> newValues;

    for (size_t i = 0; i < numNames; ++i)
    {
        if (names[i].length() == 0)
        {
            if (!values[i].isUndefined())
                throw RuntimeError("No name for v%d", (int)i+1);
            continue;
        }

        newNames.push_back(names[i]);
        newValues.push_back(values[i]);
    }

    ObjectConstSP modifiedObject = ObjectPut(object, newNames, newValues, context);
    return Value(modifiedObject);
}

Value ObjectPutMetaData(
    const Value& objectHandle,
    const Value& namesValue,
    const Value& v1,
    const Value& v2,
    const Value& v3,
    const Value& v4,
    const Value& v5,
    const Value& v6,
    const Value& v7,
    const Value& v8,
    const Value& v9,
    const Value& v10,
    const Value& v11,
    const Value& v12,
    const Value& v13,
    const Value& v14,
    const Value& v15,
    const Value& v16,
    const Value& v17,
    const Value& v18,
    const Value& v19,
    const Value& v20,
    const Value& v21,
    const Value& v22,
    const Value& v23,
    const Value& v24,
    const Value& v25,
    const InputContext* context)
{
    // Since ObjectPutMetaData (below) amends object (by modified the
    // non-const meta_data field) we have to copy the object before we start.
    // Otherwise excel gets very confused.
    ObjectConstSP object = ObjectCopy(objectHandle, context).getObject();

    Value values[25] = { v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14,
        v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25 };

    std::vector<std::string> names = namesValue.getStringVector(true);

    size_t numNames = names.size();

    if (numNames > 25)
        throw RuntimeError("More than 25 names provided");

    std::vector<std::string> newNames;
    std::vector<Value> newValues;

    for (size_t i = 0; i < numNames; ++i)
    {
        if (names[i].length() == 0)
        {
            if (!values[i].isUndefined())
                throw RuntimeError("No name for v%d", (int)i + 1);
            continue;
        }

        newNames.push_back(names[i]);
        newValues.push_back(values[i]);
    }

    ObjectPutMetaData(object, newNames, newValues, context);
    return Value(object);
}

Value ObjectToMap(
    const Value& handle,
    const InputContext* context)
{
    ObjectConstSP object    = context->ValueToObject(handle);
    ObjectConstSP mapObject = MapObject::Make(object->get_attributes());
    return Value(mapObject);
}

Value ObjectCopy(
    const Value& handle,
    const InputContext* context,
    bool deepCopy)
{
    ObjectConstSP object = context->ValueToObject(handle);
    if (!object)
        SPI_THROW_RUNTIME_ERROR("Null object");

    ObjectConstSP copy;
    ServiceSP svc = Service::CommonService();

    if (deepCopy)
    {
        // we don't have deepcopy of the map
        // hence we serialize/de-serialize instead
        std::string str = object->to_string();
        copy = svc->object_from_string(str);
    }
    else
    {
        MapConstSP m = object->get_attributes();
        ObjectMap om(m);

        copy = svc->object_from_map(&om, ObjectRefCacheSP());
    }

    // the copy must also copy the meta data which is lost at this stage
    copy->update_meta_data(object->get_meta_data());

    if (!copy)
        SPI_THROW_RUNTIME_ERROR("Could not copy object");

    return Value(copy);
}

Value ObjectSHA(const Value& handle, const Value& version, const InputContext* context)
{
    ObjectConstSP object = context->ValueToObject(handle);
    int shaVersion = context->ValueToInt(version, true, 1);
    std::string sha;

    if (shaVersion == 1)
    {
        sha = SHA1(object);
    }
    else if (shaVersion == 256)
    {
        sha = SHA256(object);
    }
    else
    {
        SPI_THROW_RUNTIME_ERROR("version " << shaVersion << " should be 1, 256 or undefined");
    }

    return Value(sha);
}

SPI_END_NAMESPACE
