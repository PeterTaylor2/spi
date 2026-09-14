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
        bool hidden) override;

    void SetString(
        const char* name,
        const std::string& value,
        bool hidden) override;

    void SetInt(
        const char* name,
        int value,
        bool hidden) override;

    void SetBool(
        const char* name,
        bool value,
        bool hidden) override;

    void SetDouble(
        const char* name,
        double value,
        bool hidden) override;

    void SetDate(
        const char* name,
        Date value,
        bool hidden) override;

    void SetDateTime(
        const char* name,
        DateTime value,
        bool hidden) override;

    void SetObject(
        const char* name,
        const ObjectConstSP& value,
        bool hidden) override;

    void SetVariant(
        const char* name,
        const Variant& value,
        bool hidden) override;

    void SetStringVector(
        const char* name,
        const std::vector<std::string>& value,
        bool hidden) override;

    void SetDoubleVector(
        const char* name,
        const std::vector<double>& value,
        bool hidden) override;

    void SetIntVector(
        const char* name,
        const std::vector<int>& value,
        bool hidden) override;

    void SetBoolVector(
        const char* name,
        const std::vector<bool>& value,
        bool hidden) override;

    void SetDateVector(
        const char* name,
        const std::vector<Date>& value,
        bool hidden) override;

    void SetDateTimeVector(
        const char* name,
        const std::vector<DateTime>& value,
        bool hidden) override;

    void SetVariantVector(
        const char* name,
        const std::vector<Variant>& value,
        bool hidden) override;

    void SetObjectVector(
        const char* name,
        const std::vector<ObjectConstSP>& value,
        bool hidden) override;

    void SetBoolMatrix(
        const char* name,
        const MatrixData<bool>& value,
        bool hidden) override;

    void SetIntMatrix(
        const char* name,
        const MatrixData<int>& value,
        bool hidden) override;

    void SetDoubleMatrix(
        const char* name,
        const MatrixData<double>& value,
        bool hidden) override;

    void SetStringMatrix(
        const char* name,
        const MatrixData<std::string>& value,
        bool hidden) override;

    void SetDateMatrix(
        const char* name,
        const MatrixData<Date>& value,
        bool hidden) override;

    void SetDateTimeMatrix(
        const char* name,
        const MatrixData<DateTime>& value,
        bool hidden) override;

    void SetObjectMatrix(
        const char* name,
        const MatrixData<ObjectConstSP>& value,
        bool hidden) override;

    void SetVariantMatrix(
        const char* name,
        const spi::MatrixData<Variant>& value,
        bool hidden) override;

    void ImportMap(const Map* aMap) override;

    void SetClassName(const std::string& className) override;

    std::string ClassName() const override;

    char GetChar(
        const char* name,
        bool optional,
        char defaultValue) override;

    std::string GetString(
        const char* name,
        bool optional,
        const char* defaultValue) override;

    int GetInt(
        const char* name,
        bool optional,
        int defaultValue) override;

    bool GetBool(
        const char* name,
        bool optional,
        bool defaultValue) override;

    double GetDouble(
        const char* name,
        bool optional,
        double defaultValue) override;

    Date GetDate(
        const char* name,
        bool optional) override;

    DateTime GetDateTime(
        const char* name,
        bool optional) override;

    ObjectConstSP GetObject(
        const char* name,
        ObjectType* objectType,
        ValueToObject& mapToObject,
        bool optional) override;

    Variant GetVariant(
        const char* name,
        ValueToObject& mapToObject,
        bool optional) override;

    std::vector<std::string> GetStringVector(
        const char* name) override;

    std::vector<double> GetDoubleVector(
        const char* name) override;

    std::vector<int> GetIntVector(
        const char* name) override;

    std::vector<bool> GetBoolVector(
        const char* name) override;

    std::vector<Date> GetDateVector(
        const char* name) override;

    std::vector<DateTime> GetDateTimeVector(
        const char* name) override;

    std::vector<ObjectConstSP> GetObjectVector(
        const char* name,
        ObjectType* objectType,
        ValueToObject& mapToObject,
        bool optional) override;

    std::vector<Variant> GetVariantVector(
        const char* name,
        ValueToObject& mapToObject,
        bool optional) override;

    MatrixData<bool> GetBoolMatrix(
        const char* name) override;

    MatrixData<int> GetIntMatrix(
        const char* name) override;

    MatrixData<double> GetDoubleMatrix(
        const char* name) override;

    MatrixData<std::string> GetStringMatrix(
        const char* name) override;

    MatrixData<Date> GetDateMatrix(
        const char* name) override;

    MatrixData<DateTime> GetDateTimeMatrix(
        const char* name) override;

    MatrixData<ObjectConstSP> GetObjectMatrix(
        const char* name,
        ObjectType* objectType,
        ValueToObject& mapToObject,
        bool optional) override;

    MatrixData<Variant> GetVariantMatrix(
        const char* name,
        ValueToObject& mapToObject,
        bool optional) override;

    bool Exists(const char* name) override;

    MapSP ExportMap() override;

private:

    IObjectMap* original;
    const InputContext* context;
    std::map<std::string, Value> indexValues;
    std::set<std::string> unusedNames;
    std::vector<std::string> namesInOrder;

    bool ModifiedValue(const std::string& name, Value& value);
};

SPI_END_NAMESPACE
