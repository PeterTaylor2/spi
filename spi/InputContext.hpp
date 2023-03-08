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
#ifndef SPI_INPUT_CONTEXT_HPP
#define SPI_INPUT_CONTEXT_HPP

/**
 * The goal is to enable an arbitrary platform to have all of its functions
 * implemented as follows (using Excel by example):
 *

xloper* xlFunc(xloper* a1, xloper* a2, xloper* a3)
{
    static FuncDef* func = 0;
    if (!func)
        func = FindFunc("Func");

    InputValues iv = ExcelInputValues(func, a1, a2, a3);
    Value ov = CallInContext(func, iv, ExcelContext());
    return ExcelOutputValue(ov);
}

 *
 * In this example, ExcelInputValues is responsible for converting from
 * xloper* to Value. This may result in an InputValues object which has
 * permuted inputs.
 *
 * Then CallInContext will be responsible for looping through the
 * permutations, and for each permutation will handle the late-binding of
 * the Value to some given type. Thus the FuncDef must have a method for
 * calling the function from a vector of Value of the correct type.
 *
 * Finally CallInContext will return an output as Value which then needs
 * to be converted to an xloper using ExcelOutputValue.
 *
 * Where are all these objects?
 *
 * FuncDef is defined within the DLL. There will be a look-up by name.
 * ExcelInputValues will be a generic function within spxl.lib.
 * CallInContext will be a generic function within spi.dll.
 * ExcelOutputValue will be a generic function within spxl.lib.
 *
 * Note that there are three types of function that we need to support.
 *
 * 1. Regular function.
 * 2. Class constructor.
 * 3. Class method.
 *
 * Class constructors will not support permuted arguments.
 * Regular functions will support permuted arguments.
 * Class methods will not allow the class instance to be permuted.
 */

#include "Value.hpp"
#include "Object.hpp"
#include "Variant.hpp"

SPI_BEGIN_NAMESPACE

SPI_DECLARE_RC_CLASS(Service);
class ObjectType;
template <typename T> class MatrixData;

/**
 * The InputContext class provides for late binding of inputs after they
 * have been first converted to spi::Value.
 */
class SPI_IMPORT InputContext
{
public:
    static InputContext* Find(const std::string& name);
    static InputContext* ExcelContext();
    static InputContext* PythonContext();
    static InputContext* NoContext();

    virtual ~InputContext() = 0;
    virtual const char* Context() const = 0;
    virtual bool ValueToBool(
        const Value& value,
        bool optional=false,
        bool defaultValue=false) const = 0;
    virtual char ValueToChar(
        const Value& value,
        bool optional=false,
        char defaultValue=' ') const = 0;
    virtual int ValueToInt(const Value& value,
        bool optional=false,
        int defaultValue=0) const = 0;
    virtual double ValueToDouble(const Value& value,
        bool optional=false,
        double defaultValue=0.0) const = 0;
    virtual std::string ValueToString(
        const Value& value,
        bool optional=false,
        const char* defaultValue="") const = 0;
    virtual Date ValueToDate(
        const Value& value,
        bool optional=false,
        Date defaultValue=Date()) const = 0;
    virtual DateTime ValueToDateTime(
        const Value& value,
        bool optional=false,
        DateTime defaultValue=DateTime()) const = 0;
    virtual ObjectConstSP ValueToObject(
        const Value& value,
        ObjectType* objectType=0,
        bool optional=false) const = 0;
    Variant ValueToVariant(
        const Value& value,
        bool optional=false) const;

    template<class T>
    typename T::outer_type ValueToInstance(
        const Value& value,
        bool optional=false) const
    {
        ObjectConstSP obj = ValueToObject(value, &T::object_type, optional);
        return T::Coerce(obj);
    }

	virtual bool AcceptScalarForArray() const = 0;
    virtual bool StripArray() const = 0;

    std::vector<bool> ValueToBoolVector(const Value& value,
        bool optional=false,
        bool defaultValue=false) const;
    std::vector<int> ValueToIntVector(const Value& value,
        bool optional=false,
        int defaultValue=0) const;
    std::vector<double> ValueToDoubleVector(const Value& value,
        bool optional=false,
        double defaultValue=0.0) const;
    std::vector<std::string> ValueToStringVector(const Value& value,
        bool optional=false,
        const char* defaultValue="") const;
    std::vector<Date> ValueToDateVector(const Value& value,
        bool optional=false,
        Date defaultValue=Date()) const;
    std::vector<DateTime> ValueToDateTimeVector(const Value& value,
        bool optional=false,
        DateTime defaultValue=DateTime()) const;
    std::vector<ObjectConstSP> ValueToObjectVector(
        const Value& value,
        ObjectType* objectType=0,
        bool optional=false) const;
    std::vector<Variant> ValueToVariantVector(const Value& value) const;

    template<class T>
    std::vector< typename T::outer_type >
    ValueToInstanceVector(const Value& value, bool optional=false) const
    {
        const std::vector<ObjectConstSP>& objects = ValueToObjectVector(
            value, &T::object_type, optional);
        return T::CoerceVector(objects);
    }

    MatrixData<bool> ValueToBoolMatrix(
        const Value& value,
        bool optional=false) const;

    MatrixData<int> ValueToIntMatrix(
        const Value& value,
        bool optional=false) const;

    MatrixData<double> ValueToDoubleMatrix(
        const Value& value,
        bool optional=false) const;

    MatrixData<std::string> ValueToStringMatrix(
        const Value& value,
        bool optional=false) const;

    MatrixData<Date> ValueToDateMatrix(
        const Value& value,
        bool optional=false) const;

    MatrixData<DateTime> ValueToDateTimeMatrix(
        const Value& value,
        bool optional=false) const;

    spi::MatrixData<Variant> ValueToVariantMatrix(
        const Value& value,
        bool optional=false) const;

    MatrixData<ObjectConstSP> ValueToObjectMatrix(
        const Value& value,
        ObjectType* objectType=0,
        bool optional=false) const;

    template<class T>
    MatrixData< typename T::outer_type >
    ValueToInstanceMatrix(const Value& value, bool optional=false) const
    {
        const MatrixData<ObjectConstSP>& objects = ValueToObjectMatrix(
            value, &T::object_type, optional);
        return T::CoerceMatrix(objects);
    }


private:
    std::vector<Value> ValueToVector(const Value& value) const;
};

class SPI_IMPORT NoInputContext : public InputContext
{
public:
    NoInputContext();

    const char* Context() const;

    bool ValueToBool(
        const Value& value,
        bool optional,
        bool defaultValue) const;
    char ValueToChar(
        const Value& value,
        bool optional,
        char defaultValue) const;
    int ValueToInt(const Value& value,
        bool optional,
        int defaultValue) const;
    double ValueToDouble(const Value& value,
        bool optional,
        double defaultValue) const;
    std::string ValueToString(
        const Value& value,
        bool optional,
        const char* defaultValue) const;
    Date ValueToDate(
        const Value& value,
        bool optional,
        Date defaultValue) const;
    DateTime ValueToDateTime(
        const Value& value,
        bool optional,
        DateTime defaultValue) const;
    ObjectConstSP ValueToObject(
        const Value& value,
        ObjectType* objectType,
        bool optional) const;
    bool AcceptScalarForArray() const;
    bool StripArray() const;
};

/*
*********************************************************************
* Various generic functions that can be used from any InputContext
* which has converted its inputs to Value.
*********************************************************************
*/
SPI_IMPORT
Value StartLogging(
    const ServiceSP&    service,
    const Value&        filename,
    const Value&        options,
    const InputContext* context);

SPI_IMPORT
Value StopLogging(const ServiceSP& service);

SPI_IMPORT
Value IsLogging(const ServiceSP& service);

SPI_IMPORT
Value HelpFunc(
    const ServiceSP& service,
    const Value&     name,
    const InputContext* context);

SPI_IMPORT
Value HelpEnum(
    const ServiceSP&    service,
    const Value&        name,
    const InputContext* context);

SPI_IMPORT
Value ObjectToString(
    const Value&        objectHandle,
    const Value&        format,
    const Value&        options,
    const Value&        metaDataHandle,
    const InputContext* context,
    bool splitString=false);

SPI_IMPORT
Value ObjectFromString(
    const ServiceSP&    service,
    const Value&        objectString,
    const InputContext* context,
    bool joinStrings=false);

SPI_IMPORT
Value ObjectToFile(
    const Value&        objectHandle,
    const Value&        fileName,
    const Value&        format,
    const Value&        options,
    const Value&        metaDataHandle,
    const InputContext* context);

SPI_IMPORT
Value ObjectFromFile(
    const ServiceSP&    service,
    const Value&        fileName,
    const InputContext* context);

SPI_IMPORT
Value ObjectFromURL(
    const ServiceSP& service,
    const Value& url,
    const Value& timeout,
    const Value& names,
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
    const InputContext* context);

SPI_IMPORT
Value ObjectGet(
    const Value&        objectHandle,
    const Value&        name,
    const InputContext* context);

SPI_IMPORT
Value ObjectPut(
    const Value& objectHandle,
    const Value& names,
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
    const InputContext* context);

SPI_IMPORT
Value ObjectCount(
    const Value&        className,
    const InputContext* context);

SPI_IMPORT
Value ObjectFree(
    const Value&        handle,
    const InputContext* context);

SPI_IMPORT
Value ObjectFreeAll();

SPI_IMPORT
Value ObjectList(
    const Value&        prefix,
    const Value&        className,
    const InputContext* context);

SPI_IMPORT
Value ObjectClassName(
    const Value&        handle,
    const InputContext* context);

SPI_IMPORT
Value ObjectCoerce(
    const ServiceSP&    service,
    const Value&        className,
    const Value&        value,
    const InputContext* context);

SPI_IMPORT
Value ObjectToMap(
    const Value& handle,
    const InputContext* context);

SPI_IMPORT
Value ObjectCopy(
    const Value& handle,
    const InputContext* context,
    bool deepCopy = false);

SPI_IMPORT
Value ObjectSHA(
    const Value& handle,
    const Value& version,
    const InputContext* context);

SPI_IMPORT
Value UrlCacheSize(
    const ServiceSP& service);

SPI_IMPORT
Value UrlCacheSave(
    const ServiceSP& service,
    const Value& filename,
    const InputContext* context);

SPI_IMPORT
Value UrlCacheLoad(
    const ServiceSP& service,
    const Value& filename,
    const InputContext* context);

SPI_IMPORT
Value UrlCacheInit(
    const ServiceSP& service);

SPI_IMPORT
Value UrlCacheClear(const ServiceSP& service);

SPI_END_NAMESPACE

#endif

