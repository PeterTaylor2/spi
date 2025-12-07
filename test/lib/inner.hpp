#ifndef TEST_INNER_INNER_HPP
#define TEST_INNER_INNER_HPP

#include <string>
#include <vector>
#include "date.hpp"

class Inner
{
public:
    Inner(bool aBool,
        int anInt,
        double aDouble,
        Date aDate,
        const std::string& aString,
        const std::vector<int>& intVec)
        :
        aBool(aBool),
        anInt(anInt),
        aDouble(aDouble),
        aDate(aDate),
        aString(aString),
        intVec(intVec)
    {
    }

    bool aBool;
    int anInt;
    double aDouble;
    Date aDate;
    std::string aString;
    std::vector<int> intVec;
};

#endif
