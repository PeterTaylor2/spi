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

#include "CDateTime.h"
#include "Error.h"

#include "Helper.hpp"

/*
**************************************************************************
* Implementation of spi_DateTime functions
**************************************************************************
*/
int spi_DateTime_from_YMDHMS(
    int year, int month, int day, int hours, int minutes, int seconds,
    spi_DateTime* dt)
{
    SPI_C_LOCK_GUARD;
    try
    {
        *dt = spi::DateTime(spi::Date(year, month, day), hours, minutes, seconds);
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_DateTime_YMDHMS(
    spi_DateTime dt,
    int* year, int* month, int* day, int* hours, int* minutes, int* seconds)
{
    SPI_C_LOCK_GUARD;
    try
    {
        auto cpp = spi::DateTime(dt);
        cpp.Date().YMD(year, month, day);
        cpp.HMS(hours, minutes, seconds);
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

void spi_DateTime_Vector_delete(spi_DateTime_Vector* c)
{
    SPI_C_LOCK_GUARD;
    if (c)
    {
        auto cpp = (std::vector<spi::DateTime>*)(c);
        delete cpp;
    }
}

void spi_DateTime_Matrix_delete(spi_DateTime_Matrix* c)
{
    SPI_C_LOCK_GUARD;
    if (c)
    {
        auto cpp = (spi::MatrixData<spi::DateTime>*)(c);
        delete cpp;
    }
}

spi_DateTime_Vector* spi_DateTime_Vector_new(int N)
{
    SPI_C_LOCK_GUARD;
    try
    {
        auto out = new std::vector<spi::DateTime>(to_size_t(N));
        return (spi_DateTime_Vector*)(out);
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return NULL;
    }
}

int spi_DateTime_Vector_get_data(const spi_DateTime_Vector* v, int N, spi_DateTime data[])
{
    SPI_C_LOCK_GUARD;
    if (!v)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const std::vector<spi::DateTime>*)(v);
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

int spi_DateTime_Vector_set_data(spi_DateTime_Vector* v, int N, spi_DateTime data[])
{
    SPI_C_LOCK_GUARD;
    if (!v)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (std::vector<spi::DateTime>*)(v);
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

spi_DateTime_Matrix* spi_DateTime_Matrix_new(int nr, int nc)
{
    SPI_C_LOCK_GUARD;
    try
    {
        auto out = new spi::MatrixData<spi::DateTime>(to_size_t(nr), to_size_t(nc));
        return (spi_DateTime_Matrix*)(out);
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return NULL;
    }
}

int spi_DateTime_Vector_item(
    const spi_DateTime_Vector* v,
    int ii,
    spi_DateTime* item)
{
    SPI_C_LOCK_GUARD;
    if (!v || !item)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const std::vector<spi::DateTime>*)(v);
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

int spi_DateTime_Vector_set_item(
    spi_DateTime_Vector* v,
    int ii,
    spi_DateTime item)
{
    SPI_C_LOCK_GUARD;
    if (!v)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (std::vector<spi::DateTime>*)(v);
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

int spi_DateTime_Vector_size(
    const spi_DateTime_Vector* v,
    int* size)
{
    SPI_C_LOCK_GUARD;
    if (!v || !size)
    {
        spi_Error_set_function(__FUNCTION__, "NULL inputs");
        return -1;
    }

    auto cpp = (const std::vector<spi::DateTime>*)(v);
    *size = to_int(cpp->size());
    return 0;
}

int spi_DateTime_Matrix_item(
    const spi_DateTime_Matrix* m,
    int ir, int ic,
    spi_DateTime* item)
{
    SPI_C_LOCK_GUARD;
    if (!m || !item)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const spi::MatrixData<spi::DateTime>*)(m);
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

int spi_DateTime_Matrix_set_item(
    spi_DateTime_Matrix* m,
    int ir, int ic,
    spi_DateTime item)
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (spi::MatrixData<spi::DateTime>*)(m);
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

int spi_DateTime_Matrix_size(
    const spi_DateTime_Matrix* m,
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
        auto cpp = (const spi::MatrixData<spi::DateTime>*)(m);
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

