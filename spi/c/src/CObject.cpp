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

/*
**************************************************************************
* Implementation of spi_Object functions
**************************************************************************
*/
void spi_Object_delete(spi_Object* item)
{
    intrusive_ptr_release((spi::Object*)item);
}

int spi_Object_to_string(
    spi_Object* self,
    const char* format,
    const char* options,
    char** str)
{
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

spi_Variant* spi_Object_get_value(spi_Object* self, const char * name)
{
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

void spi_Object_Vector_delete(spi_Object_Vector* v)
{
    spi::Vector_delete<spi::Object>(v);
}

spi_Object_Vector* spi_Object_Vector_new(int N)
{
    return spi::Vector_new<spi::Object, spi_Object_Vector>(N);
}

int spi_Object_Vector_item(
    spi_Object_Vector* v,
    int i,
    spi_Object** item)
{
    return spi::Vector_item<spi::Object>(v, i, item);
}

int spi_Object_Vector_set_item(
    spi_Object_Vector* v,
    int i,
    spi_Object* item)
{
    return spi::Vector_set_item<spi::Object>(v, i, item);
}

int spi_Object_Vector_size(
    spi_Object_Vector* v,
    int* size)
{
    return spi::Vector_size<spi::Object>(v, size);
}

void spi_Object_Matrix_delete(spi_Object_Matrix* m)
{
    spi::Matrix_delete<spi::Object>(m);
}

spi_Object_Matrix* spi_Object_Matrix_new(int nr, int nc)
{
    return spi::Matrix_new<spi::Object, spi_Object_Matrix>(nr, nc);
}

int spi_Object_Matrix_item(
    spi_Object_Matrix* m,
    int i, int j,
    spi_Object** item)
{
    return spi::Matrix_item<spi::Object>(m, i, j, item);
}

int spi_Object_Matrix_set_item(
    spi_Object_Matrix* m,
    int i, int j,
    spi_Object* item)
{
    return spi::Matrix_set_item<spi::Object>(m, i, j, item);
}

int spi_Object_Matrix_size(
    spi_Object_Matrix* m,
    int* nr, int* nc)
{
    return spi::Matrix_size<spi::Object>(m, nr, nc);
}

