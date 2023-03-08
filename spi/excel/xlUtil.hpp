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
#ifndef SPI_XL_UTIL_HPP
#define SPI_XL_UTIL_HPP

/**
 * Various utility functions for Excel.
 *
 * This should be combined with framewrk.h.
 */

#include "DeclSpec.h"
#include <spi/Namespace.hpp>
#include <spi/Date.hpp>
#include <string>

typedef struct xloper XLOPER;

SPI_BEGIN_NAMESPACE


/**
***************************************************************************
** xloperFree
**
** Frees an xloper that was dynamically allocated inside the library.
**
** All add-ins need a function called xlAutoFree which should just call
** this function.
***************************************************************************
*/
SPI_XL_IMPORT
void xloperFree(XLOPER* xloper);

/**
***************************************************************************
** xloperMakeEmpty
**
** Creates an empty xloper.
**
** Memory management is consistent with xloperFree.
***************************************************************************
*/
XLOPER* xloperMakeEmpty();

/**
***************************************************************************
** xloperSetString
**
** Given an XLOPER this function will set the value to a string.
** If the string is greater than 255 in length it will be truncated.
**
** Memory management is consistent with xloperFree.
***************************************************************************
*/
void xloperSetString(XLOPER* xloper, const std::string& str);

/**
***************************************************************************
** xloperSetArray
**
** Given an XLOPER this function will create an array where the elements
** are all empty. Note that the array is accessed left to right, then
** top to bottom.
**
** The array of XLOPER is returned for convenience.
**
** Memory management is consistent with xloperFree, but you need to set
** the elements of the array later.
**
** NOTE: this function may expand the output to fit the calling area
** Hence you must take care when populating the array of XLOPER returned
***************************************************************************
*/
XLOPER* xloperSetArray(XLOPER* xloper, int numRows, int numCols, bool expand);

/**
 * Convert double to Date.
 */
spi::Date xlDoubleToDate(double in);

/**
 * Convert Date to double.
 */
double xlDateToDouble(spi::Date in);

/**
 * Reset the date offset for each function call. This is because the Excel
 * date system is workbook specific.
 */
void xlDateReset();

/**
***************************************************************************
** Returns a string representing the cell name which called the XLL.
**
** If we were called from VBA we might not have a cell name - in which
** case we return "None" rather than throw an exception.
***************************************************************************
*/
std::string xlCellName();

/**
***************************************************************************
** Call this when you load an add-in to initialise various state variables
***************************************************************************
*/
SPI_XL_IMPORT
void xlInitializeState();

/**
 * Returns the excel version number string 
 */
const std::string& xlVersionString();

/**
 * Returns the excel major version number.
 */
int xlMajorVersion();

SPI_END_NAMESPACE

#endif // SPI_XL_UTIL_HPP

