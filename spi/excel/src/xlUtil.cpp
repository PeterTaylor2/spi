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
/**
***************************************************************************
** spi/excel/xlUtil.cpp
***************************************************************************
** Various excel utility functions.
***************************************************************************
*/

#include "xlUtil.hpp"

#include "xlcall.h"
#include "xlInput.hpp"
#include "framewrk.h"

#include <spi/RuntimeError.hpp>
#include <spi/StringUtil.hpp>
#include <spi/CommonRuntime.hpp>

#define NEW(T)         (T*)calloc(sizeof(T),1)
#define NEW_ARRAY(T,n) (T*)calloc(sizeof(T),n)
#define FREE(p)        if (p) free(p)

SPI_BEGIN_NAMESPACE

namespace
{

void xloperFreeContents(XLOPER* xloper);

/*
***************************************************************************
** xloperFreeContents
**
** Frees the contents of an XLOPER.
***************************************************************************
*/
void xloperFreeContents(XLOPER* xloper)
{
    if (xloper)
    {
        if (xloper->xltype & xltypeMulti)
        {
            size_t  size; /* Size of the array */
            size_t  pos;  /* Iterator for array */
            XLOPER* xpos; /* XLOPER at pos */

            size = xloper->val.array.rows * xloper->val.array.columns;
            for (pos = 0; pos < size; ++pos)
            {
                xpos = xloper->val.array.lparray + pos;
                xloperFreeContents(xpos);
            }
            FREE (xloper->val.array.lparray);
        }
        else if (xloper->xltype & xltypeStr)
        {
            FREE (xloper->val.str);
        }
        xloper->xltype = 0;
    }
}

int EXCEL_DATE_OFFSET = 0;
int xlGetDateOffset()
{
    if (EXCEL_DATE_OFFSET == 0)
    {
        XLOPER x;

        EXCEL_DATE_OFFSET = 109205; /* Corresponds to 1/1/1901 */

        /*
        ** We get the date for 1/1/1904.
        ** If date format is 1904, then the result will be zero.
        ** Otherwise the result will be 4 years worth.
        */
        Excel (xlfDate, (LPXLOPER)&x, 3,
               TempNum(1904), TempNum(1), TempNum(1));

        if (x.xltype & xltypeNum)
        {
            if (x.val.num <= 1)
            { /* 1904 date system in use */
                EXCEL_DATE_OFFSET += 1462;
            }
        }
    }
    return EXCEL_DATE_OFFSET;
}

} // end of anonymous namespace

/*
***************************************************************************
** xloperFree
**
** Frees an xloper that was dynamically allocated inside the library.
**
** All add-ins need a function called xlAutoFree which should just call
** this function.
***************************************************************************
*/
void xloperFree(XLOPER* xloper)
{
    if (xloper)
    {
        xloperFreeContents(xloper);
        FREE(xloper);
    }
}

/**
***************************************************************************
** xloperMakeEmpty
**
** Creates an empty xloper.
**
** Memory management is consistent with xloperFree.
**
** Just before you return the XLOPER to Excel you need to add the bit
** which tells Excel to call you back (xlAutoFree) to free the XLOPER
** memory.
***************************************************************************
*/
XLOPER* xloperMakeEmpty()
{
    return NEW(XLOPER);
}

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
void xloperSetString(XLOPER* xloper, const std::string& str)
{
    size_t len = str.length();
    if (len > 255)
        len = 255;

    xloper->val.str = NEW_ARRAY(char, len+1);
    if (xloper->val.str == NULL)
        throw RuntimeError("Memory allocation failure");

    xloper->xltype = xltypeStr;
    xloper->val.str[0] = (unsigned char)len;
    if (len > 0)
        strncpy(xloper->val.str+1, str.c_str(), len);
}

static void xlCallArea
(int *rows,
 int *cols)
{
    XLOPER xloper;

    // There are two ways for Excel to fail to get the call area.
    // 1. xlfCaller does not return xlretSuccess
    // 2. xlfCaller returns an XLOPER which is not of type xltypeSRef
    if (Excel(xlfCaller, &xloper, 0) != xlretSuccess)
    {
        throw std::runtime_error("Could not find area from which function was called");
    }

    if (xloper.xltype != xltypeSRef)
    {
        // now we have to free the reference before throwing an exception
        Excel(xlFree, 0, 1, &xloper);
        throw std::runtime_error("Could not find area from which function was called");
    }

    *rows = xloper.val.sref.ref.rwLast - xloper.val.sref.ref.rwFirst + 1;
    *cols = xloper.val.sref.ref.colLast - xloper.val.sref.ref.colFirst + 1;

    // it would be nice to know if there is a TRANSPOSE in the function call
    // then we could switch rows and columns
    // however I cannot find any such method in the Excel4 SDK

    // free the Excel reference before returning
    Excel(xlFree, 0, 1, &xloper);
}

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
***************************************************************************
*/
XLOPER* xloperSetArray(XLOPER* xloper, int numRows, int numCols, bool expand)
{
    // inside the structure rows and columns are of type WORD
    // this is much smaller than int, so we had better validate
    static int maxWord = 0xffff;

    if (numRows > maxWord || numRows < 0)
    {
        throw RuntimeError("Number of rows %d out of range", int(numRows));
    }

    if (numCols > maxWord || numCols < 0)
    {
        throw RuntimeError("Number of columns %d out of range", (int)numCols);
    }

    if (numRows == 0 || numCols == 0)
    {
        xloper->xltype = xltypeNil;
        return NULL;
    }

    // for an array we expand to fill the calling area
    // potentially this should be a matter for configuration
    if (expand)
    {
        try
        {
            int callRows, callCols;
            xlCallArea(&callRows, &callCols);
            if (callRows > numRows)
                numRows = callRows;

            if (callCols > numCols)
                numCols = callCols;
        }
        catch (...)
        {
            // ignore failures
        }
    }

    xloper->val.array.lparray = NEW_ARRAY(XLOPER, numRows * numCols);
    if (!xloper->val.array.lparray)
        throw RuntimeError("Memory allocation failure");

    xloper->val.array.rows    = (WORD)numRows;
    xloper->val.array.columns = (WORD)numCols;
    xloper->xltype            = xltypeMulti;

    int arraySize = numRows * numCols;
    for (int i = 0; i < arraySize; ++i)
        xloper->val.array.lparray[i].xltype = xltypeNil;

    return xloper->val.array.lparray;
}

/*
***************************************************************************
** Convert from double to Date
***************************************************************************
*/
spi::Date xlDoubleToDate(double in)
{
    int dt;
    if (in > 0)
    {
        int offset = xlGetDateOffset();
        dt = (int)in + offset;
    }
    else
    {
        dt = 0;
    }
    return spi::Date(dt);
}

/*
***************************************************************************
* Convert Date to double.
***************************************************************************
*/
double xlDateToDouble(spi::Date in)
{
    int offset = xlGetDateOffset();
    double out = (int)in - offset;

    if (out < 0.0)
        out = 0.0;

    return out;
}


/*
***************************************************************************
* Reset the date offset for each function call. This is because the Excel
* date system is workbook specific.
***************************************************************************
*/
void xlDateReset()
{
    EXCEL_DATE_OFFSET = 0;
}

/*
***************************************************************************
** Gets the current cell name.
**
** We use xlfCaller to get an XLOPER which represents the current cell.
**
** Then we use xlSheetNm and xlfAddress to break down this XLOPER into
** the sheet name and cell address.
***************************************************************************
*/
std::string xlCellName()
{

#if SPI_XL_VERSION >= 12

    std::string cellName;

    XLOPER12 xCaller;
    XLOPER12 xSheet;
    XLOPER12 xAddress;

    bool freeCaller = false;
    bool freeSheet = false;
    bool freeAddress = false;

    int status = -1;


    if (Excel12(xlfCaller, &xCaller, 0) != xlretSuccess ||
        !(xCaller.xltype & xltypeSRef))
    {
        goto done; /* failure */
    }
    freeCaller = true;

    if (Excel12(xlSheetNm, &xSheet, 1, &xCaller) != xlretSuccess ||
        !(xSheet.xltype & xltypeStr))
    {
        goto done; /* failure */
    }
    freeSheet = true;

    if (Excel12f(xlfAddress,
        &xAddress,
        3,
        TempNum12(xCaller.val.sref.ref.rwFirst + 1),
        TempNum12(xCaller.val.sref.ref.colFirst + 1),
        TempNum12(4)) != xlretSuccess ||
        !(xAddress.xltype & xltypeStr))
    {
        goto done; /* failure */
    }
    freeAddress = true;

    {
        std::string sheet = xloper12ToString(&xSheet, "sheet");
        std::string address = xloper12ToString(&xAddress, "address");
        cellName = StringFormat("%s!%s", sheet.c_str(), address.c_str());
    }
    status = 0;

done:

    if (freeCaller)
        Excel12(xlFree, 0, 1, &xCaller);

    if (freeSheet)
        Excel12(xlFree, 0, 1, &xSheet);

    if (freeAddress)
        Excel12(xlFree, 0, 1, &xAddress);

    if (status != 0)
        return "None";

    return cellName;


#else

    std::string cellName;

    XLOPER xCaller;
    XLOPER xSheet;
    XLOPER xAddress;

    bool freeCaller = false;
    bool freeSheet = false;
    bool freeAddress = false;

    int status = -1;

    if (Excel (xlfCaller, &xCaller, 0) != xlretSuccess ||
        !(xCaller.xltype & xltypeSRef))
    {
        goto done; /* failure */
    }
    freeCaller = true;

    if (Excel (xlSheetNm, &xSheet, 1, &xCaller) != xlretSuccess ||
        !(xSheet.xltype & xltypeStr))
    {
        goto done; /* failure */
    }
    freeSheet = true;

    if (Excel (xlfAddress,
               &xAddress,
               3,
               TempNum(xCaller.val.sref.ref.rwFirst + 1),
               TempNum(xCaller.val.sref.ref.colFirst + 1),
               TempNum(4)) != xlretSuccess ||
        !(xAddress.xltype & xltypeStr))
    {
        goto done; /* failure */
    }
    freeAddress = true;

    {
        std::string sheet   = xloperToString(&xSheet, "sheet");
        std::string address = xloperToString(&xAddress, "address");
        cellName = StringFormat("%s!%s", sheet.c_str(), address.c_str());
    }
    status = 0;

done:

    if (freeCaller)
        Excel (xlFree,0,1,&xCaller);

    if (freeSheet)
        Excel (xlFree,0,1,&xSheet);

    if (freeAddress)
        Excel (xlFree,0,1,&xAddress);

    if (status != 0)
        return "None";

    return cellName;
#endif

}

std::string g_xl_version_string;
int g_xl_major_version = 0;

/**
***************************************************************************
** Call this when you load an add-in to initialise various state variables
***************************************************************************
*/
void xlInitializeState()
{
    InitFramework();

    try
    {
        XLOPER x;
        Excel(xlfGetWorkspace, &x, 1, TempInt(2));
        g_xl_version_string = spi::xloperToString(&x, "GetWorkspace (2)");
        Excel(xlFree, 0, 1, &x);
    }
    catch (...)
    {
        g_xl_version_string.clear();
        Excel(xlcAlert, 0, 2,
            TempStrConst("Excel xlfGetWorkspace function failed"),
            TempInt(2));
    }

    if (g_xl_version_string.empty())
    {
        g_xl_major_version = 0;
    }
    else
    {
        try
        {
            std::vector<std::string> versionParts = spi_util::StringSplit(
                g_xl_version_string, '.');
            g_xl_major_version = spi_util::StringToInt(versionParts[0]);
        }
        catch (...)
        {
            g_xl_major_version = 0;
            Excel(xlcAlert, 0, 2,
                TempStrConst("Could not get major version number from version string"),
                TempInt(2));
        }
    }

    try
    {
        spi::CommonRuntime::AddContext("EXCEL");
    }
    catch (std::exception& e)
    {
        Excel(xlcAlert, 0, 2, TempStrConst(e.what()), TempInt(2));
    }
}

/**
* Returns the excel version number string
*/
const std::string& xlVersionString()
{
    return g_xl_version_string;
}

/**
* Returns the excel major version number.
*/
int xlMajorVersion()
{
    return g_xl_major_version;
}

SPI_END_NAMESPACE
