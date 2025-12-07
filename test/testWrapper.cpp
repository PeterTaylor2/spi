#include <spi/spi.hpp>
#include <spi_boost/shared_ptr.hpp>
#include <dll/test_wrapper_classes.hpp>
#include <dll/test_dll_service.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>

#include <spi_util/TestRunner.hpp>

static void testWrapperObjectStream(void)
{
    spi::ServiceSP svc = test::test_exported_service();

    std::vector<int> intVector;
    intVector.push_back(1);
    intVector.push_back(2);
    test::OuterConstSP item = test::Outer::Make(
        true, 42, 3.1415926, spi::Date(2012,9,20), "Hello world",
        intVector);

    std::ostringstream oss;

    for (const char* format : { "TEXT", "JSON", "BIN"})
    {
        std::string fn = "wrapper." + spi_util::StringLower(format);

        std::cout << "\nUsing format '" << format << "'\n" << std::endl;

        std::cout << "Writing object of type Outer to " << fn << std::endl;
        item->to_file(fn.c_str(), format);

        std::cout << "Reading object of type Outer from " << fn << std::endl;
        auto d2 = test::Outer::from_file(fn);
        auto mdo = spi::MapObject::Make(d2->get_meta_data());

        std::cout << d2->to_string() << "\n" << std::endl;
        std::cout << mdo->to_string() << "\n" << std::endl;
    }
}

int main(int argc, char* argv[])
{
    return spi_util::TestRunner(argc, argv,
                      "WrapperObjectStream", testWrapperObjectStream,
                      0);
}
