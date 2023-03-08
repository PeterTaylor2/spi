#include <spi_util/FileUtil.hpp>
#include <spi_util/RuntimeError.hpp>

#include <stdio.h>
#include <iostream>

#include <spi_util/TestRunner.hpp>

static char* g_exe = NULL;

static void testWalkerFromRoot(const char* root)
{

    spi_util::DirectoryWalker walker(root);

    for (spi_util::Directory* d = walker.Next();
         d != NULL;
         d = walker.Next())
    {
        std::cout << "DN:\t" << d->dn << std::endl;
        std::cout << "FNS:";
        int count = 0;
        for (std::set<std::string>::const_iterator iter = d->fns.begin();
             iter != d->fns.end(); ++iter)
        {
            ++count;
            std::cout << "\t" << *iter << std::endl;
        }
        if (count == 0)
            std::cout << std::endl;
        count = 0;
        std::cout << "DNS:";
        for (std::set<std::string>::const_iterator iter = d->dns.begin();
             iter != d->dns.end(); ++iter)
        {
            ++count;
            std::cout << "\t" << *iter << std::endl;
        }
        if (count == 0)
            std::cout << std::endl;
    }
}

static void testWalker()
{
    testWalkerFromRoot(".");
    testWalkerFromRoot("../test");
}

int main(int argc, char* argv[])
{
    g_exe = argv[0];
    int errors = spi_util::TestRunner(argc, argv,
                                      "walker", testWalker,
                                      (char*)0);
    return errors;
}

