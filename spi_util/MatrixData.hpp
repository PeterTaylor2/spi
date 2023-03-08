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
** MatrixData.hpp
***************************************************************************
** MatrixData template classes - for regular 2D arrays.
***************************************************************************
*/

#ifndef SPI_UTIL_MATRIX_DATA_HPP
#define SPI_UTIL_MATRIX_DATA_HPP

#include "Namespace.hpp"
#include "RuntimeError.hpp"

#include <vector>
#include <spi_boost/shared_array.hpp>

SPI_UTIL_NAMESPACE

template <typename T>
class MatrixData
{
public:
    // The Row class enables us to use [][] notation for both
    // reading from and writing to the MatrixData data
    //
    // We can only construct instances of Row class from with
    // MatrixData since it has a private constructor, but MatrixData
    // is declared as a friend
    class Row
    {
    public:
        friend class MatrixData;
        T operator[](size_t i) const
        {
            if (i >= m_nbCols)
                SPI_UTIL_THROW_RUNTIME_ERROR("Out of bounds access (" <<
                    i << ") to matrix row of size " << m_nbCols);
            return m_row[i];
        }
        T& operator[](size_t i)
        {
            if (i >= m_nbCols)
                SPI_UTIL_THROW_RUNTIME_ERROR("Out of bounds access (" <<
                    i << ") to matrix row of size " << m_nbCols);
            return m_row[i];
        }
    private:
        size_t m_nbCols;
        T* m_row;
        Row(size_t nbCols, T* row)
            :
            m_nbCols(nbCols),
            m_row(row)
        {
            SPI_UTIL_PRE_CONDITION(nbCols > 0);
            SPI_UTIL_PRE_CONDITION(row != NULL);
        }
    };

    // constructor of an empty zero size matrix
    MatrixData()
        :
        m_nbRows(0),
        m_nbCols(0),
        m_data()
    {}

    // constructor of an empty (potentially) non-zero size matrix
    // subsequently you need to fill it element-by-element
    // values within the matrix initially correspond to T()
    MatrixData(size_t nbRows, size_t nbCols)
        :
        m_nbRows(nbRows),
        m_nbCols(nbCols),
        m_data()
    {
        if (nbRows == 0 || nbCols == 0)
        {
            m_nbRows = 0;
            m_nbCols = 0;
        }
        else
        {
            m_data.reset(new T[nbRows*nbCols]());
        }
    }

    // constructor of a (potentially) non-zero size matrix using a 1D-vector
    // to initialise the data
    //
    // the 1D-vector is row-by-row - thus 4 rows, 3 columns the first
    // 3-elements are row zero, the next 3-elements are row one etc.
    MatrixData(size_t nbRows, size_t nbCols,
        const std::vector<T>& data)
        :
        m_nbRows(nbRows),
        m_nbCols(nbCols),
        m_data()
    {
        SPI_UTIL_PRE_CONDITION(data.size() == nbRows*nbCols);

        if (nbRows == 0 || nbCols == 0)
        {
            m_nbRows = 0;
            m_nbCols = 0;
        }
        else
        {
            m_data.reset(new T[nbRows*nbCols]());
            size_t N = data.size();
            for (size_t i = 0; i < N; ++i)
                m_data[i] = data[i];
        }
    }

    // constructor of a (potentially) non-zero size matrix using a 1D-array 
    // to initialise the data that we trust to be of size (nbRows*nbCols)
    //
    // the 1D-array is row-by-row - thus 4 rows, 3 columns the first
    // 3-elements are row zero, the next 3-elements are row one etc.
    //
    // set transpose=true to reverse the order of the 1D-array
    MatrixData(size_t nbRows, size_t nbCols, const T* data, bool transpose=false)
        :
        m_nbRows(nbRows),
        m_nbCols(nbCols),
        m_data()
    {
        if (nbRows == 0 || nbCols == 0)
        {
            m_nbRows = 0;
            m_nbCols = 0;
        }
        else
        {
            SPI_UTIL_PRE_CONDITION(data);
            m_data.reset(new T[nbRows * nbCols]());
            size_t N = nbRows * nbCols;
            if (transpose)
            {
                for (size_t i = 0; i < nbRows; ++i)
                {
                    for (size_t j = 0; j < nbCols; ++j)
                    {
                        size_t rcPos = i * nbCols + j;
                        size_t crPos = j * nbRows + i;
                        m_data[rcPos] = data[crPos];
                    }
                }
            }
            else
            {
                for (size_t i = 0; i < N; ++i)
                    m_data[i] = data[i];
            }
        }
    }

    // copies matrix data to pre-allocated 1D-array which we trust to be of size (nbRows*nbCols)
    //
    // the 1D-array is row-by-row - thus 4 rows, 3 columns the first
    // 3-elements are row zero, the next 3-elements are row one etc.
    //
    // set transpose=true to reverse the order of the 1D-array
    void copy_data(T* data, bool transpose=false) const
    {
        size_t size = m_nbRows * m_nbCols;
        if (size > 0)
        {
            SPI_UTIL_PRE_CONDITION(data);
            if (transpose)
            {
                for (size_t i = 0; i < m_nbRows; ++i)
                {
                    for (size_t j = 0; j < m_nbCols; ++j)
                    {
                        size_t rcPos = i * m_nbCols + j;
                        size_t crPos = j * m_nbRows + i;
                        data[crPos] = m_data[rcPos];
                    }
                }
            }
            else
            {
                std::copy(&m_data[0], &m_data[0] + size, data);
            }

        }
    }

    // resizes the matrix
    //
    // this must either match the total size of the original matrix
    // or set the size for a matrix that was initially empty
    void Resize(size_t nbRows, size_t nbCols)
    {
        if (m_nbRows == 0 || m_nbCols == 0)
        {
            if (nbRows == 0 || nbCols == 0)
            {
                m_nbRows = 0;
                m_nbCols = 0;
            }
            else
            {
                m_nbRows = nbRows;
                m_nbCols = nbCols;
                m_data.reset(new T[nbRows*nbCols]());
            }
        }
        else
        {
            SPI_UTIL_PRE_CONDITION(nbRows*nbCols == m_nbCols*m_nbRows);
            m_nbRows = nbRows;
            m_nbCols = nbCols;
        }
    }

    // const Row operator[] allows for read-only access
    const Row operator[](size_t i) const
    {
        if (i >= m_nbRows)
            SPI_UTIL_THROW_RUNTIME_ERROR("Out of bounds access (" <<
                i << ") to matrix with " << m_nbRows << " rows");

        return Row(m_nbCols, &m_data[i*m_nbCols]);
    }

    // non-const Row operator[] allows for write access
    Row operator[](size_t i)
    {
        if (i >= m_nbRows)
            SPI_UTIL_THROW_RUNTIME_ERROR("Out of bounds access (" <<
                i << ") to matrix with " << m_nbRows << " rows");

        return Row(m_nbCols, &m_data[i*m_nbCols]);
    }

    // number of rows of the matrix
    size_t Rows() const
    {
        return m_nbRows;
    }

    // number of columns of the matrix
    size_t Cols() const
    {
        return m_nbCols;
    }

    // copy of the internal data of the matrix as a 1D-array vector
    //
    // the 1D-vector is row-by-row - thus 4 rows, 3 columns the first
    // 3-elements are row zero, the next 3-elements are row one etc.
    std::vector<T> Data() const
    {
        size_t size = m_nbRows * m_nbCols;
        if (size == 0)
            return std::vector<T>();
        const T* start = &m_data[0];
        return std::vector<T>(start, start+size);
    }

    // copy of the internal data of the matrix as a 1D-array
    //
    // the data is row-by-row - thus 4 rows, 3 columns the first
    // 3-elements are row zero, the next 3-elements are row one etc.
    const T* DataPointer() const
    {
        size_t size = m_nbRows * m_nbCols;
        if (size == 0)
            SPI_UTIL_THROW_RUNTIME_ERROR("No data for empty matrix");
        return &m_data[0];
    }

    // copy of the internal data of the matrix as a 1D-array
    //
    // the data is row-by-row - thus 4 rows, 3 columns the first
    // 3-elements are row zero, the next 3-elements are row one etc.
    T* DataPointer()
    {
        size_t size = m_nbRows * m_nbCols;
        if (size == 0)
            SPI_UTIL_THROW_RUNTIME_ERROR("No data for empty matrix");
        return &m_data[0];
    }

    // whether the matrix is defined
    // you can construct a Matrix using the no parameter constructor - that
    // is our definition of an undefined matrix
    operator bool() const
    {
        if (m_nbRows == 0 || m_nbCols == 0)
            return false;

        return true;
    }

    T& at(size_t r, size_t c)
    {
        if (r >= m_nbRows || c >= m_nbCols)
            SPI_UTIL_THROW_RUNTIME_ERROR("Out of bounds access (" << r << ", " << c
                << ") to matrix with size [" << m_nbRows << ", " << m_nbCols << "]");

        return m_data[r*m_nbCols + c];
    }

    const T& at(size_t r, size_t c) const
    {
        if (r >= m_nbRows || c >= m_nbCols)
            SPI_UTIL_THROW_RUNTIME_ERROR("Out of bounds access (" << r << ", " << c
                << ") to matrix with size [" << m_nbRows << ", " << m_nbCols << "]");

        return m_data[r*m_nbCols + c];
    }

protected:
    size_t m_nbRows;
    size_t m_nbCols;

    // the benefit of shared_array<T> over vector<T>
    // is that we don't need the resize overhead and
    // for the Row<T> class we can safely use pointer
    // arithmetic (counter-example when using vector
    // is when T=bool)
    //
    // possible scoped_array is even better!
    spi_boost::shared_array<T> m_data;
};

SPI_UTIL_END_NAMESPACE

#endif
