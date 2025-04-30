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
** CSV.cpp
***************************************************************************
** Implements functions for dealing with CSV files.
***************************************************************************
*/

#include "CSV.hpp"

#include "DateUtil.hpp"
#include "RuntimeError.hpp"
#include "StringUtil.hpp"
#include "StreamUtil.hpp"

#include <ctype.h>
#include <string.h>

SPI_UTIL_NAMESPACE

#ifdef _MSC_VER
#define strdup _strdup
#endif

namespace
{
    const char* SkipWhiteSpace(const char* ptr)
    {
        // skips white space in input ptr and returns pointer to first non white-space
        while (isspace(*ptr))
            ++ptr;

        return ptr;
    }

}

namespace csv
{
    std::vector<std::string> ParseLine(const char* line, bool& blankLine)
    {
        blankLine = true;
        SPI_UTIL_PRE_CONDITION(line);

        std::vector<std::string> out;

        char sep = ',';
        const char* work = line;
        while (*work)
        {
            work = SkipWhiteSpace(work);
            if (*work == '"')
            {
                // look for the end of the string
                // allow backslashes to escape out the next character
                // allow "" to indicate a single quote
                bool escaped = false;

                std::string str;
                const char* ptr = work + 1;

                bool doubleQuote;
                do
                {
                    while ((*ptr != '"' || escaped) && *ptr != '\0')
                    {
                        const char& c = *ptr;
                        if (escaped)
                        {
                            if (c != '\\' && c != '"')
                            {
                                str.push_back('\\');
                            }
                            str.push_back(c);
                            escaped = false;
                        }
                        else if (c == '\\')
                        {
                            escaped = true;
                        }
                        else
                        {
                            str.push_back(c);
                        }
                        ++ptr;
                    }

                    // the above loop can end with *ptr == '\0' which is an error
                    // or with *ptr = '"' which is good news
                    if (!*ptr)
                        SPI_UTIL_THROW_RUNTIME_ERROR("Unterminated string " << work);

                    SPI_UTIL_POST_CONDITION(*ptr == '"');
                    doubleQuote = (*(ptr + 1) == '"');

                    if (doubleQuote)
                    {
                        str.push_back('"');
                        ++ptr;
                        ++ptr;
                    }
                } while (doubleQuote);

                work = SkipWhiteSpace(ptr + 1);

                if (*work == '\0' || *work == sep)
                {
                    if (blankLine && !str.empty())
                        blankLine = false;
                    out.push_back(str);
                    if (*work == sep)
                        ++work;
                }
            }
            else if (*work == sep)
            {
                out.push_back(std::string());
                ++work;
            }
            else
            {
                const char* ptr = strchr(work, sep);
                if (!ptr)
                {
                    std::string str = std::string(work);
                    if (blankLine && !str.empty())
                        blankLine = false;
                    out.push_back(str);
                    work += str.length();
                }
                else
                {
                    // more to come
                    std::string str = std::string(work, ptr - work);
                    if (blankLine && !str.empty())
                        blankLine = false;
                    out.push_back(str);
                    work = ptr + 1;
                    if (!*work) // trailing comma
                    {
                        out.push_back(std::string());
                    }
                }
            }
        }
        return out;
    }

    void WriteLine(std::ostream& os, const std::vector<std::string>& row)
    {
        size_t N = row.size();
        if (N > 0)
        {
            WriteItem(os, row[0]);
        }
        for (size_t i = 1; i < N; ++i)
        {
            os << ',';
            WriteItem(os, row[i]);
        }
    }

    void WriteItem(std::ostream& os, const std::string& item)
    {
        // from perusing ParseLine it appears that the best case scenario
        // we just write the item
        // 
        // otherwise we enclose the item in "..."
        // 
        // in the latter case any " in the item needs to be replaced with ""
        // 
        // so what is otherwise?
        // basically a comma in item triggers otherwise

        const char* work = item.c_str();
        const char* p = strchr(work, ',');
        if (!p)
        {
            // no commas in the string
            os << item;
        }
        else
        {
            // commas in the string
            // convert " to "" inside the quotes
            os << '"';
            p = strchr(work, '"');
            while (p)
            {
                os << std::string(work, p - work);
                os << '"' << '"';
                work = p + 1;
                p = strchr(work, '"');
            }
            os << work << '"';
        }
    }

    Data::Data()
        :
        m_rows(),
        m_numRows(),
        m_numColumns()
    {
        verifyAndComplete();
    }

    Data::Data(const std::vector<std::vector<std::string>>& rows)
        :
        m_rows(rows),
        m_numRows(),
        m_numColumns()
    {
        verifyAndComplete();
    }

    void Data::addRow(const std::vector<std::string>& row)
    {
        m_rows.push_back(row);
    
        size_t numColumns = row.size();
        if (numColumns > m_numColumns)
            m_numColumns = numColumns;
        ++m_numRows;
    }

    std::vector<std::string> Data::removeLastRow()
    {
        if (m_numRows == 0)
            SPI_UTIL_THROW_RUNTIME_ERROR("No rows remaining");

        std::vector<std::string> lastRow = m_rows.back();
        m_rows.pop_back();
        --m_numRows;
        return lastRow;
    }

    std::vector<std::string> Data::row(size_t i) const
    {
        if (i >= m_numRows)
            SPI_UTIL_THROW_RUNTIME_ERROR(i << " out of range");
        return m_rows[i];
    }

    std::vector<std::string> Data::column(size_t j) const
    {
        if (j >= m_numColumns)
            SPI_UTIL_THROW_RUNTIME_ERROR(j << " out of range");

        std::vector<std::string> out;
        out.reserve(m_numRows);

        for (size_t i = 0; i < m_numRows; ++i)
        {
            const std::vector<std::string>& row = m_rows[i];
            if (j >= row.size())
                out.push_back(std::string());
            else
                out.push_back(row[j]);
        }

        return out;
    }

    std::string Data::item(size_t i, size_t j) const
    {
        if (i >= m_numRows)
            SPI_UTIL_THROW_RUNTIME_ERROR(i << " out of range");
        if (j >= m_numColumns)
            SPI_UTIL_THROW_RUNTIME_ERROR(j << " out of range");

        const std::vector<std::string>& row = m_rows[i];
        if (j >= row.size())
            return std::string();

        return row[j];
    }

    DataSP Data::Read(const std::string& filename, bool stopOnBlankLine)
    {
        std::ifstream istr(filename.c_str());
        if (!istr)
            SPI_UTIL_THROW_RUNTIME_ERROR("Could not open file " << filename);

        return Read(filename, istr, stopOnBlankLine);
    }

    DataSP Data::Read(const std::string& name, std::istream& istr, bool stopOnBlankLine)
    {
        std::vector<std::string> lines = StreamReadLines(istr);

        if (lines.size() == 0)
            SPI_UTIL_THROW_RUNTIME_ERROR("No contents for '" << name << "'");

        DataSP out(new Data());

        size_t NL = lines.size();

        for (size_t i = 0; i < NL; ++i)
        {
            bool blankLine;
            std::vector<std::string> row = ParseLine(lines[i].c_str(), blankLine);

            if (stopOnBlankLine && blankLine)
                break;

            if (row.size() > 0)
                out->addRow(row);
        }

        return out;
    }

    void csv::Data::Write(std::ostream& ostr) const
    {
        size_t N = m_rows.size();

        for (size_t i = 0; i < N; ++i)
        {
            spi_util::csv::WriteLine(ostr, m_rows[i]);
            ostr << '\n';
        }
        ostr.flush();
    }

    void Data::verifyAndComplete()
    {
        m_numRows = m_rows.size();
        m_numColumns = 0;
        for (size_t i = 0; i < m_numRows; ++i)
        {
            size_t numColumns = m_rows[i].size();
            if (numColumns > m_numColumns)
                m_numColumns = numColumns;
        }
    }

} // end of namespace csv

SPI_UTIL_END_NAMESPACE

std::ostream& operator<<(std::ostream& ostr, const spi_util::csv::DataConstSP& data)
{
    SPI_UTIL_PRE_CONDITION(data);
    data->Write(ostr);
    return ostr;
}
