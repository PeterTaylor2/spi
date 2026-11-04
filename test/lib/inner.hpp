#ifndef TEST_INNER_INNER_HPP
#define TEST_INNER_INNER_HPP

#include <string>
#include <vector>
#include "iDate.hpp"

class Inner
{
public:
    Inner(bool aBool,
        int anInt,
        double aDouble,
        IDate aDate,
        const std::string& aString,
        const std::vector<int>& intVec);

    bool aBool;
    int anInt;
    double aDouble;
    IDate aDate;
    std::string aString;
    std::vector<int> intVec;
};

#endif
