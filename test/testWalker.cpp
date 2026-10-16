#include <spi_util/FileUtil.hpp>
#include <spi_util/RuntimeError.hpp>
#include <spi_util/StringUtil.hpp>

#include <stdio.h>
#include <iostream>

#include <spi_util/TestRunner.hpp>

static char* g_exe = NULL;

static void testWalkerFromRoot(const char* root, bool depthFirst)
{
    std::set<std::string> excludeDirNames;
    spi_util::DirectoryWalker walker(root, excludeDirNames, depthFirst);

    std::string dirname = spi_util::path::dirname(root);
    size_t skip = dirname.length();
    if (skip > 0)
        ++skip;

    std::cout << "root: " << root << "\n"
	      << "depthFirst: " << (depthFirst ? "true" : "false") << "\n\n";

    std::cout << "DirectoryName                            NumFiles NumDirs\n"
              << "======================================== ======== ========\n";
    /*            123456789 123456789 123456789 123456789  12345678 12345678 */

    for (spi_util::Directory* d = walker.Next();
        d != NULL;
        d = walker.Next())
    {
        std::string dn = d->dn.substr(skip);
        std::cout << spi_util::StringFormat("%-40s %8d %8d\n", dn.c_str(), d->fns.size(), d->dns.size());
    }
}

static void testWalker()
{
    std::string exeFullPath = spi_util::path::abspath(g_exe);
    std::string testDir = spi_util::path::normpath(spi_util::path::join(exeFullPath.c_str(), "..", "..", "..", 0));

    std::cout << std::endl;
    testWalkerFromRoot(testDir.c_str(), false);
    std::cout << std::endl;
    testWalkerFromRoot(testDir.c_str(), true);
    std::cout << std::endl;
}

int main(int argc, char* argv[])
{
    g_exe = argv[0];
    int errors = spi_util::TestRunner(argc, argv,
                                      "walker", testWalker,
                                      (char*)0);
    return errors;
}

