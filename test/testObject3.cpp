#include <spi/Map.hpp>
#include <spi/Date.hpp>
#include <spi/Service.hpp>
#include <spi/IObjectMap.hpp>

#include <dll/test_data_classes.hpp>
#include <dll/test_dll_service.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>

#include <spi_util/TestRunner.hpp>

static void testCompoundObjectStream(void)
{
    spi::ServiceSP svc = test::test_exported_service();

    std::vector<int> intVector;
    intVector.push_back(1);
    intVector.push_back(2);
    test::SubDataConstSP subItem = test::SubData::Make("Hello world");
    test::DataConstSP item = test::Data::Make(
        true, 42, 3.1415926, spi::Date(2012, 9, 20),
        subItem, intVector, subItem);

    std::ostringstream oss;

    std::cout << "Writing object of type Data to stream" << std::endl;
    item->to_stream(oss);

    std::string str = oss.str();
    std::cout << str << std::endl;

    std::cout << "Reading object of type Data from string" << std::endl;
    test::DataConstSP d2 = test::Data::from_string(str);
    std::ostringstream oss2;
    d2->to_stream(oss2);
    std::cout << oss2.str() << std::endl;

    std::cout << "Writing object of type Data to data.bin" << std::endl;
    item->to_file("data.bin", "BIN");

    // can we read from binary string?
    test::DataConstSP d3 = test::Data::from_file("data.bin");

    std::cout << "Does binary serialization preserve the property that anInstance and anotherInstance are the same?" << std::endl;
    std::cout << d3->to_string() << std::endl;

    test::DataConstSP item2 = test::Data::Make(
        true, 42, 3.1415926, spi::Date(2012, 9, 20),
        "Hello world", intVector, "Goodbye");
    item2->to_file("data2.bin", "BIN"); // examine the file for field name definitions
    test::DataConstSP d4 = test::Data::from_file("data2.bin");
    std::cout << "More binary serialization testing" << std::endl;
    std::cout << d4->to_string() << std::endl;
}

int main(int argc, char* argv[])
{
    return spi_util::TestRunner(argc, argv,
                      "CompoundObjectStream", testCompoundObjectStream,
                      0);
}
