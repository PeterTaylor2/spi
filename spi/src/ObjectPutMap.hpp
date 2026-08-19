/*
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
** ObjectPutMap.hpp
***************************************************************************
** Defines ObjectPutMap which is a private class used in ObjectPut.
***************************************************************************
*/

#include "ObjectPut.hpp"

#include "../IObjectMap.hpp"

#include <set>

SPI_BEGIN_NAMESPACE

class ObjectPutMap : public IObjectMap
{
public:
    ObjectPutMap(
        IObjectMap* original,
        const std::vector<std::string>& names,
        const std::vector<Value>& values,
        const InputContext* context);

    const std::vector<std::string> Unused() const;

    void SetChar(
        const char* name,
        char value,
        bool hidden);

    void SetString(
        const char* name,
        const std::string& value,
        bool hidden);

    void SetInt(
        const char* name,
        int value,
        bool hidden);

    void SetBool(
        const char* name,
        bool value,
        bool hidden);

    void SetDouble(
        const char* name,
        double value,
        bool hidden);

    void SetDate(
        const char* name,
        Date value,
        bool hidden);

    void SetDateTime(
        const char* name,
        DateTime value,
        bool hidden);

    void SetObject(
        const char* name,
        const ObjectConstSP& value,
        bool hidden);

    void SetVariant(
        const char* name,
        const Variant& value,
        bool hidden);

    void SetStringVector(
        const char* name,
        const std::vector<std::string>& value,
        bool hidden);

    void SetDoubleVector(
        const char* name,
        const std::vector<double>& value,
        bool hidden);

    void SetIntVector(
        const char* name,
        const std::vector<int>& value,
        bool hidden);

    void SetBoolVector(
        const char* name,
        const std::vector<bool>& value,
        bool hidden);

    void SetDateVector(
        const char* name,
        const std::vector<Date>& value,
        bool hidden);

    void SetDateTimeVector(
        const char* name,
        const std::vector<DateTime>& value,
        bool hidden);

    void SetVariantVector(
        const char* name,
        const std::vector<Variant>& value,
        bool hidden);

    void SetObjectVector(
        const char* name,
        const std::vector<ObjectConstSP>& value,
        bool hidden);

    void SetBoolMatrix(
        const char* name,
        const MatrixData<bool>& value,
        bool hidden);

    void SetIntMatrix(
        const char* name,
        const MatrixData<int>& value,
        bool hidden);

    void SetDoubleMatrix(
        const char* name,
        const MatrixData<double>& value,
        bool hidden);

    void SetStringMatrix(
        const char* name,
        const MatrixData<std::string>& value,
        bool hidden);

    void SetDateMatrix(
        const char* name,
        const MatrixData<Date>& value,
        bool hidden);

    void SetDateTimeMatrix(
        const char* name,
        const MatrixData<DateTime>& value,
        bool hidden);

    void SetObjectMatrix(
        const char* name,
        const MatrixData<ObjectConstSP>& value,
        bool hidden);

    void SetVariantMatrix(
        const char* name,
        const spi::MatrixData<Variant>& value,
        bool hidden);

    void ImportMap(const Map* aMap);

    void SetClassName(const std::string& className);

    std::string ClassName() const override;

    char GetChar(
        const char* name,
        bool optional,
        char defaultValue);

    std::string GetString(
        const char* name,
        bool optional,
        const char* defaultValue);

    int GetInt(
        const char* name,
        bool optional,
        int defaultValue);

    bool GetBool(
        const char* name,
        bool optional,
        bool defaultValue);

    double GetDouble(
        const char* name,
        bool optional,
        double defaultValue);

    Date GetDate(
        const char* name,
        bool optional);

    DateTime GetDateTime(
        const char* name,
        bool optional);

    ObjectConstSP GetObject(
        const char* name,
        ObjectType* objectType,
        ValueToObject& mapToObject,
        bool optional);

    Variant GetVariant(
        const char* name,
        ValueToObject& mapToObject,
        bool optional);

    std::vector<std::string> GetStringVector(
        const char* name);

    std::vector<double> GetDoubleVector(
        const char* name);

    std::vector<int> GetIntVector(
        const char* name);

    std::vector<bool> GetBoolVector(
        const char* name);

    std::vector<Date> GetDateVector(
        const char* name);

    std::vector<DateTime> GetDateTimeVector(
        const char* name);

    std::vector<ObjectConstSP> GetObjectVector(
        const char* name,
        ObjectType* objectType,
        ValueToObject& mapToObject,
        bool optional);

    std::vector<Variant> GetVariantVector(
        const char* name,
        ValueToObject& mapToObject,
        bool optional);

    MatrixData<bool> GetBoolMatrix(
        const char* name);

    MatrixData<int> GetIntMatrix(
        const char* name);

    MatrixData<double> GetDoubleMatrix(
        const char* name);

    MatrixData<std::string> GetStringMatrix(
        const char* name);

    MatrixData<Date> GetDateMatrix(
        const char* name);

    MatrixData<DateTime> GetDateTimeMatrix(
        const char* name);

    MatrixData<ObjectConstSP> GetObjectMatrix(
        const char* name,
        ObjectType* objectType,
        ValueToObject& mapToObject,
        bool optional);

    MatrixData<Variant> GetVariantMatrix(
        const char* name,
        ValueToObject& mapToObject,
        bool optional);

    bool Exists(const char* name);

    MapSP ExportMap();

private:

    IObjectMap* original;
    const InputContext* context;
    std::map<std::string, Value> indexValues;
    std::set<std::string> unusedNames;
    std::vector<std::string> namesInOrder;

    bool ModifiedValue(const std::string& name, Value& value);
};

SPI_END_NAMESPACE
