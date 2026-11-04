#include <spi/Map.hpp>
#include <spi/Date.hpp>
#include <spi/Service.hpp>
#include <spi/IObjectMap.hpp>

#include <dll/testlib_stringType.hpp>
#include <dll/test_dll_service.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>

#include <spi_util/TestRunner.hpp>

static void testStringType(void)
{
    spi::ServiceSP svc = testlib::test_exported_service();

    // StringType is a rather foolish type defined by the test service
    // it is defined by typedef string StringType<std::string&> and
    // the conversion routines on input enclose in [...] and on output
    // string the enclosing [...] (which must exist)

    std::string innerValue = testlib::StringTypeInnerValue("Hello");
    SPI_POST_CONDITION(innerValue == "[Hello]");

    std::string outerValue = testlib::StringTypeOuterValue(innerValue);
    SPI_POST_CONDITION(outerValue == "Hello");
}

int main(int argc, char* argv[])
{
    return spi_util::TestRunner(argc, argv,
                      "StringType", testStringType,
                      0);
}
