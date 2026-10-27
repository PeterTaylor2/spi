#include <spi/MapObject.hpp>
#include <spi/ObjectPut.hpp>
#include <spi/Service.hpp>
#include <spi/Version.hpp>
#include <emscripten/bind.h>

namespace {

// Own an SPI reference, so JS handles never borrow a temporary C++ object.
class Object
{
public:
    Object()
    {
        spi::Service::CommonService();
        object = spi::ObjectPut({}, std::vector<std::string>(), std::vector<spi::Value>());
    }

    explicit Object(const spi::ObjectConstSP& value) : object(value)
    {
        SPI_POST_CONDITION(object);
    }

    static Object fromString(const std::string& text)
    {
        return Object(spi::Service::CommonService()->object_from_string(text));
    }

    std::string toString(const std::string& format) const
    {
        return object->to_string(format.c_str());
    }

    std::string typeName() const { return object->get_class_name(); }

    template<class T> T get(const std::string& name) const;

    template<class T> void set(const std::string& name, T value)
    {
        object = spi::ObjectPut(object, name.c_str(), spi::Value(value));
    }

    Object getObject(const std::string& name) const
    {
        return Object(object->get_value(name).getObject());
    }

    void setObject(const std::string& name, const Object& value)
    {
        object = spi::ObjectPut(object, name.c_str(), spi::Value(value.object));
    }

private:
    spi::ObjectConstSP object;
};

template<> std::string Object::get(const std::string& name) const
{ return object->get_value(name).getString(); }
template<> int Object::get(const std::string& name) const
{ return object->get_value(name).getInt(); }
template<> double Object::get(const std::string& name) const
{ return object->get_value(name).getDouble(); }
template<> bool Object::get(const std::string& name) const
{ return object->get_value(name).getBool(); }

}

EMSCRIPTEN_BINDINGS(spi)
{
    emscripten::function("version", +[]() { return std::string(spi::VersionString()); });
    emscripten::class_<Object>("Object")
        .constructor<>()
        .class_function("fromString", &Object::fromString)
        .function("toString", &Object::toString)
        .function("typeName", &Object::typeName)
        .function("getString", &Object::get<std::string>)
        .function("getInt", &Object::get<int>)
        .function("getDouble", &Object::get<double>)
        .function("getBool", &Object::get<bool>)
        .function("setString", &Object::set<std::string>)
        .function("setInt", &Object::set<int>)
        .function("setDouble", &Object::set<double>)
        .function("setBool", &Object::set<bool>)
        .function("getObject", &Object::getObject)
        .function("setObject", &Object::setObject);
}
