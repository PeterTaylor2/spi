/*
 * This tests the speed of various file reading methods.
 * We read a binary file to confirm that std::string can be used to return binary files.
 */

#include <spi_util/FileUtil.hpp>
#include <spi_util/RuntimeError.hpp>

#include <string.h>
#include <stdio.h>
#include <iostream>

#include <spi_util/TestRunner.hpp>

static char* g_exe = NULL;

static char g_fn[] = "../doc/spi-user-guide.pdf";

static void testReadFile(void)
{
    // call this first to get the directory into the operating system cache
    std::vector<std::string> strs;
    strs.push_back( spi_util::FileReadContents(g_fn, spi_util::FILE_READ_METHOD_C));;
    strs.push_back( spi_util::FileReadContents(g_fn, spi_util::FILE_READ_METHOD_CPP));;
    strs.push_back( spi_util::FileReadContents(g_fn, spi_util::FILE_READ_METHOD_ITERATOR));;
    strs.push_back( spi_util::FileReadContents(g_fn, spi_util::FILE_READ_METHOD_RDBUF));;

    // test that we can read a binary file - hence strlen should be different from length and size
    size_t N = strs.size();
    for (size_t i = 0; i < N; ++i)
    {
        std::cout << i+1 << ": size = " << strs[i].size()
                  << " length = " << strs[i].length() 
                  << " strlen = " << strlen(strs[i].c_str())
                  << std::endl;
    }

}

static void testReadFileDefault(void)
{
    spi_util::FileReadContents(g_fn, spi_util::FILE_READ_METHOD_DEFAULT);
}

static void testReadFileC(void)
{
    spi_util::FileReadContents(g_fn, spi_util::FILE_READ_METHOD_C);
}

static void testReadFileCPP(void)
{
    spi_util::FileReadContents(g_fn, spi_util::FILE_READ_METHOD_CPP);
}

static void testReadFileIterator(void)
{
    spi_util::FileReadContents(g_fn, spi_util::FILE_READ_METHOD_ITERATOR);
}

static void testReadFileRdbuf(void)
{
    spi_util::FileReadContents(g_fn, spi_util::FILE_READ_METHOD_RDBUF);
}

int main(int argc, char* argv[])
{
    g_exe = argv[0];
    int errors = spi_util::TestRunner(argc, argv,
                                      "readFile", testReadFile,
                                      "readFileIterator", testReadFileIterator,
                                      "readFileRdbuf", testReadFileRdbuf,
                                      "readFileC", testReadFileC,
                                      "readFileCPP", testReadFileCPP,
                                      "readFileDefault", testReadFileDefault,
                                      (char*)0);
    return errors;
}

