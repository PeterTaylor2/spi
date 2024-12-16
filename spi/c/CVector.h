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

#ifndef SPI_C_CVECTOR_H
#define SPI_C_CVECTOR_H

/*
 * Defines functions and types to be used for the C-interface for std::Vector
 * where the underlying type needs no special processing.
 *
 * See spi.h for the general rule of the C-interface.
 */

#include "DeclSpec.h"
#include <stddef.h>
#include "CBool.h"

#define DECLARE_VECTOR_METHODS(T, Import)\
typedef struct T##_Vector T##_Vector;\
Import void T##_Vector_delete(T##_Vector* v);\
Import T##_Vector* T##_Vector_new(int N);

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct spi_Int_Vector spi_Int_Vector;
    typedef struct spi_Double_Vector spi_Double_Vector;
    typedef struct spi_Bool_Vector spi_Bool_Vector;

    typedef struct spi_Instance spi_Instance;
    typedef struct spi_Instance_Vector spi_Instance_Vector;
    typedef struct spi_Enum spi_Enum;
    typedef struct spi_Enum_Vector spi_Enum_Vector;

    /* vector functions */
    SPI_C_IMPORT
    void spi_Int_Vector_delete(spi_Int_Vector* v);

    SPI_C_IMPORT
    spi_Int_Vector* spi_Int_Vector_new(
        int N);

    /*SPI_C_IMPORT*/
    spi_Int_Vector* spi_Int_Vector_make(
        int N,
        int data[]);

    SPI_C_IMPORT
    int spi_Int_Vector_get_data(
        const spi_Int_Vector* v,
        int N,
        int data[]);

    SPI_C_IMPORT
    int spi_Int_Vector_set_data(
        spi_Int_Vector* v,
        int N,
        int data[]);

    /*SPI_C_IMPORT*/
    int spi_Int_Vector_item(
        const spi_Int_Vector* v,
        int i,
        int* item /*(O)*/);

    /*SPI_C_IMPORT*/
    int spi_Int_Vector_set_item(
        spi_Int_Vector* v,
        int i,
        int item);

    SPI_C_IMPORT
    int spi_Int_Vector_size(
        const spi_Int_Vector* v,
        int* size /*(O)*/);

    SPI_C_IMPORT
    void spi_Double_Vector_delete(spi_Double_Vector* v);

    SPI_C_IMPORT
    spi_Double_Vector* spi_Double_Vector_new(
        int N);

    /*SPI_C_IMPORT*/
    spi_Double_Vector* spi_Double_Vector_make(
        int N,
        double data[]);

    SPI_C_IMPORT
    int spi_Double_Vector_get_data(
        const spi_Double_Vector* v,
        int N,
        double data[]);

    SPI_C_IMPORT
    int spi_Double_Vector_set_data(
        spi_Double_Vector* v,
        int N,
        double data[]);

    /*SPI_C_IMPORT*/
    int spi_Double_Vector_item(
        const spi_Double_Vector* v,
        int i,
        double* item /*(O)*/);

    /*SPI_C_IMPORT*/
    int spi_Double_Vector_set_item(
        spi_Double_Vector* v,
        int i,
        double item);

    SPI_C_IMPORT
    int spi_Double_Vector_size(
        const spi_Double_Vector* v,
        int* size /*(O)*/);

    SPI_C_IMPORT
    void spi_Bool_Vector_delete(spi_Bool_Vector* v);

    SPI_C_IMPORT
    spi_Bool_Vector* spi_Bool_Vector_new(
        int N);

    /*SPI_C_IMPORT*/
    spi_Bool_Vector* spi_Bool_Vector_make(
        int N,
        spi_Bool data[]);

    SPI_C_IMPORT
    int spi_Bool_Vector_get_data(
        const spi_Bool_Vector* v,
        int N,
        spi_Bool data[]);

    SPI_C_IMPORT
    int spi_Bool_Vector_set_data(
        spi_Bool_Vector* v,
        int N,
        spi_Bool data[]);

    /*SPI_C_IMPORT*/
    int spi_Bool_Vector_item(
        const spi_Bool_Vector* v,
        int i,
        int* item /*(O)*/);

    /*SPI_C_IMPORT*/
    int spi_Bool_Vector_set_item(
        spi_Bool_Vector* v,
        int i,
        spi_Bool item);

    SPI_C_IMPORT
    int spi_Bool_Vector_size(
        const spi_Bool_Vector* v,
        int* size /*(O)*/);

    /*SPI_C_IMPORT*/
    spi_Instance_Vector* spi_Instance_Vector_make(
        int N,
        spi_Instance* data[]);

    SPI_C_IMPORT
    int spi_Instance_Vector_get_data(
        spi_Instance_Vector* v,
        int N,
        spi_Instance* data[]);

    SPI_C_IMPORT
    int spi_Instance_Vector_set_data(
        spi_Instance_Vector* v,
        int N,
        spi_Instance* data[]);

    /*SPI_C_IMPORT*/
    int spi_Instance_Vector_item(
        const spi_Instance_Vector* v,
        int i,
        spi_Instance** item /*(O)*/);

    /*SPI_C_IMPORT*/
    int spi_Instance_Vector_set_item(
        spi_Instance_Vector* v,
        int i,
        spi_Instance* item);

    SPI_C_IMPORT
    int spi_Instance_Vector_size(
        const spi_Instance_Vector* v,
        int* size /*(O)*/);

    SPI_C_IMPORT
    int spi_Enum_Vector_set_data(
        spi_Enum_Vector* v,
        int N,
        int data[]);

    SPI_C_IMPORT
    int spi_Enum_Vector_get_data(
        spi_Enum_Vector* v,
        int N,
        int data[]);

    /*SPI_C_IMPORT*/
    int spi_Enum_Vector_item(
        const spi_Enum_Vector* v,
        int i,
        int* item /*(O)*/);

    /*SPI_C_IMPORT*/
    int spi_Enum_Vector_set_item(
        spi_Enum_Vector* v,
        int i,
        int item);

    SPI_C_IMPORT
    int spi_Enum_Vector_size(
        const spi_Enum_Vector* v,
        int* size /*(O)*/);

#ifdef __cplusplus
}
#endif

#endif
