/*
    Prints the compiler version number.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <iostream>

int main(int argc, char* argv[])
{
#if defined(_MSC_VER)
    std::cout << "_MSC_VER=" << _MSC_VER << std::endl;
#endif

#if defined(__clang__)
    std::cout << "CLANG_VERSION=" << __clang_major__ << "." << __clang_minor__
              << "." << __clang_patchlevel__ << std::endl;
    std::cout << "clang_version_string=" << __clang_version__ << std::endl;
#if defined(__apple_build_version__)
    std::cout << "APPLE_BUILD_VERSION=" << __apple_build_version__ << std::endl;
#endif
#elif defined(__GNUC__)
    std::cout << "GNUC_VERSION=" << __GNUC__ << "." << __GNUC_MINOR__ << "."
              << __GNUC_PATCHLEVEL__ << std::endl;
#endif

    std::cout << "__cplusplus=" << __cplusplus << std::endl;

    return 0;
}

