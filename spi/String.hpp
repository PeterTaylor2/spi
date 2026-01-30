/*

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
** String.hpp
***************************************************************************
*/

#ifndef SPI_STRING_HPP
#define SPI_STRING_HPP

#include "DeclSpec.h"
#include "RefCounter.hpp"

SPI_BEGIN_NAMESPACE

SPI_DECLARE_RC_CLASS(String);

/// <summary>
/// String is simply a reference counted wrapper of std::string.
/// </summary>
class SPI_IMPORT String : public RefCounter
{
public:
    String(const std::string& str);

    const std::string& str() const;

private:
    std::string m_str;

    String(const String&) = delete;
    String& operator=(const String&) = delete;
};

SPI_END_NAMESPACE

#endif

