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

#include "CMatrix.h"

#include "Error.h"

#include "Helper.hpp"

/*
**************************************************************************
* Implementation of spi_Int functions
**************************************************************************
*/
void spi_Int_Matrix_delete(spi_Int_Matrix* c)
{
    SPI_C_LOCK_GUARD;
    if (c)
    {
        auto cpp = (spi::MatrixData<int>*)(c);
        delete cpp;
    }
}

spi_Int_Matrix* spi_Int_Matrix_new(int nr, int nc)
{
    SPI_C_LOCK_GUARD;
    try
    {
        auto out = new spi::MatrixData<int>(to_size_t(nr), to_size_t(nc));
        return (spi_Int_Matrix*)(out);
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return NULL;
    }
}

int spi_Int_Matrix_get_data(const spi_Int_Matrix* m, int nr, int nc, int data[])
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const spi::MatrixData<int>*)(m);
        size_t unr = to_size_t(nr);
        size_t unc = to_size_t(nc);
        if (cpp->Rows() != unr || cpp->Cols() != unc)
        {
            spi_Error_set_function(__FUNCTION__, "Matrix size mismatch");
            return -1;
        }

        // because int is plain old data we can do a bulk copy
        memcpy(&data[0], cpp->DataPointer(), unr * unc * sizeof(int));

        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Int_Matrix_set_data(spi_Int_Matrix* m, int nr, int nc, int data[])
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (spi::MatrixData<int>*)(m);
        size_t unr = to_size_t(nr);
        size_t unc = to_size_t(nc);
        if (cpp->Rows() != unr || cpp->Cols() != unc)
        {
            spi_Error_set_function(__FUNCTION__, "Matrix size mismatch");
            return -1;
        }

        // because int is plain old data we can do a bulk copy
        memcpy(cpp->DataPointer(), &data[0], unr * unc * sizeof(int));

        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Int_Matrix_item(
    const spi_Int_Matrix* m,
    int ir, int ic,
    int* item)
{
    SPI_C_LOCK_GUARD;
    if (!m || !item)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const spi::MatrixData<int>*)(m);
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

int spi_Int_Matrix_set_item(
    spi_Int_Matrix* m,
    int ir, int ic,
    int item)
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (spi::MatrixData<int>*)(m);
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

int spi_Int_Matrix_size(
    const spi_Int_Matrix* m,
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
        auto cpp = (const spi::MatrixData<int>*)(m);
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

/*
**************************************************************************
* Implementation of spi_Double functions
**************************************************************************
*/
void spi_Double_Matrix_delete(spi_Double_Matrix* c)
{
    SPI_C_LOCK_GUARD;
    if (c)
    {
        auto cpp = (spi::MatrixData<double>*)(c);
        delete cpp;
    }
}

spi_Double_Matrix* spi_Double_Matrix_new(int nr, int nc)
{
    SPI_C_LOCK_GUARD;
    try
    {
        auto out = new spi::MatrixData<double>(to_size_t(nr), to_size_t(nc));
        return (spi_Double_Matrix*)(out);
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return NULL;
    }
}



int spi_Double_Matrix_get_data(const spi_Double_Matrix* m, int nr, int nc, double data[])
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const spi::MatrixData<double>*)(m);
        size_t unr = to_size_t(nr);
        size_t unc = to_size_t(nc);
        if (cpp->Rows() != unr || cpp->Cols() != unc)
        {
            spi_Error_set_function(__FUNCTION__, "Matrix size mismatch");
            return -1;
        }

        // because double is plain old data we can do a bulk copy
        memcpy(&data[0], cpp->DataPointer(), unr * unc * sizeof(double));

        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Double_Matrix_set_data(spi_Double_Matrix* m, int nr, int nc, double data[])
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (spi::MatrixData<double>*)(m);
        size_t unr = to_size_t(nr);
        size_t unc = to_size_t(nc);
        if (cpp->Rows() != unr || cpp->Cols() != unc)
        {
            spi_Error_set_function(__FUNCTION__, "Matrix size mismatch");
            return -1;
        }

        // because double is plain old data we can do this bulk copy

        memcpy(cpp->DataPointer(), &data[0], unr * unc * sizeof(double));
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Double_Matrix_item(
    const spi_Double_Matrix* m,
    int ir, int ic,
    double* item)
{
    SPI_C_LOCK_GUARD;
    if (!m || !item)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const spi::MatrixData<double>*)(m);
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

int spi_Double_Matrix_set_item(
    spi_Double_Matrix* m,
    int ir, int ic,
    double item)
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (spi::MatrixData<double>*)(m);
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

int spi_Double_Matrix_size(
    const spi_Double_Matrix* m,
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
        auto cpp = (const spi::MatrixData<double>*)(m);
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

/*
**************************************************************************
* Implementation of spi_Bool functions
**************************************************************************
*/
void spi_Bool_Matrix_delete(spi_Bool_Matrix* c)
{
    SPI_C_LOCK_GUARD;
    if (c)
    {
        auto cpp = (spi::MatrixData<bool>*)(c);
        delete cpp;
    }
}

spi_Bool_Matrix* spi_Bool_Matrix_new(int nr, int nc)
{
    SPI_C_LOCK_GUARD;
    try
    {
        auto out = new spi::MatrixData<bool>(to_size_t(nr), to_size_t(nc));
        return (spi_Bool_Matrix*)(out);
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return NULL;
    }
}

int spi_Bool_Matrix_get_data(const spi_Bool_Matrix* m, int nr, int nc, spi_Bool data[])
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const spi::MatrixData<bool>*)(m);
        size_t unr = to_size_t(nr);
        size_t unc = to_size_t(nc);
        if (cpp->Rows() != unr || cpp->Cols() != unc)
        {
            spi_Error_set_function(__FUNCTION__, "Matrix size mismatch");
            return -1;
        }

        // bool and spi_Bool are not the same so we need to copy one at a time
        size_t k = 0;
        for (size_t i = 0; i < unr; ++i)
        {
            for (size_t j = 0; j < unc; ++j)
            {
                data[k] = cpp->at(i, j) ? SPI_TRUE : SPI_FALSE;
                ++k;
            }
        }

        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Bool_Matrix_set_data(spi_Bool_Matrix* m, int nr, int nc, spi_Bool data[])
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (spi::MatrixData<bool>*)(m);
        size_t unr = to_size_t(nr);
        size_t unc = to_size_t(nc);
        if (cpp->Rows() != unr || cpp->Cols() != unc)
        {
            spi_Error_set_function(__FUNCTION__, "Matrix size mismatch");
            return -1;
        }

        // bool and spi_Bool are not the same so we need to copy one at a time
        size_t k = 0;
        for (size_t i = 0; i < unr; ++i)
        {
            for (size_t j = 0; j < unc; ++j)
            {
                cpp->at(i, j) = data[k] != SPI_FALSE;
                ++k;
            }
        }

        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Bool_Matrix_item(
    const spi_Bool_Matrix* m,
    int ir, int ic,
    spi_Bool* item)
{
    SPI_C_LOCK_GUARD;
    if (!m || !item)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const spi::MatrixData<bool>*)(m);
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

int spi_Bool_Matrix_set_item(
    spi_Bool_Matrix* m,
    int ir, int ic,
    spi_Bool item)
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (spi::MatrixData<bool>*)(m);
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

int spi_Bool_Matrix_size(
    const spi_Bool_Matrix* m,
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
        auto cpp = (const spi::MatrixData<bool>*)(m);
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

int spi_Instance_Matrix_item(
    const spi_Instance_Matrix* m,
    int r, int c,
    spi_Instance** item)
{
    SPI_C_LOCK_GUARD;
    if (!m || !item)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        spi::InstanceConstSP o = ((spi::MatrixData<spi::InstanceConstSP>*)(m))->at(
            spi_util::IntegerCast<size_t>(r),
            spi_util::IntegerCast<size_t>(c));
        *item = (spi_Instance*)(spi::RawPointer(o));
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Instance_Matrix_set_item(
    spi_Instance_Matrix* m,
    int r, int c,
    spi_Instance* item)
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }
    try
    {
        ((spi::MatrixData<spi::InstanceConstSP>*)m)->at(
            spi_util::IntegerCast<size_t>(r),
            spi_util::IntegerCast<size_t>(c)) = spi::InstanceConstSP((spi::Instance*)item);
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Instance_Matrix_size(
    const spi_Instance_Matrix* m,
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
        *nr = spi_util::IntegerCast<int>(((spi::MatrixData<spi::InstanceConstSP>*)m)->Rows());
        *nc = spi_util::IntegerCast<int>(((spi::MatrixData<spi::InstanceConstSP>*)m)->Cols());
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Enum_Matrix_get_data(const spi_Enum_Matrix* m, int nr, int nc, int data[])
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const spi::MatrixData<spi::Enum>*)(m);
        size_t unr = to_size_t(nr);
        size_t unc = to_size_t(nc);
        if (cpp->Rows() != unr || cpp->Cols() != unc)
        {
            spi_Error_set_function(__FUNCTION__, "Matrix size mismatch");
            return -1;
        }

        // int and spi_Enum are not the same so we need to copy one at a time
        // actually they might be sufficiently similar to allow a block copy
        size_t k = 0;
        for (size_t i = 0; i < unr; ++i)
        {
            for (size_t j = 0; j < unc; ++j)
            {
                spi::Enum e = cpp->at(i, j);
                data[k] = e.value;
                ++k;
            }
        }

        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Enum_Matrix_set_data(spi_Enum_Matrix* m, int nr, int nc, int data[])
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (spi::MatrixData<spi::Enum>*)(m);
        size_t unr = to_size_t(nr);
        size_t unc = to_size_t(nc);
        if (cpp->Rows() != unr || cpp->Cols() != unc)
        {
            spi_Error_set_function(__FUNCTION__, "Matrix size mismatch");
            return -1;
        }

        // int and spi_Enum are not the same so we need to copy one at a time
        // actually they might be sufficiently similar to allow a block copy
        size_t k = 0;
        for (size_t i = 0; i < unr; ++i)
        {
            for (size_t j = 0; j < unc; ++j)
            {
                cpp->at(i, j) = data[k];
            }
        }

        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}



int spi_Enum_Matrix_item(
    const spi_Enum_Matrix* m,
    int r, int c,
    int* item)
{
    SPI_C_LOCK_GUARD;
    if (!m || !item)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        spi::Enum e = ((spi::MatrixData<spi::Enum>*)(m))->at(
            spi_util::IntegerCast<size_t>(r),
            spi_util::IntegerCast<size_t>(c));
        *item = e.value;
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }

}

int spi_Enum_Matrix_set_item(
    spi_Enum_Matrix* m,
    int r, int c,
    int item)
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }
    try
    {
        ((spi::MatrixData<spi::Enum>*)m)->at(
            spi_util::IntegerCast<size_t>(r),
            spi_util::IntegerCast<size_t>(c)) = item;
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Enum_Matrix_size(
    const spi_Enum_Matrix* m,
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
        *nr = spi_util::IntegerCast<int>(((spi::MatrixData<spi::Enum>*)m)->Rows());
        *nc = spi_util::IntegerCast<int>(((spi::MatrixData<spi::Enum>*)m)->Cols());
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }

}

