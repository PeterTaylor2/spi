#include <spi_util/FileUtil.hpp>
#include <spi_util/RuntimeError.hpp>

#include <stdio.h>
#include <iostream>

#include <spi_util/TestRunner.hpp>
#include <spi_util/LRUCache.hpp>

static char* g_exe = NULL;

static void testLRUCache(void)
{
    spi_util::LRUCache<int, std::string> cache(5);

    cache.insert(1, "one");
    cache.insert(2, "two");
    cache.insert(3, "three");
    cache.insert(4, "four");
    cache.insert(5, "five");
    cache.insert(6, "six");
    cache.insert(7, "seven");

    std::vector<std::pair<int, std::string>> all = cache.all();
    size_t size = all.size();
    SPI_UTIL_POST_CONDITION(size == 5);
    for (size_t i = 0; i < size; ++i)
        std::cout << i << ":" << all[i].first << ":" << all[i].second << std::endl; 

    std::string value;
    SPI_UTIL_POST_CONDITION(cache.find(4, value));
    SPI_UTIL_POST_CONDITION(value == "four");
    SPI_UTIL_POST_CONDITION(!cache.find(1, value));

    cache.insert(1, "one");

    all = cache.all();
    size = all.size();
    SPI_UTIL_POST_CONDITION(size == 5);
    for (size_t i = 0; i < size; ++i)
        std::cout << i << ":" << all[i].first << ":" << all[i].second << std::endl; 
}

int main(int argc, char* argv[])
{
    g_exe = argv[0];
    int errors = spi_util::TestRunner(argc, argv,
                                      "lruCache", testLRUCache,
                                      (char*)0);
    return errors;
}

