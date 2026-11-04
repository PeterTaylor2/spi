#include <spi/spi.hpp>
#include <spi/ObjectHelper.hpp>

#include <dll/testlib_data_classes.hpp>
#include <dll/test_dll_service.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>

#include <spi_util/TestRunner.hpp>

static void testCompoundObjectStream(void)
{
    spi::ServiceSP svc = testlib::test_exported_service();

    std::vector<int> intVector;
    intVector.push_back(1);
    intVector.push_back(2);
    testlib::DataConstSP item = testlib::Data::Make(
        true, 42, 3.1415926, spi::Date(2012,9,20),
        testlib::SubData::Make("Hello world"),
        intVector);

    // demonstrates the operator string() method on SubData class
    // this only works because we declared as_string in the SubData class
    // what happens is that item->anInstance returns testlib::SubDataConstSP
    // testlib::SubDataConstSP tries to implement the operator std::string()
    // it can do it because testlib::SubData class defines the as_string() method
    std::string helloWorld = std::string(item->anInstance());
    SPI_POST_CONDITION(helloWorld == "Hello world");

    // but what about these comparison?
    // we haven't used the operator std::string to extract a string
    // so can we compare the instance object with std::string or literal?
    SPI_POST_CONDITION(item->anInstance() == "Hello world");
    SPI_POST_CONDITION(item->anInstance() == std::string("Hello world"));
    SPI_POST_CONDITION("Hello world" == item->anInstance());
    SPI_POST_CONDITION(std::string("Hello world") == item->anInstance());

    int null_comparisons = 0;
    // we don't particularly want item != NULL to be used but it has been in places
    // it will continue to work if the class does not support as_string()
    //
    // but it actually fails to compile if the class does support as_string()
    // although you can use != nullptr instead of != NULL as we show here
    if (item != NULL)
    {
       null_comparisons += 1;
    }

    if (item->anInstance() != nullptr)
    {
       null_comparisons += 2;
    }

    SPI_POST_CONDITION(null_comparisons==3);

    auto this_instance = item->anInstance();
    testlib::SubDataConstSP null_instance;

    // the following lines of code should not be ambiguous
    // changing the ObjectSmartPtr to be a non-explicit std::operator will break

    const std::string hello("Hello");
    auto instance1 = this_instance ? this_instance : hello;
    auto instance2 = null_instance ? null_instance : hello;

    SPI_POST_CONDITION(instance1 == "Hello world");
    SPI_POST_CONDITION(instance2 == hello);

#if false
    std::string itemText = item; // this won't compile because Data does not have as_string
#endif

    std::ostringstream oss;

    std::cout << "Writing object of type Data to stream" << std::endl;
    item->to_stream(oss);

    std::string str = oss.str();
    std::cout << str << std::endl;

    std::cout << "Reading object of type Data from stream" << std::endl;
    testlib::DataConstSP d2 = testlib::Data::from_string(str);
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
