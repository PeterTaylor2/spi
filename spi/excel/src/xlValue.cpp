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
** spi/excel/xlValue.cpp
***************************************************************************
** Functions to convert from XLOPER to/from spi::Value.
**
** XLOPER is rather limited in its data representations - everything is
** a number or a string or an array. Hence we need to use another
** module to convert from an Excel defined spi::Value to basic data types.
**
** However going first to spi::Value will enable us to handle coercion,
** and make decisions on the data type later rather than immediately.
***************************************************************************
*/

#include "xlValue.hpp"
#include "xlUtil.hpp"
#include "xlInput.hpp"
#include "xlOutput.hpp"
#include "xlFuncWizard.hpp"

#include <spi/Array.hpp>
#include <spi/ObjectHandle.hpp>
#include <spi/StringUtil.hpp>
#include <spi/Map.hpp>
#include <spi/MatrixData.hpp>
#include <spi_util/Utils.hpp>

#include <limits>

#define INT(x) spi_util::IntegerCast<int>(x)

#include "xlcall.h"

#include <float.h>

SPI_BEGIN_NAMESPACE

namespace
{
    std::string xlStringToString(const char* in)
    {
        char   buf[257];
        size_t len = (unsigned char)in[0];

        strncpy(buf, &in[1], len);
        buf[len] = '\0';

        return std::string(buf);
    }

#if SPI_XL_VERSION >= 12
    std::string xl12StringToString(const XCHAR* in)
    {
        int cch = in[0];
        if (cch < 0)
            SPI_THROW_RUNTIME_ERROR("String length " << cch << " is less than zero");

        char* ast = (char*)malloc((cch + 1) * sizeof(char));
        if (ast == NULL)
            SPI_THROW_RUNTIME_ERROR("Could not allocate memory");

        // this call taken from Microsoft function XLOper12ToXLOper
        // note that xlcall.h includes <windows.h> (using WIN32_LEAN_AND_MEAN)
        WideCharToMultiByte(CP_ACP, 0, in + 1, cch, ast, cch, NULL, NULL);
        ast[cch] = '\0';
        std::string str(ast);
        free(ast);
        return str;
    }
#endif
}

/*
***************************************************************************
** Convert xloper to Value
***************************************************************************
*/
spi::Value xloperToValue(XLOPER* oper)
{
    if (!oper)
        return spi::Value();

    WORD xltype = oper->xltype & 0x0FFF;

    switch (xltype)
    {
    case xltypeNum:
    {
        double value = oper->val.num;
        return spi::Value(value);
    }
    case xltypeBool:
    {
        bool value = oper->val.boolean != 0;
        return spi::Value(value);
    }
    case xltypeStr:
    {
        const std::string& value = xlStringToString(oper->val.str);
        // should we strip the white space?
        return spi::Value(value);
    }
    case xltypeMissing:
    case xltypeNil:
    {
        return spi::Value(); // undefined
    }
    case xltypeErr:
    {
        switch (oper->val.err)
        {
        case xlerrNum:
            return spi::Value(std::numeric_limits<double>::quiet_NaN());
        case xlerrDiv0:
            return spi::Value(std::numeric_limits<double>::infinity());
        }
        throw ExcelInputError();
    }
    case xltypeMulti:
    {
        // we flatten out arrays in the first implementation
        // we read them from left to right then top to bottom
        int numRows = oper->val.array.rows;
        int numCols = oper->val.array.columns;
        XLOPER* ptr = oper->val.array.lparray;

        if (numRows == 1 && numCols == 1)
        {
            // collapse array of size one to a scalar
            //
            // essentially this is because array of size one really
            // only arises as a return value from a previous function
            // rather than from a value in a single cell
            //
            // hence you might get some strange counter-intuitive
            // effects from treating an array of size one as an array
            return xloperToValue(ptr);
        }

        std::vector<size_t> dimensions;
        dimensions.push_back(spi_util::IntegerCast<size_t>(numRows));
        dimensions.push_back(spi_util::IntegerCast<size_t>(numCols));

        spi::IArraySP anArray(new spi::ValueArray(dimensions));

        size_t i = 0;
        for (int row = 0; row < numRows; ++row)
        {
            for (int col = 0; col < numCols; ++col)
            {
                anArray->assign(i, xloperToValue(ptr));
                ++i;
                ++ptr;
            }
        }
        return spi::Value(spi::IArrayConstSP(anArray));
    }
    default:
        throw spi::RuntimeError("Unsupported xloper type %d",
                                (int)xltype);
    }
}

#if SPI_XL_VERSION >= 12

spi::Value xloper12ToValue(XLOPER12 * oper)
{
    if (!oper)
        return spi::Value();

    WORD xltype = oper->xltype & 0x0FFF;

    switch (xltype)
    {
    case xltypeNum:
    {
        double value = oper->val.num;
        return spi::Value(value);
    }
    case xltypeBool:
    {
        bool value = oper->val.xbool != 0;
        return spi::Value(value);
    }
    case xltypeStr:
    {
        const std::string& value = xl12StringToString(oper->val.str);
        // should we strip the white space?
        return spi::Value(value);
    }
    case xltypeMissing:
    case xltypeNil:
    {
        return spi::Value(); // undefined
    }
    case xltypeErr:
    {
        throw ExcelInputError();
    }
    case xltypeMulti:
    {
        // we flatten out arrays in the first implementation
        // we read them from left to right then top to bottom
        int numRows = oper->val.array.rows;
        int numCols = oper->val.array.columns;
        XLOPER12* ptr = oper->val.array.lparray;

        if (numRows == 1 && numCols == 1)
        {
            // collapse array of size one to a scalar
            //
            // essentially this is because array of size one really
            // only arises as a return value from a previous function
            // rather than from a value in a single cell
            //
            // hence you might get some strange counter-intuitive
            // effects from treating an array of size one as an array
            return xloper12ToValue(ptr);
        }

        std::vector<size_t> dimensions;
        dimensions.push_back(spi_util::IntegerCast<size_t>(numRows));
        dimensions.push_back(spi_util::IntegerCast<size_t>(numCols));

        spi::IArraySP anArray(new spi::ValueArray(dimensions));

        size_t i = 0;
        for (int row = 0; row < numRows; ++row)
        {
            for (int col = 0; col < numCols; ++col)
            {
                anArray->assign(i, xloper12ToValue(ptr));
                ++i;
                ++ptr;
            }
        }
        return spi::Value(spi::IArrayConstSP(anArray));
    }
    default:
        throw spi::RuntimeError("Unsupported xloper type %d",
            (int)xltype);
    }

}

#endif

///*
//**************************************************************************
//* Creates an XLOPER from a Value which is supposed to be an object.
//*************************************************************************
//*/
//XLOPER* xloperMakeFromObjectValue(
//    const spi::Value& in,
//    const std::string& baseName)
//{
//    if (in.getType() == spi::Value::ARRAY)
//    {
//        const std::vector<spi::ObjectConstSP>& objects =
//            in.getConstObjectVector();
//        return xloperFromObjectVector(objects, baseName);
//    }
//    else
//    {
//        const spi::ObjectConstSP& object = in.getConstObject();
//        return xloperFromObject(object, baseName);
//    }
//}

/**
***************************************************************************
** Creates an XLOPER from a Value.
***************************************************************************
*/
XLOPER* xloperMakeFromValue(
    const spi::Value& in,
    bool expandArrays,
    size_t numVars,
    const spi::Value& baseNameValue,
    bool mandatoryBaseName,
    const std::string& baseNamePrefix,
    bool fillBlank)
{
    SPI_PRE_CONDITION(numVars >= 1);

    XLOPER* xloper = xloperMakeEmpty();
    try
    {
        if (numVars == 1)
        {
            std::string baseName = baseNamePrefix + baseNameValue.getString(true);
            xloperSetFromValue(xloper, in, expandArrays, baseName, 0, mandatoryBaseName, fillBlank);
        }
        else
        {
            // we expect Value to be an array of size numVars
            IArrayConstSP vars = in.getArray();
            IArrayConstSP baseNames = baseNameValue.getArray(true);
            size_t nbBaseNames = baseNames->size();
            if (vars->size() != numVars)
                throw spi::RuntimeError("array size mismatch");

            // first of all we need to determine whether any of the
            // outputs are in fact arrays - in that case we need to
            // create a rectangular array
            //
            // otherwise we create a flat array
            //
            // FIXME: if more than one of the values is an object
            // then we will have a problem with baseName
            size_t maxSize = 1;
            for (size_t i = 0; i < numVars; ++i)
            {
                Value var = vars->getItem(i);
                if (var.getType() != Value::ARRAY)
                    continue;
                size_t arraySize = var.getArray()->size();
                if (arraySize > maxSize)
                    maxSize = arraySize;
            }
            XLOPER* x = xloperSetArray(xloper, (int)maxSize, (int)numVars,
                expandArrays);

            int numCols = xloper->val.array.columns;
            int numRows = xloper->val.array.rows;
            SPI_POST_CONDITION(numCols >= (int)numVars);
            SPI_POST_CONDITION(numRows >= (int)maxSize);

            // first variable in first column
            // second variable in second column etc.
            for (size_t i = 0; i < numVars; ++i)
            {
                Value var = vars->getItem(i);
                std::string baseName = baseNamePrefix + (i < nbBaseNames ?
                     baseNames->getItem(i).getString(true) : "");
                size_t arraySize = 0;
                if (var.getType() != Value::ARRAY)
                {
                    xloperSetFromValue(x+i, var, false, baseName, 0, mandatoryBaseName, fillBlank);
                    arraySize = 1;
                }
                else
                {
                    IArrayConstSP values = var.getArray();
                    arraySize = values->size();
                    for (size_t j = 0; j < arraySize; ++j)
                    {
                        xloperSetFromValue(x+i+j*numCols, values->getItem(j), false,
                            baseName, (int)j+1, mandatoryBaseName, fillBlank);
                    }
                }
                if (expandArrays)
                {
                    // TBD: should we try and match the type of the last element
                    for (int j = (int)arraySize; j < numRows; ++j)
                    {
                        XLOPER* xlo = x+i+j*numCols;
                        xloperSetString(xlo, std::string());
                    }
                }
            }
        }
        return xloper;
    }
    catch (...)
    {
        xloperFree(xloper);
        throw;
    }
}

/*
***************************************************************************
** Set the value in an xloper to the given Value.
***************************************************************************
*/
void xloperSetFromValue(
    XLOPER* xloper,
    const spi::Value& in,
    bool expandArrays,
    const std::string& baseName,
    int baseNameIndex,
    bool mandatoryBaseName,
    bool fillBlank)
{
    switch (in.getType())
    {
    case Value::UNDEFINED:
        xloperSetString(xloper, std::string());
        break;
    case Value::CHAR:
        // treat as string of size one
        xloperSetString(xloper, std::string(1, in.getChar()));
        break;
    case Value::STRING:
        xloperSetString(xloper, in.getString());
        break;
    case Value::INT:
        xloper->xltype = xltypeNum;
        xloper->val.num = in.getInt();
        break;
    case Value::DOUBLE:
    {
        double value = in.getDouble();

        if (_isnan(value))
        {
            xloper->xltype = xltypeErr;
            xloper->val.err = xlerrNum;
        }
        else if (!_finite(value))
        {
            xloper->xltype = xltypeErr;
            xloper->val.err = xlerrDiv0;
        }
        else
        {
            xloper->xltype = xltypeNum;
            xloper->val.num = value;
        }
        break;
    }
    case Value::BOOL:
        // treat as bool
        xloper->xltype = xltypeBool;
        xloper->val.boolean = in.getBool() ? 1 : 0;
        break;
    case Value::DATE:
        // treat as double
        xloper->xltype = xltypeNum;
        xloper->val.num = xlDateToDouble(in.getDate());
        break;
    case Value::DATETIME:
        // treat as double
        {
            spi::DateTime dt = in.getDateTime();
            double datePart = xlDateToDouble(dt.Date());
            double timePart = spi::DateTime::TimeToDouble(dt.Time());
            xloper->xltype = xltypeNum;
            xloper->val.num = datePart + timePart;
        }
        break;
    case Value::ARRAY:
    {
        // depending on the dimensions of the output create a
        // rectangular array - we cannot output more than two
        // dimensions in Excel
        //
        // populate the array by calling back into this function
        IArrayConstSP valarray = in.getArray();
        size_t arraySize = valarray->size();
        std::vector<size_t> dimensions = valarray->dimensions();

        size_t numRows;
        size_t numCols;
        switch (dimensions.size())
        {
        case 0:
            numRows = 1;
            numCols = 1;
            break;
        case 1:
            numRows = dimensions[0];
            numCols = 1;
            break;
        case 2:
            numRows = dimensions[0];
            numCols = dimensions[1];
            break;
        default:
            SPI_THROW_RUNTIME_ERROR("Cannot convert " << dimensions.size()
                << " dimensional array to Excel array");
        }
        SPI_POST_CONDITION(arraySize == (numCols*numRows));

        int xNumRows = numRows > 0 ? (int)numRows : 1;
        int xNumCols = numCols > 0 ? (int)numCols : 1;

        XLOPER* xlarray = xloperSetArray(xloper, xNumRows, xNumCols, expandArrays);
        // if we have expandArrays then xNumCols and xNumRows can increase
        xNumCols = xloper->val.array.columns;
        xNumRows = xloper->val.array.rows;
        // but cannot be allowed to decrease
        SPI_POST_CONDITION(xNumRows >= (int)numRows);
        SPI_POST_CONDITION(xNumCols >= (int)numCols);

        // now we use numRows,numCols for indexing into valarray
        // but xNumRows and xNumCols for indexing into xlarray

        for (size_t i = 0; i < numRows; ++i)
        {
            for (size_t j = 0; j < numCols; ++j)
            {
                size_t k = i * numCols + j;
                xloperSetFromValue(&xlarray[i * xNumCols + j], valarray->getItem(k),
                    false, baseName, (int)k + 1, mandatoryBaseName);
            }
        }

        if ((int)numRows < xNumRows)
        {
            // we need to fill each the array with a type that matches the
            // type of the values in the array
            //
            // if the array is empty we use NULL and the rest blanks
            //
            // if the array is not empty we use the type of the last
            // field in each column for the remainder of that column
            //
            // it appears that setting the values to xltypeNil ends
            // up as 0 on the spreadsheet
            if (numRows == 0)
            {
                for (size_t j = 0; j < numCols; ++j)
                {
                    XLOPER* xlo = &xlarray[j];
                    xloperSetString(xlo, "NULL");
                    for (int i = 1; i < xNumRows; ++i)
                    {
                        XLOPER* xlo = &xlarray[i*xNumCols + j];
                        xloperSetString(xlo, std::string());
                    }
                }
            }
            else if (fillBlank)
            {
                for (size_t j = 0; j < numCols; ++j)
                {
                    // fill with blank strings
                    for (int i = (int)numRows; i < xNumRows; ++i)
                    {
                        XLOPER* xlo = &xlarray[i*xNumCols + j];
                        xloperSetString(xlo, std::string());
                    }
                }
            }
            else
            {
                for (size_t j = 0; j < numCols; ++j)
                {
                    Value lastValue = valarray->getItem((numRows - 1)*numCols + j);
                    switch (lastValue.getType())
                    {
                    case Value::INT:
                    case Value::DOUBLE:
                    case Value::DATE:
                    case Value::DATETIME:
                        // fill with zeroes (why was this done in the past?)
                        for (int i = (int)numRows; i < xNumRows; ++i)
                        {
                            XLOPER* xlo = &xlarray[i*xNumCols + j];
                            xlo->xltype = xltypeNum;
                            xlo->val.num = 0.0;
                        }
                        break;
                    default:
                        // fill with blank strings
                        for (int i = (int)numRows; i < xNumRows; ++i)
                        {
                            XLOPER* xlo = &xlarray[i*xNumCols + j];
                            xloperSetString(xlo, std::string());
                        }
                        break;
                    }
                }
            }
        }

        if ((int)numCols < xNumCols)
        {
            // for extra columns we will use #N/A
            for (int i = 0; i < xNumRows; ++i)
            {
                for (int j = (int)numCols; j < xNumCols; ++j)
                {
                    XLOPER* xlo = &xlarray[i*xNumCols + j];
                    xlo->xltype = xltypeErr;
                    xlo->val.err = xlerrNA;

                    // if we change our mind and want to use empty string
                    // then use the following function call instead
                    //xloperSetString(xlo, std::string());
                }
            }
        }
        break;
    }
    case Value::OBJECT:
    {
        bool noCellName = false;
        std::ostringstream fullBaseName;
        if (baseName.length() > 0)
        {
            if (baseName[baseName.length()-1] == '#')
                noCellName = true;
            fullBaseName << baseName;
        }
        else if (mandatoryBaseName)
        {
            throw RuntimeError("No base name provided");
        }

        if (baseNameIndex > 0)
            fullBaseName << "[" << baseNameIndex << "]";

        if (!noCellName)
            fullBaseName << "#" << xlCellName();

        std::string handle = ObjectHandleSave(fullBaseName.str(), in);
        xloperSetFromValue(xloper, Value(handle), false);
        break;
    }
    case Value::MAP:
    {
        MapConstSP vm = in.getMap();
        switch(vm->MapType())
        {
        case Map::VARIANT:
        {
            Variant var(vm);
            xloperSetFromValue(xloper, var.GetValue(), false);
            break;
        }
        case Map::MATRIX:
        {
            MatrixData<Value> matrix = MatrixData<Value>::FromMap(vm);
            int rows = INT(matrix.Rows());
            int cols = INT(matrix.Cols());
            const std::vector<Value>& data = matrix.Data();
            XLOPER* xlarray = xloperSetArray(xloper, rows, cols, expandArrays);
            int xlRows = xloper->val.array.rows;
            int xlCols = xloper->val.array.columns;
            SPI_POST_CONDITION(xlRows >= rows);
            SPI_POST_CONDITION(xlCols >= cols);
            for (int r = 0; r < rows; ++r)
            {
                for (int c = 0; c < cols; ++c)
                {
                    int i = r*cols + c;
                    int j = r*xlCols + c;
                    xloperSetFromValue(&xlarray[j], data[j], false, baseName,
                        (int)i+1, mandatoryBaseName);
                }
            }
            break;
        }
        default:
            throw RuntimeError("Cannot create Excel output from Map");
        }
        break;
    }
    default:
        // create #NUM! error
        xloper->xltype = xltypeErr;
        xloper->xltype = xlerrNum;
        break;
    }
}

XLInputValues::XLInputValues(const char* name)
    :
    iv(name),
    baseName(),
    noCallInWizard(false)
{}

XLInputValues xlGetInputValues(
    FunctionCaller* func,
    int baseNamePos,
    ...)
{
    va_list args;
    va_start(args, baseNamePos);

    try
    {
        XLInputValues x(func->name);
        int nbArgs = (int)func->nbArgs;
        bool baseNameAtEnd = false;
        bool hasBaseName = baseNamePos != 0;
        if (hasBaseName)
        {
            if (baseNamePos == -1)
            {
                baseNameAtEnd = true;
                ++nbArgs;
            }
            else if (baseNamePos < 0 || baseNamePos > nbArgs + 1)
            {
                throw spi::RuntimeError("Bad value for baseNamePos %d", baseNamePos);
            }
            else if (baseNamePos == nbArgs + 1)
            {
                ++nbArgs;
            }
        }

        // extract list of all inputs - then decide how to handle baseName
        std::vector<Value> inputs;
        inputs.reserve(nbArgs);
        for (int i = 0; i < nbArgs; ++i)
        {
            XLOPER* xloper = va_arg(args, XLOPER*);
            inputs.push_back(xloperToValue(xloper));
        }

        if (baseNameAtEnd)
        {
            int nbInputs = 0;
            // work backwards through all the inputs
            // nbInputs is then set when we discover a defined value
            // note that an empty string is defined
            for (int i = 0; i < nbArgs; ++i)
            {
                int j = nbArgs - 1 - i;
                if (!inputs[j].isUndefined())
                {
                    nbInputs = j + 1;
                    break;
                }
            }

            // if we have less inputs than we should then we don't call the
            // function when we are in the function wizard - this means that
            // a working function call with less than the expected inputs
            // will always check for the function wizard - this is an overhead
            // but it does preserve backward compatibility for object constructors
            // when we add extra optional parameters at the end of the argument
            // list
            if (nbInputs < nbArgs)
            {
                x.noCallInWizard = true;
            }
            for (int i = 0; i < nbInputs-1; ++i)
            {
                x.iv.AddValue(func->args[i], inputs[i]);
            }
            for (int i = nbInputs-1; i < nbArgs-1; ++i)
            {
                x.iv.AddValue(func->args[i], Value());
            }
            if (nbInputs > 0)
            {
                x.baseName = inputs[nbInputs - 1];
            }
        }
        else
        {
            for (size_t i = 0; i < func->nbArgs; ++i)
            {
                x.iv.AddValue(func->args[i], inputs[i]);
            }
            if (baseNamePos > 0)
            {
                SPI_POST_CONDITION(baseNamePos <= nbArgs);
                x.baseName = inputs[baseNamePos - 1];
            }
        }
        va_end(args);
        return x;
    }
    catch (std::exception&)
    {
        va_end(args);
        throw;
    }
}

SPI_END_NAMESPACE
