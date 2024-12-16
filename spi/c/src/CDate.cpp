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

#include "CDate.h"
#include "Error.h"

#include "Helper.hpp"


/*
**************************************************************************
* Implementation of spi_Date functions
**************************************************************************
*/
int spi_Date_from_YMD(
    int year, int month, int day, spi_Date* date)
{
    SPI_C_LOCK_GUARD;
    try
    {
        *date = spi::Date(year, month, day);
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Date_YMD(
    spi_Date date, int* year, int* month, int* day)
{
    SPI_C_LOCK_GUARD;
    try
    {
        auto cpp = spi::Date(date);
        cpp.YMD(year, month, day);
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

void spi_Date_Vector_delete(spi_Date_Vector* c)
{
    SPI_C_LOCK_GUARD;
    if (c)
    {
        auto cpp = (std::vector<spi::Date>*)(c);
        delete cpp;
    }
}

void spi_Date_Matrix_delete(spi_Date_Matrix* c)
{
    SPI_C_LOCK_GUARD;
    if (c)
    {
        auto cpp = (spi::MatrixData<spi::Date>*)(c);
        delete cpp;
    }
}

spi_Date_Vector* spi_Date_Vector_new(int N)
{
    SPI_C_LOCK_GUARD;
    try
    {
        auto out = new std::vector<spi::Date>(to_size_t(N));
        return (spi_Date_Vector*)(out);
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return NULL;
    }
}

int spi_Date_Vector_get_data(const spi_Date_Vector* v, int N, spi_Date data[])
{
    SPI_C_LOCK_GUARD;
    if (!v)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const std::vector<spi::Date>*)(v);
        size_t uN = to_size_t(N);
        if (uN != cpp->size())
        {
            spi_Error_set_function(__FUNCTION__, "Array size mismatch");
            return -1;
        }
        for (int i = 0; i < N; ++i)
            data[i] = cpp->at(i);
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
    return 0;
}

int spi_Date_Vector_set_data(spi_Date_Vector* v, int N, spi_Date data[])
{
    SPI_C_LOCK_GUARD;
    if (!v)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (std::vector<spi::Date>*)(v);
        size_t uN = to_size_t(N);
        if (uN != cpp->size())
        {
            spi_Error_set_function(__FUNCTION__, "Array size mismatch");
            return -1;
        }
        for (int i = 0; i < N; ++i)
            cpp->at(i) = data[i];
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
    return 0;
}

spi_Date_Matrix* spi_Date_Matrix_new(int nr, int nc)
{
    SPI_C_LOCK_GUARD;
    try
    {
        auto out = new spi::MatrixData<spi::Date>(to_size_t(nr), to_size_t(nc));
        return (spi_Date_Matrix*)(out);
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return NULL;
    }
}

int spi_Date_Vector_item(
    const spi_Date_Vector* v,
    int ii,
    spi_Date* item)
{
    SPI_C_LOCK_GUARD;
    if (!v || !item)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const std::vector<spi::Date>*)(v);
        size_t i = to_size_t(ii); 
        if (i >= cpp->size())
        {
            spi_Error_set_function(__FUNCTION__, "Array bounds mismatch");
            return -1;
        }
        *item = (*cpp)[i];
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Date_Vector_set_item(
    spi_Date_Vector* v,
    int ii,
    spi_Date item)
{
    SPI_C_LOCK_GUARD;
    if (!v)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (std::vector<spi::Date>*)(v);
        size_t i = to_size_t(ii); 
        if (i >= cpp->size())
        {
            spi_Error_set_function(__FUNCTION__, "Array bounds mismatch");
            return -1;
        }
        (*cpp)[i] = item;
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Date_Vector_size(
    const spi_Date_Vector* v,
    int* size)
{
    SPI_C_LOCK_GUARD;
    if (!v || !size)
    {
        spi_Error_set_function(__FUNCTION__, "NULL inputs");
        return -1;
    }

    auto cpp = (const std::vector<spi::Date>*)(v);
    *size = to_int(cpp->size());
    return 0;
}

int spi_Date_Matrix_item(
    const spi_Date_Matrix* m,
    int ir, int ic,
    spi_Date* item)
{
    SPI_C_LOCK_GUARD;
    if (!m || !item)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const spi::MatrixData<spi::Date>*)(m);
        size_t r = to_size_t(ir);
        size_t c = to_size_t(ic); 
        if (r >= cpp->Rows() || c >= cpp->Cols())
        {
            spi_Error_set_function(__FUNCTION__, "Array bounds mismatch");
            return -1;
        }
        *item = (*cpp)[r][c];
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Date_Matrix_set_item(
    spi_Date_Matrix* m,
    int ir, int ic,
    spi_Date item)
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (spi::MatrixData<spi::Date>*)(m);
        size_t r = to_size_t(ir);
        size_t c = to_size_t(ic); 
        if (r >= cpp->Rows() || c >= cpp->Cols())
        {
            spi_Error_set_function(__FUNCTION__, "Array bounds mismatch");
            return -1;
        }
        (*cpp)[r][c] = item;
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Date_Matrix_size(
    const spi_Date_Matrix* m,
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
        auto cpp = (const spi::MatrixData<spi::Date>*)(m);
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

