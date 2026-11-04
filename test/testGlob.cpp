#include <spi_util/FileUtil.hpp>
#include <spi_util/RuntimeError.hpp>
#include <spi_util/StringUtil.hpp>
#include <spi_util/Utils.hpp>

#include <stdio.h>
#include <iostream>

#include <spi_util/TestRunner.hpp>

static char* g_exe = NULL;

static void testMatcher()
{
    struct Case { const char* pattern; const char* name; bool expected; };
    static const Case cases[] = {
        {"*.txt",   "a.txt",   true },
        {"*.txt",   "a.cpp",   false},
        {"a?c",     "abc",     true },
        {"a?c",     "ac",      false},
        {"a*b*c",   "aXXbYYc", true },
        {"a*b*c",   "aXXbYY",  false},
        {"[a-c]x",  "bx",      true },
        {"[a-c]x",  "dx",      false},
        {"[!a]x",   "bx",      true },
        {"[!a]x",   "ax",      false},
        {"[",       "[",       true },   // unclosed bracket is literal
        {"*",       "",        true },
        {"",        "",        true },
        {"",        "a",       false},
    };

    std::cout << "Pattern    Name       Expected\n"
        << "========== ========== ========\n";
    //      1234567890 1234567890 12345678

    for (const Case& item : cases)
    {
        bool expected = item.expected;
        bool actual = spi_util::glob::Match(item.pattern, item.name);

        std::cout << spi_util::StringFormat("%-10s %-10s %-8s\n",
            spi_util::StringFormat("\"%s\"", item.pattern).c_str(),
            spi_util::StringFormat("\"%s\"", item.name).c_str(),
            item.expected ? "True" : "False");

        SPI_UTIL_POST_CONDITION(actual == expected);
    }
}

static void testGlob()
{
    std::string exeFullPath = spi_util::path::abspath(g_exe);
    std::string testDir = spi_util::path::normpath(spi_util::path::join(exeFullPath.c_str(), "..", "..", "..", 0));

    std::string cppPattern = "*.cpp";
#ifdef _MSC_VER
    cppPattern = spi_util::StringUpper(cppPattern);
#endif

    // define the pattern using the known testDir in case cwd is somewhere else
    const std::vector<std::string>& cppFiles = spi_util::glob::Expand(
        spi_util::path::join(testDir.c_str(), cppPattern.c_str(), 0));

    // we can't really check the list since we add tests all the time
    // what we will check is that this file is in the list
    std::string me = spi_util::path::join(testDir.c_str(), "testGlob.cpp", 0);
    SPI_UTIL_POST_CONDITION(spi_util::Contains(cppFiles, me));
    std::cout << "found " << me << std::endl;

    // testing recursive search algorithm
    std::string depPattern = "**/*.d";
    const std::vector<std::string>& depFiles = spi_util::glob::Expand(
        spi_util::path::join(testDir.c_str(), depPattern.c_str(), 0),
        true);

    std::set<std::string> dfs;
    for (auto fn : depFiles)
    {
        dfs.insert(spi_util::path::basename(fn));
    }

    // we can't really check the all the files - the .d files arise from many
    // different builds - but we will check for some files that should always
    // have been compiled
    SPI_UTIL_POST_CONDITION(dfs.count("testGlob.d") == 1);
    SPI_UTIL_POST_CONDITION(dfs.count("inner.d") == 1);
    SPI_UTIL_POST_CONDITION(dfs.count("test_dll_service_manager.d") == 1);

    for (std::string fn : { "testGlob.d", "inner.d", "test_dll_service_manager.d"})
    {
        SPI_UTIL_POST_CONDITION(dfs.count(fn) == 1);
        std::cout << "found " << fn << std::endl;
    }
}

int main(int argc, char* argv[])
{
    g_exe = argv[0];
    int errors = spi_util::TestRunner(argc, argv,
        "matcher", testMatcher,
        "glob", testGlob,
        (char*)0);
    return errors;
}

