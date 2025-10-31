#include <spi_util/FileUtil.hpp>
#include <spi_util/RuntimeError.hpp>
#include "commonTools.hpp"

#include <stdio.h>
#include <iostream>

#include <spi_util/TestRunner.hpp>

static char* g_exe = NULL;

static void helloWorld(void)
{
    std::cout << "hello world" << std::endl;

    std::map<std::string, std::string> values;

    values["name"] = "Peter";

    std::ifstream istr("test.in");
    if (!istr)
        throw std::runtime_error("Could not open test.in");

    generateFromTemplate(istr, "test.in", values, "test.out", ".");
}


int main(int argc, char* argv[])
{
    g_exe = argv[0];
    int errors = spi_util::TestRunner(argc, argv,
                                      "helloWorld", helloWorld,
                                      (char*)0);
    return errors;
}

