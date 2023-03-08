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
#ifndef SPI_MAP_HPP
#define SPI_MAP_HPP

/*
***************************************************************************
** Map.hpp
***************************************************************************
** The Map class. This is a map of name/value pairs, with the order
** that the objects were inserted into the map preserved.
**
** The map also has a className (can be empty).
***************************************************************************
*/

#include "RefCounter.hpp"
#include "Value.hpp"
#include "Object.hpp"

#include <map>
#include <string>
#include <vector>
#include <unordered_map>

SPI_BEGIN_NAMESPACE

SPI_DECLARE_RC_CLASS(Map);

class SPI_IMPORT Map : public RefCounter
{
public:
    enum Type
    {
        NAMED,
        VARIANT,
        MATRIX
    };

    Map(const char* className, int ref=0, bool permissive=false);
    Map(Type type);
    ~Map();

    /**
     * Gets the class name for the map - corresponding to the class name of
     * the corresponding object. This can be empty for an anonymous map.
     */
    const std::string& ClassName() const;

    /**
     * Returns the field names in the order that they were added to the map.
     */
    const std::vector<std::string>& FieldNames() const;

    /**
     * Returns the field names with associated types in the order that they
     * were added to the map.
     */
    std::vector<std::string> FieldTypesAndNames() const;

    /**
     * Gets the value corresponding to name. If name is not in the map
     * then will return an UNDEFINED value.
     */
    Value GetValue(const std::string& name) const;

    /**
     * Unconditionally sets the class name overwriting any previous value.
     */
    void SetClassName(const std::string& className);

    /**
     * Sets the value for the given name.
     *
     * If the name is previously unused, then the name will be added
     * to the end of the field names unless you set excludeFromNames.
     *
     * If the name was previously used, then the previous value will be
     * overwritten, and the list of field names will not be changed.
     */
    void SetValue(const std::string& name, const Value& value, bool excludeFromNames=false);

    /**
     * Gets the reference number defined when the Map was constructed.
     */
    int GetRef() const;

    /**
     * Returns whether a field exists within the map.
     */
    bool Exists(const std::string& name) const;

    /**
     * Creates a one-level deep copy of the map.
     *
     * This means that the data values are shallow copied. Value class has
     * copy semantics, but when the Value is a complex class this usually
     * involves increasing a reference count.
     *
     * Currently there are no plans for DeepCopy.
     */
    MapSP Copy() const;

    /**
     * Create a copy of the map resolving all objects from maps and references.
     */
    MapSP Copy(ValueToObject& valueToObject) const;

    /**
     * Indicates whether the map is permissive - this means that
     * we can convert the fields to specific types later even if
     * they are not entered as that type initially.
     */
    bool Permissive() const;

    /**
     * Gets the type of the map - this is when it was created
     * using Map(Type type) without a name.
     */
    Type MapType() const;

    /**
     * Sets case insensitivity flag for all maps.
     * The normal behaviour is that maps are case sensitive.
     * Case insensitivity is a performance overhead.
     */
    static bool SetCaseInsensitive(bool caseInsensitive);

    /**
     * Returns the number of fields in the Map.
     */
    size_t NumFields() const;

private:

    typedef std::/*tr1::unordered_*/map<std::string, Value> DictType;

    DictType                    m_dict;
    std::vector<std::string>    m_fieldNames;
    std::string                 m_className;
    int                         m_ref;
    bool                        m_permissive;
    Type                        m_type;

    static bool g_caseInsensitive;

    Map();
    Map(const Map&);
    Map& operator=(const Map&);

};

SPI_END_NAMESPACE

#endif /* SPI_MAP_HPP */


