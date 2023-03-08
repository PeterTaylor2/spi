#include <spi/Map.hpp>
#include <spi/Date.hpp>
#include <spi/Service.hpp>
#include <spi/IObjectMap.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>

#include <spi_util/TestRunner.hpp>

bool isLogging = false;

SPI_DECLARE_RC_CLASS(Data);

class Data : public spi::Object
{
public:
    static DataConstSP Make(bool aBool, int anInt, double aDouble,
                            spi::Date aDate, const std::string& aString,
                            const std::vector<int>& intVec)
    {
        return DataConstSP(
            new Data(aBool, anInt, aDouble, aDate, aString, intVec));
    }

    static DataSP Coerce(const spi::ObjectSP& o)
    {
        return spi::CoerceFromObject<Data>(o);
    }

    static DataConstSP Coerce(const spi::ObjectConstSP& o)
    {
        return spi::CoerceFromObject<Data>(o);
    }

private:
    Data(bool aBool,
         int anInt,
         double aDouble,
         spi::Date aDate,
         const std::string& aString,
         const std::vector<int>& intVec)
        :
        aBool(aBool),
        anInt(anInt),
        aDouble(aDouble),
        aDate(aDate),
        aString(aString),
        intVec(intVec)
    {}

public:
    spi::Object::Type* get_object_type() const
    {
        return &object_type;
    }

    void to_map(spi::IObjectMap* m, bool publicOnly) const
    {
        // m->SetClassName("Data");
        m->SetBool("aBool", aBool);
        m->SetInt("anInt", anInt);
        m->SetDouble("aDouble", aDouble);
        m->SetDate("aDate", aDate);
        m->SetString("aString", aString);
        m->SetIntVector("intVec", intVec);
    }

    static spi::ObjectSP object_from_map(spi::IObjectMap* m)
    {
        return new Data(m->GetBool("aBool"),
                        m->GetInt("anInt"),
                        m->GetDouble("aDouble"),
                        m->GetDate("aDate"),
                        m->GetString("aString"),
                        m->GetIntVector("intVec"));
    }

    static spi::Object::Type object_type;

private:
    bool aBool;
    int anInt;
    double aDouble;
    spi::Date aDate;
    std::string aString;
    std::vector<int> intVec;
};

spi::Object::Type Data::object_type(
    "Data",
    Data::object_from_map,
    spi::IsInstance<Data>);


static spi::ServiceSP MakeService()
{
    spi::ServiceSP svc = spi::Service::Make(
        "testObject","testObject", &isLogging);
    svc->add_object_type(&Data::object_type);

    return svc;
}

static void testSimpleObjectStream(void)
{
    spi::ServiceSP svc = MakeService();

    std::vector<int> intVector;
    intVector.push_back(1);
    intVector.push_back(2);
    DataConstSP item = Data::Make(
        true, 42, 3.1415926, spi::Date(2012,9,20), "Hello world",
        intVector);

    std::ostringstream oss;

    std::cout << "Writing object of type Data to stream" << std::endl;
    item->to_stream(oss);

    std::string str = oss.str();
    std::cout << str << std::endl;

    std::istringstream iss(str);

    std::cout << "Reading object of type Data from stream" << std::endl;
    spi::ObjectSP o = svc->object_from_stream(iss);
    DataSP d2 = Data::Coerce(o);
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
