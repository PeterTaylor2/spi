#include <spi_util/FileUtil.hpp>
#include <spi_util/RuntimeError.hpp>

#include <stdio.h>
#include <iostream>

#include <spi_util/TestRunner.hpp>

static char* g_exe = NULL;

static void testDirname(void)
{
    std::string dn;

    dn = spi_util::path::dirname("hello\\world");
    SPI_UTIL_POST_CONDITION(dn == "hello");

    dn = spi_util::path::dirname("world");
    SPI_UTIL_POST_CONDITION(dn == "");

    dn = spi_util::path::dirname("/world");
    SPI_UTIL_POST_CONDITION(dn == "/");

    dn = spi_util::path::dirname("hello/brave/new/world");
    SPI_UTIL_POST_CONDITION(dn == "hello/brave/new");

}

static void testBasename(void)
{
    std::string bn;

    bn = spi_util::path::basename("hello\\world");
    SPI_UTIL_POST_CONDITION(bn == "world");

    bn = spi_util::path::basename("world");
    SPI_UTIL_POST_CONDITION(bn == "world");

    bn = spi_util::path::basename("/world");
    SPI_UTIL_POST_CONDITION(bn == "world");

    bn = spi_util::path::basename("hello/brave/new/world");
    SPI_UTIL_POST_CONDITION(bn == "world");

}

static void testJoin(void)
{
    std::string bn;

    std::string ffn = spi_util::path::join("hello", "brave", "new", "world", 0);
    SPI_UTIL_POST_CONDITION(ffn == "hello/brave/new/world");

    ffn = spi_util::path::join("", "hello", 0);
    SPI_UTIL_POST_CONDITION(ffn == "hello");
}

static void testNormpath(void)
{
    std::string bn;

    std::string ffn = spi_util::path::normpath("hello/brave/new/world");
    SPI_UTIL_POST_CONDITION(ffn == "hello/brave/new/world");

    ffn = spi_util::path::normpath("./hello/brave/new/world");
    SPI_UTIL_POST_CONDITION(ffn == "hello/brave/new/world");

    ffn = spi_util::path::normpath("../hello/brave/new/./world");
    SPI_UTIL_POST_CONDITION(ffn == "../hello/brave/new/world");

    ffn = spi_util::path::normpath("../hello/brave/new/./world");
    SPI_UTIL_POST_CONDITION(ffn == "../hello/brave/new/world");

    ffn = spi_util::path::normpath("../hello/../brave/new/./world");
    SPI_UTIL_POST_CONDITION(ffn == "../brave/new/world");

    ffn = spi_util::path::normpath("../hello/../hello/brave/new/./world");
    SPI_UTIL_POST_CONDITION(ffn == "../hello/brave/new/world");

    ffn = spi_util::path::normpath("hello/../brave/../hello/brave/new/world");
    SPI_UTIL_POST_CONDITION(ffn == "hello/brave/new/world");

    ffn = spi_util::path::normpath("hello/brave/new/world/../../../world");
    SPI_UTIL_POST_CONDITION(ffn == "hello/world");

}

static void testCwd(void)
{
    std::string cwd = spi_util::path::getcwd();
    std::string exe = spi_util::path::relativePath(g_exe, "");

    std::cout << "cwd: " << cwd << std::endl;
    std::cout << "exe: " << g_exe << std::endl;
    std::cout << "exe: " << exe << std::endl;
}


int main(int argc, char* argv[])
{
    g_exe = argv[0];
    int errors = spi_util::TestRunner(argc, argv,
                                      "dirname", testDirname,
                                      "basename", testBasename,
                                      "join", testJoin,
                                      "normpath", testNormpath,
                                      "cwd", testCwd,
                                      (char*)0);
    return errors;
}

