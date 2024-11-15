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

#ifndef SPI_C_CMAP_H
#define SPI_C_CMAP_H

/*
 * Defines functions and types to be used for the C-interface for spi::MatrixData
 * where the underlying type needs no special processing.
 *
 * See spi.h for the general rule of the C-interface.
 */

#include "DeclSpec.h"
#include <stddef.h>

#include "CVector.h"
#include "CMatrix.h"


#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct spi_Map spi_Map;
    typedef struct spi_Object spi_Object;

    /* Map functions */
    SPI_C_IMPORT
        void spi_Map_delete(spi_Map* item);

    DECLARE_VECTOR_METHODS(spi_Map, SPI_C_IMPORT);
    DECLARE_MATRIX_METHODS(spi_Map, SPI_C_IMPORT);

    SPI_C_IMPORT
    spi_Map* spi_Map_dynamic_cast(spi_Object* obj);

#ifdef __cplusplus
}
#endif

#endif
