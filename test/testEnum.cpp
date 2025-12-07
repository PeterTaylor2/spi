#include <spi/EnumHelper.hpp>
#include <spi/RuntimeError.hpp>
#include <spi/Value.hpp>

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <stdio.h>

#include <spi_util/TestRunner.hpp>

/*
***************************************************************************
** Implementation of DayCountConv enumerated type.
***************************************************************************
*/
class DayCountConv
{
public:
    enum Enum
    {
        ACT_ACT = 1,
        ACT_365F = 2,
        ACT_360 = 3,
        B30_360 = 4,
        B30E_360 = 5,
	UNINITIALIZED_VALUE=6
    };
    SPI_DECLARE_ENUM_STRING_CONV(DayCountConv);

    DayCountConv(DayCountConv::Enum value) : value(value) {}
    operator DayCountConv::Enum() const { return value; }
private:
    DayCountConv::Enum value;
};

DayCountConv::Enum DayCountConv::from_string(const char* str)
{
    switch(toupper(*str))
    {
    case 'A':
        if (_stricmp(str, "ACT/ACT") == 0)
            return DayCountConv::ACT_ACT;
        if (_stricmp(str, "ACT/365F") == 0)
            return DayCountConv::ACT_365F;
        if (_stricmp(str, "ACT/360") == 0)
            return DayCountConv::ACT_360;
        break;
    case 'B':
        if (_stricmp(str, "B30/360") == 0)
            return DayCountConv::B30_360;
        if (_stricmp(str, "B30E/360") == 0)
            return DayCountConv::B30E_360;
        break;
    }
    throw spi::RuntimeError("Could not convert %s to DayCountConv", str);
}

const char* DayCountConv::to_string(DayCountConv::Enum value)
{
    switch(value)
    {
    case DayCountConv::ACT_ACT:
        return "ACT/ACT";
    case DayCountConv::ACT_365F:
        return "ACT/365F";
    case DayCountConv::ACT_360:
        return "ACT/360";
    case DayCountConv::B30_360:
        return "B30/360";
    case DayCountConv::B30E_360:
        return "B30E/360";
    default:
        throw spi::RuntimeError("Bad DayCountConv::Enum %d", (int)value);
    }
}

static void testOneDayCountConv(DayCountConv::Enum value)
{
    DayCountConv dcc = value;
    const char* str = dcc;
    DayCountConv dcc2 = str;
    DayCountConv::Enum value2 = dcc2;

    std::cout << "testing day count convention: "
              << (DayCountConv::Enum)dcc << " - "
              << (const char*) dcc << std::endl;

    SPI_POST_CONDITION(value == value2);
}

static void testDayCountConv(void)
{
    testOneDayCountConv(DayCountConv::ACT_ACT);
    testOneDayCountConv(DayCountConv::ACT_365F);
    testOneDayCountConv(DayCountConv::ACT_360);
    testOneDayCountConv(DayCountConv::B30_360);
    testOneDayCountConv(DayCountConv::B30E_360);
}

int main(int argc, char* argv[])
{
    return spi_util::TestRunner(argc, argv,
                      "DayCountConv", testDayCountConv,
                      0);
}
