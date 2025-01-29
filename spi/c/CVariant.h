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

#ifndef SPI_C_CVARIANT_H
#define SPI_C_CVARIANT_H

/*
 * Defines functions and types to be used for the C-interface for spi::Variant.
 *
 * See spi.h for the general rule of the C-interface.
 */

#include "DeclSpec.h"
#include <stddef.h>

#include "CBool.h"
#include "CDate.h"
#include "CDateTime.h"
#include "CMap.h"
#include "CMatrix.h"
#include "CObject.h"
#include "CString.h"
#include "CVector.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct spi_Variant spi_Variant;
    typedef struct spi_Variant_Vector spi_Variant_Vector;
    typedef struct spi_Variant_Matrix spi_Variant_Matrix;

    /* Variant functions */
    SPI_C_IMPORT
    void spi_Variant_delete(spi_Variant* var);

    SPI_C_IMPORT
    spi_Variant* spi_Variant_new_String(const char* str);

    SPI_C_IMPORT
    spi_Variant* spi_Variant_new_Date(spi_Date dt);

    SPI_C_IMPORT
    spi_Variant* spi_Variant_new_DateTime(spi_DateTime dt);

    SPI_C_IMPORT
    spi_Variant* spi_Variant_new_Double(double d);

    SPI_C_IMPORT
    spi_Variant* spi_Variant_new_Bool(spi_Bool b);

    SPI_C_IMPORT
    spi_Variant* spi_Variant_new_Int(int i);

    SPI_C_IMPORT
    int spi_Variant_type(
        spi_Variant* var,
        char** vt);

    SPI_C_IMPORT
    int spi_Variant_array_element_type(
        spi_Variant* var,
        char** vt);

    SPI_C_IMPORT
    int spi_Variant_array_num_dimensions(
        spi_Variant* var,
        int* numDimensions);

    SPI_C_IMPORT
    int spi_Variant_array_dimensions(
        spi_Variant* var,
        int ND,
        int dimensions[]);

    SPI_C_IMPORT
    int spi_Variant_String(
        spi_Variant* var,
        char** str);

    SPI_C_IMPORT
    int spi_Variant_Date(
        spi_Variant* var,
        spi_Date* dt);

    SPI_C_IMPORT
    int spi_Variant_DateTime(
        spi_Variant* var,
        spi_DateTime* dt);

    SPI_C_IMPORT
    int spi_Variant_Double(
        spi_Variant* var,
        double* d);

    SPI_C_IMPORT
    int spi_Variant_Bool(
        spi_Variant* var,
        spi_Bool* b);

    SPI_C_IMPORT
    int spi_Variant_Int(
        spi_Variant* var,
        int* i);

    SPI_C_IMPORT
    int spi_Variant_Object(
        spi_Variant* var,
        spi_Object** obj);

    SPI_C_IMPORT
    int spi_Variant_String_Vector(
        spi_Variant* var,
        spi_String_Vector** str);

    SPI_C_IMPORT
    int spi_Variant_Date_Vector(
        spi_Variant* var,
        spi_Date_Vector** dt);

    SPI_C_IMPORT
    int spi_Variant_DateTime_Vector(
        spi_Variant* var,
        spi_DateTime_Vector** dt);

    SPI_C_IMPORT
    int spi_Variant_Double_Vector(
        spi_Variant* var,
        spi_Double_Vector** d);

    SPI_C_IMPORT
    int spi_Variant_Bool_Vector(
        spi_Variant* var,
        spi_Bool_Vector** b);

    SPI_C_IMPORT
    int spi_Variant_Int_Vector(
        spi_Variant* var,
        spi_Int_Vector** i);

    SPI_C_IMPORT
    int spi_Variant_Object_Vector(
        spi_Variant* var,
        spi_Object_Vector** obj);

    SPI_C_IMPORT
    void spi_Variant_Vector_delete(spi_Variant_Vector* v);

    SPI_C_IMPORT
    spi_Variant_Vector* spi_Variant_Vector_new(
        int N);

    SPI_C_IMPORT
    int spi_Variant_Vector_item(
        const spi_Variant_Vector* v,
        int i,
        spi_Variant** item /*(O)*/);

    SPI_C_IMPORT
    int spi_Variant_Vector_set_item(
        spi_Variant_Vector* v,
        int i,
        spi_Variant* item);

    SPI_C_IMPORT
    int spi_Variant_Vector_size(
        const spi_Variant_Vector* v,
        int* size /*(O)*/);

    SPI_C_IMPORT
    void spi_Map_Vector_delete(spi_Map_Vector* v);

    SPI_C_IMPORT
    void spi_Variant_Matrix_delete(spi_Variant_Matrix* m);

    SPI_C_IMPORT
    spi_Variant_Matrix* spi_Variant_Matrix_new(
        int nr, int nc);

    SPI_C_IMPORT
    int spi_Variant_Matrix_item(
        const spi_Variant_Matrix* m,
        int r, int c,
        spi_Variant** item /*(O)*/);

    SPI_C_IMPORT
    int spi_Variant_Matrix_set_item(
        spi_Variant_Matrix* m,
        int r, int c,
        spi_Variant* item);

    SPI_C_IMPORT
    int spi_Variant_Matrix_size(
        const spi_Variant_Matrix*,
        int* nr /*(O)*/,
        int* nc /*(O)*/);

    SPI_C_IMPORT
    void spi_Variant_Matrix_delete(spi_Variant_Matrix* m);

#ifdef __cplusplus
}
#endif

#endif
