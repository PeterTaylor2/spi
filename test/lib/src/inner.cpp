#include "../inner.hpp"

Inner::Inner(bool aBool,
    int anInt,
    double aDouble,
    IDate aDate,
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

