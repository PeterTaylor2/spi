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

#include "CVariant.h"

#include "Error.h"

#include "Helper.hpp"

/*
**************************************************************************
* Implementation of spi_Variant functions
**************************************************************************
*/

void spi_Variant_delete(spi_Variant * var)
{
    SPI_C_LOCK_GUARD;
    delete ((spi::Variant*) var);
}

spi_Variant* spi_Variant_new_String(const char * str)
{
    SPI_C_LOCK_GUARD;
    try
    {
        spi::Variant* var = new spi::Variant(str);
        return (spi_Variant*)var;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return nullptr;
    }
}

spi_Variant* spi_Variant_new_Date(spi_Date dt)
{
    SPI_C_LOCK_GUARD;
    try
    {
        spi::Variant* var = new spi::Variant(spi::Date(dt));
        return (spi_Variant*)var;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return nullptr;
    }
}

spi_Variant* spi_Variant_new_DateTime(spi_DateTime dt)
{
    SPI_C_LOCK_GUARD;
    try
    {
        SPI_THROW_RUNTIME_ERROR("Constructor of Variant from DateTime not supported");
        //spi::Variant* var = new spi::Variant(spi::DateTime(dt));
        //return (spi_Variant*)var;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return nullptr;
    }
}

spi_Variant* spi_Variant_new_Double(double d)
{
    SPI_C_LOCK_GUARD;
    try
    {
        spi::Variant* var = new spi::Variant(d);
        return (spi_Variant*)var;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return nullptr;
    }
}

spi_Variant* spi_Variant_new_Bool(spi_Bool b)
{
    SPI_C_LOCK_GUARD;
    try
    {
        spi::Variant* var = new spi::Variant(b);
        return (spi_Variant*)var;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return nullptr;
    }
}

spi_Variant* spi_Variant_new_Int(int i)
{
    SPI_C_LOCK_GUARD;
    try
    {
        spi::Variant* var = new spi::Variant(i);
        return (spi_Variant*)var;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return nullptr;
    }
}

int spi_Variant_type(spi_Variant* var, char** vt)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!var || !vt)
        {
            spi_Error_set_function(__FUNCTION__, "NULL inputs");
            return -1;
        }
        spi::Value::Type valueType = ((spi::Variant*)var)->ValueType();
        *vt = spi_String_copy(spi::Value::TypeToString(valueType));
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
    return 0;
}

int spi_Variant_array_element_type(spi_Variant* var, char** vt)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!var || !vt)
        {
            spi_Error_set_function(__FUNCTION__, "NULL inputs");
            return -1;
        }
        spi::Value::Type valueType = ((spi::Variant*)var)->ValueType();
        spi::Value::Type arrayElementType = spi::Value::UNDEFINED;
        switch (valueType)
        {
        case spi::Value::ARRAY:
            arrayElementType = ((spi::Variant*)var)->GetValue().getArray()->elementType();
            break;
        default:
            spi_Error_set_function(__FUNCTION__, "Input is not an array");
            return -1;
        }
        *vt = spi_String_copy(spi::Value::TypeToString(arrayElementType));
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
    return 0;
}

int spi_Variant_array_num_dimensions(spi_Variant* var, int* numDimensions)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!var || !numDimensions)
        {
            spi_Error_set_function(__FUNCTION__, "NULL inputs");
            return -1;
        }
        spi::IArrayConstSP array = ((spi::Variant*)var)->GetValue().getArray();
        std::vector<size_t> dimensions = array->dimensions();
        *numDimensions = to_int(dimensions.size());
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
    return 0;
}

int spi_Variant_array_dimensions(spi_Variant* var, int ND, int dimensions[])
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!var)
        {
            spi_Error_set_function(__FUNCTION__, "NULL inputs");
            return -1;
        }
        spi::IArrayConstSP array = ((spi::Variant*)var)->GetValue().getArray();
        size_t uND = to_size_t(ND);
        const std::vector<size_t>& arrayDimensions = array->dimensions();
        if (arrayDimensions.size() != uND)
        {
            spi_Error_set_function(__FUNCTION__, "Array size mismatch");
            return -1;
        }
        for (size_t i = 0; i < uND; ++i)
            dimensions[i] = to_int(arrayDimensions[i]);
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
    return 0;

}

int spi_Variant_String(spi_Variant * var, char ** str)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!var || !str)
        {
            spi_Error_set_function(__FUNCTION__, "NULL inputs");
            return -1;
        }
        std::string vs = ((spi::Variant*)var)->ToString();
        *str = spi_String_copy(vs.c_str());
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Variant_Date(spi_Variant * var, spi_Date * dt)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!var || !dt)
        {
            spi_Error_set_function(__FUNCTION__, "NULL inputs");
            return -1;
        }
        *dt = ((spi::Variant*)var)->ToDate();
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Variant_DateTime(spi_Variant * var, spi_DateTime * dt)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!var || !dt)
        {
            spi_Error_set_function(__FUNCTION__, "NULL inputs");
            return -1;
        }
        *dt = (double)((spi::Variant*)var)->ToDateTime() - SPI_DATE_TIME_OFFSET;
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Variant_Double(spi_Variant * var, double * d)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!var || !d)
        {
            spi_Error_set_function(__FUNCTION__, "NULL inputs");
            return -1;
        }
        *d = ((spi::Variant*)var)->ToDouble();
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Variant_Bool(spi_Variant * var, spi_Bool * b)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!var || !b)
        {
            spi_Error_set_function(__FUNCTION__, "NULL inputs");
            return -1;
        }
        *b = ((spi::Variant*)var)->ToBool() ? SPI_TRUE : SPI_FALSE;
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Variant_Int(spi_Variant * var, int * i)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!var || !i)
        {
            spi_Error_set_function(__FUNCTION__, "NULL inputs");
            return -1;
        }
        *i = ((spi::Variant*)var)->ToInt();
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Variant_Object(spi_Variant* var, spi_Object** obj)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!var || !obj)
        {
            spi_Error_set_function(__FUNCTION__, "NULL inputs");
            return -1;
        }
        spi::ObjectConstSP o = ((spi::Variant*)var)->ToConstObject();
        *obj = spi::convert_out<spi_Object>(o);
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Variant_String_Vector(spi_Variant* var, spi_String_Vector** str)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!var || !str)
        {
            spi_Error_set_function(__FUNCTION__, "NULL inputs");
            return -1;
        }
        *str = (spi_String_Vector*)(new std::vector<std::string>(
            ((spi::Variant*)var)->ToStringVector()));
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Variant_Date_Vector(spi_Variant* var, spi_Date_Vector** dt)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!var || !dt)
        {
            spi_Error_set_function(__FUNCTION__, "NULL inputs");
            return -1;
        }
        *dt = (spi_Date_Vector*)(new std::vector<spi::Date>(
            ((spi::Variant*)var)->ToDateVector()));
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Variant_DateTime_Vector(spi_Variant* var, spi_DateTime_Vector** dt)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!var || !dt)
        {
            spi_Error_set_function(__FUNCTION__, "NULL inputs");
            return -1;
        }
        *dt = (spi_DateTime_Vector*)(new std::vector<spi::DateTime>(
            ((spi::Variant*)var)->ToDateTimeVector()));
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Variant_Double_Vector(spi_Variant* var, spi_Double_Vector** d)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!var || !d)
        {
            spi_Error_set_function(__FUNCTION__, "NULL inputs");
            return -1;
        }
        *d = (spi_Double_Vector*)(new std::vector<double> (
            ((spi::Variant*)var)->ToDoubleVector()));
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Variant_Bool_Vector(spi_Variant* var, spi_Bool_Vector** b)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!var || !b)
        {
            spi_Error_set_function(__FUNCTION__, "NULL inputs");
            return -1;
        }
        *b = (spi_Bool_Vector*)(new std::vector<bool>(
            ((spi::Variant*)var)->ToBoolVector()));
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Variant_Int_Vector(spi_Variant* var, spi_Int_Vector** i)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!var || !i)
        {
            spi_Error_set_function(__FUNCTION__, "NULL inputs");
            return -1;
        }
        *i = (spi_Int_Vector*)(new std::vector<int>(
            ((spi::Variant*)var)->ToIntVector()));
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Variant_Object_Vector(spi_Variant* var, spi_Object_Vector** obj)
{
    SPI_C_LOCK_GUARD;
    try
    {
        if (!var || !obj)
        {
            spi_Error_set_function(__FUNCTION__, "NULL inputs");
            return -1;
        }
        const std::vector<spi::ObjectConstSP>& vec = ((spi::Variant*)var)->ToConstObjectVector();
        *obj = (spi_Object_Vector*)(new std::vector<spi::ObjectConstSP>(vec));
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

void spi_Variant_Vector_delete(spi_Variant_Vector* c)
{
    SPI_C_LOCK_GUARD;
    if (c)
    {
        auto cpp = (std::vector<spi::Variant>*)(c);
        delete cpp;
    }
}

void spi_Variant_Matrix_delete(spi_Variant_Matrix* c)
{
    SPI_C_LOCK_GUARD;
    if (c)
    {
        auto cpp = (spi::MatrixData<spi::Variant>*)(c);
        delete cpp;
    }
}

spi_Variant_Vector* spi_Variant_Vector_new(int N)
{
    SPI_C_LOCK_GUARD;
    try
    {
        auto out = new std::vector<spi::Variant>(to_size_t(N));
        return (spi_Variant_Vector*)(out);
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return NULL;
    }
}

spi_Variant_Matrix* spi_Variant_Matrix_new(int nr, int nc)
{
    SPI_C_LOCK_GUARD;
    try
    {
        auto out = new spi::MatrixData<spi::Variant>(to_size_t(nr), to_size_t(nc));
        return (spi_Variant_Matrix*)(out);
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return NULL;
    }
}

int spi_Variant_Vector_item(
    const spi_Variant_Vector* v,
    int ii,
    spi_Variant** item)
{
    SPI_C_LOCK_GUARD;
    if (!v || !item)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const std::vector<spi::Variant>*)(v);
        size_t i = to_size_t(ii); 
        if (i >= cpp->size())
        {
            spi_Error_set_function(__FUNCTION__, "Array bounds mismatch");
            return -1;
        }
        *item = (spi_Variant*)(new spi::Variant((*cpp)[i]));
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Variant_Vector_set_item(
    spi_Variant_Vector* v,
    int ii,
    spi_Variant* item)
{
    SPI_C_LOCK_GUARD;
    if (!v)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (std::vector<spi::Variant>*)(v);
        size_t i = to_size_t(ii); 
        if (i >= cpp->size())
        {
            spi_Error_set_function(__FUNCTION__, "Array bounds mismatch");
            return -1;
        }
        if (item)
        {
            (*cpp)[i] = *(spi::Variant*)item;
        }
        else
        {
            (*cpp)[i] = spi::Variant();
        }
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Variant_Vector_size(
    const spi_Variant_Vector* v,
    int* size)
{
    SPI_C_LOCK_GUARD;
    if (!v || !size)
    {
        spi_Error_set_function(__FUNCTION__, "NULL inputs");
        return -1;
    }

    auto cpp = (const std::vector<spi::Variant>*)(v);
    *size = to_int(cpp->size());
    return 0;
}

int spi_Variant_Matrix_item(
    const spi_Variant_Matrix* m,
    int ir, int ic,
    spi_Variant** item)
{
    SPI_C_LOCK_GUARD;
    if (!m || !item)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (const spi::MatrixData<spi::Variant>*)(m);
        size_t r = to_size_t(ir);
        size_t c = to_size_t(ic); 
        if (r >= cpp->Rows() || c >= cpp->Cols())
        {
            spi_Error_set_function(__FUNCTION__, "Array bounds mismatch");
            return -1;
        }
        *item = (spi_Variant*)(new spi::Variant((*cpp)[r][c]));
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Variant_Matrix_set_item(
    spi_Variant_Matrix* m,
    int ir, int ic,
    spi_Variant* item)
{
    SPI_C_LOCK_GUARD;
    if (!m)
    {
        spi_Error_set_function(__FUNCTION__, "NULL pointer");
        return -1;
    }

    try
    {
        auto cpp = (spi::MatrixData<spi::Variant>*)(m);
        size_t r = to_size_t(ir);
        size_t c = to_size_t(ic); 
        if (r >= cpp->Rows() || c >= cpp->Cols())
        {
            spi_Error_set_function(__FUNCTION__, "Array bounds mismatch");
            return -1;
        }
        if (item)
        {
            (*cpp)[r][c] = *(spi::Variant*)(item);
        }
        else
        {
            (*cpp)[r][c] = spi::Variant();
        }
        return 0;
    }
    catch (std::exception& e)
    {
        spi_Error_set_function(__FUNCTION__, e.what());
        return -1;
    }
}

int spi_Variant_Matrix_size(
    const spi_Variant_Matrix* m,
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
        auto cpp = (const spi::MatrixData<spi::Variant>*)(m);
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

