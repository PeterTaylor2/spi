#include <spi/Map.hpp>
#include <spi/Date.hpp>
#include <spi/Service.hpp>
#include <spi/IObjectMap.hpp>
#include <spi_boost/shared_ptr.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>

#include <spi_util/TestRunner.hpp>

bool isLogging = false;

SPI_DECLARE_RC_CLASS(Outer);

class Inner;

class Outer : public spi::Object
{
public:
    typedef spi_boost::shared_ptr<Inner> inner_type;
    static Object::Type object_type;

    static OuterSP Make(bool aBool, int anInt, double aDouble,
                        spi::Date aDate, const std::string& aString,
                        const std::vector<int>& intVec)
    {
        return OuterSP(
            new Outer(aBool, anInt, aDouble, aDate, aString, intVec));
    }

    void to_map(spi::IObjectMap* m, bool publicOnly) const;
    static spi::ObjectSP object_from_map(spi::IObjectMap* m);
    Object::Type* get_object_type() const
    {
        return &Outer::object_type;
    }

    static OuterSP Coerce(const spi::ObjectSP& o)
    {
        return spi::CoerceFromObject<Outer>(o);
    }

    static OuterConstSP Coerce(const spi::ObjectConstSP& o)
    {
        return spi::CoerceFromObject<Outer>(o);
    }

    inner_type get_inner()
    {
        return m_inner;
    }

protected:
    void set_inner(const inner_type& inner)
    {
        m_inner = inner;
    }

private:
    Outer(bool aBool,
          int anInt,
          double aDouble,
          spi::Date aDate,
          const std::string& aString,
          const std::vector<int>& intVec);

    inner_type m_inner;
};

class Inner
{
public:
    Inner(bool aBool,
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

    bool aBool;
    int anInt;
    double aDouble;
    spi::Date aDate;
    std::string aString;
    std::vector<int> intVec;
};

Outer::Outer(bool aBool,
             int anInt,
             double aDouble,
             spi::Date aDate,
             const std::string& aString,
             const std::vector<int>& intVec)
{
    Inner* inner = new Inner(aBool, anInt, aDouble, aDate, aString, intVec);
    set_inner(spi_boost::shared_ptr<Inner>(inner));
}

void Outer::to_map(spi::IObjectMap* m, bool publicOnly) const
{
    Inner* self = m_inner.get();

    m->SetBool("aBool", self->aBool);
    m->SetInt("anInt", self->anInt);
    m->SetDouble("aDouble", self->aDouble);
    m->SetDate("aDate", self->aDate);
    m->SetString("aString", self->aString);
    m->SetIntVector("intVec", self->intVec);
}

spi::ObjectSP Outer::object_from_map(spi::IObjectMap* m)
{
    bool aBool
        = m->GetBool("aBool");
    int anInt
        = m->GetInt("anInt");
    double aDouble
        = m->GetDouble("aDouble");
    spi::Date aDate
        = m->GetDate("aDate");
    std::string aString
        = m->GetString("aString");
    const std::vector<int>& intVec
        = m->GetIntVector("intVec");

    return Outer::Make(aBool, anInt, aDouble, aDate, aString, intVec);
}

spi::Object::Type Outer::object_type(
    "Outer",
    Outer::object_from_map,
    spi::IsInstance<Outer>);

static spi::ServiceSP MakeService()
{
    spi::ServiceSP svc = spi::Service::Make(
        "testObject", "testObject", &isLogging);
    svc->add_object_type(&Outer::object_type);

    return svc;
}

static void testWrapperObjectStream(void)
{
    spi::ServiceSP svc = MakeService();

    std::vector<int> intVector;
    intVector.push_back(1);
    intVector.push_back(2);
    OuterConstSP item = Outer::Make(
        true, 42, 3.1415926, spi::Date(2012,9,20), "Hello world",
        intVector);

    std::ostringstream oss;

    std::cout << "Writing object of type Outer to stream" << std::endl;
    item->to_stream(oss);

    std::string str = oss.str();
    std::cout << str << std::endl;

    std::istringstream iss(str);

    std::cout << "Reading object of type Outer from stream" << std::endl;
    spi::ObjectSP o = svc->object_from_stream(iss);
    OuterSP d2 = Outer::Coerce(o);
    std::ostringstream oss2;
    d2->to_stream(oss2);
    std::cout << oss2.str() << std::endl;
}

int main(int argc, char* argv[])
{
    return spi_util::TestRunner(argc, argv,
                      "WrapperObjectStream", testWrapperObjectStream,
                      0);
}
