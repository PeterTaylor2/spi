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

#ifndef SPI_C_CSTRING_H
#define SPI_C_CSTRING_H

/*
 * Defines functions and types to be used for the C-interface for std::string.
 *
 * See spi.h for the general rule of the C-interface.
 */

#include "DeclSpec.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct spi_String_Vector spi_String_Vector;
    typedef struct spi_String_Matrix spi_String_Matrix;

    /* String functions */
    SPI_C_IMPORT
    char* spi_String_copy(const char* str);

    SPI_C_IMPORT
    void spi_String_delete(char* str);


    SPI_C_IMPORT
    void spi_String_Vector_delete(spi_String_Vector* v);

    SPI_C_IMPORT
    spi_String_Vector* spi_String_Vector_new(
        int N);

    SPI_C_IMPORT
    int spi_String_Vector_item(
        const spi_String_Vector* v,
        int i,
        char** item /*(O)*/);

    SPI_C_IMPORT
    int spi_String_Vector_set_item(
        spi_String_Vector* v,
        int i,
        const char* item);

    SPI_C_IMPORT
    int spi_String_Vector_size(
        const spi_String_Vector* v,
        int* size /*(O)*/);

    SPI_C_IMPORT
    void spi_String_Matrix_delete(spi_String_Matrix* m);

    SPI_C_IMPORT
    spi_String_Matrix* spi_String_Matrix_new(
        int nr, int nc);

    SPI_C_IMPORT
    int spi_String_Matrix_item(
        const spi_String_Matrix* m,
        int r, int c,
        char** item /*(O)*/);

    SPI_C_IMPORT
    int spi_String_Matrix_set_item(
        spi_String_Matrix* m,
        int r, int c,
        const char* item);

    SPI_C_IMPORT
    int spi_String_Matrix_size(
        const spi_String_Matrix*,
        int* nr /*(O)*/,
        int* nc /*(O)*/);

#ifdef __cplusplus
}
#endif

#endif
