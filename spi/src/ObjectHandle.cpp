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
** ObjectHandle.cpp
**
** Defines functions for mapping Object to string and back again.
**
** Although this is only going to be used by Excel, we keep the code at
** a higher level so that other platforms can read/write object handles
** if they are interacting with Excel at some other level.
***************************************************************************
*/

#include "ObjectHandle.hpp"

#include "Service.hpp"
#include "RuntimeError.hpp"
#include "StringUtil.hpp"

#include <map>

SPI_BEGIN_NAMESPACE

namespace
{
    /**
     * This is what we store in the INDEX_STRING_OBJECT.
     *
     * We store the count as well to ensure that when we ask for the object
     * the count matches (avoids the stale handle problem).
     */
    struct ObjectCount
    {
        ObjectConstSP object;
        int           count;

        ObjectCount()
            :
            object(),
            count(0)
        {}

        ObjectCount(
            const ObjectConstSP& object,
            int count)
            :
            object(object),
            count(count)
        {}
    };

    /*
     * This is a break-down of the full string into a base name (which
     * usually contains the cell name) and an integer part. The integer
     * part is the number of objects that have been created with that
     * base name.
     */
    struct StringHandle
    {
        std::string baseName;
        int         count;

        StringHandle()
            :
            baseName(),
            count(0)
        {}

        StringHandle(
            const std::string& baseName,
            int count)
            :
            baseName(baseName),
            count(count)
        {}

        /*
         * This constructor takes the full string and breaks it down into
         * baseName and integer. It is marked as explicit to prevent
         * accidental type coercion.
         */
        explicit StringHandle(const std::string& handle)
            :
            baseName(),
            count(0)
        {
            try
            {
                size_t pos = handle.find_last_of('#');
                if (pos == std::string::npos)
                {
                    baseName = handle;
                    count    = 0;
                }
                else
                {
                    baseName = handle.substr(0,pos);
                    count    = StringToInt(handle.substr(pos+1));
                    if (count <= 0)
                    {
                        throw RuntimeError("Count %d in handle %s must be positive", count, handle.c_str());
                    }
                }
            }
            catch (std::exception&)
            {
                throw RuntimeError("%s is not a valid object handle string",
                                   handle.c_str());
            }
        }

        std::string toString()
        {
            if (count > 0)
                return StringFormat("%s#%d", baseName.c_str(), count);
            return baseName;
        }
    };
}

static std::map<std::string, ObjectCount> INDEX_STRING_OBJECT;

/*
***************************************************************************
* Converts an object into a string object handle.
***************************************************************************
*/
std::string ObjectHandleSave (
    const std::string&   baseName,
    const ObjectConstSP& object,
    bool noCount)
{
    if (!object)
        return "NULL";

    StringHandle stringHandle(baseName, 0);

    std::map<std::string,ObjectCount>::iterator iter =
        INDEX_STRING_OBJECT.find(baseName);

    if (iter == INDEX_STRING_OBJECT.end())
    {
        /* first time we have seen an object with this name */
        int count = noCount ? 0 : 1;
        ObjectCount handle(object, count);
        stringHandle.count = count;
        INDEX_STRING_OBJECT[baseName] = handle;
    }
    else
    {
        /* we have already seen an object with this name */
        /* overwriting object will delete the previous object */
        iter->second.object = object;
        if (iter->second.count > 0)
            ++(iter->second.count);
        stringHandle.count = iter->second.count;
    }
    return stringHandle.toString();
}


/*
***************************************************************************
* Converts a string object handle into an object.
***************************************************************************
*/
ObjectConstSP ObjectHandleFind(
    const std::string& handle)
{
    /* this constructor will fail if the handle is not in the correct format */
    StringHandle stringHandle (handle);

    std::map<std::string,ObjectCount>::const_iterator iter =
        INDEX_STRING_OBJECT.find(stringHandle.baseName);

    if (iter == INDEX_STRING_OBJECT.end())
        throw RuntimeError("Object handle '%s' cannot be found",
                           stringHandle.baseName.c_str());

    if (iter->second.count != stringHandle.count)
    {
        throw RuntimeError("'%s' is a stale object handle - count %d should "
                           "be %d", handle.c_str(),
                           stringHandle.count, iter->second.count);
    }

    return iter->second.object;
}

int ObjectHandleFreeAll()
{
    int count = (int)INDEX_STRING_OBJECT.size();
    INDEX_STRING_OBJECT.clear();
    return count;
}

int ObjectHandleCount(const std::string& className)
{
    if (className.empty())
        return (int)INDEX_STRING_OBJECT.size();

    int count = 0;

    std::map<std::string,ObjectCount>::const_iterator iter =
        INDEX_STRING_OBJECT.begin();

    for(; iter != INDEX_STRING_OBJECT.end(); ++iter)
    {
        ObjectConstSP obj = iter->second.object;
        if (obj->get_class_name() == className)
            ++count;
    }
    return count;
}

void ObjectHandleFree(const std::string& handle)
{
    /* this constructor will fail if the handle is not in the correct format */
    StringHandle stringHandle (handle);

    std::map<std::string,ObjectCount>::iterator iter =
        INDEX_STRING_OBJECT.find(stringHandle.baseName);

    if (iter == INDEX_STRING_OBJECT.end())
        throw RuntimeError("Object handle '%s' cannot be found",
                           stringHandle.baseName.c_str());

    INDEX_STRING_OBJECT.erase(iter);
}

namespace {

    std::string ToString(
        const std::map<std::string,ObjectCount>::const_iterator& iter)
    {
        StringHandle stringHandle(iter->first, iter->second.count);
        return stringHandle.toString();
    }
}

std::vector<std::string> ObjectHandleList(
    const std::string& baseName,
    const std::string& className)
{
    std::vector<std::string> handles;
    if (baseName.empty())
    {
        // just iterate the entire map
        std::map<std::string,ObjectCount>::const_iterator iter =
            INDEX_STRING_OBJECT.begin();

        if (className.empty())
        {
            for (; iter != INDEX_STRING_OBJECT.end(); ++iter)
            {
                handles.push_back(ToString(iter));
            }
        }
        else
        {
            for (; iter != INDEX_STRING_OBJECT.end(); ++iter)
            {
                const ObjectConstSP& obj = iter->second.object;
                if (obj->get_class_name() == className)
                    handles.push_back(ToString(iter));
            }
        }
    }
    else
    {
        // find lower bound and then test the base name
        std::string nameHash = baseName + '#';
        std::map<std::string, ObjectCount>::const_iterator iter =
            INDEX_STRING_OBJECT.lower_bound(nameHash);

        if (className.empty())
        {
            for (; iter != INDEX_STRING_OBJECT.end(); ++iter)
            {
                if (!StringStartsWith(iter->first, nameHash))
                    break;

                handles.push_back(ToString(iter));
            }
        }
        else
        {
            for (; iter != INDEX_STRING_OBJECT.end(); ++iter)
            {
                if (!StringStartsWith(iter->first, nameHash))
                    break;

                const ObjectConstSP& obj = iter->second.object;
                if (obj->get_class_name() == className)
                    handles.push_back(ToString(iter));
            }
        }
    }

    // FIXME: ugly method of dealing with Excel nuisance behaviour
    while (handles.size() < 2)
        handles.push_back(std::string());

    return handles;
}

std::string ObjectHandleClassName(
    const std::string& handle)
{
    if (IsNullHandle(handle))
        return std::string();

    const ObjectConstSP& obj = ObjectHandleFind(handle);
    if (!obj) // probably doesn't happen
        return std::string();

    return obj->get_class_name();
}

bool IsNullHandle(const std::string& handle)
{
    size_t len = handle.size();
    if (len == 0)
        return true;
    if (len == 4)
    {
        if (StringUpper(handle) == "NULL")
            return true;
    }
    return false;
}

SPI_END_NAMESPACE

