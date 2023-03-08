#include <spi/StringUtil.hpp>
#include <spi/RuntimeError.hpp>

#include <string.h>
#include <stdio.h>
#include <iostream>

#include <spi_util/TestRunner.hpp>

static void testStringParser(void)
{
    std::vector<std::string> output;

    output = spi::StringParser("2012-07-12", "--");
    SPI_POST_CONDITION(output.size() == 3);
    SPI_POST_CONDITION(output[0] == "2012");
    SPI_POST_CONDITION(output[1] == "07");
    SPI_POST_CONDITION(output[2] == "12");

    output = spi::StringParser("2012-07", "--", true);
    SPI_POST_CONDITION(output.size() == 3);
    SPI_POST_CONDITION(output[0] == "2012");
    SPI_POST_CONDITION(output[1] == "07");
    SPI_POST_CONDITION(output[2] == "");
}

static void testStringSplit(void)
{
    std::vector<std::string> output;

    output = spi::StringSplit("2012-07-12", '-');
    SPI_POST_CONDITION(output.size() == 3);
    SPI_POST_CONDITION(output[0] == "2012");
    SPI_POST_CONDITION(output[1] == "07");
    SPI_POST_CONDITION(output[2] == "12");

    output = spi::StringSplit("2013-09", '-');
    SPI_POST_CONDITION(output.size() == 2);
    SPI_POST_CONDITION(output[0] == "2013");
    SPI_POST_CONDITION(output[1] == "09");
}

static void testStringStrip(void)
{
    std::string output;

    output = spi::StringStrip("   3.1415926   ");
    SPI_POST_CONDITION(output == "3.1415926");

    output = spi::StringStrip("   3.1415926   ", true, false);
    SPI_POST_CONDITION(output == "3.1415926   ");

    output = spi::StringStrip("   3.1415926   ", false, true);
    SPI_POST_CONDITION(output == "   3.1415926");

    output = spi::StringStrip("  ");
    SPI_POST_CONDITION(output == "");

    output = spi::StringStrip("  ", true, false);
    SPI_POST_CONDITION(output == "");

    output = spi::StringStrip("  ", false, true);
    SPI_POST_CONDITION(output == "");

    output = spi::StringStrip("  ", false, false);
    SPI_POST_CONDITION(output == "  ");

    output = spi::StringStrip("3.1415926");
    SPI_POST_CONDITION(output == "3.1415926");
}



static void testStringUpper(void)
{
    std::string output;

    output = spi::StringUpper("abc123");
    SPI_POST_CONDITION(output == "ABC123");

    output = spi::StringUpper("Abc123");
    SPI_POST_CONDITION(output == "ABC123");

    output = spi::StringUpper("ABC123");
    SPI_POST_CONDITION(output == "ABC123");
}


static void testStringLower(void)
{
    std::string output;

    output = spi::StringLower("abc123");
    SPI_POST_CONDITION(output == "abc123");

    output = spi::StringLower("Abc123");
    SPI_POST_CONDITION(output == "abc123");

    output = spi::StringLower("ABC123");
    SPI_POST_CONDITION(output == "abc123");
}

static void testStringJoin(void)
{
    std::string output;
    std::string test;

    test = "a-b-c-d";
    output = spi::StringJoin("-", spi::StringSplit(test, '-'));
    SPI_POST_CONDITION(output == test);
}

static void testStringToNumber(void)
{
    SPI_POST_CONDITION(spi::StringToLong("102") == 102);
    SPI_POST_CONDITION(spi::StringToInt("-42") == -42);
    SPI_POST_CONDITION(spi::StringToDouble("1.03") == 1.03);
    SPI_POST_CONDITION(spi::StringToInt("09") == 9);
}

static void testStringEscape(void)
{
    const char* str;
    std::string escaped;
    std::string captured;

    str = "\n\t\\\"";
    SPI_POST_CONDITION(strlen(str) == 4);
    escaped = spi::StringEscape(str);
    SPI_POST_CONDITION(escaped.length() == 8);
    captured = spi::StringCapture(escaped.c_str());
    SPI_POST_CONDITION(captured == str);

    str = "Hello world!";
    escaped = spi::StringEscape(str);
    SPI_POST_CONDITION(escaped == str);
    captured = spi::StringCapture(escaped.c_str());
    SPI_POST_CONDITION(captured == str);

    str = "Hello world\n";
    escaped = spi::StringEscape(str);
    SPI_POST_CONDITION(escaped.length() == strlen(str)+1);
    captured = spi::StringCapture(escaped.c_str());
    SPI_POST_CONDITION(captured == str);

    str = "Hello \"name\"\n";
    escaped = spi::StringEscape(str);
    SPI_POST_CONDITION(escaped.length() == strlen(str)+3);
    captured = spi::StringCapture(escaped.c_str());
    SPI_POST_CONDITION(captured == str);
}

void testStringStartsWith()
{
    SPI_POST_CONDITION(spi::StringStartsWith("echobravo", "echo") == true);
    SPI_POST_CONDITION(spi::StringStartsWith("echo", "echobravo") == false);
    SPI_POST_CONDITION(spi::StringStartsWith("echo", "echo") == true);
}

void testStringEndsWith()
{
    SPI_POST_CONDITION(spi::StringEndsWith("echobravo", "bravo") == true);
    SPI_POST_CONDITION(spi::StringEndsWith("bravo", "echobravo") == false);
    SPI_POST_CONDITION(spi::StringEndsWith("echo", "echo") == true);
}

static void testStringMapFormat(void)
{
    char* format = "The quick brown %animal1% jumped over the lazy %animal2%";
    std::string expected = "The quick brown fox jumped over the lazy dog";

    std::map<std::string, std::string> values;

    values["animal1"] = "fox";
    values["animal2"] = "dog";

    std::string output = spi::StringMapFormat(format, values);

    SPI_POST_CONDITION(output == expected);
}


int main(int argc, char* argv[])
{
    int errors = spi_util::TestRunner(argc, argv,
                            "StringParser", testStringParser,
                            "StringSplit", testStringSplit,
                            "StringJoin", testStringJoin,
                            "StringStrip", testStringStrip,
                            "StringUpper", testStringUpper,
                            "StringLower", testStringLower,
                            "StringToNumber", testStringToNumber,
                            "StringEscape", testStringEscape,
                            "StringStartsWith", testStringStartsWith,
                            "StringEndsWith", testStringEndsWith,
                            "StringMapFormat", testStringMapFormat,
                            (char*)0);
    return errors;
}

