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

#include "CMap.h"

#include "Error.h"

#include "Helper.hpp"

/*
**************************************************************************
* Implementation of spi_Map functions
**************************************************************************
*/
void spi_Map_delete(spi_Map* item)
{
    intrusive_ptr_release((spi::MapObject*)item);
}

void spi_Map_Vector_delete(spi_Map_Vector* v)
{
    spi::Vector_delete<spi::MapObject>(v);
}

spi_Map_Vector* spi_Map_Vector_new(int N)
{
    return spi::Vector_new<spi::MapObject, spi_Map_Vector>(N);
}

int spi_Map_Vector_item(
    spi_Map_Vector* v,
    int i,
    spi_Map** item)
{
    return spi::Vector_item<spi::MapObject>(v, i, item);
}

int spi_Map_Vector_set_item(
    spi_Map_Vector* v,
    int i,
    spi_Map* item)
{
    return spi::Vector_set_item<spi::MapObject>(v, i, item);
}

int spi_Map_Vector_size(
    spi_Map_Vector* v,
    int* size)
{
    return spi::Vector_size<spi::MapObject>(v, size);
}

void spi_Map_Matrix_delete(spi_Map_Matrix* m)
{
    spi::Matrix_delete<spi::MapObject>(m);
}

spi_Map_Matrix* spi_Map_Matrix_new(int nr, int nc)
{
    return spi::Matrix_new<spi::MapObject, spi_Map_Matrix>(nr, nc);
}

int spi_Map_Matrix_item(
    spi_Map_Matrix* m,
    int i, int j,
    spi_Map** item)
{
    return spi::Matrix_item<spi::MapObject>(m, i, j, item);
}

int spi_Map_Matrix_set_item(
    spi_Map_Matrix* m,
    int i, int j,
    spi_Map* item)
{
    return spi::Matrix_set_item<spi::MapObject>(m, i, j, item);
}

int spi_Map_Matrix_size(
    spi_Map_Matrix* m,
    int* nr, int* nc)
{
    return spi::Matrix_size<spi::MapObject>(m, nr, nc);
}
