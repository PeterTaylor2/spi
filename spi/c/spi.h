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

#ifndef SPI_C_SPI_H
#define SPI_C_SPI_H

/*
 * Defines functions and types to be used for the C-interface.
 *
 * The general rule of the C-interface is as follows:
 *
 * 1. Constructor functions return a pointer on success, NULL on failure.
 *    The caller is responsible for freeing the pointer using the corresponding
 *    delete function for that type.
 *
 * 2. Destructor functions have void return type.
 *
 * 3. All other functions return int - a value of 0 indicates success, any other
 *    value indicates failure.
 *
 * 4. On error (for functions type (1) and (3)) then you should call spi_Error_get
 *    function to collect the error - this is simply a c-string which you can use
 *    in an exception. The string returned by spi_Error_get remains owned by the
 *    library and you should not delete it.
 *
 * 5. Sub-classes of Object are returned as the raw pointer for which the reference
 *    count has been incremented by 1. Hence in the C# code you need to wrap this
 *    pointer without incrementing its reference count.
 *
 *    Hence in the C# hierarchy the pointer is only stored at the level of Object.
 *    It will be an instance of the corresponding C++ class for any class in that
 *    hierarchy so when you pass it into a function expecting a sub-class of Object
 *    it should work just fine.
 *
 * 6. A vector or matrix is returned as a pointer to the underlying C++ type (vector
 *    or matrix). Hence there needs to be a vector or matrix for each underlying C++
 *    type for which we use vector or matrix. There are the following functions for
 *    each such vector (or matrix):
 *        a. new - given the size creates an empty vector (or matrix).
 *        b. delete - deletes the vector (or matrix).
 *        c. size - returns the size of the vector (or matrix).
 *        d. set_item - puts an item into the vector (or matrix) at a given position
 *           (the position will be bounds checked)
 *        e. item - gets an item from the vector (or matrix) from a given position
 *           (the position will be bounds checked)
 *
 *    The set_item/item function will take as input/output the C-version of the type
 *    in C#. These may then need to be converted to the C++ type used within the
 *    vector (or matrix).
 *
 *    Potentially if there is no conversion needed (or it is automatically marshalled)
 *    then we could represent the vector as two inputs (array and size of array) - this
 *    would avoid the need for repeated crossing of the P/INVOKE layer to set/get
 *    individual elements of the vector.
 *
 * Exceptions to this policy will be noted for any particular function.
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
#include "CVariant.h"
#include "CVector.h"
#include "Error.h"

#endif
