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

#ifndef SPI_C_CMATRIX_H
#define SPI_C_CMATRIX_H

/*
 * Defines functions and types to be used for the C-interface for spi::MatrixData
 * where the underlying type needs no special processing.
 *
 * See spi.h for the general rule of the C-interface.
 */

#include "DeclSpec.h"
#include <stddef.h>
#include "CBool.h"

#define DECLARE_MATRIX_METHODS(T, Import)\
typedef struct T##_Matrix T##_Matrix;\
Import void T##_Matrix_delete(T##_Matrix* m);\
Import T##_Matrix* T##_Matrix_new(int nr, int nc)

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct spi_Int_Matrix spi_Int_Matrix;
    typedef struct spi_Double_Matrix spi_Double_Matrix;
    typedef struct spi_Bool_Matrix spi_Bool_Matrix;

    typedef struct spi_Instance spi_Instance;
    typedef struct spi_Instance_Matrix spi_Instance_Matrix;
    typedef struct spi_Enum spi_Enum;
    typedef struct spi_Enum_Matrix spi_Enum_Matrix;

    /* matrix functions */
    SPI_C_IMPORT
    void spi_Int_Matrix_delete(spi_Int_Matrix* m);

    SPI_C_IMPORT
    spi_Int_Matrix* spi_Int_Matrix_new(
        int nr, int nc);

    SPI_C_IMPORT
    int spi_Int_Matrix_get_data(
        const spi_Int_Matrix* m,
        int nr, int nc,
        int data[]);

    SPI_C_IMPORT
    int spi_Int_Matrix_set_data(
        spi_Int_Matrix* m,
        int nr, int nc,
        int data[]);

    SPI_C_IMPORT
    int spi_Int_Matrix_size(
        const spi_Int_Matrix*,
        int* nr /*(O)*/,
        int* nc /*(O)*/);

    SPI_C_IMPORT
    void spi_Double_Matrix_delete(spi_Double_Matrix* m);

    SPI_C_IMPORT
    spi_Double_Matrix* spi_Double_Matrix_new(
        int nr, int nc);

    SPI_C_IMPORT
    int spi_Double_Matrix_get_data(
        const spi_Double_Matrix* m,
        int nr,
        int nc,
        double data[]);

    SPI_C_IMPORT
    int spi_Double_Matrix_set_data(
        spi_Double_Matrix* m,
        int nr,
        int nc,
        double data[]);

    SPI_C_IMPORT
    int spi_Double_Matrix_size(
        const spi_Double_Matrix*,
        int* nr /*(O)*/,
        int* nc /*(O)*/);

    SPI_C_IMPORT
    void spi_Bool_Matrix_delete(spi_Bool_Matrix* m);

    SPI_C_IMPORT
    spi_Bool_Matrix* spi_Bool_Matrix_new(
        int nr, int nc);

    SPI_C_IMPORT
    int spi_Bool_Matrix_get_data(
        const spi_Bool_Matrix* m,
        int nr,
        int nc,
        System_Bool data[]);

    SPI_C_IMPORT
    int spi_Bool_Matrix_set_data(
        spi_Bool_Matrix* m,
        int nr,
        int nc,
        System_Bool data[]);

    SPI_C_IMPORT
    int spi_Bool_Matrix_size(
        const spi_Bool_Matrix*,
        int* nr /*(O)*/,
        int* nc /*(O)*/);

    SPI_C_IMPORT
    int spi_Instance_Matrix_item(
        const spi_Instance_Matrix* m,
        int r, int c,
        spi_Instance** item /*(O)*/);

    SPI_C_IMPORT
    int spi_Instance_Matrix_set_item(
        spi_Instance_Matrix* m,
        int r, int c,
        spi_Instance* item);

    SPI_C_IMPORT
    int spi_Instance_Matrix_size(
        const spi_Instance_Matrix*,
        int* nr /*(O)*/,
        int* nc /*(O)*/);

    SPI_C_IMPORT
    int spi_Enum_Matrix_get_data(
        const spi_Enum_Matrix* m,
        int nr,
        int nc,
        int data[]);

    SPI_C_IMPORT
    int spi_Enum_Matrix_set_data(
        spi_Enum_Matrix* m,
        int nr,
        int nc,
        int data[]);

    SPI_C_IMPORT
    int spi_Enum_Matrix_size(
        const spi_Enum_Matrix*,
        int* nr /*(O)*/,
        int* nc /*(O)*/);

#ifdef __cplusplus
}
#endif

#endif
