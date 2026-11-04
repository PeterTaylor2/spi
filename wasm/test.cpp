#include <spi/MapObject.hpp>
#include <spi/ObjectPut.hpp>
#include <spi/Service.hpp>
#include <spi/spdoc_constant.hpp>
#include <spi_util/UUID.hpp>
#include <emscripten/bind.h>
#include <sstream>
#include <cstdio>

namespace {
std::string runTests()
{
    try
    {
        const auto service = spi::Service::CommonService();
        const std::vector<int> ints = { -2147483647 - 1, 0, 2147483647 };
        const std::vector<std::string> strings = { "", "hello", "π 世界" };
        const auto child = spi::ObjectPut({}, "text", "nested");
        const auto object = spi::ObjectPut({},
            { "ints", "strings", "date", "left", "right", "null" },
            { ints, strings, spi::Date(2024, 2, 29), child, child, spi::ObjectConstSP() });

        for (const char* format : { "TEXT", "BIN", "Z" })
        {
            std::ostringstream stream;
            const bool binary = object->to_stream(stream, format);
            const auto copy = service->object_from_data(stream.str(), "browser test", binary);
            SPI_POST_CONDITION(copy);
            SPI_POST_CONDITION(copy->get_value("ints").getIntVector() == ints);
            SPI_POST_CONDITION(copy->get_value("strings").getStringVector() == strings);
            SPI_POST_CONDITION(copy->get_value("date").getDate() == spi::Date(2024, 2, 29));
            SPI_POST_CONDITION(!copy->get_value("null").getObject());
            const auto left = copy->get_value("left").getObject();
            const auto right = copy->get_value("right").getObject();
            SPI_POST_CONDITION(left->get_value("text").getString() == "nested");
            SPI_POST_CONDITION(left == right);
        }

        object->to_file("/roundtrip.bin", "BIN");
        const auto fromFile = service->object_from_file("/roundtrip.bin");
        SPI_POST_CONDITION(fromFile->get_value("ints").getIntVector() == ints);
        std::remove("/roundtrip.bin");

        bool caught = false;
        try { service->object_from_string("invalid SPI data"); }
        catch (const std::exception&) { caught = true; }
        SPI_POST_CONDITION(caught);

        // Exercise generated C++ classes as well as untyped maps.
        const auto constant = spdoc::IntConstant::Make(42);
        const auto restored = spdoc::IntConstant::from_string(constant->to_string());
        SPI_POST_CONDITION(restored->getInt() == 42);

        // Encoded dimensions and byte counts must not wrap on wasm32.
        // The last field in this one-field object is an INT_ARRAY (tag 2).
        const auto arrayObject = spi::ObjectPut({}, "values", std::vector<int>{ 7 });
        std::ostringstream arrayStream;
        arrayObject->to_stream(arrayStream, "BIN");
        const std::string valid = arrayStream.str();
        SPI_POST_CONDITION(valid.size() >= 13 && valid[valid.size() - 13] == 2);
        const std::string prefix = valid.substr(0, valid.size() - 12);
        for (const std::vector<uint32_t>& dimensions : {
            std::vector<uint32_t>{ 0x40000000 },
            std::vector<uint32_t>{ 65536, 65536 } })
        {
            std::string bad = prefix;
            const uint32_t count = dimensions.size();
            bad.append(reinterpret_cast<const char*>(&count), sizeof(count));
            bad.append(reinterpret_cast<const char*>(dimensions.data()), count * sizeof(uint32_t));
            caught = false;
            try { service->object_from_data(bad, "overflow", true); }
            catch (const std::exception&) { caught = true; }
            SPI_POST_CONDITION(caught);
        }

        const std::string uuid = spi_util::uuid_string();
        SPI_POST_CONDITION(uuid.size() == 36 && uuid[14] == '4');
        SPI_POST_CONDITION(uuid != spi_util::uuid_string());
        return "PASS";
    }
    catch (const std::exception& error)
    {
        return error.what();
    }
}
}

EMSCRIPTEN_BINDINGS(spi_tests)
{
    emscripten::function("runTests", &runTests);
}
