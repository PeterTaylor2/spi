#include <spi/Date.hpp>
#include <spi/RuntimeError.hpp>
#include <spi/MatrixData.hpp>

#include <iostream>

#include <spi_util/TestRunner.hpp>

static void testMatrix()
{
    spi::MatrixData<double> m(3,2);

    m[0][0] = 1.0;
    m[0][1] = 1.5;
    m[1][0] = 0.6;
    m[1][1] = 0.9;

    for (size_t i = 0; i < 3; ++i)
    {
        std::cout << "[" << i << "] = " << m[i][0] << "," << m[i][1]
                  << std::endl;
    }

    m[2][0] = 0.3;
    m[2][1] = 1.0;
}

int main(int argc, char* argv[])
{
    return spi_util::TestRunner(argc, argv,
                                "Matrix", testMatrix,
                                0);
}
