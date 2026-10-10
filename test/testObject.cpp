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

static void testSimpleObjectStream(void)
{
    spi::ServiceSP svc = test::test_exported_service();

    std::vector<int> intVector;
    intVector.push_back(1);
    intVector.push_back(2);
    test::DataConstSP item = test::Data::Make(
        true, 42, 3.1415926, spi::Date(2012,9,20), "Hello world",
        intVector);

    std::ostringstream oss;

    std::cout << "Writing object of type Data to stream" << std::endl;
    item->to_stream(oss);

    std::string str = oss.str();
    std::cout << str << std::endl;

    std::cout << "Reading object of type Data from string" << std::endl;
    test::DataConstSP d2 = test::Data::from_string(str);
    SPI_POST_CONDITION(d2);
    SPI_POST_CONDITION(d2->aBool() == item->aBool());
    SPI_POST_CONDITION(d2->anInt() == item->anInt());
    SPI_POST_CONDITION(d2->aDouble() == item->aDouble());
    SPI_POST_CONDITION(d2->aDate() == item->aDate());
    SPI_POST_CONDITION(d2->intVec() == intVector);
    SPI_POST_CONDITION(d2->anInstance());
    SPI_POST_CONDITION(d2->anInstance()->aString() == "Hello world");
    SPI_POST_CONDITION(!d2->anotherInstance());
    std::ostringstream oss2;
    d2->to_stream(oss2);
    std::cout << oss2.str() << std::endl;
}

int main(int argc, char* argv[])
{
    return spi_util::TestRunner(argc, argv,
                      "SimpleObjectStream", testSimpleObjectStream,
                      0);
}
