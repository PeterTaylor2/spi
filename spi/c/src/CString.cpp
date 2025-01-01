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

#include "CString.h"
#include "Error.h"

#include "Helper.hpp"

#include <string.h>

/*
**************************************************************************
* Implementation of spi_String functions
**************************************************************************
*/
char* spi_String_copy(const char * str)
{
    SPI_C_LOCK_GUARD;
    if (!str)
    {
        spi_Error_set_function(__FUNCTION__, "NULL inputs");
        return NULL;
    }

    char* copy = (char*)g_malloc(strlen(str) + 1);
    if (!copy)
    {
        spi_Error_set_function(__FUNCTION__, "Memory allocation failure");
        return NULL;
    }

    strcpy(copy, str);
    return copy;
}

void spi_String_delete(char * str)
{
    SPI_C_LOCK_GUARD;
    if (str)
        free(str);
}

void spi_String_Vector_delete(spi_String_Vector* c)
{
    SPI_C_LOCK_GUARD;
    if (c)
    {
        auto cpp = (std::vector<std::string>*)(c);
        delete cpp;
    }
}

void spi_String_Matrix_delete(spi_String_Matrix* c)
{
    SPI_C_LOCK_GUARD;
    if (c)
    {
        auto cpp = (spi::MatrixData<std::string>*)(c);
        delete cpp;
    }
}

spi_String_Vector* spi_String_Vector_new(int N)
{
    SPI_C_LOCK_GUARD;
    try
    {
        auto out = new std::vector<std::string>(to_size_t(N));
        return (spi_String_Vector*)(out);
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return NULL;
    }
}

int spi_String_Vector_get_data(const spi_String_Vector* v, int N, char* data[])
{
    SPI_C_LOCK_GUARD;
    if (!v)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const std::vector<std::string>*)(v);
        size_t uN = to_size_t(N);
        if (uN != cpp->size())
        {
            spi_Error_set_function(__FUNCTION__, "Array size mismatch");
            return -1;
        }
        for (int i = 0; i < N; ++i)
            data[i] = spi_String_copy(cpp->at(i).c_str());
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
    return 0;

}

int spi_String_Vector_set_data(spi_String_Vector* v, int N, char* data[])
{
    SPI_C_LOCK_GUARD;
    if (!v)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (std::vector<std::string>*)(v);
        size_t uN = to_size_t(N);
        if (uN != cpp->size())
        {
            spi_Error_set_function(__FUNCTION__, "Array size mismatch");
            return -1;
        }
        for (int i = 0; i < N; ++i)
            cpp->at(i) = data[i] ? std::string(data[i]) : std::string();
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
    return 0;
}

spi_String_Matrix* spi_String_Matrix_new(int nr, int nc)
{
    SPI_C_LOCK_GUARD;
    try
    {
        auto out = new spi::MatrixData<std::string>(to_size_t(nr), to_size_t(nc));
        return (spi_String_Matrix*)(out);
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return NULL;
    }
}

int spi_String_Matrix_get_data(const spi_String_Matrix* m, int nr, int nc, char* data[])
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const spi::MatrixData<std::string>*)(m);
        size_t unr = to_size_t(nr);
        size_t unc = to_size_t(nc);
        if (unr != cpp->Rows() || unc != cpp->Cols())
        {
            spi_Error_set_function(__FUNCTION__, "Matrix size mismatch");
            return -1;
        }
        size_t N = unr * unc;
        const std::string* p = cpp->DataPointer();
        for (int i = 0; i < N; ++i)
            data[i] = spi_String_copy(p[i].c_str());
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
    return 0;
}

int spi_String_Matrix_set_data(spi_String_Matrix* m, int nr, int nc, const char* data[])
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (spi::MatrixData<std::string>*)(m);
        size_t unr = to_size_t(nr);
        size_t unc = to_size_t(nc);
        if (unr != cpp->Rows() || unc != cpp->Cols())
        {
            spi_Error_set_function(__FUNCTION__, "Matrix size mismatch");
            return -1;
        }
        size_t N = unr * unc;
        std::string* p = cpp->DataPointer();
        for (int i = 0; i < N; ++i)
            p[i] = data[i] ? std::string(data[i]) : std::string();
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
    return 0;
}

int spi_String_Vector_size(
    const spi_String_Vector* v,
    int* size)
{
    SPI_C_LOCK_GUARD;
    if (!v || !size)
    {
        spi_Error_set_function(__FUNCTION__, "NULL inputs");
        return -1;
    }

    auto cpp = (const std::vector<std::string>*)(v);
    *size = to_int(cpp->size());
    return 0;
}

int spi_String_Matrix_size(
    const spi_String_Matrix* m,
    int* nr, int* nc)
{
    SPI_C_LOCK_GUARD;
    if (!nr || !nc)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    if (!m)
    {
        *nr = 0;
        *nc = 0;
        return 0;
    }

    try
    {
        auto cpp = (const spi::MatrixData<std::string>*)(m);
        *nr = to_int(cpp->Rows());
        *nc = to_int(cpp->Cols());
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

