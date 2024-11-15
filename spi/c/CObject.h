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

#ifndef SPI_C_COBJECT_H
#define SPI_C_COBJECT_H

/*
 * Defines functions and types to be used for the C-interface for spi::Object.
 *
 * See spi.h for the general rule of the C-interface.
 */


#include "DeclSpec.h"
#include <stddef.h>
#include "CVector.h"
#include "CMatrix.h"
#include "CString.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct spi_Object spi_Object;
    typedef struct spi_Variant spi_Variant;

    /* Object functions */
    SPI_C_IMPORT
    void spi_Object_delete(spi_Object* item);

    SPI_C_IMPORT
    int spi_Object_get_object_id(spi_Object* item, char** objectId);

    SPI_C_IMPORT
    int spi_Object_get_class_name(spi_Object* item, char** className);

    SPI_C_IMPORT
    int spi_Object_to_string(
        spi_Object* self,
        const char* format,
        const char* options,
        char** str);

    SPI_C_IMPORT
    int spi_Object_to_file(
        spi_Object* self,
        const char* filename,
        const char* format,
        const char* options);

    SPI_C_IMPORT
    spi_Object* spi_Object_from_string(
        const char* str);

    SPI_C_IMPORT
    spi_Object* spi_Object_from_file(
        const char* filename);

    SPI_C_IMPORT
    spi_Variant* spi_Object_get_value(
        spi_Object* self,
        const char* name);

    /* Object handle functions */
    SPI_C_IMPORT
    int spi_Object_handle_save(
        const char* baseName,
        spi_Object* obj,
        spi_Bool noCount,
        char** handle);

    SPI_C_IMPORT
    int spi_Object_handle_find(
        const char* handle,
        spi_Object** obj);

    SPI_C_IMPORT
    int spi_Object_handle_free_all(
        int *count);

    SPI_C_IMPORT
    int spi_Object_handle_count(
        const char* className,
        int* count);

    SPI_C_IMPORT
    int spi_Object_handle_free(
        const char* handle);

    SPI_C_IMPORT
    int spi_Object_handle_list(
        const char* baseName,
        const char* className,
        spi_String_Vector** handles);

    SPI_C_IMPORT
    int spi_Object_handle_class_name(
        const char* handle,
        char** className);

    /* vector functions */
    typedef struct spi_Object_Vector spi_Object_Vector;

    SPI_C_IMPORT
    void spi_Object_Vector_delete(spi_Object_Vector* v);

    SPI_C_IMPORT
    spi_Object_Vector* spi_Object_Vector_new(
        int N);

    SPI_C_IMPORT
    int spi_Object_Vector_item(
        const spi_Object_Vector* v,
        int i,
        spi_Object** item /*(O)*/);

    SPI_C_IMPORT
    int spi_Object_Vector_set_item(
        spi_Object_Vector* v,
        int i,
        spi_Object* item);

    SPI_C_IMPORT
    int spi_Object_Vector_size(
        const spi_Object_Vector* v,
        int* size /*(O)*/);

    /* matrix functions */
    typedef struct spi_Object_Matrix spi_Object_Matrix;

    SPI_C_IMPORT
    void spi_Object_Matrix_delete(spi_Object_Matrix* m);

    SPI_C_IMPORT
    spi_Object_Matrix* spi_Object_Matrix_new(int nr, int nc);

    SPI_C_IMPORT
    int spi_Object_Matrix_item(
        const spi_Object_Matrix* m,
        int i, int j,
        spi_Object** item);

    SPI_C_IMPORT
    int spi_Object_Matrix_set_item(
        spi_Object_Matrix* m,
        int i, int j,
        spi_Object* item);

    SPI_C_IMPORT
    int spi_Object_Matrix_size(
        const spi_Object_Matrix* m,
        int* nr, int* nc);


#ifdef __cplusplus
}
#endif

#endif
