/*

    Sartorial Programming Interface (SPI) runtime libraries
    Copyright (C) 2012-2023 Sartorial Programming Ltd.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA

*/
/*
***************************************************************************
** CSV.hpp
***************************************************************************
** Defines functions for dealing with CSV files.
***************************************************************************
*/

#ifndef SPI_UTIL_CSV_HPP
#define SPI_UTIL_CSV_HPP

#include "DeclSpec.h"
#include "Namespace.hpp"
#include "RefCounter.hpp"

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>

SPI_UTIL_NAMESPACE

namespace csv
{
    std::vector<std::string> ParseLine(const char* line);

    SPI_UTIL_DECLARE_RC_CLASS(Data);

    class SPI_UTIL_IMPORT Data : public RefCounter
    {
    public:
        Data();
        Data(const std::vector<std::vector<std::string>>& rows);

        void addRow(const std::vector<std::string>& row);
        // idea is that we read Data, then discover the size, and then transform by removing the last row
        // one at a time - the idea is to keep the total memory footprint as small as possible
        std::vector<std::string> removeLastRow();

        size_t numRows() const { return m_numRows; }
        size_t numColumns() const { return m_numColumns; }
        std::vector<std::string> row(size_t i) const;
        std::vector<std::string> column(size_t j) const;
        std::string item(size_t i, size_t j) const;

        const std::vector<std::vector<std::string>>& rows() const { return m_rows; }

        static DataSP Read(const std::string& filename);
        static DataSP Read(const std::string& name, std::istream& istr);

    private:
        std::vector<std::vector<std::string>> m_rows;
        size_t m_numRows;
        size_t m_numColumns;

        void verifyAndComplete();
    };

} // end of namespace csv

SPI_UTIL_END_NAMESPACE

#endif


