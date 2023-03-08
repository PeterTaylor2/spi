#include "xltest.hpp"

#include "excel.h"

#include <spi/RuntimeError.hpp>
#include <stdarg.h>

#include <spi/excel/xlUtil.hpp>
#include <spi/excel/xlValue.hpp>

SPI_BEGIN_NAMESPACE

void RegisterXLL(const std::string& name)
{
    if (xllOpen(name.c_str()) != 0)
        SPI_THROW_RUNTIME_ERROR("Could not register " << name);
}

void UnregisterXLL(const std::string& name)
{
    xllClose(name.c_str());
}

Value CallXLL(const std::string& funcName, int count, ...)
{
    std::vector<Value> vArgs;
    va_list args;

    va_start(args, count);
    for (int i = 0; i < count; ++i)
    {
        Value arg = va_arg(args, Value);
        vArgs.push_back(arg);
    }
    va_end(args);

    // we need to create some temporary LPXLOPERs for the call to the Excel API
    // we will be deleting them at the end
    std::vector<LPXLOPER> opers;
    Value output;
    try
    {
        for (int i = 0; i < count; ++i)
        {
            opers.push_back(xloperMakeFromValue(vArgs[i]));
        }
        XL_OUTPUT xlo = CallXLFunction(funcName.c_str(), count, count > 0 ? &opers[0] : NULL);
        output = xloperToValue(xlo.value);
        FreeXLOutput(&xlo);
    }
    catch (std::exception& e)
    {
        output = Value(e);
    }

    for (std::vector<LPXLOPER>::const_iterator iter = opers.begin();
        iter != opers.end(); ++iter)
    {
        xloperFree(*iter);
    }

    return output;
}

SPI_END_NAMESPACE

#if SPI_XL_VERSION == 12
int PASCAL MdCallBack12(int xlfn, int count, LPXLOPER12 opers[], LPXLOPER12 result)
{
    return CallExcel12(xlfn, count, opers, result);
}
#endif

