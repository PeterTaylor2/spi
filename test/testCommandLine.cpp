#include <spi_util/CommandLine.hpp>

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <stdio.h>

#include <spi_util/TestRunner.hpp>
#include <spi_util/RuntimeError.hpp>

static void testShortOptions(void)
{
    char* argv[] = {"hello", "-abc", "-d", "yes", "world"};

    spi_util::CommandLine commandLine(5, argv, "abcd=");

    std::cout << commandLine.toString() << std::endl;

    SPI_UTIL_POST_CONDITION(commandLine.exeName == "hello");
    SPI_UTIL_POST_CONDITION(commandLine.optionCodes.size() == 4);
    SPI_UTIL_POST_CONDITION(commandLine.args.size() == 1);

}



static void testLongOptions(void)
{
    char* argv[] = {"hello", "-abc", "--debug=yes", "world"};

    spi_util::CommandLine commandLine(4, argv, "abc", "debug=");

    std::cout << commandLine.toString() << std::endl;

    SPI_UTIL_POST_CONDITION(commandLine.exeName == "hello");
    SPI_UTIL_POST_CONDITION(commandLine.optionCodes.size() == 4);
    SPI_UTIL_POST_CONDITION(commandLine.args.size() == 1);
}


static void testBadOptions(void)
{
    char* argv[] = {"hello", "-abc", "--debug=yes", "world"};

    try
    {
        spi_util::CommandLine commandLine(4, argv, "abd", "debug=");
    }
    catch(std::exception &e)
    {
        std::cout << "expecting error: " << e.what() << std::endl;
    }

    try
    {
        spi_util::CommandLine commandLine(4, argv, "abc", "debug");
    }
    catch(std::exception &e)
    {
        std::cout << "expecting error: " << e.what() << std::endl;
    }

    try
    {
        spi_util::CommandLine commandLine(4, argv, "ab=c", "debug=");
    }
    catch(std::exception &e)
    {
        std::cout << "expecting error: " << e.what() << std::endl;
    }
}


int main(int argc, char* argv[])
{
    return spi_util::TestRunner(argc, argv,
                                "ShortOptions", testShortOptions,
                                "LongOptions", testLongOptions,
                                "BadOptions", testBadOptions,
                                0);
}
