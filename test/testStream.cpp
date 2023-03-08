#include <spi_util/StreamUtil.hpp>
#include <spi/RuntimeError.hpp>

#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

#include <spi_util/TestRunner.hpp>

static void testStreamReadLines(void)
{
    std::ifstream str("testStream.cpp");
    std::vector<std::string> lines = spi_util::StreamReadLines(str);

    for (size_t i = 0; i < lines.size(); ++i)
        printf("%d: %s\n", i+1, lines[i].c_str());

}

int main(int argc, char* argv[])
{
    int errors = spi_util::TestRunner(argc, argv,
                            "StreamReadLines", testStreamReadLines,
                            (char*)0);
    return errors;
}

