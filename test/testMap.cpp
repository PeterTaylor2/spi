#include <spi/Map.hpp>
#include <spi/Date.hpp>
#include <spi/Service.hpp>
#include <spi/MapObject.hpp>
#include <spi/ObjectPut.hpp>
#include <spi/IObjectMap.hpp>
#include <spi/RuntimeError.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>

#include <spi_util/TestRunner.hpp>
#include <spi_util/ClockUtil.hpp>

static void testMapStreaming(void)
{
    spi::Object::g_add_object_id_file = true;

    spi::ServiceSP commonService = spi::Service::CommonService();

    std::vector<int> ints = { 27,53,-129,238 };
    std::vector<std::string> names = { "Matthew", "Mark", "Luke", "John" };

    spi::ObjectConstSP m1 = spi::ObjectPut({}, "aString", "Hello world");

    spi::ObjectConstSP m = spi::ObjectPut({},
        { "ints", "names", "map1", "map2" },
        { ints, names, m1, m1 });

    std::ostringstream oss;

    std::cout << "Writing map object to stream" << std::endl;
    m->to_stream(oss);

    std::string str = oss.str();
    std::cout << str << "\n" << std::endl;

    spi_util::Sleep(1000); // sleep for 1000ms
    std::cout << "Writing map object to map.text" << std::endl;
    m->to_file("map.text");

    spi_util::Sleep(1000); // sleep for 1000ms
    std::cout << "Writing map object to map.bin" << std::endl;
    m->to_file("map.bin", "BIN");

    // read the files back
    for (const char* fn : { "map.text", "map.bin" })
    {
        spi::MapObjectConstSP m2 = spi::MapObject::from_file(fn);
        SPI_POST_CONDITION(m2);
        SPI_POST_CONDITION(m2->get_value("ints").getIntVector() == ints);
        SPI_POST_CONDITION(m2->get_value("names").getStringVector() == names);
        spi::ObjectConstSP map1 = m2->get_value("map1").getObject();
        spi::ObjectConstSP map2 = m2->get_value("map2").getObject();
        SPI_POST_CONDITION(map1);
        SPI_POST_CONDITION(map2);
        SPI_POST_CONDITION(map1->get_value("aString").getString() == "Hello world");
        SPI_POST_CONDITION(map2->get_value("aString").getString() == "Hello world");
        SPI_POST_CONDITION(map1 == map2);
        spi::MapObjectConstSP m3 = spi::MapObject::Make(m2->get_meta_data());
        std::cout << "\nmeta_data" << std::endl;
        std::cout << m3->to_string() << std::endl;
        std::cout << "\nmap object:" << std::endl;
        std::cout << m2->to_string() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    return spi_util::TestRunner(argc, argv,
        "MapStreaming", testMapStreaming,
        0);
}
