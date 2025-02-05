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

#ifndef SPI_C_CDATETIME_H
#define SPI_C_CDATETIME_H

/*
 * Defines functions and types to be used for the C-interface for DateTime.
 *
 * See spi.h for the general rule of the C-interface.
 */

#include "DeclSpec.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef double spi_DateTime;
    typedef struct spi_DateTime_Vector spi_DateTime_Vector;
    typedef struct spi_DateTime_Matrix spi_DateTime_Matrix;

#define SPI_DATE_TIME_OFFSET 109205

#if 0
    /* DateTime functions */
    SPI_C_IMPORT
    int spi_DateTime_from_DATE(
        double inp,
        spi_DateTime* out);

    SPI_C_IMPORT
    int spi_DateTime_to_DATE(
        spi_DateTime inp,
        double* out);

    SPI_C_IMPORT
    int spi_DateTime_from_YMDHMS(
        int year, int month, int day, int hours, int minutes, int seconds,
        spi_DateTime* dateTime);

    SPI_C_IMPORT
    int spi_DateTime_YMDHMS(
        spi_DateTime date,
        int* year, int* month, int* day, int* hours, int* minutes, int* seconds);
#endif

    SPI_C_IMPORT
    void spi_DateTime_Vector_delete(spi_DateTime_Vector* v);

    SPI_C_IMPORT
    spi_DateTime_Vector* spi_DateTime_Vector_new(
        int N);

    SPI_C_IMPORT
    int spi_DateTime_Vector_get_data(
        const spi_DateTime_Vector* v,
        int N,
        spi_DateTime data[]);

    SPI_C_IMPORT
    int spi_DateTime_Vector_set_data(
        spi_DateTime_Vector* v,
        int N,
        spi_DateTime data[]);

    SPI_C_IMPORT
    int spi_DateTime_Vector_size(
        const spi_DateTime_Vector* v,
        int* size /*(O)*/);

    SPI_C_IMPORT
    void spi_DateTime_Matrix_delete(spi_DateTime_Matrix* m);

    SPI_C_IMPORT
    spi_DateTime_Matrix* spi_DateTime_Matrix_new(
        int nr, int nc);

    SPI_C_IMPORT
    int spi_DateTime_Matrix_get_data(
        const spi_DateTime_Matrix* m,
        int nr, int nc,
        spi_DateTime data[]);

    SPI_C_IMPORT
    int spi_DateTime_Matrix_set_data(
        spi_DateTime_Matrix* m,
        int nr, int nc,
        spi_DateTime data[]);

    SPI_C_IMPORT
    int spi_DateTime_Matrix_size(
        const spi_DateTime_Matrix*,
        int* nr /*(O)*/,
        int* nc /*(O)*/);


#ifdef __cplusplus
}
#endif

#endif
