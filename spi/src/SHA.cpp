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

#include "SHA.hpp"

#include "ObjectMap.hpp" // used by SHA1 function

#undef SPI_UTIL_CLOCK_EVENTS
#include <spi_util/ClockUtil.hpp>
#include <spi_util/SHA1.hpp>
#include <spi_util/SHA256.hpp>
#include <spi_util/StringUtil.hpp>
#include <spi_util/Utils.hpp>

#include <algorithm>

SPI_BEGIN_NAMESPACE

namespace
{
    void StringToSHA(
        spi_util::SHA& sha,
        const std::string& str)
    {
        sha.Update((unsigned char*)str.c_str(), 
            spi_util::IntegerCast<uint32_t>(str.length()));
    }

    void StringToSHA(
        spi_util::SHA& sha,
        const char* str)
    {
        if (!str)
        {
            sha.Update((unsigned char*)str, 
                spi_util::IntegerCast<uint32_t>(strlen(str)));
        }
    }

    template<typename T>
    void NumberToSHA(
        spi_util::SHA& sha,
        T number)
    {
        sha.Update((unsigned char*)&number, sizeof(number));
    }

    void CharToSHA(
        spi_util::SHA& sha,
        char c)
    {
        sha.Update((unsigned char*)&c, 1);
    }

    void ValueToSHA(
        spi_util::SHA& sha,
        const Value& value);

    void MapToSHA(
        spi_util::SHA& sha,
        const MapConstSP& m)
    {
        const Map* mp = m.get();
        if (!mp)
        {
            StringToSHA(sha, "NULL");
        }
        else
        {
            const std::string& className = mp->ClassName();
            if (!className.empty())
            {
                StringToSHA(sha, className);
            }

            const std::vector<std::string>& fieldNames = mp->FieldNames();
            size_t nbFields = fieldNames.size();
            for (size_t i = 0; i < nbFields; ++i)
            {
                const std::string& name = fieldNames[i];
                const Value& value = mp->GetValue(name);
                StringToSHA(sha, name);
                ValueToSHA(sha, value);
            }
        }
    }

    void ObjectToSHA(
        spi_util::SHA& sha,
        const ObjectConstSP& obj)
    {
        if (!obj)
        {
            StringToSHA(sha, "NULL");
        }
        else
        {
            MapSP aMap(new Map(obj->get_class_name()));
            ObjectMap om(aMap);
            obj->to_map(&om, false);

            MapToSHA(sha, aMap);
        }
    }

    void ArrayToSHA(
        spi_util::SHA& sha,
        const IArray* array)
    {
        const std::vector<size_t> dimensions = array->dimensions();
        size_t numDims = dimensions.size();
        size_t size = array->size();

        SPI_POST_CONDITION(numDims >= 1);

        for (size_t i = 0; i < numDims; ++i)
        {
            NumberToSHA(sha, dimensions[i]);
        }

        for (size_t i = 0; i < size; ++i)
        {
            const Value& value = array->getItem(i);
            ValueToSHA(sha, value);
        }
    }

    void ValueToSHA(
        spi_util::SHA& sha,
        const Value& value)
    {
        switch (value.getType())
        {
        case Value::UNDEFINED:
            throw RuntimeError("PROGRAM BUG");
        case Value::CHAR:
            CharToSHA(sha, value.getChar());
            break;
        case Value::SHORT_STRING:
        case Value::STRING:
            StringToSHA(sha, value.getString());
            break;
        case Value::INT:
            NumberToSHA(sha, value.getInt());
            break;
        case Value::DOUBLE:
            NumberToSHA(sha, value.getDouble());
            break;
        case Value::BOOL:
            NumberToSHA(sha, value.getBool() ? 1 : 0);
            break;
        case Value::DATE:
            NumberToSHA(sha, int(value.getDate()));
            break;
        case Value::DATETIME:
            NumberToSHA(sha, double(value.getDateTime()));
            break;
        case Value::MAP:
            MapToSHA(sha, value.getMap());
            break;
        case Value::OBJECT:
            ObjectToSHA(sha, value.getObject());
            break;
        case Value::OBJECT_REF:
            SPI_THROW_RUNTIME_ERROR("Object references not supported in SHA");
            break;
        case Value::ARRAY:
            ArrayToSHA(sha, value.getArray().get());
            break;
        case Value::ERROR:
            StringToSHA(sha, "ERROR:");
            StringToSHA(sha, value.getError());
            break;
        default:
            SPI_THROW_RUNTIME_ERROR("Cannot convert value of type " <<
                Value::TypeToString(value.getType()) << " to SHA encoding");
        }
    }
}

std::string SHA1(const MapConstSP& m)
{
    spi_util::SHA1 sha1;

    MapToSHA(sha1, m);
    return sha1.Final();
}


std::string SHA1(const ObjectConstSP& obj)
{
    if (!obj)
        return std::string();

    spi_util::SHA1 sha1;

    ObjectToSHA(sha1, obj);
    return sha1.Final();
}

std::string SHA256(const MapConstSP& m)
{
    spi_util::SHA256 sha256;

    MapToSHA(sha256, m);
    return sha256.Final();
}


std::string SHA256(const ObjectConstSP& obj)
{
    if (!obj)
        return std::string();

    spi_util::SHA256 sha256;

    ObjectToSHA(sha256, obj);
    return sha256.Final();
}

SPI_END_NAMESPACE

#if 0
/*

Copyright (C) 2012-2021 Sartorial Programming Ltd.

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include "SHA256.hpp"

#include "ObjectMap.hpp" // used by SHA256 function

#undef SPI_UTIL_CLOCK_EVENTS
#include <spi_util/ClockUtil.hpp>
#include <spi_util/SHA256.hpp>
#include <spi_util/StringUtil.hpp>

#include <algorithm>

SPI_BEGIN_NAMESPACE

namespace
{
    void StringToSHA256(
        spi_util::SHA256& sha,
        const std::string& str)
    {
        sha.Update((const unsigned char*)str.c_str(), str.length() + 1);
    }

    void StringToSHA256(
        spi_util::SHA256& sha,
        const char* str)
    {
        if (!str)
        {
            sha.Update((const unsigned char*)str, strlen(str) + 1);
        }
    }

    template<typename T>
    void NumberToSHA256(
        spi_util::SHA256& sha,
        T number)
    {
        sha.Update((const unsigned char*)&number, sizeof(number));
    }

    void CharToSHA256(
        spi_util::SHA256& sha,
        char c)
    {
        sha.Update((const unsigned char*)&c, 1);
    }

    void ValueToSHA256(
        spi_util::SHA256& sha,
        const Value& value);

    void MapToSHA256(
        spi_util::SHA256& sha,
        const MapConstSP& m)
    {
        const Map* mp = m.get();
        if (!mp)
        {
            StringToSHA256(sha, "NULL");
        }
        else
        {
            const std::string& className = mp->ClassName();
            if (!className.empty())
            {
                StringToSHA256(sha, className);
            }

            const std::vector<std::string>& fieldNames = mp->FieldNames();
            size_t nbFields = fieldNames.size();
            for (size_t i = 0; i < nbFields; ++i)
            {
                const std::string& name = fieldNames[i];
                const Value& value = mp->GetValue(name);
                StringToSHA256(sha, name);
                ValueToSHA256(sha, value);
            }
        }
    }

    void ObjectToSHA256(
        spi_util::SHA256& sha,
        const ObjectConstSP& obj)
    {
        if (!obj)
        {
            StringToSHA256(sha, "NULL");
        }
        else
        {
            MapSP aMap(new Map(obj->get_class_name()));
            ObjectMap om(aMap);
            obj->to_map(&om, false);

            MapToSHA256(sha, aMap);
        }
    }

    void ArrayToSHA256(
        spi_util::SHA256& sha,
        const IArray* array)
    {
        const std::vector<size_t> dimensions = array->dimensions();
        size_t numDims = dimensions.size();
        size_t size = array->size();

        SPI_POST_CONDITION(numDims >= 1);

        for (size_t i = 0; i < numDims; ++i)
        {
            NumberToSHA256(sha, dimensions[i]);
        }

        for (size_t i = 0; i < size; ++i)
        {
            const Value& value = array->getItem(i);
            ValueToSHA256(sha, value);
        }
    }

    void ValueToSHA256(
        spi_util::SHA256& sha,
        const Value& value)
    {
        switch (value.getType())
        {
        case Value::UNDEFINED:
            throw RuntimeError("PROGRAM BUG");
        case Value::CHAR:
            CharToSHA256(sha, value.getChar());
            break;
        case Value::SHORT_STRING:
        case Value::STRING:
            StringToSHA256(sha, value.getString());
            break;
        case Value::INT:
            NumberToSHA256(sha, value.getInt());
            break;
        case Value::DOUBLE:
            NumberToSHA256(sha, value.getDouble());
            break;
        case Value::BOOL:
            NumberToSHA256(sha, value.getBool() ? 1 : 0);
            break;
        case Value::DATE:
            NumberToSHA256(sha, int(value.getDate()));
            break;
        case Value::DATETIME:
            NumberToSHA256(sha, double(value.getDateTime()));
            break;
        case Value::MAP:
            MapToSHA256(sha, value.getMap());
            break;
        case Value::OBJECT:
            ObjectToSHA256(sha, value.getObject());
            break;
        case Value::OBJECT_REF:
            SPI_THROW_RUNTIME_ERROR("Object references not supported in SHA256");
            break;
        case Value::ARRAY:
            ArrayToSHA256(sha, value.getArray().get());
            break;
        case Value::ERROR:
            StringToSHA256(sha, "ERROR:");
            StringToSHA256(sha, value.getError());
            break;
        default:
            SPI_THROW_RUNTIME_ERROR("Cannot convert value of type " <<
                Value::TypeToString(value.getType()) << " to SHA256 encoding");
        }
    }

}

std::string SHA256(const MapConstSP& m)
{
    spi_util::SHA256 sha256;

    MapToSHA256(sha256, m);

    return sha256.Final();
}


std::string SHA256(const ObjectConstSP& obj)
{
    if (!obj)
        return std::string();

    spi_util::SHA256 sha256;

    ObjectToSHA256(sha256, obj);

    return sha256.Final();
}

SPI_END_NAMESPACE

#endif
