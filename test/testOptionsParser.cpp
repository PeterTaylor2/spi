#include <spi/OptionsParser.hpp>
#include <spi/RuntimeError.hpp>

#include <stdio.h>
#include <iostream>

#include <spi_util/TestRunner.hpp>

static void testOptionsParser(void)
{
    spi::OptionsParser parser("testOptionsParser");

    parser.SetChar("aChar", 'C');
    parser.SetInt("anInt", 42);
    parser.SetDouble("aDouble", 3.14159);
    parser.SetDate("aDate", spi::Date(2012,10,15));
    parser.SetString("aString", "hello world");
    parser.SetBool("aBool", false);

    parser.Parse("aChar='E' aDate=2012-12-25");

    SPI_POST_CONDITION(parser.GetChar("aChar") == 'E');
    SPI_POST_CONDITION(parser.GetInt("anInt") == 42);
    SPI_POST_CONDITION(parser.GetDouble("aDouble") == 3.14159);
    SPI_POST_CONDITION(parser.GetDate("aDate") == spi::Date(2012,12,25));
    SPI_POST_CONDITION(parser.GetString("aString") == "hello world");
    SPI_POST_CONDITION(parser.GetBool("aBool") == false);

    // test that we remember the original defaults if we re-use the parser

    parser.Parse("aChar='F' anInt=420");

    SPI_POST_CONDITION(parser.GetChar("aChar") == 'F');
    SPI_POST_CONDITION(parser.GetInt("anInt") == 420);
    SPI_POST_CONDITION(parser.GetDouble("aDouble") == 3.14159);
    SPI_POST_CONDITION(parser.GetDate("aDate") == spi::Date(2012,10,15));
    SPI_POST_CONDITION(parser.GetString("aString") == "hello world");
    SPI_POST_CONDITION(parser.GetBool("aBool") == false);


}

int main(int argc, char* argv[])
{
    int errors = spi_util::TestRunner(argc, argv,
                            "OptionsParser", testOptionsParser,
                            0);
    return errors;
}

