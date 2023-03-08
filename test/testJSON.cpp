#include <spi_util/JSON.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdio.h>

#include <spi_util/TestRunner.hpp>
#include <spi_util/RuntimeError.hpp>


using namespace spi_util;

static void testSimpleJSON(void)
{
    JSONMapSP aMap(new JSONMap);

    aMap->Insert("aDouble", 3.141592654);
    aMap->Insert("anInt", (double)42);
    aMap->Insert("aBool", true);
    aMap->Insert("aNull", JSONValue());

    JSONValue value (aMap);

    // convert to string
    std::string str = JSONValueToString(value);

    // parse from string
    JSONValue copy = JSONValueFromString(str);

    // convert parsed copy to string
    std::string str2 = JSONValueToString(copy);

    // they should be the same
    SPI_UTIL_POST_CONDITION(str == str2);

    std::cout << str << std::endl;
}

int main(int argc, char* argv[])
{
    return spi_util::TestRunner(
        argc, argv,
        "SimpleJSON", testSimpleJSON,
        0);
}
