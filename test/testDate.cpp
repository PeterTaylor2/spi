#include <spi/Date.hpp>
#include <spi/RuntimeError.hpp>

#include <isda/dateconv.h>

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <stdio.h>

#include <spi_util/TestRunner.hpp>

/*
***************************************************************************
** Implementation of various date functions via ISDA code.
***************************************************************************
*/
void DateToYMD(int date, int* pYear, int* pMonth, int* pDay)
{
    TMonthDayYear mdy;

    if (JpmcdsDateToMDY((TDate)date, &mdy) != SUCCESS)
        throw spi::RuntimeError("Could not convert %d to MDY", date);

    *pYear  = mdy.year;
    *pMonth = mdy.month;
    *pDay   = mdy.day;
}

int YMDToDate(int year, int month, int day)
{
    int date = JpmcdsDate(year, month, day);
    if (date == -1)
        throw spi::RuntimeError("Could not convert %d,%d,%d to date",
                                year, month, day);

    return (int)date;
}

static void testDate(int date)
{
    int year;
    int month;
    int day;

    int year2;
    int month2;
    int day2;

    spi::Date(date).YMD(&year,&month,&day);
    DateToYMD(date, &year2, &month2, &day2);

    SPI_POST_CONDITION(year == year2);
    SPI_POST_CONDITION(month == month2);
    SPI_POST_CONDITION(day == day2);
    SPI_POST_CONDITION(date == YMDToDate(year, month, day));
}

static void testDates(void)
{
    int start = 0;
    int end   = 365 * 600;

    for (int date = start; date <= end; ++date)
        testDate(date);

    std::cout << "tested " << (end-start+1) << " dates" << std::endl;
}

static void testBadDates(void)
{
    // we are testing for dates we know to be bad
    static int badDates[] = {
        1900,2,29,
        2100,2,29,
        2000,1,32,
        2000,2,30,
        2000,3,32,
        2000,4,31,
        2000,5,32,
        2000,6,31,
        2000,7,32,
        2000,8,32,
        2000,9,31,
        2000,10,32,
        2000,11,31,
        2000,12,32,
        0,0,0,
        2013,0,0,
        2013,6,0,
        2013,13,7};

    int numBadDates = sizeof(badDates) / sizeof(int) / 3;

    for (int i = 0; i < numBadDates; ++i)
    {
        int year  = badDates[3*i];
        int month = badDates[3*i+1];
        int day   = badDates[3*i+2];

        bool failed = false;
        try
        {
            spi::Date date (year, month, day);
        }
        catch (std::exception &)
        {
            failed = true;
        }
        SPI_POST_CONDITION(failed);
    }

    std::cout << "tested " << numBadDates << " bad dates" << std::endl;
}



int main(int argc, char* argv[])
{
    return spi_util::TestRunner(argc, argv,
                      "DateConversions", testDates,
                      "BadDates", testBadDates,
                      0);
}
