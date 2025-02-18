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

namespace 
{
    const int SPI_DATE_OFFSET = 109205;
}

spi_Date spi_Date_convert_in(System_Date dt)
{
    spi_Date out = (spi_Date)dt;
    if (out <= 0)
        out = 0;
    else
        out = out + SPI_DATE_OFFSET;
    return out;
}

System_Date spi_Date_convert_out(spi_Date dt)
{
    const int y400 = 365 * 400 + 97; /* number of days in 400 years */
    System_Date out = dt > 0 ?
        dt - SPI_DATE_OFFSET : /* relative to 1900 */
        -4 * y400 - SPI_DATE_OFFSET; /* we want to get default(System.DateTime) */
    return out;
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

int spi_Date_Vector_get_data(
    const spi_Date_Vector* v,
    int N,
    System_Date data[])
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
            data[i] = spi_Date_convert_out(cpp->at(i));
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
    return 0;
}

int spi_Date_Vector_set_data(
    spi_Date_Vector* v,
    int N,
    System_Date data[])
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
            cpp->at(i) = spi_Date_convert_in(data[i]);
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
    return 0;
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

void spi_Date_Matrix_delete(spi_Date_Matrix* c)
{
    SPI_C_LOCK_GUARD;
    if (c)
    {
        auto cpp = (spi::MatrixData<spi::Date>*)(c);
        delete cpp;
    }
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

int spi_Date_Matrix_get_data(
    const spi_Date_Matrix* m,
    int nr,
    int nc,
    System_Date data[])
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const spi::MatrixData<spi::Date>*)(m);
        size_t unr = to_size_t(nr);
        size_t unc = to_size_t(nc);
        if (cpp->Rows() != unr || cpp->Cols() != unc)
        {
            spi_Error_set_function(__FUNCTION__, "Matrix size mismatch");
            return -1;
        }

        size_t N = unr * unc;
        const spi::Date* p = cpp->DataPointer();
        for (size_t i = 0; i < N; ++i)
        {
            data[i] = spi_Date_convert_out(p[i]);
        }

        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }

}

int spi_Date_Matrix_set_data(
    spi_Date_Matrix* m,
    int nr,
    int nc,
    System_Date data[])
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
        size_t unr = to_size_t(nr);
        size_t unc = to_size_t(nc);
        if (cpp->Rows() != unr || cpp->Cols() != unc)
        {
            spi_Error_set_function(__FUNCTION__, "Matrix size mismatch");
            return -1;
        }

        size_t N = unr * unc;
        spi::Date* p = cpp->DataPointer();
        for (size_t i = 0; i < N; ++i)
        {
            p[i] = spi_Date_convert_in(data[i]);
        }

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

