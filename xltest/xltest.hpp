#ifndef SPI_XLTEST_HPP
#define SPI_XLTEST_HPP

#include <spi/Value.hpp>
#include "xlcall.h"

SPI_BEGIN_NAMESPACE

void RegisterXLL(const std::string& name);
void UnregisterXLL(const std::string& name);
Value CallXLL(const std::string& funcName, int count, ...);

SPI_END_NAMESPACE

#if SPI_XL_VERSION == 12
extern "C"
{
int PASCAL MdCallBack12(int, int, LPXLOPER12[], LPXLOPER12);
}
#endif

#endif

