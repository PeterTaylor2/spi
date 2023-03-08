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
#ifndef CONSTANT_HPP
#define CONSTANT_HPP

/*
***************************************************************************
** constant.hpp
***************************************************************************
** Defines the Constant class and sub-classes.
***************************************************************************
*/

#include <vector>

#include <spi/Date.hpp>
#include <spi/DateTime.hpp>
#include <spi/Service.hpp>
#include <spi/spdoc_namespace.hpp>
#include <spi/spdoc_publicType.hpp>

SPI_DECLARE_RC_CLASS(Constant);
SPI_DECLARE_RC_CLASS(IntConstant);
SPI_DECLARE_RC_CLASS(DateConstant);
SPI_DECLARE_RC_CLASS(DateTimeConstant);
SPI_DECLARE_RC_CLASS(DoubleConstant);
SPI_DECLARE_RC_CLASS(CharConstant);
SPI_DECLARE_RC_CLASS(StringConstant);
SPI_DECLARE_RC_CLASS(BoolConstant);
SPI_DECLARE_RC_CLASS(UndefinedConstant);

SPDOC_BEGIN_NAMESPACE
SPI_DECLARE_OBJECT_CLASS(Constant);
SPDOC_END_NAMESPACE

/**
 * Interface class defining a constant scalar value.
 */
class Constant : public spi::RefCounter
{
public:
    virtual std::string toCode(spdoc::PublicType publicType=spdoc::PublicType(spdoc::PublicType::UNINITIALIZED_VALUE)) const = 0;
    virtual const char* typeName() const = 0;

    virtual int getInt() const;
    virtual spi::Date getDate() const;
    virtual spi::DateTime getDateTime() const;
    virtual double getDouble() const;
    virtual char getChar() const;
    virtual std::string getString() const;
    virtual bool getBool() const;
    virtual bool isUndefined() const;
    virtual spdoc::ConstantConstSP getDoc() const = 0;

    static BoolConstantConstSP TRUE;
    static BoolConstantConstSP FALSE;
    static UndefinedConstantConstSP UNDEFINED;

protected:
    Constant();
};

/**
 * Integer constant defined in the configuration file.
 */
class IntConstant : public Constant
{
public:
    static IntConstantConstSP Make(int value);

    std::string toCode(spdoc::PublicType publicType) const;
    const char* typeName() const;
    int getInt() const;
    spdoc::ConstantConstSP getDoc() const;

protected:
    IntConstant(int value);

private:
    int m_value;
};

/**
 * Date constant defined in the configuration file.
 */
class DateConstant : public Constant
{
public:
    static DateConstantConstSP Make(spi::Date value);

    std::string toCode(spdoc::PublicType publicType) const;
    const char* typeName() const;
    spi::Date getDate() const;
    spdoc::ConstantConstSP getDoc() const;

protected:
    DateConstant(spi::Date value);

private:
    spi::Date m_value;
};

/**
 * Date constant defined in the configuration file.
 */
class DateTimeConstant : public Constant
{
public:
    static DateTimeConstantConstSP Make(spi::DateTime value);

    std::string toCode(spdoc::PublicType publicType) const;
    const char* typeName() const;
    spi::DateTime getDateTime() const;
    spdoc::ConstantConstSP getDoc() const;

protected:
    DateTimeConstant(spi::DateTime value);

private:
    spi::DateTime m_value;
};


/**
 * Double constant defined in the configuration file.
 */
class DoubleConstant : public Constant
{
public:
    static DoubleConstantConstSP Make(double value);

    std::string toCode(spdoc::PublicType publicType) const;
    const char* typeName() const;
    double getDouble() const;
    spdoc::ConstantConstSP getDoc() const;

protected:
    DoubleConstant(double value);

private:
    double m_value;
};

/**
 * Char constant defined in the configuration file.
 */
class CharConstant : public Constant
{
public:
    static CharConstantConstSP Make(char value);

    std::string toCode(spdoc::PublicType publicType) const;
    const char* typeName() const;
    char getChar() const;
    spdoc::ConstantConstSP getDoc() const;

protected:
    CharConstant(char value);

private:
    char m_value;
};

/**
 * String constant defined in the configuration file
 */
class StringConstant : public Constant
{
public:
    static StringConstantConstSP Make(const std::string& value=std::string());

    std::string toCode(spdoc::PublicType publicType) const;
    const char* typeName() const;
    std::string getString() const;
    spdoc::ConstantConstSP getDoc() const;

protected:
    StringConstant(const std::string& value);

private:
    std::string m_value;
};

/**
 * Bool constant defined in the configuration file
 */
class BoolConstant : public Constant
{
public:
    static BoolConstantConstSP Make(bool value);

    std::string toCode(spdoc::PublicType publicType) const;
    const char* typeName() const;
    bool getBool() const;

    BoolConstant(bool value);
    spdoc::ConstantConstSP getDoc() const;

private:
    bool m_value;
};

/**
 * Undefined constant - this will give a sensible value for everything
 * other than char and cannot be converted toCode.
 */
class UndefinedConstant : public Constant
{
public:
    static UndefinedConstantConstSP Make();
    static UndefinedConstantConstSP UNDEFINED;

    std::string toCode(spdoc::PublicType publicType) const;
    const char* typeName() const;

    int getInt() const;
    spi::Date getDate() const;
    spi::DateTime getDateTime() const;
    double getDouble() const;
    std::string getString() const;
    bool getBool() const;
    bool isUndefined() const;
    spdoc::ConstantConstSP getDoc() const;

    UndefinedConstant();

};

#endif
