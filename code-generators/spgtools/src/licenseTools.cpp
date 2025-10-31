/*

    Sartorial Programming Interface (SPI) code generators
    Copyright (C) 2012-2023 Sartorial Programming Ltd.

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

#include "licenseTools.hpp"
#include "generatedOutput.hpp"
#include <spi/RuntimeError.hpp>

#include <sstream>
#include <iostream>
#include <fstream>

namespace
{

const char* copyright()
{
    return "Copyright (C) 2012-2023 Sartorial Programming Ltd.";
}

}

void printBanner(const std::string& exe, bool showLicense)
{
    std::cout << exe << " " << copyright() << std::endl;
    if (showLicense)
    {
        std::cout << "\n";
        printLicense();
    }
}

void printLicense()
{
    std::cout << "This program comes with ABSOLUTELY NO WARRANTY; for details see the GNU General Public License.\n\n"
        << "This program is free software: you can redistribute it and/or modify it under the terms of the\n"
        << "GNU General Public License as published by the Free Software Foundation, either version 3 of the\n"
        << "License, or (at your option) any later version.\n\n";
}

std::string readLicenseFile(const std::string& fn)
{
    std::ifstream fp(fn.c_str());
    if (!fp)
        SPI_THROW_RUNTIME_ERROR("Cannot open license file: " << fn);
    std::stringstream buf;
    buf << fp.rdbuf();

    return buf.str();
}

void writeLicense(GeneratedOutput& ostr, const std::string& license)
{
    if (!license.empty())
        ostr << license << "\n";
}
