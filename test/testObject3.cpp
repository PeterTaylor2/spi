#include <spi/Map.hpp>
#include <spi/Date.hpp>
#include <spi/Service.hpp>
#include <spi/IObjectMap.hpp>
#include <spi/RuntimeError.hpp>

#include <dll/test_data_classes.hpp>
#include <dll/test_dll_service.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>

#include <spi_util/TestRunner.hpp>

static void checkData(const test::DataConstSP& actual, const test::DataConstSP& expected)
{
    SPI_POST_CONDITION(actual);
    SPI_POST_CONDITION(actual->aBool() == expected->aBool());
    SPI_POST_CONDITION(actual->anInt() == expected->anInt());
    SPI_POST_CONDITION(actual->aDouble() == expected->aDouble());
    SPI_POST_CONDITION(actual->aDate() == expected->aDate());
    SPI_POST_CONDITION(actual->intVec() == expected->intVec());
    SPI_POST_CONDITION(actual->anInstance());
    SPI_POST_CONDITION(actual->anotherInstance());
    SPI_POST_CONDITION(actual->anInstance()->aString() == expected->anInstance()->aString());
    SPI_POST_CONDITION(actual->anotherInstance()->aString() == expected->anotherInstance()->aString());
    SPI_POST_CONDITION((actual->anInstance() == actual->anotherInstance()) ==
                       (expected->anInstance() == expected->anotherInstance()));
}

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
    checkData(d2, item);
    std::ostringstream oss2;
    d2->to_stream(oss2);
    std::cout << oss2.str() << std::endl;

    std::cout << "Writing object of type Data to data.bin" << std::endl;
    item->to_file("data.bin", "BIN");

    // can we read from binary string?
    test::DataConstSP d3 = test::Data::from_file("data.bin");
    checkData(d3, item);

    std::cout << "Does binary serialization preserve the property that anInstance and anotherInstance are the same?" << std::endl;
    std::cout << d3->to_string() << std::endl;

    test::DataConstSP item2 = test::Data::Make(
        true, 42, 3.1415926, spi::Date(2012, 9, 20),
        "Hello world", intVector, "Goodbye");
    item2->to_file("data2.bin", "BIN"); // examine the file for field name definitions
    test::DataConstSP d4 = test::Data::from_file("data2.bin");
    checkData(d4, item2);
    std::cout << "More binary serialization testing" << std::endl;
    std::cout << d4->to_string() << std::endl;
}

int main(int argc, char* argv[])
{
    return spi_util::TestRunner(argc, argv,
                      "CompoundObjectStream", testCompoundObjectStream,
                      0);
}
