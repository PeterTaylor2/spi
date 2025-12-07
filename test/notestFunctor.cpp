#include <spi/Map.hpp>
#include <spi/Date.hpp>
#include <spi/Service.hpp>
#include <spi/Function.hpp>
#include <spi/IObjectMap.hpp>
#include <spi/spi.hpp>
#include <spi/ObjectHelper.hpp>

#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>

#include <spi_util/TestRunner.hpp>

spi::ServiceSP theService;

spi::Service* get_the_service()
{
    return theService.get();
}

SPI_DECLARE_RC_CLASS(AddFunc);

class AddFunc : public spi::Function
{
public:
    SPI_DECLARE_OBJECT_TYPE(AddFunc);

    spi::Value call() const
    {
        return x+y;
    }

    AddFunc(
        const spi::Service* service,
        double x,
        double y)
        :
        spi::Function(service, "Add", {x,y}),
        x(x),
        y(y)
    {}

private:

    double x;
    double y;
};

void AddFunc::to_map(spi::IObjectMap* m, bool publicOnly) const
{
    m->SetDouble("x", x);
    m->SetDouble("y", y);
}

spi::ObjectSP AddFunc::object_from_map(spi::IObjectMap* m)
{
    double x = m->GetDouble("x");
    double y = m->GetDouble("y");

    return new AddFunc(x,y);
}

SPI_IMPLEMENT_OBJECT_TYPE(AddFunc, "Add", get_the_service,false,false);

bool isLogging = false;

static spi::ServiceSP MakeService()
{
    spi::ServiceSP svc = spi::Service::Make(
        "testFunctor", "testFunctor", "1.0.0.0");
    svc->add_object_type(&AddFunc::object_type);

    return svc;
}

double Add(double x, double y)
{
    AddFunc func(x,y);
    return func.call_func();
}

static void testFunctor(void)
{
    try
    {
        spi::ServiceSP svc = MakeService();
        theService = svc;

        double out = Add(2,2);
        SPI_PRE_CONDITION(out == 4.0);

        AddFunc func(2,5);

        std::ostringstream oss;

        std::cout << "Writing object of type AddFunc to stream" << std::endl;
        func.to_stream(oss);

        std::string str = oss.str();
        std::cout << str << std::endl;

        std::istringstream iss(str);

        std::cout << "Reading object of type AddFunc from stream" << std::endl;
        AddFuncSP f2 = AddFunc::FromStream(iss);

        std::ostringstream oss2;
        f2->to_stream(oss2);
        std::cout << oss2.str() << std::endl;

        out = f2->call_func();

        SPI_POST_CONDITION(out == 7.0);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        throw;
    }
}

int main(int argc, char* argv[])
{
    return spi_util::TestRunner(argc, argv,
                      "Functor", testFunctor,
                      0);
}
