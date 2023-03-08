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

#include "CVector.h"

#include "Error.h"

#include "Helper.hpp"

/*
**************************************************************************
* Implementation of spi_Int functions
**************************************************************************
*/
void spi_Int_Vector_delete(spi_Int_Vector* c)
{
    if (c)
    {
        auto cpp = (std::vector<int>*)(c);
        delete cpp;
    }
}

spi_Int_Vector* spi_Int_Vector_new(int N)
{
    try
    {
        auto out = new std::vector<int>(to_size_t(N));
        return (spi_Int_Vector*)(out);
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return NULL;
    }
}

int spi_Int_Vector_item(
    const spi_Int_Vector* v,
    int ii,
    int* item)
{
    if (!v || !item)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const std::vector<int>*)(v);
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

int spi_Int_Vector_set_item(
    spi_Int_Vector* v,
    int ii,
    int item)
{
    if (!v)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (std::vector<int>*)(v);
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

int spi_Int_Vector_size(
    const spi_Int_Vector* v,
    int* size)
{
    if (!v || !size)
    {
        spi_Error_set_function(__FUNCTION__, "NULL inputs");
        return -1;
    }

    auto cpp = (const std::vector<int>*)(v);
    *size = to_int(cpp->size());
    return 0;
}

/*
**************************************************************************
* Implementation of spi_Double functions
**************************************************************************
*/
void spi_Double_Vector_delete(spi_Double_Vector* c)
{
    if (c)
    {
        auto cpp = (std::vector<double>*)(c);
        delete cpp;
    }
}

spi_Double_Vector* spi_Double_Vector_new(int N)
{
    try
    {
        auto out = new std::vector<double>(to_size_t(N));
        return (spi_Double_Vector*)(out);
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return NULL;
    }
}

int spi_Double_Vector_item(
    const spi_Double_Vector* v,
    int ii,
    double* item)
{
    if (!v || !item)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const std::vector<double>*)(v);
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

int spi_Double_Vector_set_item(
    spi_Double_Vector* v,
    int ii,
    double item)
{
    if (!v)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (std::vector<double>*)(v);
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

int spi_Double_Vector_size(
    const spi_Double_Vector* v,
    int* size)
{
    if (!v || !size)
    {
        spi_Error_set_function(__FUNCTION__, "NULL inputs");
        return -1;
    }

    auto cpp = (const std::vector<double>*)(v);
    *size = to_int(cpp->size());
    return 0;
}


/*
**************************************************************************
* Implementation of spi_Bool functions
**************************************************************************
*/
void spi_Bool_Vector_delete(spi_Bool_Vector* c)
{
    if (c)
    {
        auto cpp = (std::vector<bool>*)(c);
        delete cpp;
    }
}

spi_Bool_Vector* spi_Bool_Vector_new(int N)
{
    try
    {
        auto out = new std::vector<bool>(to_size_t(N));
        return (spi_Bool_Vector*)(out);
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return NULL;
    }
}

int spi_Bool_Vector_item(
    const spi_Bool_Vector* v,
    int ii,
    spi_Bool* item)
{
    if (!v || !item)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const std::vector<bool>*)(v);
        size_t i = to_size_t(ii); 
        if (i >= cpp->size())
        {
            spi_Error_set_function(__FUNCTION__, "Array bounds mismatch");
            return -1;
        }
        *item = (*cpp)[i] ? SPI_TRUE : SPI_FALSE;
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Bool_Vector_set_item(
    spi_Bool_Vector* v,
    int ii,
    spi_Bool item)
{
    if (!v)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (std::vector<bool>*)(v);
        size_t i = to_size_t(ii); 
        if (i >= cpp->size())
        {
            spi_Error_set_function(__FUNCTION__, "Array bounds mismatch");
            return -1;
        }
        (*cpp)[i] = item != SPI_FALSE;
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Bool_Vector_size(
    const spi_Bool_Vector* v,
    int* size)
{
    if (!v || !size)
    {
        spi_Error_set_function(__FUNCTION__, "NULL inputs");
        return -1;
    }

    auto cpp = (const std::vector<bool>*)(v);
    *size = to_int(cpp->size());
    return 0;
}

int spi_Instance_Vector_item(
    const spi_Instance_Vector* v,
    int i,
    spi_Instance** item)
{
    if (!v || !item)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }
    try
    {
        spi::InstanceConstSP o = ((std::vector<spi::InstanceConstSP>*)v)->at(
            spi_util::IntegerCast<size_t>(i));

        *item = (spi_Instance*)(spi::RawPointer(o));
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }

}

int spi_Instance_Vector_set_item(
    spi_Instance_Vector* v,
    int i,
    spi_Instance* item)
{
    if (!v)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }
    try
    {
        ((std::vector<spi::InstanceConstSP>*)v)->at(spi_util::IntegerCast<size_t>(i)) =
            spi::InstanceConstSP((spi::Instance*)(item));
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }

}

int spi_Instance_Vector_size(
    const spi_Instance_Vector* v,
    int* size)
{
    if (!v || !size)
    {
        spi_Error_set_function(__FUNCTION__, "NULL inputs");
        return -1;
    }
    *size = spi_util::IntegerCast<int>(((std::vector<spi::InstanceConstSP>*)v)->size());
    return 0;
}


int spi_Enum_Vector_item(
    const spi_Enum_Vector* v,
    int i,
    int* item)
{
    if (!v || !item)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }
    try
    {
        spi::Enum e = ((std::vector<spi::Enum>*)v)->at(
            spi_util::IntegerCast<size_t>(i));

        *item = e.value;
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }

}

int spi_Enum_Vector_set_item(
    spi_Enum_Vector* v,
    int i,
    int item)
{
    if (!v)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }
    try
    {
        ((std::vector<spi::Enum>*)v)->at(spi_util::IntegerCast<size_t>(i)) = item;
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }

}

int spi_Enum_Vector_size(
    const spi_Enum_Vector* v,
    int* size)
{
    if (!v || !size)
    {
        spi_Error_set_function(__FUNCTION__, "NULL inputs");
        return -1;
    }
    *size = spi_util::IntegerCast<int>(((std::vector<spi::Enum>*)v)->size());
    return 0;
}
