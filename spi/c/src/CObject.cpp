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

#include "CObject.h"

#include "CString.h"
#include "Error.h"

#include "Helper.hpp"
#include <spi/ObjectHandle.hpp>

/*
**************************************************************************
* Implementation of spi_Object functions
**************************************************************************
*/
void spi_Object_delete(spi_Object* item)
{
    SPI_C_LOCK_GUARD;
    intrusive_ptr_release((spi::Object*)item);
}

int spi_Object_get_object_id(spi_Object* item, char** objectId)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!item || !objectId)
            SPI_THROW_RUNTIME_ERROR("Null inputs");

        const std::string& i_objectId = ((const spi::Object*)item)->get_object_id();

        *objectId = spi_String_copy(i_objectId.c_str());
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Object_get_class_name(spi_Object* item, char** className)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!item || !className)
            SPI_THROW_RUNTIME_ERROR("Null inputs");

        const char* i_className = ((const spi::Object*)item)->get_class_name();

        *className = spi_String_copy(i_className);
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Object_to_string(
    spi_Object* self,
    const char* format,
    const char* options,
    char** str)
{
    SPI_C_LOCK_GUARD;
    if (!self || !str)
    {
        spi_Error_set_function(__FUNCTION__, "NULL inputs");
        return -1;
    }

    try
    {
        std::string cstr = ((spi::Object*)self)->to_string(format, options);
        *str = spi_String_copy(cstr.c_str());
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Object_to_file(
    spi_Object* self,
    const char* filename,
    const char* format,
    const char* options)
{
    SPI_C_LOCK_GUARD;
    if (!self || !filename)
    {
        spi_Error_set_function(__FUNCTION__, "NULL inputs");
        return -1;
    }

    try
    {
        ((spi::Object*)self)->to_file(filename, format, options);
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

spi_Object* spi_Object_from_string(const char* str)
{
    SPI_C_LOCK_GUARD;
    if (!str)
    {
        spi_Error_set_function(__FUNCTION__, "NULL inputs");
        return nullptr;
    }

    try
    {
        spi::ServiceSP commonService = spi::Service::CommonService();
        spi::ObjectConstSP obj = commonService->object_from_string(std::string(str));

        return spi::convert_out<spi_Object>(obj);
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return nullptr;
    }
}

spi_Object* spi_Object_from_file(const char* filename)
{
    SPI_C_LOCK_GUARD;
    if (!filename)
    {
        spi_Error_set_function(__FUNCTION__, "NULL inputs");
        return nullptr;
    }

    try
    {
        spi::ServiceSP commonService = spi::Service::CommonService();
        spi::ObjectConstSP obj = commonService->object_from_file(std::string(filename));

        return spi::convert_out<spi_Object>(obj);
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return nullptr;
    }
}

spi_Variant* spi_Object_get_value(spi_Object* self, const char * name)
{
    SPI_C_LOCK_GUARD;
    try
    {
        spi::Value value = spi::ObjectGet(((spi::Object*)self), name);
        return (spi_Variant*)(new spi::Variant(value));
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return nullptr;
    }
}

int spi_Object_handle_save(
    const char* baseName,
    spi_Object* obj,
    System_Bool noCount,
    char** handle)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!baseName || !obj || !handle)
            SPI_THROW_RUNTIME_ERROR("Null inputs");

        const std::string& cHandle = spi::ObjectHandleSave(
            baseName,
            (spi::Object*)obj,
            noCount == SYSTEM_TRUE);

        *handle = spi_String_copy(cHandle.c_str());
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Object_handle_find(
    const char* handle,
    spi_Object** obj)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!handle || !obj)
            SPI_THROW_RUNTIME_ERROR("Null inputs");

        *obj = spi::convert_out<spi_Object>(spi::ObjectHandleFind(handle));
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Object_handle_free_all(int* count)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!count)
            SPI_THROW_RUNTIME_ERROR("Null inputs");
        
        *count = spi::ObjectHandleFreeAll();
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Object_handle_count(
    const char* className,
    int* count)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!className || !count)
            SPI_THROW_RUNTIME_ERROR("Null inputs");

        *count = spi::ObjectHandleCount(className);
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Object_handle_free(const char* handle)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!handle)
            SPI_THROW_RUNTIME_ERROR("Null inputs");

        spi::ObjectHandleFree(handle);
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }

}

int spi_Object_handle_list(
    const char* baseName,
    const char* className,
    spi_String_Vector** handles)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!baseName || !className || !handles)
            SPI_THROW_RUNTIME_ERROR("Null inputs");

        *handles = (spi_String_Vector*)(new std::vector<std::string>(
            spi::ObjectHandleList(baseName, className)));

        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Object_handle_class_name(const char* handle, char** className)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!handle || !className)
            SPI_THROW_RUNTIME_ERROR("Null inputs");

        std::string cClassName = spi::ObjectHandleClassName(handle);
        *className = spi_String_copy(cClassName.c_str());

        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

void spi_Object_Vector_delete(spi_Object_Vector* v)
{
    SPI_C_LOCK_GUARD;
    spi::Vector_delete<spi::Object>(v);
}

spi_Object_Vector* spi_Object_Vector_new(int N)
{
    SPI_C_LOCK_GUARD;
    return spi::Vector_new<spi::Object, spi_Object_Vector>(N);
}

int spi_Object_Vector_item(
    const spi_Object_Vector* v,
    int i,
    spi_Object** item)
{
    SPI_C_LOCK_GUARD;
    return spi::Vector_item<spi::Object>(v, i, item);
}

int spi_Object_Vector_set_item(
    spi_Object_Vector* v,
    int i,
    spi_Object* item)
{
    SPI_C_LOCK_GUARD;
    return spi::Vector_set_item<spi::Object>(v, i, item);
}

int spi_Object_Vector_size(
    const spi_Object_Vector* v,
    int* size)
{
    SPI_C_LOCK_GUARD;
    return spi::Vector_size<spi::Object>(v, size);
}

void spi_Object_Matrix_delete(spi_Object_Matrix* m)
{
    SPI_C_LOCK_GUARD;
    spi::Matrix_delete<spi::Object>(m);
}

spi_Object_Matrix* spi_Object_Matrix_new(int nr, int nc)
{
    SPI_C_LOCK_GUARD;
    return spi::Matrix_new<spi::Object, spi_Object_Matrix>(nr, nc);
}

int spi_Object_Matrix_item(
    const spi_Object_Matrix* m,
    int i, int j,
    spi_Object** item)
{
    SPI_C_LOCK_GUARD;
    return spi::Matrix_item<spi::Object>(m, i, j, item);
}

int spi_Object_Matrix_set_item(
    spi_Object_Matrix* m,
    int i, int j,
    spi_Object* item)
{
    SPI_C_LOCK_GUARD;
    return spi::Matrix_set_item<spi::Object>(m, i, j, item);
}

int spi_Object_Matrix_size(
    const spi_Object_Matrix* m,
    int* nr, int* nc)
{
    SPI_C_LOCK_GUARD;
    return spi::Matrix_size<spi::Object>(m, nr, nc);
}

