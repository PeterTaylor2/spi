#include <spi/spi.hpp>
#include <spi/ObjectHelper.hpp>

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
    test::DataConstSP item = test::Data::Make(
        true, 42, 3.1415926, spi::Date(2012,9,20),
        test::SubData::Make("Hello world"),
        intVector);

    // demonstrates the operator string() method on SubData class
    // this only works because we declared as_string in the SubData class
    // what happens is that item->anInstance returns test::SubDataConstSP
    // test::SubDataConstSP tries to implement the operator std::string()
    // it can do it because test::SubData class defines the as_string() method
    std::string helloWorld = item->anInstance();
    SPI_POST_CONDITION(helloWorld == "Hello world");

    // but what about these comparison?
    // we haven't used the operator std::string to extract a string
    // so can we compare the instance object with std::string or literal?
    SPI_POST_CONDITION(item->anInstance() == "Hello world");
    SPI_POST_CONDITION(item->anInstance() == std::string("Hello world"));
    SPI_POST_CONDITION("Hello world" == item->anInstance());
    SPI_POST_CONDITION(std::string("Hello world") == item->anInstance());

#if false
    std::string itemText = item; // this won't compile because Data does not have as_string
#endif

    std::ostringstream oss;

    std::cout << "Writing object of type Data to stream" << std::endl;
    item->to_stream(oss);

    std::string str = oss.str();
    std::cout << str << std::endl;

    std::cout << "Reading object of type Data from stream" << std::endl;
    test::DataConstSP d2 = test::Data::from_string(str);
    std::ostringstream oss2;

    d2->to_stream(oss2);
    std::cout << oss2.str() << std::endl;
}

int main(int argc, char* argv[])
{
    return spi_util::TestRunner(argc, argv,
                      "CompoundObjectStream", testCompoundObjectStream,
                      0);
}
